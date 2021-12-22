//WDC 65C816 CPU core
//* Ricoh 5A22
//* Nintendo SA-1

#pragma once

namespace Processor {

struct WDC65816 {
  virtual auto idle() -> void = 0;
  virtual auto idleBranch() -> void {}
  virtual auto idleJump() -> void {}
  virtual auto read(unsigned addr) -> uint8_t = 0;
  virtual auto write(unsigned addr, uint8_t data) -> void = 0;
  virtual auto lastCycle() -> void = 0;
  virtual auto interruptPending() const -> bool = 0;
  virtual auto interrupt() -> void;
  virtual auto synchronizing() const -> bool = 0;

  virtual auto readDisassembler(unsigned addr) -> uint8_t { return 0; }

  inline auto irq() const -> bool { return r.irq; }
  inline auto irq(bool line) -> void { r.irq = line; }

  using r8 = uint8_t;

  union r16 {
    inline r16() : w(0) {}
    inline r16(unsigned data) : w(data) {}
    inline auto& operator=(unsigned data) { w = data; return *this; }

    uint16_t w;
    struct { uint8_t order_lsb2(l, h); };
  };

  union r24 {
    inline r24() : d(0) {}
    inline r24(unsigned data) : d(data) {}
    inline auto& operator=(unsigned data) { d = data; return *this; }

    nall::Natural<24> d;
    struct { uint16_t order_lsb2(w, x); };
    struct {  uint8_t order_lsb4(l, h, b, y); };
  };

  //wdc65816.cpp
  auto power() -> void;

  //memory.cpp
  inline auto idleIRQ() -> void;
  inline auto idle2() -> void;
  inline auto idle4(uint16_t x, uint16_t y) -> void;
  inline auto idle6(uint16_t address) -> void;
  inline auto fetch() -> uint8_t;
  inline auto pull() -> uint8_t;
               auto push(uint8_t data) -> void;
  inline auto pullN() -> uint8_t;
  inline auto pushN(uint8_t data) -> void;
  inline auto readDirect(unsigned address) -> uint8_t;
  inline auto writeDirect(unsigned address, uint8_t data) -> void;
  inline auto readDirectN(unsigned address) -> uint8_t;
  inline auto readBank(unsigned address) -> uint8_t;
  inline auto writeBank(unsigned address, uint8_t data) -> void;
  inline auto readLong(unsigned address) -> uint8_t;
  inline auto writeLong(unsigned address, uint8_t data) -> void;
  inline auto readStack(unsigned address) -> uint8_t;
  inline auto writeStack(unsigned address, uint8_t data) -> void;

  //algorithms.cpp
  using  alu8 = auto (WDC65816::*)( uint8_t) ->  uint8_t;
  using alu16 = auto (WDC65816::*)(uint16_t) -> uint16_t;

  auto algorithmADC8(uint8_t) -> uint8_t;
  auto algorithmADC16(uint16_t) -> uint16_t;
  auto algorithmAND8(uint8_t) -> uint8_t;
  auto algorithmAND16(uint16_t) -> uint16_t;
  auto algorithmASL8(uint8_t) -> uint8_t;
  auto algorithmASL16(uint16_t) -> uint16_t;
  auto algorithmBIT8(uint8_t) -> uint8_t;
  auto algorithmBIT16(uint16_t) -> uint16_t;
  auto algorithmCMP8(uint8_t) -> uint8_t;
  auto algorithmCMP16(uint16_t) -> uint16_t;
  auto algorithmCPX8(uint8_t) -> uint8_t;
  auto algorithmCPX16(uint16_t) -> uint16_t;
  auto algorithmCPY8(uint8_t) -> uint8_t;
  auto algorithmCPY16(uint16_t) -> uint16_t;
  auto algorithmDEC8(uint8_t) -> uint8_t;
  auto algorithmDEC16(uint16_t) -> uint16_t;
  auto algorithmEOR8(uint8_t) -> uint8_t;
  auto algorithmEOR16(uint16_t) -> uint16_t;
  auto algorithmINC8(uint8_t) -> uint8_t;
  auto algorithmINC16(uint16_t) -> uint16_t;
  auto algorithmLDA8(uint8_t) -> uint8_t;
  auto algorithmLDA16(uint16_t) -> uint16_t;
  auto algorithmLDX8(uint8_t) -> uint8_t;
  auto algorithmLDX16(uint16_t) -> uint16_t;
  auto algorithmLDY8(uint8_t) -> uint8_t;
  auto algorithmLDY16(uint16_t) -> uint16_t;
  auto algorithmLSR8(uint8_t) -> uint8_t;
  auto algorithmLSR16(uint16_t) -> uint16_t;
  auto algorithmORA8(uint8_t) -> uint8_t;
  auto algorithmORA16(uint16_t) -> uint16_t;
  auto algorithmROL8(uint8_t) -> uint8_t;
  auto algorithmROL16(uint16_t) -> uint16_t;
  auto algorithmROR8(uint8_t) -> uint8_t;
  auto algorithmROR16(uint16_t) -> uint16_t;
  auto algorithmSBC8(uint8_t) -> uint8_t;
  auto algorithmSBC16(uint16_t) -> uint16_t;
  auto algorithmTRB8(uint8_t) -> uint8_t;
  auto algorithmTRB16(uint16_t) -> uint16_t;
  auto algorithmTSB8(uint8_t) -> uint8_t;
  auto algorithmTSB16(uint16_t) -> uint16_t;

  //instructions-read.cpp
  auto instructionImmediateRead8(alu8) -> void;
  auto instructionImmediateRead16(alu16) -> void;
  auto instructionBankRead8(alu8) -> void;
  auto instructionBankRead16(alu16) -> void;
  auto instructionBankRead8(alu8, r16) -> void;
  auto instructionBankRead16(alu16, r16) -> void;
  auto instructionLongRead8(alu8, r16 = {}) -> void;
  auto instructionLongRead16(alu16, r16 = {}) -> void;
  auto instructionDirectRead8(alu8) -> void;
  auto instructionDirectRead16(alu16) -> void;
  auto instructionDirectRead8(alu8, r16) -> void;
  auto instructionDirectRead16(alu16, r16) -> void;
  auto instructionIndirectRead8(alu8) -> void;
  auto instructionIndirectRead16(alu16) -> void;
  auto instructionIndexedIndirectRead8(alu8) -> void;
  auto instructionIndexedIndirectRead16(alu16) -> void;
  auto instructionIndirectIndexedRead8(alu8) -> void;
  auto instructionIndirectIndexedRead16(alu16) -> void;
  auto instructionIndirectLongRead8(alu8, r16 = {}) -> void;
  auto instructionIndirectLongRead16(alu16, r16 = {}) -> void;
  auto instructionStackRead8(alu8) -> void;
  auto instructionStackRead16(alu16) -> void;
  auto instructionIndirectStackRead8(alu8) -> void;
  auto instructionIndirectStackRead16(alu16) -> void;

  //instructions-write.cpp
  auto instructionBankWrite8(r16) -> void;
  auto instructionBankWrite16(r16) -> void;
  auto instructionBankWrite8(r16, r16) -> void;
  auto instructionBankWrite16(r16, r16) -> void;
  auto instructionLongWrite8(r16 = {}) -> void;
  auto instructionLongWrite16(r16 = {}) -> void;
  auto instructionDirectWrite8(r16) -> void;
  auto instructionDirectWrite16(r16) -> void;
  auto instructionDirectWrite8(r16, r16) -> void;
  auto instructionDirectWrite16(r16, r16) -> void;
  auto instructionIndirectWrite8() -> void;
  auto instructionIndirectWrite16() -> void;
  auto instructionIndexedIndirectWrite8() -> void;
  auto instructionIndexedIndirectWrite16() -> void;
  auto instructionIndirectIndexedWrite8() -> void;
  auto instructionIndirectIndexedWrite16() -> void;
  auto instructionIndirectLongWrite8(r16 = {}) -> void;
  auto instructionIndirectLongWrite16(r16 = {}) -> void;
  auto instructionStackWrite8() -> void;
  auto instructionStackWrite16() -> void;
  auto instructionIndirectStackWrite8() -> void;
  auto instructionIndirectStackWrite16() -> void;

  //instructions-modify.cpp
  auto instructionImpliedModify8(alu8, r16&) -> void;
  auto instructionImpliedModify16(alu16, r16&) -> void;
  auto instructionBankModify8(alu8) -> void;
  auto instructionBankModify16(alu16) -> void;
  auto instructionBankIndexedModify8(alu8) -> void;
  auto instructionBankIndexedModify16(alu16) -> void;
  auto instructionDirectModify8(alu8) -> void;
  auto instructionDirectModify16(alu16) -> void;
  auto instructionDirectIndexedModify8(alu8) -> void;
  auto instructionDirectIndexedModify16(alu16) -> void;

  //instructions-pc.cpp
  auto instructionBranch(bool = 1) -> void;
  auto instructionBranchLong() -> void;
  auto instructionJumpShort() -> void;
  auto instructionJumpLong() -> void;
  auto instructionJumpIndirect() -> void;
  auto instructionJumpIndexedIndirect() -> void;
  auto instructionJumpIndirectLong() -> void;
  auto instructionCallShort() -> void;
  auto instructionCallLong() -> void;
  auto instructionCallIndexedIndirect() -> void;
  auto instructionReturnInterrupt() -> void;
  auto instructionReturnShort() -> void;
  auto instructionReturnLong() -> void;

  //instructions-misc.cpp
  auto instructionBitImmediate8() -> void;
  auto instructionBitImmediate16() -> void;
  auto instructionNoOperation() -> void;
  auto instructionPrefix() -> void;
  auto instructionExchangeBA() -> void;
  auto instructionBlockMove8(int) -> void;
  auto instructionBlockMove16(int) -> void;
  auto instructionInterrupt(r16) -> void;
  auto instructionStop() -> void;
  auto instructionWait() -> void;
  auto instructionExchangeCE() -> void;
  auto instructionSetFlag(bool&) -> void;
  auto instructionClearFlag(bool&) -> void;
  auto instructionResetP() -> void;
  auto instructionSetP() -> void;
  auto instructionTransfer8(r16, r16&) -> void;
  auto instructionTransfer16(r16, r16&) -> void;
  auto instructionTransferCS() -> void;
  auto instructionTransferSX8() -> void;
  auto instructionTransferSX16() -> void;
  auto instructionTransferXS() -> void;
  auto instructionPush8(r16) -> void;
  auto instructionPush16(r16) -> void;
  auto instructionPushD() -> void;
  auto instructionPull8(r16&) -> void;
  auto instructionPull16(r16&) -> void;
  auto instructionPullD() -> void;
  auto instructionPullB() -> void;
  auto instructionPullP() -> void;
  auto instructionPushEffectiveAddress() -> void;
  auto instructionPushEffectiveIndirectAddress() -> void;
  auto instructionPushEffectiveRelativeAddress() -> void;

  //instruction.cpp
  auto instruction() -> void;

  //serialization.cpp
  auto serialize(serializer&) -> void;

  //disassembler.cpp
  /*auto disassemble() -> nall::string;
  auto disassemble(nall::Natural<24> addr, bool e, bool m, bool x) -> nall::string;*/

  struct f8 {
    bool c = 0;  //carry
    bool z = 0;  //zero
    bool i = 0;  //interrupt disable
    bool d = 0;  //decimal mode
    bool x = 0;  //index register mode
    bool m = 0;  //accumulator mode
    bool v = 0;  //overflow
    bool n = 0;  //negative

    inline operator unsigned() const {
      return c << 0 | z << 1 | i << 2 | d << 3 | x << 4 | m << 5 | v << 6 | n << 7;
    }

    inline auto& operator=(unsigned data) {
      c = data & 0x01;
      z = data & 0x02;
      i = data & 0x04;
      d = data & 0x08;
      x = data & 0x10;
      m = data & 0x20;
      v = data & 0x40;
      n = data & 0x80;
      return *this;
    }
  };

  struct Registers {
    r24 pc;
    r16 a;
    r16 x;
    r16 y;
    r16 z;
    r16 s;
    r16 d;
     r8 b;
     f8 p;

    bool e   = 0;  //emulation mode
    bool irq = 0;  //IRQ pin (0 = low, 1 = trigger)
    bool wai = 0;  //raised during wai, cleared after interrupt triggered
    bool stp = 0;  //raised during stp, never cleared

    uint16_t vector;  //interrupt vector address
    nall::Natural<24> mar;     //memory address register
     r8 mdr;      //memory data register

    r24 u;  //temporary register
    r24 v;  //temporary register
    r24 w;  //temporary register
  } r;
};

}
