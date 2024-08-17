/*
 * bsnes-jg - Super Nintendo emulator
 *
 * Copyright (C) 2004-2020 byuu
 * Copyright (C) 2020-2024 Rupert Carmichael
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

#include <cstddef>

#include "audio.hpp"
#include "cartridge.hpp"
#include "cheat.hpp"
#include "controller.hpp"
#include "coprocessor/icd.hpp"
#include "coprocessor/msu1.hpp"
#include "expansion/expansion.hpp"
#include "logger.hpp"
#include "ppu.hpp"
#include "serializer.hpp"
#include "settings.hpp"
#include "system.hpp"

#include "bsnes.hpp"

namespace SuperFamicom {
Configuration configuration;
};

static std::vector<std::string> cheatList;

bool Bsnes::loaded() {
  return SuperFamicom::system.loaded();
}

bool Bsnes::load() {
  return SuperFamicom::system.load();
}

void Bsnes::save() {
  SuperFamicom::system.save();
}

void Bsnes::unload() {
  save();
  SuperFamicom::system.unload();
}

unsigned Bsnes::connected(unsigned port) {
  switch(port) {
    case SuperFamicom::ID::Port::Controller1: return SuperFamicom::configuration.controllerPort1;
    case SuperFamicom::ID::Port::Controller2: return SuperFamicom::configuration.controllerPort2;
    case SuperFamicom::ID::Port::Expansion:   return SuperFamicom::configuration.expansionPort;
    default: return 0;
  }
}

void Bsnes::power() {
  SuperFamicom::system.power(/* reset = */ false);
}

void Bsnes::reset() {
  SuperFamicom::system.power(/* reset = */ true);
}

void Bsnes::run() {
  SuperFamicom::system.run();
}

bool Bsnes::rtc() {
  return (SuperFamicom::cartridge.has.EpsonRTC || SuperFamicom::cartridge.has.SharpRTC);
}

void Bsnes::serialize(std::vector<uint8_t>& state, bool synchronize) {
  serializer s = SuperFamicom::system.serialize(synchronize);
  state = std::vector<uint8_t>(s.data(), s.data() + s.size());
}

bool Bsnes::unserialize(std::vector<uint8_t>& state) {
  serializer s(state.data(), state.size());
  return SuperFamicom::system.unserialize(s);
}

void Bsnes::cheatsApply(const std::vector<std::string>& list) {
  if(SuperFamicom::cartridge.has.ICD) {
    SuperFamicom::cheat.assign(list);
    return;
  }

  //make all ROM data writable temporarily
  SuperFamicom::Memory::GlobalWriteEnable = true;

  SuperFamicom::Cheat oldCheat = SuperFamicom::cheat;
  SuperFamicom::Cheat newCheat;
  newCheat.assign(list);

  //determine all old codes to remove
  for(SuperFamicom::Cheat::Code& oldCode : oldCheat.codes) {
    bool found = false;
    for(SuperFamicom::Cheat::Code& newCode : newCheat.codes) {
      if(oldCode == newCode) {
        found = true;
        break;
      }
    }
    if(!found) {
      //remove old cheat
      if(oldCode.enable) {
        SuperFamicom::bus.write(oldCode.address, oldCode.restore);
      }
    }
  }

  //determine all new codes to create
  for(SuperFamicom::Cheat::Code& newCode : newCheat.codes) {
    bool found = false;
    for(SuperFamicom::Cheat::Code& oldCode : oldCheat.codes) {
      if(newCode == oldCode) {
        found = true;
        break;
      }
    }
    if(!found) {
      //create new cheat
      newCode.restore = SuperFamicom::bus.read(newCode.address);
      if(!newCode.compare || newCode.compare == newCode.restore) {
        newCode.enable = true;
        SuperFamicom::bus.write(newCode.address, newCode.data);
      } else {
        newCode.enable = false;
      }
    }
  }

  SuperFamicom::cheat = newCheat;

  //restore ROM write protection
  SuperFamicom::Memory::GlobalWriteEnable = false;
}

void Bsnes::cheatsClear() {
  cheatList.clear();
  cheatsApply(cheatList);
}

bool Bsnes::cheatsDecode(int sys, std::string code) {
  bool decoded = false;
  if (sys == 1) // Game Boy
    decoded = CheatDecoder::gb(code);
  else // Other
    decoded = CheatDecoder::snes(code);

  if (decoded) {
    cheatList.push_back(code);
    cheatsApply(cheatList);
  }

  return decoded;
}

bool Bsnes::runAhead() {
  return SuperFamicom::system.runAhead;
}

void Bsnes::setRunAhead(bool runAhead) {
  SuperFamicom::system.runAhead = runAhead;
}

unsigned Bsnes::getRegion() {
  return SuperFamicom::cartridge.region() == "PAL" ? Region::PAL : Region::NTSC;
}

void Bsnes::setAudioBuffer(float *buf) {
  SuperFamicom::audio.setBuffer(buf);
}

void Bsnes::setAudioSpec(Audio::Spec spec) {
  SuperFamicom::audio.setFrequency(spec.freq);
  SuperFamicom::audio.setSpf(spec.spf);
  SuperFamicom::audio.setQuality(spec.rsqual);
  SuperFamicom::audio.setCallback(spec.ptr, spec.cb);
}

void Bsnes::setCoprocDelayedSync(bool value) {
  SuperFamicom::configuration.coprocessor.delayedSync = value;
}

void Bsnes::setCoprocPreferHLE(bool value) {
  SuperFamicom::configuration.coprocessor.preferHLE = value;
}

void Bsnes::setInputDevice(Input::Spec spec) {
  switch(spec.port) {
    case SuperFamicom::ID::Port::Controller1:
      SuperFamicom::controllerPort1.connect(
        SuperFamicom::configuration.controllerPort1 = spec.device,
        spec.ptr, spec.cb
      );
      break;
    case SuperFamicom::ID::Port::Controller2:
      SuperFamicom::controllerPort2.connect(
        SuperFamicom::configuration.controllerPort2 = spec.device,
        spec.ptr, spec.cb
      );
      break;
    case SuperFamicom::ID::Port::Expansion:
      SuperFamicom::expansionPort.connect(
        SuperFamicom::configuration.expansionPort = spec.device
      );
      break;
  }
}

void Bsnes::setLogCallback(void (*cb)(int, const char *, ...)) {
  logger.setCallback(cb);
}

void Bsnes::setOpenStreamCallback(bool (*cb)(std::string, std::stringstream&)) {
  SuperFamicom::cartridge.setOpenStreamCallback(cb);
}

void Bsnes::setOpenFileCallback(bool (*cb)(unsigned, std::string, std::vector<uint8_t>&)) {
  SuperFamicom::cartridge.setOpenFileCallback(cb);
  SuperFamicom::icd.setOpenFileCallback(cb);
  SuperFamicom::msu1.setOpenFileCallback(cb);
}

void Bsnes::setRegion(std::string region) {
  SuperFamicom::cartridge.setRegion(region);
}

void Bsnes::setRomCallback(bool (*cb)(unsigned)) {
  SuperFamicom::cartridge.setRomCallback(cb);
}

void Bsnes::setRomBSMemory(std::vector<uint8_t>& data, std::string& loc) {
  SuperFamicom::cartridge.setRomBSMemory(data, loc);
}

void Bsnes::setRomGB(const uint8_t *data, size_t size) {
  SuperFamicom::icd.setRom(data, size);
}

void Bsnes::setRomSufamiTurboA(std::vector<uint8_t>& data, std::string& loc) {
  SuperFamicom::cartridge.setRomSufamiTurboA(data, loc);
}

void Bsnes::setRomSufamiTurboB(std::vector<uint8_t>& data, std::string& loc) {
  SuperFamicom::cartridge.setRomSufamiTurboB(data, loc);
}

void Bsnes::setRomSuperFamicom(std::vector<uint8_t>& data, std::string& loc) {
  SuperFamicom::cartridge.setRomSuperFamicom(data, loc);
}

void Bsnes::setWriteCallback(void (*cb)(unsigned, std::string, const uint8_t*, unsigned)) {
  SuperFamicom::cartridge.setWriteCallback(cb);
  SuperFamicom::icd.setWriteCallback(cb);
}

void Bsnes::setVideoBuffer(uint16_t *buffer) {
  SuperFamicom::ppu.setBuffer(buffer);
}

void Bsnes::setVideoPixelFormat(unsigned pixfmt) {
  SuperFamicom::ppu.setPixelFormat(pixfmt);
}

void Bsnes::setVideoCallback(void (*cb)(const uint16_t*, unsigned, unsigned, unsigned)) {
  SuperFamicom::ppu.setCallback(cb);
}
