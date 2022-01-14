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

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

#include "serializer.hpp"
#include "audio.hpp"
#include "cartridge.hpp"
#include "cheat.hpp"
#include "controller.hpp"
#include "coprocessor/icd.hpp"
#include "coprocessor/msu1.hpp"
#include "expansion/expansion.hpp"
#include "ppu.hpp"
#include "settings.hpp"
#include "system.hpp"

#include "interface.hpp"

namespace SuperFamicom {

extern Emulator::Cheat cheat;

Configuration configuration;

static std::vector<std::string> cheatList;

bool Interface::loaded() {
  return system.loaded();
}

bool Interface::load() {
  return system.load();
}

void Interface::save() {
  system.save();
}

void Interface::unload() {
  save();
  system.unload();
}

void Interface::configure(std::string name, int value) {
  if (name == "CoprocessorDelayedSync") {
    configuration.coprocessor.delayedSync = (bool)value;
  }
  else if (name == "CoprocessorPreferHLE") {
    configuration.coprocessor.preferHLE = (bool)value;
  }
}

unsigned Interface::connected(unsigned port) {
  if(port == ID::Port::Controller1) return configuration.controllerPort1;
  if(port == ID::Port::Controller2) return configuration.controllerPort2;
  if(port == ID::Port::Expansion) return configuration.expansionPort;
  return 0;
}

void Interface::connect(unsigned port, unsigned device) {
  if(port == ID::Port::Controller1) controllerPort1.connect(configuration.controllerPort1 = device);
  if(port == ID::Port::Controller2) controllerPort2.connect(configuration.controllerPort2 = device);
  if(port == ID::Port::Expansion) expansionPort.connect(configuration.expansionPort = device);
}

void Interface::power() {
  system.power(/* reset = */ false);
}

void Interface::reset() {
  system.power(/* reset = */ true);
}

void Interface::run() {
  system.run();
}

bool Interface::rtc() {
  if(cartridge.has.EpsonRTC) return true;
  if(cartridge.has.SharpRTC) return true;
  return false;
}

serializer Interface::serialize(bool synchronize) {
  return system.serialize(synchronize);
}

bool Interface::unserialize(serializer& s) {
  return system.unserialize(s);
}

void Interface::cheatsApply(const std::vector<std::string>& list) {
  if(cartridge.has.ICD) {
    icd.cheats.assign(list);
    return;
  }

  //make all ROM data writable temporarily
  Memory::GlobalWriteEnable = true;

  Emulator::Cheat oldCheat = cheat;
  Emulator::Cheat newCheat;
  newCheat.assign(list);

  //determine all old codes to remove
  for(auto& oldCode : oldCheat.codes) {
    bool found = false;
    for(auto& newCode : newCheat.codes) {
      if(oldCode == newCode) {
        found = true;
        break;
      }
    }
    if(!found) {
      //remove old cheat
      if(oldCode.enable) {
        bus.write(oldCode.address, oldCode.restore);
      }
    }
  }

  //determine all new codes to create
  for(auto& newCode : newCheat.codes) {
    bool found = false;
    for(auto& oldCode : oldCheat.codes) {
      if(newCode == oldCode) {
        found = true;
        break;
      }
    }
    if(!found) {
      //create new cheat
      newCode.restore = bus.read(newCode.address);
      if(!newCode.compare || newCode.compare == newCode.restore) {
        newCode.enable = true;
        bus.write(newCode.address, newCode.data);
      } else {
        newCode.enable = false;
      }
    }
  }

  cheat = newCheat;

  //restore ROM write protection
  Memory::GlobalWriteEnable = false;
}

void Interface::cheatsClear() {
  cheatList.clear();
  cheatsApply(cheatList);
}

bool Interface::cheatsDecode(int sys, std::string code) {
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

bool Interface::runAhead() {
  return system.runAhead;
}

void Interface::setRunAhead(bool runAhead) {
  system.runAhead = runAhead;
}

std::string Interface::getRegion() {
  return cartridge.region();
}

void Interface::setAudioBuffer(float *buf) {
  audio.setBuffer(buf);
}
void Interface::setAudioCallback(void (*cb)(size_t)) {
  audio.setCallback(cb);
}

void Interface::setAudioSpf(unsigned spf) {
  audio.setSpf(spf);
}

void Interface::setAudioQuality(unsigned rsqual) {
  audio.setQuality(rsqual);
}

void Interface::setInputCallback(int16_t (*cb)(unsigned, unsigned, unsigned)) {
  setInputPoll(cb);
}

void Interface::setOpenCallback(std::ifstream (*cb)(unsigned, std::string)) {
  cartridge.setOpenCallback(cb);
  icd.setOpenCallback(cb);
  msu1.setOpenCallback(cb);
}

void Interface::setRomCallback(bool (*cb)(unsigned)) {
  cartridge.setRomCallback(cb);
}

void Interface::setRomBSMemory(std::vector<uint8_t>& data, std::string& loc) {
  cartridge.setRomBSMemory(data, loc);
}

void Interface::setRomGB(const uint8_t *data, size_t size) {
  icd.setRom(data, size);
}

void Interface::setRomSufamiTurboA(std::vector<uint8_t>& data, std::string& loc) {
  cartridge.setRomSufamiTurboA(data, loc);
}

void Interface::setRomSufamiTurboB(std::vector<uint8_t>& data, std::string& loc) {
  cartridge.setRomSufamiTurboB(data, loc);
}

void Interface::setRomSuperFamicom(std::vector<uint8_t>& data, std::string& loc) {
  cartridge.setRomSuperFamicom(data, loc);
}

void Interface::setWriteCallback(void (*cb)(unsigned, std::string, const uint8_t*, unsigned)) {
  cartridge.setWriteCallback(cb);
  icd.setWriteCallback(cb);
}

void Interface::setVideoBuffer(uint16_t *buffer) {
  ppu.setBuffer(buffer);
}

void Interface::setVideoCallback(void (*cb)(const uint16_t*, unsigned, unsigned, unsigned)) {
  ppu.setCallback(cb);
}

}
