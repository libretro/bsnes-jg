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

  void armInstructionBranch(nall::Integer<24>, nall::Natural< 1>);
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
  void thumbInstructionBranchFarPrefix(nall::Integer<11>);
  void thumbInstructionBranchFarSuffix(nall::Natural<11>);
  void thumbInstructionBranchNear(nall::Integer<11>);
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

  //disassembler.cpp
  /*nall::string disassemble(std::optional<uint32_t> pc = std::nullopt, std::optional<bool> thumb = std::nullopt);
  nall::string disassembleRegisters();*/

  struct GPR {
    inline operator uint32_t() const { return data; }
    inline GPR& operator=(const GPR& value) { return operator=(value.data); }

    inline GPR& operator=(uint32_t value) {
      data = value;
      if(modify) modify();
      return *this;
    }

    uint32_t data;
    nall::function<void ()> modify;
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

  nall::function<void (uint32_t opcode)> armInstruction[4096];
  nall::function<void ()> thumbInstruction[65536];

  //disassembler.cpp
  /*nall::string armDisassembleBranch(nall::Integer<24>, nall::Natural< 1>);
  nall::string armDisassembleBranchExchangeRegister(nall::Natural< 4>);
  nall::string armDisassembleDataImmediate(uint8_t, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 4>);
  nall::string armDisassembleDataImmediateShift(nall::Natural< 4>, nall::Natural< 2>, nall::Natural< 5>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 4>);
  nall::string armDisassembleDataRegisterShift(nall::Natural< 4>, nall::Natural< 2>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 4>);
  nall::string armDisassembleLoadImmediate(uint8_t, nall::Natural< 1>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>);
  nall::string armDisassembleLoadRegister(nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>);
  nall::string armDisassembleMemorySwap(nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>);
  nall::string armDisassembleMoveHalfImmediate(uint8_t, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>);
  nall::string armDisassembleMoveHalfRegister(nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>);
  nall::string armDisassembleMoveImmediateOffset(nall::Natural<12>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>);
  nall::string armDisassembleMoveMultiple(uint16_t, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>);
  nall::string armDisassembleMoveRegisterOffset(nall::Natural< 4>, nall::Natural< 2>, nall::Natural< 5>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>);
  nall::string armDisassembleMoveToRegisterFromStatus(nall::Natural< 4>, nall::Natural< 1>);
  nall::string armDisassembleMoveToStatusFromImmediate(uint8_t, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>);
  nall::string armDisassembleMoveToStatusFromRegister(nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>);
  nall::string armDisassembleMultiply(nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>);
  nall::string armDisassembleMultiplyLong(nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 1>, nall::Natural< 1>, nall::Natural< 1>);
  nall::string armDisassembleSoftwareInterrupt(nall::Natural<24>);
  nall::string armDisassembleUndefined();

  nall::string thumbDisassembleALU(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 4>);
  nall::string thumbDisassembleALUExtended(nall::Natural< 4>, nall::Natural< 4>, nall::Natural< 2>);
  nall::string thumbDisassembleAddRegister(uint8_t, nall::Natural< 3>, nall::Natural< 1>);
  nall::string thumbDisassembleAdjustImmediate(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 1>);
  nall::string thumbDisassembleAdjustRegister(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 1>);
  nall::string thumbDisassembleAdjustStack(nall::Natural< 7>, nall::Natural< 1>);
  nall::string thumbDisassembleBranchExchange(nall::Natural< 4>);
  nall::string thumbDisassembleBranchFarPrefix(nall::Integer<11>);
  nall::string thumbDisassembleBranchFarSuffix(nall::Natural<11>);
  nall::string thumbDisassembleBranchNear(nall::Integer<11>);
  nall::string thumbDisassembleBranchTest(int8_t, nall::Natural< 4>);
  nall::string thumbDisassembleImmediate(uint8_t, nall::Natural< 3>, nall::Natural< 2>);
  nall::string thumbDisassembleLoadLiteral(uint8_t, nall::Natural< 3>);
  nall::string thumbDisassembleMoveByteImmediate(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 5>, nall::Natural< 1>);
  nall::string thumbDisassembleMoveHalfImmediate(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 5>, nall::Natural< 1>);
  nall::string thumbDisassembleMoveMultiple(uint8_t, nall::Natural< 3>, nall::Natural< 1>);
  nall::string thumbDisassembleMoveRegisterOffset(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 3>);
  nall::string thumbDisassembleMoveStack(uint8_t, nall::Natural< 3>, nall::Natural< 1>);
  nall::string thumbDisassembleMoveWordImmediate(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 5>, nall::Natural< 1>);
  nall::string thumbDisassembleShiftImmediate(nall::Natural< 3>, nall::Natural< 3>, nall::Natural< 5>, nall::Natural< 2>);
  nall::string thumbDisassembleSoftwareInterrupt(uint8_t);
  nall::string thumbDisassembleStackMultiple(uint8_t, nall::Natural< 1>, nall::Natural< 1>);
  nall::string thumbDisassembleUndefined();

  nall::function<nall::string (uint32_t opcode)> armDisassemble[4096];
  nall::function<nall::string ()> thumbDisassemble[65536];*/

  uint32_t _pc;
  //nall::string _c;
};

}
