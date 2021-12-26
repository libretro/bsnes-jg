#include "sfc.hpp"

namespace SuperFamicom {

Settings settings;

Configuration configuration;

void Configuration::process(nall::Markup::Node document, bool load) {
  #define bind(type, path, name) \
    if(load) { \
      if(auto node = document[path]) name = node.type(); \
    } else { \
      document(path).setValue(name); \
    } \

  bind(boolean, "Hacks/Coprocessor/DelayedSync", hacks.coprocessor.delayedSync);
  bind(boolean, "Hacks/Coprocessor/PreferHLE", hacks.coprocessor.preferHLE);

  #undef bind
}

nall::string Configuration::read() {
  nall::Markup::Node document;
  process(document, false);
  return nall::BML::serialize(document, " ");
}

nall::string Configuration::read(nall::string name) {
  auto document = nall::BML::unserialize(read());
  return document[name].text();
}

bool Configuration::write(nall::string configuration) {
  *this = {};

  if(auto document = nall::BML::unserialize(configuration)) {
    return process(document, true), true;
  }

  return false;
}

bool Configuration::write(nall::string name, nall::string value) {
  if(SuperFamicom::system.loaded() && name.beginsWith("System/")) return false;

  auto document = nall::BML::unserialize(read());
  if(auto node = document[name]) {
    node.setValue(value);
    return process(document, true), true;
  }

  return false;
}

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

void Interface::synchronize(uint64_t timestamp) {
  // The line below was commented because in bsnes, there always seems to be
  // a timestamp. This allows nall/chrono.hpp to be removed.
  //if(!timestamp) timestamp = chrono::timestamp();
  if (!timestamp) printf("No timestamp in Interface::synchronize!!!\n");
  if(cartridge.has.EpsonRTC) epsonrtc.synchronize(timestamp);
  if(cartridge.has.SharpRTC) sharprtc.synchronize(timestamp);
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

nall::string Interface::configuration() {
  return SuperFamicom::configuration.read();
}

nall::string Interface::configuration(nall::string name) {
  return SuperFamicom::configuration.read(name);
}

bool Interface::configure(nall::string configuration) {
  return SuperFamicom::configuration.write(configuration);
}

bool Interface::configure(nall::string name, nall::string value) {
  return SuperFamicom::configuration.write(name, value);
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
