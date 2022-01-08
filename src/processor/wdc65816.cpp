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
  return read((D.w + address) & 0xffff);
}

void WDC65816::writeDirect(unsigned address, uint8_t data) {
  if(EF && !D.l) return write(D.w | (address & 0xff), data);
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

}
