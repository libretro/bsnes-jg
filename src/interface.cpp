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

  bind(natural, "System/CPU/Version", system.cpu.version);
  bind(natural, "System/PPU1/Version", system.ppu1.version);
  bind(natural, "System/PPU1/VRAM/Size", system.ppu1.vram.size);
  bind(natural, "System/PPU2/Version", system.ppu2.version);
  bind(text,    "System/Serialization/Method", system.serialization.method);

  bind(boolean, "Video/BlurEmulation", video.blurEmulation);
  bind(boolean, "Video/ColorEmulation", video.colorEmulation);

  bind(boolean, "Hacks/Hotfixes", hacks.hotfixes);
  bind(text,    "Hacks/Entropy", hacks.entropy);
  bind(natural, "Hacks/CPU/Overclock", hacks.cpu.overclock);
  bind(boolean, "Hacks/CPU/FastMath", hacks.cpu.fastMath);
  bind(boolean, "Hacks/Coprocessor/DelayedSync", hacks.coprocessor.delayedSync);
  bind(boolean, "Hacks/Coprocessor/PreferHLE", hacks.coprocessor.preferHLE);
  bind(natural, "Hacks/SA1/Overclock", hacks.sa1.overclock);
  bind(natural, "Hacks/SuperFX/Overclock", hacks.superfx.overclock);

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

auto Interface::information() -> Information {
  Information information;
  information.manufacturer = "Nintendo";
  information.name         = "Super Famicom";
  information.extension    = "sfc";
  information.resettable   = true;
  return information;
}

auto Interface::display() -> Display {
  Display display;
  display.type   = Display::Type::CRT;
  display.colors = 1 << 19;
  display.width  = 256;
  display.height = 240;
  display.internalWidth  = 512;
  display.internalHeight = 480;
  display.aspectCorrection = 8.0 / 7.0;
  return display;
}

uint64_t Interface::color(uint32_t color) {
  unsigned r = color >>  0 & 31;
  unsigned g = color >>  5 & 31;
  unsigned b = color >> 10 & 31;
  unsigned l = color >> 15 & 15;

  auto normalize = [](uint64_t color, unsigned sourceDepth, unsigned targetDepth) {
    if(sourceDepth == 0 || targetDepth == 0) return (uint64_t)0;
    while(sourceDepth < targetDepth) {
      color = (color << sourceDepth) | color;
      sourceDepth += sourceDepth;
    }
    if(targetDepth < sourceDepth) color >>= (sourceDepth - targetDepth);
    return color;
  };

  //luma=0 is not 100% black; but it's much darker than normal linear scaling
  //exact effect seems to be analog; requires > 24-bit color depth to represent accurately
  double L = (1.0 + l) / 16.0 * (l ? 1.0 : 0.25);
  uint64_t R = L * normalize(r, 5, 16);
  uint64_t G = L * normalize(g, 5, 16);
  uint64_t B = L * normalize(b, 5, 16);

  if(SuperFamicom::configuration.video.colorEmulation) {
    static const uint8_t gammaRamp[32] = {
      0x00, 0x01, 0x03, 0x06, 0x0a, 0x0f, 0x15, 0x1c,
      0x24, 0x2d, 0x37, 0x42, 0x4e, 0x5b, 0x69, 0x78,
      0x88, 0x90, 0x98, 0xa0, 0xa8, 0xb0, 0xb8, 0xc0,
      0xc8, 0xd0, 0xd8, 0xe0, 0xe8, 0xf0, 0xf8, 0xff,
    };
    R = L * gammaRamp[r] * 0x0101;
    G = L * gammaRamp[g] * 0x0101;
    B = L * gammaRamp[b] * 0x0101;
  }

  return R << 32 | G << 16 | B << 0;
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

auto Interface::ports() -> std::vector<Port> { return {
  {ID::Port::Controller1, "Controller Port 1"},
  {ID::Port::Controller2, "Controller Port 2"},
  {ID::Port::Expansion,   "Expansion Port"   }};
}

auto Interface::devices(unsigned port) -> std::vector<Device> {
  if(port == ID::Port::Controller1) return {
    {ID::Device::None,    "None"   },
    {ID::Device::Gamepad, "Gamepad"},
    {ID::Device::Mouse,   "Mouse"  }
  };

  if(port == ID::Port::Controller2) return {
    {ID::Device::None,          "None"          },
    {ID::Device::Gamepad,       "Gamepad"       },
    {ID::Device::Mouse,         "Mouse"         },
    {ID::Device::SuperMultitap, "Super Multitap"},
    {ID::Device::SuperScope,    "Super Scope"   },
    {ID::Device::Justifier,     "Justifier"     },
    {ID::Device::Justifiers,    "Justifiers"    }
  };

  if(port == ID::Port::Expansion) return {
    {ID::Device::None,        "None"       },
    {ID::Device::Satellaview, "Satellaview"},
    {ID::Device::S21FX,       "21fx"       }
  };

  return {};
}

auto Interface::inputs(unsigned device) -> std::vector<Input> {
  if(device == ID::Device::None) return {
  };

  if(device == ID::Device::Gamepad) return {
    {Input::Type::Hat,     "Up"    },
    {Input::Type::Hat,     "Down"  },
    {Input::Type::Hat,     "Left"  },
    {Input::Type::Hat,     "Right" },
    {Input::Type::Button,  "B"     },
    {Input::Type::Button,  "A"     },
    {Input::Type::Button,  "Y"     },
    {Input::Type::Button,  "X"     },
    {Input::Type::Trigger, "L"     },
    {Input::Type::Trigger, "R"     },
    {Input::Type::Control, "Select"},
    {Input::Type::Control, "Start" }
  };

  if(device == ID::Device::Mouse) return {
    {Input::Type::Axis,   "X-axis"},
    {Input::Type::Axis,   "Y-axis"},
    {Input::Type::Button, "Left"  },
    {Input::Type::Button, "Right" }
  };

  // Not strictly needed, used for GUI interfaces, fix after nall removal
  /*if(device == ID::Device::SuperMultitap) {
    vector<Input> inputs;
    for(unsigned p = 2; p <= 5; p++) inputs.append({
      {Input::Type::Hat,     {"Port ", p, " - ", "Up"    }},
      {Input::Type::Hat,     {"Port ", p, " - ", "Down"  }},
      {Input::Type::Hat,     {"Port ", p, " - ", "Left"  }},
      {Input::Type::Hat,     {"Port ", p, " - ", "Right" }},
      {Input::Type::Button,  {"Port ", p, " - ", "B"     }},
      {Input::Type::Button,  {"Port ", p, " - ", "A"     }},
      {Input::Type::Button,  {"Port ", p, " - ", "Y"     }},
      {Input::Type::Button,  {"Port ", p, " - ", "X"     }},
      {Input::Type::Trigger, {"Port ", p, " - ", "L"     }},
      {Input::Type::Trigger, {"Port ", p, " - ", "R"     }},
      {Input::Type::Control, {"Port ", p, " - ", "Select"}},
      {Input::Type::Control, {"Port ", p, " - ", "Start" }}
    });
    return inputs;
  }*/

  if(device == ID::Device::SuperScope) return {
    {Input::Type::Axis,    "X-axis" },
    {Input::Type::Axis,    "Y-axis" },
    {Input::Type::Control, "Trigger"},
    {Input::Type::Control, "Cursor" },
    {Input::Type::Control, "Turbo"  },
    {Input::Type::Control, "Pause"  }
  };

  if(device == ID::Device::Justifier) return {
    {Input::Type::Axis,    "X-axis" },
    {Input::Type::Axis,    "Y-axis" },
    {Input::Type::Control, "Trigger"},
    {Input::Type::Control, "Start"  }
  };

  if(device == ID::Device::Justifiers) return {
    {Input::Type::Axis,    "Port 1 - X-axis" },
    {Input::Type::Axis,    "Port 1 - Y-axis" },
    {Input::Type::Control, "Port 1 - Trigger"},
    {Input::Type::Control, "Port 1 - Start"  },
    {Input::Type::Axis,    "Port 2 - X-axis" },
    {Input::Type::Axis,    "Port 2 - Y-axis" },
    {Input::Type::Control, "Port 2 - Trigger"},
    {Input::Type::Control, "Port 2 - Start"  }
  };

  if(device == ID::Device::Satellaview) return {
  };

  if(device == ID::Device::S21FX) return {
  };

  return {};
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
