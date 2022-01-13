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

void WDC65816::ImmediateRead8(alu8 op) {
  lastCycle();
  W.r24_lsb4.l = fetch();
  alu(W.r24_lsb4.l);
}

void WDC65816::ImmediateRead16(alu16 op) {
  W.r24_lsb4.l = fetch();
  lastCycle();
  W.r24_lsb4.h = fetch();
  alu(W.r24_lsb2.w);
}

void WDC65816::BankRead8(alu8 op) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  lastCycle();
  W.r24_lsb4.l = readBank(V.r24_lsb2.w + 0);
  alu(W.r24_lsb4.l);
}

void WDC65816::BankRead16(alu16 op) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  W.r24_lsb4.l = readBank(V.r24_lsb2.w + 0);
  lastCycle();
  W.r24_lsb4.h = readBank(V.r24_lsb2.w + 1);
  alu(W.r24_lsb2.w);
}

void WDC65816::BankRead8(alu8 op, r16 I) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  idle4(V.r24_lsb2.w, V.r24_lsb2.w + I.w);
  lastCycle();
  W.r24_lsb4.l = readBank(V.r24_lsb2.w + I.w + 0);
  alu(W.r24_lsb4.l);
}

void WDC65816::BankRead16(alu16 op, r16 I) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  idle4(V.r24_lsb2.w, V.r24_lsb2.w + I.w);
  W.r24_lsb4.l = readBank(V.r24_lsb2.w + I.w + 0);
  lastCycle();
  W.r24_lsb4.h = readBank(V.r24_lsb2.w + I.w + 1);
  alu(W.r24_lsb2.w);
}

void WDC65816::LongRead8(alu8 op, r16 I) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  V.r24_lsb4.b = fetch();
  lastCycle();
  W.r24_lsb4.l = readLong(V.d + I.w + 0);
  alu(W.r24_lsb4.l);
}

void WDC65816::LongRead16(alu16 op, r16 I) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  V.r24_lsb4.b = fetch();
  W.r24_lsb4.l = readLong(V.d + I.w + 0);
  lastCycle();
  W.r24_lsb4.h = readLong(V.d + I.w + 1);
  alu(W.r24_lsb2.w);
}

void WDC65816::DirectRead8(alu8 op) {
  U.r24_lsb4.l = fetch();
  idle2();
  lastCycle();
  W.r24_lsb4.l = readDirect(U.r24_lsb4.l + 0);
  alu(W.r24_lsb4.l);
}

void WDC65816::DirectRead16(alu16 op) {
  U.r24_lsb4.l = fetch();
  idle2();
  W.r24_lsb4.l = readDirect(U.r24_lsb4.l + 0);
  lastCycle();
  W.r24_lsb4.h = readDirect(U.r24_lsb4.l + 1);
  alu(W.r24_lsb2.w);
}

void WDC65816::DirectRead8(alu8 op, r16 I) {
  U.r24_lsb4.l = fetch();
  idle2();
  idle();
  lastCycle();
  W.r24_lsb4.l = readDirect(U.r24_lsb4.l + I.w + 0);
  alu(W.r24_lsb4.l);
}

void WDC65816::DirectRead16(alu16 op, r16 I) {
  U.r24_lsb4.l = fetch();
  idle2();
  idle();
  W.r24_lsb4.l = readDirect(U.r24_lsb4.l + I.w + 0);
  lastCycle();
  W.r24_lsb4.h = readDirect(U.r24_lsb4.l + I.w + 1);
  alu(W.r24_lsb2.w);
}

void WDC65816::IndirectRead8(alu8 op) {
  U.r24_lsb4.l = fetch();
  idle2();
  V.r24_lsb4.l = readDirect(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readDirect(U.r24_lsb4.l + 1);
  lastCycle();
  W.r24_lsb4.l = readBank(V.r24_lsb2.w + 0);
  alu(W.r24_lsb4.l);
}

void WDC65816::IndirectRead16(alu16 op) {
  U.r24_lsb4.l = fetch();
  idle2();
  V.r24_lsb4.l = readDirect(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readDirect(U.r24_lsb4.l + 1);
  W.r24_lsb4.l = readBank(V.r24_lsb2.w + 0);
  lastCycle();
  W.r24_lsb4.h = readBank(V.r24_lsb2.w + 1);
  alu(W.r24_lsb2.w);
}

void WDC65816::IndexedIndirectRead8(alu8 op) {
  U.r24_lsb4.l = fetch();
  idle2();
  idle();
  V.r24_lsb4.l = readDirect(U.r24_lsb4.l + X.w + 0);
  V.r24_lsb4.h = readDirect(U.r24_lsb4.l + X.w + 1);
  lastCycle();
  W.r24_lsb4.l = readBank(V.r24_lsb2.w + 0);
  alu(W.r24_lsb4.l);
}

void WDC65816::IndexedIndirectRead16(alu16 op) {
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

void WDC65816::IndirectIndexedRead8(alu8 op) {
  U.r24_lsb4.l = fetch();
  idle2();
  V.r24_lsb4.l = readDirect(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readDirect(U.r24_lsb4.l + 1);
  idle4(V.r24_lsb2.w, V.r24_lsb2.w + Y.w);
  lastCycle();
  W.r24_lsb4.l = readBank(V.r24_lsb2.w + Y.w + 0);
  alu(W.r24_lsb4.l);
}

void WDC65816::IndirectIndexedRead16(alu16 op) {
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

void WDC65816::IndirectLongRead8(alu8 op, r16 I) {
  U.r24_lsb4.l = fetch();
  idle2();
  V.r24_lsb4.l = readDirectN(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readDirectN(U.r24_lsb4.l + 1);
  V.r24_lsb4.b = readDirectN(U.r24_lsb4.l + 2);
  lastCycle();
  W.r24_lsb4.l = readLong(V.d + I.w + 0);
  alu(W.r24_lsb4.l);
}

void WDC65816::IndirectLongRead16(alu16 op, r16 I) {
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

void WDC65816::StackRead8(alu8 op) {
  U.r24_lsb4.l = fetch();
  idle();
  lastCycle();
  W.r24_lsb4.l = readStack(U.r24_lsb4.l + 0);
  alu(W.r24_lsb4.l);
}

void WDC65816::StackRead16(alu16 op) {
  U.r24_lsb4.l = fetch();
  idle();
  W.r24_lsb4.l = readStack(U.r24_lsb4.l + 0);
  lastCycle();
  W.r24_lsb4.h = readStack(U.r24_lsb4.l + 1);
  alu(W.r24_lsb2.w);
}

void WDC65816::IndirectStackRead8(alu8 op) {
  U.r24_lsb4.l = fetch();
  idle();
  V.r24_lsb4.l = readStack(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readStack(U.r24_lsb4.l + 1);
  idle();
  lastCycle();
  W.r24_lsb4.l = readBank(V.r24_lsb2.w + Y.w + 0);
  alu(W.r24_lsb4.l);
}

void WDC65816::IndirectStackRead16(alu16 op) {
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

void WDC65816::BankWrite8(r16 F) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  lastCycle();
  writeBank(V.r24_lsb2.w + 0, F.r16_lsb2.l);
}

void WDC65816::BankWrite16(r16 F) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  writeBank(V.r24_lsb2.w + 0, F.r16_lsb2.l);
  lastCycle();
  writeBank(V.r24_lsb2.w + 1, F.r16_lsb2.h);
}

void WDC65816::BankWrite8(r16 F, r16 I) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  idle();
  lastCycle();
  writeBank(V.r24_lsb2.w + I.w + 0, F.r16_lsb2.l);
}

void WDC65816::BankWrite16(r16 F, r16 I) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  idle();
  writeBank(V.r24_lsb2.w + I.w + 0, F.r16_lsb2.l);
  lastCycle();
  writeBank(V.r24_lsb2.w + I.w + 1, F.r16_lsb2.h);
}

void WDC65816::LongWrite8(r16 I) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  V.r24_lsb4.b = fetch();
  lastCycle();
  writeLong(V.d + I.w + 0, A.r16_lsb2.l);
}

void WDC65816::LongWrite16(r16 I) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  V.r24_lsb4.b = fetch();
  writeLong(V.d + I.w + 0, A.r16_lsb2.l);
  lastCycle();
  writeLong(V.d + I.w + 1, A.r16_lsb2.h);
}

void WDC65816::DirectWrite8(r16 F) {
  U.r24_lsb4.l = fetch();
  idle2();
  lastCycle();
  writeDirect(U.r24_lsb4.l + 0, F.r16_lsb2.l);
}

void WDC65816::DirectWrite16(r16 F) {
  U.r24_lsb4.l = fetch();
  idle2();
  writeDirect(U.r24_lsb4.l + 0, F.r16_lsb2.l);
  lastCycle();
  writeDirect(U.r24_lsb4.l + 1, F.r16_lsb2.h);
}

void WDC65816::DirectWrite8(r16 F, r16 I) {
  U.r24_lsb4.l = fetch();
  idle2();
  idle();
  lastCycle();
  writeDirect(U.r24_lsb4.l + I.w + 0, F.r16_lsb2.l);
}

void WDC65816::DirectWrite16(r16 F, r16 I) {
  U.r24_lsb4.l = fetch();
  idle2();
  idle();
  writeDirect(U.r24_lsb4.l + I.w + 0, F.r16_lsb2.l);
  lastCycle();
  writeDirect(U.r24_lsb4.l + I.w + 1, F.r16_lsb2.h);
}

void WDC65816::IndirectWrite8() {
  U.r24_lsb4.l = fetch();
  idle2();
  V.r24_lsb4.l = readDirect(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readDirect(U.r24_lsb4.l + 1);
  lastCycle();
  writeBank(V.r24_lsb2.w + 0, A.r16_lsb2.l);
}

void WDC65816::IndirectWrite16() {
  U.r24_lsb4.l = fetch();
  idle2();
  V.r24_lsb4.l = readDirect(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readDirect(U.r24_lsb4.l + 1);
  writeBank(V.r24_lsb2.w + 0, A.r16_lsb2.l);
  lastCycle();
  writeBank(V.r24_lsb2.w + 1, A.r16_lsb2.h);
}

void WDC65816::IndexedIndirectWrite8() {
  U.r24_lsb4.l = fetch();
  idle2();
  idle();
  V.r24_lsb4.l = readDirect(U.r24_lsb4.l + X.w + 0);
  V.r24_lsb4.h = readDirect(U.r24_lsb4.l + X.w + 1);
  lastCycle();
  writeBank(V.r24_lsb2.w + 0, A.r16_lsb2.l);
}

void WDC65816::IndexedIndirectWrite16() {
  U.r24_lsb4.l = fetch();
  idle2();
  idle();
  V.r24_lsb4.l = readDirect(U.r24_lsb4.l + X.w + 0);
  V.r24_lsb4.h = readDirect(U.r24_lsb4.l + X.w + 1);
  writeBank(V.r24_lsb2.w + 0, A.r16_lsb2.l);
  lastCycle();
  writeBank(V.r24_lsb2.w + 1, A.r16_lsb2.h);
}

void WDC65816::IndirectIndexedWrite8() {
  U.r24_lsb4.l = fetch();
  idle2();
  V.r24_lsb4.l = readDirect(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readDirect(U.r24_lsb4.l + 1);
  idle();
  lastCycle();
  writeBank(V.r24_lsb2.w + Y.w + 0, A.r16_lsb2.l);
}

void WDC65816::IndirectIndexedWrite16() {
  U.r24_lsb4.l = fetch();
  idle2();
  V.r24_lsb4.l = readDirect(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readDirect(U.r24_lsb4.l + 1);
  idle();
  writeBank(V.r24_lsb2.w + Y.w + 0, A.r16_lsb2.l);
  lastCycle();
  writeBank(V.r24_lsb2.w + Y.w + 1, A.r16_lsb2.h);
}

void WDC65816::IndirectLongWrite8(r16 I) {
  U.r24_lsb4.l = fetch();
  idle2();
  V.r24_lsb4.l = readDirectN(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readDirectN(U.r24_lsb4.l + 1);
  V.r24_lsb4.b = readDirectN(U.r24_lsb4.l + 2);
  lastCycle();
  writeLong(V.d + I.w + 0, A.r16_lsb2.l);
}

void WDC65816::IndirectLongWrite16(r16 I) {
  U.r24_lsb4.l = fetch();
  idle2();
  V.r24_lsb4.l = readDirectN(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readDirectN(U.r24_lsb4.l + 1);
  V.r24_lsb4.b = readDirectN(U.r24_lsb4.l + 2);
  writeLong(V.d + I.w + 0, A.r16_lsb2.l);
  lastCycle();
  writeLong(V.d + I.w + 1, A.r16_lsb2.h);
}

void WDC65816::StackWrite8() {
  U.r24_lsb4.l = fetch();
  idle();
  lastCycle();
  writeStack(U.r24_lsb4.l + 0, A.r16_lsb2.l);
}

void WDC65816::StackWrite16() {
  U.r24_lsb4.l = fetch();
  idle();
  writeStack(U.r24_lsb4.l + 0, A.r16_lsb2.l);
  lastCycle();
  writeStack(U.r24_lsb4.l + 1, A.r16_lsb2.h);
}

void WDC65816::IndirectStackWrite8() {
  U.r24_lsb4.l = fetch();
  idle();
  V.r24_lsb4.l = readStack(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readStack(U.r24_lsb4.l + 1);
  idle();
  lastCycle();
  writeBank(V.r24_lsb2.w + Y.w + 0, A.r16_lsb2.l);
}

void WDC65816::IndirectStackWrite16() {
  U.r24_lsb4.l = fetch();
  idle();
  V.r24_lsb4.l = readStack(U.r24_lsb4.l + 0);
  V.r24_lsb4.h = readStack(U.r24_lsb4.l + 1);
  idle();
  writeBank(V.r24_lsb2.w + Y.w + 0, A.r16_lsb2.l);
  lastCycle();
  writeBank(V.r24_lsb2.w + Y.w + 1, A.r16_lsb2.h);
}

void WDC65816::ImpliedModify8(alu8 op, r16& M) {
  lastCycle();
  idleIRQ();
  M.r16_lsb2.l = alu(M.r16_lsb2.l);
}

void WDC65816::ImpliedModify16(alu16 op, r16& M) {
  lastCycle();
  idleIRQ();
  M.w = alu(M.w);
}

void WDC65816::BankModify8(alu8 op) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  W.r24_lsb4.l = readBank(V.r24_lsb2.w + 0);
  idle();
  W.r24_lsb4.l = alu(W.r24_lsb4.l);
  lastCycle();
  writeBank(V.r24_lsb2.w + 0, W.r24_lsb4.l);
}

void WDC65816::BankModify16(alu16 op) {
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

void WDC65816::BankIndexedModify8(alu8 op) {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  idle();
  W.r24_lsb4.l = readBank(V.r24_lsb2.w + X.w + 0);
  idle();
  W.r24_lsb4.l = alu(W.r24_lsb4.l);
  lastCycle();
  writeBank(V.r24_lsb2.w + X.w + 0, W.r24_lsb4.l);
}

void WDC65816::BankIndexedModify16(alu16 op) {
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

void WDC65816::DirectModify8(alu8 op) {
  U.r24_lsb4.l = fetch();
  idle2();
  W.r24_lsb4.l = readDirect(U.r24_lsb4.l + 0);
  idle();
  W.r24_lsb4.l = alu(W.r24_lsb4.l);
  lastCycle();
  writeDirect(U.r24_lsb4.l + 0, W.r24_lsb4.l);
}

void WDC65816::DirectModify16(alu16 op) {
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

void WDC65816::DirectIndexedModify8(alu8 op) {
  U.r24_lsb4.l = fetch();
  idle2();
  idle();
  W.r24_lsb4.l = readDirect(U.r24_lsb4.l + X.w + 0);
  idle();
  W.r24_lsb4.l = alu(W.r24_lsb4.l);
  lastCycle();
  writeDirect(U.r24_lsb4.l + X.w + 0, W.r24_lsb4.l);
}

void WDC65816::DirectIndexedModify16(alu16 op) {
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

void WDC65816::Branch(bool take) {
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

void WDC65816::BranchLong() {
  U.r24_lsb4.l = fetch();
  U.r24_lsb4.h = fetch();
  V.r24_lsb2.w = PC.d + (int16_t)U.r24_lsb2.w;
  lastCycle();
  idle();
  PC.r24_lsb2.w = V.r24_lsb2.w;
  idleBranch();
}

void WDC65816::JumpShort() {
  W.r24_lsb4.l = fetch();
  lastCycle();
  W.r24_lsb4.h = fetch();
  PC.r24_lsb2.w = W.r24_lsb2.w;
  idleJump();
}

void WDC65816::JumpLong() {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  lastCycle();
  V.r24_lsb4.b = fetch();
  PC.d = V.d;
  idleJump();
}

void WDC65816::JumpIndirect() {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  W.r24_lsb4.l = read(uint16_t(V.r24_lsb2.w + 0));
  lastCycle();
  W.r24_lsb4.h = read(uint16_t(V.r24_lsb2.w + 1));
  PC.r24_lsb2.w = W.r24_lsb2.w;
  idleJump();
}

void WDC65816::JumpIndexedIndirect() {
  V.r24_lsb4.l = fetch();
  V.r24_lsb4.h = fetch();
  idle();
  W.r24_lsb4.l = read(PC.r24_lsb4.b << 16 | uint16_t(V.r24_lsb2.w + X.w + 0));
  lastCycle();
  W.r24_lsb4.h = read(PC.r24_lsb4.b << 16 | uint16_t(V.r24_lsb2.w + X.w + 1));
  PC.r24_lsb2.w = W.r24_lsb2.w;
  idleJump();
}

void WDC65816::JumpIndirectLong() {
  U.r24_lsb4.l = fetch();
  U.r24_lsb4.h = fetch();
  V.r24_lsb4.l = read(uint16_t(U.r24_lsb2.w + 0));
  V.r24_lsb4.h = read(uint16_t(U.r24_lsb2.w + 1));
  lastCycle();
  V.r24_lsb4.b = read(uint16_t(U.r24_lsb2.w + 2));
  PC.d = V.d;
  idleJump();
}

void WDC65816::CallShort() {
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

void WDC65816::CallLong() {
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

void WDC65816::CallIndexedIndirect() {
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

void WDC65816::ReturnInterrupt() {
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

void WDC65816::ReturnShort() {
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

void WDC65816::ReturnLong() {
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

void WDC65816::BitImmediate8() {
  lastCycle();
  U.r24_lsb4.l = fetch();
  ZF = (U.r24_lsb4.l & A.r16_lsb2.l) == 0;
}

void WDC65816::BitImmediate16() {
  U.r24_lsb4.l = fetch();
  lastCycle();
  U.r24_lsb4.h = fetch();
  ZF = (U.r24_lsb2.w & A.w) == 0;
}

void WDC65816::NoOperation() {
  lastCycle();
  idleIRQ();
}

void WDC65816::Prefix() {
  lastCycle();
  fetch();
}

void WDC65816::ExchangeBA() {
  idle();
  lastCycle();
  idle();
  A.w = A.w >> 8 | A.w << 8;
  ZF = A.r16_lsb2.l == 0;
  NF = A.r16_lsb2.l & 0x80;
}

void WDC65816::BlockMove8(int adjust) {
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

void WDC65816::BlockMove16(int adjust) {
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

void WDC65816::Interrupt(r16 vector) {
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

void WDC65816::ExchangeCE() {
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

void WDC65816::SetFlag(bool& flag) {
  lastCycle();
  idleIRQ();
  flag = 1;
}

void WDC65816::ClearFlag(bool& flag) {
  lastCycle();
  idleIRQ();
  flag = 0;
}

void WDC65816::ResetP() {
  W.r24_lsb4.l = fetch();
  lastCycle();
  idle();
  P = P & ~W.r24_lsb4.l;
  if(r.e) XF = 1, MF = 1;
  if(XF) X.r16_lsb2.h = 0x00, Y.r16_lsb2.h = 0x00;
}

void WDC65816::SetP() {
  W.r24_lsb4.l = fetch();
  lastCycle();
  idle();
  P = P | W.r24_lsb4.l;
  if(r.e) XF = 1, MF = 1;
  if(XF) X.r16_lsb2.h = 0x00, Y.r16_lsb2.h = 0x00;
}

void WDC65816::Transfer8(r16 F, r16& T) {
  lastCycle();
  idleIRQ();
  T.r16_lsb2.l = F.r16_lsb2.l;
  ZF = T.r16_lsb2.l == 0;
  NF = T.r16_lsb2.l & 0x80;
}

void WDC65816::Transfer16(r16 F, r16& T) {
  lastCycle();
  idleIRQ();
  T.w = F.w;
  ZF = T.w == 0;
  NF = T.w & 0x8000;
}

void WDC65816::TransferCS() {
  lastCycle();
  idleIRQ();
  S.w = A.w;
  if(r.e) S.r16_lsb2.h = 0x01;
}

void WDC65816::TransferSX8() {
  lastCycle();
  idleIRQ();
  X.r16_lsb2.l = S.r16_lsb2.l;
  ZF = X.r16_lsb2.l == 0;
  NF = X.r16_lsb2.l & 0x80;
}

void WDC65816::TransferSX16() {
  lastCycle();
  idleIRQ();
  X.w = S.w;
  ZF = X.w == 0;
  NF = X.w & 0x8000;
}

void WDC65816::TransferXS() {
  lastCycle();
  idleIRQ();
  if(r.e) S.r16_lsb2.l = X.r16_lsb2.l;
  if(!r.e) S.w = X.w;
}

void WDC65816::Push8(r16 F) {
  idle();
  lastCycle();
  push(F.r16_lsb2.l);
}

void WDC65816::Push16(r16 F) {
  idle();
  push(F.r16_lsb2.h);
  lastCycle();
  push(F.r16_lsb2.l);
}

void WDC65816::PushD() {
  idle();
  pushN(D.r16_lsb2.h);
  lastCycle();
  pushN(D.r16_lsb2.l);
  if(r.e) S.r16_lsb2.h = 0x01;
}

void WDC65816::Pull8(r16& T) {
  idle();
  idle();
  lastCycle();
  T.r16_lsb2.l = pull();
  ZF = T.r16_lsb2.l == 0;
  NF = T.r16_lsb2.l & 0x80;
}

void WDC65816::Pull16(r16& T) {
  idle();
  idle();
  T.r16_lsb2.l = pull();
  lastCycle();
  T.r16_lsb2.h = pull();
  ZF = T.w == 0;
  NF = T.w & 0x8000;
}

void WDC65816::PullD() {
  idle();
  idle();
  D.r16_lsb2.l = pullN();
  lastCycle();
  D.r16_lsb2.h = pullN();
  ZF = D.w == 0;
  NF = D.w & 0x8000;
  if(r.e) S.r16_lsb2.h = 0x01;
}

void WDC65816::PullB() {
  idle();
  idle();
  lastCycle();
  B = pull();
  ZF = B == 0;
  NF = B & 0x80;
}

void WDC65816::PullP() {
  idle();
  idle();
  lastCycle();
  P = pull();
  if(r.e) XF = 1, MF = 1;
  if(XF) X.r16_lsb2.h = 0x00, Y.r16_lsb2.h = 0x00;
}

void WDC65816::PushEffectiveAddress() {
  W.r24_lsb4.l = fetch();
  W.r24_lsb4.h = fetch();
  pushN(W.r24_lsb4.h);
  lastCycle();
  pushN(W.r24_lsb4.l);
  if(r.e) S.r16_lsb2.h = 0x01;
}

void WDC65816::PushEffectiveIndirectAddress() {
  U.r24_lsb4.l = fetch();
  idle2();
  W.r24_lsb4.l = readDirectN(U.r24_lsb4.l + 0);
  W.r24_lsb4.h = readDirectN(U.r24_lsb4.l + 1);
  pushN(W.r24_lsb4.h);
  lastCycle();
  pushN(W.r24_lsb4.l);
  if(r.e) S.r16_lsb2.h = 0x01;
}

void WDC65816::PushEffectiveRelativeAddress() {
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
void WDC65816::instruction() {
  switch(fetch()) {
    //emulation mode lacks BRK vector; uses IRQ vector instead
    case 0x00: return Interrupt(EF ? (r16)0xfffe : (r16)0xffe6);
    case 0x01: return MF ? IndexedIndirectRead8(&WDC65816::algorithmORA8) : IndexedIndirectRead16(&WDC65816::algorithmORA16);
    case 0x02: return Interrupt(EF ? (r16)0xfff4 : (r16)0xffe4);
    case 0x03: return MF ? StackRead8(&WDC65816::algorithmORA8) : StackRead16(&WDC65816::algorithmORA16);
    case 0x04: return MF ? DirectModify8(&WDC65816::algorithmTSB8) : DirectModify16(&WDC65816::algorithmTSB16);
    case 0x05: return MF ? DirectRead8(&WDC65816::algorithmORA8) : DirectRead16(&WDC65816::algorithmORA16);
    case 0x06: return MF ? DirectModify8(&WDC65816::algorithmASL8) : DirectModify16(&WDC65816::algorithmASL16);
    case 0x07: return MF ? IndirectLongRead8(&WDC65816::algorithmORA8) : IndirectLongRead16(&WDC65816::algorithmORA16);
    case 0x08: return Push8((r16)P);
    case 0x09: return MF ? ImmediateRead8(&WDC65816::algorithmORA8) : ImmediateRead16(&WDC65816::algorithmORA16);
    case 0x0a: return MF ? ImpliedModify8(&WDC65816::algorithmASL8, A) : ImpliedModify16(&WDC65816::algorithmASL16, A);
    case 0x0b: return PushD();
    case 0x0c: return MF ? BankModify8(&WDC65816::algorithmTSB8) : BankModify16(&WDC65816::algorithmTSB16);
    case 0x0d: return MF ? BankRead8(&WDC65816::algorithmORA8) : BankRead16(&WDC65816::algorithmORA16);
    case 0x0e: return MF ? BankModify8(&WDC65816::algorithmASL8) : BankModify16(&WDC65816::algorithmASL16);
    case 0x0f: return MF ? LongRead8(&WDC65816::algorithmORA8) : LongRead16(&WDC65816::algorithmORA16);
    case 0x10: return Branch(NF == 0);
    case 0x11: return MF ? IndirectIndexedRead8(&WDC65816::algorithmORA8) : IndirectIndexedRead16(&WDC65816::algorithmORA16);
    case 0x12: return MF ? IndirectRead8(&WDC65816::algorithmORA8) : IndirectRead16(&WDC65816::algorithmORA16);
    case 0x13: return MF ? IndirectStackRead8(&WDC65816::algorithmORA8) : IndirectStackRead16(&WDC65816::algorithmORA16);
    case 0x14: return MF ? DirectModify8(&WDC65816::algorithmTRB8) : DirectModify16(&WDC65816::algorithmTRB16);
    case 0x15: return MF ? DirectRead8(&WDC65816::algorithmORA8, X) : DirectRead16(&WDC65816::algorithmORA16, X);
    case 0x16: return MF ? DirectIndexedModify8(&WDC65816::algorithmASL8) : DirectIndexedModify16(&WDC65816::algorithmASL16);
    case 0x17: return MF ? IndirectLongRead8(&WDC65816::algorithmORA8, Y) : IndirectLongRead16(&WDC65816::algorithmORA16, Y);
    case 0x18: return ClearFlag(CF);
    case 0x19: return MF ? BankRead8(&WDC65816::algorithmORA8, Y) : BankRead16(&WDC65816::algorithmORA16, Y);
    case 0x1a: return MF ? ImpliedModify8(&WDC65816::algorithmINC8, A) : ImpliedModify16(&WDC65816::algorithmINC16, A);
    case 0x1b: return TransferCS();
    case 0x1c: return MF ? BankModify8(&WDC65816::algorithmTRB8) : BankModify16(&WDC65816::algorithmTRB16);
    case 0x1d: return MF ? BankRead8(&WDC65816::algorithmORA8, X) : BankRead16(&WDC65816::algorithmORA16, X);
    case 0x1e: return MF ? BankIndexedModify8(&WDC65816::algorithmASL8) : BankIndexedModify16(&WDC65816::algorithmASL16);
    case 0x1f: return MF ? LongRead8(&WDC65816::algorithmORA8, X) : LongRead16(&WDC65816::algorithmORA16, X);
    case 0x20: return CallShort();
    case 0x21: return MF ? IndexedIndirectRead8(&WDC65816::algorithmAND8) : IndexedIndirectRead16(&WDC65816::algorithmAND16);
    case 0x22: return CallLong();
    case 0x23: return MF ? StackRead8(&WDC65816::algorithmAND8) : StackRead16(&WDC65816::algorithmAND16);
    case 0x24: return MF ? DirectRead8(&WDC65816::algorithmBIT8) : DirectRead16(&WDC65816::algorithmBIT16);
    case 0x25: return MF ? DirectRead8(&WDC65816::algorithmAND8) : DirectRead16(&WDC65816::algorithmAND16);
    case 0x26: return MF ? DirectModify8(&WDC65816::algorithmROL8) : DirectModify16(&WDC65816::algorithmROL16);
    case 0x27: return MF ? IndirectLongRead8(&WDC65816::algorithmAND8) : IndirectLongRead16(&WDC65816::algorithmAND16);
    case 0x28: return PullP();
    case 0x29: return MF ? ImmediateRead8(&WDC65816::algorithmAND8) : ImmediateRead16(&WDC65816::algorithmAND16);
    case 0x2a: return MF ? ImpliedModify8(&WDC65816::algorithmROL8, A) : ImpliedModify16(&WDC65816::algorithmROL16, A);
    case 0x2b: return PullD();
    case 0x2c: return MF ? BankRead8(&WDC65816::algorithmBIT8) : BankRead16(&WDC65816::algorithmBIT16);
    case 0x2d: return MF ? BankRead8(&WDC65816::algorithmAND8) : BankRead16(&WDC65816::algorithmAND16);
    case 0x2e: return MF ? BankModify8(&WDC65816::algorithmROL8) : BankModify16(&WDC65816::algorithmROL16);
    case 0x2f: return MF ? LongRead8(&WDC65816::algorithmAND8) : LongRead16(&WDC65816::algorithmAND16);
    case 0x30: return Branch(NF == 1);
    case 0x31: return MF ? IndirectIndexedRead8(&WDC65816::algorithmAND8) : IndirectIndexedRead16(&WDC65816::algorithmAND16);
    case 0x32: return MF ? IndirectRead8(&WDC65816::algorithmAND8) : IndirectRead16(&WDC65816::algorithmAND16);
    case 0x33: return MF ? IndirectStackRead8(&WDC65816::algorithmAND8) : IndirectStackRead16(&WDC65816::algorithmAND16);
    case 0x34: return MF ? DirectRead8(&WDC65816::algorithmBIT8, X) : DirectRead16(&WDC65816::algorithmBIT16, X);
    case 0x35: return MF ? DirectRead8(&WDC65816::algorithmAND8, X) : DirectRead16(&WDC65816::algorithmAND16, X);
    case 0x36: return MF ? DirectIndexedModify8(&WDC65816::algorithmROL8) : DirectIndexedModify16(&WDC65816::algorithmROL16);
    case 0x37: return MF ? IndirectLongRead8(&WDC65816::algorithmAND8, Y) : IndirectLongRead16(&WDC65816::algorithmAND16, Y);
    case 0x38: return SetFlag(CF);
    case 0x39: return MF ? BankRead8(&WDC65816::algorithmAND8, Y) : BankRead16(&WDC65816::algorithmAND16, Y);
    case 0x3a: return MF ? ImpliedModify8(&WDC65816::algorithmDEC8, A) : ImpliedModify16(&WDC65816::algorithmDEC16, A);
    case 0x3b: return Transfer16(S, A);
    case 0x3c: return MF ? BankRead8(&WDC65816::algorithmBIT8, X) : BankRead16(&WDC65816::algorithmBIT16, X);
    case 0x3d: return MF ? BankRead8(&WDC65816::algorithmAND8, X) : BankRead16(&WDC65816::algorithmAND16, X);
    case 0x3e: return MF ? BankIndexedModify8(&WDC65816::algorithmROL8) : BankIndexedModify16(&WDC65816::algorithmROL16);
    case 0x3f: return MF ? LongRead8(&WDC65816::algorithmAND8, X) : LongRead16(&WDC65816::algorithmAND16, X);
    case 0x40: return ReturnInterrupt();
    case 0x41: return MF ? IndexedIndirectRead8(&WDC65816::algorithmEOR8) : IndexedIndirectRead16(&WDC65816::algorithmEOR16);
    case 0x42: return Prefix();
    case 0x43: return MF ? StackRead8(&WDC65816::algorithmEOR8) : StackRead16(&WDC65816::algorithmEOR16);
    case 0x44: return XF ? BlockMove8(-1) : BlockMove16(-1);
    case 0x45: return MF ? DirectRead8(&WDC65816::algorithmEOR8) : DirectRead16(&WDC65816::algorithmEOR16);
    case 0x46: return MF ? DirectModify8(&WDC65816::algorithmLSR8) : DirectModify16(&WDC65816::algorithmLSR16);
    case 0x47: return MF ? IndirectLongRead8(&WDC65816::algorithmEOR8) : IndirectLongRead16(&WDC65816::algorithmEOR16);
    case 0x48: return MF ? Push8(A) : Push16(A);
    case 0x49: return MF ? ImmediateRead8(&WDC65816::algorithmEOR8) : ImmediateRead16(&WDC65816::algorithmEOR16);
    case 0x4a: return MF ? ImpliedModify8(&WDC65816::algorithmLSR8, A) : ImpliedModify16(&WDC65816::algorithmLSR16, A);
    case 0x4b: return Push8((r16)PC.r24_lsb4.b);
    case 0x4c: return JumpShort();
    case 0x4d: return MF ? BankRead8(&WDC65816::algorithmEOR8) : BankRead16(&WDC65816::algorithmEOR16);
    case 0x4e: return MF ? BankModify8(&WDC65816::algorithmLSR8) : BankModify16(&WDC65816::algorithmLSR16);
    case 0x4f: return MF ? LongRead8(&WDC65816::algorithmEOR8) : LongRead16(&WDC65816::algorithmEOR16);
    case 0x50: return Branch(VF == 0);
    case 0x51: return MF ? IndirectIndexedRead8(&WDC65816::algorithmEOR8) : IndirectIndexedRead16(&WDC65816::algorithmEOR16);
    case 0x52: return MF ? IndirectRead8(&WDC65816::algorithmEOR8) : IndirectRead16(&WDC65816::algorithmEOR16);
    case 0x53: return MF ? IndirectStackRead8(&WDC65816::algorithmEOR8) : IndirectStackRead16(&WDC65816::algorithmEOR16);
    case 0x54: return XF ? BlockMove8(+1) : BlockMove16(+1);
    case 0x55: return MF ? DirectRead8(&WDC65816::algorithmEOR8, X) : DirectRead16(&WDC65816::algorithmEOR16, X);
    case 0x56: return MF ? DirectIndexedModify8(&WDC65816::algorithmLSR8) : DirectIndexedModify16(&WDC65816::algorithmLSR16);
    case 0x57: return MF ? IndirectLongRead8(&WDC65816::algorithmEOR8, Y) : IndirectLongRead16(&WDC65816::algorithmEOR16, Y);
    case 0x58: return ClearFlag(IF);
    case 0x59: return MF ? BankRead8(&WDC65816::algorithmEOR8, Y) : BankRead16(&WDC65816::algorithmEOR16, Y);
    case 0x5a: return XF ? Push8(Y) : Push16(Y);
    case 0x5b: return Transfer16(A, D);
    case 0x5c: return JumpLong();
    case 0x5d: return MF ? BankRead8(&WDC65816::algorithmEOR8, X) : BankRead16(&WDC65816::algorithmEOR16, X);
    case 0x5e: return MF ? BankIndexedModify8(&WDC65816::algorithmLSR8) : BankIndexedModify16(&WDC65816::algorithmLSR16);
    case 0x5f: return MF ? LongRead8(&WDC65816::algorithmEOR8, X) : LongRead16(&WDC65816::algorithmEOR16, X);
    case 0x60: return ReturnShort();
    case 0x61: return MF ? IndexedIndirectRead8(&WDC65816::algorithmADC8) : IndexedIndirectRead16(&WDC65816::algorithmADC16);
    case 0x62: return PushEffectiveRelativeAddress();
    case 0x63: return MF ? StackRead8(&WDC65816::algorithmADC8) : StackRead16(&WDC65816::algorithmADC16);
    case 0x64: return MF ? DirectWrite8(Z) : DirectWrite16(Z);
    case 0x65: return MF ? DirectRead8(&WDC65816::algorithmADC8) : DirectRead16(&WDC65816::algorithmADC16);
    case 0x66: return MF ? DirectModify8(&WDC65816::algorithmROR8) : DirectModify16(&WDC65816::algorithmROR16);
    case 0x67: return MF ? IndirectLongRead8(&WDC65816::algorithmADC8) : IndirectLongRead16(&WDC65816::algorithmADC16);
    case 0x68: return MF ? Pull8(A) : Pull16(A);
    case 0x69: return MF ? ImmediateRead8(&WDC65816::algorithmADC8) : ImmediateRead16(&WDC65816::algorithmADC16);
    case 0x6a: return MF ? ImpliedModify8(&WDC65816::algorithmROR8, A) : ImpliedModify16(&WDC65816::algorithmROR16, A);
    case 0x6b: return ReturnLong();
    case 0x6c: return JumpIndirect();
    case 0x6d: return MF ? BankRead8(&WDC65816::algorithmADC8) : BankRead16(&WDC65816::algorithmADC16);
    case 0x6e: return MF ? BankModify8(&WDC65816::algorithmROR8) : BankModify16(&WDC65816::algorithmROR16);
    case 0x6f: return MF ? LongRead8(&WDC65816::algorithmADC8) : LongRead16(&WDC65816::algorithmADC16);
    case 0x70: return Branch(VF == 1);
    case 0x71: return MF ? IndirectIndexedRead8(&WDC65816::algorithmADC8) : IndirectIndexedRead16(&WDC65816::algorithmADC16);
    case 0x72: return MF ? IndirectRead8(&WDC65816::algorithmADC8) : IndirectRead16(&WDC65816::algorithmADC16);
    case 0x73: return MF ? IndirectStackRead8(&WDC65816::algorithmADC8) : IndirectStackRead16(&WDC65816::algorithmADC16);
    case 0x74: return MF ? DirectWrite8(Z, X) : DirectWrite16(Z, X);
    case 0x75: return MF ? DirectRead8(&WDC65816::algorithmADC8, X) : DirectRead16(&WDC65816::algorithmADC16, X);
    case 0x76: return MF ? DirectIndexedModify8(&WDC65816::algorithmROR8) : DirectIndexedModify16(&WDC65816::algorithmROR16);
    case 0x77: return MF ? IndirectLongRead8(&WDC65816::algorithmADC8, Y) : IndirectLongRead16(&WDC65816::algorithmADC16, Y);
    case 0x78: return SetFlag(IF);
    case 0x79: return MF ? BankRead8(&WDC65816::algorithmADC8, Y) : BankRead16(&WDC65816::algorithmADC16, Y);
    case 0x7a: return XF ? Pull8(Y) : Pull16(Y);
    case 0x7b: return Transfer16(D, A);
    case 0x7c: return JumpIndexedIndirect();
    case 0x7d: return MF ? BankRead8(&WDC65816::algorithmADC8, X) : BankRead16(&WDC65816::algorithmADC16, X);
    case 0x7e: return MF ? BankIndexedModify8(&WDC65816::algorithmROR8) : BankIndexedModify16(&WDC65816::algorithmROR16);
    case 0x7f: return MF ? LongRead8(&WDC65816::algorithmADC8, X) : LongRead16(&WDC65816::algorithmADC16, X);
    case 0x80: return Branch();
    case 0x81: return MF ? IndexedIndirectWrite8() : IndexedIndirectWrite16();
    case 0x82: return BranchLong();
    case 0x83: return MF ? StackWrite8() : StackWrite16();
    case 0x84: return XF ? DirectWrite8(Y) : DirectWrite16(Y);
    case 0x85: return MF ? DirectWrite8(A) : DirectWrite16(A);
    case 0x86: return XF ? DirectWrite8(X) : DirectWrite16(X);
    case 0x87: return MF ? IndirectLongWrite8() : IndirectLongWrite16();
    case 0x88: return XF ? ImpliedModify8(&WDC65816::algorithmDEC8, Y) : ImpliedModify16(&WDC65816::algorithmDEC16, Y);
    case 0x89: return MF ? BitImmediate8() : BitImmediate16();
    case 0x8a: return MF ? Transfer8(X, A) : Transfer16(X, A);
    case 0x8b: return Push8((r16)B);
    case 0x8c: return XF ? BankWrite8(Y) : BankWrite16(Y);
    case 0x8d: return MF ? BankWrite8(A) : BankWrite16(A);
    case 0x8e: return XF ? BankWrite8(X) : BankWrite16(X);
    case 0x8f: return MF ? LongWrite8() : LongWrite16();
    case 0x90: return Branch(CF == 0);
    case 0x91: return MF ? IndirectIndexedWrite8() : IndirectIndexedWrite16();
    case 0x92: return MF ? IndirectWrite8() : IndirectWrite16();
    case 0x93: return MF ? IndirectStackWrite8() : IndirectStackWrite16();
    case 0x94: return XF ? DirectWrite8(Y, X) : DirectWrite16(Y, X);
    case 0x95: return MF ? DirectWrite8(A, X) : DirectWrite16(A, X);
    case 0x96: return XF ? DirectWrite8(X, Y) : DirectWrite16(X, Y);
    case 0x97: return MF ? IndirectLongWrite8(Y) : IndirectLongWrite16(Y);
    case 0x98: return MF ? Transfer8(Y, A) : Transfer16(Y, A);
    case 0x99: return MF ? BankWrite8(A, Y) : BankWrite16(A, Y);
    case 0x9a: return TransferXS();
    case 0x9b: return XF ? Transfer8(X, Y) : Transfer16(X, Y);
    case 0x9c: return MF ? BankWrite8(Z) : BankWrite16(Z);
    case 0x9d: return MF ? BankWrite8(A, X) : BankWrite16(A, X);
    case 0x9e: return MF ? BankWrite8(Z, X) : BankWrite16(Z, X);
    case 0x9f: return MF ? LongWrite8(X) : LongWrite16(X);
    case 0xa0: return XF ? ImmediateRead8(&WDC65816::algorithmLDY8) : ImmediateRead16(&WDC65816::algorithmLDY16);
    case 0xa1: return MF ? IndexedIndirectRead8(&WDC65816::algorithmLDA8) : IndexedIndirectRead16(&WDC65816::algorithmLDA16);
    case 0xa2: return XF ? ImmediateRead8(&WDC65816::algorithmLDX8) : ImmediateRead16(&WDC65816::algorithmLDX16);
    case 0xa3: return MF ? StackRead8(&WDC65816::algorithmLDA8) : StackRead16(&WDC65816::algorithmLDA16);
    case 0xa4: return XF ? DirectRead8(&WDC65816::algorithmLDY8) : DirectRead16(&WDC65816::algorithmLDY16);
    case 0xa5: return MF ? DirectRead8(&WDC65816::algorithmLDA8) : DirectRead16(&WDC65816::algorithmLDA16);
    case 0xa6: return XF ? DirectRead8(&WDC65816::algorithmLDX8) : DirectRead16(&WDC65816::algorithmLDX16);
    case 0xa7: return MF ? IndirectLongRead8(&WDC65816::algorithmLDA8) : IndirectLongRead16(&WDC65816::algorithmLDA16);
    case 0xa8: return XF ? Transfer8(A, Y) : Transfer16(A, Y);
    case 0xa9: return MF ? ImmediateRead8(&WDC65816::algorithmLDA8) : ImmediateRead16(&WDC65816::algorithmLDA16);
    case 0xaa: return XF ? Transfer8(A, X) : Transfer16(A, X);
    case 0xab: return PullB();
    case 0xac: return XF ? BankRead8(&WDC65816::algorithmLDY8) : BankRead16(&WDC65816::algorithmLDY16);
    case 0xad: return MF ? BankRead8(&WDC65816::algorithmLDA8) : BankRead16(&WDC65816::algorithmLDA16);
    case 0xae: return XF ? BankRead8(&WDC65816::algorithmLDX8) : BankRead16(&WDC65816::algorithmLDX16);
    case 0xaf: return MF ? LongRead8(&WDC65816::algorithmLDA8) : LongRead16(&WDC65816::algorithmLDA16);
    case 0xb0: return Branch(CF == 1);
    case 0xb1: return MF ? IndirectIndexedRead8(&WDC65816::algorithmLDA8) : IndirectIndexedRead16(&WDC65816::algorithmLDA16);
    case 0xb2: return MF ? IndirectRead8(&WDC65816::algorithmLDA8) : IndirectRead16(&WDC65816::algorithmLDA16);
    case 0xb3: return MF ? IndirectStackRead8(&WDC65816::algorithmLDA8) : IndirectStackRead16(&WDC65816::algorithmLDA16);
    case 0xb4: return XF ? DirectRead8(&WDC65816::algorithmLDY8, X) : DirectRead16(&WDC65816::algorithmLDY16, X);
    case 0xb5: return MF ? DirectRead8(&WDC65816::algorithmLDA8, X) : DirectRead16(&WDC65816::algorithmLDA16, X);
    case 0xb6: return XF ? DirectRead8(&WDC65816::algorithmLDX8, Y) : DirectRead16(&WDC65816::algorithmLDX16, Y);
    case 0xb7: return MF ? IndirectLongRead8(&WDC65816::algorithmLDA8, Y) : IndirectLongRead16(&WDC65816::algorithmLDA16, Y);
    case 0xb8: return ClearFlag(VF);
    case 0xb9: return MF ? BankRead8(&WDC65816::algorithmLDA8, Y) : BankRead16(&WDC65816::algorithmLDA16, Y);
    case 0xba: return XF ? TransferSX8() : TransferSX16();
    case 0xbb: return XF ? Transfer8(Y, X) : Transfer16(Y, X);
    case 0xbc: return XF ? BankRead8(&WDC65816::algorithmLDY8, X) : BankRead16(&WDC65816::algorithmLDY16, X);
    case 0xbd: return MF ? BankRead8(&WDC65816::algorithmLDA8, X) : BankRead16(&WDC65816::algorithmLDA16, X);
    case 0xbe: return XF ? BankRead8(&WDC65816::algorithmLDX8, Y) : BankRead16(&WDC65816::algorithmLDX16, Y);
    case 0xbf: return MF ? LongRead8(&WDC65816::algorithmLDA8, X) : LongRead16(&WDC65816::algorithmLDA16, X);
    case 0xc0: return XF ? ImmediateRead8(&WDC65816::algorithmCPY8) : ImmediateRead16(&WDC65816::algorithmCPY16);
    case 0xc1: return MF ? IndexedIndirectRead8(&WDC65816::algorithmCMP8) : IndexedIndirectRead16(&WDC65816::algorithmCMP16);
    case 0xc2: return ResetP();
    case 0xc3: return MF ? StackRead8(&WDC65816::algorithmCMP8) : StackRead16(&WDC65816::algorithmCMP16);
    case 0xc4: return XF ? DirectRead8(&WDC65816::algorithmCPY8) : DirectRead16(&WDC65816::algorithmCPY16);
    case 0xc5: return MF ? DirectRead8(&WDC65816::algorithmCMP8) : DirectRead16(&WDC65816::algorithmCMP16);
    case 0xc6: return MF ? DirectModify8(&WDC65816::algorithmDEC8) : DirectModify16(&WDC65816::algorithmDEC16);
    case 0xc7: return MF ? IndirectLongRead8(&WDC65816::algorithmCMP8) : IndirectLongRead16(&WDC65816::algorithmCMP16);
    case 0xc8: return XF ? ImpliedModify8(&WDC65816::algorithmINC8, Y) : ImpliedModify16(&WDC65816::algorithmINC16, Y);
    case 0xc9: return MF ? ImmediateRead8(&WDC65816::algorithmCMP8) : ImmediateRead16(&WDC65816::algorithmCMP16);
    case 0xca: return XF ? ImpliedModify8(&WDC65816::algorithmDEC8, X) : ImpliedModify16(&WDC65816::algorithmDEC16, X);
    case 0xcb: return instructionWait();
    case 0xcc: return XF ? BankRead8(&WDC65816::algorithmCPY8) : BankRead16(&WDC65816::algorithmCPY16);
    case 0xcd: return MF ? BankRead8(&WDC65816::algorithmCMP8) : BankRead16(&WDC65816::algorithmCMP16);
    case 0xce: return MF ? BankModify8(&WDC65816::algorithmDEC8) : BankModify16(&WDC65816::algorithmDEC16);
    case 0xcf: return MF ? LongRead8(&WDC65816::algorithmCMP8) : LongRead16(&WDC65816::algorithmCMP16);
    case 0xd0: return Branch(ZF == 0);
    case 0xd1: return MF ? IndirectIndexedRead8(&WDC65816::algorithmCMP8) : IndirectIndexedRead16(&WDC65816::algorithmCMP16);
    case 0xd2: return MF ? IndirectRead8(&WDC65816::algorithmCMP8) : IndirectRead16(&WDC65816::algorithmCMP16);
    case 0xd3: return MF ? IndirectStackRead8(&WDC65816::algorithmCMP8) : IndirectStackRead16(&WDC65816::algorithmCMP16);
    case 0xd4: return PushEffectiveIndirectAddress();
    case 0xd5: return MF ? DirectRead8(&WDC65816::algorithmCMP8, X) : DirectRead16(&WDC65816::algorithmCMP16, X);
    case 0xd6: return MF ? DirectIndexedModify8(&WDC65816::algorithmDEC8) : DirectIndexedModify16(&WDC65816::algorithmDEC16);
    case 0xd7: return MF ? IndirectLongRead8(&WDC65816::algorithmCMP8, Y) : IndirectLongRead16(&WDC65816::algorithmCMP16, Y);
    case 0xd8: return ClearFlag(DF);
    case 0xd9: return MF ? BankRead8(&WDC65816::algorithmCMP8, Y) : BankRead16(&WDC65816::algorithmCMP16, Y);
    case 0xda: return XF ? Push8(X) : Push16(X);
    case 0xdb: return instructionStop();
    case 0xdc: return JumpIndirectLong();
    case 0xdd: return MF ? BankRead8(&WDC65816::algorithmCMP8, X) : BankRead16(&WDC65816::algorithmCMP16, X);
    case 0xde: return MF ? BankIndexedModify8(&WDC65816::algorithmDEC8) : BankIndexedModify16(&WDC65816::algorithmDEC16);
    case 0xdf: return MF ? LongRead8(&WDC65816::algorithmCMP8, X) : LongRead16(&WDC65816::algorithmCMP16, X);
    case 0xe0: return XF ? ImmediateRead8(&WDC65816::algorithmCPX8) : ImmediateRead16(&WDC65816::algorithmCPX16);
    case 0xe1: return MF ? IndexedIndirectRead8(&WDC65816::algorithmSBC8) : IndexedIndirectRead16(&WDC65816::algorithmSBC16);
    case 0xe2: return SetP();
    case 0xe3: return MF ? StackRead8(&WDC65816::algorithmSBC8) : StackRead16(&WDC65816::algorithmSBC16);
    case 0xe4: return XF ? DirectRead8(&WDC65816::algorithmCPX8) : DirectRead16(&WDC65816::algorithmCPX16);
    case 0xe5: return MF ? DirectRead8(&WDC65816::algorithmSBC8) : DirectRead16(&WDC65816::algorithmSBC16);
    case 0xe6: return MF ? DirectModify8(&WDC65816::algorithmINC8) : DirectModify16(&WDC65816::algorithmINC16);
    case 0xe7: return MF ? IndirectLongRead8(&WDC65816::algorithmSBC8) : IndirectLongRead16(&WDC65816::algorithmSBC16);
    case 0xe8: return XF ? ImpliedModify8(&WDC65816::algorithmINC8, X) : ImpliedModify16(&WDC65816::algorithmINC16, X);
    case 0xe9: return MF ? ImmediateRead8(&WDC65816::algorithmSBC8) : ImmediateRead16(&WDC65816::algorithmSBC16);
    case 0xea: return NoOperation();
    case 0xeb: return ExchangeBA();
    case 0xec: return XF ? BankRead8(&WDC65816::algorithmCPX8) : BankRead16(&WDC65816::algorithmCPX16);
    case 0xed: return MF ? BankRead8(&WDC65816::algorithmSBC8) : BankRead16(&WDC65816::algorithmSBC16);
    case 0xee: return MF ? BankModify8(&WDC65816::algorithmINC8) : BankModify16(&WDC65816::algorithmINC16);
    case 0xef: return MF ? LongRead8(&WDC65816::algorithmSBC8) : LongRead16(&WDC65816::algorithmSBC16);
    case 0xf0: return Branch(ZF == 1);
    case 0xf1: return MF ? IndirectIndexedRead8(&WDC65816::algorithmSBC8) : IndirectIndexedRead16(&WDC65816::algorithmSBC16);
    case 0xf2: return MF ? IndirectRead8(&WDC65816::algorithmSBC8) : IndirectRead16(&WDC65816::algorithmSBC16);
    case 0xf3: return MF ? IndirectStackRead8(&WDC65816::algorithmSBC8) : IndirectStackRead16(&WDC65816::algorithmSBC16);
    case 0xf4: return PushEffectiveAddress();
    case 0xf5: return MF ? DirectRead8(&WDC65816::algorithmSBC8, X) : DirectRead16(&WDC65816::algorithmSBC16, X);
    case 0xf6: return MF ? DirectIndexedModify8(&WDC65816::algorithmINC8) : DirectIndexedModify16(&WDC65816::algorithmINC16);
    case 0xf7: return MF ? IndirectLongRead8(&WDC65816::algorithmSBC8, Y) : IndirectLongRead16(&WDC65816::algorithmSBC16, Y);
    case 0xf8: return SetFlag(DF);
    case 0xf9: return MF ? BankRead8(&WDC65816::algorithmSBC8, Y) : BankRead16(&WDC65816::algorithmSBC16, Y);
    case 0xfa: return XF ? Pull8(X) : Pull16(X);
    case 0xfb: return ExchangeCE();
    case 0xfc: return CallIndexedIndirect();
    case 0xfd: return MF ? BankRead8(&WDC65816::algorithmSBC8, X) : BankRead16(&WDC65816::algorithmSBC16, X);
    case 0xfe: return MF ? BankIndexedModify8(&WDC65816::algorithmINC8) : BankIndexedModify16(&WDC65816::algorithmINC16);
    case 0xff: return MF ? LongRead8(&WDC65816::algorithmSBC8, X) : LongRead16(&WDC65816::algorithmSBC16, X);
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
