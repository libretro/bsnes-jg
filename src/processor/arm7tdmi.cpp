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

#include <bitset>
#include <cassert>

#include "serializer.hpp"

#include "arm7tdmi.hpp"

namespace Processor {

ARM7TDMI::~ARM7TDMI() {}

ARM7TDMI::GPR::operator uint32_t() const {
  return data;
}

ARM7TDMI::GPR& ARM7TDMI::GPR::operator=(const GPR& value) {
  return operator=(value.data);
}

ARM7TDMI::GPR& ARM7TDMI::GPR::operator=(uint32_t value) {
  data = value;
  if(modify) modify();
  return *this;
}

ARM7TDMI::GPR& ARM7TDMI::r(uint8_t index) {
  switch(index) {
  case  0: return processor.r0;
  case  1: return processor.r1;
  case  2: return processor.r2;
  case  3: return processor.r3;
  case  4: return processor.r4;
  case  5: return processor.r5;
  case  6: return processor.r6;
  case  7: return processor.r7;
  case  8: return processor.cpsr.m == PSR::FIQ ? processor.fiq.r8  : processor.r8;
  case  9: return processor.cpsr.m == PSR::FIQ ? processor.fiq.r9  : processor.r9;
  case 10: return processor.cpsr.m == PSR::FIQ ? processor.fiq.r10 : processor.r10;
  case 11: return processor.cpsr.m == PSR::FIQ ? processor.fiq.r11 : processor.r11;
  case 12: return processor.cpsr.m == PSR::FIQ ? processor.fiq.r12 : processor.r12;
  case 13: switch(processor.cpsr.m) {
    case PSR::FIQ: return processor.fiq.r13;
    case PSR::IRQ: return processor.irq.r13;
    case PSR::SVC: return processor.svc.r13;
    case PSR::ABT: return processor.abt.r13;
    case PSR::UND: return processor.und.r13;
    default: return processor.r13;
  }
  case 14: switch(processor.cpsr.m) {
    case PSR::FIQ: return processor.fiq.r14;
    case PSR::IRQ: return processor.irq.r14;
    case PSR::SVC: return processor.svc.r14;
    case PSR::ABT: return processor.abt.r14;
    case PSR::UND: return processor.und.r14;
    default: return processor.r14;
  }
  case 15: return processor.r15;
  }
  throw; // unreachable
}

ARM7TDMI::PSR& ARM7TDMI::cpsr() {
  return processor.cpsr;
}

ARM7TDMI::PSR& ARM7TDMI::spsr() {
  switch(processor.cpsr.m) {
  case PSR::FIQ: return processor.fiq.spsr;
  case PSR::IRQ: return processor.irq.spsr;
  case PSR::SVC: return processor.svc.spsr;
  case PSR::ABT: return processor.abt.spsr;
  case PSR::UND: return processor.und.spsr;
  }
  throw;
}

bool ARM7TDMI::privileged() const {
  return processor.cpsr.m != PSR::USR;
}

bool ARM7TDMI::exception() const {
  return privileged() && processor.cpsr.m != PSR::SYS;
}

void ARM7TDMI::idle() {
  pipeline.nonsequential = true;
  sleep();
}

uint32_t ARM7TDMI::read(unsigned mode, uint32_t address) {
  return get(mode, address);
}

uint32_t ARM7TDMI::load(unsigned mode, uint32_t address) {
  pipeline.nonsequential = true;
  uint32_t word = get(Load | mode, address);
  if(mode & Half) {
    address &= 1;
    word = mode & Signed ? (uint32_t)(int16_t)word : (uint32_t)(uint16_t)word;
  }
  if(mode & Byte) {
    address &= 0;
    word = mode & Signed ? (uint32_t)(int8_t)word : (uint32_t)(uint8_t)word;
  }
  if(mode & Signed) {
    word = ASR(word, (address & 3) << 3);
  } else {
    word = ROR(word, (address & 3) << 3);
  }
  idle();
  return word;
}

void ARM7TDMI::write(unsigned mode, uint32_t address, uint32_t word) {
  pipeline.nonsequential = true;
  return set(mode, address, word);
}

void ARM7TDMI::store(unsigned mode, uint32_t address, uint32_t word) {
  pipeline.nonsequential = true;
  if(mode & Half) { word &= 0xffff; word |= word << 16; }
  if(mode & Byte) { word &= 0xff; word |= word << 8; word |= word << 16; }
  return set(Store | mode, address, word);
}

uint32_t ARM7TDMI::ADD(uint32_t source, uint32_t modify, bool flagc) {
  uint32_t result = source + modify + flagc;
  if(cpsr().t || (opcode & 1 << 20)) {
    uint32_t overflow = ~(source ^ modify) & (source ^ result);
    cpsr().v = (unsigned{1} << 31) & (overflow);
    cpsr().c = (unsigned{1} << 31) & (overflow ^ source ^ modify ^ result);
    cpsr().z = result == 0;
    cpsr().n = result >> 31;
  }
  return result;
}

uint32_t ARM7TDMI::ASR(uint32_t source, uint8_t shift) {
  carry = cpsr().c;
  if(shift == 0) return source;
  carry = shift > 32 ? source & (unsigned{1} << 31) : source & 1 << (shift - 1);
  source = shift > 31 ? (int32_t)source >> 31 : (int32_t)source >> shift;
  return source;
}

uint32_t ARM7TDMI::BIT(uint32_t result) {
  if(cpsr().t || (opcode & 1 << 20)) {
    cpsr().c = carry;
    cpsr().z = result == 0;
    cpsr().n = result >> 31;
  }
  return result;
}

uint32_t ARM7TDMI::LSL(uint32_t source, uint8_t shift) {
  carry = cpsr().c;
  if(shift == 0) return source;
  carry = shift > 32 ? 0 : source & 1 << (32 - shift);
  source = shift > 31 ? 0 : source << shift;
  return source;
}

uint32_t ARM7TDMI::LSR(uint32_t source, uint8_t shift) {
  carry = cpsr().c;
  if(shift == 0) return source;
  carry = shift > 32 ? 0 : source & 1 << (shift - 1);
  source = shift > 31 ? 0 : source >> shift;
  return source;
}

uint32_t ARM7TDMI::MUL(uint32_t product, uint32_t multiplicand, uint32_t multiplier) {
  idle();
  if(multiplier >>  8 && multiplier >>  8 != 0xffffff) idle();
  if(multiplier >> 16 && multiplier >> 16 !=   0xffff) idle();
  if(multiplier >> 24 && multiplier >> 24 !=     0xff) idle();
  product += multiplicand * multiplier;
  if(cpsr().t || (opcode & 1 << 20)) {
    cpsr().z = product == 0;
    cpsr().n = product >> 31;
  }
  return product;
}

uint32_t ARM7TDMI::ROR(uint32_t source, uint8_t shift) {
  carry = cpsr().c;
  if(shift == 0) return source;
  if(shift &= 31) source = source << (32 - shift) | source >> shift;
  carry = source & (unsigned{1} << 31);
  return source;
}

uint32_t ARM7TDMI::RRX(uint32_t source) {
  carry = source & 1;
  return cpsr().c << 31 | source >> 1;
}

uint32_t ARM7TDMI::SUB(uint32_t source, uint32_t modify, bool flagc) {
  return ADD(source, ~modify, flagc);
}

bool ARM7TDMI::TST(uint8_t mode) {
  switch(mode) {
  case  0: return cpsr().z == 1;                          //EQ (equal)
  case  1: return cpsr().z == 0;                          //NE (not equal)
  case  2: return cpsr().c == 1;                          //CS (carry set)
  case  3: return cpsr().c == 0;                          //CC (carry clear)
  case  4: return cpsr().n == 1;                          //MI (negative)
  case  5: return cpsr().n == 0;                          //PL (positive)
  case  6: return cpsr().v == 1;                          //VS (overflow)
  case  7: return cpsr().v == 0;                          //VC (no overflow)
  case  8: return cpsr().c == 1 && cpsr().z == 0;         //HI (unsigned higher)
  case  9: return cpsr().c == 0 || cpsr().z == 1;         //LS (unsigned lower or same)
  case 10: return cpsr().n == cpsr().v;                   //GE (signed greater than or equal)
  case 11: return cpsr().n != cpsr().v;                   //LT (signed less than)
  case 12: return cpsr().z == 0 && cpsr().n == cpsr().v;  //GT (signed greater than)
  case 13: return cpsr().z == 1 || cpsr().n != cpsr().v;  //LE (signed less than or equal)
  case 14: return true;                                   //AL (always)
  case 15: return false;                                  //NV (never)
  }
  return 0; // unreachable
}

void ARM7TDMI::fetch() {
  pipeline.execute = pipeline.decode;
  pipeline.decode = pipeline.fetch;
  pipeline.decode.thumb = cpsr().t;

  unsigned sequential = Sequential;
  if(pipeline.nonsequential) {
    pipeline.nonsequential = false;
    sequential = Nonsequential;
  }

  unsigned mask = !cpsr().t ? 3 : 1;
  unsigned size = !cpsr().t ? Word : Half;

  r(15).data += size >> 3;
  pipeline.fetch.address = r(15) & ~mask;
  pipeline.fetch.instruction = read(Prefetch | size | sequential, pipeline.fetch.address);
}

void ARM7TDMI::instruction() {
  unsigned mask = !cpsr().t ? 3 : 1;
  unsigned size = !cpsr().t ? Word : Half;

  if(pipeline.reload) {
    pipeline.reload = false;
    r(15).data &= ~mask;
    pipeline.fetch.address = r(15) & ~mask;
    pipeline.fetch.instruction = read(Prefetch | size | Nonsequential, pipeline.fetch.address);
    fetch();
  }
  fetch();

  if(irq && !cpsr().i) {
    exception(PSR::IRQ, 0x18);
    if(pipeline.execute.thumb) r(14).data += 2;
    return;
  }

  opcode = pipeline.execute.instruction;
  if(!pipeline.execute.thumb) {
    if(!TST(opcode >> 28)) return;
    uint16_t index = (opcode & 0x0ff00000) >> 16 | (opcode & 0x000000f0) >> 4;
    armInstruction[index](opcode);
  } else {
    thumbInstruction[(uint16_t)opcode]();
  }
}

void ARM7TDMI::exception(unsigned mode, uint32_t address) {
  PSR psr = cpsr();
  cpsr().m = mode;
  spsr() = psr;
  cpsr().t = 0;
  if(cpsr().m == PSR::FIQ) cpsr().f = 1;
  cpsr().i = 1;
  r(14) = pipeline.decode.address;
  r(15) = address;
}

void ARM7TDMI::armInitialize() {
  #define pattern(s) \
    std::integral_constant<uint32_t, s>::value
    //std::integral_constant<uint32_t, nall::test(s)>::value

  #define bit1(value, index) (value >> index & 1)
  #define bits(value, lo, hi) (value >> lo & ((1ull << (hi - lo + 1)) - 1))

  #define arguments \
    bits(opcode, 0,23),  /* displacement */ \
    bit1(opcode,24)      /* link */
  for(unsigned displacementLo = 0; displacementLo < 16; ++displacementLo)
  for(unsigned displacementHi = 0; displacementHi < 16; ++displacementHi)
  for(unsigned link = 0; link < 2; ++link) {
    //uint32_t op = pattern(".... 101? ???? ???? ???? ???? ???? ????")
    uint32_t op = pattern(0xa000000)
                | displacementLo << 4 | displacementHi << 20 | link << 24;
    unsigned index = (op & 0x0ff00000) >> 16 | (op & 0x000000f0) >> 4;
    assert(!armInstruction[index]);
    armInstruction[index] = [&](uint32_t) { return armInstructionBranch(arguments); };
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 3)   /* m */
  {
    //uint32_t op = pattern(".... 0001 0010 ---- ---- ---- 0001 ????");
    uint32_t op = pattern(0x1200010);
    unsigned index = (op & 0x0ff00000) >> 16 | (op & 0x000000f0) >> 4;
    assert(!armInstruction[index]);
    armInstruction[index] = [&](uint32_t) { return armInstructionBranchExchangeRegister(arguments); };
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 7),  /* immediate */ \
    bits(opcode, 8,11),  /* shift */ \
    bits(opcode,12,15),  /* d */ \
    bits(opcode,16,19),  /* n */ \
    bit1(opcode,20),     /* save */ \
    bits(opcode,21,24)   /* mode */
  for(unsigned shiftHi = 0; shiftHi < 16; ++shiftHi)
  for(unsigned save = 0; save < 2; ++save)
  for(unsigned mode = 0; mode < 16; ++mode) {
    if(mode >= 8 && mode <= 11 && !save) continue;  //TST, TEQ, CMP, CMN
    //uint32_t op = pattern(".... 001? ???? ???? ???? ???? ???? ????") | shiftHi << 4 | save << 20 | mode << 21;
    uint32_t op = pattern(0x2000000) | shiftHi << 4 | save << 20 | mode << 21;
    unsigned index = (op & 0x0ff00000) >> 16 | (op & 0x000000f0) >> 4;
    assert(!armInstruction[index]);
    armInstruction[index] = [&](uint32_t) { return armInstructionDataImmediate(arguments); };
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 3),  /* m */ \
    bits(opcode, 5, 6),  /* type */ \
    bits(opcode, 7,11),  /* shift */ \
    bits(opcode,12,15),  /* d */ \
    bits(opcode,16,19),  /* n */ \
    bit1(opcode,20),     /* save */ \
    bits(opcode,21,24)   /* mode */
  for(unsigned type = 0; type < 4; ++type)
  for(unsigned shiftLo = 0; shiftLo < 2; ++shiftLo)
  for(unsigned save = 0; save < 2; ++save)
  for(unsigned mode = 0; mode < 16; ++mode) {
    if(mode >= 8 && mode <= 11 && !save) continue;  //TST, TEQ, CMP, CMN
    //uint32_t op = pattern(".... 000? ???? ???? ???? ???? ???0 ????") | type << 5 | shiftLo << 7 | save << 20 | mode << 21;
    uint32_t op = pattern(0x0000000) | type << 5 | shiftLo << 7 | save << 20 | mode << 21;
    unsigned index = (op & 0x0ff00000) >> 16 | (op & 0x000000f0) >> 4;
    assert(!armInstruction[index]);
    armInstruction[index] = [&](uint32_t) { return armInstructionDataImmediateShift(arguments); };
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 3),  /* m */ \
    bits(opcode, 5, 6),  /* type */ \
    bits(opcode, 8,11),  /* s */ \
    bits(opcode,12,15),  /* d */ \
    bits(opcode,16,19),  /* n */ \
    bit1(opcode,20),     /* save */ \
    bits(opcode,21,24)   /* mode */
  for(unsigned type = 0; type < 4; ++type)
  for(unsigned save = 0; save < 2; ++save)
  for(unsigned mode = 0; mode < 16; ++mode) {
    if(mode >= 8 && mode <= 11 && !save) continue;  //TST, TEQ, CMP, CMN
    //uint32_t op = pattern(".... 000? ???? ???? ???? ???? 0??1 ????") | type << 5 | save << 20 | mode << 21;
    uint32_t op = pattern(0x0000010) | type << 5 | save << 20 | mode << 21;
    unsigned index = (op & 0x0ff00000) >> 16 | (op & 0x000000f0) >> 4;
    assert(!armInstruction[index]);
    armInstruction[index] = [&](uint32_t) { return armInstructionDataRegisterShift(arguments); };
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 3) | bits(opcode, 8,11) << 4,  /* immediate */ \
    bit1(opcode, 5),     /* half */ \
    bits(opcode,12,15),  /* d */ \
    bits(opcode,16,19),  /* n */ \
    bit1(opcode,21),     /* writeback */ \
    bit1(opcode,23),     /* up */ \
    bit1(opcode,24)      /* pre */
  for(unsigned half = 0; half < 2; ++half)
  for(unsigned writeback = 0; writeback < 2; ++writeback)
  for(unsigned up = 0; up < 2; ++up)
  for(unsigned pre = 0; pre < 2; ++pre) {
    //uint32_t op = pattern(".... 000? ?1?1 ???? ???? ???? 11?1 ????") | half << 5 | writeback << 21 | up << 23 | pre << 24;
    uint32_t op = pattern(0x05000d0) | half << 5 | writeback << 21 | up << 23 | pre << 24;
    unsigned index = (op & 0x0ff00000) >> 16 | (op & 0x000000f0) >> 4;
    assert(!armInstruction[index]);
    armInstruction[index] = [&](uint32_t) { return armInstructionLoadImmediate(arguments); };
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 3),  /* m */ \
    bit1(opcode, 5),     /* half */ \
    bits(opcode,12,15),  /* d */ \
    bits(opcode,16,19),  /* n */ \
    bit1(opcode,21),     /* writeback */ \
    bit1(opcode,23),     /* up */ \
    bit1(opcode,24)      /* pre */
  for(unsigned half = 0; half < 2; ++half)
  for(unsigned writeback = 0; writeback < 2; ++writeback)
  for(unsigned up = 0; up < 2; ++up)
  for(unsigned pre = 0; pre < 2; ++pre) {
    //uint32_t op = pattern(".... 000? ?0?1 ???? ???? ---- 11?1 ????") | half << 5 | writeback << 21 | up << 23 | pre << 24;
    uint32_t op = pattern(0x01000d0) | half << 5 | writeback << 21 | up << 23 | pre << 24;
    unsigned index = (op & 0x0ff00000) >> 16 | (op & 0x000000f0) >> 4;
    assert(!armInstruction[index]);
    armInstruction[index] = [&](uint32_t) { return armInstructionLoadRegister(arguments); };
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 3),  /* m */ \
    bits(opcode,12,15),  /* d */ \
    bits(opcode,16,19),  /* n */ \
    bit1(opcode,22)      /* byte */
  for(unsigned byte = 0; byte < 2; ++byte) {
    //uint32_t op = pattern(".... 0001 0?00 ???? ???? ---- 1001 ????") | byte << 22;
    uint32_t op = pattern(0x1000090) | byte << 22;
    unsigned index = (op & 0x0ff00000) >> 16 | (op & 0x000000f0) >> 4;
    assert(!armInstruction[index]);
    armInstruction[index] = [&](uint32_t) { return armInstructionMemorySwap(arguments); };
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 3) | bits(opcode, 8,11) << 4,  /* immediate */ \
    bits(opcode,12,15),  /* d */ \
    bits(opcode,16,19),  /* n */ \
    bit1(opcode,20),     /* mode */ \
    bit1(opcode,21),     /* writeback */ \
    bit1(opcode,23),     /* up */ \
    bit1(opcode,24)      /* pre */
  for(unsigned mode = 0; mode < 2; ++mode)
  for(unsigned writeback = 0; writeback < 2; ++writeback)
  for(unsigned up = 0; up < 2; ++up)
  for(unsigned pre = 0; pre < 2; ++pre) {
    //uint32_t op = pattern(".... 000? ?1?? ???? ???? ???? 1011 ????") | mode << 20 | writeback << 21 | up << 23 | pre << 24;
    uint32_t op = pattern(0x04000b0) | mode << 20 | writeback << 21 | up << 23 | pre << 24;
    unsigned index = (op & 0x0ff00000) >> 16 | (op & 0x000000f0) >> 4;
    assert(!armInstruction[index]);
    armInstruction[index] = [&](uint32_t) { return armInstructionMoveHalfImmediate(arguments); };
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 3),  /* m */ \
    bits(opcode,12,15),  /* d */ \
    bits(opcode,16,19),  /* n */ \
    bit1(opcode,20),     /* mode */ \
    bit1(opcode,21),     /* writeback */ \
    bit1(opcode,23),     /* up */ \
    bit1(opcode,24)      /* pre */
  for(unsigned mode = 0; mode < 2; ++mode)
  for(unsigned writeback = 0; writeback < 2; ++writeback)
  for(unsigned up = 0; up < 2; ++up)
  for(unsigned pre = 0; pre < 2; ++pre) {
    //uint32_t op = pattern(".... 000? ?0?? ???? ???? ---- 1011 ????") | mode << 20 | writeback << 21 | up << 23 | pre << 24;
    uint32_t op = pattern(0x00000b0) | mode << 20 | writeback << 21 | up << 23 | pre << 24;
    unsigned index = (op & 0x0ff00000) >> 16 | (op & 0x000000f0) >> 4;
    assert(!armInstruction[index]);
    armInstruction[index] = [&](uint32_t) { return armInstructionMoveHalfRegister(arguments); };
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0,11),  /* immediate */ \
    bits(opcode,12,15),  /* d */ \
    bits(opcode,16,19),  /* n */ \
    bit1(opcode,20),     /* mode */ \
    bit1(opcode,21),     /* writeback */ \
    bit1(opcode,22),     /* byte */ \
    bit1(opcode,23),     /* up */ \
    bit1(opcode,24)      /* pre */
  for(unsigned immediatePart = 0; immediatePart < 16; ++immediatePart)
  for(unsigned mode = 0; mode < 2; ++mode)
  for(unsigned writeback = 0; writeback < 2; ++writeback)
  for(unsigned byte = 0; byte < 2; ++byte)
  for(unsigned up = 0; up < 2; ++up)
  for(unsigned pre = 0; pre < 2; ++pre) {
    //uint32_t op = pattern(".... 010? ???? ???? ???? ???? ???? ????")
    uint32_t op = pattern(0x4000000)
                | immediatePart << 4 | mode << 20 | writeback << 21 | byte << 22 | up << 23 | pre << 24;
    unsigned index = (op & 0x0ff00000) >> 16 | (op & 0x000000f0) >> 4;
    assert(!armInstruction[index]);
    armInstruction[index] = [&](uint32_t) { return armInstructionMoveImmediateOffset(arguments); };
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0,15),  /* list */ \
    bits(opcode,16,19),  /* n */ \
    bit1(opcode,20),     /* mode */ \
    bit1(opcode,21),     /* writeback */ \
    bit1(opcode,22),     /* type */ \
    bit1(opcode,23),     /* up */ \
    bit1(opcode,24)      /* pre */
  for(unsigned listPart = 0; listPart < 16; ++listPart)
  for(unsigned mode = 0; mode < 2; ++mode)
  for(unsigned writeback = 0; writeback < 2; ++writeback)
  for(unsigned type = 0; type < 2; ++type)
  for(unsigned up = 0; up < 2; ++up)
  for(unsigned pre = 0; pre < 2; ++pre) {
    //uint32_t op = pattern(".... 100? ???? ???? ???? ???? ???? ????")
    uint32_t op = pattern(0x8000000)
                | listPart << 4 | mode << 20 | writeback << 21 | type << 22 | up << 23 | pre << 24;
    unsigned index = (op & 0x0ff00000) >> 16 | (op & 0x000000f0) >> 4;
    assert(!armInstruction[index]);
    armInstruction[index] = [&](uint32_t) { return armInstructionMoveMultiple(arguments); };
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 3),  /* m */ \
    bits(opcode, 5, 6),  /* type */ \
    bits(opcode, 7,11),  /* shift */ \
    bits(opcode,12,15),  /* d */ \
    bits(opcode,16,19),  /* n */ \
    bit1(opcode,20),     /* mode */ \
    bit1(opcode,21),     /* writeback */ \
    bit1(opcode,22),     /* byte */ \
    bit1(opcode,23),     /* up */ \
    bit1(opcode,24)      /* pre */
  for(unsigned type = 0; type < 4; ++type)
  for(unsigned shiftLo = 0; shiftLo < 2; ++shiftLo)
  for(unsigned mode = 0; mode < 2; ++mode)
  for(unsigned writeback = 0; writeback < 2; ++writeback)
  for(unsigned byte = 0; byte < 2; ++byte)
  for(unsigned up = 0; up < 2; ++up)
  for(unsigned pre = 0; pre < 2; ++pre) {
    //uint32_t op = pattern(".... 011? ???? ???? ???? ???? ???0 ????")
    uint32_t op = pattern(0x6000000)
                | type << 5 | shiftLo << 7 | mode << 20 | writeback << 21 | byte << 22 | up << 23 | pre << 24;
    unsigned index = (op & 0x0ff00000) >> 16 | (op & 0x000000f0) >> 4;
    assert(!armInstruction[index]);
    armInstruction[index] = [&](uint32_t) { return armInstructionMoveRegisterOffset(arguments); };
  }
  #undef arguments

  #define arguments \
    bits(opcode,12,15),  /* d */ \
    bit1(opcode,22)      /* mode */
  for(unsigned mode = 0; mode < 2; ++mode) {
    //uint32_t op = pattern(".... 0001 0?00 ---- ???? ---- 0000 ----") | mode << 22;
    uint32_t op = pattern(0x1000000) | mode << 22;
    unsigned index = (op & 0x0ff00000) >> 16 | (op & 0x000000f0) >> 4;
    assert(!armInstruction[index]);
    armInstruction[index] = [&](uint32_t) { return armInstructionMoveToRegisterFromStatus(arguments); };
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 7),  /* immediate */ \
    bits(opcode, 8,11),  /* rotate */ \
    bits(opcode,16,19),  /* field */ \
    bit1(opcode,22)      /* mode */
  for(unsigned immediateHi = 0; immediateHi < 16; ++immediateHi)
  for(unsigned mode = 0; mode < 2; ++mode) {
    //uint32_t op = pattern(".... 0011 0?10 ???? ---- ???? ???? ????") | immediateHi << 4 | mode << 22;
    uint32_t op = pattern(0x3200000) | immediateHi << 4 | mode << 22;
    unsigned index = (op & 0x0ff00000) >> 16 | (op & 0x000000f0) >> 4;
    assert(!armInstruction[index]);
    armInstruction[index] = [&](uint32_t) { return armInstructionMoveToStatusFromImmediate(arguments); };
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 3),  /* m */ \
    bits(opcode,16,19),  /* field */ \
    bit1(opcode,22)      /* mode */
  for(unsigned mode = 0; mode < 2; ++mode) {
    //uint32_t op = pattern(".... 0001 0?10 ???? ---- ---- 0000 ????") | mode << 22;
    uint32_t op = pattern(0x1200000) | mode << 22;
    unsigned index = (op & 0x0ff00000) >> 16 | (op & 0x000000f0) >> 4;
    assert(!armInstruction[index]);
    armInstruction[index] = [&](uint32_t) { return armInstructionMoveToStatusFromRegister(arguments); };
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 3),  /* m */ \
    bits(opcode, 8,11),  /* s */ \
    bits(opcode,12,15),  /* n */ \
    bits(opcode,16,19),  /* d */ \
    bit1(opcode,20),     /* save */ \
    bit1(opcode,21)      /* accumulate */
  for(unsigned save = 0; save < 2; ++save)
  for(unsigned accumulate = 0; accumulate < 2; ++accumulate) {
    //uint32_t op = pattern(".... 0000 00?? ???? ???? ???? 1001 ????") | save << 20 | accumulate << 21;
    uint32_t op = pattern(0x0000090) | save << 20 | accumulate << 21;
    unsigned index = (op & 0x0ff00000) >> 16 | (op & 0x000000f0) >> 4;
    assert(!armInstruction[index]);
    armInstruction[index] = [&](uint32_t) { return armInstructionMultiply(arguments); };
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0, 3),  /* m */ \
    bits(opcode, 8,11),  /* s */ \
    bits(opcode,12,15),  /* l */ \
    bits(opcode,16,19),  /* h */ \
    bit1(opcode,20),     /* save */ \
    bit1(opcode,21),     /* accumulate */ \
    bit1(opcode,22)      /* sign */
  for(unsigned save = 0; save < 2; ++save)
  for(unsigned accumulate = 0; accumulate < 2; ++accumulate)
  for(unsigned sign = 0; sign < 2; ++sign) {
    //uint32_t op = pattern(".... 0000 1??? ???? ???? ???? 1001 ????") | save << 20 | accumulate << 21 | sign << 22;
    uint32_t op = pattern(0x0800090) | save << 20 | accumulate << 21 | sign << 22;
    unsigned index = (op & 0x0ff00000) >> 16 | (op & 0x000000f0) >> 4;
    assert(!armInstruction[index]);
    armInstruction[index] = [&](uint32_t) { return armInstructionMultiplyLong(arguments); };
  }
  #undef arguments

  #define arguments \
    bits(opcode, 0,23)  /* immediate */
  for(unsigned immediateLo = 0; immediateLo < 16; ++immediateLo)
  for(unsigned immediateHi = 0; immediateHi < 16; ++immediateHi) {
    //uint32_t op = pattern(".... 1111 ???? ???? ???? ???? ???? ????") | immediateLo << 4 | immediateHi << 20;
    uint32_t op = pattern(0xf000000) | immediateLo << 4 | immediateHi << 20;
    unsigned index = (op & 0x0ff00000) >> 16 | (op & 0x000000f0) >> 4;
    assert(!armInstruction[index]);
    armInstruction[index] = [&](uint32_t) { return armInstructionSoftwareInterrupt(arguments); };
  }
  #undef arguments

  #define arguments
  for(unsigned id = 0; id < 4096; ++id) {
    if(armInstruction[id]) continue;
    //uint32_t op = pattern(".... ???? ???? ---- ---- ---- ???? ----") | bits(id,0,3) << 4 | bits(id,4,11) << 20;
    uint32_t op = pattern(0x0000000) | bits(id,0,3) << 4 | bits(id,4,11) << 20;
    unsigned index = (op & 0x0ff00000) >> 16 | (op & 0x000000f0) >> 4;
    assert(!armInstruction[index]);
    armInstruction[index] = [&](uint32_t) { return armInstructionUndefined(arguments); };
  }
  #undef arguments

  #undef pattern
}

void ARM7TDMI::thumbInitialize() {
  #define pattern(s) \
    std::integral_constant<uint16_t, s>::value
    //std::integral_constant<uint16_t, nall::test(s)>::value

  for(unsigned d = 0; d < 8; ++d)
  for(unsigned m = 0; m < 8; ++m)
  for(unsigned mode = 0; mode < 16; ++mode) {
    //uint16_t op = pattern("0100 00?? ???? ????") | d | m << 3 | mode << 6;
    uint16_t op = pattern(0x4000) | d | m << 3 | mode << 6;
    assert(!thumbInstruction[op]);
    thumbInstruction[op] = [=] { return thumbInstructionALU(d, m, mode); };
  }

  for(unsigned d = 0; d < 16; ++d)
  for(unsigned m = 0; m < 16; ++m)
  for(unsigned mode = 0; mode < 4; ++mode) {
    if(mode == 3) continue;
    //uint16_t op = pattern("0100 01?? ???? ????") | bits(d,0,2) | m << 3 | bit1(d,3) << 7 | mode << 8;
    uint16_t op = pattern(0x4400) | bits(d,0,2) | m << 3 | bit1(d,3) << 7 | mode << 8;
    assert(!thumbInstruction[op]);
    thumbInstruction[op] = [=] { return thumbInstructionALUExtended(d, m, mode); };
  }

  for(unsigned immediate = 0; immediate < 256; ++immediate)
  for(unsigned d = 0; d < 8; ++d)
  for(unsigned mode = 0; mode < 2; ++mode) {
    //uint16_t op = pattern("1010 ???? ???? ????") | immediate | d << 8 | mode << 11;
    uint16_t op = pattern(0xa000) | immediate | d << 8 | mode << 11;
    assert(!thumbInstruction[op]);
    thumbInstruction[op] = [=] { return thumbInstructionAddRegister(immediate, d, mode); };
  }

  for(unsigned d = 0; d < 8; ++d)
  for(unsigned n = 0; n < 8; ++n)
  for(unsigned immediate = 0; immediate < 8; ++immediate)
  for(unsigned mode = 0; mode < 2; ++mode) {
    //uint16_t op = pattern("0001 11?? ???? ????") | d | n << 3 | immediate << 6 | mode << 9;
    uint16_t op = pattern(0x1c00) | d | n << 3 | immediate << 6 | mode << 9;
    assert(!thumbInstruction[op]);
    thumbInstruction[op] = [=] { return thumbInstructionAdjustImmediate(d, n, immediate, mode); };
  }

  for(unsigned d = 0; d < 8; ++d)
  for(unsigned n = 0; n < 8; ++n)
  for(unsigned m = 0; m < 8; ++m)
  for(unsigned mode = 0; mode < 2; ++mode) {
    //uint16_t op = pattern("0001 10?? ???? ????") | d | n << 3 | m << 6 | mode << 9;
    uint16_t op = pattern(0x1800) | d | n << 3 | m << 6 | mode << 9;
    assert(!thumbInstruction[op]);
    thumbInstruction[op] = [=] { return thumbInstructionAdjustRegister(d, n, m, mode); };
  }

  for(unsigned immediate = 0; immediate < 128; ++immediate)
  for(unsigned mode = 0; mode < 2; ++mode) {
    //uint16_t op = pattern("1011 0000 ???? ????") | immediate | mode << 7;
    uint16_t op = pattern(0xb000) | immediate | mode << 7;
    assert(!thumbInstruction[op]);
    thumbInstruction[op] = [=] { return thumbInstructionAdjustStack(immediate, mode); };
  }

  for(unsigned _ = 0; _ < 8; ++_)
  for(unsigned m = 0; m < 16; ++m) {
    //uint16_t op = pattern("0100 0111 0??? ?---") | _ | m << 3;
    uint16_t op = pattern(0x4700) | _ | m << 3;
    assert(!thumbInstruction[op]);
    thumbInstruction[op] = [=] { return thumbInstructionBranchExchange(m); };
  }

  for(unsigned displacement = 0; displacement < 2048; ++displacement) {
    //uint16_t op = pattern("1111 0??? ???? ????") | displacement;
    uint16_t op = pattern(0xf000) | displacement;
    assert(!thumbInstruction[op]);
    thumbInstruction[op] = [=] { return thumbInstructionBranchFarPrefix(displacement); };
  }

  for(unsigned displacement = 0; displacement < 2048; ++displacement) {
    //uint16_t op = pattern("1111 1??? ???? ????") | displacement;
    uint16_t op = pattern(0xf800) | displacement;
    assert(!thumbInstruction[op]);
    thumbInstruction[op] = [=] { return thumbInstructionBranchFarSuffix(displacement); };
  }

  for(unsigned displacement = 0; displacement < 2048; ++displacement) {
    //uint16_t op = pattern("1110 0??? ???? ????") | displacement;
    uint16_t op = pattern(0xe000) | displacement;
    assert(!thumbInstruction[op]);
    thumbInstruction[op] = [=] { return thumbInstructionBranchNear(displacement); };
  }

  for(unsigned displacement = 0; displacement < 256; ++displacement)
  for(unsigned condition = 0; condition < 16; ++condition) {
    if(condition == 15) continue;  //BNV
    //uint16_t op = pattern("1101 ???? ???? ????") | displacement | condition << 8;
    uint16_t op = pattern(0xd000) | displacement | condition << 8;
    assert(!thumbInstruction[op]);
    thumbInstruction[op] = [=] { return thumbInstructionBranchTest(displacement, condition); };
  }

  for(unsigned immediate = 0; immediate < 256; ++immediate)
  for(unsigned d = 0; d < 8; ++d)
  for(unsigned mode = 0; mode < 4; ++mode) {
    //uint16_t op = pattern("001? ???? ???? ????") | immediate | d << 8 | mode << 11;
    uint16_t op = pattern(0x2000) | immediate | d << 8 | mode << 11;
    assert(!thumbInstruction[op]);
    thumbInstruction[op] = [=] { return thumbInstructionImmediate(immediate, d, mode); };
  }

  for(unsigned displacement = 0; displacement < 256; ++displacement)
  for(unsigned d = 0; d < 8; ++d) {
    //uint16_t op = pattern("0100 1??? ???? ????") | displacement | d << 8;
    uint16_t op = pattern(0x4800) | displacement | d << 8;
    assert(!thumbInstruction[op]);
    thumbInstruction[op] = [=] { return thumbInstructionLoadLiteral(displacement, d); };
  }

  for(unsigned d = 0; d < 8; ++d)
  for(unsigned n = 0; n < 8; ++n)
  for(unsigned immediate = 0; immediate < 32; ++immediate)
  for(unsigned mode = 0; mode < 2; ++mode) {
    //uint16_t op = pattern("0111 ???? ???? ????") | d | n << 3 | immediate << 6 | mode << 11;
    uint16_t op = pattern(0x7000) | d | n << 3 | immediate << 6 | mode << 11;
    assert(!thumbInstruction[op]);
    thumbInstruction[op] = [=] { return thumbInstructionMoveByteImmediate(d, n, immediate, mode); };
  }

  for(unsigned d = 0; d < 8; ++d)
  for(unsigned n = 0; n < 8; ++n)
  for(unsigned immediate = 0; immediate < 32; ++immediate)
  for(unsigned mode = 0; mode < 2; ++mode) {
    //uint16_t op = pattern("1000 ???? ???? ????") | d | n << 3 | immediate << 6 | mode << 11;
    uint16_t op = pattern(0x8000) | d | n << 3 | immediate << 6 | mode << 11;
    assert(!thumbInstruction[op]);
    thumbInstruction[op] = [=] { return thumbInstructionMoveHalfImmediate(d, n, immediate, mode); };
  }

  for(unsigned list = 0; list < 256; ++list)
  for(unsigned n = 0; n < 8; ++n)
  for(unsigned mode = 0; mode < 2; ++mode) {
    //uint16_t op = pattern("1100 ???? ???? ????") | list | n << 8 | mode << 11;
    uint16_t op = pattern(0xc000) | list | n << 8 | mode << 11;
    assert(!thumbInstruction[op]);
    thumbInstruction[op] = [=] { return thumbInstructionMoveMultiple(list, n, mode); };
  }

  for(unsigned d = 0; d < 8; ++d)
  for(unsigned n = 0; n < 8; ++n)
  for(unsigned m = 0; m < 8; ++m)
  for(unsigned mode = 0; mode < 8; ++mode) {
    //uint16_t op = pattern("0101 ???? ???? ????") | d | n << 3 | m << 6 | mode << 9;
    uint16_t op = pattern(0x5000) | d | n << 3 | m << 6 | mode << 9;
    assert(!thumbInstruction[op]);
    thumbInstruction[op] = [=] { return thumbInstructionMoveRegisterOffset(d, n, m, mode); };
  }

  for(unsigned immediate = 0; immediate < 256; ++immediate)
  for(unsigned d = 0; d < 8; ++d)
  for(unsigned mode = 0; mode < 2; ++mode) {
    //uint16_t op = pattern("1001 ???? ???? ????") | immediate | d << 8 | mode << 11;
    uint16_t op = pattern(0x9000) | immediate | d << 8 | mode << 11;
    assert(!thumbInstruction[op]);
    thumbInstruction[op] = [=] { return thumbInstructionMoveStack(immediate, d, mode); };
  }

  for(unsigned d = 0; d < 8; ++d)
  for(unsigned n = 0; n < 8; ++n)
  for(unsigned offset = 0; offset < 32; ++offset)
  for(unsigned mode = 0; mode < 2; ++mode) {
    //uint16_t op = pattern("0110 ???? ???? ????") | d | n << 3 | offset << 6 | mode << 11;
    uint16_t op = pattern(0x6000) | d | n << 3 | offset << 6 | mode << 11;
    assert(!thumbInstruction[op]);
    thumbInstruction[op] = [=] { return thumbInstructionMoveWordImmediate(d, n, offset, mode); };
  }

  for(unsigned d = 0; d < 8; ++d)
  for(unsigned m = 0; m < 8; ++m)
  for(unsigned immediate = 0; immediate < 32; ++immediate)
  for(unsigned mode = 0; mode < 4; ++mode) {
    if(mode == 3) continue;
    //uint16_t op = pattern("000? ???? ???? ????") | d | m << 3 | immediate << 6 | mode << 11;
    uint16_t op = pattern(0x0000) | d | m << 3 | immediate << 6 | mode << 11;
    assert(!thumbInstruction[op]);
    thumbInstruction[op] = [=] { return thumbInstructionShiftImmediate(d, m, immediate, mode); };
  }

  for(unsigned immediate = 0; immediate < 256; ++immediate) {
    //uint16_t op = pattern("1101 1111 ???? ????") | immediate;
    uint16_t op = pattern(0xdf00) | immediate;
    assert(!thumbInstruction[op]);
    thumbInstruction[op] = [=] { return thumbInstructionSoftwareInterrupt(immediate); };
  }

  for(unsigned list = 0; list < 256; ++list)
  for(unsigned lrpc = 0; lrpc < 2; ++lrpc)
  for(unsigned mode = 0; mode < 2; ++mode) {
    //uint16_t op = pattern("1011 ?10? ???? ????") | list | lrpc << 8 | mode << 11;
    uint16_t op = pattern(0xb400) | list | lrpc << 8 | mode << 11;
    assert(!thumbInstruction[op]);
    thumbInstruction[op] = [=] { return thumbInstructionStackMultiple(list, lrpc, mode); };
  }

  for(unsigned id = 0; id < 65536; ++id) {
    if(thumbInstruction[id]) continue;
    //uint16_t op = pattern("???? ???? ???? ????") | id;
    uint16_t op = pattern(0x0000) | id;
    assert(!thumbInstruction[op]);
    thumbInstruction[op] = [=] { return thumbInstructionUndefined(); };
  }

  #undef bit1
  #undef bits
  #undef pattern
}

void ARM7TDMI::armALU(uint8_t mode, uint8_t d, uint8_t n, uint32_t rm) {
  uint32_t rn = r(n);

  switch(mode) {
  case  0: r(d) = BIT(rn & rm); break;  //AND
  case  1: r(d) = BIT(rn ^ rm); break;  //EOR
  case  2: r(d) = SUB(rn, rm, 1); break;  //SUB
  case  3: r(d) = SUB(rm, rn, 1); break;  //RSB
  case  4: r(d) = ADD(rn, rm, 0); break;  //ADD
  case  5: r(d) = ADD(rn, rm, cpsr().c); break;  //ADC
  case  6: r(d) = SUB(rn, rm, cpsr().c); break;  //SBC
  case  7: r(d) = SUB(rm, rn, cpsr().c); break;  //RSC
  case  8:        BIT(rn & rm); break;  //TST
  case  9:        BIT(rn ^ rm); break;  //TEQ
  case 10:        SUB(rn, rm, 1); break;  //CMP
  case 11:        ADD(rn, rm, 0); break;  //CMN
  case 12: r(d) = BIT(rn | rm); break;  //ORR
  case 13: r(d) = BIT(rm); break;  //MOV
  case 14: r(d) = BIT(rn & ~rm); break;  //BIC
  case 15: r(d) = BIT(~rm); break;  //MVN
  }

  if(exception() && d == 15 && (opcode & 1 << 20)) {
    cpsr() = spsr();
  }
}

void ARM7TDMI::armMoveToStatus(uint8_t field, uint8_t mode, uint32_t data) {
  if(mode && (cpsr().m == PSR::USR || cpsr().m == PSR::SYS)) return;
  PSR& psr = mode ? spsr() : cpsr();

  if(field & 1) {
    if(mode || privileged()) {
      psr.m = data >> 0 & 31;
      psr.t = data >> 5 & 1;
      psr.f = data >> 6 & 1;
      psr.i = data >> 7 & 1;
      if(!mode && psr.t) r(15).data += 2;
    }
  }

  if(field & 8) {
    psr.v = data >> 28 & 1;
    psr.c = data >> 29 & 1;
    psr.z = data >> 30 & 1;
    psr.n = data >> 31 & 1;
  }
}

void ARM7TDMI::armInstructionBranch
(uint32_t displacement, uint8_t link) {
  if(link) r(14) = r(15) - 4;
  r(15) = r(15) + signextend<int32_t,24>(displacement) * 4;
}

void ARM7TDMI::armInstructionBranchExchangeRegister(uint8_t m) {
  uint32_t address = r(m);
  cpsr().t = address & 1;
  r(15) = address;
}

void ARM7TDMI::armInstructionDataImmediate
(uint8_t immediate, uint8_t shift, uint8_t d, uint8_t n, uint8_t save, uint8_t mode) {
  if (save) {}
  uint32_t data = immediate;
  carry = cpsr().c;
  if(shift) data = ROR(data, shift << 1);
  armALU(mode, d, n, data);
}

void ARM7TDMI::armInstructionDataImmediateShift
(uint8_t m, uint8_t type, uint8_t shift, uint8_t d, uint8_t n, uint8_t save, uint8_t mode) {
  if (save) {}
  uint32_t rm = r(m);
  carry = cpsr().c;

  switch(type) {
  case 0: rm = LSL(rm, shift); break;
  case 1: rm = LSR(rm, shift ? (unsigned)shift : 32); break;
  case 2: rm = ASR(rm, shift ? (unsigned)shift : 32); break;
  case 3: rm = shift ? ROR(rm, shift) : RRX(rm); break;
  }

  armALU(mode, d, n, rm);
}

void ARM7TDMI::armInstructionDataRegisterShift
(uint8_t m, uint8_t type, uint8_t s, uint8_t d, uint8_t n, uint8_t save, uint8_t mode) {
  if (save) {}
  uint8_t rs = r(s) + (s == 15 ? 4 : 0);
  uint32_t rm = r(m) + (m == 15 ? 4 : 0);
  carry = cpsr().c;

  switch(type) {
  case 0: rm = LSL(rm, rs < 33 ? rs : (uint8_t)33); break;
  case 1: rm = LSR(rm, rs < 33 ? rs : (uint8_t)33); break;
  case 2: rm = ASR(rm, rs < 32 ? rs : (uint8_t)32); break;
  case 3: if(rs) rm = ROR(rm, rs & 31 ? unsigned(rs & 31) : 32); break;
  }

  armALU(mode, d, n, rm);
}

void ARM7TDMI::armInstructionLoadImmediate
(uint8_t immediate, uint8_t half, uint8_t d, uint8_t n, uint8_t writeback, uint8_t up, uint8_t pre) {
  uint32_t rn = r(n);
  uint32_t rd = r(d);

  if(pre == 1) rn = up ? rn + immediate : rn - immediate;
  rd = load((half ? Half : Byte) | Nonsequential | Signed, rn);
  if(pre == 0) rn = up ? rn + immediate : rn - immediate;

  if(pre == 0 || writeback) r(n) = rn;
  r(d) = rd;
}

void ARM7TDMI::armInstructionLoadRegister
(uint8_t m, uint8_t half, uint8_t d, uint8_t n, uint8_t writeback, uint8_t up, uint8_t pre) {
  uint32_t rn = r(n);
  uint32_t rm = r(m);
  uint32_t rd = r(d);

  if(pre == 1) rn = up ? rn + rm : rn - rm;
  rd = load((half ? Half : Byte) | Nonsequential | Signed, rn);
  if(pre == 0) rn = up ? rn + rm : rn - rm;

  if(pre == 0 || writeback) r(n) = rn;
  r(d) = rd;
}

void ARM7TDMI::armInstructionMemorySwap
(uint8_t m, uint8_t d, uint8_t n, uint8_t byte) {
  uint32_t word = load((byte ? Byte : Word) | Nonsequential, r(n));
  store((byte ? Byte : Word) | Nonsequential, r(n), r(m));
  r(d) = word;
}

void ARM7TDMI::armInstructionMoveHalfImmediate
(uint8_t immediate, uint8_t d, uint8_t n, uint8_t mode, uint8_t writeback, uint8_t up, uint8_t pre) {
  uint32_t rn = r(n);
  uint32_t rd = r(d);

  if(pre == 1) rn = up ? rn + immediate : rn - immediate;
  if(mode == 1) rd = load(Half | Nonsequential, rn);
  if(mode == 0) store(Half | Nonsequential, rn, rd);
  if(pre == 0) rn = up ? rn + immediate : rn - immediate;

  if(pre == 0 || writeback) r(n) = rn;
  if(mode == 1) r(d) = rd;
}

void ARM7TDMI::armInstructionMoveHalfRegister
(uint8_t m, uint8_t d, uint8_t n, uint8_t mode, uint8_t writeback, uint8_t up, uint8_t pre) {
  uint32_t rn = r(n);
  uint32_t rm = r(m);
  uint32_t rd = r(d);

  if(pre == 1) rn = up ? rn + rm : rn - rm;
  if(mode == 1) rd = load(Half | Nonsequential, rn);
  if(mode == 0) store(Half | Nonsequential, rn, rd);
  if(pre == 0) rn = up ? rn + rm : rn - rm;

  if(pre == 0 || writeback) r(n) = rn;
  if(mode == 1) r(d) = rd;
}

void ARM7TDMI::armInstructionMoveImmediateOffset
(uint16_t immediate, uint8_t d, uint8_t n, uint8_t mode, uint8_t writeback, uint8_t byte, uint8_t up, uint8_t pre) {
  uint32_t rn = r(n);
  uint32_t rd = r(d);

  if(pre == 1) rn = up ? rn + immediate : rn - immediate;
  if(mode == 1) rd = load((byte ? Byte : Word) | Nonsequential, rn);
  if(mode == 0) store((byte ? Byte : Word) | Nonsequential, rn, rd);
  if(pre == 0) rn = up ? rn + immediate : rn - immediate;

  if(pre == 0 || writeback) r(n) = rn;
  if(mode == 1) r(d) = rd;
}

void ARM7TDMI::armInstructionMoveMultiple
(uint16_t list, uint8_t n, uint8_t mode, uint8_t writeback, uint8_t type, uint8_t up, uint8_t pre) {
  uint32_t rn = r(n);
  size_t bcount = std::bitset<16>(list).count();
  if(pre == 0 && up == 1) rn = rn + 0;  //IA
  if(pre == 1 && up == 1) rn = rn + 4;  //IB
  if(pre == 1 && up == 0) rn = rn - bcount * 4 + 0;  //DB
  if(pre == 0 && up == 0) rn = rn - bcount * 4 + 4;  //DA

  if(writeback && mode == 1) {
    if(up == 1) r(n) = r(n) + bcount * 4;  //IA,IB
    if(up == 0) r(n) = r(n) - bcount * 4;  //DA,DB
  }

  uint8_t cpsrMode = cpsr().m;
  bool usr = false;
  if(type && mode == 1 && !(list & 0x8000)) usr = true;
  if(type && mode == 0) usr = true;
  if(usr) cpsr().m = PSR::USR;

  unsigned sequential = Nonsequential;
  for(unsigned m = 0; m < 16; ++m) {
    if(!(list & 1 << m)) continue;
    if(mode == 1) r(m) = read(Word | sequential, rn);
    if(mode == 0) write(Word | sequential, rn, r(m));
    rn += 4;
    sequential = Sequential;
  }

  if(usr) cpsr().m = cpsrMode;

  if(mode) {
    idle();
    if(type && (list & 0x8000) && cpsr().m != PSR::USR && cpsr().m != PSR::SYS) {
      cpsr() = spsr();
    }
  } else {
    pipeline.nonsequential = true;
  }

  if(writeback && mode == 0) {
    if(up == 1) r(n) = r(n) + bcount * 4;  //IA,IB
    if(up == 0) r(n) = r(n) - bcount * 4;  //DA,DB
  }
}

void ARM7TDMI::armInstructionMoveRegisterOffset
(uint8_t m, uint8_t type, uint8_t shift, uint8_t d, uint8_t n, uint8_t mode, uint8_t writeback, uint8_t byte, uint8_t up, uint8_t pre) {
  uint32_t rm = r(m);
  uint32_t rd = r(d);
  uint32_t rn = r(n);
  carry = cpsr().c;

  switch(type) {
  case 0: rm = LSL(rm, shift); break;
  case 1: rm = LSR(rm, shift ? (unsigned)shift : 32); break;
  case 2: rm = ASR(rm, shift ? (unsigned)shift : 32); break;
  case 3: rm = shift ? ROR(rm, shift) : RRX(rm); break;
  }

  if(pre == 1) rn = up ? rn + rm : rn - rm;
  if(mode == 1) rd = load((byte ? Byte : Word) | Nonsequential, rn);
  if(mode == 0) store((byte ? Byte : Word) | Nonsequential, rn, rd);
  if(pre == 0) rn = up ? rn + rm : rn - rm;

  if(pre == 0 || writeback) r(n) = rn;
  if(mode == 1) r(d) = rd;
}

void ARM7TDMI::armInstructionMoveToRegisterFromStatus
(uint8_t d, uint8_t mode) {
  if(mode && (cpsr().m == PSR::USR || cpsr().m == PSR::SYS)) return;
  r(d) = mode ? spsr() : cpsr();
}

void ARM7TDMI::armInstructionMoveToStatusFromImmediate
(uint8_t immediate, uint8_t rotate, uint8_t field, uint8_t mode) {
  uint32_t data = immediate;
  if(rotate) data = ROR(data, rotate << 1);
  armMoveToStatus(field, mode, data);
}

void ARM7TDMI::armInstructionMoveToStatusFromRegister
(uint8_t m, uint8_t field, uint8_t mode) {
  armMoveToStatus(field, mode, r(m));
}

void ARM7TDMI::armInstructionMultiply
(uint8_t m, uint8_t s, uint8_t n, uint8_t d, uint8_t save, uint8_t accumulate) {
  if (save) {}
  if(accumulate) idle();
  r(d) = MUL(accumulate ? r(n) : 0, r(m), r(s));
}

void ARM7TDMI::armInstructionMultiplyLong
(uint8_t m, uint8_t s, uint8_t l, uint8_t h, uint8_t save, uint8_t accumulate, uint8_t sign) {
  uint64_t rm = r(m);
  uint64_t rs = r(s);

  idle();
  idle();
  if(accumulate) idle();

  if(sign) {
    if(rs >>  8 && rs >>  8 != 0xffffff) idle();
    if(rs >> 16 && rs >> 16 !=   0xffff) idle();
    if(rs >> 24 && rs >> 24 !=     0xff) idle();
    rm = (int32_t)rm;
    rs = (int32_t)rs;
  } else {
    if(rs >>  8) idle();
    if(rs >> 16) idle();
    if(rs >> 24) idle();
  }

  uint64_t rd = rm * rs;
  if(accumulate) rd += (uint64_t)r(h) << 32 | (uint64_t)r(l);

  r(h) = rd >> 32;
  r(l) = rd >>  0;

  if(save) {
    cpsr().z = rd == 0;
    cpsr().n = rd >> 63 & 1;
  }
}

void ARM7TDMI::armInstructionSoftwareInterrupt(uint32_t immediate) {
  if (immediate) {}
  exception(PSR::SVC, 0x08);
}

void ARM7TDMI::armInstructionUndefined() {
  exception(PSR::UND, 0x04);
}

void ARM7TDMI::thumbInstructionALU
(uint8_t d, uint8_t m, uint8_t mode) {
  switch(mode) {
  case  0: r(d) = BIT(r(d) & r(m)); break;  //AND
  case  1: r(d) = BIT(r(d) ^ r(m)); break;  //EOR
  case  2: r(d) = BIT(LSL(r(d), r(m))); break;  //LSL
  case  3: r(d) = BIT(LSR(r(d), r(m))); break;  //LSR
  case  4: r(d) = BIT(ASR(r(d), r(m))); break;  //ASR
  case  5: r(d) = ADD(r(d), r(m), cpsr().c); break;  //ADC
  case  6: r(d) = SUB(r(d), r(m), cpsr().c); break;  //SBC
  case  7: r(d) = BIT(ROR(r(d), r(m))); break;  //ROR
  case  8:        BIT(r(d) & r(m)); break;  //TST
  case  9: r(d) = SUB(0, r(m), 1); break;  //NEG
  case 10:        SUB(r(d), r(m), 1); break;  //CMP
  case 11:        ADD(r(d), r(m), 0); break;  //CMN
  case 12: r(d) = BIT(r(d) | r(m)); break;  //ORR
  case 13: r(d) = MUL(0, r(m), r(d)); break;  //MUL
  case 14: r(d) = BIT(r(d) & ~r(m)); break;  //BIC
  case 15: r(d) = BIT(~r(m)); break;  //MVN
  }
}

void ARM7TDMI::thumbInstructionALUExtended
(uint8_t d, uint8_t m, uint8_t mode) {
  switch(mode) {
  case 0: r(d) = r(d) + r(m); break;  //ADD
  case 1: SUB(r(d), r(m), 1); break;  //SUBS
  case 2: r(d) = r(m); break;  //MOV
  }
}

void ARM7TDMI::thumbInstructionAddRegister
(uint8_t immediate, uint8_t d, uint8_t mode) {
  switch(mode) {
  case 0: r(d) = (r(15) & ~3) + immediate * 4; break;  //ADD pc
  case 1: r(d) = r(13) + immediate * 4; break;  //ADD sp
  }
}

void ARM7TDMI::thumbInstructionAdjustImmediate
(uint8_t d, uint8_t n, uint8_t immediate, uint8_t mode) {
  switch(mode) {
  case 0: r(d) = ADD(r(n), immediate, 0); break;  //ADD
  case 1: r(d) = SUB(r(n), immediate, 1); break;  //SUB
  }
}

void ARM7TDMI::thumbInstructionAdjustRegister
(uint8_t d, uint8_t n, uint8_t m, uint8_t mode) {
  switch(mode) {
  case 0: r(d) = ADD(r(n), r(m), 0); break;  //ADD
  case 1: r(d) = SUB(r(n), r(m), 1); break;  //SUB
  }
}

void ARM7TDMI::thumbInstructionAdjustStack
(uint8_t immediate, uint8_t mode) {
  switch(mode) {
  case 0: r(13) = r(13) + immediate * 4; break;  //ADD
  case 1: r(13) = r(13) - immediate * 4; break;  //SUB
  }
}

void ARM7TDMI::thumbInstructionBranchExchange(uint8_t m) {
  uint32_t address = r(m);
  cpsr().t = address & 1;
  r(15) = address;
}

void ARM7TDMI::thumbInstructionBranchFarPrefix
(uint16_t displacement) {
  r(14) = r(15) + (signextend<int16_t,11>(displacement) * 2 << 11);
}

void ARM7TDMI::thumbInstructionBranchFarSuffix
(uint16_t displacement) {
  r(15) = r(14) + (displacement * 2);
  r(14) = pipeline.decode.address | 1;
}

void ARM7TDMI::thumbInstructionBranchNear
(uint16_t displacement) {
  r(15) = r(15) + signextend<int16_t,11>(displacement) * 2;
}

void ARM7TDMI::thumbInstructionBranchTest
(int8_t displacement, uint8_t condition) {
  if(!TST(condition)) return;
  r(15) = r(15) + displacement * 2;
}

void ARM7TDMI::thumbInstructionImmediate
(uint8_t immediate, uint8_t d, uint8_t mode) {
  switch(mode) {
  case 0: r(d) = BIT(immediate); break;  //MOV
  case 1:        SUB(r(d), immediate, 1); break;  //CMP
  case 2: r(d) = ADD(r(d), immediate, 0); break;  //ADD
  case 3: r(d) = SUB(r(d), immediate, 1); break;  //SUB
  }
}

void ARM7TDMI::thumbInstructionLoadLiteral
(uint8_t displacement, uint8_t d) {
  uint32_t address = (r(15) & ~3) + (displacement << 2);
  r(d) = load(Word | Nonsequential, address);
}

void ARM7TDMI::thumbInstructionMoveByteImmediate
(uint8_t d, uint8_t n, uint8_t offset, uint8_t mode) {
  switch(mode) {
  case 0: store(Byte | Nonsequential, r(n) + offset, r(d)); break;  //STRB
  case 1: r(d) = load(Byte | Nonsequential, r(n) + offset); break;  //LDRB
  }
}

void ARM7TDMI::thumbInstructionMoveHalfImmediate
(uint8_t d, uint8_t n, uint8_t offset, uint8_t mode) {
  switch(mode) {
  case 0: store(Half | Nonsequential, r(n) + offset * 2, r(d)); break;  //STRH
  case 1: r(d) = load(Half | Nonsequential, r(n) + offset * 2); break;  //LDRH
  }
}

void ARM7TDMI::thumbInstructionMoveMultiple
(uint8_t list, uint8_t n, uint8_t mode) {
  uint32_t rn = r(n);

  for(unsigned m = 0; m < 8; ++m) {
    if(!(list & 1 << m)) continue;
    switch(mode) {
    case 0: write(Word | Nonsequential, rn, r(m)); break;  //STMIA
    case 1: r(m) = read(Word | Nonsequential, rn); break;  //LDMIA
    }
    rn += 4;
  }

  if(mode == 0 || !(list & 1 << n)) r(n) = rn;
  if(mode == 1) idle();
}

void ARM7TDMI::thumbInstructionMoveRegisterOffset
(uint8_t d, uint8_t n, uint8_t m, uint8_t mode) {
  switch(mode) {
  case 0: store(Word | Nonsequential, r(n) + r(m), r(d)); break;  //STR
  case 1: store(Half | Nonsequential, r(n) + r(m), r(d)); break;  //STRH
  case 2: store(Byte | Nonsequential, r(n) + r(m), r(d)); break;  //STRB
  case 3: r(d) = load(Byte | Nonsequential | Signed, r(n) + r(m)); break;  //LDSB
  case 4: r(d) = load(Word | Nonsequential, r(n) + r(m)); break;  //LDR
  case 5: r(d) = load(Half | Nonsequential, r(n) + r(m)); break;  //LDRH
  case 6: r(d) = load(Byte | Nonsequential, r(n) + r(m)); break;  //LDRB
  case 7: r(d) = load(Half | Nonsequential | Signed, r(n) + r(m)); break;  //LDSH
  }
}

void ARM7TDMI::thumbInstructionMoveStack
(uint8_t immediate, uint8_t d, uint8_t mode) {
  switch(mode) {
  case 0: store(Word | Nonsequential, r(13) + immediate * 4, r(d)); break;  //STR
  case 1: r(d) = load(Word | Nonsequential, r(13) + immediate * 4); break;  //LDR
  }
}

void ARM7TDMI::thumbInstructionMoveWordImmediate
(uint8_t d, uint8_t n, uint8_t offset, uint8_t mode) {
  switch(mode) {
  case 0: store(Word | Nonsequential, r(n) + offset * 4, r(d)); break;  //STR
  case 1: r(d) = load(Word | Nonsequential, r(n) + offset * 4); break;  //LDR
  }
}

void ARM7TDMI::thumbInstructionShiftImmediate
(uint8_t d, uint8_t m, uint8_t immediate, uint8_t mode) {
  switch(mode) {
  case 0: r(d) = BIT(LSL(r(m), immediate)); break;  //LSL
  case 1: r(d) = BIT(LSR(r(m), immediate ? (unsigned)immediate : 32)); break;  //LSR
  case 2: r(d) = BIT(ASR(r(m), immediate ? (unsigned)immediate : 32)); break;  //ASR
  }
}

void ARM7TDMI::thumbInstructionSoftwareInterrupt(uint8_t immediate) {
  if (immediate) {}
  exception(PSR::SVC, 0x08);
}

void ARM7TDMI::thumbInstructionStackMultiple
(uint8_t list, uint8_t lrpc, uint8_t mode) {
  uint32_t sp = 0;
  size_t bcount = std::bitset<8>(list).count();
  switch(mode) {
  case 0: sp = r(13) - (bcount + lrpc) * 4; break;  //PUSH
  case 1: sp = r(13);  //POP
  }

  unsigned sequential = Nonsequential;
  for(unsigned m = 0; m < 8; ++m) {
    if(!(list & 1 << m)) continue;
    switch(mode) {
    case 0: write(Word | sequential, sp, r(m)); break;  //PUSH
    case 1: r(m) = read(Word | sequential, sp); break;  //POP
    }
    sp += 4;
    sequential = Sequential;
  }

  if(lrpc) {
    switch(mode) {
    case 0: write(Word | sequential, sp, r(14)); break;  //PUSH
    case 1: r(15) = read(Word | sequential, sp); break;  //POP
    }
    sp += 4;
  }

  if(mode == 1) {
    idle();
    r(13) = r(13) + (bcount + lrpc) * 4;  //POP
  } else {
    pipeline.nonsequential = true;
    r(13) = r(13) - (bcount + lrpc) * 4;  //PUSH
  }
}

void ARM7TDMI::thumbInstructionUndefined() {
  exception(PSR::UND, 0x04);
}

void ARM7TDMI::serialize(serializer& s) {
  processor.serialize(s);
  pipeline.serialize(s);
  s.boolean(carry);
  s.boolean(irq);
}

void ARM7TDMI::Processor::serialize(serializer& s) {
  s.integer(r0.data);
  s.integer(r1.data);
  s.integer(r2.data);
  s.integer(r3.data);
  s.integer(r4.data);
  s.integer(r5.data);
  s.integer(r6.data);
  s.integer(r7.data);
  s.integer(r8.data);
  s.integer(r9.data);
  s.integer(r10.data);
  s.integer(r11.data);
  s.integer(r12.data);
  s.integer(r13.data);
  s.integer(r14.data);
  s.integer(r15.data);
  cpsr.serialize(s);
  s.integer(fiq.r8.data);
  s.integer(fiq.r9.data);
  s.integer(fiq.r10.data);
  s.integer(fiq.r11.data);
  s.integer(fiq.r12.data);
  s.integer(fiq.r13.data);
  s.integer(fiq.r14.data);
  fiq.spsr.serialize(s);
  s.integer(irq.r13.data);
  s.integer(irq.r14.data);
  irq.spsr.serialize(s);
  s.integer(svc.r13.data);
  s.integer(svc.r14.data);
  svc.spsr.serialize(s);
  s.integer(abt.r13.data);
  s.integer(abt.r14.data);
  abt.spsr.serialize(s);
  s.integer(und.r13.data);
  s.integer(und.r14.data);
  und.spsr.serialize(s);
}

ARM7TDMI::PSR::operator uint32_t() const {
  return m << 0 | t << 5 | f << 6 | i << 7 | v << 28 | c << 29 | z << 30 | n << 31;
}

ARM7TDMI::PSR& ARM7TDMI::PSR::operator=(uint32_t data) {
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

void ARM7TDMI::PSR::serialize(serializer& s) {
  s.integer(m);
  s.boolean(t);
  s.boolean(f);
  s.boolean(i);
  s.boolean(v);
  s.boolean(c);
  s.boolean(z);
  s.boolean(n);
}

void ARM7TDMI::Pipeline::serialize(serializer& s) {
  s.integer(reload);
  s.integer(nonsequential);
  s.integer(fetch.address);
  s.integer(fetch.instruction);
  s.boolean(fetch.thumb);
  s.integer(decode.address);
  s.integer(decode.instruction);
  s.boolean(decode.thumb);
  s.integer(execute.address);
  s.integer(execute.instruction);
  s.boolean(execute.thumb);
}

#undef _s
#undef _move
#undef _comp
#undef _save

ARM7TDMI::ARM7TDMI() {
  armInitialize();
  thumbInitialize();
}

void ARM7TDMI::power() {
  processor = {};
  processor.r15.modify = [&] { pipeline.reload = true; };
  pipeline = {};
  carry = 0;
  irq = 0;
  cpsr().f = 1;
  exception(PSR::SVC, 0x00);
}

}
