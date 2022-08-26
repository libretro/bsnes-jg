/*
 * bsnes-jg - Super Nintendo emulator
 *
 * Copyright (C) 2004-2020 byuu
 * Copyright (C) 2020-2022 Rupert Carmichael
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, specifically version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

namespace Processor {

struct GSU {
  struct Register {
    uint16_t data = 0;
    bool modified = false;

    inline operator unsigned() const;
    inline uint16_t assign(unsigned);
    inline uint16_t operator++();
    inline uint16_t operator--();
    inline unsigned operator++(int);
    inline unsigned operator--(int);
    inline uint16_t operator = (unsigned);
    inline uint16_t operator |= (unsigned);
    inline uint16_t operator ^= (unsigned);
    inline uint16_t operator &= (unsigned);
    inline uint16_t operator <<= (unsigned);
    inline uint16_t operator >>= (unsigned);
    inline uint16_t operator += (unsigned);
    inline uint16_t operator -= (unsigned);
    inline uint16_t operator *= (unsigned);
    inline uint16_t operator /= (unsigned);
    inline uint16_t operator %= (unsigned);
    inline uint16_t operator = (const Register&);

    Register() = default;
    Register(const Register&) = delete;
  };

  union SFR {
    uint16_t data = 0;
    struct {
      uint16_t : 1, z : 1, cy : 1, s : 1, ov : 1, g : 1, r : 1, : 1,
            alt1 : 1, alt2 : 1, il : 1, ih : 1, b : 1, : 2, irq : 1;
    } flag;
  };

  struct SCMR {
    unsigned ht;
    bool ron;
    bool ran;
    unsigned md;

    operator unsigned() const;
    SCMR& operator=(unsigned);
  };

  struct POR {
    bool obj;
    bool freezehigh;
    bool highnibble;
    bool dither;
    bool transparent;

    operator unsigned() const;
    POR& operator=(unsigned);
  };

  struct CFGR {
    bool irq;
    bool ms0;

    operator unsigned() const;
    CFGR& operator=(unsigned);
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

    unsigned romcl;     //clock ticks until romdr is valid
    uint8_t romdr;      //ROM buffer data register

    unsigned ramcl;     //clock ticks until ramdr is valid
    uint16_t ramar;     //RAM buffer address register
    uint8_t ramdr;      //RAM buffer data register

    unsigned sreg;
    unsigned dreg;

    Register& sr();     //source register (from)
    Register& dr();     //destination register (to)
    void reset();
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

  virtual ~GSU() = 0;
  virtual void step(unsigned) = 0;

  virtual void stop() = 0;
  virtual uint8_t color(uint8_t) = 0;
  virtual void plot(uint8_t, uint8_t) = 0;
  virtual uint8_t rpix(uint8_t, uint8_t) = 0;

  virtual uint8_t pipe() = 0;
  virtual void syncROMBuffer() = 0;
  virtual uint8_t readROMBuffer() = 0;
  virtual void syncRAMBuffer() = 0;
  virtual uint8_t readRAMBuffer(uint16_t) = 0;
  virtual void writeRAMBuffer(uint16_t, uint8_t) = 0;
  virtual void flushCache() = 0;

  virtual uint8_t read(unsigned, uint8_t = 0x00) = 0;
  virtual void write(unsigned, uint8_t) = 0;

  //gsu.cpp
  void power();

  //instructions.cpp
  void instructionADD_ADC(unsigned);
  void instructionALT1();
  void instructionALT2();
  void instructionALT3();
  void instructionAND_BIC(unsigned);
  void instructionASR_DIV2();
  void instructionBranch(bool);
  void instructionCACHE();
  void instructionCOLOR_CMODE();
  void instructionDEC(unsigned);
  void instructionFMULT_LMULT();
  void instructionFROM_MOVES(unsigned);
  void instructionGETB();
  void instructionGETC_RAMB_ROMB();
  void instructionHIB();
  void instructionIBT_LMS_SMS(unsigned);
  void instructionINC(unsigned n);
  void instructionIWT_LM_SM(unsigned);
  void instructionJMP_LJMP(unsigned);
  void instructionLINK(unsigned);
  void instructionLoad(unsigned);
  void instructionLOB();
  void instructionLOOP();
  void instructionLSR();
  void instructionMERGE();
  void instructionMULT_UMULT(unsigned);
  void instructionNOP();
  void instructionNOT();
  void instructionOR_XOR(unsigned);
  void instructionPLOT_RPIX();
  void instructionROL();
  void instructionROR();
  void instructionSBK();
  void instructionSEX();
  void instructionStore(unsigned);
  void instructionSTOP();
  void instructionSUB_SBC_CMP(unsigned);
  void instructionSWAP();
  void instructionTO_MOVE(unsigned);
  void instructionWITH(unsigned);

  //switch.cpp
  void instruction(uint8_t);

  //serialization.cpp
  void serialize(serializer&);

  //disassembler.cpp
  void disassembleOpcode(char*);
  void disassembleALT0(char*);
  void disassembleALT1(char*);
  void disassembleALT2(char*);
  void disassembleALT3(char*);
};

GSU::Register::operator unsigned() const {
  return data;
}

uint16_t GSU::Register::assign(unsigned value) {
  modified = true;
  return data = value;
}

uint16_t GSU::Register::operator++() {
  return assign(data + 1);
}

uint16_t GSU::Register::operator--() {
  return assign(data - 1);
}

unsigned GSU::Register::operator++(int) {
  unsigned r = data;
  assign(data + 1);
  return r;
}

unsigned GSU::Register::operator--(int) {
  unsigned r = data;
  assign(data - 1);
  return r;
}

uint16_t GSU::Register::operator = (unsigned i) {
  return assign(i);
}

uint16_t GSU::Register::operator |= (unsigned i) {
  return assign(data | i);
}

uint16_t GSU::Register::operator ^= (unsigned i) {
  return assign(data ^ i);
}

uint16_t GSU::Register::operator &= (unsigned i) {
  return assign(data & i);
}

uint16_t GSU::Register::operator <<= (unsigned i) {
  return assign(data << i);
}

uint16_t GSU::Register::operator >>= (unsigned i) {
  return assign(data >> i);
}

uint16_t GSU::Register::operator += (unsigned i) {
  return assign(data + i);
}

uint16_t GSU::Register::operator -= (unsigned i) {
  return assign(data - i);
}

uint16_t GSU::Register::operator *= (unsigned i) {
  return assign(data * i);
}

uint16_t GSU::Register::operator /= (unsigned i) {
  return assign(data / i);
}

uint16_t GSU::Register::operator %= (unsigned i) {
  return assign(data % i);
}

uint16_t GSU::Register::operator = (const Register& value) {
  return assign(value);
}

}
