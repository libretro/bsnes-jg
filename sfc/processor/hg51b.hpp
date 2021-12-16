#pragma once

//Hitachi HG51B S169

namespace Processor {

struct HG51B {
  //instruction.cpp
  HG51B();

  //hg51b.cpp
  virtual auto step(unsigned clocks) -> void;
  virtual auto isROM(unsigned address) -> bool = 0;
  virtual auto isRAM(unsigned address) -> bool = 0;
  virtual auto read(unsigned address) -> uint8_t = 0;
  virtual auto write(unsigned address, uint8_t data) -> void = 0;
  virtual auto lock() -> void;
  virtual auto halt() -> void;
  auto wait(nall::Natural<24> address) -> unsigned;
  auto main() -> void;
  auto execute() -> void;
  auto advance() -> void;
  auto suspend() -> void;
  auto cache() -> bool;
  auto dma() -> void;
  auto running() const -> bool;
  auto busy() const -> bool;

  auto power() -> void;

  //instructions.cpp
  auto push() -> void;
  auto pull() -> void;

  auto algorithmADD(nall::Natural<24> x, nall::Natural<24> y) -> nall::Natural<24>;
  auto algorithmAND(nall::Natural<24> x, nall::Natural<24> y) -> nall::Natural<24>;
  auto algorithmASR(nall::Natural<24> a, nall::Natural< 5> s) -> nall::Natural<24>;
  auto algorithmMUL(int24 x, int24 y) -> nall::Natural<48>;
  auto algorithmOR(nall::Natural<24> x, nall::Natural<24> y) -> nall::Natural<24>;
  auto algorithmROR(nall::Natural<24> a, nall::Natural< 5> s) -> nall::Natural<24>;
  auto algorithmSHL(nall::Natural<24> a, nall::Natural< 5> s) -> nall::Natural<24>;
  auto algorithmSHR(nall::Natural<24> a, nall::Natural< 5> s) -> nall::Natural<24>;
  auto algorithmSUB(nall::Natural<24> x, nall::Natural<24> y) -> nall::Natural<24>;
  auto algorithmSX(nall::Natural<24> x) -> nall::Natural<24>;
  auto algorithmXNOR(nall::Natural<24> x, nall::Natural<24> y) -> nall::Natural<24>;
  auto algorithmXOR(nall::Natural<24> x, nall::Natural<24> y) -> nall::Natural<24>;

  auto instructionADD(nall::Natural< 7> reg, nall::Natural< 5> shift) -> void;
  auto instructionADD(uint8_t imm, nall::Natural< 5> shift) -> void;
  auto instructionAND(nall::Natural< 7> reg, nall::Natural< 5> shift) -> void;
  auto instructionAND(uint8_t imm, nall::Natural< 5> shift) -> void;
  auto instructionASR(nall::Natural< 7> reg) -> void;
  auto instructionASR(nall::Natural< 5> imm) -> void;
  auto instructionCLEAR() -> void;
  auto instructionCMP(nall::Natural< 7> reg, nall::Natural< 5> shift) -> void;
  auto instructionCMP(uint8_t imm, nall::Natural< 5> shift) -> void;
  auto instructionCMPR(nall::Natural< 7> reg, nall::Natural< 5> shift) -> void;
  auto instructionCMPR(uint8_t imm, nall::Natural< 5> shift) -> void;
  auto instructionHALT() -> void;
  auto instructionINC(nall::Natural<24>& reg) -> void;
  auto instructionJMP(uint8_t data, uint1 far, const uint1& take) -> void;
  auto instructionJSR(uint8_t data, uint1 far, const uint1& take) -> void;
  auto instructionLD(nall::Natural<24>& out, nall::Natural< 7> reg) -> void;
  auto instructionLD(nall::Natural<15>& out, uint4 reg) -> void;
  auto instructionLD(nall::Natural<24>& out, uint8_t imm) -> void;
  auto instructionLD(nall::Natural<15>& out, uint8_t imm) -> void;
  auto instructionLDL(nall::Natural<15>& out, uint8_t imm) -> void;
  auto instructionLDH(nall::Natural<15>& out, nall::Natural< 7> imm) -> void;
  auto instructionMUL(nall::Natural< 7> reg) -> void;
  auto instructionMUL(uint8_t imm) -> void;
  auto instructionNOP() -> void;
  auto instructionOR(nall::Natural< 7> reg, nall::Natural< 5> shift) -> void;
  auto instructionOR(uint8_t imm, nall::Natural< 5> shift) -> void;
  auto instructionRDRAM(uint2 byte, nall::Natural<24>& a) -> void;
  auto instructionRDRAM(uint2 byte, uint8_t imm) -> void;
  auto instructionRDROM(nall::Natural<24>& reg) -> void;
  auto instructionRDROM(nall::Natural<10> imm) -> void;
  auto instructionROR(nall::Natural< 7> reg) -> void;
  auto instructionROR(nall::Natural< 5> imm) -> void;
  auto instructionRTS() -> void;
  auto instructionSHL(nall::Natural< 7> reg) -> void;
  auto instructionSHL(nall::Natural< 5> imm) -> void;
  auto instructionSHR(nall::Natural< 7> reg) -> void;
  auto instructionSHR(nall::Natural< 5> imm) -> void;
  auto instructionSKIP(uint1 take, const uint1& flag) -> void;
  auto instructionST(nall::Natural< 7> reg, nall::Natural<24>& in) -> void;
  auto instructionSUB(nall::Natural< 7> reg, nall::Natural< 5> shift) -> void;
  auto instructionSUB(uint8_t imm, nall::Natural< 5> shift) -> void;
  auto instructionSUBR(nall::Natural< 7> reg, nall::Natural< 5> shift) -> void;
  auto instructionSUBR(uint8_t imm, nall::Natural< 5> shift) -> void;
  auto instructionSWAP(nall::Natural<24>& a, uint4 reg) -> void;
  auto instructionSXB() -> void;
  auto instructionSXW() -> void;
  auto instructionWAIT() -> void;
  auto instructionWRRAM(uint2 byte, nall::Natural<24>& a) -> void;
  auto instructionWRRAM(uint2 byte, uint8_t imm) -> void;
  auto instructionXNOR(nall::Natural< 7> reg, nall::Natural< 5> shift) -> void;
  auto instructionXNOR(uint8_t imm, nall::Natural< 5> shift) -> void;
  auto instructionXOR(nall::Natural< 7> reg, nall::Natural< 5> shift) -> void;
  auto instructionXOR(uint8_t imm, nall::Natural< 5> shift) -> void;

  //serialization.cpp
  auto serialize(serializer&) -> void;

  uint16_t programRAM[2][256];  //instruction cache
  nall::Natural<24> dataROM[1024];
  uint8_t dataRAM[3072];

  //registers.cpp
  auto readRegister(nall::Natural< 7> address) -> nall::Natural<24>;
  auto writeRegister(nall::Natural< 7> address, nall::Natural<24> data) -> void;

protected:
  struct Registers {
    nall::Natural<15> pb;    //program bank
    uint8_t  pc;  //program counter

    Boolean n;  //negative
    Boolean z;  //zero
    Boolean c;  //carry
    Boolean v;  //overflow
    Boolean i;  //interrupt

    nall::Natural<24> a;              //accumulator
    nall::Natural<15> p;              //page register
    nall::Natural<48> mul; //multiplier
    nall::Natural<24> mdr;            //bus memory data register
    nall::Natural<24> rom;            //data ROM data buffer
    nall::Natural<24> ram;            //data RAM data buffer
    nall::Natural<24> mar;            //bus memory address register
    nall::Natural<24> dpr;            //data RAM address pointer
    nall::Natural<24> gpr[16];        //general purpose registers
  } r;

  struct IO {
    uint1 lock;
    uint1 halt = 1;
    uint1 irq;      //0 = enable, 1 = disable
    uint1 rom = 1;  //0 = 2 ROMs, 1 = 1 ROM
    uint8_t vector[32];

    struct Wait {
      uint3 rom = 3;
      uint3 ram = 3;
    } wait;

    struct Suspend {
      uint1 enable;
      uint8_t duration;
    } suspend;

    struct Cache {
      uint1  enable;
      uint1  page;
      uint1  lock[2];
      nall::Natural<24> address[2];  //cache address is in bytes; so 24-bit
      nall::Natural<24> base;        //base address is also in bytes
      nall::Natural<15> pb;
      uint8_t  pc;
    } cache;

    struct DMA {
      uint1  enable;
      nall::Natural<24> source;
      nall::Natural<24> target;
      uint16_t length;
    } dma;

    struct Bus {
      uint1  enable;
      uint1  reading;
      uint1  writing;
      uint4  pending;
      nall::Natural<24> address;
    } bus;
  } io;

  nall::Natural<23> stack[8];
  function<void ()> instructionTable[65536];
};

}
