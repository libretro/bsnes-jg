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

#include "serializer.hpp"
#include "cpu.hpp"
#include "random.hpp"

#include "armdsp.hpp"

namespace SuperFamicom {

//note: timings are completely unverified
//due to the ST018 chip design (on-die ROM), testing is nearly impossible

void ArmDSP::sleep() {
  step(1);
}

uint32_t ArmDSP::get(unsigned mode, uint32_t addr) {
  step(1);

  static auto memory = [&](const uint8_t* mem, unsigned _mode, uint32_t _addr) -> uint32_t {
    if(_mode & Word) {
      mem += _addr & ~3;
      return mem[0] << 0 | mem[1] << 8 | mem[2] << 16 | mem[3] << 24;
    } else if(_mode & Byte) {
      return mem[_addr];
    } else {
      return 0;  //should never occur
    }
  };

  switch(addr & 0xe0000000) {
  case 0x00000000: return memory(programROM, mode, addr & 0x1ffff);
  case 0x20000000: return pipeline.fetch.instruction;
  case 0x40000000: break;
  case 0x60000000: return 0x40404001;
  case 0x80000000: return pipeline.fetch.instruction;
  case 0xa0000000: return memory(dataROM, mode, addr & 0x7fff);
  case 0xc0000000: return pipeline.fetch.instruction;
  case 0xe0000000: return memory(programRAM, mode, addr & 0x3fff);
  }

  addr &= 0xe000003f;

  if(addr == 0x40000010) {
    if(bridge.cputoarm.ready) {
      bridge.cputoarm.ready = false;
      return bridge.cputoarm.data;
    }
  }

  if(addr == 0x40000020) {
    return bridge.status();
  }

  return 0;
}

void ArmDSP::set(unsigned mode, uint32_t addr, uint32_t word) {
  step(1);

  static auto memory = [](uint8_t *mem, unsigned _mode, uint32_t _addr, uint32_t shrt) {
    if(_mode & Word) {
      mem += _addr & ~3;
      *mem++ = shrt >>  0;
      *mem++ = shrt >>  8;
      *mem++ = shrt >> 16;
      *mem++ = shrt >> 24;
    } else if(_mode & Byte) {
      mem += _addr;
      *mem++ = shrt >>  0;
    }
  };

  switch(addr & 0xe0000000) {
  case 0x00000000: return;
  case 0x20000000: return;
  case 0x40000000: break;
  case 0x60000000: return;
  case 0x80000000: return;
  case 0xa0000000: return;
  case 0xc0000000: return;
  case 0xe0000000: return memory(programRAM, mode, addr & 0x3fff, word);
  }

  addr &= 0xe000003f;
  word &= 0x000000ff;

  if(addr == 0x40000000) {
    bridge.armtocpu.ready = true;
    bridge.armtocpu.data = word;
  }

  if(addr == 0x40000010) bridge.signal = true;

  if(addr == 0x40000020) bridge.timerlatch = (bridge.timerlatch & 0xffff00) | word <<  0;
  if(addr == 0x40000024) bridge.timerlatch = (bridge.timerlatch & 0xff00ff) | word <<  8;
  if(addr == 0x40000028) bridge.timerlatch = (bridge.timerlatch & 0x00ffff) | word << 16;

  if(addr == 0x4000002c) bridge.timer = bridge.timerlatch;
}

std::vector<uint8_t> ArmDSP::firmware() const {
  std::vector<uint8_t> buffer;
  buffer.reserve(128 * 1024 + 32 * 1024);
  for(unsigned n = 0; n < (128 * 1024); ++n) buffer.push_back(programROM[n]);
  for(unsigned n = 0; n < (32 * 1024); ++n) buffer.push_back(dataROM[n]);
  return buffer;
}

void ArmDSP::serialize(serializer& s) {
  ARM7TDMI::serialize(s);
  Thread::serialize(s);

  s.array(programRAM, 16 * 1024);

  s.integer(bridge.cputoarm.ready);
  s.integer(bridge.cputoarm.data);
  s.integer(bridge.armtocpu.ready);
  s.integer(bridge.armtocpu.data);
  s.integer(bridge.timer);
  s.integer(bridge.timerlatch);
  s.integer(bridge.reset);
  s.integer(bridge.ready);
  s.integer(bridge.signal);
}

ArmDSP armdsp;

void ArmDSP::synchronizeCPU() {
  if(clock >= 0) scheduler.resume(cpu.thread);
}

void ArmDSP::Enter() {
  armdsp.boot();
  while(true) {
    scheduler.synchronize();
    armdsp.main();
  }
}

void ArmDSP::boot() {
  //reset hold delay
  while(bridge.reset) {
    step(1);
    continue;
  }

  //reset sequence delay
  if(bridge.ready == false) {
    step(65536);
    bridge.ready = true;
  }
}

void ArmDSP::main() {
  processor.cpsr.t = 0;  //force ARM mode
  instruction();
}

void ArmDSP::step(unsigned clocks) {
  if(bridge.timer) --bridge.timer;
  clock += clocks * (uint64_t)cpu.frequency;
  synchronizeCPU();
}

//MMIO: 00-3f,80-bf:3800-38ff
//3800-3807 mirrored throughout
//a0 ignored

uint8_t ArmDSP::read(unsigned addr, uint8_t) {
  cpu.synchronizeCoprocessors();

  uint8_t data = 0x00;
  addr &= 0xff06;

  if(addr == 0x3800) {
    if(bridge.armtocpu.ready) {
      bridge.armtocpu.ready = false;
      data = bridge.armtocpu.data;
    }
  }

  if(addr == 0x3802) {
    bridge.signal = false;
  }

  if(addr == 0x3804) {
    data = bridge.status();
  }

  return data;
}

void ArmDSP::write(unsigned addr, uint8_t data) {
  cpu.synchronizeCoprocessors();

  addr &= 0xff06;

  if(addr == 0x3802) {
    bridge.cputoarm.ready = true;
    bridge.cputoarm.data = data;
  }

  if(addr == 0x3804) {
    data &= 1;
    if(!bridge.reset && data) reset();
    bridge.reset = data;
  }
}

void ArmDSP::power() {
  random.array((uint8_t*)programRAM, sizeof(programRAM));
  bridge.reset = false;
  reset();
}

void ArmDSP::reset() {
  ARM7TDMI::power();
  create(ArmDSP::Enter, Frequency);

  bridge.ready = false;
  bridge.signal = false;
  bridge.timer = 0;
  bridge.timerlatch = 0;
  bridge.cputoarm.ready = false;
  bridge.armtocpu.ready = false;
}

uint8_t ArmDSP::Bridge::status() const {
  return (
    armtocpu.ready   << 0
    | signal         << 2
    | cputoarm.ready << 3
    | ready          << 7
  );
}

}
