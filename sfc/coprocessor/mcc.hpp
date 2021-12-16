//MCC - Memory Controller Chip
//Custom logic chip inside the BS-X Satellaview base cartridge

struct MCC {
  ReadableMemory rom;
  WritableMemory psram;

  //mcc.cpp
  auto unload() -> void;
  auto power() -> void;
  auto commit() -> void;

  auto read(unsigned address, uint8_t data) -> uint8_t;
  auto write(unsigned address, uint8_t data) -> void;

  auto mcuRead(unsigned address, uint8_t data) -> uint8_t;
  auto mcuWrite(unsigned address, uint8_t data) -> void;

  auto mcuAccess(bool mode, unsigned address, uint8_t data) -> uint8_t;
  auto romAccess(bool mode, unsigned address, uint8_t data) -> uint8_t;
  auto psramAccess(bool mode, unsigned address, uint8_t data) -> uint8_t;
  auto exAccess(bool mode, unsigned address, uint8_t data) -> uint8_t;
  auto bsAccess(bool mode, unsigned address, uint8_t data) -> uint8_t;

  //serialization.cpp
  auto serialize(serializer&) -> void;

private:
  struct IRQ {
    nall::Natural< 1> flag;    //bit 0
    nall::Natural< 1> enable;  //bit 1
  } irq;

  struct Registers {
    nall::Natural< 1> mapping;             //bit  2 (0 = ignore A15; 1 = use A15)
    nall::Natural< 1> psramEnableLo;       //bit  3
    nall::Natural< 1> psramEnableHi;       //bit  4
    nall::Natural< 2> psramMapping;        //bits 5-6
    nall::Natural< 1> romEnableLo;         //bit  7
    nall::Natural< 1> romEnableHi;         //bit  8
    nall::Natural< 1> exEnableLo;          //bit  9
    nall::Natural< 1> exEnableHi;          //bit 10
    nall::Natural< 1> exMapping;           //bit 11
    nall::Natural< 1> internallyWritable;  //bit 12 (1 = MCC allows writes to BS Memory Cassette)
    nall::Natural< 1> externallyWritable;  //bit 13 (1 = BS Memory Cassette allows writes to flash memory)
  } r, w;

  //bit 14 = commit
  //bit 15 = unknown (test register interface?)
};

extern MCC mcc;
