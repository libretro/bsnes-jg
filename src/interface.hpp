#include <vector>

namespace SuperFamicom {

struct ID {
  enum : unsigned {
    System,
    SuperFamicom,
    GameBoy,
    BSMemory,
    SufamiTurboA,
    SufamiTurboB,
  };

  struct Port { enum : unsigned {
    Controller1,
    Controller2,
    Expansion,
  };};

  struct Device { enum : unsigned {
    None,
    Gamepad,
    Mouse,
    SuperMultitap,
    SuperScope,
    Justifier,
    Justifiers,

    Satellaview,
    S21FX,
  };};
};

struct Interface : Emulator::Interface {
  auto information() -> Information;

  auto display() -> Display override;
  auto color(uint32_t color) -> uint64_t override;

  auto loaded() -> bool override;
  auto hashes() -> std::vector<string> override;
  auto manifests() -> std::vector<string> override;
  auto titles() -> std::vector<string> override;
  auto title() -> string override;
  auto load() -> bool override;
  auto save() -> void override;
  auto unload() -> void override;

  auto ports() -> std::vector<Port> override;
  auto devices(unsigned port) -> std::vector<Device> override;
  auto inputs(unsigned device) -> std::vector<Input> override;

  auto connected(unsigned port) -> unsigned override;
  auto connect(unsigned port, unsigned device) -> void override;
  auto power() -> void override;
  auto reset() -> void override;
  auto run() -> void override;

  auto rtc() -> bool override;
  auto synchronize(uint64_t timestamp) -> void override;

  auto serialize(bool synchronize = true) -> serializer override;
  auto unserialize(serializer&) -> bool override;

  auto read(nall::Natural<24> address) -> uint8_t override;
  auto cheats(const std::vector<string>&) -> void override;

  auto configuration() -> string override;
  auto configuration(string name) -> string override;
  auto configure(string configuration) -> bool override;
  auto configure(string name, string value) -> bool override;

  auto frameSkip() -> unsigned override;
  auto setFrameSkip(unsigned frameSkip) -> void override;

  auto runAhead() -> bool override;
  auto setRunAhead(bool runAhead) -> void override;
};

struct Configuration {
  auto read() -> string;
  auto read(string) -> string;
  auto write(string) -> bool;
  auto write(string, string) -> bool;

  struct System {
    struct CPU {
      unsigned version = 2;
    } cpu;
    struct PPU1 {
      unsigned version = 1;
      struct VRAM {
        unsigned size = 0x10000;
      } vram;
    } ppu1;
    struct PPU2 {
      unsigned version = 3;
    } ppu2;
    struct Serialization {
      string method = "Fast";
    } serialization;
  } system;

  struct Video {
    bool blurEmulation = true;
    bool colorEmulation = true;
  } video;

  struct Hacks {
    bool hotfixes = true;
    string entropy = "Low";
    struct CPU {
      unsigned overclock = 100;
      bool fastMath = false;
    } cpu;
    struct Coprocessor {
      bool delayedSync = true;
      bool preferHLE = false;
    } coprocessor;
    struct SA1 {
      unsigned overclock = 100;
    } sa1;
    struct SuperFX {
      unsigned overclock = 100;
    } superfx;
  } hacks;

private:
  auto process(Markup::Node document, bool load) -> void;
};

extern Configuration configuration;

struct Settings {
  unsigned controllerPort1 = ID::Device::Gamepad;
  unsigned controllerPort2 = ID::Device::Gamepad;
  unsigned expansionPort = ID::Device::None;
  bool random = true;
};

extern Settings settings;

}
