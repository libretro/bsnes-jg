#include <sfc/sfc.hpp>

namespace SuperFamicom {

Settings settings;

Configuration configuration;

auto Configuration::process(Markup::Node document, bool load) -> void {
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
  bind(boolean, "Hacks/PPU/Fast", hacks.ppu.fast);
  bind(boolean, "Hacks/PPU/Deinterlace", hacks.ppu.deinterlace);
  bind(natural, "Hacks/PPU/RenderCycle", hacks.ppu.renderCycle);
  bind(boolean, "Hacks/PPU/NoSpriteLimit", hacks.ppu.noSpriteLimit);
  bind(boolean, "Hacks/PPU/NoVRAMBlocking", hacks.ppu.noVRAMBlocking);
  bind(natural, "Hacks/PPU/Mode7/Scale", hacks.ppu.mode7.scale);
  bind(boolean, "Hacks/PPU/Mode7/Perspective", hacks.ppu.mode7.perspective);
  bind(boolean, "Hacks/PPU/Mode7/Supersample", hacks.ppu.mode7.supersample);
  bind(boolean, "Hacks/PPU/Mode7/Mosaic", hacks.ppu.mode7.mosaic);
  bind(boolean, "Hacks/Coprocessor/DelayedSync", hacks.coprocessor.delayedSync);
  bind(boolean, "Hacks/Coprocessor/PreferHLE", hacks.coprocessor.preferHLE);
  bind(natural, "Hacks/SA1/Overclock", hacks.sa1.overclock);
  bind(natural, "Hacks/SuperFX/Overclock", hacks.superfx.overclock);

  #undef bind
}

auto Configuration::read() -> string {
  Markup::Node document;
  process(document, false);
  return BML::serialize(document, " ");
}

auto Configuration::read(string name) -> string {
  auto document = BML::unserialize(read());
  return document[name].text();
}

auto Configuration::write(string configuration) -> bool {
  *this = {};

  if(auto document = BML::unserialize(configuration)) {
    return process(document, true), true;
  }

  return false;
}

auto Configuration::write(string name, string value) -> bool {
  if(SuperFamicom::system.loaded() && name.beginsWith("System/")) return false;

  auto document = BML::unserialize(read());
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

auto Interface::color(uint32_t color) -> uint64_t {
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

auto Interface::loaded() -> bool {
  return system.loaded();
}

auto Interface::hashes() -> vector<string> {
  return cartridge.hashes();
}

auto Interface::manifests() -> vector<string> {
  return cartridge.manifests();
}

auto Interface::titles() -> vector<string> {
  return cartridge.titles();
}

auto Interface::title() -> string {
  return cartridge.title();
}

auto Interface::load() -> bool {
  return system.load(this);
}

auto Interface::save() -> void {
  system.save();
}

auto Interface::unload() -> void {
  save();
  system.unload();
}

auto Interface::ports() -> vector<Port> { return {
  {ID::Port::Controller1, "Controller Port 1"},
  {ID::Port::Controller2, "Controller Port 2"},
  {ID::Port::Expansion,   "Expansion Port"   }};
}

auto Interface::devices(unsigned port) -> vector<Device> {
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

auto Interface::inputs(unsigned device) -> vector<Input> {
  using Type = Input::Type;

  if(device == ID::Device::None) return {
  };

  if(device == ID::Device::Gamepad) return {
    {Type::Hat,     "Up"    },
    {Type::Hat,     "Down"  },
    {Type::Hat,     "Left"  },
    {Type::Hat,     "Right" },
    {Type::Button,  "B"     },
    {Type::Button,  "A"     },
    {Type::Button,  "Y"     },
    {Type::Button,  "X"     },
    {Type::Trigger, "L"     },
    {Type::Trigger, "R"     },
    {Type::Control, "Select"},
    {Type::Control, "Start" }
  };

  if(device == ID::Device::Mouse) return {
    {Type::Axis,   "X-axis"},
    {Type::Axis,   "Y-axis"},
    {Type::Button, "Left"  },
    {Type::Button, "Right" }
  };

  if(device == ID::Device::SuperMultitap) {
    vector<Input> inputs;
    for(unsigned p = 2; p <= 5; p++) inputs.append({
      {Type::Hat,     {"Port ", p, " - ", "Up"    }},
      {Type::Hat,     {"Port ", p, " - ", "Down"  }},
      {Type::Hat,     {"Port ", p, " - ", "Left"  }},
      {Type::Hat,     {"Port ", p, " - ", "Right" }},
      {Type::Button,  {"Port ", p, " - ", "B"     }},
      {Type::Button,  {"Port ", p, " - ", "A"     }},
      {Type::Button,  {"Port ", p, " - ", "Y"     }},
      {Type::Button,  {"Port ", p, " - ", "X"     }},
      {Type::Trigger, {"Port ", p, " - ", "L"     }},
      {Type::Trigger, {"Port ", p, " - ", "R"     }},
      {Type::Control, {"Port ", p, " - ", "Select"}},
      {Type::Control, {"Port ", p, " - ", "Start" }}
    });
    return inputs;
  }

  if(device == ID::Device::SuperScope) return {
    {Type::Axis,    "X-axis" },
    {Type::Axis,    "Y-axis" },
    {Type::Control, "Trigger"},
    {Type::Control, "Cursor" },
    {Type::Control, "Turbo"  },
    {Type::Control, "Pause"  }
  };

  if(device == ID::Device::Justifier) return {
    {Type::Axis,    "X-axis" },
    {Type::Axis,    "Y-axis" },
    {Type::Control, "Trigger"},
    {Type::Control, "Start"  }
  };

  if(device == ID::Device::Justifiers) return {
    {Type::Axis,    "Port 1 - X-axis" },
    {Type::Axis,    "Port 1 - Y-axis" },
    {Type::Control, "Port 1 - Trigger"},
    {Type::Control, "Port 1 - Start"  },
    {Type::Axis,    "Port 2 - X-axis" },
    {Type::Axis,    "Port 2 - Y-axis" },
    {Type::Control, "Port 2 - Trigger"},
    {Type::Control, "Port 2 - Start"  }
  };

  if(device == ID::Device::Satellaview) return {
  };

  if(device == ID::Device::S21FX) return {
  };

  return {};
}

auto Interface::connected(unsigned port) -> unsigned {
  if(port == ID::Port::Controller1) return settings.controllerPort1;
  if(port == ID::Port::Controller2) return settings.controllerPort2;
  if(port == ID::Port::Expansion) return settings.expansionPort;
  return 0;
}

auto Interface::connect(unsigned port, unsigned device) -> void {
  if(port == ID::Port::Controller1) controllerPort1.connect(settings.controllerPort1 = device);
  if(port == ID::Port::Controller2) controllerPort2.connect(settings.controllerPort2 = device);
  if(port == ID::Port::Expansion) expansionPort.connect(settings.expansionPort = device);
}

auto Interface::power() -> void {
  system.power(/* reset = */ false);
}

auto Interface::reset() -> void {
  system.power(/* reset = */ true);
}

auto Interface::run() -> void {
  system.run();
}

auto Interface::rtc() -> bool {
  if(cartridge.has.EpsonRTC) return true;
  if(cartridge.has.SharpRTC) return true;
  return false;
}

auto Interface::synchronize(uint64_t timestamp) -> void {
  // The line below was commented because in bsnes, there always seems to be
  // a timestamp. This allows nall/chrono.hpp to be removed.
  //if(!timestamp) timestamp = chrono::timestamp();
  if (!timestamp) printf("No timestamp in Interface::synchronize!!!\n");
  if(cartridge.has.EpsonRTC) epsonrtc.synchronize(timestamp);
  if(cartridge.has.SharpRTC) sharprtc.synchronize(timestamp);
}

auto Interface::serialize(bool synchronize) -> serializer {
  return system.serialize(synchronize);
}

auto Interface::unserialize(serializer& s) -> bool {
  return system.unserialize(s);
}

auto Interface::read(uint24 address) -> uint8_t {
  return cpu.readDisassembler(address);
}

auto Interface::cheats(const vector<string>& list) -> void {
  if(cartridge.has.ICD) {
    icd.cheats.assign(list);
    return;
  }

  //make all ROM data writable temporarily
  Memory::GlobalWriteEnable = true;

  Cheat oldCheat = cheat;
  Cheat newCheat;
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
      if(!newCode.compare || newCode.compare() == newCode.restore) {
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

auto Interface::configuration() -> string {
  return SuperFamicom::configuration.read();
}

auto Interface::configuration(string name) -> string {
  return SuperFamicom::configuration.read(name);
}

auto Interface::configure(string configuration) -> bool {
  return SuperFamicom::configuration.write(configuration);
}

auto Interface::configure(string name, string value) -> bool {
  return SuperFamicom::configuration.write(name, value);
}

auto Interface::frameSkip() -> unsigned {
  return system.frameSkip;
}

auto Interface::setFrameSkip(unsigned frameSkip) -> void {
  system.frameSkip = frameSkip;
  system.frameCounter = frameSkip;
}

auto Interface::runAhead() -> bool {
  return system.runAhead;
}

auto Interface::setRunAhead(bool runAhead) -> void {
  system.runAhead = runAhead;
}

}
