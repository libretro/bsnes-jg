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

#include "endian.hpp"

#if defined(__clang__) || defined(__GNUC__)
  #define alwaysinline inline __attribute__((always_inline))
#else
  #define alwaysinline inline
#endif

namespace Processor {

struct SPC700 {
  virtual ~SPC700() = 0;
  virtual void idle() = 0;
  virtual uint8_t read(uint16_t) = 0;
  virtual void write(uint16_t, uint8_t) = 0;
  virtual bool synchronizing() const = 0;

  void power();

  alwaysinline uint8_t fetch();
  alwaysinline uint8_t load(uint8_t);
  alwaysinline void store(uint8_t, uint8_t);
  alwaysinline uint8_t pull();
  alwaysinline void push(uint8_t);

  void instruction();

  uint8_t algorithmADC(uint8_t, uint8_t);
  uint8_t algorithmAND(uint8_t, uint8_t);
  uint8_t algorithmASL(uint8_t);
  uint8_t algorithmCMP(uint8_t, uint8_t);
  uint8_t algorithmDEC(uint8_t);
  uint8_t algorithmEOR(uint8_t, uint8_t);
  uint8_t algorithmINC(uint8_t);
  uint8_t algorithmLD (uint8_t, uint8_t);
  uint8_t algorithmLSR(uint8_t);
  uint8_t algorithmOR (uint8_t, uint8_t);
  uint8_t algorithmROL(uint8_t);
  uint8_t algorithmROR(uint8_t);
  uint8_t algorithmSBC(uint8_t, uint8_t);
  uint16_t algorithmADW(uint16_t, uint16_t);
  uint16_t algorithmCPW(uint16_t, uint16_t);
  uint16_t algorithmLDW(uint16_t, uint16_t);
  uint16_t algorithmSBW(uint16_t, uint16_t);

  using fps = uint8_t (SPC700::*)(uint8_t);
  using fpb = uint8_t (SPC700::*)(uint8_t, uint8_t);
  using fpw = uint16_t (SPC700::*)(uint16_t, uint16_t);

  void instructionAbsoluteBitModify(uint8_t);
  void instructionAbsoluteBitSet(uint8_t, bool);
  void instructionAbsoluteRead(fpb, uint8_t&);
  void instructionAbsoluteModify(fps);
  void instructionAbsoluteWrite(uint8_t&);
  void instructionAbsoluteIndexedRead(fpb, uint8_t&);
  void instructionAbsoluteIndexedWrite(uint8_t&);
  void instructionBranch(bool);
  void instructionBranchBit(uint8_t, bool);
  void instructionBranchNotDirect();
  void instructionBranchNotDirectDecrement();
  void instructionBranchNotDirectIndexed(uint8_t&);
  void instructionBranchNotYDecrement();
  void instructionBreak();
  void instructionCallAbsolute();
  void instructionCallPage();
  void instructionCallTable(uint8_t);
  void instructionComplementCarry();
  void instructionDecimalAdjustAdd();
  void instructionDecimalAdjustSub();
  void instructionDirectRead(fpb, uint8_t&);
  void instructionDirectModify(fps);
  void instructionDirectWrite(uint8_t&);
  void instructionDirectDirectCompare(fpb);
  void instructionDirectDirectModify(fpb);
  void instructionDirectDirectWrite();
  void instructionDirectImmediateCompare(fpb);
  void instructionDirectImmediateModify(fpb);
  void instructionDirectImmediateWrite();
  void instructionDirectCompareWord(fpw);
  void instructionDirectReadWord(fpw);
  void instructionDirectModifyWord(int);
  void instructionDirectWriteWord();
  void instructionDirectIndexedRead(fpb, uint8_t&, uint8_t&);
  void instructionDirectIndexedModify(fps, uint8_t&);
  void instructionDirectIndexedWrite(uint8_t&, uint8_t&);
  void instructionDivide();
  void instructionExchangeNibble();
  void instructionFlagSet(bool&, bool);
  void instructionImmediateRead(fpb, uint8_t&);
  void instructionImpliedModify(fps, uint8_t&);
  void instructionIndexedIndirectRead(fpb, uint8_t&);
  void instructionIndexedIndirectWrite(uint8_t&, uint8_t&);
  void instructionIndirectIndexedRead(fpb, uint8_t&);
  void instructionIndirectIndexedWrite(uint8_t&, uint8_t&);
  void instructionIndirectXRead(fpb);
  void instructionIndirectXWrite(uint8_t&);
  void instructionIndirectXIncrementRead(uint8_t&);
  void instructionIndirectXIncrementWrite(uint8_t&);
  void instructionIndirectXCompareIndirectY(fpb);
  void instructionIndirectXWriteIndirectY(fpb);
  void instructionJumpAbsolute();
  void instructionJumpIndirectX();
  void instructionMultiply();
  void instructionNoOperation();
  void instructionOverflowClear();
  void instructionPull(uint8_t&);
  void instructionPullP();
  void instructionPush(uint8_t);
  void instructionReturnInterrupt();
  void instructionReturnSubroutine();
  void instructionStop();
  void instructionTestSetBitsAbsolute(bool);
  void instructionTransfer(uint8_t&, uint8_t&);
  void instructionWait();

  void serialize(serializer&);

  struct Flags {
    bool c = 0;  //carry
    bool z = 0;  //zero
    bool i = 0;  //interrupt disable
    bool h = 0;  //half-carry
    bool b = 0;  //break
    bool p = 0;  //page
    bool v = 0;  //overflow
    bool n = 0;  //negative

    inline operator unsigned() const;
    inline Flags& operator=(uint8_t);
  };

  struct Registers {
    union Pair {
      Pair() : w(0) {}
      uint16_t w;
      struct Byte { uint8_t order_lsb2(l, h); } byte;
    } pc, ya;
    uint8_t x = 0;
    uint8_t s = 0;
    Flags p;

    bool wait = 0;
    bool stop = 0;
  } r;
};

SPC700::Flags::operator unsigned() const {
  return c << 0 | z << 1 | i << 2 | h << 3 | b << 4 | p << 5 | v << 6 | n << 7;
}

SPC700::Flags& SPC700::Flags::operator=(uint8_t data) {
  c = data & 0x01;
  z = data & 0x02;
  i = data & 0x04;
  h = data & 0x08;
  b = data & 0x10;
  p = data & 0x20;
  v = data & 0x40;
  n = data & 0x80;
  return *this;
}

}

#undef alwaysinline
