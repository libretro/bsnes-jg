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

#include "../emulator.hpp"

#include "wdc65816.hpp"

namespace Processor {

#define PC r.pc
#define A  r.a
#define X  r.x
#define Y  r.y
#define Z  r.z
#define S  r.s
#define D  r.d
#define B  r.b
#define P  r.p

#define CF r.p.c
#define ZF r.p.z
#define IF r.p.i
#define DF r.p.d
#define XF r.p.x
#define MF r.p.m
#define VF r.p.v
#define NF r.p.n
#define EF r.e

#define U r.u
#define V r.v
#define W r.w

#define alu(...) (this->*op)(__VA_ARGS__)

//immediate, 2-cycle opcodes with idle cycle will become bus read
//when an IRQ is to be triggered immediately after opcode completion.
//this affects the following opcodes:
//  clc, cld, cli, clv, sec, sed, sei,
//  tax, tay, txa, txy, tya, tyx,
//  tcd, tcs, tdc, tsc, tsx, txs,
//  inc, inx, iny, dec, dex, dey,
//  asl, lsr, rol, ror, nop, xce.
void WDC65816::idleIRQ() {
  if(interruptPending()) {
    //modify I/O cycle to bus read cycle, do not increment PC
    read(PC.d);
  } else {
    idle();
  }
}

void WDC65816::idle2() {
  if(D.l) idle();
}

void WDC65816::idle4(uint16_t x, uint16_t y) {
  if(!XF || x >> 8 != y >> 8) idle();
}

void WDC65816::idle6(uint16_t address) {
  if(EF && PC.h != address >> 8) idle();
}

uint8_t WDC65816::fetch() {
  return read(PC.b << 16 | PC.w++);
}

uint8_t WDC65816::pull() {
  EF ? (void)S.l++ : (void)S.w++;
  return read(S.w);
}

void WDC65816::push(uint8_t data) {
  write(S.w, data);
  EF ? (void)S.l-- : (void)S.w--;
}

uint8_t WDC65816::pullN() {
  return read(++S.w);
}

void WDC65816::pushN(uint8_t data) {
  write(S.w--, data);
}

uint8_t WDC65816::readDirect(unsigned address) {
  if(EF && !D.l) return read(D.w | (address & 0xff));
  return read(D.w + address & 0xffff);
}

void WDC65816::writeDirect(unsigned address, uint8_t data) {
  if(EF && !D.l) return write(D.w | (address & 0xff), data);
  write(D.w + address & 0xffff, data);
}

uint8_t WDC65816::readDirectN(unsigned address) {
  return read(D.w + address & 0xffff);
}

uint8_t WDC65816::readBank(unsigned address) {
  return read((B << 16) + address & 0xffffff);
}

void WDC65816::writeBank(unsigned address, uint8_t data) {
  write((B << 16) + address & 0xffffff, data);
}

uint8_t WDC65816::readLong(unsigned address) {
  return read(address & 0xffffff);
}

void WDC65816::writeLong(unsigned address, uint8_t data) {
  write(address & 0xffffff, data);
}

uint8_t WDC65816::readStack(unsigned address) {
  return read(S.w + address & 0xffff);
}

void WDC65816::writeStack(unsigned address, uint8_t data) {
  write(S.w + address & 0xffff, data);
}

uint8_t WDC65816::algorithmADC8(uint8_t data) {
  int result;

  if(!DF) {
    result = A.l + data + CF;
  } else {
    result = (A.l & 0x0f) + (data & 0x0f) + (CF << 0);
    if(result > 0x09) result += 0x06;
    CF = result > 0x0f;
    result = (A.l & 0xf0) + (data & 0xf0) + (CF << 4) + (result & 0x0f);
  }

  VF = ~(A.l ^ data) & (A.l ^ result) & 0x80;
  if(DF && result > 0x9f) result += 0x60;
  CF = result > 0xff;
  ZF = (uint8_t)result == 0;
  NF = result & 0x80;

  return A.l = result;
}

uint16_t WDC65816::algorithmADC16(uint16_t data) {
  int result;

  if(!DF) {
    result = A.w + data + CF;
  } else {
    result = (A.w & 0x000f) + (data & 0x000f) + (CF <<  0);
    if(result > 0x0009) result += 0x0006;
    CF = result > 0x000f;
    result = (A.w & 0x00f0) + (data & 0x00f0) + (CF <<  4) + (result & 0x000f);
    if(result > 0x009f) result += 0x0060;
    CF = result > 0x00ff;
    result = (A.w & 0x0f00) + (data & 0x0f00) + (CF <<  8) + (result & 0x00ff);
    if(result > 0x09ff) result += 0x0600;
    CF = result > 0x0fff;
    result = (A.w & 0xf000) + (data & 0xf000) + (CF << 12) + (result & 0x0fff);
  }

  VF = ~(A.w ^ data) & (A.w ^ result) & 0x8000;
  if(DF && result > 0x9fff) result += 0x6000;
  CF = result > 0xffff;
  ZF = (uint16_t)result == 0;
  NF = result & 0x8000;

  return A.w = result;
}

uint8_t WDC65816::algorithmAND8(uint8_t data) {
  A.l &= data;
  ZF = A.l == 0;
  NF = A.l & 0x80;
  return A.l;
}

uint16_t WDC65816::algorithmAND16(uint16_t data) {
  A.w &= data;
  ZF = A.w == 0;
  NF = A.w & 0x8000;
  return A.w;
}

uint8_t WDC65816::algorithmASL8(uint8_t data) {
  CF = data & 0x80;
  data <<= 1;
  ZF = data == 0;
  NF = data & 0x80;
  return data;
}

uint16_t WDC65816::algorithmASL16(uint16_t data) {
  CF = data & 0x8000;
  data <<= 1;
  ZF = data == 0;
  NF = data & 0x8000;
  return data;
}

uint8_t WDC65816::algorithmBIT8(uint8_t data) {
  ZF = (data & A.l) == 0;
  VF = data & 0x40;
  NF = data & 0x80;
  return data;
}

uint16_t WDC65816::algorithmBIT16(uint16_t data) {
  ZF = (data & A.w) == 0;
  VF = data & 0x4000;
  NF = data & 0x8000;
  return data;
}

uint8_t WDC65816::algorithmCMP8(uint8_t data) {
  int result = A.l - data;
  CF = result >= 0;
  ZF = (uint8_t)result == 0;
  NF = result & 0x80;
  return result;
}

uint16_t WDC65816::algorithmCMP16(uint16_t data) {
  int result = A.w - data;
  CF = result >= 0;
  ZF = (uint16_t)result == 0;
  NF = result & 0x8000;
  return result;
}

uint8_t WDC65816::algorithmCPX8(uint8_t data) {
  int result = X.l - data;
  CF = result >= 0;
  ZF = (uint8_t)result == 0;
  NF = result & 0x80;
  return result;
}

uint16_t WDC65816::algorithmCPX16(uint16_t data) {
  int result = X.w - data;
  CF = result >= 0;
  ZF = (uint16_t)result == 0;
  NF = result & 0x8000;
  return result;
}

uint8_t WDC65816::algorithmCPY8(uint8_t data) {
  int result = Y.l - data;
  CF = result >= 0;
  ZF = (uint8_t)result == 0;
  NF = result & 0x80;
  return result;
}

uint16_t WDC65816::algorithmCPY16(uint16_t data) {
  int result = Y.w - data;
  CF = result >= 0;
  ZF = (uint16_t)result == 0;
  NF = result & 0x8000;
  return result;
}

uint8_t WDC65816::algorithmDEC8(uint8_t data) {
  data--;
  ZF = data == 0;
  NF = data & 0x80;
  return data;
}

uint16_t WDC65816::algorithmDEC16(uint16_t data) {
  data--;
  ZF = data == 0;
  NF = data & 0x8000;
  return data;
}

uint8_t WDC65816::algorithmEOR8(uint8_t data) {
  A.l ^= data;
  ZF = A.l == 0;
  NF = A.l & 0x80;
  return A.l;
}

uint16_t WDC65816::algorithmEOR16(uint16_t data) {
  A.w ^= data;
  ZF = A.w == 0;
  NF = A.w & 0x8000;
  return A.w;
}

uint8_t WDC65816::algorithmINC8(uint8_t data) {
  data++;
  ZF = data == 0;
  NF = data & 0x80;
  return data;
}

uint16_t WDC65816::algorithmINC16(uint16_t data) {
  data++;
  ZF = data == 0;
  NF = data & 0x8000;
  return data;
}

uint8_t WDC65816::algorithmLDA8(uint8_t data) {
  A.l = data;
  ZF = A.l == 0;
  NF = A.l & 0x80;
  return data;
}

uint16_t WDC65816::algorithmLDA16(uint16_t data) {
  A.w = data;
  ZF = A.w == 0;
  NF = A.w & 0x8000;
  return data;
}

uint8_t WDC65816::algorithmLDX8(uint8_t data) {
  X.l = data;
  ZF = X.l == 0;
  NF = X.l & 0x80;
  return data;
}

uint16_t WDC65816::algorithmLDX16(uint16_t data) {
  X.w = data;
  ZF = X.w == 0;
  NF = X.w & 0x8000;
  return data;
}

uint8_t WDC65816::algorithmLDY8(uint8_t data) {
  Y.l = data;
  ZF = Y.l == 0;
  NF = Y.l & 0x80;
  return data;
}

uint16_t WDC65816::algorithmLDY16(uint16_t data) {
  Y.w = data;
  ZF = Y.w == 0;
  NF = Y.w & 0x8000;
  return data;
}

uint8_t WDC65816::algorithmLSR8(uint8_t data) {
  CF = data & 1;
  data >>= 1;
  ZF = data == 0;
  NF = data & 0x80;
  return data;
}

uint16_t WDC65816::algorithmLSR16(uint16_t data) {
  CF = data & 1;
  data >>= 1;
  ZF = data == 0;
  NF = data & 0x8000;
  return data;
}

uint8_t WDC65816::algorithmORA8(uint8_t data) {
  A.l |= data;
  ZF = A.l == 0;
  NF = A.l & 0x80;
  return A.l;
}

uint16_t WDC65816::algorithmORA16(uint16_t data) {
  A.w |= data;
  ZF = A.w == 0;
  NF = A.w & 0x8000;
  return A.w;
}

uint8_t WDC65816::algorithmROL8(uint8_t data) {
  bool carry = CF;
  CF = data & 0x80;
  data = data << 1 | carry;
  ZF = data == 0;
  NF = data & 0x80;
  return data;
}

uint16_t WDC65816::algorithmROL16(uint16_t data) {
  bool carry = CF;
  CF = data & 0x8000;
  data = data << 1 | carry;
  ZF = data == 0;
  NF = data & 0x8000;
  return data;
}

uint8_t WDC65816::algorithmROR8(uint8_t data) {
  bool carry = CF;
  CF = data & 1;
  data = carry << 7 | data >> 1;
  ZF = data == 0;
  NF = data & 0x80;
  return data;
}

uint16_t WDC65816::algorithmROR16(uint16_t data) {
  bool carry = CF;
  CF = data & 1;
  data = carry << 15 | data >> 1;
  ZF = data == 0;
  NF = data & 0x8000;
  return data;
}

uint8_t WDC65816::algorithmSBC8(uint8_t data) {
  int result;
  data = ~data;

  if(!DF) {
    result = A.l + data + CF;
  } else {
    result = (A.l & 0x0f) + (data & 0x0f) + (CF << 0);
    if(result <= 0x0f) result -= 0x06;
    CF = result > 0x0f;
    result = (A.l & 0xf0) + (data & 0xf0) + (CF << 4) + (result & 0x0f);
  }

  VF = ~(A.l ^ data) & (A.l ^ result) & 0x80;
  if(DF && result <= 0xff) result -= 0x60;
  CF = result > 0xff;
  ZF = (uint8_t)result == 0;
  NF = result & 0x80;

  return A.l = result;
}

uint16_t WDC65816::algorithmSBC16(uint16_t data) {
  int result;
  data = ~data;

  if(!DF) {
    result = A.w + data + CF;
  } else {
    result = (A.w & 0x000f) + (data & 0x000f) + (CF <<  0);
    if(result <= 0x000f) result -= 0x0006;
    CF = result > 0x000f;
    result = (A.w & 0x00f0) + (data & 0x00f0) + (CF <<  4) + (result & 0x000f);
    if(result <= 0x00ff) result -= 0x0060;
    CF = result > 0x00ff;
    result = (A.w & 0x0f00) + (data & 0x0f00) + (CF <<  8) + (result & 0x00ff);
    if(result <= 0x0fff) result -= 0x0600;
    CF = result > 0x0fff;
    result = (A.w & 0xf000) + (data & 0xf000) + (CF << 12) + (result & 0x0fff);
  }

  VF = ~(A.w ^ data) & (A.w ^ result) & 0x8000;
  if(DF && result <= 0xffff) result -= 0x6000;
  CF = result > 0xffff;
  ZF = (uint16_t)result == 0;
  NF = result & 0x8000;

  return A.w = result;
}

uint8_t WDC65816::algorithmTRB8(uint8_t data) {
  ZF = (data & A.l) == 0;
  data &= ~A.l;
  return data;
}

uint16_t WDC65816::algorithmTRB16(uint16_t data) {
  ZF = (data & A.w) == 0;
  data &= ~A.w;
  return data;
}

uint8_t WDC65816::algorithmTSB8(uint8_t data) {
  ZF = (data & A.l) == 0;
  data |= A.l;
  return data;
}

uint16_t WDC65816::algorithmTSB16(uint16_t data) {
  ZF = (data & A.w) == 0;
  data |= A.w;
  return data;
}

void WDC65816::instructionImmediateRead8(alu8 op) {
  lastCycle();
  W.l = fetch();
  alu(W.l);
}

void WDC65816::instructionImmediateRead16(alu16 op) {
  W.l = fetch();
  lastCycle();
  W.h = fetch();
  alu(W.w);
}

void WDC65816::instructionBankRead8(alu8 op) {
  V.l = fetch();
  V.h = fetch();
  lastCycle();
  W.l = readBank(V.w + 0);
  alu(W.l);
}

void WDC65816::instructionBankRead16(alu16 op) {
  V.l = fetch();
  V.h = fetch();
  W.l = readBank(V.w + 0);
  lastCycle();
  W.h = readBank(V.w + 1);
  alu(W.w);
}

void WDC65816::instructionBankRead8(alu8 op, r16 I) {
  V.l = fetch();
  V.h = fetch();
  idle4(V.w, V.w + I.w);
  lastCycle();
  W.l = readBank(V.w + I.w + 0);
  alu(W.l);
}

void WDC65816::instructionBankRead16(alu16 op, r16 I) {
  V.l = fetch();
  V.h = fetch();
  idle4(V.w, V.w + I.w);
  W.l = readBank(V.w + I.w + 0);
  lastCycle();
  W.h = readBank(V.w + I.w + 1);
  alu(W.w);
}

void WDC65816::instructionLongRead8(alu8 op, r16 I) {
  V.l = fetch();
  V.h = fetch();
  V.b = fetch();
  lastCycle();
  W.l = readLong(V.d + I.w + 0);
  alu(W.l);
}

void WDC65816::instructionLongRead16(alu16 op, r16 I) {
  V.l = fetch();
  V.h = fetch();
  V.b = fetch();
  W.l = readLong(V.d + I.w + 0);
  lastCycle();
  W.h = readLong(V.d + I.w + 1);
  alu(W.w);
}

void WDC65816::instructionDirectRead8(alu8 op) {
  U.l = fetch();
  idle2();
  lastCycle();
  W.l = readDirect(U.l + 0);
  alu(W.l);
}

void WDC65816::instructionDirectRead16(alu16 op) {
  U.l = fetch();
  idle2();
  W.l = readDirect(U.l + 0);
  lastCycle();
  W.h = readDirect(U.l + 1);
  alu(W.w);
}

void WDC65816::instructionDirectRead8(alu8 op, r16 I) {
  U.l = fetch();
  idle2();
  idle();
  lastCycle();
  W.l = readDirect(U.l + I.w + 0);
  alu(W.l);
}

void WDC65816::instructionDirectRead16(alu16 op, r16 I) {
  U.l = fetch();
  idle2();
  idle();
  W.l = readDirect(U.l + I.w + 0);
  lastCycle();
  W.h = readDirect(U.l + I.w + 1);
  alu(W.w);
}

void WDC65816::instructionIndirectRead8(alu8 op) {
  U.l = fetch();
  idle2();
  V.l = readDirect(U.l + 0);
  V.h = readDirect(U.l + 1);
  lastCycle();
  W.l = readBank(V.w + 0);
  alu(W.l);
}

void WDC65816::instructionIndirectRead16(alu16 op) {
  U.l = fetch();
  idle2();
  V.l = readDirect(U.l + 0);
  V.h = readDirect(U.l + 1);
  W.l = readBank(V.w + 0);
  lastCycle();
  W.h = readBank(V.w + 1);
  alu(W.w);
}

void WDC65816::instructionIndexedIndirectRead8(alu8 op) {
  U.l = fetch();
  idle2();
  idle();
  V.l = readDirect(U.l + X.w + 0);
  V.h = readDirect(U.l + X.w + 1);
  lastCycle();
  W.l = readBank(V.w + 0);
  alu(W.l);
}

void WDC65816::instructionIndexedIndirectRead16(alu16 op) {
  U.l = fetch();
  idle2();
  idle();
  V.l = readDirect(U.l + X.w + 0);
  V.h = readDirect(U.l + X.w + 1);
  W.l = readBank(V.w + 0);
  lastCycle();
  W.h = readBank(V.w + 1);
  alu(W.w);
}

void WDC65816::instructionIndirectIndexedRead8(alu8 op) {
  U.l = fetch();
  idle2();
  V.l = readDirect(U.l + 0);
  V.h = readDirect(U.l + 1);
  idle4(V.w, V.w + Y.w);
  lastCycle();
  W.l = readBank(V.w + Y.w + 0);
  alu(W.l);
}

void WDC65816::instructionIndirectIndexedRead16(alu16 op) {
  U.l = fetch();
  idle2();
  V.l = readDirect(U.l + 0);
  V.h = readDirect(U.l + 1);
  idle4(V.w, V.w + Y.w);
  W.l = readBank(V.w + Y.w + 0);
  lastCycle();
  W.h = readBank(V.w + Y.w + 1);
  alu(W.w);
}

void WDC65816::instructionIndirectLongRead8(alu8 op, r16 I) {
  U.l = fetch();
  idle2();
  V.l = readDirectN(U.l + 0);
  V.h = readDirectN(U.l + 1);
  V.b = readDirectN(U.l + 2);
  lastCycle();
  W.l = readLong(V.d + I.w + 0);
  alu(W.l);
}

void WDC65816::instructionIndirectLongRead16(alu16 op, r16 I) {
  U.l = fetch();
  idle2();
  V.l = readDirectN(U.l + 0);
  V.h = readDirectN(U.l + 1);
  V.b = readDirectN(U.l + 2);
  W.l = readLong(V.d + I.w + 0);
  lastCycle();
  W.h = readLong(V.d + I.w + 1);
  alu(W.w);
}

void WDC65816::instructionStackRead8(alu8 op) {
  U.l = fetch();
  idle();
  lastCycle();
  W.l = readStack(U.l + 0);
  alu(W.l);
}

void WDC65816::instructionStackRead16(alu16 op) {
  U.l = fetch();
  idle();
  W.l = readStack(U.l + 0);
  lastCycle();
  W.h = readStack(U.l + 1);
  alu(W.w);
}

void WDC65816::instructionIndirectStackRead8(alu8 op) {
  U.l = fetch();
  idle();
  V.l = readStack(U.l + 0);
  V.h = readStack(U.l + 1);
  idle();
  lastCycle();
  W.l = readBank(V.w + Y.w + 0);
  alu(W.l);
}

void WDC65816::instructionIndirectStackRead16(alu16 op) {
  U.l = fetch();
  idle();
  V.l = readStack(U.l + 0);
  V.h = readStack(U.l + 1);
  idle();
  W.l = readBank(V.w + Y.w + 0);
  lastCycle();
  W.h = readBank(V.w + Y.w + 1);
  alu(W.w);
}

void WDC65816::instructionBankWrite8(r16 F) {
  V.l = fetch();
  V.h = fetch();
  lastCycle();
  writeBank(V.w + 0, F.l);
}

void WDC65816::instructionBankWrite16(r16 F) {
  V.l = fetch();
  V.h = fetch();
  writeBank(V.w + 0, F.l);
  lastCycle();
  writeBank(V.w + 1, F.h);
}

void WDC65816::instructionBankWrite8(r16 F, r16 I) {
  V.l = fetch();
  V.h = fetch();
  idle();
  lastCycle();
  writeBank(V.w + I.w + 0, F.l);
}

void WDC65816::instructionBankWrite16(r16 F, r16 I) {
  V.l = fetch();
  V.h = fetch();
  idle();
  writeBank(V.w + I.w + 0, F.l);
  lastCycle();
  writeBank(V.w + I.w + 1, F.h);
}

void WDC65816::instructionLongWrite8(r16 I) {
  V.l = fetch();
  V.h = fetch();
  V.b = fetch();
  lastCycle();
  writeLong(V.d + I.w + 0, A.l);
}

void WDC65816::instructionLongWrite16(r16 I) {
  V.l = fetch();
  V.h = fetch();
  V.b = fetch();
  writeLong(V.d + I.w + 0, A.l);
  lastCycle();
  writeLong(V.d + I.w + 1, A.h);
}

void WDC65816::instructionDirectWrite8(r16 F) {
  U.l = fetch();
  idle2();
  lastCycle();
  writeDirect(U.l + 0, F.l);
}

void WDC65816::instructionDirectWrite16(r16 F) {
  U.l = fetch();
  idle2();
  writeDirect(U.l + 0, F.l);
  lastCycle();
  writeDirect(U.l + 1, F.h);
}

void WDC65816::instructionDirectWrite8(r16 F, r16 I) {
  U.l = fetch();
  idle2();
  idle();
  lastCycle();
  writeDirect(U.l + I.w + 0, F.l);
}

void WDC65816::instructionDirectWrite16(r16 F, r16 I) {
  U.l = fetch();
  idle2();
  idle();
  writeDirect(U.l + I.w + 0, F.l);
  lastCycle();
  writeDirect(U.l + I.w + 1, F.h);
}

void WDC65816::instructionIndirectWrite8() {
  U.l = fetch();
  idle2();
  V.l = readDirect(U.l + 0);
  V.h = readDirect(U.l + 1);
  lastCycle();
  writeBank(V.w + 0, A.l);
}

void WDC65816::instructionIndirectWrite16() {
  U.l = fetch();
  idle2();
  V.l = readDirect(U.l + 0);
  V.h = readDirect(U.l + 1);
  writeBank(V.w + 0, A.l);
  lastCycle();
  writeBank(V.w + 1, A.h);
}

void WDC65816::instructionIndexedIndirectWrite8() {
  U.l = fetch();
  idle2();
  idle();
  V.l = readDirect(U.l + X.w + 0);
  V.h = readDirect(U.l + X.w + 1);
  lastCycle();
  writeBank(V.w + 0, A.l);
}

void WDC65816::instructionIndexedIndirectWrite16() {
  U.l = fetch();
  idle2();
  idle();
  V.l = readDirect(U.l + X.w + 0);
  V.h = readDirect(U.l + X.w + 1);
  writeBank(V.w + 0, A.l);
  lastCycle();
  writeBank(V.w + 1, A.h);
}

void WDC65816::instructionIndirectIndexedWrite8() {
  U.l = fetch();
  idle2();
  V.l = readDirect(U.l + 0);
  V.h = readDirect(U.l + 1);
  idle();
  lastCycle();
  writeBank(V.w + Y.w + 0, A.l);
}

void WDC65816::instructionIndirectIndexedWrite16() {
  U.l = fetch();
  idle2();
  V.l = readDirect(U.l + 0);
  V.h = readDirect(U.l + 1);
  idle();
  writeBank(V.w + Y.w + 0, A.l);
  lastCycle();
  writeBank(V.w + Y.w + 1, A.h);
}

void WDC65816::instructionIndirectLongWrite8(r16 I) {
  U.l = fetch();
  idle2();
  V.l = readDirectN(U.l + 0);
  V.h = readDirectN(U.l + 1);
  V.b = readDirectN(U.l + 2);
  lastCycle();
  writeLong(V.d + I.w + 0, A.l);
}

void WDC65816::instructionIndirectLongWrite16(r16 I) {
  U.l = fetch();
  idle2();
  V.l = readDirectN(U.l + 0);
  V.h = readDirectN(U.l + 1);
  V.b = readDirectN(U.l + 2);
  writeLong(V.d + I.w + 0, A.l);
  lastCycle();
  writeLong(V.d + I.w + 1, A.h);
}

void WDC65816::instructionStackWrite8() {
  U.l = fetch();
  idle();
  lastCycle();
  writeStack(U.l + 0, A.l);
}

void WDC65816::instructionStackWrite16() {
  U.l = fetch();
  idle();
  writeStack(U.l + 0, A.l);
  lastCycle();
  writeStack(U.l + 1, A.h);
}

void WDC65816::instructionIndirectStackWrite8() {
  U.l = fetch();
  idle();
  V.l = readStack(U.l + 0);
  V.h = readStack(U.l + 1);
  idle();
  lastCycle();
  writeBank(V.w + Y.w + 0, A.l);
}

void WDC65816::instructionIndirectStackWrite16() {
  U.l = fetch();
  idle();
  V.l = readStack(U.l + 0);
  V.h = readStack(U.l + 1);
  idle();
  writeBank(V.w + Y.w + 0, A.l);
  lastCycle();
  writeBank(V.w + Y.w + 1, A.h);
}

void WDC65816::instructionImpliedModify8(alu8 op, r16& M) {
  lastCycle();
  idleIRQ();
  M.l = alu(M.l);
}

void WDC65816::instructionImpliedModify16(alu16 op, r16& M) {
  lastCycle();
  idleIRQ();
  M.w = alu(M.w);
}

void WDC65816::instructionBankModify8(alu8 op) {
  V.l = fetch();
  V.h = fetch();
  W.l = readBank(V.w + 0);
  idle();
  W.l = alu(W.l);
  lastCycle();
  writeBank(V.w + 0, W.l);
}

void WDC65816::instructionBankModify16(alu16 op) {
  V.l = fetch();
  V.h = fetch();
  W.l = readBank(V.w + 0);
  W.h = readBank(V.w + 1);
  idle();
  W.w = alu(W.w);
  writeBank(V.w + 1, W.h);
  lastCycle();
  writeBank(V.w + 0, W.l);
}

void WDC65816::instructionBankIndexedModify8(alu8 op) {
  V.l = fetch();
  V.h = fetch();
  idle();
  W.l = readBank(V.w + X.w + 0);
  idle();
  W.l = alu(W.l);
  lastCycle();
  writeBank(V.w + X.w + 0, W.l);
}

void WDC65816::instructionBankIndexedModify16(alu16 op) {
  V.l = fetch();
  V.h = fetch();
  idle();
  W.l = readBank(V.w + X.w + 0);
  W.h = readBank(V.w + X.w + 1);
  idle();
  W.w = alu(W.w);
  writeBank(V.w + X.w + 1, W.h);
  lastCycle();
  writeBank(V.w + X.w + 0, W.l);
}

void WDC65816::instructionDirectModify8(alu8 op) {
  U.l = fetch();
  idle2();
  W.l = readDirect(U.l + 0);
  idle();
  W.l = alu(W.l);
  lastCycle();
  writeDirect(U.l + 0, W.l);
}

void WDC65816::instructionDirectModify16(alu16 op) {
  U.l = fetch();
  idle2();
  W.l = readDirect(U.l + 0);
  W.h = readDirect(U.l + 1);
  idle();
  W.w = alu(W.w);
  writeDirect(U.l + 1, W.h);
  lastCycle();
  writeDirect(U.l + 0, W.l);
}

void WDC65816::instructionDirectIndexedModify8(alu8 op) {
  U.l = fetch();
  idle2();
  idle();
  W.l = readDirect(U.l + X.w + 0);
  idle();
  W.l = alu(W.l);
  lastCycle();
  writeDirect(U.l + X.w + 0, W.l);
}

void WDC65816::instructionDirectIndexedModify16(alu16 op) {
  U.l = fetch();
  idle2();
  idle();
  W.l = readDirect(U.l + X.w + 0);
  W.h = readDirect(U.l + X.w + 1);
  idle();
  W.w = alu(W.w);
  writeDirect(U.l + X.w + 1, W.h);
  lastCycle();
  writeDirect(U.l + X.w + 0, W.l);
}

void WDC65816::instructionBranch(bool take) {
  if(!take) {
  lastCycle();
    fetch();
  } else {
    U.l = fetch();
    V.w = PC.d + (int8_t)U.l;
    idle6(V.w);
  lastCycle();
    idle();
    PC.w = V.w;
    idleBranch();
  }
}

void WDC65816::instructionBranchLong() {
  U.l = fetch();
  U.h = fetch();
  V.w = PC.d + (int16_t)U.w;
  lastCycle();
  idle();
  PC.w = V.w;
  idleBranch();
}

void WDC65816::instructionJumpShort() {
  W.l = fetch();
  lastCycle();
  W.h = fetch();
  PC.w = W.w;
  idleJump();
}

void WDC65816::instructionJumpLong() {
  V.l = fetch();
  V.h = fetch();
  lastCycle();
  V.b = fetch();
  PC.d = V.d;
  idleJump();
}

void WDC65816::instructionJumpIndirect() {
  V.l = fetch();
  V.h = fetch();
  W.l = read(uint16_t(V.w + 0));
  lastCycle();
  W.h = read(uint16_t(V.w + 1));
  PC.w = W.w;
  idleJump();
}

void WDC65816::instructionJumpIndexedIndirect() {
  V.l = fetch();
  V.h = fetch();
  idle();
  W.l = read(PC.b << 16 | uint16_t(V.w + X.w + 0));
  lastCycle();
  W.h = read(PC.b << 16 | uint16_t(V.w + X.w + 1));
  PC.w = W.w;
  idleJump();
}

void WDC65816::instructionJumpIndirectLong() {
  U.l = fetch();
  U.h = fetch();
  V.l = read(uint16_t(U.w + 0));
  V.h = read(uint16_t(U.w + 1));
  lastCycle();
  V.b = read(uint16_t(U.w + 2));
  PC.d = V.d;
  idleJump();
}

void WDC65816::instructionCallShort() {
  W.l = fetch();
  W.h = fetch();
  idle();
  PC.w--;
  push(PC.h);
  lastCycle();
  push(PC.l);
  PC.w = W.w;
  idleJump();
}

void WDC65816::instructionCallLong() {
  V.l = fetch();
  V.h = fetch();
  pushN(PC.b);
  idle();
  V.b = fetch();
  PC.w--;
  pushN(PC.h);
  lastCycle();
  pushN(PC.l);
  PC.d = V.d;
  if(r.e) S.h = 0x01;
  idleJump();
}

void WDC65816::instructionCallIndexedIndirect() {
  V.l = fetch();
  pushN(PC.h);
  pushN(PC.l);
  V.h = fetch();
  idle();
  W.l = read(PC.b << 16 | uint16_t(V.w + X.w + 0));
  lastCycle();
  W.h = read(PC.b << 16 | uint16_t(V.w + X.w + 1));
  PC.w = W.w;
  if(r.e) S.h = 0x01;
  idleJump();
}

void WDC65816::instructionReturnInterrupt() {
  idle();
  idle();
  P = pull();
  if(r.e) XF = 1, MF = 1;
  if(XF) X.h = 0x00, Y.h = 0x00;
  PC.l = pull();
  if(EF) {
    lastCycle();
    PC.h = pull();
  } else {
    PC.h = pull();
    lastCycle();
    PC.b = pull();
  }
  idleJump();
}

void WDC65816::instructionReturnShort() {
  idle();
  idle();
  W.l = pull();
  W.h = pull();
  lastCycle();
  idle();
  PC.w = W.w;
  PC.w++;
  idleJump();
}

void WDC65816::instructionReturnLong() {
  idle();
  idle();
  V.l = pullN();
  V.h = pullN();
  lastCycle();
  V.b = pullN();
  PC.d = V.d;
  PC.w++;
  if(r.e) S.h = 0x01;
  idleJump();
}

void WDC65816::instructionBitImmediate8() {
  lastCycle();
  U.l = fetch();
  ZF = (U.l & A.l) == 0;
}

void WDC65816::instructionBitImmediate16() {
  U.l = fetch();
  lastCycle();
  U.h = fetch();
  ZF = (U.w & A.w) == 0;
}

void WDC65816::instructionNoOperation() {
  lastCycle();
  idleIRQ();
}

void WDC65816::instructionPrefix() {
  lastCycle();
  fetch();
}

void WDC65816::instructionExchangeBA() {
  idle();
  lastCycle();
  idle();
  A.w = A.w >> 8 | A.w << 8;
  ZF = A.l == 0;
  NF = A.l & 0x80;
}

void WDC65816::instructionBlockMove8(int adjust) {
  U.b = fetch();
  V.b = fetch();
  B = U.b;
  W.l = read(V.b << 16 | X.w);
  write(U.b << 16 | Y.w, W.l);
  idle();
  X.l += adjust;
  Y.l += adjust;
  lastCycle();
  idle();
  if(A.w--) PC.w -= 3;
}

void WDC65816::instructionBlockMove16(int adjust) {
  U.b = fetch();
  V.b = fetch();
  B = U.b;
  W.l = read(V.b << 16 | X.w);
  write(U.b << 16 | Y.w, W.l);
  idle();
  X.w += adjust;
  Y.w += adjust;
  lastCycle();
  idle();
  if(A.w--) PC.w -= 3;
}

void WDC65816::instructionInterrupt(r16 vector) {
  fetch();
  if(!r.e) push(PC.b);
  push(PC.h);
  push(PC.l);
  push(P);
  IF = 1;
  DF = 0;
  PC.l = read(vector.w + 0);
  lastCycle();
  PC.h = read(vector.w + 1);
  PC.b = 0x00;
}

void WDC65816::instructionStop() {
  r.stp = true;
  while(r.stp && !synchronizing()) {
    lastCycle();
    idle();
  }
}

void WDC65816::instructionWait() {
  r.wai = true;
  while(r.wai && !synchronizing()) {
    lastCycle();
    idle();
  }
  idle();
}

void WDC65816::instructionExchangeCE() {
  lastCycle();
  idleIRQ();
  std::swap(CF, EF);
  if(EF) {
    XF = 1;
    MF = 1;
    X.h = 0x00;
    Y.h = 0x00;
    S.h = 0x01;
  }
}

void WDC65816::instructionSetFlag(bool& flag) {
  lastCycle();
  idleIRQ();
  flag = 1;
}

void WDC65816::instructionClearFlag(bool& flag) {
  lastCycle();
  idleIRQ();
  flag = 0;
}

void WDC65816::instructionResetP() {
  W.l = fetch();
  lastCycle();
  idle();
  P = P & ~W.l;
  if(r.e) XF = 1, MF = 1;
  if(XF) X.h = 0x00, Y.h = 0x00;
}

void WDC65816::instructionSetP() {
  W.l = fetch();
  lastCycle();
  idle();
  P = P | W.l;
  if(r.e) XF = 1, MF = 1;
  if(XF) X.h = 0x00, Y.h = 0x00;
}

void WDC65816::instructionTransfer8(r16 F, r16& T) {
  lastCycle();
  idleIRQ();
  T.l = F.l;
  ZF = T.l == 0;
  NF = T.l & 0x80;
}

void WDC65816::instructionTransfer16(r16 F, r16& T) {
  lastCycle();
  idleIRQ();
  T.w = F.w;
  ZF = T.w == 0;
  NF = T.w & 0x8000;
}

void WDC65816::instructionTransferCS() {
  lastCycle();
  idleIRQ();
  S.w = A.w;
  if(r.e) S.h = 0x01;
}

void WDC65816::instructionTransferSX8() {
  lastCycle();
  idleIRQ();
  X.l = S.l;
  ZF = X.l == 0;
  NF = X.l & 0x80;
}

void WDC65816::instructionTransferSX16() {
  lastCycle();
  idleIRQ();
  X.w = S.w;
  ZF = X.w == 0;
  NF = X.w & 0x8000;
}

void WDC65816::instructionTransferXS() {
  lastCycle();
  idleIRQ();
  if(r.e) S.l = X.l;
  if(!r.e) S.w = X.w;
}

void WDC65816::instructionPush8(r16 F) {
  idle();
  lastCycle();
  push(F.l);
}

void WDC65816::instructionPush16(r16 F) {
  idle();
  push(F.h);
  lastCycle();
  push(F.l);
}

void WDC65816::instructionPushD() {
  idle();
  pushN(D.h);
  lastCycle();
  pushN(D.l);
  if(r.e) S.h = 0x01;
}

void WDC65816::instructionPull8(r16& T) {
  idle();
  idle();
  lastCycle();
  T.l = pull();
  ZF = T.l == 0;
  NF = T.l & 0x80;
}

void WDC65816::instructionPull16(r16& T) {
  idle();
  idle();
  T.l = pull();
  lastCycle();
  T.h = pull();
  ZF = T.w == 0;
  NF = T.w & 0x8000;
}

void WDC65816::instructionPullD() {
  idle();
  idle();
  D.l = pullN();
  lastCycle();
  D.h = pullN();
  ZF = D.w == 0;
  NF = D.w & 0x8000;
  if(r.e) S.h = 0x01;
}

void WDC65816::instructionPullB() {
  idle();
  idle();
  lastCycle();
  B = pull();
  ZF = B == 0;
  NF = B & 0x80;
}

void WDC65816::instructionPullP() {
  idle();
  idle();
  lastCycle();
  P = pull();
  if(r.e) XF = 1, MF = 1;
  if(XF) X.h = 0x00, Y.h = 0x00;
}

void WDC65816::instructionPushEffectiveAddress() {
  W.l = fetch();
  W.h = fetch();
  pushN(W.h);
  lastCycle();
  pushN(W.l);
  if(r.e) S.h = 0x01;
}

void WDC65816::instructionPushEffectiveIndirectAddress() {
  U.l = fetch();
  idle2();
  W.l = readDirectN(U.l + 0);
  W.h = readDirectN(U.l + 1);
  pushN(W.h);
  lastCycle();
  pushN(W.l);
  if(r.e) S.h = 0x01;
}

void WDC65816::instructionPushEffectiveRelativeAddress() {
  V.l = fetch();
  V.h = fetch();
  idle();
  W.w = PC.d + (int16_t)V.w;
  pushN(W.h);
  lastCycle();
  pushN(W.l);
  if(r.e) S.h = 0x01;
}

void WDC65816::interrupt() {
  read(PC.d);
  idle();
  if(!r.e) push(PC.b);
  push(PC.h);
  push(PC.l);
  push(EF ? P & ~0x10 : P);
  IF = 1;
  DF = 0;
  PC.l = read(r.vector + 0);
  lastCycle();
  PC.h = read(r.vector + 1);
  PC.b = 0x00;
  idleJump();
}

//both the accumulator and index registers can independently be in either 8-bit or 16-bit mode.
//controlled via the M/X flags, this changes the execution details of various instructions.
//rather than implement four instruction tables for all possible combinations of these bits,
//instead use macro abuse to generate all four tables based off of a single template table.
void WDC65816::instruction() {
  //a = instructions unaffected by M/X flags
  //m = instructions affected by M flag (1 = 8-bit; 0 = 16-bit)
  //x = instructions affected by X flag (1 = 8-bit; 0 = 16-bit)

  #define opA(id, name, ...) case id: return instruction##name(__VA_ARGS__);
  if(MF) {
    #define opM(id, name, ...) case id: return instruction##name##8(__VA_ARGS__);
    #define m(name) &WDC65816::algorithm##name##8
    if(XF) {
      #define opX(id, name, ...) case id: return instruction##name##8(__VA_ARGS__);
      #define x(name) &WDC65816::algorithm##name##8
      #include "wdc65816_instruction.hpp"
      #undef opX
      #undef x
    } else {
      #define opX(id, name, ...) case id: return instruction##name##16(__VA_ARGS__);
      #define x(name) &WDC65816::algorithm##name##16
      #include "wdc65816_instruction.hpp"
      #undef opX
      #undef x
    }
    #undef opM
    #undef m
  } else {
    #define opM(id, name, ...) case id: return instruction##name##16(__VA_ARGS__);
    #define m(name) &WDC65816::algorithm##name##16
    if(XF) {
      #define opX(id, name, ...) case id: return instruction##name##8(__VA_ARGS__);
      #define x(name) &WDC65816::algorithm##name##8
      #include "wdc65816_instruction.hpp"
      #undef opX
      #undef x
    } else {
      #define opX(id, name, ...) case id: return instruction##name##16(__VA_ARGS__);
      #define x(name) &WDC65816::algorithm##name##16
      #include "wdc65816_instruction.hpp"
      #undef opX
      #undef x
    }
    #undef opM
    #undef m
  }
  #undef opA
}

void WDC65816::power() {
  r.pc.d = 0x000000;
  r.a  = 0x0000;
  r.x  = 0x0000;
  r.y  = 0x0000;
  r.s  = 0x01ff;
  r.d  = 0x0000;
  r.b  = 0x00;
  r.p  = 0x34;
  r.e  = 1;

  r.irq = 0;
  r.wai = 0;
  r.stp = 0;
  r.mar = 0x000000;
  r.mdr = 0x00;

  r.vector = 0xfffc;  //reset vector address
}

void WDC65816::serialize(serializer& s) {
  s.integer(r.pc.d);

  s.integer(r.a.w);
  s.integer(r.x.w);
  s.integer(r.y.w);
  s.integer(r.z.w);
  s.integer(r.s.w);
  s.integer(r.d.w);
  s.integer(r.b);

  s.integer(r.p.c);
  s.integer(r.p.z);
  s.integer(r.p.i);
  s.integer(r.p.d);
  s.integer(r.p.x);
  s.integer(r.p.m);
  s.integer(r.p.v);
  s.integer(r.p.n);

  s.integer(r.e);
  s.integer(r.irq);
  s.integer(r.wai);
  s.integer(r.stp);

  s.integer(r.vector);
  s.integer(r.mar);
  s.integer(r.mdr);

  s.integer(r.u.d);
  s.integer(r.v.d);
  s.integer(r.w.d);
}

// Fix this when nall is gone
/*nall::string WDC65816::disassemble() {
  return disassemble(r.pc.d, r.e, r.p.m, r.p.x);
}

nall::string WDC65816::disassemble(nall::Natural<24> address, bool e, bool m, bool x) {
  nall::string s;

  nall::Natural<24> pc = address;
  s = {nall::hex(pc, 6), "  "};

  nall::string name;
  nall::string operand;
  std::optional<nall::Natural<24>> effective;

  auto read = [&](nall::Natural<24> address) -> uint8_t {
    //$00-3f,80-bf:2000-5fff: do not attempt to read I/O registers from the disassembler:
    //this is because such reads are much more likely to have side effects to emulation.
    if((address & 0x40ffff) >= 0x2000 && (address & 0x40ffff) <= 0x5fff) return 0x00;
    return readDisassembler(address);
  };

  auto readByte = [&](nall::Natural<24> address) -> uint8_t {
    return read(address);
  };
  auto readWord = [&](nall::Natural<24> address) -> uint16_t {
    uint16_t data = readByte(address + 0) << 0;
    return data | readByte(address + 1) << 8;
  };
  auto readLong = [&](nall::Natural<24> address) -> nall::Natural<24> {
    nall::Natural<24> data = readByte(address + 0) << 0;
    return data | readWord(address + 1) << 8;
  };

  auto opcode   = read(address); address.bit(0,15)++;
  auto operand0 = read(address); address.bit(0,15)++;
  auto operand1 = read(address); address.bit(0,15)++;
  auto operand2 = read(address); address.bit(0,15)++;

  uint8_t operandByte = operand0 << 0;
  uint16_t operandWord = operand0 << 0 | operand1 << 8;
  nall::Natural<24> operandLong = operand0 << 0 | operand1 << 8 | operand2 << 16;

  auto absolute = [&]() -> nall::string {
    effective = r.b << 16 | operandWord;
    return {"$", nall::hex(operandWord, 4L)};
  };

  auto absolutePC = [&]() -> nall::string {
    effective = pc & 0xff0000 | operandWord;
    return {"$", nall::hex(operandWord, 4L)};
  };

  auto absoluteX = [&]() -> nall::string {
    effective = (r.b << 16) + operandWord + r.x.w;
    return {"$", nall::hex(operandWord, 4L), ",x"};
  };

  auto absoluteY = [&]() -> nall::string {
    effective = (r.b << 16) + operandWord + r.y.w;
    return {"$", nall::hex(operandWord, 4L), ",y"};
  };

  auto absoluteLong = [&]() -> nall::string {
    effective = operandLong;
    return {"$", nall::hex(operandLong, 6L)};
  };

  auto absoluteLongX = [&]() -> nall::string {
    effective = operandLong + r.x.w;
    return {"$", nall::hex(operandLong, 6L), ",x"};
  };

  auto direct = [&]() -> nall::string {
    effective = uint16_t(r.d.w + operandByte);
    return {"$", nall::hex(operandByte, 2L)};
  };

  auto directX = [&]() -> nall::string {
    effective = uint16_t(r.d.w + operandByte + r.x.w);
    return {"$", nall::hex(operandByte, 2L), ",x"};
  };

  auto directY = [&]() -> nall::string {
    effective = uint16_t(r.d.w + operandByte + r.y.w);
    return {"$", nall::hex(operandByte, 2L), ",y"};
  };

  auto immediate = [&]() -> nall::string {
    return {"#$", nall::hex(operandByte, 2L)};
  };

  auto immediateA = [&]() -> nall::string {
    return {"#$", m ? nall::hex(operandByte, 2L) : nall::hex(operandWord, 4L)};
  };

  auto immediateX = [&]() -> nall::string {
    return {"#$", x ? nall::hex(operandByte, 2L) : nall::hex(operandWord, 4L)};
  };

  auto implied = [&]() -> nall::string {
    return {};
  };

  auto indexedIndirectX = [&]() -> nall::string {
    effective = uint16_t(r.d.w + operandByte + r.x.w);
    effective = r.b << 16 | readWord(*effective);
    return {"($", nall::hex(operandByte, 2L), ",x)"};
  };

  auto indirect = [&]() -> nall::string {
    effective = uint16_t(r.d.w + operandByte);
    effective = (r.b << 16) + readWord(*effective);
    return {"($", nall::hex(operandByte, 2L), ")"};
  };

  auto indirectPC = [&]() -> nall::string {
    effective = operandWord;
    effective = pc & 0xff0000 | readWord(*effective);
    return {"($", nall::hex(operandWord, 4L), ")"};
  };

  auto indirectX = [&]() -> nall::string {
    effective = operandWord;
    effective = pc & 0xff0000 | uint16_t(*effective + r.x.w);
    effective = pc & 0xff0000 | readWord(*effective);
    return {"($", nall::hex(operandWord, 4L), ",x)"};
  };

  auto indirectIndexedY = [&]() -> nall::string {
    effective = uint16_t(r.d.w + operandByte);
    effective = (r.b << 16) + readWord(*effective) + r.y.w;
    return {"($", nall::hex(operandByte, 2L), "),y"};
  };

  auto indirectLong = [&]() -> nall::string {
    effective = uint16_t(r.d.w + operandByte);
    effective = readLong(*effective);
    return {"[$", nall::hex(operandByte, 2L), "]"};
  };

  auto indirectLongPC = [&]() -> nall::string {
    effective = readLong(operandWord);
    return {"[$", nall::hex(operandWord, 4L), "]"};
  };

  auto indirectLongY = [&]() -> nall::string {
    effective = uint16_t(r.d.w + operandByte);
    effective = readLong(*effective) + r.y.w;
    return {"[$", nall::hex(operandByte, 2L), "],y"};
  };

  auto move = [&]() -> nall::string {
    return {"$", nall::hex(operand0, 2L), "=$", nall::hex(operand1, 2L)};
  };

  auto relative = [&]() -> nall::string {
    effective = pc & 0xff0000 | uint16_t(pc + 2 + (int8_t)operandByte);
    return {"$", nall::hex(*effective, 4L)};
  };

  auto relativeWord = [&]() -> nall::string {
    effective = pc & 0xff0000 | uint16_t(pc + 3 + (int16_t)operandWord);
    return {"$", nall::hex(*effective, 4L)};
  };

  auto stack = [&]() -> nall::string {
    effective = uint16_t(r.s.w + operandByte);
    return {"$", nall::hex(operandByte, 2L), ",s"};
  };

  auto stackIndirect = [&]() -> nall::string {
    effective = uint16_t(operandByte + r.s.w);
    effective = (r.b << 16) + readWord(*effective) + r.y.w;
    return {"($", nall::hex(operandByte, 2L), ",s),y"};
  };

  #define op(id, label, function) case id: name = label; operand = function(); break;
  switch(opcode) {
  op(0x00, "brk", immediate)
  op(0x01, "ora", indexedIndirectX)
  op(0x02, "cop", immediate)
  op(0x03, "ora", stack)
  op(0x04, "tsb", direct)
  op(0x05, "ora", direct)
  op(0x06, "asl", direct)
  op(0x07, "ora", indirectLong)
  op(0x08, "php", implied)
  op(0x09, "ora", immediateA)
  op(0x0a, "asl", implied)
  op(0x0b, "phd", implied)
  op(0x0c, "tsb", absolute)
  op(0x0d, "ora", absolute)
  op(0x0e, "asl", absolute)
  op(0x0f, "ora", absoluteLong)
  op(0x10, "bpl", relative)
  op(0x11, "ora", indirectIndexedY)
  op(0x12, "ora", indirect)
  op(0x13, "ora", stackIndirect)
  op(0x14, "trb", direct)
  op(0x15, "ora", directX)
  op(0x16, "asl", directX)
  op(0x17, "ora", indirectLongY)
  op(0x18, "clc", implied)
  op(0x19, "ora", absoluteY)
  op(0x1a, "inc", implied)
  op(0x1b, "tas", implied)
  op(0x1c, "trb", absolute)
  op(0x1d, "ora", absoluteX)
  op(0x1e, "asl", absoluteX)
  op(0x1f, "ora", absoluteLongX)

  op(0x20, "jsr", absolutePC)
  op(0x21, "and", indexedIndirectX)
  op(0x22, "jsl", absoluteLong)
  op(0x23, "and", stack)
  op(0x24, "bit", direct)
  op(0x25, "and", direct)
  op(0x26, "rol", direct)
  op(0x27, "and", indirectLong)
  op(0x28, "plp", implied)
  op(0x29, "and", immediateA)
  op(0x2a, "rol", implied)
  op(0x2b, "pld", implied)
  op(0x2c, "bit", absolute)
  op(0x2d, "and", absolute)
  op(0x2e, "rol", absolute)
  op(0x2f, "and", absoluteLong)
  op(0x30, "bmi", relative)
  op(0x31, "and", indirectIndexedY)
  op(0x32, "and", indirect)
  op(0x33, "and", stackIndirect)
  op(0x34, "bit", directX)
  op(0x35, "and", directX)
  op(0x36, "rol", directX)
  op(0x37, "and", indirectLongY)
  op(0x38, "sec", implied)
  op(0x39, "and", absoluteY)
  op(0x3a, "dec", implied)
  op(0x3b, "tsa", implied)
  op(0x3c, "bit", absoluteX)
  op(0x3d, "and", absoluteX)
  op(0x3e, "rol", absoluteX)
  op(0x3f, "and", absoluteLongX)

  op(0x40, "rti", implied)
  op(0x41, "eor", indexedIndirectX)
  op(0x42, "wdm", immediate)
  op(0x43, "eor", stack)
  op(0x44, "mvp", move)
  op(0x45, "eor", direct)
  op(0x46, "lsr", direct)
  op(0x47, "eor", indirectLong)
  op(0x48, "pha", implied)
  op(0x49, "eor", immediateA)
  op(0x4a, "lsr", implied)
  op(0x4b, "phk", implied)
  op(0x4c, "jmp", absolutePC)
  op(0x4d, "eor", absolute)
  op(0x4e, "lsr", absolute)
  op(0x4f, "eor", absoluteLong)
  op(0x50, "bvc", relative)
  op(0x51, "eor", indirectIndexedY)
  op(0x52, "eor", indirect)
  op(0x53, "eor", stackIndirect)
  op(0x54, "mvn", move)
  op(0x55, "eor", directX)
  op(0x56, "lsr", directX)
  op(0x57, "eor", indirectLongY)
  op(0x58, "cli", implied)
  op(0x59, "eor", absoluteY)
  op(0x5a, "phy", implied)
  op(0x5b, "tad", implied)
  op(0x5c, "jml", absoluteLong)
  op(0x5d, "eor", absoluteX)
  op(0x5e, "lsr", absoluteX)
  op(0x5f, "eor", absoluteLongX)

  op(0x60, "rts", implied)
  op(0x61, "adc", indexedIndirectX)
  op(0x62, "per", absolute)
  op(0x63, "adc", stack)
  op(0x64, "stz", direct)
  op(0x65, "adc", direct)
  op(0x66, "ror", direct)
  op(0x67, "adc", indirectLong)
  op(0x68, "pla", implied)
  op(0x69, "adc", immediateA)
  op(0x6a, "ror", implied)
  op(0x6b, "rtl", implied)
  op(0x6c, "jmp", indirectPC)
  op(0x6d, "adc", absolute)
  op(0x6e, "ror", absolute)
  op(0x6f, "adc", absoluteLong)
  op(0x70, "bvs", relative)
  op(0x71, "adc", indirectIndexedY)
  op(0x72, "adc", indirect)
  op(0x73, "adc", stackIndirect)
  op(0x74, "stz", directX)
  op(0x75, "adc", directX)
  op(0x76, "ror", directX)
  op(0x77, "adc", indirectLongY)
  op(0x78, "sei", implied)
  op(0x79, "adc", absoluteY)
  op(0x7a, "ply", implied)
  op(0x7b, "tda", implied)
  op(0x7c, "jmp", indirectX)
  op(0x7d, "adc", absoluteX)
  op(0x7e, "ror", absoluteX)
  op(0x7f, "adc", absoluteLongX)

  op(0x80, "bra", relative)
  op(0x81, "sta", indexedIndirectX)
  op(0x82, "brl", relativeWord)
  op(0x83, "sta", stack)
  op(0x84, "sty", direct)
  op(0x85, "sta", direct)
  op(0x86, "stx", direct)
  op(0x87, "sta", indirectLong)
  op(0x88, "dey", implied)
  op(0x89, "bit", immediateA)
  op(0x8a, "txa", implied)
  op(0x8b, "phb", implied)
  op(0x8c, "sty", absolute)
  op(0x8d, "sta", absolute)
  op(0x8e, "stx", absolute)
  op(0x8f, "sta", absoluteLong)
  op(0x90, "bcc", relative)
  op(0x91, "sta", indirectIndexedY)
  op(0x92, "sta", indirect)
  op(0x93, "sta", stackIndirect)
  op(0x94, "sty", directX)
  op(0x95, "sta", directX)
  op(0x96, "stx", directY)
  op(0x97, "sta", indirectLongY)
  op(0x98, "tya", implied)
  op(0x99, "sta", absoluteY)
  op(0x9a, "txs", implied)
  op(0x9b, "txy", implied)
  op(0x9c, "stz", absolute)
  op(0x9d, "sta", absoluteX)
  op(0x9e, "stz", absoluteX)
  op(0x9f, "sta", absoluteLongX)

  op(0xa0, "ldy", immediateX)
  op(0xa1, "lda", indexedIndirectX)
  op(0xa2, "ldx", immediateX)
  op(0xa3, "lda", stack)
  op(0xa4, "ldy", direct)
  op(0xa5, "lda", direct)
  op(0xa6, "ldx", direct)
  op(0xa7, "lda", indirectLong)
  op(0xa8, "tay", implied)
  op(0xa9, "lda", immediateA)
  op(0xaa, "tax", implied)
  op(0xab, "plb", implied)
  op(0xac, "ldy", absolute)
  op(0xad, "lda", absolute)
  op(0xae, "ldx", absolute)
  op(0xaf, "lda", absoluteLong)
  op(0xb0, "bcs", relative)
  op(0xb1, "lda", indirectIndexedY)
  op(0xb2, "lda", indirect)
  op(0xb3, "lda", stackIndirect)
  op(0xb4, "ldy", directX)
  op(0xb5, "lda", directX)
  op(0xb6, "ldx", directY)
  op(0xb7, "lda", indirectLongY)
  op(0xb8, "clv", implied)
  op(0xb9, "lda", absoluteY)
  op(0xba, "tsx", implied)
  op(0xbb, "tyx", implied)
  op(0xbc, "ldy", absoluteX)
  op(0xbd, "lda", absoluteX)
  op(0xbe, "ldx", absoluteY)
  op(0xbf, "lda", absoluteLongX)

  op(0xc0, "cpy", immediateX)
  op(0xc1, "cmp", indexedIndirectX)
  op(0xc2, "rep", immediate)
  op(0xc3, "cmp", stack)
  op(0xc4, "cpy", direct)
  op(0xc5, "cmp", direct)
  op(0xc6, "dec", direct)
  op(0xc7, "cmp", indirectLong)
  op(0xc8, "iny", implied)
  op(0xc9, "cmp", immediateA)
  op(0xca, "dex", implied)
  op(0xcb, "wai", implied)
  op(0xcc, "cpy", absolute)
  op(0xcd, "cmp", absolute)
  op(0xce, "dec", absolute)
  op(0xcf, "cmp", absoluteLong)
  op(0xd0, "bne", relative)
  op(0xd1, "cmp", indirectIndexedY)
  op(0xd2, "cmp", indirect)
  op(0xd3, "cmp", stackIndirect)
  op(0xd4, "pei", indirect)
  op(0xd5, "cmp", directX)
  op(0xd6, "dec", directX)
  op(0xd7, "cmp", indirectLongY)
  op(0xd8, "cld", implied)
  op(0xd9, "cmp", absoluteY)
  op(0xda, "phx", implied)
  op(0xdb, "stp", implied)
  op(0xdc, "jmp", indirectLongPC)
  op(0xdd, "cmp", absoluteX)
  op(0xde, "dec", absoluteX)
  op(0xdf, "cmp", absoluteLongX)

  op(0xe0, "cpx", immediateX)
  op(0xe1, "sbc", indexedIndirectX)
  op(0xe2, "sep", immediate)
  op(0xe3, "sbc", stack)
  op(0xe4, "cpx", direct)
  op(0xe5, "sbc", direct)
  op(0xe6, "inc", direct)
  op(0xe7, "sbc", indirectLong)
  op(0xe8, "inx", implied)
  op(0xe9, "sbc", immediateA)
  op(0xea, "nop", implied)
  op(0xeb, "xba", implied)
  op(0xec, "cpx", absolute)
  op(0xed, "sbc", absolute)
  op(0xee, "inc", absolute)
  op(0xef, "sbc", absoluteLong)
  op(0xf0, "beq", relative)
  op(0xf1, "sbc", indirectIndexedY)
  op(0xf2, "sbc", indirect)
  op(0xf3, "sbc", stackIndirect)
  op(0xf4, "pea", absolute)
  op(0xf5, "sbc", directX)
  op(0xf6, "inc", directX)
  op(0xf7, "sbc", indirectLongY)
  op(0xf8, "sed", implied)
  op(0xf9, "sbc", absoluteY)
  op(0xfa, "plx", implied)
  op(0xfb, "xce", implied)
  op(0xfc, "jsr", indirectX)
  op(0xfd, "sbc", absoluteX)
  op(0xfe, "inc", absoluteX)
  op(0xff, "sbc", absoluteLongX)
  }
  #undef op

  s.append(name, " ", operand);
  while(s.size() < 23) s.append(" ");
  if(effective) s.append("[", nall::hex(*effective, 6L), "]");
  while(s.size() < 31) s.append(" ");

  s.append(" A:", nall::hex(r.a.w, 4L));
  s.append(" X:", nall::hex(r.x.w, 4L));
  s.append(" Y:", nall::hex(r.y.w, 4L));
  s.append(" S:", nall::hex(r.s.w, 4L));
  s.append(" D:", nall::hex(r.d.w, 4L));
  s.append(" B:", nall::hex(r.b  , 2L));

  if(e) {
    s.append(' ',
      r.p.n ? 'N' : 'n', r.p.v ? 'V' : 'v',
      r.p.m ? '1' : '0', r.p.x ? 'B' : 'b',
      r.p.d ? 'D' : 'd', r.p.i ? 'I' : 'i',
      r.p.z ? 'Z' : 'z', r.p.c ? 'C' : 'c'
    );
  } else {
    s.append(' ',
      r.p.n ? 'N' : 'n', r.p.v ? 'V' : 'v',
      r.p.m ? 'M' : 'm', r.p.x ? 'X' : 'x',
      r.p.d ? 'D' : 'd', r.p.i ? 'I' : 'i',
      r.p.z ? 'Z' : 'z', r.p.c ? 'C' : 'c'
    );
  }

  return s;
}*/

}
