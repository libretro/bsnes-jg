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

//ARMv4 (ARM7TDMI)

#pragma once

#include "../function.hpp"
#include "../serializer.hpp"

#include "../../nall/primitives.hpp"

namespace Processor {

struct ARM7TDMI {
  enum : unsigned {
    Nonsequential = 1 << 0,  //N cycle
    Sequential    = 1 << 1,  //S cycle
    Prefetch      = 1 << 2,  //instruction fetch
    Byte          = 1 << 3,  // 8-bit access
    Half          = 1 << 4,  //16-bit access
    Word          = 1 << 5,  //32-bit access
    Load          = 1 << 6,  //load operation
    Store         = 1 << 7,  //store operation
    Signed        = 1 << 8,  //sign-extend
  };

  virtual void step(unsigned clocks) = 0;
  virtual void sleep() = 0;
  virtual uint32_t get(unsigned mode, uint32_t address) = 0;
  virtual void set(unsigned mode, uint32_t address, uint32_t word) = 0;

  //arm7tdmi.cpp
  ARM7TDMI();
  void power();

  //registers.cpp
  struct GPR;
  struct PSR;
  inline GPR& r(nall::Natural< 4>);
  inline PSR& cpsr();
  inline PSR& spsr();
  inline bool privileged() const;
  inline bool exception() const;

  //memory.cpp
  void idle();
  uint32_t read(unsigned mode, uint32_t address);
  uint32_t load(unsigned mode, uint32_t address);
  void write(unsigned mode, uint32_t address, uint32_t word);
  void store(unsigned mode, uint32_t address, uint32_t word);

  //algorithms.cpp
  uint32_t ADD(uint32_t, uint32_t, bool);
  uint32_t ASR(uint32_t, uint8_t);
  uint32_t BIT(uint32_t);
  uint32_t LSL(uint32_t, uint8_t);
  uint32_t LSR(uint32_t, uint8_t);
  uint32_t MUL(uint32_t, uint32_t, uint32_t);
  uint32_t ROR(uint32_t, uint8_t);
  uint32_t RRX(uint32_t);
  uint32_t SUB(uint32_t, uint32_t, bool);
  bool TST(nall::Natural< 4>);

  //instruction.cpp
  void fetch();
  void instruction();
  void exception(unsigned mode, uint32_t address);
  void armInitialize();
  void thumbInitialize();

  //instructions-arm.cpp
  void armALU(nall::Natural< 4> mode, nall::Natural< 4> target, nall::Natural< 4> source, uint32_t data);
  void armMoveToStatus(nall::Natural< 4> field, nall::Natural< 1> source, uint32_t data);

  void armInstructionBranch(uint32_t, nall::Natural< 1>);
  void armInstructionBranchExchangeRegister(nall::Natural< 4>);
  void armInstructionDataImmediate(uint8_t, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 4>);
  void armInstructionDataImmediateShift(nall::Natural< 4>, nall::Natural< 2>, nall::Natural< 5>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 4>);
  void armInstructionDataRegisterShift(nall::Natural< 4>, nall::Natural< 2>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 4>);
  void armInstructionLoadImmediate(uint8_t, nall::Natural< 1>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>);
  void armInstructionLoadRegister(nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>);
  void armInstructionMemorySwap(nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>);
  void armInstructionMoveHalfImmediate(uint8_t, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>);
  void armInstructionMoveHalfRegister(nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>);
  void armInstructionMoveImmediateOffset(nall::Natural<12>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>);
  void armInstructionMoveMultiple(uint16_t, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>);
  void armInstructionMoveRegisterOffset(nall::Natural< 4>, nall::Natural< 2>, nall::Natural< 5>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>);
  void armInstructionMoveToRegisterFromStatus(nall::Natural< 4>, nall::Natural< 1>);
  void armInstructionMoveToStatusFromImmediate(uint8_t, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>);
  void armInstructionMoveToStatusFromRegister(nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>);
  void armInstructionMultiply(nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>);
  void armInstructionMultiplyLong(nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>);
  void armInstructionSoftwareInterrupt(nall::Natural<24> immediate);
  void armInstructionUndefined();

  //instructions-thumb.cpp
  void thumbInstructionALU(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 4>);
  void thumbInstructionALUExtended(nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 2>);
  void thumbInstructionAddRegister(uint8_t, nall::Natural< 3>, nall::Natural< 1>);
  void thumbInstructionAdjustImmediate(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 1>);
  void thumbInstructionAdjustRegister(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 1>);
  void thumbInstructionAdjustStack(nall::Natural< 7>, nall::Natural< 1>);
  void thumbInstructionBranchExchange(nall::Natural< 4>);
  void thumbInstructionBranchFarPrefix(uint16_t);
  void thumbInstructionBranchFarSuffix(nall::Natural<11>);
  void thumbInstructionBranchNear(uint16_t);
  void thumbInstructionBranchTest(int8_t, nall::Natural< 4>);
  void thumbInstructionImmediate(uint8_t, nall::Natural< 3>, nall::Natural< 2>);
  void thumbInstructionLoadLiteral(uint8_t, nall::Natural< 3>);
  void thumbInstructionMoveByteImmediate(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 5>, nall::Natural< 1>);
  void thumbInstructionMoveHalfImmediate(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 5>, nall::Natural< 1>);
  void thumbInstructionMoveMultiple(uint8_t, nall::Natural< 3>, nall::Natural< 1>);
  void thumbInstructionMoveRegisterOffset(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 3>);
  void thumbInstructionMoveStack(uint8_t, nall::Natural< 3>, nall::Natural< 1>);
  void thumbInstructionMoveWordImmediate(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 5>, nall::Natural< 1>);
  void thumbInstructionShiftImmediate(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 5>, nall::Natural< 2>);
  void thumbInstructionSoftwareInterrupt(uint8_t);
  void thumbInstructionStackMultiple(uint8_t, nall::Natural< 1>, nall::Natural< 1>);
  void thumbInstructionUndefined();

  //serialization.cpp
  void serialize(serializer&);

  struct GPR {
    inline operator uint32_t() const { return data; }
    inline GPR& operator=(const GPR& value) { return operator=(value.data); }

    inline GPR& operator=(uint32_t value) {
      data = value;
      if(modify) modify();
      return *this;
    }

    uint32_t data;
    bfunction<void ()> modify;
  };

  struct PSR {
    enum : unsigned {
      USR = 0x10,  //user
      FIQ = 0x11,  //fast interrupt
      IRQ = 0x12,  //interrupt
      SVC = 0x13,  //service
      ABT = 0x17,  //abort
      UND = 0x1b,  //undefined
      SYS = 0x1f,  //system
    };

    inline operator uint32_t() const {
      return m << 0 | t << 5 | f << 6 | i << 7 | v << 28 | c << 29 | z << 30 | n << 31;
    }

    inline PSR& operator=(uint32_t data) {
      m = data >>  0 & 31;
      t = data >>  5 & 1;
      f = data >>  6 & 1;
      i = data >>  7 & 1;
      v = data >> 28 & 1;
      c = data >> 29 & 1;
      z = data >> 30 & 1;
      n = data >> 31 & 1;
      return *this;
    }

    //serialization.cpp
    void serialize(serializer&);

    nall::Natural< 5> m;    //mode
    bool t;  //thumb
    bool f;  //fiq
    bool i;  //irq
    bool v;  //overflow
    bool c;  //carry
    bool z;  //zero
    bool n;  //negative
  };

  struct Processor {
    //serialization.cpp
    void serialize(serializer&);

    GPR r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15;
    PSR cpsr;

    struct FIQ {
      GPR r8, r9, r10, r11, r12, r13, r14;
      PSR spsr;
    } fiq;

    struct IRQ {
      GPR r13, r14;
      PSR spsr;
    } irq;

    struct SVC {
      GPR r13, r14;
      PSR spsr;
    } svc;

    struct ABT {
      GPR r13, r14;
      PSR spsr;
    } abt;

    struct UND {
      GPR r13, r14;
      PSR spsr;
    } und;
  } processor;

  struct Pipeline {
    //serialization.cpp
    void serialize(serializer&);

    struct Instruction {
      uint32_t address;
      uint32_t instruction;
      bool thumb;  //not used by fetch stage
    };

    nall::Natural< 1> reload = 1;
    nall::Natural< 1> nonsequential = 1;
    Instruction fetch;
    Instruction decode;
    Instruction execute;
  } pipeline;

  uint32_t opcode;
  bool carry;
  bool irq;

  bfunction<void (uint32_t opcode)> armInstruction[4096];
  bfunction<void ()> thumbInstruction[65536];

  uint32_t _pc;
};

}
