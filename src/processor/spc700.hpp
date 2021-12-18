#pragma once

#include "endian.hpp"

namespace Processor {

struct SPC700 {
  virtual auto idle() -> void = 0;
  virtual auto read(uint16_t address) -> uint8_t = 0;
  virtual auto write(uint16_t address, uint8_t data) -> void = 0;
  virtual auto synchronizing() const -> bool = 0;

  virtual auto readDisassembler(uint16_t address) -> uint8_t { return 0; }

  //spc700.cpp
  auto power() -> void;

  //memory.cpp
  inline auto fetch() -> uint8_t;
  inline auto load(uint8_t address) -> uint8_t;
  inline auto store(uint8_t address, uint8_t data) -> void;
  inline auto pull() -> uint8_t;
  inline auto push(uint8_t data) -> void;

  //instruction.cpp
  auto instruction() -> void;

  //algorithms.cpp
  auto algorithmADC(uint8_t, uint8_t) -> uint8_t;
  auto algorithmAND(uint8_t, uint8_t) -> uint8_t;
  auto algorithmASL(uint8_t) -> uint8_t;
  auto algorithmCMP(uint8_t, uint8_t) -> uint8_t;
  auto algorithmDEC(uint8_t) -> uint8_t;
  auto algorithmEOR(uint8_t, uint8_t) -> uint8_t;
  auto algorithmINC(uint8_t) -> uint8_t;
  auto algorithmLD (uint8_t, uint8_t) -> uint8_t;
  auto algorithmLSR(uint8_t) -> uint8_t;
  auto algorithmOR (uint8_t, uint8_t) -> uint8_t;
  auto algorithmROL(uint8_t) -> uint8_t;
  auto algorithmROR(uint8_t) -> uint8_t;
  auto algorithmSBC(uint8_t, uint8_t) -> uint8_t;
  auto algorithmADW(uint16_t, uint16_t) -> uint16_t;
  auto algorithmCPW(uint16_t, uint16_t) -> uint16_t;
  auto algorithmLDW(uint16_t, uint16_t) -> uint16_t;
  auto algorithmSBW(uint16_t, uint16_t) -> uint16_t;

  //instructions.cpp
  using fps = auto (SPC700::*)(uint8_t) -> uint8_t;
  using fpb = auto (SPC700::*)(uint8_t, uint8_t) -> uint8_t;
  using fpw = auto (SPC700::*)(uint16_t, uint16_t) -> uint16_t;

  auto instructionAbsoluteBitModify(nall::Natural< 3>) -> void;
  auto instructionAbsoluteBitSet(nall::Natural< 3>, bool) -> void;
  auto instructionAbsoluteRead(fpb, uint8_t&) -> void;
  auto instructionAbsoluteModify(fps) -> void;
  auto instructionAbsoluteWrite(uint8_t&) -> void;
  auto instructionAbsoluteIndexedRead(fpb, uint8_t&) -> void;
  auto instructionAbsoluteIndexedWrite(uint8_t&) -> void;
  auto instructionBranch(bool) -> void;
  auto instructionBranchBit(nall::Natural< 3>, bool) -> void;
  auto instructionBranchNotDirect() -> void;
  auto instructionBranchNotDirectDecrement() -> void;
  auto instructionBranchNotDirectIndexed(uint8_t&) -> void;
  auto instructionBranchNotYDecrement() -> void;
  auto instructionBreak() -> void;
  auto instructionCallAbsolute() -> void;
  auto instructionCallPage() -> void;
  auto instructionCallTable(nall::Natural< 4>) -> void;
  auto instructionComplementCarry() -> void;
  auto instructionDecimalAdjustAdd() -> void;
  auto instructionDecimalAdjustSub() -> void;
  auto instructionDirectRead(fpb, uint8_t&) -> void;
  auto instructionDirectModify(fps) -> void;
  auto instructionDirectWrite(uint8_t&) -> void;
  auto instructionDirectDirectCompare(fpb) -> void;
  auto instructionDirectDirectModify(fpb) -> void;
  auto instructionDirectDirectWrite() -> void;
  auto instructionDirectImmediateCompare(fpb) -> void;
  auto instructionDirectImmediateModify(fpb) -> void;
  auto instructionDirectImmediateWrite() -> void;
  auto instructionDirectCompareWord(fpw) -> void;
  auto instructionDirectReadWord(fpw) -> void;
  auto instructionDirectModifyWord(int) -> void;
  auto instructionDirectWriteWord() -> void;
  auto instructionDirectIndexedRead(fpb, uint8_t&, uint8_t&) -> void;
  auto instructionDirectIndexedModify(fps, uint8_t&) -> void;
  auto instructionDirectIndexedWrite(uint8_t&, uint8_t&) -> void;
  auto instructionDivide() -> void;
  auto instructionExchangeNibble() -> void;
  auto instructionFlagSet(bool&, bool) -> void;
  auto instructionImmediateRead(fpb, uint8_t&) -> void;
  auto instructionImpliedModify(fps, uint8_t&) -> void;
  auto instructionIndexedIndirectRead(fpb, uint8_t&) -> void;
  auto instructionIndexedIndirectWrite(uint8_t&, uint8_t&) -> void;
  auto instructionIndirectIndexedRead(fpb, uint8_t&) -> void;
  auto instructionIndirectIndexedWrite(uint8_t&, uint8_t&) -> void;
  auto instructionIndirectXRead(fpb) -> void;
  auto instructionIndirectXWrite(uint8_t&) -> void;
  auto instructionIndirectXIncrementRead(uint8_t&) -> void;
  auto instructionIndirectXIncrementWrite(uint8_t&) -> void;
  auto instructionIndirectXCompareIndirectY(fpb) -> void;
  auto instructionIndirectXWriteIndirectY(fpb) -> void;
  auto instructionJumpAbsolute() -> void;
  auto instructionJumpIndirectX() -> void;
  auto instructionMultiply() -> void;
  auto instructionNoOperation() -> void;
  auto instructionOverflowClear() -> void;
  auto instructionPull(uint8_t&) -> void;
  auto instructionPullP() -> void;
  auto instructionPush(uint8_t) -> void;
  auto instructionReturnInterrupt() -> void;
  auto instructionReturnSubroutine() -> void;
  auto instructionStop() -> void;
  auto instructionTestSetBitsAbsolute(bool) -> void;
  auto instructionTransfer(uint8_t&, uint8_t&) -> void;
  auto instructionWait() -> void;

  //serialization.cpp
  auto serialize(serializer&) -> void;

  //disassembler.cpp
  auto disassemble(uint16_t address, bool p) -> string;

  struct Flags {
    bool c = 0;  //carry
    bool z = 0;  //zero
    bool i = 0;  //interrupt disable
    bool h = 0;  //half-carry
    bool b = 0;  //break
    bool p = 0;  //page
    bool v = 0;  //overflow
    bool n = 0;  //negative

    inline operator unsigned() const {
      return c << 0 | z << 1 | i << 2 | h << 3 | b << 4 | p << 5 | v << 6 | n << 7;
    }

    inline auto& operator=(uint8_t data) {
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

}
