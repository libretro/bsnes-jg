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
  inline auto r(nall::Natural< 4>) -> GPR&;
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
  auto TST(nall::Natural< 4>) -> bool;

  //instruction.cpp
  auto fetch() -> void;
  auto instruction() -> void;
  auto exception(unsigned mode, uint32_t address) -> void;
  auto armInitialize() -> void;
  auto thumbInitialize() -> void;

  //instructions-arm.cpp
  auto armALU(nall::Natural< 4> mode, nall::Natural< 4> target, nall::Natural< 4> source, uint32_t data) -> void;
  auto armMoveToStatus(nall::Natural< 4> field, nall::Natural< 1> source, uint32_t data) -> void;

  auto armInstructionBranch(nall::Integer<24>, nall::Natural< 1>) -> void;
  auto armInstructionBranchExchangeRegister(nall::Natural< 4>) -> void;
  auto armInstructionDataImmediate(uint8_t, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 4>) -> void;
  auto armInstructionDataImmediateShift(nall::Natural< 4>, nall::Natural< 2>, nall::Natural< 5>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 4>) -> void;
  auto armInstructionDataRegisterShift(nall::Natural< 4>, nall::Natural< 2>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 4>) -> void;
  auto armInstructionLoadImmediate(uint8_t, nall::Natural< 1>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>) -> void;
  auto armInstructionLoadRegister(nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>) -> void;
  auto armInstructionMemorySwap(nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>) -> void;
  auto armInstructionMoveHalfImmediate(uint8_t, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>) -> void;
  auto armInstructionMoveHalfRegister(nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>) -> void;
  auto armInstructionMoveImmediateOffset(nall::Natural<12>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>) -> void;
  auto armInstructionMoveMultiple(uint16_t, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>) -> void;
  auto armInstructionMoveRegisterOffset(nall::Natural< 4>, nall::Natural< 2>, nall::Natural< 5>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>) -> void;
  auto armInstructionMoveToRegisterFromStatus(nall::Natural< 4>, nall::Natural< 1>) -> void;
  auto armInstructionMoveToStatusFromImmediate(uint8_t, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>) -> void;
  auto armInstructionMoveToStatusFromRegister(nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>) -> void;
  auto armInstructionMultiply(nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>) -> void;
  auto armInstructionMultiplyLong(nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>) -> void;
  auto armInstructionSoftwareInterrupt(nall::Natural<24> immediate) -> void;
  auto armInstructionUndefined() -> void;

  //instructions-thumb.cpp
  auto thumbInstructionALU(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 4>) -> void;
  auto thumbInstructionALUExtended(nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 2>) -> void;
  auto thumbInstructionAddRegister(uint8_t, nall::Natural< 3>, nall::Natural< 1>) -> void;
  auto thumbInstructionAdjustImmediate(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 1>) -> void;
  auto thumbInstructionAdjustRegister(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 1>) -> void;
  auto thumbInstructionAdjustStack(nall::Natural< 7>, nall::Natural< 1>) -> void;
  auto thumbInstructionBranchExchange(nall::Natural< 4>) -> void;
  auto thumbInstructionBranchFarPrefix(nall::Integer<11>) -> void;
  auto thumbInstructionBranchFarSuffix(nall::Natural<11>) -> void;
  auto thumbInstructionBranchNear(nall::Integer<11>) -> void;
  auto thumbInstructionBranchTest(int8_t, nall::Natural< 4>) -> void;
  auto thumbInstructionImmediate(uint8_t, nall::Natural< 3>, nall::Natural< 2>) -> void;
  auto thumbInstructionLoadLiteral(uint8_t, nall::Natural< 3>) -> void;
  auto thumbInstructionMoveByteImmediate(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 5>, nall::Natural< 1>) -> void;
  auto thumbInstructionMoveHalfImmediate(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 5>, nall::Natural< 1>) -> void;
  auto thumbInstructionMoveMultiple(uint8_t, nall::Natural< 3>, nall::Natural< 1>) -> void;
  auto thumbInstructionMoveRegisterOffset(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 3>) -> void;
  auto thumbInstructionMoveStack(uint8_t, nall::Natural< 3>, nall::Natural< 1>) -> void;
  auto thumbInstructionMoveWordImmediate(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 5>, nall::Natural< 1>) -> void;
  auto thumbInstructionShiftImmediate(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 5>, nall::Natural< 2>) -> void;
  auto thumbInstructionSoftwareInterrupt(uint8_t) -> void;
  auto thumbInstructionStackMultiple(uint8_t, nall::Natural< 1>, nall::Natural< 1>) -> void;
  auto thumbInstructionUndefined() -> void;

  //serialization.cpp
  auto serialize(serializer&) -> void;

  //disassembler.cpp
  /*auto disassemble(nall::maybe<uint32_t> pc = nall::nothing, nall::maybe<nall::Boolean> thumb = nall::nothing) -> nall::string;
  auto disassembleRegisters() -> nall::string;*/

  struct GPR {
    inline operator uint32_t() const { return data; }
    inline auto operator=(const GPR& value) -> GPR& { return operator=(value.data); }

    inline auto operator=(uint32_t value) -> GPR& {
      data = value;
      if(modify) modify();
      return *this;
    }

    uint32_t data;
    nall::function<auto () -> void> modify;
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

    nall::Natural< 5> m;    //mode
    nall::Boolean t;  //thumb
    nall::Boolean f;  //fiq
    nall::Boolean i;  //irq
    nall::Boolean v;  //overflow
    nall::Boolean c;  //carry
    nall::Boolean z;  //zero
    nall::Boolean n;  //negative
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
      nall::Boolean thumb;  //not used by fetch stage
    };

    nall::Natural< 1> reload = 1;
    nall::Natural< 1> nonsequential = 1;
    Instruction fetch;
    Instruction decode;
    Instruction execute;
  } pipeline;

  uint32_t opcode;
  nall::Boolean carry;
  nall::Boolean irq;

  nall::function<auto (uint32_t opcode) -> void> armInstruction[4096];
  nall::function<auto () -> void> thumbInstruction[65536];

  //disassembler.cpp
  /*auto armDisassembleBranch(nall::Integer<24>, nall::Natural< 1>) -> nall::string;
  auto armDisassembleBranchExchangeRegister(nall::Natural< 4>) -> nall::string;
  auto armDisassembleDataImmediate(uint8_t, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 4>) -> nall::string;
  auto armDisassembleDataImmediateShift(nall::Natural< 4>, nall::Natural< 2>, nall::Natural< 5>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 4>) -> nall::string;
  auto armDisassembleDataRegisterShift(nall::Natural< 4>, nall::Natural< 2>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 4>) -> nall::string;
  auto armDisassembleLoadImmediate(uint8_t, nall::Natural< 1>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>) -> nall::string;
  auto armDisassembleLoadRegister(nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>) -> nall::string;
  auto armDisassembleMemorySwap(nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>) -> nall::string;
  auto armDisassembleMoveHalfImmediate(uint8_t, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>) -> nall::string;
  auto armDisassembleMoveHalfRegister(nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>) -> nall::string;
  auto armDisassembleMoveImmediateOffset(nall::Natural<12>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>) -> nall::string;
  auto armDisassembleMoveMultiple(uint16_t, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>) -> nall::string;
  auto armDisassembleMoveRegisterOffset(nall::Natural< 4>, nall::Natural< 2>, nall::Natural< 5>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>) -> nall::string;
  auto armDisassembleMoveToRegisterFromStatus(nall::Natural< 4>, nall::Natural< 1>) -> nall::string;
  auto armDisassembleMoveToStatusFromImmediate(uint8_t, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>) -> nall::string;
  auto armDisassembleMoveToStatusFromRegister(nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>) -> nall::string;
  auto armDisassembleMultiply(nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>) -> nall::string;
  auto armDisassembleMultiplyLong(nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>) -> nall::string;
  auto armDisassembleSoftwareInterrupt(nall::Natural<24>) -> nall::string;
  auto armDisassembleUndefined() -> nall::string;

  auto thumbDisassembleALU(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 4>) -> nall::string;
  auto thumbDisassembleALUExtended(nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 2>) -> nall::string;
  auto thumbDisassembleAddRegister(uint8_t, nall::Natural< 3>, nall::Natural< 1>) -> nall::string;
  auto thumbDisassembleAdjustImmediate(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 1>) -> nall::string;
  auto thumbDisassembleAdjustRegister(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 1>) -> nall::string;
  auto thumbDisassembleAdjustStack(nall::Natural< 7>, nall::Natural< 1>) -> nall::string;
  auto thumbDisassembleBranchExchange(nall::Natural< 4>) -> nall::string;
  auto thumbDisassembleBranchFarPrefix(nall::Integer<11>) -> nall::string;
  auto thumbDisassembleBranchFarSuffix(nall::Natural<11>) -> nall::string;
  auto thumbDisassembleBranchNear(nall::Integer<11>) -> nall::string;
  auto thumbDisassembleBranchTest(int8_t, nall::Natural< 4>) -> nall::string;
  auto thumbDisassembleImmediate(uint8_t, nall::Natural< 3>, nall::Natural< 2>) -> nall::string;
  auto thumbDisassembleLoadLiteral(uint8_t, nall::Natural< 3>) -> nall::string;
  auto thumbDisassembleMoveByteImmediate(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 5>, nall::Natural< 1>) -> nall::string;
  auto thumbDisassembleMoveHalfImmediate(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 5>, nall::Natural< 1>) -> nall::string;
  auto thumbDisassembleMoveMultiple(uint8_t, nall::Natural< 3>, nall::Natural< 1>) -> nall::string;
  auto thumbDisassembleMoveRegisterOffset(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 3>) -> nall::string;
  auto thumbDisassembleMoveStack(uint8_t, nall::Natural< 3>, nall::Natural< 1>) -> nall::string;
  auto thumbDisassembleMoveWordImmediate(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 5>, nall::Natural< 1>) -> nall::string;
  auto thumbDisassembleShiftImmediate(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 5>, nall::Natural< 2>) -> nall::string;
  auto thumbDisassembleSoftwareInterrupt(uint8_t) -> nall::string;
  auto thumbDisassembleStackMultiple(uint8_t, nall::Natural< 1>, nall::Natural< 1>) -> nall::string;
  auto thumbDisassembleUndefined() -> nall::string;

  nall::function<auto (uint32_t opcode) -> nall::string> armDisassemble[4096];
  nall::function<auto () -> nall::string> thumbDisassemble[65536];*/

  uint32_t _pc;
  //nall::string _c;
};

}
