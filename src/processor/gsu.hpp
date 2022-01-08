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

#include "../../nall/primitives.hpp"

namespace Processor {

struct GSU {
  struct Register {
    uint16_t data = 0;
    bool modified = false;

    inline operator unsigned() const {
      return data;
    }

    inline uint16_t assign(unsigned value) {
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

    nall::BitField<16, 1> z   {&data};  //zero flag
    nall::BitField<16, 2> cy  {&data};  //carry flag
    nall::BitField<16, 3> s   {&data};  //sign flag
    nall::BitField<16, 4> ov  {&data};  //overflow flag
    nall::BitField<16, 5> g   {&data};  //go flag
    nall::BitField<16, 6> r   {&data};  //ROM r14 flag
    nall::BitField<16, 8> alt1{&data};  //alt1 instruction mode
    nall::BitField<16, 9> alt2{&data};  //alt2 instruction mode
    nall::BitField<16,10> il  {&data};  //immediate lower 8-bit flag
    nall::BitField<16,11> ih  {&data};  //immediate upper 8-bit flag
    nall::BitField<16,12> b   {&data};  //with flag
    nall::BitField<16,15> irq {&data};  //interrupt flag

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

    void reset() {
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

  virtual void step(unsigned clocks) = 0;

  virtual void stop() = 0;
  virtual uint8_t color(uint8_t source) = 0;
  virtual void plot(uint8_t x, uint8_t y) = 0;
  virtual uint8_t rpix(uint8_t x, uint8_t y) = 0;

  virtual uint8_t pipe() = 0;
  virtual void syncROMBuffer() = 0;
  virtual uint8_t readROMBuffer() = 0;
  virtual void syncRAMBuffer() = 0;
  virtual uint8_t readRAMBuffer(uint16_t addr) = 0;
  virtual void writeRAMBuffer(uint16_t addr, uint8_t data) = 0;
  virtual void flushCache() = 0;

  virtual uint8_t read(unsigned addr, uint8_t data = 0x00) = 0;
  virtual void write(unsigned addr, uint8_t data) = 0;

  //gsu.cpp
  void power();

  //instructions.cpp
  void instructionADD_ADC(unsigned n);
  void instructionALT1();
  void instructionALT2();
  void instructionALT3();
  void instructionAND_BIC(unsigned n);
  void instructionASR_DIV2();
  void instructionBranch(bool c);
  void instructionCACHE();
  void instructionCOLOR_CMODE();
  void instructionDEC(unsigned n);
  void instructionFMULT_LMULT();
  void instructionFROM_MOVES(unsigned n);
  void instructionGETB();
  void instructionGETC_RAMB_ROMB();
  void instructionHIB();
  void instructionIBT_LMS_SMS(unsigned n);
  void instructionINC(unsigned n);
  void instructionIWT_LM_SM(unsigned n);
  void instructionJMP_LJMP(unsigned n);
  void instructionLINK(unsigned n);
  void instructionLoad(unsigned n);
  void instructionLOB();
  void instructionLOOP();
  void instructionLSR();
  void instructionMERGE();
  void instructionMULT_UMULT(unsigned n);
  void instructionNOP();
  void instructionNOT();
  void instructionOR_XOR(unsigned n);
  void instructionPLOT_RPIX();
  void instructionROL();
  void instructionROR();
  void instructionSBK();
  void instructionSEX();
  void instructionStore(unsigned n);
  void instructionSTOP();
  void instructionSUB_SBC_CMP(unsigned n);
  void instructionSWAP();
  void instructionTO_MOVE(unsigned n);
  void instructionWITH(unsigned n);

  //switch.cpp
  void instruction(uint8_t opcode);

  //serialization.cpp
  void serialize(serializer&);

  //disassembler.cpp
  void disassembleOpcode(char* output);
  void disassembleALT0(char* output);
  void disassembleALT1(char* output);
  void disassembleALT2(char* output);
  void disassembleALT3(char* output);
};

}
