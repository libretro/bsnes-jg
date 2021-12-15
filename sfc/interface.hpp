namespace SuperFamicom {

struct ID {
  enum : uint {
    System,
    SuperFamicom,
    GameBoy,
    BSMemory,
    SufamiTurboA,
    SufamiTurboB,
  };

  struct Port { enum : uint {
    Controller1,
    Controller2,
    Expansion,
  };};

  struct Device { enum : uint {
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
  auto color(uint32 color) -> uint64 override;

  auto loaded() -> bool override;
  auto hashes() -> vector<string> override;
  auto manifests() -> vector<string> override;
  auto titles() -> vector<string> override;
  auto title() -> string override;
  auto load() -> bool override;
  auto save() -> void override;
  auto unload() -> void override;

  auto ports() -> vector<Port> override;
  auto devices(uint port) -> vector<Device> override;
  auto inputs(uint device) -> vector<Input> override;

  auto connected(uint port) -> uint override;
  auto connect(uint port, uint device) -> void override;
  auto power() -> void override;
  auto reset() -> void override;
  auto run() -> void override;

  auto rtc() -> bool override;
  auto synchronize(uint64 timestamp) -> void override;

  auto serialize(bool synchronize = true) -> serializer override;
  auto unserialize(serializer&) -> bool override;

  auto read(uint24 address) -> uint8 override;
  auto cheats(const vector<string>&) -> void override;

  auto configuration() -> string override;
  auto configuration(string name) -> string override;
  auto configure(string configuration) -> bool override;
  auto configure(string name, string value) -> bool override;

  auto frameSkip() -> uint override;
  auto setFrameSkip(uint frameSkip) -> void override;

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
      uint version = 2;
    } cpu;
    struct PPU1 {
      uint version = 1;
      struct VRAM {
        uint size = 0x10000;
      } vram;
    } ppu1;
    struct PPU2 {
      uint version = 3;
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
      uint overclock = 100;
      bool fastMath = false;
    } cpu;
    struct PPU {
      bool fast = true;
      bool deinterlace = true;
      bool noSpriteLimit = false;
      bool noVRAMBlocking = false;
      uint renderCycle = 512;
      struct Mode7 {
        uint scale = 1;
        bool perspective = true;
        bool supersample = false;
        bool mosaic = true;
      } mode7;
    } ppu;
    struct DSP {
      bool fast = true;
      bool cubic = false;
      bool echoShadow = false;
    } dsp;
    struct Coprocessor {
      bool delayedSync = true;
      bool preferHLE = false;
    } coprocessor;
    struct SA1 {
      uint overclock = 100;
    } sa1;
    struct SuperFX {
      uint overclock = 100;
    } superfx;
  } hacks;

private:
  auto process(Markup::Node document, bool load) -> void;
};

extern Configuration configuration;

struct Settings {
  uint controllerPort1 = ID::Device::Gamepad;
  uint controllerPort2 = ID::Device::Gamepad;
  uint expansionPort = ID::Device::None;
  bool random = true;
};

extern Settings settings;

}
