#include "sfc.hpp"

namespace SuperFamicom {

Settings settings;
Configuration configuration;


bool Interface::loaded() {
  return system.loaded();
}

std::vector<std::string> Interface::hashes() {
  return cartridge.hashes();
}

std::vector<std::string> Interface::manifests() {
  return cartridge.manifests();
}

std::vector<std::string> Interface::titles() {
  return cartridge.titles();
}

std::string Interface::title() {
  return cartridge.title();
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

uint8_t Interface::read(nall::Natural<24> address) {
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
