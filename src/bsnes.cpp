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

unsigned Interface::connected(unsigned port) {
  switch(port) {
    case ID::Port::Controller1: return configuration.controllerPort1;
    case ID::Port::Controller2: return configuration.controllerPort2;
    case ID::Port::Expansion:   return configuration.expansionPort;
    default: return 0;
  }
}

void Interface::connect(unsigned port, unsigned device) {
  switch(port) {
    case ID::Port::Controller1:
      controllerPort1.connect(configuration.controllerPort1 = device);
      break;
    case ID::Port::Controller2:
      controllerPort2.connect(configuration.controllerPort2 = device);
      break;
    case ID::Port::Expansion:
      expansionPort.connect(configuration.expansionPort = device);
      break;
  }
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
  return (cartridge.has.EpsonRTC || cartridge.has.SharpRTC);
}

void Interface::serialize(std::vector<uint8_t>& state, bool synchronize) {
  serializer s = system.serialize(synchronize);
  state = std::vector<uint8_t>(s.data(), s.data() + s.size());
}

bool Interface::unserialize(std::vector<uint8_t>& state) {
  serializer s(state.data(), state.size());
  return system.unserialize(s);
}

void Interface::cheatsApply(const std::vector<std::string>& list) {
  if(cartridge.has.ICD) {
    cheat.assign(list);
    return;
  }

  //make all ROM data writable temporarily
  Memory::GlobalWriteEnable = true;

  Cheat oldCheat = cheat;
  Cheat newCheat;
  newCheat.assign(list);

  //determine all old codes to remove
  for(Cheat::Code& oldCode : oldCheat.codes) {
    bool found = false;
    for(Cheat::Code& newCode : newCheat.codes) {
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
  for(Cheat::Code& newCode : newCheat.codes) {
    bool found = false;
    for(Cheat::Code& oldCode : oldCheat.codes) {
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

void Interface::setAudioFrequency(double freq) {
  audio.setFrequency(freq);
}

void Interface::setAudioSpf(unsigned spf) {
  audio.setSpf(spf);
}

void Interface::setAudioQuality(unsigned rsqual) {
  audio.setQuality(rsqual);
}

void Interface::setCoprocDelayedSync(bool value) {
  configuration.coprocessor.delayedSync = value;
}

void Interface::setCoprocPreferHLE(bool value) {
  configuration.coprocessor.preferHLE = value;
}

void Interface::setInputGamepadCallback(unsigned (*cb)(unsigned)) {
  setInputPollGamepad(cb);
}

void Interface::setInputMouseCallback(int (*cb)(unsigned, unsigned)) {
  setInputPollMouse(cb);
}

void Interface::setInputSuperScopeCallback(int (*cb)(unsigned, unsigned)) {
  setInputPollSuperScope(cb);
}

void Interface::setLogCallback(void (*cb)(int, const char *, ...)) {
  logger.setCallback(cb);
}

void Interface::setOpenStreamCallback(bool (*cb)(std::string, std::stringstream&)) {
  cartridge.setOpenStreamCallback(cb);
}

void Interface::setOpenFileCallback(bool (*cb)(unsigned, std::string, std::vector<uint8_t>&)) {
  cartridge.setOpenFileCallback(cb);
  icd.setOpenFileCallback(cb);
  msu1.setOpenFileCallback(cb);
}

void Interface::setRegion(std::string region) {
    cartridge.setRegion(region);
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

void Interface::setVideoPixelFormat(unsigned pixfmt) {
  ppu.setPixelFormat(pixfmt);
}

void Interface::setVideoCallback(void (*cb)(const uint16_t*, unsigned, unsigned, unsigned)) {
  ppu.setCallback(cb);
}

}
