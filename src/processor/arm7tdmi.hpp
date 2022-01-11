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
  inline GPR& r(uint8_t);
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
  bool TST(uint8_t);

  //instruction.cpp
  void fetch();
  void instruction();
  void exception(unsigned mode, uint32_t address);
  void armInitialize();
  void thumbInitialize();

  //instructions-arm.cpp
  void armALU(uint8_t mode, uint8_t target, uint8_t source, uint32_t data);
  void armMoveToStatus(uint8_t field, uint8_t source, uint32_t data);

  void armInstructionBranch(uint32_t, uint8_t);
  void armInstructionBranchExchangeRegister(uint8_t);
  void armInstructionDataImmediate(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
  void armInstructionDataImmediateShift(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
  void armInstructionDataRegisterShift(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
  void armInstructionLoadImmediate(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
  void armInstructionLoadRegister(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
  void armInstructionMemorySwap(uint8_t, uint8_t, uint8_t, uint8_t);
  void armInstructionMoveHalfImmediate(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
  void armInstructionMoveHalfRegister(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
  void armInstructionMoveImmediateOffset(uint16_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
  void armInstructionMoveMultiple(uint16_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
  void armInstructionMoveRegisterOffset(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
  void armInstructionMoveToRegisterFromStatus(uint8_t, uint8_t);
  void armInstructionMoveToStatusFromImmediate(uint8_t, uint8_t, uint8_t, uint8_t);
  void armInstructionMoveToStatusFromRegister(uint8_t, uint8_t, uint8_t);
  void armInstructionMultiply(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
  void armInstructionMultiplyLong(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
  void armInstructionSoftwareInterrupt(uint32_t immediate);
  void armInstructionUndefined();

  //instructions-thumb.cpp
  void thumbInstructionALU(uint8_t, uint8_t, uint8_t);
  void thumbInstructionALUExtended(uint8_t, uint8_t, uint8_t);
  void thumbInstructionAddRegister(uint8_t, uint8_t, uint8_t);
  void thumbInstructionAdjustImmediate(uint8_t, uint8_t, uint8_t, uint8_t);
  void thumbInstructionAdjustRegister(uint8_t, uint8_t, uint8_t, uint8_t);
  void thumbInstructionAdjustStack(uint8_t, uint8_t);
  void thumbInstructionBranchExchange(uint8_t);
  void thumbInstructionBranchFarPrefix(uint16_t);
  void thumbInstructionBranchFarSuffix(uint16_t);
  void thumbInstructionBranchNear(uint16_t);
  void thumbInstructionBranchTest(int8_t, uint8_t);
  void thumbInstructionImmediate(uint8_t, uint8_t, uint8_t);
  void thumbInstructionLoadLiteral(uint8_t, uint8_t);
  void thumbInstructionMoveByteImmediate(uint8_t, uint8_t, uint8_t, uint8_t);
  void thumbInstructionMoveHalfImmediate(uint8_t, uint8_t, uint8_t, uint8_t);
  void thumbInstructionMoveMultiple(uint8_t, uint8_t, uint8_t);
  void thumbInstructionMoveRegisterOffset(uint8_t, uint8_t, uint8_t, uint8_t);
  void thumbInstructionMoveStack(uint8_t, uint8_t, uint8_t);
  void thumbInstructionMoveWordImmediate(uint8_t, uint8_t, uint8_t, uint8_t);
  void thumbInstructionShiftImmediate(uint8_t, uint8_t, uint8_t, uint8_t);
  void thumbInstructionSoftwareInterrupt(uint8_t);
  void thumbInstructionStackMultiple(uint8_t, uint8_t, uint8_t);
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

    uint8_t m;    //mode
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

    uint8_t reload = 1;
    uint8_t nonsequential = 1;
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
