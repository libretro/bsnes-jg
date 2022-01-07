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
#include "../../nall/primitives.hpp"

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

void SPC700::instructionAbsoluteBitModify(nall::Natural< 3> mode) {
  uint16_t address = fetch();
  address |= fetch() << 8;
  nall::Natural< 3> bit = address >> 13;
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

void SPC700::instructionAbsoluteBitSet(nall::Natural< 3> bit, bool value) {
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

void SPC700::instructionBranchBit(nall::Natural< 3> bit, bool match) {
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

void SPC700::instructionCallTable(nall::Natural< 4> vector) {
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

#define op(id, name, ...) case id: return instruction##name(__VA_ARGS__);
#define fp(name) &SPC700::algorithm##name

void SPC700::instruction() {
  switch(fetch()) {
  op(0x00, NoOperation)
  op(0x01, CallTable, 0)
  op(0x02, AbsoluteBitSet, 0, true)
  op(0x03, BranchBit, 0, true)
  op(0x04, DirectRead, fp(OR), A)
  op(0x05, AbsoluteRead, fp(OR), A)
  op(0x06, IndirectXRead, fp(OR))
  op(0x07, IndexedIndirectRead, fp(OR), X)
  op(0x08, ImmediateRead, fp(OR), A)
  op(0x09, DirectDirectModify, fp(OR))
  op(0x0a, AbsoluteBitModify, 0)
  op(0x0b, DirectModify, fp(ASL))
  op(0x0c, AbsoluteModify, fp(ASL))
  op(0x0d, Push, P)
  op(0x0e, TestSetBitsAbsolute, true)
  op(0x0f, Break)
  op(0x10, Branch, NF == 0)
  op(0x11, CallTable, 1)
  op(0x12, AbsoluteBitSet, 0, false)
  op(0x13, BranchBit, 0, false)
  op(0x14, DirectIndexedRead, fp(OR), A, X)
  op(0x15, AbsoluteIndexedRead, fp(OR), X)
  op(0x16, AbsoluteIndexedRead, fp(OR), Y)
  op(0x17, IndirectIndexedRead, fp(OR), Y)
  op(0x18, DirectImmediateModify, fp(OR))
  op(0x19, IndirectXWriteIndirectY, fp(OR))
  op(0x1a, DirectModifyWord, -1)
  op(0x1b, DirectIndexedModify, fp(ASL), X)
  op(0x1c, ImpliedModify, fp(ASL), A)
  op(0x1d, ImpliedModify, fp(DEC), X)
  op(0x1e, AbsoluteRead, fp(CMP), X)
  op(0x1f, JumpIndirectX)
  op(0x20, FlagSet, PF, false)
  op(0x21, CallTable, 2)
  op(0x22, AbsoluteBitSet, 1, true)
  op(0x23, BranchBit, 1, true)
  op(0x24, DirectRead, fp(AND), A)
  op(0x25, AbsoluteRead, fp(AND), A)
  op(0x26, IndirectXRead, fp(AND))
  op(0x27, IndexedIndirectRead, fp(AND), X)
  op(0x28, ImmediateRead, fp(AND), A)
  op(0x29, DirectDirectModify, fp(AND))
  op(0x2a, AbsoluteBitModify, 1)
  op(0x2b, DirectModify, fp(ROL))
  op(0x2c, AbsoluteModify, fp(ROL))
  op(0x2d, Push, A)
  op(0x2e, BranchNotDirect)
  op(0x2f, Branch, true)
  op(0x30, Branch, NF == 1)
  op(0x31, CallTable, 3)
  op(0x32, AbsoluteBitSet, 1, false)
  op(0x33, BranchBit, 1, false)
  op(0x34, DirectIndexedRead, fp(AND), A, X)
  op(0x35, AbsoluteIndexedRead, fp(AND), X)
  op(0x36, AbsoluteIndexedRead, fp(AND), Y)
  op(0x37, IndirectIndexedRead, fp(AND), Y)
  op(0x38, DirectImmediateModify, fp(AND))
  op(0x39, IndirectXWriteIndirectY, fp(AND))
  op(0x3a, DirectModifyWord, +1)
  op(0x3b, DirectIndexedModify, fp(ROL), X)
  op(0x3c, ImpliedModify, fp(ROL), A)
  op(0x3d, ImpliedModify, fp(INC), X)
  op(0x3e, DirectRead, fp(CMP), X)
  op(0x3f, CallAbsolute)
  op(0x40, FlagSet, PF, true)
  op(0x41, CallTable, 4)
  op(0x42, AbsoluteBitSet, 2, true)
  op(0x43, BranchBit, 2, true)
  op(0x44, DirectRead, fp(EOR), A)
  op(0x45, AbsoluteRead, fp(EOR), A)
  op(0x46, IndirectXRead, fp(EOR))
  op(0x47, IndexedIndirectRead, fp(EOR), X)
  op(0x48, ImmediateRead, fp(EOR), A)
  op(0x49, DirectDirectModify, fp(EOR))
  op(0x4a, AbsoluteBitModify, 2)
  op(0x4b, DirectModify, fp(LSR))
  op(0x4c, AbsoluteModify, fp(LSR))
  op(0x4d, Push, X)
  op(0x4e, TestSetBitsAbsolute, false)
  op(0x4f, CallPage)
  op(0x50, Branch, VF == 0)
  op(0x51, CallTable, 5)
  op(0x52, AbsoluteBitSet, 2, false)
  op(0x53, BranchBit, 2, false)
  op(0x54, DirectIndexedRead, fp(EOR), A, X)
  op(0x55, AbsoluteIndexedRead, fp(EOR), X)
  op(0x56, AbsoluteIndexedRead, fp(EOR), Y)
  op(0x57, IndirectIndexedRead, fp(EOR), Y)
  op(0x58, DirectImmediateModify, fp(EOR))
  op(0x59, IndirectXWriteIndirectY, fp(EOR))
  op(0x5a, DirectCompareWord, fp(CPW))
  op(0x5b, DirectIndexedModify, fp(LSR), X)
  op(0x5c, ImpliedModify, fp(LSR), A)
  op(0x5d, Transfer, A, X)
  op(0x5e, AbsoluteRead, fp(CMP), Y)
  op(0x5f, JumpAbsolute)
  op(0x60, FlagSet, CF, false)
  op(0x61, CallTable, 6)
  op(0x62, AbsoluteBitSet, 3, true)
  op(0x63, BranchBit, 3, true)
  op(0x64, DirectRead, fp(CMP), A)
  op(0x65, AbsoluteRead, fp(CMP), A)
  op(0x66, IndirectXRead, fp(CMP))
  op(0x67, IndexedIndirectRead, fp(CMP), X)
  op(0x68, ImmediateRead, fp(CMP), A)
  op(0x69, DirectDirectCompare, fp(CMP))
  op(0x6a, AbsoluteBitModify, 3)
  op(0x6b, DirectModify, fp(ROR))
  op(0x6c, AbsoluteModify, fp(ROR))
  op(0x6d, Push, Y)
  op(0x6e, BranchNotDirectDecrement)
  op(0x6f, ReturnSubroutine)
  op(0x70, Branch, VF == 1)
  op(0x71, CallTable, 7)
  op(0x72, AbsoluteBitSet, 3, false)
  op(0x73, BranchBit, 3, false)
  op(0x74, DirectIndexedRead, fp(CMP), A, X)
  op(0x75, AbsoluteIndexedRead, fp(CMP), X)
  op(0x76, AbsoluteIndexedRead, fp(CMP), Y)
  op(0x77, IndirectIndexedRead, fp(CMP), Y)
  op(0x78, DirectImmediateCompare, fp(CMP))
  op(0x79, IndirectXCompareIndirectY, fp(CMP))
  op(0x7a, DirectReadWord, fp(ADW))
  op(0x7b, DirectIndexedModify, fp(ROR), X)
  op(0x7c, ImpliedModify, fp(ROR), A)
  op(0x7d, Transfer, X, A)
  op(0x7e, DirectRead, fp(CMP), Y)
  op(0x7f, ReturnInterrupt)
  op(0x80, FlagSet, CF, true)
  op(0x81, CallTable, 8)
  op(0x82, AbsoluteBitSet, 4, true)
  op(0x83, BranchBit, 4, true)
  op(0x84, DirectRead, fp(ADC), A)
  op(0x85, AbsoluteRead, fp(ADC), A)
  op(0x86, IndirectXRead, fp(ADC))
  op(0x87, IndexedIndirectRead, fp(ADC), X)
  op(0x88, ImmediateRead, fp(ADC), A)
  op(0x89, DirectDirectModify, fp(ADC))
  op(0x8a, AbsoluteBitModify, 4)
  op(0x8b, DirectModify, fp(DEC))
  op(0x8c, AbsoluteModify, fp(DEC))
  op(0x8d, ImmediateRead, fp(LD), Y)
  op(0x8e, PullP)
  op(0x8f, DirectImmediateWrite)
  op(0x90, Branch, CF == 0)
  op(0x91, CallTable, 9)
  op(0x92, AbsoluteBitSet, 4, false)
  op(0x93, BranchBit, 4, false)
  op(0x94, DirectIndexedRead, fp(ADC), A, X)
  op(0x95, AbsoluteIndexedRead, fp(ADC), X)
  op(0x96, AbsoluteIndexedRead, fp(ADC), Y)
  op(0x97, IndirectIndexedRead, fp(ADC), Y)
  op(0x98, DirectImmediateModify, fp(ADC))
  op(0x99, IndirectXWriteIndirectY, fp(ADC))
  op(0x9a, DirectReadWord, fp(SBW))
  op(0x9b, DirectIndexedModify, fp(DEC), X)
  op(0x9c, ImpliedModify, fp(DEC), A)
  op(0x9d, Transfer, S, X)
  op(0x9e, Divide)
  op(0x9f, ExchangeNibble)
  op(0xa0, FlagSet, IF, true)
  op(0xa1, CallTable, 10)
  op(0xa2, AbsoluteBitSet, 5, true)
  op(0xa3, BranchBit, 5, true)
  op(0xa4, DirectRead, fp(SBC), A)
  op(0xa5, AbsoluteRead, fp(SBC), A)
  op(0xa6, IndirectXRead, fp(SBC))
  op(0xa7, IndexedIndirectRead, fp(SBC), X)
  op(0xa8, ImmediateRead, fp(SBC), A)
  op(0xa9, DirectDirectModify, fp(SBC))
  op(0xaa, AbsoluteBitModify, 5)
  op(0xab, DirectModify, fp(INC))
  op(0xac, AbsoluteModify, fp(INC))
  op(0xad, ImmediateRead, fp(CMP), Y)
  op(0xae, Pull, A)
  op(0xaf, IndirectXIncrementWrite, A)
  op(0xb0, Branch, CF == 1)
  op(0xb1, CallTable, 11)
  op(0xb2, AbsoluteBitSet, 5, false)
  op(0xb3, BranchBit, 5, false)
  op(0xb4, DirectIndexedRead, fp(SBC), A, X)
  op(0xb5, AbsoluteIndexedRead, fp(SBC), X)
  op(0xb6, AbsoluteIndexedRead, fp(SBC), Y)
  op(0xb7, IndirectIndexedRead, fp(SBC), Y)
  op(0xb8, DirectImmediateModify, fp(SBC))
  op(0xb9, IndirectXWriteIndirectY, fp(SBC))
  op(0xba, DirectReadWord, fp(LDW))
  op(0xbb, DirectIndexedModify, fp(INC), X)
  op(0xbc, ImpliedModify, fp(INC), A)
  op(0xbd, Transfer, X, S)
  op(0xbe, DecimalAdjustSub)
  op(0xbf, IndirectXIncrementRead, A)
  op(0xc0, FlagSet, IF, false)
  op(0xc1, CallTable, 12)
  op(0xc2, AbsoluteBitSet, 6, true)
  op(0xc3, BranchBit, 6, true)
  op(0xc4, DirectWrite, A)
  op(0xc5, AbsoluteWrite, A)
  op(0xc6, IndirectXWrite, A)
  op(0xc7, IndexedIndirectWrite, A, X)
  op(0xc8, ImmediateRead, fp(CMP), X)
  op(0xc9, AbsoluteWrite, X)
  op(0xca, AbsoluteBitModify, 6)
  op(0xcb, DirectWrite, Y)
  op(0xcc, AbsoluteWrite, Y)
  op(0xcd, ImmediateRead, fp(LD), X)
  op(0xce, Pull, X)
  op(0xcf, Multiply)
  op(0xd0, Branch, ZF == 0)
  op(0xd1, CallTable, 13)
  op(0xd2, AbsoluteBitSet, 6, false)
  op(0xd3, BranchBit, 6, false)
  op(0xd4, DirectIndexedWrite, A, X)
  op(0xd5, AbsoluteIndexedWrite, X)
  op(0xd6, AbsoluteIndexedWrite, Y)
  op(0xd7, IndirectIndexedWrite, A, Y)
  op(0xd8, DirectWrite, X)
  op(0xd9, DirectIndexedWrite, X, Y)
  op(0xda, DirectWriteWord)
  op(0xdb, DirectIndexedWrite, Y, X)
  op(0xdc, ImpliedModify, fp(DEC), Y)
  op(0xdd, Transfer, Y, A)
  op(0xde, BranchNotDirectIndexed, X)
  op(0xdf, DecimalAdjustAdd)
  op(0xe0, OverflowClear)
  op(0xe1, CallTable, 14)
  op(0xe2, AbsoluteBitSet, 7, true)
  op(0xe3, BranchBit, 7, true)
  op(0xe4, DirectRead, fp(LD), A)
  op(0xe5, AbsoluteRead, fp(LD), A)
  op(0xe6, IndirectXRead, fp(LD))
  op(0xe7, IndexedIndirectRead, fp(LD), X)
  op(0xe8, ImmediateRead, fp(LD), A)
  op(0xe9, AbsoluteRead, fp(LD), X)
  op(0xea, AbsoluteBitModify, 7)
  op(0xeb, DirectRead, fp(LD), Y)
  op(0xec, AbsoluteRead, fp(LD), Y)
  op(0xed, ComplementCarry)
  op(0xee, Pull, Y)
  op(0xef, Wait)
  op(0xf0, Branch, ZF == 1)
  op(0xf1, CallTable, 15)
  op(0xf2, AbsoluteBitSet, 7, false)
  op(0xf3, BranchBit, 7, false)
  op(0xf4, DirectIndexedRead, fp(LD), A, X)
  op(0xf5, AbsoluteIndexedRead, fp(LD), X)
  op(0xf6, AbsoluteIndexedRead, fp(LD), Y)
  op(0xf7, IndirectIndexedRead, fp(LD), Y)
  op(0xf8, DirectRead, fp(LD), X)
  op(0xf9, DirectIndexedRead, fp(LD), X, Y)
  op(0xfa, DirectDirectWrite)
  op(0xfb, DirectIndexedRead, fp(LD), Y, X)
  op(0xfc, ImpliedModify, fp(INC), Y)
  op(0xfd, Transfer, A, Y)
  op(0xfe, BranchNotYDecrement)
  op(0xff, Stop)
  }
}

#undef op
#undef fp

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
