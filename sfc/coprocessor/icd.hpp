struct ICD : Emulator::Platform, Thread {
  shared_pointer<Emulator::Stream> stream;
  Emulator::Cheat cheats;

  inline auto pathID() const -> uint { return information.pathID; }

  auto synchronizeCPU() -> void;
  static auto Enter() -> void;
  auto main() -> void;
  auto step(uint clocks) -> void;
  auto clockFrequency() const -> uint;

  auto load() -> bool;
  auto save() -> void;
  auto unload() -> void;
  auto power(bool reset = false) -> void;

  //interface.cpp
  auto ppuHreset() -> void;
  auto ppuVreset() -> void;
  auto ppuWrite(uint2 color) -> void;
  auto apuWrite(float left, float right) -> void;
  auto joypWrite(bool p14, bool p15) -> void;

  //io.cpp
  auto readIO(uint addr, uint8_t data) -> uint8_t;
  auto writeIO(uint addr, uint8_t data) -> void;

  //boot-roms.cpp
  static const uint8_t SGB1BootROM[256];
  static const uint8_t SGB2BootROM[256];

  //serialization.cpp
  auto serialize(serializer&) -> void;

  uint Revision = 0;
  uint Frequency = 0;

private:
  struct Packet {
    auto operator[](uint4 address) -> uint8_t& { return data[address]; }
    uint8_t data[16];
  };
  Packet packet[64];
  uint7 packetSize;

  uint2 joypID;
  uint1 joypLock;
  uint1 pulseLock;
  uint1 strobeLock;
  uint1 packetLock;
  Packet joypPacket;
  uint4 packetOffset;
  uint8_t bitData;
  uint3 bitOffset;

  uint8_t output[4 * 512];
  uint2 readBank;
  uint9 readAddress;
  uint2 writeBank;

  uint8_t r6003;      //control port
  uint8_t r6004;      //joypad 1
  uint8_t r6005;      //joypad 2
  uint8_t r6006;      //joypad 3
  uint8_t r6007;      //joypad 4
  uint8_t r7000[16];  //JOYP packet data
  uint8_t mltReq;     //number of active joypads

  uint8_t hcounter;
  uint8_t vcounter;

  struct Information {
    uint pathID = 0;
  } information;

public:
  //warning: the size of this object will be too large due to C++ size rules differing from C rules.
  //in practice, this won't pose a problem so long as the struct is never accessed from C++ code,
  //as the offsets of all member variables will be wrong compared to what the C SameBoy code expects.
  GB_gameboy_t sameboy;
  uint32_t bitmap[160 * 144];
};

extern ICD icd;
