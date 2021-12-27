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
  bool loaded() override;
  std::vector<std::string> hashes() override;
  std::vector<std::string> manifests() override;
  std::vector<std::string> titles() override;
  std::string title() override;
  bool load() override;
  void save() override;
  void unload() override;

  unsigned connected(unsigned port) override;
  void connect(unsigned port, unsigned device) override;
  void power() override;
  void reset() override;
  void run() override;

  bool rtc() override;

  serializer serialize(bool synchronize = true) override;
  bool unserialize(serializer&) override;

  uint8_t read(nall::Natural<24> address) override;
  void cheats(const std::vector<std::string>&) override;

  unsigned frameSkip() override;
  void setFrameSkip(unsigned frameSkip) override;

  bool runAhead() override;
  void setRunAhead(bool runAhead) override;
};

struct Configuration {
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
      std::string method = "Fast";
    } serialization;
  } system;

  bool hotfixes = true;
  std::string entropy = "Low";
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

private:
  void process(Markup::Node document, bool load);
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
