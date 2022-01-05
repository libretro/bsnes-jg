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

#include "controller/controller.hpp"
#include "interface.hpp"

#include "cartridge.hpp"
#include "coprocessor/icd.hpp"
#include "cpu.hpp"
#include "expansion/expansion.hpp"
#include "cheat.hpp"

namespace SuperFamicom {

extern Emulator::Cheat cheat;

Settings settings;
Configuration configuration;


bool Interface::loaded() {
  return system.loaded();
}

bool Interface::load() {
  return system.load(this);
}

void Interface::save() {
  system.save();
}

void Interface::unload() {
  save();
  system.unload();
}

unsigned Interface::connected(unsigned port) {
  if(port == ID::Port::Controller1) return settings.controllerPort1;
  if(port == ID::Port::Controller2) return settings.controllerPort2;
  if(port == ID::Port::Expansion) return settings.expansionPort;
  return 0;
}

void Interface::connect(unsigned port, unsigned device) {
  if(port == ID::Port::Controller1) controllerPort1.connect(settings.controllerPort1 = device);
  if(port == ID::Port::Controller2) controllerPort2.connect(settings.controllerPort2 = device);
  if(port == ID::Port::Expansion) expansionPort.connect(settings.expansionPort = device);
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

uint8_t Interface::read(uint32_t address) {
  return cpu.readDisassembler(address);
}

void Interface::cheats(const std::vector<std::string>& list) {
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

unsigned Interface::frameSkip() {
  return system.frameSkip;
}

void Interface::setFrameSkip(unsigned frameSkip) {
  system.frameSkip = frameSkip;
  system.frameCounter = frameSkip;
}

bool Interface::runAhead() {
  return system.runAhead;
}

void Interface::setRunAhead(bool runAhead) {
  system.runAhead = runAhead;
}

}
