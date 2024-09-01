/*
 * bsnes-jg - Super Nintendo emulator
 *
 * Copyright (C) 2004-2020 byuu
 * Copyright (C) 2020-2022 Rupert Carmichael
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, specifically version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <cstring>

#include "audio.hpp"
#include "cartridge.hpp"
#include "cheat.hpp"
#include "controller.hpp"
#include "coprocessor/armdsp.hpp"
#include "coprocessor/cx4.hpp"
#include "coprocessor/dip.hpp"
#include "coprocessor/dsp1.hpp"
#include "coprocessor/dsp2.hpp"
#include "coprocessor/dsp4.hpp"
#include "coprocessor/epsonrtc.hpp"
#include "coprocessor/event.hpp"
#include "coprocessor/hitachidsp.hpp"
#include "coprocessor/icd.hpp"
#include "coprocessor/mcc.hpp"
#include "coprocessor/msu1.hpp"
#include "coprocessor/necdsp.hpp"
#include "coprocessor/obc1.hpp"
#include "coprocessor/sa1.hpp"
#include "coprocessor/sdd1.hpp"
#include "coprocessor/sharprtc.hpp"
#include "coprocessor/spc7110.hpp"
#include "coprocessor/st0010.hpp"
#include "coprocessor/superfx.hpp"
#include "bsmemory.hpp"
#include "cpu.hpp"
#include "dsp.hpp"
#include "emulator.hpp"
#include "expansion/expansion.hpp"
#include "random.hpp"
#include "settings.hpp"
#include "sufamiturbo.hpp"
#include "smp.hpp"

namespace SuperFamicom {

System system;
Scheduler scheduler;

serializer System::serialize(bool synchronize) {
  //deterministic serialization (synchronize=false) is only possible with select libco methods
  if(!co_serializable()) synchronize = true;

  if(!information.serializeSize[synchronize]) return {};  //should never occur
  if(synchronize) runToSave();

  unsigned signature = 0x31545342;
  unsigned serializeSize = information.serializeSize[synchronize];
  char version[16] = {};
  char description[512] = {};
  bool placeholder = false;
  std::memcpy(&version, (const char*)SerializerVersion.c_str(), SerializerVersion.size());

  serializer s(serializeSize);
  s.integer(signature);
  s.integer(serializeSize);
  s.array(version);
  s.array(description);
  s.boolean(synchronize);
  s.boolean(placeholder);
  serializeAll(s, synchronize);
  return s;
}

bool System::unserialize(serializer& s) {
  unsigned signature = 0;
  unsigned serializeSize = 0;
  char version[16] = {};
  char description[512] = {};
  bool synchronize = false;
  bool placeholder = false;

  s.integer(signature);
  s.integer(serializeSize);
  s.array(version);
  s.array(description);
  s.boolean(synchronize);
  s.boolean(placeholder);

  if((signature != 0x31545342)
      || (serializeSize != information.serializeSize[synchronize])
      || (std::string{version} != SerializerVersion))
    return false;

  if(synchronize) power(/* reset = */ false);
  serializeAll(s, synchronize);
  return true;
}

//internal

void System::serializeAll(serializer& s, bool synchronize) {
  random.serialize(s);
  cartridge.serialize(s);
  cpu.serialize(s);
  smp.serialize(s);
  ppu.serialize(s);
  dsp.serialize(s);

  if(cartridge.has.ICD) icd.serialize(s);
  if(cartridge.has.MCC) mcc.serialize(s);
  if(cartridge.has.DIP) dip.serialize(s);
  if(cartridge.has.Event) event.serialize(s);
  if(cartridge.has.SA1) sa1.serialize(s);
  if(cartridge.has.SuperFX) superfx.serialize(s);
  if(cartridge.has.ARMDSP) armdsp.serialize(s);
  if(cartridge.has.HitachiDSP) hitachidsp.serialize(s);
  if(cartridge.has.NECDSP) necdsp.serialize(s);
  if(cartridge.has.EpsonRTC) epsonrtc.serialize(s);
  if(cartridge.has.SharpRTC) sharprtc.serialize(s);
  if(cartridge.has.SPC7110) spc7110.serialize(s);
  if(cartridge.has.SDD1) sdd1.serialize(s);
  if(cartridge.has.OBC1) obc1.serialize(s);
  if(cartridge.has.MSU1) msu1.serialize(s);

  if(cartridge.has.Cx4) cx4.serialize(s);
  if(cartridge.has.DSP1) dsp1.serialize(s);
  if(cartridge.has.DSP2) dsp2.serialize(s);
  if(cartridge.has.DSP4) dsp4.serialize(s);
  if(cartridge.has.ST0010) st0010.serialize(s);

  if(cartridge.has.BSMemorySlot) bsmemory.serialize(s);
  if(cartridge.has.SufamiTurboSlotA) sufamiturboA.serialize(s);
  if(cartridge.has.SufamiTurboSlotB) sufamiturboB.serialize(s);

  controllerPort1.serialize(s);
  controllerPort2.serialize(s);
  expansionPort.serialize(s);

  if(!synchronize) {
    cpu.serializeStack(s);
    smp.serializeStack(s);
    ppu.serializeStack(s);
    for(Thread* coprocessor : cpu.coprocessors) {
      coprocessor->serializeStack(s);
    }
  }
}

//perform dry-run state save:
//determines exactly how many bytes are needed to save state for this cartridge,
//as amount varies per game (eg different RAM sizes, special chips, etc.)
unsigned System::serializeInit(bool synchronize) {
  serializer s;

  unsigned signature = 0;
  unsigned serializeSize = 0;
  char version[16] = {};
  char description[512] = {};

  s.integer(signature);
  s.integer(serializeSize);
  s.array(version);
  s.array(description);
  s.boolean(synchronize);
  s.boolean(hacks.fastPPU);
  serializeAll(s, synchronize);
  return s.size();
}

void System::run() {
  scheduler.mode = Scheduler::Mode::Run;
  scheduler.enter();
  if(scheduler.event == Scheduler::Event::Frame) frameEvent();
}

void System::runToSave() {
  scheduler.mode = Scheduler::Mode::Synchronize;

  //run every thread until it cleanly hits a synchronization point
  //if it fails, start resynchronizing every thread again
  auto synchronize = [&](cothread_t thread) -> bool {
    scheduler.active = thread;
    while(true) {
      scheduler.enter();
      if(scheduler.event == Scheduler::Event::Frame) frameEvent();
      if(scheduler.event == Scheduler::Event::Synchronized) break;
      if(scheduler.event == Scheduler::Event::Desynchronized) return false;
    }
    return true;
  };

  while(true) {
    //SMP thread is synchronized twice to ensure the CPU and SMP are closely aligned:
    //this is extremely critical for Tales of Phantasia and Star Ocean.
    if(!synchronize(smp.thread)) continue;
    if(!synchronize(cpu.thread)) continue;
    if(!synchronize(smp.thread)) continue;
    if(!synchronize(ppu.thread)) continue;

    bool synchronized = true;
    for(Thread* coprocessor : cpu.coprocessors) {
      if(!synchronize(coprocessor->thread)) {
        synchronized = false; break;
      }
    }
    if(!synchronized) continue;

    break;
  }

  scheduler.mode = Scheduler::Mode::Run;
  scheduler.active = cpu.thread;
}

void System::frameEvent() {
  ppu.refresh();

  //refresh all cheat codes once per frame
  Memory::GlobalWriteEnable = true;
  for(Cheat::Code& code : cheats.codes) {
    bus.write(code.address, code.data);
  }
  Memory::GlobalWriteEnable = false;
}

bool System::load() {
  information = {};
  bus.reset();

  if(!cpu.load()
      || !smp.load()
      || !ppu.load()
      || !dsp.load()
      || !cartridge.load()
      || (cartridge.has.ICD && !icd.load()))
    return false;

  const std::string region = cartridge.region();

  if(region == "NTSC") {
    information.region = Region::NTSC;
    information.cpuFrequency = FREQ_NTSC * 6.0;
  }
  else if(region == "PAL") {
    information.region = Region::PAL;
    information.cpuFrequency = FREQ_PAL * 4.8;
  }

  if(configuration.hotfixes) {
    const std::string headerTitle = cartridge.headerTitle();

   /* Dirt Racer (Europe) relies on uninitialized memory containing certain
      values to boot without freezing. The game itself is broken and will fail
      to run sometimes on real hardware, but for the sake of expedience WRAM
      is initialized to a constant value that will allow this game to always
      boot successfully.
    */
    if(headerTitle == "DIRT RACER")
      cpu.quirk();

   /* Magical Drop (Japan) does not initialize the DSP registers at startup,
      tokoton mode will hang forever in some instances even on real hardware.
    */
    else if(headerTitle == "MAGICAL DROP")
      dsp.quirk();

   /* Due to poor programming Rendering Ranger R2 will rarely lock up at
      32040 * 768hz.
    */
    else if(headerTitle == "RENDERING RANGER R2")
      information.apuFrequency = 32000.0 * 768.0;
  }

  if(cartridge.has.BSMemorySlot) bsmemory.load();

  return information.loaded = true;
}

void System::save() {
  if(loaded())
    cartridge.save();
}

void System::unload() {
  if(loaded()) {
    controllerPort1.unload();
    controllerPort2.unload();
    expansionPort.unload();

    if(cartridge.has.ICD) icd.unload();
    if(cartridge.has.MCC) mcc.unload();
    if(cartridge.has.Event) event.unload();
    if(cartridge.has.SA1) sa1.unload();
    if(cartridge.has.SuperFX) superfx.unload();
    if(cartridge.has.ARMDSP) armdsp.unload();
    if(cartridge.has.HitachiDSP) hitachidsp.unload();
    if(cartridge.has.NECDSP) necdsp.unload();
    if(cartridge.has.EpsonRTC) epsonrtc.unload();
    if(cartridge.has.SharpRTC) sharprtc.unload();
    if(cartridge.has.SPC7110) spc7110.unload();
    if(cartridge.has.SDD1) sdd1.unload();
    if(cartridge.has.OBC1) obc1.unload();
    if(cartridge.has.MSU1) msu1.unload();
    if(cartridge.has.BSMemorySlot) bsmemory.unload();
    if(cartridge.has.SufamiTurboSlotA) sufamiturboA.unload();
    if(cartridge.has.SufamiTurboSlotB) sufamiturboB.unload();

    cartridge.unload();
    information.loaded = false;
  }
}

void System::power(bool reset) {
  audio.reset();

  random.entropy((Random::Entropy)configuration.entropy);

  cpu.power(reset);
  smp.power();
  dsp.power(reset);
  ppu.power(reset);

  if(cartridge.has.ICD) icd.power();
  if(cartridge.has.MCC) mcc.power();
  if(cartridge.has.DIP) dip.power();
  if(cartridge.has.Event) event.power();
  if(cartridge.has.SA1) sa1.power();
  if(cartridge.has.SuperFX) superfx.power();
  if(cartridge.has.ARMDSP) armdsp.power();
  if(cartridge.has.HitachiDSP) hitachidsp.power();
  if(cartridge.has.NECDSP) necdsp.power();
  if(cartridge.has.EpsonRTC) epsonrtc.power();
  if(cartridge.has.SharpRTC) sharprtc.power();
  if(cartridge.has.SPC7110) spc7110.power();
  if(cartridge.has.SDD1) sdd1.power();
  if(cartridge.has.OBC1) obc1.power();
  if(cartridge.has.MSU1) msu1.power();
  if(cartridge.has.Cx4) cx4.power();
  if(cartridge.has.DSP1) dsp1.power();
  if(cartridge.has.DSP2) dsp2.power();
  if(cartridge.has.DSP4) dsp4.power();
  if(cartridge.has.ST0010) st0010.power();
  if(cartridge.has.BSMemorySlot) bsmemory.power();
  if(cartridge.has.SufamiTurboSlotA) sufamiturboA.power();
  if(cartridge.has.SufamiTurboSlotB) sufamiturboB.power();

  if(cartridge.has.ICD) cpu.coprocessors.push_back(&icd);
  if(cartridge.has.Event) cpu.coprocessors.push_back(&event);
  if(cartridge.has.SA1) cpu.coprocessors.push_back(&sa1);
  if(cartridge.has.SuperFX) cpu.coprocessors.push_back(&superfx);
  if(cartridge.has.ARMDSP) cpu.coprocessors.push_back(&armdsp);
  if(cartridge.has.HitachiDSP) cpu.coprocessors.push_back(&hitachidsp);
  if(cartridge.has.NECDSP) cpu.coprocessors.push_back(&necdsp);
  if(cartridge.has.EpsonRTC) cpu.coprocessors.push_back(&epsonrtc);
  if(cartridge.has.SharpRTC) cpu.coprocessors.push_back(&sharprtc);
  if(cartridge.has.SPC7110) cpu.coprocessors.push_back(&spc7110);
  if(cartridge.has.MSU1) cpu.coprocessors.push_back(&msu1);
  if(cartridge.has.BSMemorySlot) cpu.coprocessors.push_back(&bsmemory);

  scheduler.active = cpu.thread;

  controllerPort1.power(ID::Port::Controller1);
  controllerPort2.power(ID::Port::Controller2);

  expansionPort.power();
  expansionPort.connect(configuration.expansionPort);

  information.serializeSize[0] = serializeInit(0);
  information.serializeSize[1] = serializeInit(1);
}

}
