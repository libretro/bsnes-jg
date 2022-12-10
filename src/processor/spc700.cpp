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

#include "serializer.hpp"

#include "spc700.hpp"

namespace Processor {

#define PC r.pc.w
#define YA r.ya.w
#define A r.ya.byte.l
#define X r.x
#define Y r.ya.byte.h
#define S r.s
#define P r.p

#define CF r.p.c
#define ZF r.p.z
#define IF r.p.i
#define HF r.p.h
#define BF r.p.b
#define PF r.p.p
#define VF r.p.v
#define NF r.p.n

#define alu (this->*op)

SPC700::~SPC700() {}

uint8_t SPC700::fetch() {
  return read(PC++);
}

uint8_t SPC700::load(uint8_t address) {
  return read(PF << 8 | address);
}

void SPC700::store(uint8_t address, uint8_t data) {
  return write(PF << 8 | address, data);
}

uint8_t SPC700::pull() {
  return read(1 << 8 | ++S);
}

void SPC700::push(uint8_t data) {
  return write(1 << 8 | S--, data);
}

uint8_t SPC700::algorithmADC(uint8_t x, uint8_t y) {
  int z = x + y + CF;
  CF = z > 0xff;
  ZF = (uint8_t)z == 0;
  HF = (x ^ y ^ z) & 0x10;
  VF = ~(x ^ y) & (x ^ z) & 0x80;
  NF = z & 0x80;
  return z;
}

uint8_t SPC700::algorithmAND(uint8_t x, uint8_t y) {
  x &= y;
  ZF = x == 0;
  NF = x & 0x80;
  return x;
}

uint8_t SPC700::algorithmASL(uint8_t x) {
  CF = x & 0x80;
  x <<= 1;
  ZF = x == 0;
  NF = x & 0x80;
  return x;
}

uint8_t SPC700::algorithmCMP(uint8_t x, uint8_t y) {
  int z = x - y;
  CF = z >= 0;
  ZF = (uint8_t)z == 0;
  NF = z & 0x80;
  return x;
}

uint8_t SPC700::algorithmDEC(uint8_t x) {
  x--;
  ZF = x == 0;
  NF = x & 0x80;
  return x;
}

uint8_t SPC700::algorithmEOR(uint8_t x, uint8_t y) {
  x ^= y;
  ZF = x == 0;
  NF = x & 0x80;
  return x;
}

uint8_t SPC700::algorithmINC(uint8_t x) {
  x++;
  ZF = x == 0;
  NF = x & 0x80;
  return x;
}

uint8_t SPC700::algorithmLD(uint8_t x, uint8_t y) {
  if (x) {}
  ZF = y == 0;
  NF = y & 0x80;
  return y;
}

uint8_t SPC700::algorithmLSR(uint8_t x) {
  CF = x & 0x01;
  x >>= 1;
  ZF = x == 0;
  NF = x & 0x80;
  return x;
}

uint8_t SPC700::algorithmOR(uint8_t x, uint8_t y) {
  x |= y;
  ZF = x == 0;
  NF = x & 0x80;
  return x;
}

uint8_t SPC700::algorithmROL(uint8_t x) {
  bool carry = CF;
  CF = x & 0x80;
  x = x << 1 | carry;
  ZF = x == 0;
  NF = x & 0x80;
  return x;
}

uint8_t SPC700::algorithmROR(uint8_t x) {
  bool carry = CF;
  CF = x & 0x01;
  x = carry << 7 | x >> 1;
  ZF = x == 0;
  NF = x & 0x80;
  return x;
}

uint8_t SPC700::algorithmSBC(uint8_t x, uint8_t y) {
  return algorithmADC(x, ~y);
}

uint16_t SPC700::algorithmADW(uint16_t x, uint16_t y) {
  uint16_t z;
  CF = 0;
  z  = algorithmADC(x, y);
  z |= algorithmADC(x >> 8, y >> 8) << 8;
  ZF = z == 0;
  return z;
}

uint16_t SPC700::algorithmCPW(uint16_t x, uint16_t y) {
  int z = x - y;
  CF = z >= 0;
  ZF = (uint16_t)z == 0;
  NF = z & 0x8000;
  return x;
}

uint16_t SPC700::algorithmLDW(uint16_t x, uint16_t y) {
  if (x) {}
  ZF = y == 0;
  NF = y & 0x8000;
  return y;
}

uint16_t SPC700::algorithmSBW(uint16_t x, uint16_t y) {
  uint16_t z;
  CF = 1;
  z  = algorithmSBC(x, y);
  z |= algorithmSBC(x >> 8, y >> 8) << 8;
  ZF = z == 0;
  return z;
}

void SPC700::instructionAbsoluteBitModify(uint8_t mode) {
  mode &= 0x07;
  uint16_t address = fetch();
  address |= fetch() << 8;
  uint8_t bit = address >> 13;
  address &= 0x1fff;
  uint8_t data = read(address);
  switch(mode) {
  case 0:  //or addr:bit
    idle();
    CF |= bool(data & 1 << bit);
    break;
  case 1:  //or !addr:bit
    idle();
    CF |= !bool(data & 1 << bit);
    break;
  case 2:  //and addr:bit
    CF &= bool(data & 1 << bit);
    break;
  case 3:  //and !addr:bit
    CF &= !bool(data & 1 << bit);
    break;
  case 4:  //eor addr:bit
    idle();
    CF ^= bool(data & 1 << bit);
    break;
  case 5:  //ld addr:bit
    CF = bool(data & 1 << bit);
    break;
  case 6:  //st addr:bit
    idle();
    data &= ~(1 << bit);
    data |= CF << bit;
    write(address, data);
    break;
  case 7:  //not addr:bit
    data ^= 1 << bit;
    write(address, data);
    break;
  }
}

void SPC700::instructionAbsoluteBitSet(uint8_t bit, bool value) {
  bit &= 0x07;
  uint8_t address = fetch();
  uint8_t data = load(address);
  data &= ~(1 << bit);
  data |= value << bit;
  store(address, data);
}

void SPC700::instructionAbsoluteRead(fpb op, uint8_t &target) {
  uint16_t address = fetch();
  address |= fetch() << 8;
  uint8_t data = read(address);
  target = alu(target, data);
}

void SPC700::instructionAbsoluteModify(fps op) {
  uint16_t address = fetch();
  address |= fetch() << 8;
  uint8_t data = read(address);
  write(address, alu(data));
}

void SPC700::instructionAbsoluteWrite(uint8_t &data) {
  uint16_t address = fetch();
  address |= fetch() << 8;
  read(address);
  write(address, data);
}

void SPC700::instructionAbsoluteIndexedRead(fpb op, uint8_t &index) {
  uint16_t address = fetch();
  address |= fetch() << 8;
  idle();
  uint8_t data = read(address + index);
  A = alu(A, data);
}

void SPC700::instructionAbsoluteIndexedWrite(uint8_t &index) {
  uint16_t address = fetch();
  address |= fetch() << 8;
  idle();
  read(address + index);
  write(address + index, A);
}

void SPC700::instructionBranch(bool take) {
  uint8_t data = fetch();
  if(!take) return;
  idle();
  idle();
  PC += (int8_t)data;
}

void SPC700::instructionBranchBit(uint8_t bit, bool match) {
  bit &= 0x07;
  uint8_t address = fetch();
  uint8_t data = load(address);
  idle();
  uint8_t displacement = fetch();
  if(bool(data & 1 << bit) != match) return;
  idle();
  idle();
  PC += (int8_t)displacement;
}

void SPC700::instructionBranchNotDirect() {
  uint8_t address = fetch();
  uint8_t data = load(address);
  idle();
  uint8_t displacement = fetch();
  if(A == data) return;
  idle();
  idle();
  PC += (int8_t)displacement;
}

void SPC700::instructionBranchNotDirectDecrement() {
  uint8_t address = fetch();
  uint8_t data = load(address);
  store(address, --data);
  uint8_t displacement = fetch();
  if(data == 0) return;
  idle();
  idle();
  PC += (int8_t)displacement;
}

void SPC700::instructionBranchNotDirectIndexed(uint8_t &index) {
  uint8_t address = fetch();
  idle();
  uint8_t data = load(address + index);
  idle();
  uint8_t displacement = fetch();
  if(A == data) return;
  idle();
  idle();
  PC += (int8_t)displacement;
}

void SPC700::instructionBranchNotYDecrement() {
  read(PC);
  idle();
  uint8_t displacement = fetch();
  if(--Y == 0) return;
  idle();
  idle();
  PC += (int8_t)displacement;
}

void SPC700::instructionBreak() {
  read(PC);
  push(PC >> 8);
  push(PC >> 0);
  push(P);
  idle();
  uint16_t address = read(0xffde + 0);
  address |= read(0xffde + 1) << 8;
  PC = address;
  IF = 0;
  BF = 1;
}

void SPC700::instructionCallAbsolute() {
  uint16_t address = fetch();
  address |= fetch() << 8;
  idle();
  push(PC >> 8);
  push(PC >> 0);
  idle();
  idle();
  PC = address;
}

void SPC700::instructionCallPage() {
  uint8_t address = fetch();
  idle();
  push(PC >> 8);
  push(PC >> 0);
  idle();
  PC = 0xff00 | address;
}

void SPC700::instructionCallTable(uint8_t vector) {
  vector &= 0x0f;
  read(PC);
  idle();
  push(PC >> 8);
  push(PC >> 0);
  idle();
  uint16_t address = 0xffde - (vector << 1);
  uint16_t pc = read(address + 0);
  pc |= read(address + 1) << 8;
  PC = pc;
}

void SPC700::instructionComplementCarry() {
  read(PC);
  idle();
  CF = !CF;
}

void SPC700::instructionDecimalAdjustAdd() {
  read(PC);
  idle();
  if(CF || A > 0x99) {
    A += 0x60;
    CF = 1;
  }
  if(HF || (A & 15) > 0x09) {
    A += 0x06;
  }
  ZF = A == 0;
  NF = A & 0x80;
}

void SPC700::instructionDecimalAdjustSub() {
  read(PC);
  idle();
  if(!CF || A > 0x99) {
    A -= 0x60;
    CF = 0;
  }
  if(!HF || (A & 15) > 0x09) {
    A -= 0x06;
  }
  ZF = A == 0;
  NF = A & 0x80;
}

void SPC700::instructionDirectRead(fpb op, uint8_t &target) {
  uint8_t address = fetch();
  uint8_t data = load(address);
  target = alu(target, data);
}

void SPC700::instructionDirectModify(fps op) {
  uint8_t address = fetch();
  uint8_t data = load(address);
  store(address, alu(data));
}

void SPC700::instructionDirectWrite(uint8_t &data) {
  uint8_t address = fetch();
  load(address);
  store(address, data);
}

void SPC700::instructionDirectDirectCompare(fpb op) {
  uint8_t source = fetch();
  uint8_t rhs = load(source);
  uint8_t target = fetch();
  uint8_t lhs = load(target);
  lhs = alu(lhs, rhs);
  idle();
}

void SPC700::instructionDirectDirectModify(fpb op) {
  uint8_t source = fetch();
  uint8_t rhs = load(source);
  uint8_t target = fetch();
  uint8_t lhs = load(target);
  lhs = alu(lhs, rhs);
  store(target, lhs);
}

void SPC700::instructionDirectDirectWrite() {
  uint8_t source = fetch();
  uint8_t data = load(source);
  uint8_t target = fetch();
  store(target, data);
}

void SPC700::instructionDirectImmediateCompare(fpb op) {
  uint8_t immediate = fetch();
  uint8_t address = fetch();
  uint8_t data = load(address);
  data = alu(data, immediate);
  idle();
}

void SPC700::instructionDirectImmediateModify(fpb op) {
  uint8_t immediate = fetch();
  uint8_t address = fetch();
  uint8_t data = load(address);
  data = alu(data, immediate);
  store(address, data);
}

void SPC700::instructionDirectImmediateWrite() {
  uint8_t immediate = fetch();
  uint8_t address = fetch();
  load(address);
  store(address, immediate);
}

void SPC700::instructionDirectCompareWord(fpw op) {
  uint8_t address = fetch();
  uint16_t data = load(address + 0);
  data |= load(address + 1) << 8;
  YA = alu(YA, data);
}

void SPC700::instructionDirectReadWord(fpw op) {
  uint8_t address = fetch();
  uint16_t data = load(address + 0);
  idle();
  data |= load(address + 1) << 8;
  YA = alu(YA, data);
}

void SPC700::instructionDirectModifyWord(int adjust) {
  uint8_t address = fetch();
  uint16_t data = load(address + 0) + adjust;
  store(address + 0, data >> 0);
  data += load(address + 1) << 8;
  store(address + 1, data >> 8);
  ZF = data == 0;
  NF = data & 0x8000;
}

void SPC700::instructionDirectWriteWord() {
  uint8_t address = fetch();
  load(address + 0);
  store(address + 0, A);
  store(address + 1, Y);
}

void SPC700::instructionDirectIndexedRead(fpb op, uint8_t &target, uint8_t &index) {
  uint8_t address = fetch();
  idle();
  uint8_t data = load(address + index);
  target = alu(target, data);
}

void SPC700::instructionDirectIndexedModify(fps op, uint8_t &index) {
  uint8_t address = fetch();
  idle();
  uint8_t data = load(address + index);
  store(address + index, alu(data));
}

void SPC700::instructionDirectIndexedWrite(uint8_t &data, uint8_t &index) {
  uint8_t address = fetch();
  idle();
  load(address + index);
  store(address + index, data);
}

void SPC700::instructionDivide() {
  read(PC);
  idle();
  idle();
  idle();
  idle();
  idle();
  idle();
  idle();
  idle();
  idle();
  idle();
  uint16_t ya = YA;
  //overflow set if quotient >= 256
  HF = (Y & 15) >= (X & 15);
  VF = Y >= X;
  if(Y < (X << 1)) {
    //if quotient is <= 511 (will fit into 9-bit result)
    A = ya / X;
    Y = ya % X;
  } else {
    //otherwise, the quotient won't fit into VF + A
    //this emulates the odd behavior of the S-SMP in this case
    A = 255 - (ya - (X << 9)) / (256 - X);
    Y = X   + (ya - (X << 9)) % (256 - X);
  }
  //result is set based on a (quotient) only
  ZF = A == 0;
  NF = A & 0x80;
}

void SPC700::instructionExchangeNibble() {
  read(PC);
  idle();
  idle();
  idle();
  A = A >> 4 | A << 4;
  ZF = A == 0;
  NF = A & 0x80;
}

void SPC700::instructionFlagSet(bool& flag, bool value) {
  read(PC);
  if(&flag == &IF) idle();
  flag = value;
}

void SPC700::instructionImmediateRead(fpb op, uint8_t &target) {
  uint8_t data = fetch();
  target = alu(target, data);
}

void SPC700::instructionImpliedModify(fps op, uint8_t &target) {
  read(PC);
  target = alu(target);
}

void SPC700::instructionIndexedIndirectRead(fpb op, uint8_t &index) {
  uint8_t indirect = fetch();
  idle();
  uint16_t address = load(indirect + index + 0);
  address |= load(indirect + index + 1) << 8;
  uint8_t data = read(address);
  A = alu(A, data);
}

void SPC700::instructionIndexedIndirectWrite(uint8_t &data, uint8_t &index) {
  uint8_t indirect = fetch();
  idle();
  uint16_t address = load(indirect + index + 0);
  address |= load(indirect + index + 1) << 8;
  read(address);
  write(address, data);
}

void SPC700::instructionIndirectIndexedRead(fpb op, uint8_t &index) {
  uint8_t indirect = fetch();
  uint16_t address = load(indirect + 0);
  address |= load(indirect + 1) << 8;
  idle();
  uint8_t data = read(address + index);
  A = alu(A, data);
}

void SPC700::instructionIndirectIndexedWrite(uint8_t &data, uint8_t &index) {
  uint8_t indirect = fetch();
  uint16_t address = load(indirect + 0);
  address |= load(indirect + 1) << 8;
  idle();
  read(address + index);
  write(address + index, data);
}

void SPC700::instructionIndirectXRead(fpb op) {
  read(PC);
  uint8_t data = load(X);
  A = alu(A, data);
}

void SPC700::instructionIndirectXWrite(uint8_t &data) {
  read(PC);
  load(X);
  store(X, data);
}

void SPC700::instructionIndirectXIncrementRead(uint8_t &data) {
  read(PC);
  data = load(X++);
  idle();  //quirk: consumes extra idle cycle compared to most read instructions
  ZF = data == 0;
  NF = data & 0x80;
}

void SPC700::instructionIndirectXIncrementWrite(uint8_t &data) {
  read(PC);
  idle();  //quirk: not a read cycle as with most write instructions
  store(X++, data);
}

void SPC700::instructionIndirectXCompareIndirectY(fpb op) {
  read(PC);
  uint8_t rhs = load(Y);
  uint8_t lhs = load(X);
  lhs = alu(lhs, rhs);
  idle();
}

void SPC700::instructionIndirectXWriteIndirectY(fpb op) {
  read(PC);
  uint8_t rhs = load(Y);
  uint8_t lhs = load(X);
  lhs = alu(lhs, rhs);
  store(X, lhs);
}

void SPC700::instructionJumpAbsolute() {
  uint16_t address = fetch();
  address |= fetch() << 8;
  PC = address;
}

void SPC700::instructionJumpIndirectX() {
  uint16_t address = fetch();
  address |= fetch() << 8;
  idle();
  uint16_t pc = read(address + X + 0);
  pc |= read(address + X + 1) << 8;
  PC = pc;
}

void SPC700::instructionMultiply() {
  read(PC);
  idle();
  idle();
  idle();
  idle();
  idle();
  idle();
  idle();
  uint16_t ya = Y * A;
  A = ya >> 0;
  Y = ya >> 8;
  //result is set based on y (high-byte) only
  ZF = Y == 0;
  NF = Y & 0x80;
}

void SPC700::instructionNoOperation() {
  read(PC);
}

void SPC700::instructionOverflowClear() {
  read(PC);
  HF = 0;
  VF = 0;
}

void SPC700::instructionPull(uint8_t &data) {
  read(PC);
  idle();
  data = pull();
}

void SPC700::instructionPullP() {
  read(PC);
  idle();
  P = pull();
}

void SPC700::instructionPush(uint8_t data) {
  read(PC);
  push(data);
  idle();
}

void SPC700::instructionReturnInterrupt() {
  read(PC);
  idle();
  P = pull();
  uint16_t address = pull();
  address |= pull() << 8;
  PC = address;
}

void SPC700::instructionReturnSubroutine() {
  read(PC);
  idle();
  uint16_t address = pull();
  address |= pull() << 8;
  PC = address;
}

void SPC700::instructionStop() {
  r.stop = true;
  while(r.stop && !synchronizing()) {
    read(PC);
    idle();
  }
}

void SPC700::instructionTestSetBitsAbsolute(bool set) {
  uint16_t address = fetch();
  address |= fetch() << 8;
  uint8_t data = read(address);
  ZF = (A - data) == 0;
  NF = (A - data) & 0x80;
  read(address);
  write(address, set ? data | A : data & ~A);
}

void SPC700::instructionTransfer(uint8_t &from, uint8_t &to) {
  read(PC);
  to = from;
  if(&to == &S) return;
  ZF = to == 0;
  NF = to & 0x80;
}

void SPC700::instructionWait() {
  r.wait = true;
  while(r.wait && !synchronizing()) {
    read(PC);
    idle();
  }
}

void SPC700::instruction() {
  switch(fetch()) {
    case 0x00: return instructionNoOperation();
    case 0x01: return instructionCallTable(0);
    case 0x02: return instructionAbsoluteBitSet(0, true);
    case 0x03: return instructionBranchBit(0, true);
    case 0x04: return instructionDirectRead(&SPC700::algorithmOR, A);
    case 0x05: return instructionAbsoluteRead(&SPC700::algorithmOR, A);
    case 0x06: return instructionIndirectXRead(&SPC700::algorithmOR);
    case 0x07: return instructionIndexedIndirectRead(&SPC700::algorithmOR, X);
    case 0x08: return instructionImmediateRead(&SPC700::algorithmOR, A);
    case 0x09: return instructionDirectDirectModify(&SPC700::algorithmOR);
    case 0x0a: return instructionAbsoluteBitModify(0);
    case 0x0b: return instructionDirectModify(&SPC700::algorithmASL);
    case 0x0c: return instructionAbsoluteModify(&SPC700::algorithmASL);
    case 0x0d: return instructionPush(P);
    case 0x0e: return instructionTestSetBitsAbsolute(true);
    case 0x0f: return instructionBreak();
    case 0x10: return instructionBranch(NF == 0);
    case 0x11: return instructionCallTable(1);
    case 0x12: return instructionAbsoluteBitSet(0, false);
    case 0x13: return instructionBranchBit(0, false);
    case 0x14: return instructionDirectIndexedRead(&SPC700::algorithmOR, A, X);
    case 0x15: return instructionAbsoluteIndexedRead(&SPC700::algorithmOR, X);
    case 0x16: return instructionAbsoluteIndexedRead(&SPC700::algorithmOR, Y);
    case 0x17: return instructionIndirectIndexedRead(&SPC700::algorithmOR, Y);
    case 0x18: return instructionDirectImmediateModify(&SPC700::algorithmOR);
    case 0x19: return instructionIndirectXWriteIndirectY(&SPC700::algorithmOR);
    case 0x1a: return instructionDirectModifyWord(-1);
    case 0x1b: return instructionDirectIndexedModify(&SPC700::algorithmASL, X);
    case 0x1c: return instructionImpliedModify(&SPC700::algorithmASL, A);
    case 0x1d: return instructionImpliedModify(&SPC700::algorithmDEC, X);
    case 0x1e: return instructionAbsoluteRead(&SPC700::algorithmCMP, X);
    case 0x1f: return instructionJumpIndirectX();
    case 0x20: return instructionFlagSet(PF, false);
    case 0x21: return instructionCallTable(2);
    case 0x22: return instructionAbsoluteBitSet(1, true);
    case 0x23: return instructionBranchBit(1, true);
    case 0x24: return instructionDirectRead(&SPC700::algorithmAND, A);
    case 0x25: return instructionAbsoluteRead(&SPC700::algorithmAND, A);
    case 0x26: return instructionIndirectXRead(&SPC700::algorithmAND);
    case 0x27: return instructionIndexedIndirectRead(&SPC700::algorithmAND, X);
    case 0x28: return instructionImmediateRead(&SPC700::algorithmAND, A);
    case 0x29: return instructionDirectDirectModify(&SPC700::algorithmAND);
    case 0x2a: return instructionAbsoluteBitModify(1);
    case 0x2b: return instructionDirectModify(&SPC700::algorithmROL);
    case 0x2c: return instructionAbsoluteModify(&SPC700::algorithmROL);
    case 0x2d: return instructionPush(A);
    case 0x2e: return instructionBranchNotDirect();
    case 0x2f: return instructionBranch(true);
    case 0x30: return instructionBranch(NF == 1);
    case 0x31: return instructionCallTable(3);
    case 0x32: return instructionAbsoluteBitSet(1, false);
    case 0x33: return instructionBranchBit(1, false);
    case 0x34: return instructionDirectIndexedRead(&SPC700::algorithmAND, A, X);
    case 0x35: return instructionAbsoluteIndexedRead(&SPC700::algorithmAND, X);
    case 0x36: return instructionAbsoluteIndexedRead(&SPC700::algorithmAND, Y);
    case 0x37: return instructionIndirectIndexedRead(&SPC700::algorithmAND, Y);
    case 0x38: return instructionDirectImmediateModify(&SPC700::algorithmAND);
    case 0x39: return instructionIndirectXWriteIndirectY(&SPC700::algorithmAND);
    case 0x3a: return instructionDirectModifyWord(+1);
    case 0x3b: return instructionDirectIndexedModify(&SPC700::algorithmROL, X);
    case 0x3c: return instructionImpliedModify(&SPC700::algorithmROL, A);
    case 0x3d: return instructionImpliedModify(&SPC700::algorithmINC, X);
    case 0x3e: return instructionDirectRead(&SPC700::algorithmCMP, X);
    case 0x3f: return instructionCallAbsolute();
    case 0x40: return instructionFlagSet(PF, true);
    case 0x41: return instructionCallTable(4);
    case 0x42: return instructionAbsoluteBitSet(2, true);
    case 0x43: return instructionBranchBit(2, true);
    case 0x44: return instructionDirectRead(&SPC700::algorithmEOR, A);
    case 0x45: return instructionAbsoluteRead(&SPC700::algorithmEOR, A);
    case 0x46: return instructionIndirectXRead(&SPC700::algorithmEOR);
    case 0x47: return instructionIndexedIndirectRead(&SPC700::algorithmEOR, X);
    case 0x48: return instructionImmediateRead(&SPC700::algorithmEOR, A);
    case 0x49: return instructionDirectDirectModify(&SPC700::algorithmEOR);
    case 0x4a: return instructionAbsoluteBitModify(2);
    case 0x4b: return instructionDirectModify(&SPC700::algorithmLSR);
    case 0x4c: return instructionAbsoluteModify(&SPC700::algorithmLSR);
    case 0x4d: return instructionPush(X);
    case 0x4e: return instructionTestSetBitsAbsolute(false);
    case 0x4f: return instructionCallPage();
    case 0x50: return instructionBranch(VF == 0);
    case 0x51: return instructionCallTable(5);
    case 0x52: return instructionAbsoluteBitSet(2, false);
    case 0x53: return instructionBranchBit(2, false);
    case 0x54: return instructionDirectIndexedRead(&SPC700::algorithmEOR, A, X);
    case 0x55: return instructionAbsoluteIndexedRead(&SPC700::algorithmEOR, X);
    case 0x56: return instructionAbsoluteIndexedRead(&SPC700::algorithmEOR, Y);
    case 0x57: return instructionIndirectIndexedRead(&SPC700::algorithmEOR, Y);
    case 0x58: return instructionDirectImmediateModify(&SPC700::algorithmEOR);
    case 0x59: return instructionIndirectXWriteIndirectY(&SPC700::algorithmEOR);
    case 0x5a: return instructionDirectCompareWord(&SPC700::algorithmCPW);
    case 0x5b: return instructionDirectIndexedModify(&SPC700::algorithmLSR, X);
    case 0x5c: return instructionImpliedModify(&SPC700::algorithmLSR, A);
    case 0x5d: return instructionTransfer(A, X);
    case 0x5e: return instructionAbsoluteRead(&SPC700::algorithmCMP, Y);
    case 0x5f: return instructionJumpAbsolute();
    case 0x60: return instructionFlagSet(CF, false);
    case 0x61: return instructionCallTable(6);
    case 0x62: return instructionAbsoluteBitSet(3, true);
    case 0x63: return instructionBranchBit(3, true);
    case 0x64: return instructionDirectRead(&SPC700::algorithmCMP, A);
    case 0x65: return instructionAbsoluteRead(&SPC700::algorithmCMP, A);
    case 0x66: return instructionIndirectXRead(&SPC700::algorithmCMP);
    case 0x67: return instructionIndexedIndirectRead(&SPC700::algorithmCMP, X);
    case 0x68: return instructionImmediateRead(&SPC700::algorithmCMP, A);
    case 0x69: return instructionDirectDirectCompare(&SPC700::algorithmCMP);
    case 0x6a: return instructionAbsoluteBitModify(3);
    case 0x6b: return instructionDirectModify(&SPC700::algorithmROR);
    case 0x6c: return instructionAbsoluteModify(&SPC700::algorithmROR);
    case 0x6d: return instructionPush(Y);
    case 0x6e: return instructionBranchNotDirectDecrement();
    case 0x6f: return instructionReturnSubroutine();
    case 0x70: return instructionBranch(VF == 1);
    case 0x71: return instructionCallTable(7);
    case 0x72: return instructionAbsoluteBitSet(3, false);
    case 0x73: return instructionBranchBit(3, false);
    case 0x74: return instructionDirectIndexedRead(&SPC700::algorithmCMP, A, X);
    case 0x75: return instructionAbsoluteIndexedRead(&SPC700::algorithmCMP, X);
    case 0x76: return instructionAbsoluteIndexedRead(&SPC700::algorithmCMP, Y);
    case 0x77: return instructionIndirectIndexedRead(&SPC700::algorithmCMP, Y);
    case 0x78: return instructionDirectImmediateCompare(&SPC700::algorithmCMP);
    case 0x79: return instructionIndirectXCompareIndirectY(&SPC700::algorithmCMP);
    case 0x7a: return instructionDirectReadWord(&SPC700::algorithmADW);
    case 0x7b: return instructionDirectIndexedModify(&SPC700::algorithmROR, X);
    case 0x7c: return instructionImpliedModify(&SPC700::algorithmROR, A);
    case 0x7d: return instructionTransfer(X, A);
    case 0x7e: return instructionDirectRead(&SPC700::algorithmCMP, Y);
    case 0x7f: return instructionReturnInterrupt();
    case 0x80: return instructionFlagSet(CF, true);
    case 0x81: return instructionCallTable(8);
    case 0x82: return instructionAbsoluteBitSet(4, true);
    case 0x83: return instructionBranchBit(4, true);
    case 0x84: return instructionDirectRead(&SPC700::algorithmADC, A);
    case 0x85: return instructionAbsoluteRead(&SPC700::algorithmADC, A);
    case 0x86: return instructionIndirectXRead(&SPC700::algorithmADC);
    case 0x87: return instructionIndexedIndirectRead(&SPC700::algorithmADC, X);
    case 0x88: return instructionImmediateRead(&SPC700::algorithmADC, A);
    case 0x89: return instructionDirectDirectModify(&SPC700::algorithmADC);
    case 0x8a: return instructionAbsoluteBitModify(4);
    case 0x8b: return instructionDirectModify(&SPC700::algorithmDEC);
    case 0x8c: return instructionAbsoluteModify(&SPC700::algorithmDEC);
    case 0x8d: return instructionImmediateRead(&SPC700::algorithmLD, Y);
    case 0x8e: return instructionPullP();
    case 0x8f: return instructionDirectImmediateWrite();
    case 0x90: return instructionBranch(CF == 0);
    case 0x91: return instructionCallTable(9);
    case 0x92: return instructionAbsoluteBitSet(4, false);
    case 0x93: return instructionBranchBit(4, false);
    case 0x94: return instructionDirectIndexedRead(&SPC700::algorithmADC, A, X);
    case 0x95: return instructionAbsoluteIndexedRead(&SPC700::algorithmADC, X);
    case 0x96: return instructionAbsoluteIndexedRead(&SPC700::algorithmADC, Y);
    case 0x97: return instructionIndirectIndexedRead(&SPC700::algorithmADC, Y);
    case 0x98: return instructionDirectImmediateModify(&SPC700::algorithmADC);
    case 0x99: return instructionIndirectXWriteIndirectY(&SPC700::algorithmADC);
    case 0x9a: return instructionDirectReadWord(&SPC700::algorithmSBW);
    case 0x9b: return instructionDirectIndexedModify(&SPC700::algorithmDEC, X);
    case 0x9c: return instructionImpliedModify(&SPC700::algorithmDEC, A);
    case 0x9d: return instructionTransfer(S, X);
    case 0x9e: return instructionDivide();
    case 0x9f: return instructionExchangeNibble();
    case 0xa0: return instructionFlagSet(IF, true);
    case 0xa1: return instructionCallTable(10);
    case 0xa2: return instructionAbsoluteBitSet(5, true);
    case 0xa3: return instructionBranchBit(5, true);
    case 0xa4: return instructionDirectRead(&SPC700::algorithmSBC, A);
    case 0xa5: return instructionAbsoluteRead(&SPC700::algorithmSBC, A);
    case 0xa6: return instructionIndirectXRead(&SPC700::algorithmSBC);
    case 0xa7: return instructionIndexedIndirectRead(&SPC700::algorithmSBC, X);
    case 0xa8: return instructionImmediateRead(&SPC700::algorithmSBC, A);
    case 0xa9: return instructionDirectDirectModify(&SPC700::algorithmSBC);
    case 0xaa: return instructionAbsoluteBitModify(5);
    case 0xab: return instructionDirectModify(&SPC700::algorithmINC);
    case 0xac: return instructionAbsoluteModify(&SPC700::algorithmINC);
    case 0xad: return instructionImmediateRead(&SPC700::algorithmCMP, Y);
    case 0xae: return instructionPull(A);
    case 0xaf: return instructionIndirectXIncrementWrite(A);
    case 0xb0: return instructionBranch(CF == 1);
    case 0xb1: return instructionCallTable(11);
    case 0xb2: return instructionAbsoluteBitSet(5, false);
    case 0xb3: return instructionBranchBit(5, false);
    case 0xb4: return instructionDirectIndexedRead(&SPC700::algorithmSBC, A, X);
    case 0xb5: return instructionAbsoluteIndexedRead(&SPC700::algorithmSBC, X);
    case 0xb6: return instructionAbsoluteIndexedRead(&SPC700::algorithmSBC, Y);
    case 0xb7: return instructionIndirectIndexedRead(&SPC700::algorithmSBC, Y);
    case 0xb8: return instructionDirectImmediateModify(&SPC700::algorithmSBC);
    case 0xb9: return instructionIndirectXWriteIndirectY(&SPC700::algorithmSBC);
    case 0xba: return instructionDirectReadWord(&SPC700::algorithmLDW);
    case 0xbb: return instructionDirectIndexedModify(&SPC700::algorithmINC, X);
    case 0xbc: return instructionImpliedModify(&SPC700::algorithmINC, A);
    case 0xbd: return instructionTransfer(X, S);
    case 0xbe: return instructionDecimalAdjustSub();
    case 0xbf: return instructionIndirectXIncrementRead(A);
    case 0xc0: return instructionFlagSet(IF, false);
    case 0xc1: return instructionCallTable(12);
    case 0xc2: return instructionAbsoluteBitSet(6, true);
    case 0xc3: return instructionBranchBit(6, true);
    case 0xc4: return instructionDirectWrite(A);
    case 0xc5: return instructionAbsoluteWrite(A);
    case 0xc6: return instructionIndirectXWrite(A);
    case 0xc7: return instructionIndexedIndirectWrite(A, X);
    case 0xc8: return instructionImmediateRead(&SPC700::algorithmCMP, X);
    case 0xc9: return instructionAbsoluteWrite(X);
    case 0xca: return instructionAbsoluteBitModify(6);
    case 0xcb: return instructionDirectWrite(Y);
    case 0xcc: return instructionAbsoluteWrite(Y);
    case 0xcd: return instructionImmediateRead(&SPC700::algorithmLD, X);
    case 0xce: return instructionPull(X);
    case 0xcf: return instructionMultiply();
    case 0xd0: return instructionBranch(ZF == 0);
    case 0xd1: return instructionCallTable(13);
    case 0xd2: return instructionAbsoluteBitSet(6, false);
    case 0xd3: return instructionBranchBit(6, false);
    case 0xd4: return instructionDirectIndexedWrite(A, X);
    case 0xd5: return instructionAbsoluteIndexedWrite(X);
    case 0xd6: return instructionAbsoluteIndexedWrite(Y);
    case 0xd7: return instructionIndirectIndexedWrite(A, Y);
    case 0xd8: return instructionDirectWrite(X);
    case 0xd9: return instructionDirectIndexedWrite(X, Y);
    case 0xda: return instructionDirectWriteWord();
    case 0xdb: return instructionDirectIndexedWrite(Y, X);
    case 0xdc: return instructionImpliedModify(&SPC700::algorithmDEC, Y);
    case 0xdd: return instructionTransfer(Y, A);
    case 0xde: return instructionBranchNotDirectIndexed(X);
    case 0xdf: return instructionDecimalAdjustAdd();
    case 0xe0: return instructionOverflowClear();
    case 0xe1: return instructionCallTable(14);
    case 0xe2: return instructionAbsoluteBitSet(7, true);
    case 0xe3: return instructionBranchBit(7, true);
    case 0xe4: return instructionDirectRead(&SPC700::algorithmLD, A);
    case 0xe5: return instructionAbsoluteRead(&SPC700::algorithmLD, A);
    case 0xe6: return instructionIndirectXRead(&SPC700::algorithmLD);
    case 0xe7: return instructionIndexedIndirectRead(&SPC700::algorithmLD, X);
    case 0xe8: return instructionImmediateRead(&SPC700::algorithmLD, A);
    case 0xe9: return instructionAbsoluteRead(&SPC700::algorithmLD, X);
    case 0xea: return instructionAbsoluteBitModify(7);
    case 0xeb: return instructionDirectRead(&SPC700::algorithmLD, Y);
    case 0xec: return instructionAbsoluteRead(&SPC700::algorithmLD, Y);
    case 0xed: return instructionComplementCarry();
    case 0xee: return instructionPull(Y);
    case 0xef: return instructionWait();
    case 0xf0: return instructionBranch(ZF == 1);
    case 0xf1: return instructionCallTable(15);
    case 0xf2: return instructionAbsoluteBitSet(7, false);
    case 0xf3: return instructionBranchBit(7, false);
    case 0xf4: return instructionDirectIndexedRead(&SPC700::algorithmLD, A, X);
    case 0xf5: return instructionAbsoluteIndexedRead(&SPC700::algorithmLD, X);
    case 0xf6: return instructionAbsoluteIndexedRead(&SPC700::algorithmLD, Y);
    case 0xf7: return instructionIndirectIndexedRead(&SPC700::algorithmLD, Y);
    case 0xf8: return instructionDirectRead(&SPC700::algorithmLD, X);
    case 0xf9: return instructionDirectIndexedRead(&SPC700::algorithmLD, X, Y);
    case 0xfa: return instructionDirectDirectWrite();
    case 0xfb: return instructionDirectIndexedRead(&SPC700::algorithmLD, Y, X);
    case 0xfc: return instructionImpliedModify(&SPC700::algorithmINC, Y);
    case 0xfd: return instructionTransfer(A, Y);
    case 0xfe: return instructionBranchNotYDecrement();
    case 0xff: return instructionStop();
  }
}

void SPC700::serialize(serializer& s) {
  s.integer(r.pc.w);
  s.integer(r.ya.w);
  s.integer(r.x);
  s.integer(r.s);
  s.integer(r.p.c);
  s.integer(r.p.z);
  s.integer(r.p.i);
  s.integer(r.p.h);
  s.integer(r.p.b);
  s.integer(r.p.p);
  s.integer(r.p.v);
  s.integer(r.p.n);

  s.integer(r.wait);
  s.integer(r.stop);
}

void SPC700::power() {
  PC = 0x0000;
  YA = 0x0000;
  X = 0x00;
  S = 0xef;
  P = 0x02;

  r.wait = false;
  r.stop = false;
}

#undef PC
#undef YA
#undef A
#undef X
#undef Y
#undef S
#undef P

#undef CF
#undef ZF
#undef IF
#undef HF
#undef BF
#undef PF
#undef VF
#undef NF

#undef alu

}
