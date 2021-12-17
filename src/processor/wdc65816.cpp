#include <emulator/emulator.hpp>
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

#define E if(r.e)
#define N if(!r.e)
#define L lastCycle();

#define alu(...) (this->*op)(__VA_ARGS__)

//immediate, 2-cycle opcodes with idle cycle will become bus read
//when an IRQ is to be triggered immediately after opcode completion.
//this affects the following opcodes:
//  clc, cld, cli, clv, sec, sed, sei,
//  tax, tay, txa, txy, tya, tyx,
//  tcd, tcs, tdc, tsc, tsx, txs,
//  inc, inx, iny, dec, dex, dey,
//  asl, lsr, rol, ror, nop, xce.
auto WDC65816::idleIRQ() -> void {
  if(interruptPending()) {
    //modify I/O cycle to bus read cycle, do not increment PC
    read(PC.d);
  } else {
    idle();
  }
}

auto WDC65816::idle2() -> void {
  if(D.l) idle();
}

auto WDC65816::idle4(uint16_t x, uint16_t y) -> void {
  if(!XF || x >> 8 != y >> 8) idle();
}

auto WDC65816::idle6(uint16_t address) -> void {
  if(EF && PC.h != address >> 8) idle();
}

auto WDC65816::fetch() -> uint8_t {
  return read(PC.b << 16 | PC.w++);
}

auto WDC65816::pull() -> uint8_t {
  EF ? (void)S.l++ : (void)S.w++;
  return read(S.w);
}

auto WDC65816::push(uint8_t data) -> void {
  write(S.w, data);
  EF ? (void)S.l-- : (void)S.w--;
}

auto WDC65816::pullN() -> uint8_t {
  return read(++S.w);
}

auto WDC65816::pushN(uint8_t data) -> void {
  write(S.w--, data);
}

auto WDC65816::readDirect(unsigned address) -> uint8_t {
  if(EF && !D.l) return read(D.w | address & 0xff);
  return read(D.w + address & 0xffff);
}

auto WDC65816::writeDirect(unsigned address, uint8_t data) -> void {
  if(EF && !D.l) return write(D.w | address & 0xff, data);
  write(D.w + address & 0xffff, data);
}

auto WDC65816::readDirectN(unsigned address) -> uint8_t {
  return read(D.w + address & 0xffff);
}

auto WDC65816::readBank(unsigned address) -> uint8_t {
  return read((B << 16) + address & 0xffffff);
}

auto WDC65816::writeBank(unsigned address, uint8_t data) -> void {
  write((B << 16) + address & 0xffffff, data);
}

auto WDC65816::readLong(unsigned address) -> uint8_t {
  return read(address & 0xffffff);
}

auto WDC65816::writeLong(unsigned address, uint8_t data) -> void {
  write(address & 0xffffff, data);
}

auto WDC65816::readStack(unsigned address) -> uint8_t {
  return read(S.w + address & 0xffff);
}

auto WDC65816::writeStack(unsigned address, uint8_t data) -> void {
  write(S.w + address & 0xffff, data);
}

auto WDC65816::algorithmADC8(uint8_t data) -> uint8_t {
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

auto WDC65816::algorithmADC16(uint16_t data) -> uint16_t {
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

auto WDC65816::algorithmAND8(uint8_t data) -> uint8_t {
  A.l &= data;
  ZF = A.l == 0;
  NF = A.l & 0x80;
  return A.l;
}

auto WDC65816::algorithmAND16(uint16_t data) -> uint16_t {
  A.w &= data;
  ZF = A.w == 0;
  NF = A.w & 0x8000;
  return A.w;
}

auto WDC65816::algorithmASL8(uint8_t data) -> uint8_t {
  CF = data & 0x80;
  data <<= 1;
  ZF = data == 0;
  NF = data & 0x80;
  return data;
}

auto WDC65816::algorithmASL16(uint16_t data) -> uint16_t {
  CF = data & 0x8000;
  data <<= 1;
  ZF = data == 0;
  NF = data & 0x8000;
  return data;
}

auto WDC65816::algorithmBIT8(uint8_t data) -> uint8_t {
  ZF = (data & A.l) == 0;
  VF = data & 0x40;
  NF = data & 0x80;
  return data;
}

auto WDC65816::algorithmBIT16(uint16_t data) -> uint16_t {
  ZF = (data & A.w) == 0;
  VF = data & 0x4000;
  NF = data & 0x8000;
  return data;
}

auto WDC65816::algorithmCMP8(uint8_t data) -> uint8_t {
  int result = A.l - data;
  CF = result >= 0;
  ZF = (uint8_t)result == 0;
  NF = result & 0x80;
  return result;
}

auto WDC65816::algorithmCMP16(uint16_t data) -> uint16_t {
  int result = A.w - data;
  CF = result >= 0;
  ZF = (uint16_t)result == 0;
  NF = result & 0x8000;
  return result;
}

auto WDC65816::algorithmCPX8(uint8_t data) -> uint8_t {
  int result = X.l - data;
  CF = result >= 0;
  ZF = (uint8_t)result == 0;
  NF = result & 0x80;
  return result;
}

auto WDC65816::algorithmCPX16(uint16_t data) -> uint16_t {
  int result = X.w - data;
  CF = result >= 0;
  ZF = (uint16_t)result == 0;
  NF = result & 0x8000;
  return result;
}

auto WDC65816::algorithmCPY8(uint8_t data) -> uint8_t {
  int result = Y.l - data;
  CF = result >= 0;
  ZF = (uint8_t)result == 0;
  NF = result & 0x80;
  return result;
}

auto WDC65816::algorithmCPY16(uint16_t data) -> uint16_t {
  int result = Y.w - data;
  CF = result >= 0;
  ZF = (uint16_t)result == 0;
  NF = result & 0x8000;
  return result;
}

auto WDC65816::algorithmDEC8(uint8_t data) -> uint8_t {
  data--;
  ZF = data == 0;
  NF = data & 0x80;
  return data;
}

auto WDC65816::algorithmDEC16(uint16_t data) -> uint16_t {
  data--;
  ZF = data == 0;
  NF = data & 0x8000;
  return data;
}

auto WDC65816::algorithmEOR8(uint8_t data) -> uint8_t {
  A.l ^= data;
  ZF = A.l == 0;
  NF = A.l & 0x80;
  return A.l;
}

auto WDC65816::algorithmEOR16(uint16_t data) -> uint16_t {
  A.w ^= data;
  ZF = A.w == 0;
  NF = A.w & 0x8000;
  return A.w;
}

auto WDC65816::algorithmINC8(uint8_t data) -> uint8_t {
  data++;
  ZF = data == 0;
  NF = data & 0x80;
  return data;
}

auto WDC65816::algorithmINC16(uint16_t data) -> uint16_t {
  data++;
  ZF = data == 0;
  NF = data & 0x8000;
  return data;
}

auto WDC65816::algorithmLDA8(uint8_t data) -> uint8_t {
  A.l = data;
  ZF = A.l == 0;
  NF = A.l & 0x80;
  return data;
}

auto WDC65816::algorithmLDA16(uint16_t data) -> uint16_t {
  A.w = data;
  ZF = A.w == 0;
  NF = A.w & 0x8000;
  return data;
}

auto WDC65816::algorithmLDX8(uint8_t data) -> uint8_t {
  X.l = data;
  ZF = X.l == 0;
  NF = X.l & 0x80;
  return data;
}

auto WDC65816::algorithmLDX16(uint16_t data) -> uint16_t {
  X.w = data;
  ZF = X.w == 0;
  NF = X.w & 0x8000;
  return data;
}

auto WDC65816::algorithmLDY8(uint8_t data) -> uint8_t {
  Y.l = data;
  ZF = Y.l == 0;
  NF = Y.l & 0x80;
  return data;
}

auto WDC65816::algorithmLDY16(uint16_t data) -> uint16_t {
  Y.w = data;
  ZF = Y.w == 0;
  NF = Y.w & 0x8000;
  return data;
}

auto WDC65816::algorithmLSR8(uint8_t data) -> uint8_t {
  CF = data & 1;
  data >>= 1;
  ZF = data == 0;
  NF = data & 0x80;
  return data;
}

auto WDC65816::algorithmLSR16(uint16_t data) -> uint16_t {
  CF = data & 1;
  data >>= 1;
  ZF = data == 0;
  NF = data & 0x8000;
  return data;
}

auto WDC65816::algorithmORA8(uint8_t data) -> uint8_t {
  A.l |= data;
  ZF = A.l == 0;
  NF = A.l & 0x80;
  return A.l;
}

auto WDC65816::algorithmORA16(uint16_t data) -> uint16_t {
  A.w |= data;
  ZF = A.w == 0;
  NF = A.w & 0x8000;
  return A.w;
}

auto WDC65816::algorithmROL8(uint8_t data) -> uint8_t {
  bool carry = CF;
  CF = data & 0x80;
  data = data << 1 | carry;
  ZF = data == 0;
  NF = data & 0x80;
  return data;
}

auto WDC65816::algorithmROL16(uint16_t data) -> uint16_t {
  bool carry = CF;
  CF = data & 0x8000;
  data = data << 1 | carry;
  ZF = data == 0;
  NF = data & 0x8000;
  return data;
}

auto WDC65816::algorithmROR8(uint8_t data) -> uint8_t {
  bool carry = CF;
  CF = data & 1;
  data = carry << 7 | data >> 1;
  ZF = data == 0;
  NF = data & 0x80;
  return data;
}

auto WDC65816::algorithmROR16(uint16_t data) -> uint16_t {
  bool carry = CF;
  CF = data & 1;
  data = carry << 15 | data >> 1;
  ZF = data == 0;
  NF = data & 0x8000;
  return data;
}

auto WDC65816::algorithmSBC8(uint8_t data) -> uint8_t {
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

auto WDC65816::algorithmSBC16(uint16_t data) -> uint16_t {
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

auto WDC65816::algorithmTRB8(uint8_t data) -> uint8_t {
  ZF = (data & A.l) == 0;
  data &= ~A.l;
  return data;
}

auto WDC65816::algorithmTRB16(uint16_t data) -> uint16_t {
  ZF = (data & A.w) == 0;
  data &= ~A.w;
  return data;
}

auto WDC65816::algorithmTSB8(uint8_t data) -> uint8_t {
  ZF = (data & A.l) == 0;
  data |= A.l;
  return data;
}

auto WDC65816::algorithmTSB16(uint16_t data) -> uint16_t {
  ZF = (data & A.w) == 0;
  data |= A.w;
  return data;
}

auto WDC65816::instructionImmediateRead8(alu8 op) -> void {
L W.l = fetch();
  alu(W.l);
}

auto WDC65816::instructionImmediateRead16(alu16 op) -> void {
  W.l = fetch();
L W.h = fetch();
  alu(W.w);
}

auto WDC65816::instructionBankRead8(alu8 op) -> void {
  V.l = fetch();
  V.h = fetch();
L W.l = readBank(V.w + 0);
  alu(W.l);
}

auto WDC65816::instructionBankRead16(alu16 op) -> void {
  V.l = fetch();
  V.h = fetch();
  W.l = readBank(V.w + 0);
L W.h = readBank(V.w + 1);
  alu(W.w);
}

auto WDC65816::instructionBankRead8(alu8 op, r16 I) -> void {
  V.l = fetch();
  V.h = fetch();
  idle4(V.w, V.w + I.w);
L W.l = readBank(V.w + I.w + 0);
  alu(W.l);
}

auto WDC65816::instructionBankRead16(alu16 op, r16 I) -> void {
  V.l = fetch();
  V.h = fetch();
  idle4(V.w, V.w + I.w);
  W.l = readBank(V.w + I.w + 0);
L W.h = readBank(V.w + I.w + 1);
  alu(W.w);
}

auto WDC65816::instructionLongRead8(alu8 op, r16 I) -> void {
  V.l = fetch();
  V.h = fetch();
  V.b = fetch();
L W.l = readLong(V.d + I.w + 0);
  alu(W.l);
}

auto WDC65816::instructionLongRead16(alu16 op, r16 I) -> void {
  V.l = fetch();
  V.h = fetch();
  V.b = fetch();
  W.l = readLong(V.d + I.w + 0);
L W.h = readLong(V.d + I.w + 1);
  alu(W.w);
}

auto WDC65816::instructionDirectRead8(alu8 op) -> void {
  U.l = fetch();
  idle2();
L W.l = readDirect(U.l + 0);
  alu(W.l);
}

auto WDC65816::instructionDirectRead16(alu16 op) -> void {
  U.l = fetch();
  idle2();
  W.l = readDirect(U.l + 0);
L W.h = readDirect(U.l + 1);
  alu(W.w);
}

auto WDC65816::instructionDirectRead8(alu8 op, r16 I) -> void {
  U.l = fetch();
  idle2();
  idle();
L W.l = readDirect(U.l + I.w + 0);
  alu(W.l);
}

auto WDC65816::instructionDirectRead16(alu16 op, r16 I) -> void {
  U.l = fetch();
  idle2();
  idle();
  W.l = readDirect(U.l + I.w + 0);
L W.h = readDirect(U.l + I.w + 1);
  alu(W.w);
}

auto WDC65816::instructionIndirectRead8(alu8 op) -> void {
  U.l = fetch();
  idle2();
  V.l = readDirect(U.l + 0);
  V.h = readDirect(U.l + 1);
L W.l = readBank(V.w + 0);
  alu(W.l);
}

auto WDC65816::instructionIndirectRead16(alu16 op) -> void {
  U.l = fetch();
  idle2();
  V.l = readDirect(U.l + 0);
  V.h = readDirect(U.l + 1);
  W.l = readBank(V.w + 0);
L W.h = readBank(V.w + 1);
  alu(W.w);
}

auto WDC65816::instructionIndexedIndirectRead8(alu8 op) -> void {
  U.l = fetch();
  idle2();
  idle();
  V.l = readDirect(U.l + X.w + 0);
  V.h = readDirect(U.l + X.w + 1);
L W.l = readBank(V.w + 0);
  alu(W.l);
}

auto WDC65816::instructionIndexedIndirectRead16(alu16 op) -> void {
  U.l = fetch();
  idle2();
  idle();
  V.l = readDirect(U.l + X.w + 0);
  V.h = readDirect(U.l + X.w + 1);
  W.l = readBank(V.w + 0);
L W.h = readBank(V.w + 1);
  alu(W.w);
}

auto WDC65816::instructionIndirectIndexedRead8(alu8 op) -> void {
  U.l = fetch();
  idle2();
  V.l = readDirect(U.l + 0);
  V.h = readDirect(U.l + 1);
  idle4(V.w, V.w + Y.w);
L W.l = readBank(V.w + Y.w + 0);
  alu(W.l);
}

auto WDC65816::instructionIndirectIndexedRead16(alu16 op) -> void {
  U.l = fetch();
  idle2();
  V.l = readDirect(U.l + 0);
  V.h = readDirect(U.l + 1);
  idle4(V.w, V.w + Y.w);
  W.l = readBank(V.w + Y.w + 0);
L W.h = readBank(V.w + Y.w + 1);
  alu(W.w);
}

auto WDC65816::instructionIndirectLongRead8(alu8 op, r16 I) -> void {
  U.l = fetch();
  idle2();
  V.l = readDirectN(U.l + 0);
  V.h = readDirectN(U.l + 1);
  V.b = readDirectN(U.l + 2);
L W.l = readLong(V.d + I.w + 0);
  alu(W.l);
}

auto WDC65816::instructionIndirectLongRead16(alu16 op, r16 I) -> void {
  U.l = fetch();
  idle2();
  V.l = readDirectN(U.l + 0);
  V.h = readDirectN(U.l + 1);
  V.b = readDirectN(U.l + 2);
  W.l = readLong(V.d + I.w + 0);
L W.h = readLong(V.d + I.w + 1);
  alu(W.w);
}

auto WDC65816::instructionStackRead8(alu8 op) -> void {
  U.l = fetch();
  idle();
L W.l = readStack(U.l + 0);
  alu(W.l);
}

auto WDC65816::instructionStackRead16(alu16 op) -> void {
  U.l = fetch();
  idle();
  W.l = readStack(U.l + 0);
L W.h = readStack(U.l + 1);
  alu(W.w);
}

auto WDC65816::instructionIndirectStackRead8(alu8 op) -> void {
  U.l = fetch();
  idle();
  V.l = readStack(U.l + 0);
  V.h = readStack(U.l + 1);
  idle();
L W.l = readBank(V.w + Y.w + 0);
  alu(W.l);
}

auto WDC65816::instructionIndirectStackRead16(alu16 op) -> void {
  U.l = fetch();
  idle();
  V.l = readStack(U.l + 0);
  V.h = readStack(U.l + 1);
  idle();
  W.l = readBank(V.w + Y.w + 0);
L W.h = readBank(V.w + Y.w + 1);
  alu(W.w);
}

auto WDC65816::instructionBankWrite8(r16 F) -> void {
  V.l = fetch();
  V.h = fetch();
L writeBank(V.w + 0, F.l);
}

auto WDC65816::instructionBankWrite16(r16 F) -> void {
  V.l = fetch();
  V.h = fetch();
  writeBank(V.w + 0, F.l);
L writeBank(V.w + 1, F.h);
}

auto WDC65816::instructionBankWrite8(r16 F, r16 I) -> void {
  V.l = fetch();
  V.h = fetch();
  idle();
L writeBank(V.w + I.w + 0, F.l);
}

auto WDC65816::instructionBankWrite16(r16 F, r16 I) -> void {
  V.l = fetch();
  V.h = fetch();
  idle();
  writeBank(V.w + I.w + 0, F.l);
L writeBank(V.w + I.w + 1, F.h);
}

auto WDC65816::instructionLongWrite8(r16 I) -> void {
  V.l = fetch();
  V.h = fetch();
  V.b = fetch();
L writeLong(V.d + I.w + 0, A.l);
}

auto WDC65816::instructionLongWrite16(r16 I) -> void {
  V.l = fetch();
  V.h = fetch();
  V.b = fetch();
  writeLong(V.d + I.w + 0, A.l);
L writeLong(V.d + I.w + 1, A.h);
}

auto WDC65816::instructionDirectWrite8(r16 F) -> void {
  U.l = fetch();
  idle2();
L writeDirect(U.l + 0, F.l);
}

auto WDC65816::instructionDirectWrite16(r16 F) -> void {
  U.l = fetch();
  idle2();
  writeDirect(U.l + 0, F.l);
L writeDirect(U.l + 1, F.h);
}

auto WDC65816::instructionDirectWrite8(r16 F, r16 I) -> void {
  U.l = fetch();
  idle2();
  idle();
L writeDirect(U.l + I.w + 0, F.l);
}

auto WDC65816::instructionDirectWrite16(r16 F, r16 I) -> void {
  U.l = fetch();
  idle2();
  idle();
  writeDirect(U.l + I.w + 0, F.l);
L writeDirect(U.l + I.w + 1, F.h);
}

auto WDC65816::instructionIndirectWrite8() -> void {
  U.l = fetch();
  idle2();
  V.l = readDirect(U.l + 0);
  V.h = readDirect(U.l + 1);
L writeBank(V.w + 0, A.l);
}

auto WDC65816::instructionIndirectWrite16() -> void {
  U.l = fetch();
  idle2();
  V.l = readDirect(U.l + 0);
  V.h = readDirect(U.l + 1);
  writeBank(V.w + 0, A.l);
L writeBank(V.w + 1, A.h);
}

auto WDC65816::instructionIndexedIndirectWrite8() -> void {
  U.l = fetch();
  idle2();
  idle();
  V.l = readDirect(U.l + X.w + 0);
  V.h = readDirect(U.l + X.w + 1);
L writeBank(V.w + 0, A.l);
}

auto WDC65816::instructionIndexedIndirectWrite16() -> void {
  U.l = fetch();
  idle2();
  idle();
  V.l = readDirect(U.l + X.w + 0);
  V.h = readDirect(U.l + X.w + 1);
  writeBank(V.w + 0, A.l);
L writeBank(V.w + 1, A.h);
}

auto WDC65816::instructionIndirectIndexedWrite8() -> void {
  U.l = fetch();
  idle2();
  V.l = readDirect(U.l + 0);
  V.h = readDirect(U.l + 1);
  idle();
L writeBank(V.w + Y.w + 0, A.l);
}

auto WDC65816::instructionIndirectIndexedWrite16() -> void {
  U.l = fetch();
  idle2();
  V.l = readDirect(U.l + 0);
  V.h = readDirect(U.l + 1);
  idle();
  writeBank(V.w + Y.w + 0, A.l);
L writeBank(V.w + Y.w + 1, A.h);
}

auto WDC65816::instructionIndirectLongWrite8(r16 I) -> void {
  U.l = fetch();
  idle2();
  V.l = readDirectN(U.l + 0);
  V.h = readDirectN(U.l + 1);
  V.b = readDirectN(U.l + 2);
L writeLong(V.d + I.w + 0, A.l);
}

auto WDC65816::instructionIndirectLongWrite16(r16 I) -> void {
  U.l = fetch();
  idle2();
  V.l = readDirectN(U.l + 0);
  V.h = readDirectN(U.l + 1);
  V.b = readDirectN(U.l + 2);
  writeLong(V.d + I.w + 0, A.l);
L writeLong(V.d + I.w + 1, A.h);
}

auto WDC65816::instructionStackWrite8() -> void {
  U.l = fetch();
  idle();
L writeStack(U.l + 0, A.l);
}

auto WDC65816::instructionStackWrite16() -> void {
  U.l = fetch();
  idle();
  writeStack(U.l + 0, A.l);
L writeStack(U.l + 1, A.h);
}

auto WDC65816::instructionIndirectStackWrite8() -> void {
  U.l = fetch();
  idle();
  V.l = readStack(U.l + 0);
  V.h = readStack(U.l + 1);
  idle();
L writeBank(V.w + Y.w + 0, A.l);
}

auto WDC65816::instructionIndirectStackWrite16() -> void {
  U.l = fetch();
  idle();
  V.l = readStack(U.l + 0);
  V.h = readStack(U.l + 1);
  idle();
  writeBank(V.w + Y.w + 0, A.l);
L writeBank(V.w + Y.w + 1, A.h);
}

auto WDC65816::instructionImpliedModify8(alu8 op, r16& M) -> void {
L idleIRQ();
  M.l = alu(M.l);
}

auto WDC65816::instructionImpliedModify16(alu16 op, r16& M) -> void {
L idleIRQ();
  M.w = alu(M.w);
}

auto WDC65816::instructionBankModify8(alu8 op) -> void {
  V.l = fetch();
  V.h = fetch();
  W.l = readBank(V.w + 0);
  idle();
  W.l = alu(W.l);
L writeBank(V.w + 0, W.l);
}

auto WDC65816::instructionBankModify16(alu16 op) -> void {
  V.l = fetch();
  V.h = fetch();
  W.l = readBank(V.w + 0);
  W.h = readBank(V.w + 1);
  idle();
  W.w = alu(W.w);
  writeBank(V.w + 1, W.h);
L writeBank(V.w + 0, W.l);
}

auto WDC65816::instructionBankIndexedModify8(alu8 op) -> void {
  V.l = fetch();
  V.h = fetch();
  idle();
  W.l = readBank(V.w + X.w + 0);
  idle();
  W.l = alu(W.l);
L writeBank(V.w + X.w + 0, W.l);
}

auto WDC65816::instructionBankIndexedModify16(alu16 op) -> void {
  V.l = fetch();
  V.h = fetch();
  idle();
  W.l = readBank(V.w + X.w + 0);
  W.h = readBank(V.w + X.w + 1);
  idle();
  W.w = alu(W.w);
  writeBank(V.w + X.w + 1, W.h);
L writeBank(V.w + X.w + 0, W.l);
}

auto WDC65816::instructionDirectModify8(alu8 op) -> void {
  U.l = fetch();
  idle2();
  W.l = readDirect(U.l + 0);
  idle();
  W.l = alu(W.l);
L writeDirect(U.l + 0, W.l);
}

auto WDC65816::instructionDirectModify16(alu16 op) -> void {
  U.l = fetch();
  idle2();
  W.l = readDirect(U.l + 0);
  W.h = readDirect(U.l + 1);
  idle();
  W.w = alu(W.w);
  writeDirect(U.l + 1, W.h);
L writeDirect(U.l + 0, W.l);
}

auto WDC65816::instructionDirectIndexedModify8(alu8 op) -> void {
  U.l = fetch();
  idle2();
  idle();
  W.l = readDirect(U.l + X.w + 0);
  idle();
  W.l = alu(W.l);
L writeDirect(U.l + X.w + 0, W.l);
}

auto WDC65816::instructionDirectIndexedModify16(alu16 op) -> void {
  U.l = fetch();
  idle2();
  idle();
  W.l = readDirect(U.l + X.w + 0);
  W.h = readDirect(U.l + X.w + 1);
  idle();
  W.w = alu(W.w);
  writeDirect(U.l + X.w + 1, W.h);
L writeDirect(U.l + X.w + 0, W.l);
}

auto WDC65816::instructionBranch(bool take) -> void {
  if(!take) {
L   fetch();
  } else {
    U.l = fetch();
    V.w = PC.d + (int8_t)U.l;
    idle6(V.w);
L   idle();
    PC.w = V.w;
    idleBranch();
  }
}

auto WDC65816::instructionBranchLong() -> void {
  U.l = fetch();
  U.h = fetch();
  V.w = PC.d + (int16_t)U.w;
L idle();
  PC.w = V.w;
  idleBranch();
}

auto WDC65816::instructionJumpShort() -> void {
  W.l = fetch();
L W.h = fetch();
  PC.w = W.w;
  idleJump();
}

auto WDC65816::instructionJumpLong() -> void {
  V.l = fetch();
  V.h = fetch();
L V.b = fetch();
  PC.d = V.d;
  idleJump();
}

auto WDC65816::instructionJumpIndirect() -> void {
  V.l = fetch();
  V.h = fetch();
  W.l = read(uint16_t(V.w + 0));
L W.h = read(uint16_t(V.w + 1));
  PC.w = W.w;
  idleJump();
}

auto WDC65816::instructionJumpIndexedIndirect() -> void {
  V.l = fetch();
  V.h = fetch();
  idle();
  W.l = read(PC.b << 16 | uint16_t(V.w + X.w + 0));
L W.h = read(PC.b << 16 | uint16_t(V.w + X.w + 1));
  PC.w = W.w;
  idleJump();
}

auto WDC65816::instructionJumpIndirectLong() -> void {
  U.l = fetch();
  U.h = fetch();
  V.l = read(uint16_t(U.w + 0));
  V.h = read(uint16_t(U.w + 1));
L V.b = read(uint16_t(U.w + 2));
  PC.d = V.d;
  idleJump();
}

auto WDC65816::instructionCallShort() -> void {
  W.l = fetch();
  W.h = fetch();
  idle();
  PC.w--;
  push(PC.h);
L push(PC.l);
  PC.w = W.w;
  idleJump();
}

auto WDC65816::instructionCallLong() -> void {
  V.l = fetch();
  V.h = fetch();
  pushN(PC.b);
  idle();
  V.b = fetch();
  PC.w--;
  pushN(PC.h);
L pushN(PC.l);
  PC.d = V.d;
E S.h = 0x01;
  idleJump();
}

auto WDC65816::instructionCallIndexedIndirect() -> void {
  V.l = fetch();
  pushN(PC.h);
  pushN(PC.l);
  V.h = fetch();
  idle();
  W.l = read(PC.b << 16 | uint16_t(V.w + X.w + 0));
L W.h = read(PC.b << 16 | uint16_t(V.w + X.w + 1));
  PC.w = W.w;
E S.h = 0x01;
  idleJump();
}

auto WDC65816::instructionReturnInterrupt() -> void {
  idle();
  idle();
  P = pull();
E XF = 1, MF = 1;
  if(XF) X.h = 0x00, Y.h = 0x00;
  PC.l = pull();
  if(EF) {
  L PC.h = pull();
  } else {
    PC.h = pull();
  L PC.b = pull();
  }
  idleJump();
}

auto WDC65816::instructionReturnShort() -> void {
  idle();
  idle();
  W.l = pull();
  W.h = pull();
L idle();
  PC.w = W.w;
  PC.w++;
  idleJump();
}

auto WDC65816::instructionReturnLong() -> void {
  idle();
  idle();
  V.l = pullN();
  V.h = pullN();
L V.b = pullN();
  PC.d = V.d;
  PC.w++;
E S.h = 0x01;
  idleJump();
}

auto WDC65816::instructionBitImmediate8() -> void {
L U.l = fetch();
  ZF = (U.l & A.l) == 0;
}

auto WDC65816::instructionBitImmediate16() -> void {
  U.l = fetch();
L U.h = fetch();
  ZF = (U.w & A.w) == 0;
}

auto WDC65816::instructionNoOperation() -> void {
L idleIRQ();
}

auto WDC65816::instructionPrefix() -> void {
L fetch();
}

auto WDC65816::instructionExchangeBA() -> void {
  idle();
L idle();
  A.w = A.w >> 8 | A.w << 8;
  ZF = A.l == 0;
  NF = A.l & 0x80;
}

auto WDC65816::instructionBlockMove8(int adjust) -> void {
  U.b = fetch();
  V.b = fetch();
  B = U.b;
  W.l = read(V.b << 16 | X.w);
  write(U.b << 16 | Y.w, W.l);
  idle();
  X.l += adjust;
  Y.l += adjust;
L idle();
  if(A.w--) PC.w -= 3;
}

auto WDC65816::instructionBlockMove16(int adjust) -> void {
  U.b = fetch();
  V.b = fetch();
  B = U.b;
  W.l = read(V.b << 16 | X.w);
  write(U.b << 16 | Y.w, W.l);
  idle();
  X.w += adjust;
  Y.w += adjust;
L idle();
  if(A.w--) PC.w -= 3;
}

auto WDC65816::instructionInterrupt(r16 vector) -> void {
  fetch();
N push(PC.b);
  push(PC.h);
  push(PC.l);
  push(P);
  IF = 1;
  DF = 0;
  PC.l = read(vector.w + 0);
L PC.h = read(vector.w + 1);
  PC.b = 0x00;
}

auto WDC65816::instructionStop() -> void {
  r.stp = true;
  while(r.stp && !synchronizing()) {
L   idle();
  }
}

auto WDC65816::instructionWait() -> void {
  r.wai = true;
  while(r.wai && !synchronizing()) {
L   idle();
  }
  idle();
}

auto WDC65816::instructionExchangeCE() -> void {
L idleIRQ();
  std::swap(CF, EF);
  if(EF) {
    XF = 1;
    MF = 1;
    X.h = 0x00;
    Y.h = 0x00;
    S.h = 0x01;
  }
}

auto WDC65816::instructionSetFlag(bool& flag) -> void {
L idleIRQ();
  flag = 1;
}

auto WDC65816::instructionClearFlag(bool& flag) -> void {
L idleIRQ();
  flag = 0;
}

auto WDC65816::instructionResetP() -> void {
  W.l = fetch();
L idle();
  P = P & ~W.l;
E XF = 1, MF = 1;
  if(XF) X.h = 0x00, Y.h = 0x00;
}

auto WDC65816::instructionSetP() -> void {
  W.l = fetch();
L idle();
  P = P | W.l;
E XF = 1, MF = 1;
  if(XF) X.h = 0x00, Y.h = 0x00;
}

auto WDC65816::instructionTransfer8(r16 F, r16& T) -> void {
L idleIRQ();
  T.l = F.l;
  ZF = T.l == 0;
  NF = T.l & 0x80;
}

auto WDC65816::instructionTransfer16(r16 F, r16& T) -> void {
L idleIRQ();
  T.w = F.w;
  ZF = T.w == 0;
  NF = T.w & 0x8000;
}

auto WDC65816::instructionTransferCS() -> void {
L idleIRQ();
  S.w = A.w;
E S.h = 0x01;
}

auto WDC65816::instructionTransferSX8() -> void {
L idleIRQ();
  X.l = S.l;
  ZF = X.l == 0;
  NF = X.l & 0x80;
}

auto WDC65816::instructionTransferSX16() -> void {
L idleIRQ();
  X.w = S.w;
  ZF = X.w == 0;
  NF = X.w & 0x8000;
}

auto WDC65816::instructionTransferXS() -> void {
L idleIRQ();
E S.l = X.l;
N S.w = X.w;
}

auto WDC65816::instructionPush8(r16 F) -> void {
  idle();
L push(F.l);
}

auto WDC65816::instructionPush16(r16 F) -> void {
  idle();
  push(F.h);
L push(F.l);
}

auto WDC65816::instructionPushD() -> void {
  idle();
  pushN(D.h);
L pushN(D.l);
E S.h = 0x01;
}

auto WDC65816::instructionPull8(r16& T) -> void {
  idle();
  idle();
L T.l = pull();
  ZF = T.l == 0;
  NF = T.l & 0x80;
}

auto WDC65816::instructionPull16(r16& T) -> void {
  idle();
  idle();
  T.l = pull();
L T.h = pull();
  ZF = T.w == 0;
  NF = T.w & 0x8000;
}

auto WDC65816::instructionPullD() -> void {
  idle();
  idle();
  D.l = pullN();
L D.h = pullN();
  ZF = D.w == 0;
  NF = D.w & 0x8000;
E S.h = 0x01;
}

auto WDC65816::instructionPullB() -> void {
  idle();
  idle();
L B = pull();
  ZF = B == 0;
  NF = B & 0x80;
}

auto WDC65816::instructionPullP() -> void {
  idle();
  idle();
L P = pull();
E XF = 1, MF = 1;
  if(XF) X.h = 0x00, Y.h = 0x00;
}

auto WDC65816::instructionPushEffectiveAddress() -> void {
  W.l = fetch();
  W.h = fetch();
  pushN(W.h);
L pushN(W.l);
E S.h = 0x01;
}

auto WDC65816::instructionPushEffectiveIndirectAddress() -> void {
  U.l = fetch();
  idle2();
  W.l = readDirectN(U.l + 0);
  W.h = readDirectN(U.l + 1);
  pushN(W.h);
L pushN(W.l);
E S.h = 0x01;
}

auto WDC65816::instructionPushEffectiveRelativeAddress() -> void {
  V.l = fetch();
  V.h = fetch();
  idle();
  W.w = PC.d + (int16_t)V.w;
  pushN(W.h);
L pushN(W.l);
E S.h = 0x01;
}

auto WDC65816::interrupt() -> void {
  read(PC.d);
  idle();
N push(PC.b);
  push(PC.h);
  push(PC.l);
  push(EF ? P & ~0x10 : P);
  IF = 1;
  DF = 0;
  PC.l = read(r.vector + 0);
L PC.h = read(r.vector + 1);
  PC.b = 0x00;
  idleJump();
}

//both the accumulator and index registers can independently be in either 8-bit or 16-bit mode.
//controlled via the M/X flags, this changes the execution details of various instructions.
//rather than implement four instruction tables for all possible combinations of these bits,
//instead use macro abuse to generate all four tables based off of a single template table.
auto WDC65816::instruction() -> void {
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

auto WDC65816::power() -> void {
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

auto WDC65816::serialize(serializer& s) -> void {
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

auto WDC65816::disassemble() -> string {
  return disassemble(r.pc.d, r.e, r.p.m, r.p.x);
}

auto WDC65816::disassemble(nall::Natural<24> address, bool e, bool m, bool x) -> string {
  string s;

  nall::Natural<24> pc = address;
  s = {hex(pc, 6), "  "};

  string name;
  string operand;
  maybe<nall::Natural<24>> effective;

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

  auto absolute = [&]() -> string {
    effective = r.b << 16 | operandWord;
    return {"$", hex(operandWord, 4L)};
  };

  auto absolutePC = [&]() -> string {
    effective = pc & 0xff0000 | operandWord;
    return {"$", hex(operandWord, 4L)};
  };

  auto absoluteX = [&]() -> string {
    effective = (r.b << 16) + operandWord + r.x.w;
    return {"$", hex(operandWord, 4L), ",x"};
  };

  auto absoluteY = [&]() -> string {
    effective = (r.b << 16) + operandWord + r.y.w;
    return {"$", hex(operandWord, 4L), ",y"};
  };

  auto absoluteLong = [&]() -> string {
    effective = operandLong;
    return {"$", hex(operandLong, 6L)};
  };

  auto absoluteLongX = [&]() -> string {
    effective = operandLong + r.x.w;
    return {"$", hex(operandLong, 6L), ",x"};
  };

  auto direct = [&]() -> string {
    effective = uint16_t(r.d.w + operandByte);
    return {"$", hex(operandByte, 2L)};
  };

  auto directX = [&]() -> string {
    effective = uint16_t(r.d.w + operandByte + r.x.w);
    return {"$", hex(operandByte, 2L), ",x"};
  };

  auto directY = [&]() -> string {
    effective = uint16_t(r.d.w + operandByte + r.y.w);
    return {"$", hex(operandByte, 2L), ",y"};
  };

  auto immediate = [&]() -> string {
    return {"#$", hex(operandByte, 2L)};
  };

  auto immediateA = [&]() -> string {
    return {"#$", m ? hex(operandByte, 2L) : hex(operandWord, 4L)};
  };

  auto immediateX = [&]() -> string {
    return {"#$", x ? hex(operandByte, 2L) : hex(operandWord, 4L)};
  };

  auto implied = [&]() -> string {
    return {};
  };

  auto indexedIndirectX = [&]() -> string {
    effective = uint16_t(r.d.w + operandByte + r.x.w);
    effective = r.b << 16 | readWord(*effective);
    return {"($", hex(operandByte, 2L), ",x)"};
  };

  auto indirect = [&]() -> string {
    effective = uint16_t(r.d.w + operandByte);
    effective = (r.b << 16) + readWord(*effective);
    return {"($", hex(operandByte, 2L), ")"};
  };

  auto indirectPC = [&]() -> string {
    effective = operandWord;
    effective = pc & 0xff0000 | readWord(*effective);
    return {"($", hex(operandWord, 4L), ")"};
  };

  auto indirectX = [&]() -> string {
    effective = operandWord;
    effective = pc & 0xff0000 | uint16_t(*effective + r.x.w);
    effective = pc & 0xff0000 | readWord(*effective);
    return {"($", hex(operandWord, 4L), ",x)"};
  };

  auto indirectIndexedY = [&]() -> string {
    effective = uint16_t(r.d.w + operandByte);
    effective = (r.b << 16) + readWord(*effective) + r.y.w;
    return {"($", hex(operandByte, 2L), "),y"};
  };

  auto indirectLong = [&]() -> string {
    effective = uint16_t(r.d.w + operandByte);
    effective = readLong(*effective);
    return {"[$", hex(operandByte, 2L), "]"};
  };

  auto indirectLongPC = [&]() -> string {
    effective = readLong(operandWord);
    return {"[$", hex(operandWord, 4L), "]"};
  };

  auto indirectLongY = [&]() -> string {
    effective = uint16_t(r.d.w + operandByte);
    effective = readLong(*effective) + r.y.w;
    return {"[$", hex(operandByte, 2L), "],y"};
  };

  auto move = [&]() -> string {
    return {"$", hex(operand0, 2L), "=$", hex(operand1, 2L)};
  };

  auto relative = [&]() -> string {
    effective = pc & 0xff0000 | uint16_t(pc + 2 + (int8_t)operandByte);
    return {"$", hex(*effective, 4L)};
  };

  auto relativeWord = [&]() -> string {
    effective = pc & 0xff0000 | uint16_t(pc + 3 + (int16_t)operandWord);
    return {"$", hex(*effective, 4L)};
  };

  auto stack = [&]() -> string {
    effective = uint16_t(r.s.w + operandByte);
    return {"$", hex(operandByte, 2L), ",s"};
  };

  auto stackIndirect = [&]() -> string {
    effective = uint16_t(operandByte + r.s.w);
    effective = (r.b << 16) + readWord(*effective) + r.y.w;
    return {"($", hex(operandByte, 2L), ",s),y"};
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
  if(effective) s.append("[", hex(*effective, 6L), "]");
  while(s.size() < 31) s.append(" ");

  s.append(" A:", hex(r.a.w, 4L));
  s.append(" X:", hex(r.x.w, 4L));
  s.append(" Y:", hex(r.y.w, 4L));
  s.append(" S:", hex(r.s.w, 4L));
  s.append(" D:", hex(r.d.w, 4L));
  s.append(" B:", hex(r.b  , 2L));

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
}

}
