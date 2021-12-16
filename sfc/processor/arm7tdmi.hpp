//ARMv4 (ARM7TDMI)

#pragma once

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

  virtual auto step(unsigned clocks) -> void = 0;
  virtual auto sleep() -> void = 0;
  virtual auto get(unsigned mode, uint32_t address) -> uint32_t = 0;
  virtual auto set(unsigned mode, uint32_t address, uint32_t word) -> void = 0;

  //arm7tdmi.cpp
  ARM7TDMI();
  auto power() -> void;

  //registers.cpp
  struct GPR;
  struct PSR;
  inline auto r(uint4) -> GPR&;
  inline auto cpsr() -> PSR&;
  inline auto spsr() -> PSR&;
  inline auto privileged() const -> bool;
  inline auto exception() const -> bool;

  //memory.cpp
  auto idle() -> void;
  auto read(unsigned mode, uint32_t address) -> uint32_t;
  auto load(unsigned mode, uint32_t address) -> uint32_t;
  auto write(unsigned mode, uint32_t address, uint32_t word) -> void;
  auto store(unsigned mode, uint32_t address, uint32_t word) -> void;

  //algorithms.cpp
  auto ADD(uint32_t, uint32_t, bool) -> uint32_t;
  auto ASR(uint32_t, uint8_t) -> uint32_t;
  auto BIT(uint32_t) -> uint32_t;
  auto LSL(uint32_t, uint8_t) -> uint32_t;
  auto LSR(uint32_t, uint8_t) -> uint32_t;
  auto MUL(uint32_t, uint32_t, uint32_t) -> uint32_t;
  auto ROR(uint32_t, uint8_t) -> uint32_t;
  auto RRX(uint32_t) -> uint32_t;
  auto SUB(uint32_t, uint32_t, bool) -> uint32_t;
  auto TST(uint4) -> bool;

  //instruction.cpp
  auto fetch() -> void;
  auto instruction() -> void;
  auto exception(unsigned mode, uint32_t address) -> void;
  auto armInitialize() -> void;
  auto thumbInitialize() -> void;

  //instructions-arm.cpp
  auto armALU(uint4 mode, uint4 target, uint4 source, uint32_t data) -> void;
  auto armMoveToStatus(uint4 field, uint1 source, uint32_t data) -> void;

  auto armInstructionBranch(int24, uint1) -> void;
  auto armInstructionBranchExchangeRegister(uint4) -> void;
  auto armInstructionDataImmediate(uint8_t, uint4, uint4, uint4, uint1, uint4) -> void;
  auto armInstructionDataImmediateShift(uint4, uint2, uint5, uint4, uint4, uint1, uint4) -> void;
  auto armInstructionDataRegisterShift(uint4, uint2, uint4, uint4, uint4, uint1, uint4) -> void;
  auto armInstructionLoadImmediate(uint8_t, uint1, uint4, uint4, uint1, uint1, uint1) -> void;
  auto armInstructionLoadRegister(uint4, uint1, uint4, uint4, uint1, uint1, uint1) -> void;
  auto armInstructionMemorySwap(uint4, uint4, uint4, uint1) -> void;
  auto armInstructionMoveHalfImmediate(uint8_t, uint4, uint4, uint1, uint1, uint1, uint1) -> void;
  auto armInstructionMoveHalfRegister(uint4, uint4, uint4, uint1, uint1, uint1, uint1) -> void;
  auto armInstructionMoveImmediateOffset(nall::Natural<12>, uint4, uint4, uint1, uint1, uint1, uint1, uint1) -> void;
  auto armInstructionMoveMultiple(uint16_t, uint4, uint1, uint1, uint1, uint1, uint1) -> void;
  auto armInstructionMoveRegisterOffset(uint4, uint2, uint5, uint4, uint4, uint1, uint1, uint1, uint1, uint1) -> void;
  auto armInstructionMoveToRegisterFromStatus(uint4, uint1) -> void;
  auto armInstructionMoveToStatusFromImmediate(uint8_t, uint4, uint4, uint1) -> void;
  auto armInstructionMoveToStatusFromRegister(uint4, uint4, uint1) -> void;
  auto armInstructionMultiply(uint4, uint4, uint4, uint4, uint1, uint1) -> void;
  auto armInstructionMultiplyLong(uint4, uint4, uint4, uint4, uint1, uint1, uint1) -> void;
  auto armInstructionSoftwareInterrupt(nall::Natural<24> immediate) -> void;
  auto armInstructionUndefined() -> void;

  //instructions-thumb.cpp
  auto thumbInstructionALU(uint3, uint3, uint4) -> void;
  auto thumbInstructionALUExtended(uint4, uint4, uint2) -> void;
  auto thumbInstructionAddRegister(uint8_t, uint3, uint1) -> void;
  auto thumbInstructionAdjustImmediate(uint3, uint3, uint3, uint1) -> void;
  auto thumbInstructionAdjustRegister(uint3, uint3, uint3, uint1) -> void;
  auto thumbInstructionAdjustStack(nall::Natural< 7>, uint1) -> void;
  auto thumbInstructionBranchExchange(uint4) -> void;
  auto thumbInstructionBranchFarPrefix(int11) -> void;
  auto thumbInstructionBranchFarSuffix(nall::Natural<11>) -> void;
  auto thumbInstructionBranchNear(int11) -> void;
  auto thumbInstructionBranchTest(int8_t, uint4) -> void;
  auto thumbInstructionImmediate(uint8_t, uint3, uint2) -> void;
  auto thumbInstructionLoadLiteral(uint8_t, uint3) -> void;
  auto thumbInstructionMoveByteImmediate(uint3, uint3, uint5, uint1) -> void;
  auto thumbInstructionMoveHalfImmediate(uint3, uint3, uint5, uint1) -> void;
  auto thumbInstructionMoveMultiple(uint8_t, uint3, uint1) -> void;
  auto thumbInstructionMoveRegisterOffset(uint3, uint3, uint3, uint3) -> void;
  auto thumbInstructionMoveStack(uint8_t, uint3, uint1) -> void;
  auto thumbInstructionMoveWordImmediate(uint3, uint3, uint5, uint1) -> void;
  auto thumbInstructionShiftImmediate(uint3, uint3, uint5, uint2) -> void;
  auto thumbInstructionSoftwareInterrupt(uint8_t) -> void;
  auto thumbInstructionStackMultiple(uint8_t, uint1, uint1) -> void;
  auto thumbInstructionUndefined() -> void;

  //serialization.cpp
  auto serialize(serializer&) -> void;

  //disassembler.cpp
  auto disassemble(maybe<uint32_t> pc = nothing, maybe<Boolean> thumb = nothing) -> string;
  auto disassembleRegisters() -> string;

  struct GPR {
    inline operator uint32_t() const { return data; }
    inline auto operator=(const GPR& value) -> GPR& { return operator=(value.data); }

    inline auto operator=(uint32_t value) -> GPR& {
      data = value;
      if(modify) modify();
      return *this;
    }

    uint32_t data;
    function<auto () -> void> modify;
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

    inline auto operator=(uint32_t data) -> PSR& {
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
    auto serialize(serializer&) -> void;

    uint5 m;    //mode
    Boolean t;  //thumb
    Boolean f;  //fiq
    Boolean i;  //irq
    Boolean v;  //overflow
    Boolean c;  //carry
    Boolean z;  //zero
    Boolean n;  //negative
  };

  struct Processor {
    //serialization.cpp
    auto serialize(serializer&) -> void;

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
    auto serialize(serializer&) -> void;

    struct Instruction {
      uint32_t address;
      uint32_t instruction;
      Boolean thumb;  //not used by fetch stage
    };

    uint1 reload = 1;
    uint1 nonsequential = 1;
    Instruction fetch;
    Instruction decode;
    Instruction execute;
  } pipeline;

  uint32_t opcode;
  Boolean carry;
  Boolean irq;

  function<auto (uint32_t opcode) -> void> armInstruction[4096];
  function<auto () -> void> thumbInstruction[65536];

  //disassembler.cpp
  auto armDisassembleBranch(int24, uint1) -> string;
  auto armDisassembleBranchExchangeRegister(uint4) -> string;
  auto armDisassembleDataImmediate(uint8_t, uint4, uint4, uint4, uint1, uint4) -> string;
  auto armDisassembleDataImmediateShift(uint4, uint2, uint5, uint4, uint4, uint1, uint4) -> string;
  auto armDisassembleDataRegisterShift(uint4, uint2, uint4, uint4, uint4, uint1, uint4) -> string;
  auto armDisassembleLoadImmediate(uint8_t, uint1, uint4, uint4, uint1, uint1, uint1) -> string;
  auto armDisassembleLoadRegister(uint4, uint1, uint4, uint4, uint1, uint1, uint1) -> string;
  auto armDisassembleMemorySwap(uint4, uint4, uint4, uint1) -> string;
  auto armDisassembleMoveHalfImmediate(uint8_t, uint4, uint4, uint1, uint1, uint1, uint1) -> string;
  auto armDisassembleMoveHalfRegister(uint4, uint4, uint4, uint1, uint1, uint1, uint1) -> string;
  auto armDisassembleMoveImmediateOffset(nall::Natural<12>, uint4, uint4, uint1, uint1, uint1, uint1, uint1) -> string;
  auto armDisassembleMoveMultiple(uint16_t, uint4, uint1, uint1, uint1, uint1, uint1) -> string;
  auto armDisassembleMoveRegisterOffset(uint4, uint2, uint5, uint4, uint4, uint1, uint1, uint1, uint1, uint1) -> string;
  auto armDisassembleMoveToRegisterFromStatus(uint4, uint1) -> string;
  auto armDisassembleMoveToStatusFromImmediate(uint8_t, uint4, uint4, uint1) -> string;
  auto armDisassembleMoveToStatusFromRegister(uint4, uint4, uint1) -> string;
  auto armDisassembleMultiply(uint4, uint4, uint4, uint4, uint1, uint1) -> string;
  auto armDisassembleMultiplyLong(uint4, uint4, uint4, uint4, uint1, uint1, uint1) -> string;
  auto armDisassembleSoftwareInterrupt(nall::Natural<24>) -> string;
  auto armDisassembleUndefined() -> string;

  auto thumbDisassembleALU(uint3, uint3, uint4) -> string;
  auto thumbDisassembleALUExtended(uint4, uint4, uint2) -> string;
  auto thumbDisassembleAddRegister(uint8_t, uint3, uint1) -> string;
  auto thumbDisassembleAdjustImmediate(uint3, uint3, uint3, uint1) -> string;
  auto thumbDisassembleAdjustRegister(uint3, uint3, uint3, uint1) -> string;
  auto thumbDisassembleAdjustStack(nall::Natural< 7>, uint1) -> string;
  auto thumbDisassembleBranchExchange(uint4) -> string;
  auto thumbDisassembleBranchFarPrefix(int11) -> string;
  auto thumbDisassembleBranchFarSuffix(nall::Natural<11>) -> string;
  auto thumbDisassembleBranchNear(int11) -> string;
  auto thumbDisassembleBranchTest(int8_t, uint4) -> string;
  auto thumbDisassembleImmediate(uint8_t, uint3, uint2) -> string;
  auto thumbDisassembleLoadLiteral(uint8_t, uint3) -> string;
  auto thumbDisassembleMoveByteImmediate(uint3, uint3, uint5, uint1) -> string;
  auto thumbDisassembleMoveHalfImmediate(uint3, uint3, uint5, uint1) -> string;
  auto thumbDisassembleMoveMultiple(uint8_t, uint3, uint1) -> string;
  auto thumbDisassembleMoveRegisterOffset(uint3, uint3, uint3, uint3) -> string;
  auto thumbDisassembleMoveStack(uint8_t, uint3, uint1) -> string;
  auto thumbDisassembleMoveWordImmediate(uint3, uint3, uint5, uint1) -> string;
  auto thumbDisassembleShiftImmediate(uint3, uint3, uint5, uint2) -> string;
  auto thumbDisassembleSoftwareInterrupt(uint8_t) -> string;
  auto thumbDisassembleStackMultiple(uint8_t, uint1, uint1) -> string;
  auto thumbDisassembleUndefined() -> string;

  function<auto (uint32_t opcode) -> string> armDisassemble[4096];
  function<auto () -> string> thumbDisassemble[65536];

  uint32_t _pc;
  string _c;
};

}
