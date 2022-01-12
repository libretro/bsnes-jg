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

#include "../serializer.hpp"

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
  if(D.r16_lsb2.l) idle();
}

void WDC65816::idle4(uint16_t x, uint16_t y) {
  if(!XF || x >> 8 != y >> 8) idle();
}

void WDC65816::idle6(uint16_t address) {
  if(EF && PC.r24_lsb4.h != address >> 8) idle();
}

uint8_t WDC65816::fetch() {
  return read(PC.r24_lsb4.b << 16 | PC.r24_lsb2.w++);
}

uint8_t WDC65816::pull() {
  EF ? (void)S.r16_lsb2.l++ : (void)S.w++;
  return read(S.w);
}

void WDC65816::push(uint8_t data) {
  write(S.w, data);
  EF ? (void)S.r16_lsb2.l-- : (void)S.w--;
}

uint8_t WDC65816::pullN() {
  return read(++S.w);
}

void WDC65816::pushN(uint8_t data) {
  write(S.w--, data);
}

uint8_t WDC65816::readDirect(unsigned address) {
  if(EF && !D.r16_lsb2.l) return read(D.w | (address & 0xff));
  return read((D.w + address) & 0xffff);
}

void WDC65816::writeDirect(unsigned address, uint8_t data) {
  if(EF && !D.r16_lsb2.l) return write(D.w | (address & 0xff), data);
  write((D.w + address) & 0xffff, data);
}

uint8_t WDC65816::readDirectN(unsigned address) {
  return read((D.w + address) & 0xffff);
}

uint8_t WDC65816::readBank(unsigned address) {
  return read(((B << 16) + address) & 0xffffff);
}

void WDC65816::writeBank(unsigned address, uint8_t data) {
  write(((B << 16) + address) & 0xffffff, data);
}

uint8_t WDC65816::readLong(unsigned address) {
  return read(address & 0xffffff);
}

void WDC65816::writeLong(unsigned address, uint8_t data) {
  write(address & 0xffffff, data);
}

uint8_t WDC65816::readStack(unsigned address) {
  return read((S.w + address) & 0xffff);
}

void WDC65816::writeStack(unsigned address, uint8_t data) {
  write((S.w + address) & 0xffff, data);
}

uint8_t WDC65816::algorithmADC8(uint8_t data) {
  int result;

  if(!DF) {
    result = A.r16_lsb2.l + data + CF;
  } else {
    result = (A.r16_lsb2.l & 0x0f) + (data & 0x0f) + (CF << 0);
    if(result > 0x09) result += 0x06;
    CF = result > 0x0f;
    result = (A.r16_lsb2.l & 0xf0) + (data & 0xf0) + (CF << 4) + (result & 0x0f);
  }

  VF = ~(A.r16_lsb2.l ^ data) & (A.r16_lsb2.l ^ result) & 0x80;
  if(DF && result > 0x9f) result += 0x60;
  CF = result > 0xff;
  ZF = (uint8_t)result == 0;
  NF = result & 0x80;

  return A.r16_lsb2.l = result;
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
  A.r16_lsb2.l &= data;
  ZF = A.r16_lsb2.l == 0;
  NF = A.r16_lsb2.l & 0x80;
  return A.r16_lsb2.l;
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
  ZF = (data & A.r16_lsb2.l) == 0;
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
  int result = A.r16_lsb2.l - data;
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
  int result = X.r16_lsb2.l - data;
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
  int result = Y.r16_lsb2.l - data;
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
  A.r16_lsb2.l ^= data;
  ZF = A.r16_lsb2.l == 0;
  NF = A.r16_lsb2.l & 0x80;
  return A.r16_lsb2.l;
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
  A.r16_lsb2.l = data;
  ZF = A.r16_lsb2.l == 0;
  NF = A.r16_lsb2.l & 0x80;
  return data;
}

uint16_t WDC65816::algorithmLDA16(uint16_t data) {
  A.w = data;
  ZF = A.w == 0;
  NF = A.w & 0x8000;
  return data;
}

uint8_t WDC65816::algorithmLDX8(uint8_t data) {
  X.r16_lsb2.l = data;
  ZF = X.r16_lsb2.l == 0;
  NF = X.r16_lsb2.l & 0x80;
  return data;
}

uint16_t WDC65816::algorithmLDX16(uint16_t data) {
  X.w = data;
  ZF = X.w == 0;
  NF = X.w & 0x8000;
  return data;
}

uint8_t WDC65816::algorithmLDY8(uint8_t data) {
  Y.r16_lsb2.l = data;
  ZF = Y.r16_lsb2.l == 0;
  NF = Y.r16_lsb2.l & 0x80;
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
  A.r16_lsb2.l |= data;
  ZF = A.r16_lsb2.l == 0;
  NF = A.r16_lsb2.l & 0x80;
  return A.r16_lsb2.l;
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
    result = A.r16_lsb2.l + data + CF;
  } else {
    result = (A.r16_lsb2.l & 0x0f) + (data & 0x0f) + (CF << 0);
    if(result <= 0x0f) result -= 0x06;
    CF = result > 0x0f;
    result = (A.r16_lsb2.l & 0xf0) + (data & 0xf0) + (CF << 4) + (result & 0x0f);
  }

  VF = ~(A.r16_lsb2.l ^ data) & (A.r16_lsb2.l ^ result) & 0x80;
  if(DF && result <= 0xff) result -= 0x60;
  CF = result > 0xff;
  ZF = (uint8_t)result == 0;
  NF = result & 0x80;

  return A.r16_lsb2.l = result;
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
  ZF = (data & A.r16_lsb2.l) == 0;
  data &= ~A.r16_lsb2.l;
  return data;
}

uint16_t WDC65816::algorithmTRB16(uint16_t data) {
  ZF = (data & A.w) == 0;
  data &= ~A.w;
  return data;
}

uint8_t WDC65816::algorithmTSB8(uint8_t data) {
  ZF = (data & A.r16_lsb2.l) == 0;
  data |= A.r16_lsb2.l;
  return data;
}

uint16_t WDC65816::algorithmTSB16(uint16_t data) {
  ZF = (data & A.w) == 0;
  data |= A.w;
  return data;
}

void WDC65816::instructionImmediateRead8(alu8 op) {
  lastCycle();
  W.r24_lsb4.l = fetch();
  alu(W.r24_lsb4.l);
}

void WDC65816::instructionImmediateRead16(alu16 op) {
  W.r24_lsb4.l = fetch();
  lastCycle();
  W.r24_lsb4.h = fetch();
  alu(W.r24_lsb2.w);
}

void WDC65816::instructionBankRead8(alu8 op) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  lastCycle();
  W.r24_lsb4.l = readBank(V.r24_lsb2.w + 0);
  alu(W.r24_lsb4.l);
}

void WDC65816::instructionBankRead16(alu16 op) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  W.r24_lsb4.l = readBank(V.r24_lsb2.w + 0);
  lastCycle();
  W.r24_lsb4.h = readBank(V.r24_lsb2.w + 1);
  alu(W.r24_lsb2.w);
}

void WDC65816::instructionBankRead8(alu8 op, r16 I) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  idle4(V.r24_lsb2.w, V.r24_lsb2.w + I.w);
  lastCycle();
  W.r24_lsb4.l = readBank(V.r24_lsb2.w + I.w + 0);
  alu(W.r24_lsb4.l);
}

void WDC65816::instructionBankRead16(alu16 op, r16 I) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  idle4(V.r24_lsb2.w, V.r24_lsb2.w + I.w);
  W.r24_lsb4.l = readBank(V.r24_lsb2.w + I.w + 0);
  lastCycle();
  W.r24_lsb4.h = readBank(V.r24_lsb2.w + I.w + 1);
  alu(W.r24_lsb2.w);
}

void WDC65816::instructionLongRead8(alu8 op, r16 I) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  V.r24_lsb4.b = fetch();
  lastCycle();
  W.r24_lsb4.l = readLong(V.d + I.w + 0);
  alu(W.r24_lsb4.l);
}

void WDC65816::instructionLongRead16(alu16 op, r16 I) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  V.r24_lsb4.b = fetch();
  W.r24_lsb4.l = readLong(V.d + I.w + 0);
  lastCycle();
  W.r24_lsb4.h = readLong(V.d + I.w + 1);
  alu(W.r24_lsb2.w);
}

void WDC65816::instructionDirectRead8(alu8 op) {
  U.r24_lsb4.l = fetch();
  idle2();
  lastCycle();
  W.r24_lsb4.l = readDirect(U.r24_lsb4.l + 0);
  alu(W.r24_lsb4.l);
}

void WDC65816::instructionDirectRead16(alu16 op) {
  U.r24_lsb4.l = fetch();
  idle2();
  W.r24_lsb4.l = readDirect(U.r24_lsb4.l + 0);
  lastCycle();
  W.r24_lsb4.h = readDirect(U.r24_lsb4.l + 1);
  alu(W.r24_lsb2.w);
}

void WDC65816::instructionDirectRead8(alu8 op, r16 I) {
  U.r24_lsb4.l = fetch();
  idle2();
  idle();
  lastCycle();
  W.r24_lsb4.l = readDirect(U.r24_lsb4.l + I.w + 0);
  alu(W.r24_lsb4.l);
}

void WDC65816::instructionDirectRead16(alu16 op, r16 I) {
  U.r24_lsb4.l = fetch();
  idle2();
  idle();
  W.r24_lsb4.l = readDirect(U.r24_lsb4.l + I.w + 0);
  lastCycle();
  W.r24_lsb4.h = readDirect(U.r24_lsb4.l + I.w + 1);
  alu(W.r24_lsb2.w);
}

void WDC65816::instructionIndirectRead8(alu8 op) {
  U.r24_lsb4.l = fetch();
  idle2();
  V.r24_lsb4.l = readDirect(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readDirect(U.r24_lsb4.l + 1);
  lastCycle();
  W.r24_lsb4.l = readBank(V.r24_lsb2.w + 0);
  alu(W.r24_lsb4.l);
}

void WDC65816::instructionIndirectRead16(alu16 op) {
  U.r24_lsb4.l = fetch();
  idle2();
  V.r24_lsb4.l = readDirect(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readDirect(U.r24_lsb4.l + 1);
  W.r24_lsb4.l = readBank(V.r24_lsb2.w + 0);
  lastCycle();
  W.r24_lsb4.h = readBank(V.r24_lsb2.w + 1);
  alu(W.r24_lsb2.w);
}

void WDC65816::instructionIndexedIndirectRead8(alu8 op) {
  U.r24_lsb4.l = fetch();
  idle2();
  idle();
  V.r24_lsb4.l = readDirect(U.r24_lsb4.l + X.w + 0);
  V.r24_lsb4.h = readDirect(U.r24_lsb4.l + X.w + 1);
  lastCycle();
  W.r24_lsb4.l = readBank(V.r24_lsb2.w + 0);
  alu(W.r24_lsb4.l);
}

void WDC65816::instructionIndexedIndirectRead16(alu16 op) {
  U.r24_lsb4.l = fetch();
  idle2();
  idle();
  V.r24_lsb4.l = readDirect(U.r24_lsb4.l + X.w + 0);
  V.r24_lsb4.h = readDirect(U.r24_lsb4.l + X.w + 1);
  W.r24_lsb4.l = readBank(V.r24_lsb2.w + 0);
  lastCycle();
  W.r24_lsb4.h = readBank(V.r24_lsb2.w + 1);
  alu(W.r24_lsb2.w);
}

void WDC65816::instructionIndirectIndexedRead8(alu8 op) {
  U.r24_lsb4.l = fetch();
  idle2();
  V.r24_lsb4.l = readDirect(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readDirect(U.r24_lsb4.l + 1);
  idle4(V.r24_lsb2.w, V.r24_lsb2.w + Y.w);
  lastCycle();
  W.r24_lsb4.l = readBank(V.r24_lsb2.w + Y.w + 0);
  alu(W.r24_lsb4.l);
}

void WDC65816::instructionIndirectIndexedRead16(alu16 op) {
  U.r24_lsb4.l = fetch();
  idle2();
  V.r24_lsb4.l = readDirect(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readDirect(U.r24_lsb4.l + 1);
  idle4(V.r24_lsb2.w, V.r24_lsb2.w + Y.w);
  W.r24_lsb4.l = readBank(V.r24_lsb2.w + Y.w + 0);
  lastCycle();
  W.r24_lsb4.h = readBank(V.r24_lsb2.w + Y.w + 1);
  alu(W.r24_lsb2.w);
}

void WDC65816::instructionIndirectLongRead8(alu8 op, r16 I) {
  U.r24_lsb4.l = fetch();
  idle2();
  V.r24_lsb4.l = readDirectN(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readDirectN(U.r24_lsb4.l + 1);
  V.r24_lsb4.b = readDirectN(U.r24_lsb4.l + 2);
  lastCycle();
  W.r24_lsb4.l = readLong(V.d + I.w + 0);
  alu(W.r24_lsb4.l);
}

void WDC65816::instructionIndirectLongRead16(alu16 op, r16 I) {
  U.r24_lsb4.l = fetch();
  idle2();
  V.r24_lsb4.l = readDirectN(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readDirectN(U.r24_lsb4.l + 1);
  V.r24_lsb4.b = readDirectN(U.r24_lsb4.l + 2);
  W.r24_lsb4.l = readLong(V.d + I.w + 0);
  lastCycle();
  W.r24_lsb4.h = readLong(V.d + I.w + 1);
  alu(W.r24_lsb2.w);
}

void WDC65816::instructionStackRead8(alu8 op) {
  U.r24_lsb4.l = fetch();
  idle();
  lastCycle();
  W.r24_lsb4.l = readStack(U.r24_lsb4.l + 0);
  alu(W.r24_lsb4.l);
}

void WDC65816::instructionStackRead16(alu16 op) {
  U.r24_lsb4.l = fetch();
  idle();
  W.r24_lsb4.l = readStack(U.r24_lsb4.l + 0);
  lastCycle();
  W.r24_lsb4.h = readStack(U.r24_lsb4.l + 1);
  alu(W.r24_lsb2.w);
}

void WDC65816::instructionIndirectStackRead8(alu8 op) {
  U.r24_lsb4.l = fetch();
  idle();
  V.r24_lsb4.l = readStack(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readStack(U.r24_lsb4.l + 1);
  idle();
  lastCycle();
  W.r24_lsb4.l = readBank(V.r24_lsb2.w + Y.w + 0);
  alu(W.r24_lsb4.l);
}

void WDC65816::instructionIndirectStackRead16(alu16 op) {
  U.r24_lsb4.l = fetch();
  idle();
  V.r24_lsb4.l = readStack(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readStack(U.r24_lsb4.l + 1);
  idle();
  W.r24_lsb4.l = readBank(V.r24_lsb2.w + Y.w + 0);
  lastCycle();
  W.r24_lsb4.h = readBank(V.r24_lsb2.w + Y.w + 1);
  alu(W.r24_lsb2.w);
}

void WDC65816::instructionBankWrite8(r16 F) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  lastCycle();
  writeBank(V.r24_lsb2.w + 0, F.r16_lsb2.l);
}

void WDC65816::instructionBankWrite16(r16 F) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  writeBank(V.r24_lsb2.w + 0, F.r16_lsb2.l);
  lastCycle();
  writeBank(V.r24_lsb2.w + 1, F.r16_lsb2.h);
}

void WDC65816::instructionBankWrite8(r16 F, r16 I) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  idle();
  lastCycle();
  writeBank(V.r24_lsb2.w + I.w + 0, F.r16_lsb2.l);
}

void WDC65816::instructionBankWrite16(r16 F, r16 I) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  idle();
  writeBank(V.r24_lsb2.w + I.w + 0, F.r16_lsb2.l);
  lastCycle();
  writeBank(V.r24_lsb2.w + I.w + 1, F.r16_lsb2.h);
}

void WDC65816::instructionLongWrite8(r16 I) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  V.r24_lsb4.b = fetch();
  lastCycle();
  writeLong(V.d + I.w + 0, A.r16_lsb2.l);
}

void WDC65816::instructionLongWrite16(r16 I) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  V.r24_lsb4.b = fetch();
  writeLong(V.d + I.w + 0, A.r16_lsb2.l);
  lastCycle();
  writeLong(V.d + I.w + 1, A.r16_lsb2.h);
}

void WDC65816::instructionDirectWrite8(r16 F) {
  U.r24_lsb4.l = fetch();
  idle2();
  lastCycle();
  writeDirect(U.r24_lsb4.l + 0, F.r16_lsb2.l);
}

void WDC65816::instructionDirectWrite16(r16 F) {
  U.r24_lsb4.l = fetch();
  idle2();
  writeDirect(U.r24_lsb4.l + 0, F.r16_lsb2.l);
  lastCycle();
  writeDirect(U.r24_lsb4.l + 1, F.r16_lsb2.h);
}

void WDC65816::instructionDirectWrite8(r16 F, r16 I) {
  U.r24_lsb4.l = fetch();
  idle2();
  idle();
  lastCycle();
  writeDirect(U.r24_lsb4.l + I.w + 0, F.r16_lsb2.l);
}

void WDC65816::instructionDirectWrite16(r16 F, r16 I) {
  U.r24_lsb4.l = fetch();
  idle2();
  idle();
  writeDirect(U.r24_lsb4.l + I.w + 0, F.r16_lsb2.l);
  lastCycle();
  writeDirect(U.r24_lsb4.l + I.w + 1, F.r16_lsb2.h);
}

void WDC65816::instructionIndirectWrite8() {
  U.r24_lsb4.l = fetch();
  idle2();
  V.r24_lsb4.l = readDirect(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readDirect(U.r24_lsb4.l + 1);
  lastCycle();
  writeBank(V.r24_lsb2.w + 0, A.r16_lsb2.l);
}

void WDC65816::instructionIndirectWrite16() {
  U.r24_lsb4.l = fetch();
  idle2();
  V.r24_lsb4.l = readDirect(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readDirect(U.r24_lsb4.l + 1);
  writeBank(V.r24_lsb2.w + 0, A.r16_lsb2.l);
  lastCycle();
  writeBank(V.r24_lsb2.w + 1, A.r16_lsb2.h);
}

void WDC65816::instructionIndexedIndirectWrite8() {
  U.r24_lsb4.l = fetch();
  idle2();
  idle();
  V.r24_lsb4.l = readDirect(U.r24_lsb4.l + X.w + 0);
  V.r24_lsb4.h = readDirect(U.r24_lsb4.l + X.w + 1);
  lastCycle();
  writeBank(V.r24_lsb2.w + 0, A.r16_lsb2.l);
}

void WDC65816::instructionIndexedIndirectWrite16() {
  U.r24_lsb4.l = fetch();
  idle2();
  idle();
  V.r24_lsb4.l = readDirect(U.r24_lsb4.l + X.w + 0);
  V.r24_lsb4.h = readDirect(U.r24_lsb4.l + X.w + 1);
  writeBank(V.r24_lsb2.w + 0, A.r16_lsb2.l);
  lastCycle();
  writeBank(V.r24_lsb2.w + 1, A.r16_lsb2.h);
}

void WDC65816::instructionIndirectIndexedWrite8() {
  U.r24_lsb4.l = fetch();
  idle2();
  V.r24_lsb4.l = readDirect(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readDirect(U.r24_lsb4.l + 1);
  idle();
  lastCycle();
  writeBank(V.r24_lsb2.w + Y.w + 0, A.r16_lsb2.l);
}

void WDC65816::instructionIndirectIndexedWrite16() {
  U.r24_lsb4.l = fetch();
  idle2();
  V.r24_lsb4.l = readDirect(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readDirect(U.r24_lsb4.l + 1);
  idle();
  writeBank(V.r24_lsb2.w + Y.w + 0, A.r16_lsb2.l);
  lastCycle();
  writeBank(V.r24_lsb2.w + Y.w + 1, A.r16_lsb2.h);
}

void WDC65816::instructionIndirectLongWrite8(r16 I) {
  U.r24_lsb4.l = fetch();
  idle2();
  V.r24_lsb4.l = readDirectN(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readDirectN(U.r24_lsb4.l + 1);
  V.r24_lsb4.b = readDirectN(U.r24_lsb4.l + 2);
  lastCycle();
  writeLong(V.d + I.w + 0, A.r16_lsb2.l);
}

void WDC65816::instructionIndirectLongWrite16(r16 I) {
  U.r24_lsb4.l = fetch();
  idle2();
  V.r24_lsb4.l = readDirectN(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readDirectN(U.r24_lsb4.l + 1);
  V.r24_lsb4.b = readDirectN(U.r24_lsb4.l + 2);
  writeLong(V.d + I.w + 0, A.r16_lsb2.l);
  lastCycle();
  writeLong(V.d + I.w + 1, A.r16_lsb2.h);
}

void WDC65816::instructionStackWrite8() {
  U.r24_lsb4.l = fetch();
  idle();
  lastCycle();
  writeStack(U.r24_lsb4.l + 0, A.r16_lsb2.l);
}

void WDC65816::instructionStackWrite16() {
  U.r24_lsb4.l = fetch();
  idle();
  writeStack(U.r24_lsb4.l + 0, A.r16_lsb2.l);
  lastCycle();
  writeStack(U.r24_lsb4.l + 1, A.r16_lsb2.h);
}

void WDC65816::instructionIndirectStackWrite8() {
  U.r24_lsb4.l = fetch();
  idle();
  V.r24_lsb4.l = readStack(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readStack(U.r24_lsb4.l + 1);
  idle();
  lastCycle();
  writeBank(V.r24_lsb2.w + Y.w + 0, A.r16_lsb2.l);
}

void WDC65816::instructionIndirectStackWrite16() {
  U.r24_lsb4.l = fetch();
  idle();
  V.r24_lsb4.l = readStack(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readStack(U.r24_lsb4.l + 1);
  idle();
  writeBank(V.r24_lsb2.w + Y.w + 0, A.r16_lsb2.l);
  lastCycle();
  writeBank(V.r24_lsb2.w + Y.w + 1, A.r16_lsb2.h);
}

void WDC65816::instructionImpliedModify8(alu8 op, r16& M) {
  lastCycle();
  idleIRQ();
  M.r16_lsb2.l = alu(M.r16_lsb2.l);
}

void WDC65816::instructionImpliedModify16(alu16 op, r16& M) {
  lastCycle();
  idleIRQ();
  M.w = alu(M.w);
}

void WDC65816::instructionBankModify8(alu8 op) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  W.r24_lsb4.l = readBank(V.r24_lsb2.w + 0);
  idle();
  W.r24_lsb4.l = alu(W.r24_lsb4.l);
  lastCycle();
  writeBank(V.r24_lsb2.w + 0, W.r24_lsb4.l);
}

void WDC65816::instructionBankModify16(alu16 op) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  W.r24_lsb4.l = readBank(V.r24_lsb2.w + 0);
  W.r24_lsb4.h = readBank(V.r24_lsb2.w + 1);
  idle();
  W.r24_lsb2.w = alu(W.r24_lsb2.w);
  writeBank(V.r24_lsb2.w + 1, W.r24_lsb4.h);
  lastCycle();
  writeBank(V.r24_lsb2.w + 0, W.r24_lsb4.l);
}

void WDC65816::instructionBankIndexedModify8(alu8 op) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  idle();
  W.r24_lsb4.l = readBank(V.r24_lsb2.w + X.w + 0);
  idle();
  W.r24_lsb4.l = alu(W.r24_lsb4.l);
  lastCycle();
  writeBank(V.r24_lsb2.w + X.w + 0, W.r24_lsb4.l);
}

void WDC65816::instructionBankIndexedModify16(alu16 op) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  idle();
  W.r24_lsb4.l = readBank(V.r24_lsb2.w + X.w + 0);
  W.r24_lsb4.h = readBank(V.r24_lsb2.w + X.w + 1);
  idle();
  W.r24_lsb2.w = alu(W.r24_lsb2.w);
  writeBank(V.r24_lsb2.w + X.w + 1, W.r24_lsb4.h);
  lastCycle();
  writeBank(V.r24_lsb2.w + X.w + 0, W.r24_lsb4.l);
}

void WDC65816::instructionDirectModify8(alu8 op) {
  U.r24_lsb4.l = fetch();
  idle2();
  W.r24_lsb4.l = readDirect(U.r24_lsb4.l + 0);
  idle();
  W.r24_lsb4.l = alu(W.r24_lsb4.l);
  lastCycle();
  writeDirect(U.r24_lsb4.l + 0, W.r24_lsb4.l);
}

void WDC65816::instructionDirectModify16(alu16 op) {
  U.r24_lsb4.l = fetch();
  idle2();
  W.r24_lsb4.l = readDirect(U.r24_lsb4.l + 0);
  W.r24_lsb4.h = readDirect(U.r24_lsb4.l + 1);
  idle();
  W.r24_lsb2.w = alu(W.r24_lsb2.w);
  writeDirect(U.r24_lsb4.l + 1, W.r24_lsb4.h);
  lastCycle();
  writeDirect(U.r24_lsb4.l + 0, W.r24_lsb4.l);
}

void WDC65816::instructionDirectIndexedModify8(alu8 op) {
  U.r24_lsb4.l = fetch();
  idle2();
  idle();
  W.r24_lsb4.l = readDirect(U.r24_lsb4.l + X.w + 0);
  idle();
  W.r24_lsb4.l = alu(W.r24_lsb4.l);
  lastCycle();
  writeDirect(U.r24_lsb4.l + X.w + 0, W.r24_lsb4.l);
}

void WDC65816::instructionDirectIndexedModify16(alu16 op) {
  U.r24_lsb4.l = fetch();
  idle2();
  idle();
  W.r24_lsb4.l = readDirect(U.r24_lsb4.l + X.w + 0);
  W.r24_lsb4.h = readDirect(U.r24_lsb4.l + X.w + 1);
  idle();
  W.r24_lsb2.w = alu(W.r24_lsb2.w);
  writeDirect(U.r24_lsb4.l + X.w + 1, W.r24_lsb4.h);
  lastCycle();
  writeDirect(U.r24_lsb4.l + X.w + 0, W.r24_lsb4.l);
}

void WDC65816::instructionBranch(bool take) {
  if(!take) {
  lastCycle();
    fetch();
  } else {
    U.r24_lsb4.l = fetch();
    V.r24_lsb2.w = PC.d + (int8_t)U.r24_lsb4.l;
    idle6(V.r24_lsb2.w);
  lastCycle();
    idle();
    PC.r24_lsb2.w = V.r24_lsb2.w;
    idleBranch();
  }
}

void WDC65816::instructionBranchLong() {
  U.r24_lsb4.l = fetch();
  U.r24_lsb4.h = fetch();
  V.r24_lsb2.w = PC.d + (int16_t)U.r24_lsb2.w;
  lastCycle();
  idle();
  PC.r24_lsb2.w = V.r24_lsb2.w;
  idleBranch();
}

void WDC65816::instructionJumpShort() {
  W.r24_lsb4.l = fetch();
  lastCycle();
  W.r24_lsb4.h = fetch();
  PC.r24_lsb2.w = W.r24_lsb2.w;
  idleJump();
}

void WDC65816::instructionJumpLong() {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  lastCycle();
  V.r24_lsb4.b = fetch();
  PC.d = V.d;
  idleJump();
}

void WDC65816::instructionJumpIndirect() {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  W.r24_lsb4.l = read(uint16_t(V.r24_lsb2.w + 0));
  lastCycle();
  W.r24_lsb4.h = read(uint16_t(V.r24_lsb2.w + 1));
  PC.r24_lsb2.w = W.r24_lsb2.w;
  idleJump();
}

void WDC65816::instructionJumpIndexedIndirect() {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  idle();
  W.r24_lsb4.l = read(PC.r24_lsb4.b << 16 | uint16_t(V.r24_lsb2.w + X.w + 0));
  lastCycle();
  W.r24_lsb4.h = read(PC.r24_lsb4.b << 16 | uint16_t(V.r24_lsb2.w + X.w + 1));
  PC.r24_lsb2.w = W.r24_lsb2.w;
  idleJump();
}

void WDC65816::instructionJumpIndirectLong() {
  U.r24_lsb4.l = fetch();
  U.r24_lsb4.h = fetch();
  V.r24_lsb4.l = read(uint16_t(U.r24_lsb2.w + 0));
  V.r24_lsb4.h = read(uint16_t(U.r24_lsb2.w + 1));
  lastCycle();
  V.r24_lsb4.b = read(uint16_t(U.r24_lsb2.w + 2));
  PC.d = V.d;
  idleJump();
}

void WDC65816::instructionCallShort() {
  W.r24_lsb4.l = fetch();
  W.r24_lsb4.h = fetch();
  idle();
  PC.r24_lsb2.w--;
  push(PC.r24_lsb4.h);
  lastCycle();
  push(PC.r24_lsb4.l);
  PC.r24_lsb2.w = W.r24_lsb2.w;
  idleJump();
}

void WDC65816::instructionCallLong() {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  pushN(PC.r24_lsb4.b);
  idle();
  V.r24_lsb4.b = fetch();
  PC.r24_lsb2.w--;
  pushN(PC.r24_lsb4.h);
  lastCycle();
  pushN(PC.r24_lsb4.l);
  PC.d = V.d;
  if(r.e) S.r16_lsb2.h = 0x01;
  idleJump();
}

void WDC65816::instructionCallIndexedIndirect() {
  V.r24_lsb4.l = fetch();
  pushN(PC.r24_lsb4.h);
  pushN(PC.r24_lsb4.l);
  V.r24_lsb4.h = fetch();
  idle();
  W.r24_lsb4.l = read(PC.r24_lsb4.b << 16 | uint16_t(V.r24_lsb2.w + X.w + 0));
  lastCycle();
  W.r24_lsb4.h = read(PC.r24_lsb4.b << 16 | uint16_t(V.r24_lsb2.w + X.w + 1));
  PC.r24_lsb2.w = W.r24_lsb2.w;
  if(r.e) S.r16_lsb2.h = 0x01;
  idleJump();
}

void WDC65816::instructionReturnInterrupt() {
  idle();
  idle();
  P = pull();
  if(r.e) XF = 1, MF = 1;
  if(XF) X.r16_lsb2.h = 0x00, Y.r16_lsb2.h = 0x00;
  PC.r24_lsb4.l = pull();
  if(EF) {
    lastCycle();
    PC.r24_lsb4.h = pull();
  } else {
    PC.r24_lsb4.h = pull();
    lastCycle();
    PC.r24_lsb4.b = pull();
  }
  idleJump();
}

void WDC65816::instructionReturnShort() {
  idle();
  idle();
  W.r24_lsb4.l = pull();
  W.r24_lsb4.h = pull();
  lastCycle();
  idle();
  PC.r24_lsb2.w = W.r24_lsb2.w;
  PC.r24_lsb2.w++;
  idleJump();
}

void WDC65816::instructionReturnLong() {
  idle();
  idle();
  V.r24_lsb4.l = pullN();
  V.r24_lsb4.h = pullN();
  lastCycle();
  V.r24_lsb4.b = pullN();
  PC.d = V.d;
  PC.r24_lsb2.w++;
  if(r.e) S.r16_lsb2.h = 0x01;
  idleJump();
}

void WDC65816::instructionBitImmediate8() {
  lastCycle();
  U.r24_lsb4.l = fetch();
  ZF = (U.r24_lsb4.l & A.r16_lsb2.l) == 0;
}

void WDC65816::instructionBitImmediate16() {
  U.r24_lsb4.l = fetch();
  lastCycle();
  U.r24_lsb4.h = fetch();
  ZF = (U.r24_lsb2.w & A.w) == 0;
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
  ZF = A.r16_lsb2.l == 0;
  NF = A.r16_lsb2.l & 0x80;
}

void WDC65816::instructionBlockMove8(int adjust) {
  U.r24_lsb4.b = fetch();
  V.r24_lsb4.b = fetch();
  B = U.r24_lsb4.b;
  W.r24_lsb4.l = read(V.r24_lsb4.b << 16 | X.w);
  write(U.r24_lsb4.b << 16 | Y.w, W.r24_lsb4.l);
  idle();
  X.r16_lsb2.l += adjust;
  Y.r16_lsb2.l += adjust;
  lastCycle();
  idle();
  if(A.w--) PC.r24_lsb2.w -= 3;
}

void WDC65816::instructionBlockMove16(int adjust) {
  U.r24_lsb4.b = fetch();
  V.r24_lsb4.b = fetch();
  B = U.r24_lsb4.b;
  W.r24_lsb4.l = read(V.r24_lsb4.b << 16 | X.w);
  write(U.r24_lsb4.b << 16 | Y.w, W.r24_lsb4.l);
  idle();
  X.w += adjust;
  Y.w += adjust;
  lastCycle();
  idle();
  if(A.w--) PC.r24_lsb2.w -= 3;
}

void WDC65816::instructionInterrupt(r16 vector) {
  fetch();
  if(!r.e) push(PC.r24_lsb4.b);
  push(PC.r24_lsb4.h);
  push(PC.r24_lsb4.l);
  push(P);
  IF = 1;
  DF = 0;
  PC.r24_lsb4.l = read(vector.w + 0);
  lastCycle();
  PC.r24_lsb4.h = read(vector.w + 1);
  PC.r24_lsb4.b = 0x00;
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
    X.r16_lsb2.h = 0x00;
    Y.r16_lsb2.h = 0x00;
    S.r16_lsb2.h = 0x01;
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
  W.r24_lsb4.l = fetch();
  lastCycle();
  idle();
  P = P & ~W.r24_lsb4.l;
  if(r.e) XF = 1, MF = 1;
  if(XF) X.r16_lsb2.h = 0x00, Y.r16_lsb2.h = 0x00;
}

void WDC65816::instructionSetP() {
  W.r24_lsb4.l = fetch();
  lastCycle();
  idle();
  P = P | W.r24_lsb4.l;
  if(r.e) XF = 1, MF = 1;
  if(XF) X.r16_lsb2.h = 0x00, Y.r16_lsb2.h = 0x00;
}

void WDC65816::instructionTransfer8(r16 F, r16& T) {
  lastCycle();
  idleIRQ();
  T.r16_lsb2.l = F.r16_lsb2.l;
  ZF = T.r16_lsb2.l == 0;
  NF = T.r16_lsb2.l & 0x80;
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
  if(r.e) S.r16_lsb2.h = 0x01;
}

void WDC65816::instructionTransferSX8() {
  lastCycle();
  idleIRQ();
  X.r16_lsb2.l = S.r16_lsb2.l;
  ZF = X.r16_lsb2.l == 0;
  NF = X.r16_lsb2.l & 0x80;
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
  if(r.e) S.r16_lsb2.l = X.r16_lsb2.l;
  if(!r.e) S.w = X.w;
}

void WDC65816::instructionPush8(r16 F) {
  idle();
  lastCycle();
  push(F.r16_lsb2.l);
}

void WDC65816::instructionPush16(r16 F) {
  idle();
  push(F.r16_lsb2.h);
  lastCycle();
  push(F.r16_lsb2.l);
}

void WDC65816::instructionPushD() {
  idle();
  pushN(D.r16_lsb2.h);
  lastCycle();
  pushN(D.r16_lsb2.l);
  if(r.e) S.r16_lsb2.h = 0x01;
}

void WDC65816::instructionPull8(r16& T) {
  idle();
  idle();
  lastCycle();
  T.r16_lsb2.l = pull();
  ZF = T.r16_lsb2.l == 0;
  NF = T.r16_lsb2.l & 0x80;
}

void WDC65816::instructionPull16(r16& T) {
  idle();
  idle();
  T.r16_lsb2.l = pull();
  lastCycle();
  T.r16_lsb2.h = pull();
  ZF = T.w == 0;
  NF = T.w & 0x8000;
}

void WDC65816::instructionPullD() {
  idle();
  idle();
  D.r16_lsb2.l = pullN();
  lastCycle();
  D.r16_lsb2.h = pullN();
  ZF = D.w == 0;
  NF = D.w & 0x8000;
  if(r.e) S.r16_lsb2.h = 0x01;
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
  if(XF) X.r16_lsb2.h = 0x00, Y.r16_lsb2.h = 0x00;
}

void WDC65816::instructionPushEffectiveAddress() {
  W.r24_lsb4.l = fetch();
  W.r24_lsb4.h = fetch();
  pushN(W.r24_lsb4.h);
  lastCycle();
  pushN(W.r24_lsb4.l);
  if(r.e) S.r16_lsb2.h = 0x01;
}

void WDC65816::instructionPushEffectiveIndirectAddress() {
  U.r24_lsb4.l = fetch();
  idle2();
  W.r24_lsb4.l = readDirectN(U.r24_lsb4.l + 0);
  W.r24_lsb4.h = readDirectN(U.r24_lsb4.l + 1);
  pushN(W.r24_lsb4.h);
  lastCycle();
  pushN(W.r24_lsb4.l);
  if(r.e) S.r16_lsb2.h = 0x01;
}

void WDC65816::instructionPushEffectiveRelativeAddress() {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  idle();
  W.r24_lsb2.w = PC.d + (int16_t)V.r24_lsb2.w;
  pushN(W.r24_lsb4.h);
  lastCycle();
  pushN(W.r24_lsb4.l);
  if(r.e) S.r16_lsb2.h = 0x01;
}

void WDC65816::interrupt() {
  read(PC.d);
  idle();
  if(!r.e) push(PC.r24_lsb4.b);
  push(PC.r24_lsb4.h);
  push(PC.r24_lsb4.l);
  push(EF ? P & ~0x10 : P);
  IF = 1;
  DF = 0;
  PC.r24_lsb4.l = read(r.vector + 0);
  lastCycle();
  PC.r24_lsb4.h = read(r.vector + 1);
  PC.r24_lsb4.b = 0x00;
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

}
