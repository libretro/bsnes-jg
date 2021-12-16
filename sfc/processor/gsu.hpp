#pragma once

namespace Processor {

struct GSU {
  struct Register {
    uint16_t data = 0;
    bool modified = false;

    inline operator unsigned() const {
      return data;
    }

    inline auto assign(unsigned value) -> uint16_t {
      modified = true;
      return data = value;
    }

    inline auto operator++() { return assign(data + 1); }
    inline auto operator--() { return assign(data - 1); }
    inline auto operator++(int) { unsigned r = data; assign(data + 1); return r; }
    inline auto operator--(int) { unsigned r = data; assign(data - 1); return r; }
    inline auto operator   = (unsigned i) { return assign(i); }
    inline auto operator  |= (unsigned i) { return assign(data | i); }
    inline auto operator  ^= (unsigned i) { return assign(data ^ i); }
    inline auto operator  &= (unsigned i) { return assign(data & i); }
    inline auto operator <<= (unsigned i) { return assign(data << i); }
    inline auto operator >>= (unsigned i) { return assign(data >> i); }
    inline auto operator  += (unsigned i) { return assign(data + i); }
    inline auto operator  -= (unsigned i) { return assign(data - i); }
    inline auto operator  *= (unsigned i) { return assign(data * i); }
    inline auto operator  /= (unsigned i) { return assign(data / i); }
    inline auto operator  %= (unsigned i) { return assign(data % i); }

    inline auto operator   = (const Register& value) { return assign(value); }

    Register() = default;
    Register(const Register&) = delete;
  };

  struct SFR {
    uint16_t data = 0;

    BitField<16, 1> z   {&data};  //zero flag
    BitField<16, 2> cy  {&data};  //carry flag
    BitField<16, 3> s   {&data};  //sign flag
    BitField<16, 4> ov  {&data};  //overflow flag
    BitField<16, 5> g   {&data};  //go flag
    BitField<16, 6> r   {&data};  //ROM r14 flag
    BitField<16, 8> alt1{&data};  //alt1 instruction mode
    BitField<16, 9> alt2{&data};  //alt2 instruction mode
    BitField<16,10> il  {&data};  //immediate lower 8-bit flag
    BitField<16,11> ih  {&data};  //immediate upper 8-bit flag
    BitField<16,12> b   {&data};  //with flag
    BitField<16,15> irq {&data};  //interrupt flag

    BitRange<16,8,9> alt{&data};  //composite instruction mode

    inline operator unsigned() const { return data & 0x9f7e; }
    inline auto& operator=(const unsigned value) { return data = value, *this; }
  };

  struct SCMR {
    unsigned ht;
    bool ron;
    bool ran;
    unsigned md;

    operator unsigned() const {
      return ((ht >> 1) << 5) | (ron << 4) | (ran << 3) | ((ht & 1) << 2) | (md);
    }

    auto& operator=(unsigned data) {
      ht  = (bool)(data & 0x20) << 1;
      ht |= (bool)(data & 0x04) << 0;
      ron = data & 0x10;
      ran = data & 0x08;
      md  = data & 0x03;
      return *this;
    }
  };

  struct POR {
    bool obj;
    bool freezehigh;
    bool highnibble;
    bool dither;
    bool transparent;

    operator unsigned() const {
      return (obj << 4) | (freezehigh << 3) | (highnibble << 2) | (dither << 1) | (transparent);
    }

    auto& operator=(unsigned data) {
      obj         = data & 0x10;
      freezehigh  = data & 0x08;
      highnibble  = data & 0x04;
      dither      = data & 0x02;
      transparent = data & 0x01;
      return *this;
    }
  };

  struct CFGR {
    bool irq;
    bool ms0;

    operator unsigned() const {
      return (irq << 7) | (ms0 << 5);
    }

    auto& operator=(unsigned data) {
      irq = data & 0x80;
      ms0 = data & 0x20;
      return *this;
    }
  };

  struct Registers {
    uint8_t pipeline;
    uint16_t ramaddr;

    Register r[16];     //general purpose registers
    SFR sfr;            //status flag register
    uint8_t pbr;        //program bank register
    uint8_t rombr;      //game pack ROM bank register
    bool rambr;         //game pack RAM bank register
    uint16_t cbr;       //cache base register
    uint8_t scbr;       //screen base register
    SCMR scmr;          //screen mode register
    uint8_t colr;       //color register
    POR por;            //plot option register
    bool bramr;         //back-up RAM register
    uint8_t vcr;        //version code register
    CFGR cfgr;          //config register
    bool clsr;          //clock select register

    unsigned romcl;         //clock ticks until romdr is valid
    uint8_t romdr;      //ROM buffer data register

    unsigned ramcl;         //clock ticks until ramdr is valid
    uint16_t ramar;     //RAM buffer address register
    uint8_t ramdr;      //RAM buffer data register

    unsigned sreg;
    unsigned dreg;
    auto& sr() { return r[sreg]; }  //source register (from)
    auto& dr() { return r[dreg]; }  //destination register (to)

    auto reset() -> void {
      sfr.b    = 0;
      sfr.alt1 = 0;
      sfr.alt2 = 0;

      sreg = 0;
      dreg = 0;
    }
  } regs;

  struct Cache {
    uint8_t buffer[512];
    bool valid[32];
  } cache;

  struct PixelCache {
    uint16_t offset;
    uint8_t bitpend;
    uint8_t data[8];
  } pixelcache[2];

  virtual auto step(unsigned clocks) -> void = 0;

  virtual auto stop() -> void = 0;
  virtual auto color(uint8_t source) -> uint8_t = 0;
  virtual auto plot(uint8_t x, uint8_t y) -> void = 0;
  virtual auto rpix(uint8_t x, uint8_t y) -> uint8_t = 0;

  virtual auto pipe() -> uint8_t = 0;
  virtual auto syncROMBuffer() -> void = 0;
  virtual auto readROMBuffer() -> uint8_t = 0;
  virtual auto syncRAMBuffer() -> void = 0;
  virtual auto readRAMBuffer(uint16_t addr) -> uint8_t = 0;
  virtual auto writeRAMBuffer(uint16_t addr, uint8_t data) -> void = 0;
  virtual auto flushCache() -> void = 0;

  virtual auto read(unsigned addr, uint8_t data = 0x00) -> uint8_t = 0;
  virtual auto write(unsigned addr, uint8_t data) -> void = 0;

  //gsu.cpp
  auto power() -> void;

  //instructions.cpp
  auto instructionADD_ADC(unsigned n) -> void;
  auto instructionALT1() -> void;
  auto instructionALT2() -> void;
  auto instructionALT3() -> void;
  auto instructionAND_BIC(unsigned n) -> void;
  auto instructionASR_DIV2() -> void;
  auto instructionBranch(bool c) -> void;
  auto instructionCACHE() -> void;
  auto instructionCOLOR_CMODE() -> void;
  auto instructionDEC(unsigned n) -> void;
  auto instructionFMULT_LMULT() -> void;
  auto instructionFROM_MOVES(unsigned n) -> void;
  auto instructionGETB() -> void;
  auto instructionGETC_RAMB_ROMB() -> void;
  auto instructionHIB() -> void;
  auto instructionIBT_LMS_SMS(unsigned n) -> void;
  auto instructionINC(unsigned n) -> void;
  auto instructionIWT_LM_SM(unsigned n) -> void;
  auto instructionJMP_LJMP(unsigned n) -> void;
  auto instructionLINK(unsigned n) -> void;
  auto instructionLoad(unsigned n) -> void;
  auto instructionLOB() -> void;
  auto instructionLOOP() -> void;
  auto instructionLSR() -> void;
  auto instructionMERGE() -> void;
  auto instructionMULT_UMULT(unsigned n) -> void;
  auto instructionNOP() -> void;
  auto instructionNOT() -> void;
  auto instructionOR_XOR(unsigned n) -> void;
  auto instructionPLOT_RPIX() -> void;
  auto instructionROL() -> void;
  auto instructionROR() -> void;
  auto instructionSBK() -> void;
  auto instructionSEX() -> void;
  auto instructionStore(unsigned n) -> void;
  auto instructionSTOP() -> void;
  auto instructionSUB_SBC_CMP(unsigned n) -> void;
  auto instructionSWAP() -> void;
  auto instructionTO_MOVE(unsigned n) -> void;
  auto instructionWITH(unsigned n) -> void;

  //switch.cpp
  auto instruction(uint8_t opcode) -> void;

  //serialization.cpp
  auto serialize(serializer&) -> void;

  //disassembler.cpp
  auto disassembleOpcode(char* output) -> void;
  auto disassembleALT0(char* output) -> void;
  auto disassembleALT1(char* output) -> void;
  auto disassembleALT2(char* output) -> void;
  auto disassembleALT3(char* output) -> void;
};

}
