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

//WDC 65C816 CPU core
//* Ricoh 5A22
//* Nintendo SA-1

#pragma once

namespace Processor {

struct WDC65816 {
  virtual void idle() = 0;
  virtual void idleBranch() {}
  virtual void idleJump() {}
  virtual uint8_t read(unsigned addr) = 0;
  virtual void write(unsigned addr, uint8_t data) = 0;
  virtual void lastCycle() = 0;
  virtual bool interruptPending() const = 0;
  virtual void interrupt();
  virtual bool synchronizing() const = 0;

  virtual uint8_t readDisassembler(unsigned addr) { return 0; }

  inline bool irq() const { return r.irq; }
  inline void irq(bool line) { r.irq = line; }

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
  void power();

  //memory.cpp
  inline void idleIRQ();
  inline void idle2();
  inline void idle4(uint16_t x, uint16_t y);
  inline void idle6(uint16_t address);
  inline uint8_t fetch();
  inline uint8_t pull();
  void push(uint8_t data);
  inline uint8_t pullN();
  inline void pushN(uint8_t data);
  inline uint8_t readDirect(unsigned address);
  inline void writeDirect(unsigned address, uint8_t data);
  inline uint8_t readDirectN(unsigned address);
  inline uint8_t readBank(unsigned address);
  inline void writeBank(unsigned address, uint8_t data);
  inline uint8_t readLong(unsigned address);
  inline void writeLong(unsigned address, uint8_t data);
  inline uint8_t readStack(unsigned address);
  inline void writeStack(unsigned address, uint8_t data);

  //algorithms.cpp
  using  alu8 = uint8_t (WDC65816::*)( uint8_t);
  using alu16 = uint16_t (WDC65816::*)(uint16_t);

  uint8_t algorithmADC8(uint8_t);
  uint16_t algorithmADC16(uint16_t);
  uint8_t algorithmAND8(uint8_t);
  uint16_t algorithmAND16(uint16_t);
  uint8_t algorithmASL8(uint8_t);
  uint16_t algorithmASL16(uint16_t);
  uint8_t algorithmBIT8(uint8_t);
  uint16_t algorithmBIT16(uint16_t);
  uint8_t algorithmCMP8(uint8_t);
  uint16_t algorithmCMP16(uint16_t);
  uint8_t algorithmCPX8(uint8_t);
  uint16_t algorithmCPX16(uint16_t);
  uint8_t algorithmCPY8(uint8_t);
  uint16_t algorithmCPY16(uint16_t);
  uint8_t algorithmDEC8(uint8_t);
  uint16_t algorithmDEC16(uint16_t);
  uint8_t algorithmEOR8(uint8_t);
  uint16_t algorithmEOR16(uint16_t);
  uint8_t algorithmINC8(uint8_t);
  uint16_t algorithmINC16(uint16_t);
  uint8_t algorithmLDA8(uint8_t);
  uint16_t algorithmLDA16(uint16_t);
  uint8_t algorithmLDX8(uint8_t);
  uint16_t algorithmLDX16(uint16_t);
  uint8_t algorithmLDY8(uint8_t);
  uint16_t algorithmLDY16(uint16_t);
  uint8_t algorithmLSR8(uint8_t);
  uint16_t algorithmLSR16(uint16_t);
  uint8_t algorithmORA8(uint8_t);
  uint16_t algorithmORA16(uint16_t);
  uint8_t algorithmROL8(uint8_t);
  uint16_t algorithmROL16(uint16_t);
  uint8_t algorithmROR8(uint8_t);
  uint16_t algorithmROR16(uint16_t);
  uint8_t algorithmSBC8(uint8_t);
  uint16_t algorithmSBC16(uint16_t);
  uint8_t algorithmTRB8(uint8_t);
  uint16_t algorithmTRB16(uint16_t);
  uint8_t algorithmTSB8(uint8_t);
  uint16_t algorithmTSB16(uint16_t);

  //instructions-read.cpp
  void instructionImmediateRead8(alu8);
  void instructionImmediateRead16(alu16);
  void instructionBankRead8(alu8);
  void instructionBankRead16(alu16);
  void instructionBankRead8(alu8, r16);
  void instructionBankRead16(alu16, r16);
  void instructionLongRead8(alu8, r16 = {});
  void instructionLongRead16(alu16, r16 = {});
  void instructionDirectRead8(alu8);
  void instructionDirectRead16(alu16);
  void instructionDirectRead8(alu8, r16);
  void instructionDirectRead16(alu16, r16);
  void instructionIndirectRead8(alu8);
  void instructionIndirectRead16(alu16);
  void instructionIndexedIndirectRead8(alu8);
  void instructionIndexedIndirectRead16(alu16);
  void instructionIndirectIndexedRead8(alu8);
  void instructionIndirectIndexedRead16(alu16);
  void instructionIndirectLongRead8(alu8, r16 = {});
  void instructionIndirectLongRead16(alu16, r16 = {});
  void instructionStackRead8(alu8);
  void instructionStackRead16(alu16);
  void instructionIndirectStackRead8(alu8);
  void instructionIndirectStackRead16(alu16);

  //instructions-write.cpp
  void instructionBankWrite8(r16);
  void instructionBankWrite16(r16);
  void instructionBankWrite8(r16, r16);
  void instructionBankWrite16(r16, r16);
  void instructionLongWrite8(r16 = {});
  void instructionLongWrite16(r16 = {});
  void instructionDirectWrite8(r16);
  void instructionDirectWrite16(r16);
  void instructionDirectWrite8(r16, r16);
  void instructionDirectWrite16(r16, r16);
  void instructionIndirectWrite8();
  void instructionIndirectWrite16();
  void instructionIndexedIndirectWrite8();
  void instructionIndexedIndirectWrite16();
  void instructionIndirectIndexedWrite8();
  void instructionIndirectIndexedWrite16();
  void instructionIndirectLongWrite8(r16 = {});
  void instructionIndirectLongWrite16(r16 = {});
  void instructionStackWrite8();
  void instructionStackWrite16();
  void instructionIndirectStackWrite8();
  void instructionIndirectStackWrite16();

  //instructions-modify.cpp
  void instructionImpliedModify8(alu8, r16&);
  void instructionImpliedModify16(alu16, r16&);
  void instructionBankModify8(alu8);
  void instructionBankModify16(alu16);
  void instructionBankIndexedModify8(alu8);
  void instructionBankIndexedModify16(alu16);
  void instructionDirectModify8(alu8);
  void instructionDirectModify16(alu16);
  void instructionDirectIndexedModify8(alu8);
  void instructionDirectIndexedModify16(alu16);

  //instructions-pc.cpp
  void instructionBranch(bool = 1);
  void instructionBranchLong();
  void instructionJumpShort();
  void instructionJumpLong();
  void instructionJumpIndirect();
  void instructionJumpIndexedIndirect();
  void instructionJumpIndirectLong();
  void instructionCallShort();
  void instructionCallLong();
  void instructionCallIndexedIndirect();
  void instructionReturnInterrupt();
  void instructionReturnShort();
  void instructionReturnLong();

  //instructions-misc.cpp
  void instructionBitImmediate8();
  void instructionBitImmediate16();
  void instructionNoOperation();
  void instructionPrefix();
  void instructionExchangeBA();
  void instructionBlockMove8(int);
  void instructionBlockMove16(int);
  void instructionInterrupt(r16);
  void instructionStop();
  void instructionWait();
  void instructionExchangeCE();
  void instructionSetFlag(bool&);
  void instructionClearFlag(bool&);
  void instructionResetP();
  void instructionSetP();
  void instructionTransfer8(r16, r16&);
  void instructionTransfer16(r16, r16&);
  void instructionTransferCS();
  void instructionTransferSX8();
  void instructionTransferSX16();
  void instructionTransferXS();
  void instructionPush8(r16);
  void instructionPush16(r16);
  void instructionPushD();
  void instructionPull8(r16&);
  void instructionPull16(r16&);
  void instructionPullD();
  void instructionPullB();
  void instructionPullP();
  void instructionPushEffectiveAddress();
  void instructionPushEffectiveIndirectAddress();
  void instructionPushEffectiveRelativeAddress();

  //instruction.cpp
  void instruction();

  //serialization.cpp
  void serialize(serializer&);

  //disassembler.cpp
  /*nall::string disassemble();
  nall::string disassemble(nall::Natural<24> addr, bool e, bool m, bool x);*/

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
     f8 p;

    uint8_t b;

    bool e   = 0;  //emulation mode
    bool irq = 0;  //IRQ pin (0 = low, 1 = trigger)
    bool wai = 0;  //raised during wai, cleared after interrupt triggered
    bool stp = 0;  //raised during stp, never cleared

    uint16_t vector;  //interrupt vector address
    nall::Natural<24> mar;     //memory address register
    uint8_t mdr;      //memory data register

    r24 u;  //temporary register
    r24 v;  //temporary register
    r24 w;  //temporary register
  } r;
};

}
