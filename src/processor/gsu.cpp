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

#include <cstring>
#include <string>

#include "serializer.hpp"

#include "gsu.hpp"

//note: multiplication results *may* sometimes be invalid when both CLSR and MS0 are set
//the product of multiplication in this mode (21mhz + fast-multiply) has not been analyzed;
//however, the timing of this mode has been confirmed to work as specified below

namespace Processor {

void GSU::instruction(uint8_t opcode) {
  #define op4(id, name) \
    case id+ 0: return instruction##name(opcode & 0x0f); \
    case id+ 1: return instruction##name(opcode & 0x0f); \
    case id+ 2: return instruction##name(opcode & 0x0f); \
    case id+ 3: return instruction##name(opcode & 0x0f); \

  #define op6(id, name) \
    op4(id, name) \
    case id+ 4: return instruction##name(opcode & 0x0f); \
    case id+ 5: return instruction##name(opcode & 0x0f); \

  #define op12(id, name) \
    op6(id, name) \
    case id+ 6: return instruction##name(opcode & 0x0f); \
    case id+ 7: return instruction##name(opcode & 0x0f); \
    case id+ 8: return instruction##name(opcode & 0x0f); \
    case id+ 9: return instruction##name(opcode & 0x0f); \
    case id+10: return instruction##name(opcode & 0x0f); \
    case id+11: return instruction##name(opcode & 0x0f); \

  #define op15(id, name) \
    op12(id, name) \
    case id+12: return instruction##name(opcode & 0x0f); \
    case id+13: return instruction##name(opcode & 0x0f); \
    case id+14: return instruction##name(opcode & 0x0f); \

  #define op16(id, name) \
    op15(id, name) \
    case id+15: return instruction##name(opcode & 0x0f); \

  switch(opcode) {
    case 0x00: return instructionSTOP();
    case 0x01: return instructionNOP();
    case 0x02: return instructionCACHE();
    case 0x03: return instructionLSR();
    case 0x04: return instructionROL();
    case 0x05: return instructionBranch(1);  //bra
    case 0x06: return instructionBranch((regs.sfr.flag.s ^ regs.sfr.flag.ov) == 0);  //blt
    case 0x07: return instructionBranch((regs.sfr.flag.s ^ regs.sfr.flag.ov) == 1);  //bge
    case 0x08: return instructionBranch(regs.sfr.flag.z == 0);  //bne
    case 0x09: return instructionBranch(regs.sfr.flag.z == 1);  //beq
    case 0x0a: return instructionBranch(regs.sfr.flag.s == 0);  //bpl
    case 0x0b: return instructionBranch(regs.sfr.flag.s == 1);  //bmi
    case 0x0c: return instructionBranch(regs.sfr.flag.cy == 0);  //bcc
    case 0x0d: return instructionBranch(regs.sfr.flag.cy == 1);  //bcs
    case 0x0e: return instructionBranch(regs.sfr.flag.ov == 0);  //bvc
    case 0x0f: return instructionBranch(regs.sfr.flag.ov == 1);  //bvs
    op16(0x10, TO_MOVE)
    op16(0x20, WITH)
    op12(0x30, Store)
    case 0x3c: return instructionLOOP();
    case 0x3d: return instructionALT1();
    case 0x3e: return instructionALT2();
    case 0x3f: return instructionALT3();
    op12(0x40, Load)
    case 0x4c: return instructionPLOT_RPIX();
    case 0x4d: return instructionSWAP();
    case 0x4e: return instructionCOLOR_CMODE();
    case 0x4f: return instructionNOT();
    op16(0x50, ADD_ADC)
    op16(0x60, SUB_SBC_CMP)
    case 0x70: return instructionMERGE();
    op15(0x71, AND_BIC)
    op16(0x80, MULT_UMULT)
    case 0x90: return instructionSBK();
    op4 (0x91, LINK)
    case 0x95: return instructionSEX();
    case 0x96: return instructionASR_DIV2();
    case 0x97: return instructionROR();
    op6 (0x98, JMP_LJMP)
    case 0x9e: return instructionLOB();
    case 0x9f: return instructionFMULT_LMULT();
    op16(0xa0, IBT_LMS_SMS)
    op16(0xb0, FROM_MOVES)
    case 0xc0: return instructionHIB();
    op15(0xc1, OR_XOR)
    op15(0xd0, INC)
    case 0xdf: return instructionGETC_RAMB_ROMB();
    op15(0xe0, DEC)
    case 0xef: return instructionGETB();
    op16(0xf0, IWT_LM_SM)
  }

  #undef op4
  #undef op6
  #undef op12
  #undef op15
  #undef op16
}

//$00 stop
void GSU::instructionSTOP() {
  if(regs.cfgr.irq == 0) {
    regs.sfr.flag.irq = 1;
    stop();
  }
  regs.sfr.flag.g = 0;
  regs.pipeline = 0x01;  //nop
  regs.reset();
}

//$01 nop
void GSU::instructionNOP() {
  regs.reset();
}

//$02 cache
void GSU::instructionCACHE() {
  if(regs.cbr != (regs.r[15] & 0xfff0)) {
    regs.cbr = regs.r[15] & 0xfff0;
    flushCache();
  }
  regs.reset();
}

//$03 lsr
void GSU::instructionLSR() {
  regs.sfr.flag.cy = (bool)(regs.sr() & 1);
  regs.dr() = regs.sr() >> 1;
  regs.sfr.flag.s = (bool)(regs.dr() & 0x8000);
  regs.sfr.flag.z = (bool)(regs.dr() == 0);
  regs.reset();
}

//$04 rol
void GSU::instructionROL() {
  bool carry = (regs.sr() & 0x8000);
  regs.dr() = (regs.sr() << 1) | regs.sfr.flag.cy;
  regs.sfr.flag.s  = (bool)(regs.dr() & 0x8000);
  regs.sfr.flag.cy = carry;
  regs.sfr.flag.z  = (bool)(regs.dr() == 0);
  regs.reset();
}

//$05 bra e
//$06 blt e
//$07 bge e
//$08 bne e
//$09 beq e
//$0a bpl e
//$0b bmi e
//$0c bcc e
//$0d bcs e
//$0e bvc e
//$0f bvs e
void GSU::instructionBranch(bool take) {
  int8_t displacement = (int8_t)pipe();
  if(take) regs.r[15] += displacement;
}

//$10-1f(b0) to rN
//$10-1f(b1) move rN
void GSU::instructionTO_MOVE(unsigned n) {
  if(!regs.sfr.flag.b) {
    regs.dreg = n;
  } else {
    regs.r[n] = regs.sr();
    regs.reset();
  }
}

//$20-2f with rN
void GSU::instructionWITH(unsigned n) {
  regs.sreg = n;
  regs.dreg = n;
  regs.sfr.flag.b = 1;
}

//$30-3b(alt0) stw (rN)
//$30-3b(alt1) stb (rN)
void GSU::instructionStore(unsigned n) {
  regs.ramaddr = regs.r[n];
  writeRAMBuffer(regs.ramaddr, regs.sr());
  if(!regs.sfr.flag.alt1) writeRAMBuffer(regs.ramaddr ^ 1, regs.sr() >> 8);
  regs.reset();
}

//$3c loop
void GSU::instructionLOOP() {
  regs.r[12]--;
  regs.sfr.flag.s = (bool)(regs.r[12] & 0x8000);
  regs.sfr.flag.z = (bool)(regs.r[12] == 0);
  if(!regs.sfr.flag.z) regs.r[15] = regs.r[13];
  regs.reset();
}

//$3d alt1
void GSU::instructionALT1() {
  regs.sfr.flag.b = 0;
  regs.sfr.flag.alt1 = 1;
}

//$3e alt2
void GSU::instructionALT2() {
  regs.sfr.flag.b = 0;
  regs.sfr.flag.alt2 = 1;
}

//$3f alt3
void GSU::instructionALT3() {
  regs.sfr.flag.b = 0;
  regs.sfr.flag.alt1 = 1;
  regs.sfr.flag.alt2 = 1;
}

//$40-4b(alt0) ldw (rN)
//$40-4b(alt1) ldb (rN)
void GSU::instructionLoad(unsigned n) {
  regs.ramaddr = regs.r[n];
  regs.dr() = readRAMBuffer(regs.ramaddr);
  if(!regs.sfr.flag.alt1) regs.dr() |= readRAMBuffer(regs.ramaddr ^ 1) << 8;
  regs.reset();
}

//$4c(alt0) plot
//$4c(alt1) rpix
void GSU::instructionPLOT_RPIX() {
  if(!regs.sfr.flag.alt1) {
    plot(regs.r[1], regs.r[2]);
    regs.r[1]++;
  } else {
    regs.dr() = rpix(regs.r[1], regs.r[2]);
    regs.sfr.flag.s = (bool)(regs.dr() & 0x8000);
    regs.sfr.flag.z = (bool)(regs.dr() == 0);
  }
  regs.reset();
}

//$4d swap
void GSU::instructionSWAP() {
  regs.dr() = regs.sr() >> 8 | regs.sr() << 8;
  regs.sfr.flag.s = (bool)(regs.dr() & 0x8000);
  regs.sfr.flag.z = (bool)(regs.dr() == 0);
  regs.reset();
}

//$4e(alt0) color
//$4e(alt1) cmode
void GSU::instructionCOLOR_CMODE() {
  if(!regs.sfr.flag.alt1) {
    regs.colr = color(regs.sr());
  } else {
    regs.por = regs.sr();
  }
  regs.reset();
}

//$4f not
void GSU::instructionNOT() {
  regs.dr() = ~regs.sr();
  regs.sfr.flag.s = (bool)(regs.dr() & 0x8000);
  regs.sfr.flag.z = (bool)(regs.dr() == 0);
  regs.reset();
}

//$50-5f(alt0) add rN
//$50-5f(alt1) adc rN
//$50-5f(alt2) add #N
//$50-5f(alt3) adc #N
void GSU::instructionADD_ADC(unsigned n) {
  if(!regs.sfr.flag.alt2) n = regs.r[n];
  int r = regs.sr() + n + (regs.sfr.flag.alt1 ? regs.sfr.flag.cy : 0);
  regs.sfr.flag.ov = (bool)(~(regs.sr() ^ n) & (n ^ r) & 0x8000);
  regs.sfr.flag.s  = (bool)(r & 0x8000);
  regs.sfr.flag.cy = (bool)(r >= 0x10000);
  regs.sfr.flag.z  = (bool)((uint16_t)r == 0);
  regs.dr() = r;
  regs.reset();
}

//$60-6f(alt0) sub rN
//$60-6f(alt1) sbc rN
//$60-6f(alt2) sub #N
//$60-6f(alt3) cmp rN
void GSU::instructionSUB_SBC_CMP(unsigned n) {
  if(!regs.sfr.flag.alt2 || regs.sfr.flag.alt1) n = regs.r[n];
  int r = regs.sr() - n - (!regs.sfr.flag.alt2 && regs.sfr.flag.alt1 ? !regs.sfr.flag.cy : 0);
  regs.sfr.flag.ov = (bool)((regs.sr() ^ n) & (regs.sr() ^ r) & 0x8000);
  regs.sfr.flag.s  = (bool)(r & 0x8000);
  regs.sfr.flag.cy = (bool)(r >= 0);
  regs.sfr.flag.z  = (bool)((uint16_t)r == 0);
  if(!regs.sfr.flag.alt2 || !regs.sfr.flag.alt1) regs.dr() = r;
  regs.reset();
}

//$70 merge
void GSU::instructionMERGE() {
  regs.dr() = (regs.r[7] & 0xff00) | (regs.r[8] >> 8);
  regs.sfr.flag.ov = (bool)(regs.dr() & 0xc0c0);
  regs.sfr.flag.s  = (bool)(regs.dr() & 0x8080);
  regs.sfr.flag.cy = (bool)(regs.dr() & 0xe0e0);
  regs.sfr.flag.z  = (bool)(regs.dr() & 0xf0f0);
  regs.reset();
}

//$71-7f(alt0) and rN
//$71-7f(alt1) bic rN
//$71-7f(alt2) and #N
//$71-7f(alt3) bic #N
void GSU::instructionAND_BIC(unsigned n) {
  if(!regs.sfr.flag.alt2) n = regs.r[n];
  regs.dr() = regs.sr() & (regs.sfr.flag.alt1 ? ~n : n);
  regs.sfr.flag.s = (bool)(regs.dr() & 0x8000);
  regs.sfr.flag.z = (bool)(regs.dr() == 0);
  regs.reset();
}

//$80-8f(alt0) mult rN
//$80-8f(alt1) umult rN
//$80-8f(alt2) mult #N
//$80-8f(alt3) umult #N
void GSU::instructionMULT_UMULT(unsigned n) {
  if(!regs.sfr.flag.alt2) n = regs.r[n];
  regs.dr() = (!regs.sfr.flag.alt1 ? uint16_t((int8_t)regs.sr() * (int8_t)n) : uint16_t((uint8_t)regs.sr() * (uint8_t)n));
  regs.sfr.flag.s = (bool)(regs.dr() & 0x8000);
  regs.sfr.flag.z = (bool)(regs.dr() == 0);
  regs.reset();
  if(!regs.cfgr.ms0) step(regs.clsr ? 1 : 2);
}

//$90 sbk
void GSU::instructionSBK() {
  writeRAMBuffer(regs.ramaddr ^ 0, regs.sr() >> 0);
  writeRAMBuffer(regs.ramaddr ^ 1, regs.sr() >> 8);
  regs.reset();
}

//$91-94 link #N
void GSU::instructionLINK(unsigned n) {
  regs.r[11] = regs.r[15] + n;
  regs.reset();
}

//$95 sex
void GSU::instructionSEX() {
  regs.dr() = (int8_t)regs.sr();
  regs.sfr.flag.s = (bool)(regs.dr() & 0x8000);
  regs.sfr.flag.z = (bool)(regs.dr() == 0);
  regs.reset();
}

//$96(alt0) asr
//$96(alt1) div2
void GSU::instructionASR_DIV2() {
  regs.sfr.flag.cy = (bool)(regs.sr() & 1);
  regs.dr() = ((int16_t)regs.sr() >> 1) + (regs.sfr.flag.alt1 ? ((regs.sr() + 1) >> 16) : 0);
  regs.sfr.flag.s = (bool)(regs.dr() & 0x8000);
  regs.sfr.flag.z = (bool)(regs.dr() == 0);
  regs.reset();
}

//$97 ror
void GSU::instructionROR() {
  bool carry = (regs.sr() & 1);
  regs.dr() = (regs.sfr.flag.cy << 15) | (regs.sr() >> 1);
  regs.sfr.flag.s  = (bool)(regs.dr() & 0x8000);
  regs.sfr.flag.cy = carry;
  regs.sfr.flag.z  = (bool)(regs.dr() == 0);
  regs.reset();
}

//$98-9d(alt0) jmp rN
//$98-9d(alt1) ljmp rN
void GSU::instructionJMP_LJMP(unsigned n) {
  if(!regs.sfr.flag.alt1) {
    regs.r[15] = regs.r[n];
  } else {
    regs.pbr = regs.r[n] & 0x7f;
    regs.r[15] = regs.sr();
    regs.cbr = regs.r[15] & 0xfff0;
    flushCache();
  }
  regs.reset();
}

//$9e lob
void GSU::instructionLOB() {
  regs.dr() = regs.sr() & 0xff;
  regs.sfr.flag.s = (bool)(regs.dr() & 0x80);
  regs.sfr.flag.z = (bool)(regs.dr() == 0);
  regs.reset();
}

//$9f(alt0) fmult
//$9f(alt1) lmult
void GSU::instructionFMULT_LMULT() {
  uint32_t result = (int16_t)regs.sr() * (int16_t)regs.r[6];
  if(regs.sfr.flag.alt1) regs.r[4] = result;
  regs.dr() = result >> 16;
  regs.sfr.flag.s  = (bool)(regs.dr() & 0x8000);
  regs.sfr.flag.cy = (bool)(result & 0x8000);
  regs.sfr.flag.z  = (bool)(regs.dr() == 0);
  regs.reset();
  step((regs.cfgr.ms0 ? 3 : 7) * (regs.clsr ? 1 : 2));
}

//$a0-af(alt0) ibt rN,#pp
//$a0-af(alt1) lms rN,(yy)
//$a0-af(alt2) sms (yy),rN
void GSU::instructionIBT_LMS_SMS(unsigned n) {
  if(regs.sfr.flag.alt1) {
    regs.ramaddr = pipe() << 1;
    uint8_t lo  = readRAMBuffer(regs.ramaddr ^ 0) << 0;
    regs.r[n] = readRAMBuffer(regs.ramaddr ^ 1) << 8 | lo;
  } else if(regs.sfr.flag.alt2) {
    regs.ramaddr = pipe() << 1;
    writeRAMBuffer(regs.ramaddr ^ 0, regs.r[n] >> 0);
    writeRAMBuffer(regs.ramaddr ^ 1, regs.r[n] >> 8);
  } else {
    regs.r[n] = (int8_t)pipe();
  }
  regs.reset();
}

//$b0-bf(b0) from rN
//$b0-bf(b1) moves rN
void GSU::instructionFROM_MOVES(unsigned n) {
  if(!regs.sfr.flag.b) {
    regs.sreg = n;
  } else {
    regs.dr() = regs.r[n];
    regs.sfr.flag.ov = (bool)(regs.dr() & 0x80);
    regs.sfr.flag.s  = (bool)(regs.dr() & 0x8000);
    regs.sfr.flag.z  = (bool)(regs.dr() == 0);
    regs.reset();
  }
}

//$c0 hib
void GSU::instructionHIB() {
  regs.dr() = regs.sr() >> 8;
  regs.sfr.flag.s = (bool)(regs.dr() & 0x80);
  regs.sfr.flag.z = (bool)(regs.dr() == 0);
  regs.reset();
}

//$c1-cf(alt0) or rN
//$c1-cf(alt1) xor rN
//$c1-cf(alt2) or #N
//$c1-cf(alt3) xor #N
void GSU::instructionOR_XOR(unsigned n) {
  if(!regs.sfr.flag.alt2) n = regs.r[n];
  regs.dr() = (!regs.sfr.flag.alt1 ? (regs.sr() | n) : (regs.sr() ^ n));
  regs.sfr.flag.s = (bool)(regs.dr() & 0x8000);
  regs.sfr.flag.z = (bool)(regs.dr() == 0);
  regs.reset();
}

//$d0-de inc rN
void GSU::instructionINC(unsigned n) {
  regs.r[n]++;
  regs.sfr.flag.s = (bool)(regs.r[n] & 0x8000);
  regs.sfr.flag.z = (bool)(regs.r[n] == 0);
  regs.reset();
}

//$df(alt0) getc
//$df(alt2) ramb
//$df(alt3) romb
void GSU::instructionGETC_RAMB_ROMB() {
  if(!regs.sfr.flag.alt2) {
    regs.colr = color(readROMBuffer());
  } else if(!regs.sfr.flag.alt1) {
    syncRAMBuffer();
    regs.rambr = regs.sr() & 0x01;
  } else {
    syncROMBuffer();
    regs.rombr = regs.sr() & 0x7f;
  }
  regs.reset();
}

//$e0-ee dec rN
void GSU::instructionDEC(unsigned n) {
  regs.r[n]--;
  regs.sfr.flag.s = (bool)(regs.r[n] & 0x8000);
  regs.sfr.flag.z = (bool)(regs.r[n] == 0);
  regs.reset();
}

//$ef(alt0) getb
//$ef(alt1) getbh
//$ef(alt2) getbl
//$ef(alt3) getbs
void GSU::instructionGETB() {
  switch(regs.sfr.flag.alt2 << 1 | regs.sfr.flag.alt1 << 0) {
  case 0: regs.dr() = readROMBuffer(); break;
  case 1: regs.dr() = readROMBuffer() << 8 | (uint8_t)regs.sr(); break;
  case 2: regs.dr() = (regs.sr() & 0xff00) | readROMBuffer(); break;
  case 3: regs.dr() = (int8_t)readROMBuffer(); break;
  }
  regs.reset();
}

//$f0-ff(alt0) iwt rN,#xx
//$f0-ff(alt1) lm rN,(xx)
//$f0-ff(alt2) sm (xx),rN
void GSU::instructionIWT_LM_SM(unsigned n) {
  if(regs.sfr.flag.alt1) {
    regs.ramaddr  = pipe() << 0;
    regs.ramaddr |= pipe() << 8;
    uint8_t lo  = readRAMBuffer(regs.ramaddr ^ 0) << 0;
    regs.r[n] = readRAMBuffer(regs.ramaddr ^ 1) << 8 | lo;
  } else if(regs.sfr.flag.alt2) {
    regs.ramaddr  = pipe() << 0;
    regs.ramaddr |= pipe() << 8;
    writeRAMBuffer(regs.ramaddr ^ 0, regs.r[n] >> 0);
    writeRAMBuffer(regs.ramaddr ^ 1, regs.r[n] >> 8);
  } else {
    uint8_t lo  = pipe();
    regs.r[n] = pipe() << 8 | lo;
  }
  regs.reset();
}

void GSU::serialize(serializer& s) {
  s.integer(regs.pipeline);
  s.integer(regs.ramaddr);

  for(unsigned n = 0; n < 16; ++n) {
    s.integer(regs.r[n].data);
    s.integer(regs.r[n].modified);
  }

  s.integer(regs.sfr.data);
  s.integer(regs.pbr);
  s.integer(regs.rombr);
  s.integer(regs.rambr);
  s.integer(regs.cbr);
  s.integer(regs.scbr);

  s.integer(regs.scmr.ht);
  s.integer(regs.scmr.ron);
  s.integer(regs.scmr.ran);
  s.integer(regs.scmr.md);

  s.integer(regs.colr);

  s.integer(regs.por.obj);
  s.integer(regs.por.freezehigh);
  s.integer(regs.por.highnibble);
  s.integer(regs.por.dither);
  s.integer(regs.por.transparent);

  s.integer(regs.bramr);
  s.integer(regs.vcr);

  s.integer(regs.cfgr.irq);
  s.integer(regs.cfgr.ms0);

  s.integer(regs.clsr);

  s.integer(regs.romcl);
  s.integer(regs.romdr);

  s.integer(regs.ramcl);
  s.integer(regs.ramar);
  s.integer(regs.ramdr);

  s.integer(regs.sreg);
  s.integer(regs.dreg);

  s.array(cache.buffer);
  s.array(cache.valid);

  for(unsigned n = 0; n < 2; ++n) {
    s.integer(pixelcache[n].offset);
    s.integer(pixelcache[n].bitpend);
    s.array(pixelcache[n].data);
  }
}

void GSU::disassembleOpcode(char* output) {
  *output = 0;

  switch(regs.sfr.flag.alt2 << 1 | regs.sfr.flag.alt1 << 0) {
  case 0: disassembleALT0(output); break;
  case 1: disassembleALT1(output); break;
  case 2: disassembleALT2(output); break;
  case 3: disassembleALT3(output); break;
  }

  unsigned length = strlen(output);
  while(length++ < 20) strcat(output, " ");
}

#define case4(id) \
  case id+ 0: case id+ 1: case id+ 2: case id+ 3
#define case6(id) \
  case id+ 0: case id+ 1: case id+ 2: case id+ 3: case id+ 4: case id+ 5
#define case12(id) \
  case id+ 0: case id+ 1: case id+ 2: case id+ 3: case id+ 4: case id+ 5: case id+ 6: case id+ 7: \
  case id+ 8: case id+ 9: case id+10: case id+11
#define case15(id) \
  case id+ 0: case id+ 1: case id+ 2: case id+ 3: case id+ 4: case id+ 5: case id+ 6: case id+ 7: \
  case id+ 8: case id+ 9: case id+10: case id+11: case id+12: case id+13: case id+14
#define case16(id) \
  case id+ 0: case id+ 1: case id+ 2: case id+ 3: case id+ 4: case id+ 5: case id+ 6: case id+ 7: \
  case id+ 8: case id+ 9: case id+10: case id+11: case id+12: case id+13: case id+14: case id+15

#define op0 regs.pipeline
#define op1 read((regs.pbr << 16) + regs.r[15] + 0)
#define op2 read((regs.pbr << 16) + regs.r[15] + 1)

void GSU::disassembleALT0(char* output) {
  char t[256] = "";
  switch(op0) {
    case  (0x00): sprintf(t, "stop"); break;
    case  (0x01): sprintf(t, "nop"); break;
    case  (0x02): sprintf(t, "cache"); break;
    case  (0x03): sprintf(t, "lsr"); break;
    case  (0x04): sprintf(t, "rol"); break;
    case  (0x05): sprintf(t, "bra %+d", (int8_t)op1); break;
    case  (0x06): sprintf(t, "blt %+d", (int8_t)op1); break;
    case  (0x07): sprintf(t, "bge %+d", (int8_t)op1); break;
    case  (0x08): sprintf(t, "bne %+d", (int8_t)op1); break;
    case  (0x09): sprintf(t, "beq %+d", (int8_t)op1); break;
    case  (0x0a): sprintf(t, "bpl %+d", (int8_t)op1); break;
    case  (0x0b): sprintf(t, "bmi %+d", (int8_t)op1); break;
    case  (0x0c): sprintf(t, "bcc %+d", (int8_t)op1); break;
    case  (0x0d): sprintf(t, "bcs %+d", (int8_t)op1); break;
    case  (0x0e): sprintf(t, "bvc %+d", (int8_t)op1); break;
    case  (0x0f): sprintf(t, "bvs %+d", (int8_t)op1); break;
    case16(0x10): sprintf(t, "to r%u", op0 & 15); break;
    case16(0x20): sprintf(t, "with r%u", op0 & 15); break;
    case12(0x30): sprintf(t, "stw (r%u)", op0 & 15); break;
    case  (0x3c): sprintf(t, "loop"); break;
    case  (0x3d): sprintf(t, "alt1"); break;
    case  (0x3e): sprintf(t, "alt2"); break;
    case  (0x3f): sprintf(t, "alt3"); break;
    case12(0x40): sprintf(t, "ldw (r%u)", op0 & 15); break;
    case  (0x4c): sprintf(t, "plot"); break;
    case  (0x4d): sprintf(t, "swap"); break;
    case  (0x4e): sprintf(t, "color"); break;
    case  (0x4f): sprintf(t, "not"); break;
    case16(0x50): sprintf(t, "add r%u", op0 & 15); break;
    case16(0x60): sprintf(t, "sub r%u", op0 & 15); break;
    case  (0x70): sprintf(t, "merge"); break;
    case15(0x71): sprintf(t, "and r%u", op0 & 15); break;
    case16(0x80): sprintf(t, "mult r%u", op0 & 15); break;
    case  (0x90): sprintf(t, "sbk"); break;
    case4 (0x91): sprintf(t, "link #%u", op0 & 15); break;
    case  (0x95): sprintf(t, "sex"); break;
    case  (0x96): sprintf(t, "asr"); break;
    case  (0x97): sprintf(t, "ror"); break;
    case6 (0x98): sprintf(t, "jmp r%u", op0 & 15); break;
    case  (0x9e): sprintf(t, "lob"); break;
    case  (0x9f): sprintf(t, "fmult"); break;
    case16(0xa0): sprintf(t, "ibt r%u,#$%.2x", op0 & 15, op1); break;
    case16(0xb0): sprintf(t, "from r%u", op0 & 15); break;
    case  (0xc0): sprintf(t, "hib"); break;
    case15(0xc1): sprintf(t, "or r%u", op0 & 15); break;
    case15(0xd0): sprintf(t, "inc r%u", op0 & 15); break;
    case  (0xdf): sprintf(t, "getc"); break;
    case15(0xe0): sprintf(t, "dec r%u", op0 & 15); break;
    case  (0xef): sprintf(t, "getb"); break;
    case16(0xf0): sprintf(t, "iwt r%u,#$%.2x%.2x", op0 & 15, op2, op1); break;
  }
  strcat(output, t);
}

void GSU::disassembleALT1(char* output) {
  char t[256] = "";
  switch(op0) {
    case  (0x00): sprintf(t, "stop"); break;
    case  (0x01): sprintf(t, "nop"); break;
    case  (0x02): sprintf(t, "cache"); break;
    case  (0x03): sprintf(t, "lsr"); break;
    case  (0x04): sprintf(t, "rol"); break;
    case  (0x05): sprintf(t, "bra %+d", (int8_t)op1); break;
    case  (0x06): sprintf(t, "blt %+d", (int8_t)op1); break;
    case  (0x07): sprintf(t, "bge %+d", (int8_t)op1); break;
    case  (0x08): sprintf(t, "bne %+d", (int8_t)op1); break;
    case  (0x09): sprintf(t, "beq %+d", (int8_t)op1); break;
    case  (0x0a): sprintf(t, "bpl %+d", (int8_t)op1); break;
    case  (0x0b): sprintf(t, "bmi %+d", (int8_t)op1); break;
    case  (0x0c): sprintf(t, "bcc %+d", (int8_t)op1); break;
    case  (0x0d): sprintf(t, "bcs %+d", (int8_t)op1); break;
    case  (0x0e): sprintf(t, "bvc %+d", (int8_t)op1); break;
    case  (0x0f): sprintf(t, "bvs %+d", (int8_t)op1); break;
    case16(0x10): sprintf(t, "to r%u", op0 & 15); break;
    case16(0x20): sprintf(t, "with r%u", op0 & 15); break;
    case12(0x30): sprintf(t, "stb (r%u)", op0 & 15); break;
    case  (0x3c): sprintf(t, "loop"); break;
    case  (0x3d): sprintf(t, "alt1"); break;
    case  (0x3e): sprintf(t, "alt2"); break;
    case  (0x3f): sprintf(t, "alt3"); break;
    case12(0x40): sprintf(t, "ldb (r%u)", op0 & 15); break;
    case  (0x4c): sprintf(t, "rpix"); break;
    case  (0x4d): sprintf(t, "swap"); break;
    case  (0x4e): sprintf(t, "cmode"); break;
    case  (0x4f): sprintf(t, "not"); break;
    case16(0x50): sprintf(t, "adc r%u", op0 & 15); break;
    case16(0x60): sprintf(t, "sbc r%u", op0 & 15); break;
    case  (0x70): sprintf(t, "merge"); break;
    case15(0x71): sprintf(t, "bic r%u", op0 & 15); break;
    case16(0x80): sprintf(t, "umult r%u", op0 & 15); break;
    case  (0x90): sprintf(t, "sbk"); break;
    case4 (0x91): sprintf(t, "link #%u", op0 & 15); break;
    case  (0x95): sprintf(t, "sex"); break;
    case  (0x96): sprintf(t, "div2"); break;
    case  (0x97): sprintf(t, "ror"); break;
    case6 (0x98): sprintf(t, "ljmp r%u", op0 & 15); break;
    case  (0x9e): sprintf(t, "lob"); break;
    case  (0x9f): sprintf(t, "lmult"); break;
    case16(0xa0): sprintf(t, "lms r%u,(#$%.4x)", op0 & 15, op1 << 1); break;
    case16(0xb0): sprintf(t, "from r%u", op0 & 15); break;
    case  (0xc0): sprintf(t, "hib"); break;
    case15(0xc1): sprintf(t, "xor r%u", op0 & 15); break;
    case15(0xd0): sprintf(t, "inc r%u", op0 & 15); break;
    case  (0xdf): sprintf(t, "getc"); break;
    case15(0xe0): sprintf(t, "dec r%u", op0 & 15); break;
    case  (0xef): sprintf(t, "getbh"); break;
    case16(0xf0): sprintf(t, "lm r%u", op0 & 15); break;
  }
  strcat(output, t);
}

void GSU::disassembleALT2(char* output) {
  char t[256] = "";
  switch(op0) {
    case  (0x00): sprintf(t, "stop"); break;
    case  (0x01): sprintf(t, "nop"); break;
    case  (0x02): sprintf(t, "cache"); break;
    case  (0x03): sprintf(t, "lsr"); break;
    case  (0x04): sprintf(t, "rol"); break;
    case  (0x05): sprintf(t, "bra %+d", (int8_t)op1); break;
    case  (0x06): sprintf(t, "blt %+d", (int8_t)op1); break;
    case  (0x07): sprintf(t, "bge %+d", (int8_t)op1); break;
    case  (0x08): sprintf(t, "bne %+d", (int8_t)op1); break;
    case  (0x09): sprintf(t, "beq %+d", (int8_t)op1); break;
    case  (0x0a): sprintf(t, "bpl %+d", (int8_t)op1); break;
    case  (0x0b): sprintf(t, "bmi %+d", (int8_t)op1); break;
    case  (0x0c): sprintf(t, "bcc %+d", (int8_t)op1); break;
    case  (0x0d): sprintf(t, "bcs %+d", (int8_t)op1); break;
    case  (0x0e): sprintf(t, "bvc %+d", (int8_t)op1); break;
    case  (0x0f): sprintf(t, "bvs %+d", (int8_t)op1); break;
    case16(0x10): sprintf(t, "to r%u", op0 & 15); break;
    case16(0x20): sprintf(t, "with r%u", op0 & 15); break;
    case12(0x30): sprintf(t, "stw (r%u)", op0 & 15); break;
    case  (0x3c): sprintf(t, "loop"); break;
    case  (0x3d): sprintf(t, "alt1"); break;
    case  (0x3e): sprintf(t, "alt2"); break;
    case  (0x3f): sprintf(t, "alt3"); break;
    case12(0x40): sprintf(t, "ldw (r%u)", op0 & 15); break;
    case  (0x4c): sprintf(t, "plot"); break;
    case  (0x4d): sprintf(t, "swap"); break;
    case  (0x4e): sprintf(t, "color"); break;
    case  (0x4f): sprintf(t, "not"); break;
    case16(0x50): sprintf(t, "add #%u", op0 & 15); break;
    case16(0x60): sprintf(t, "sub #%u", op0 & 15); break;
    case  (0x70): sprintf(t, "merge"); break;
    case15(0x71): sprintf(t, "and #%u", op0 & 15); break;
    case16(0x80): sprintf(t, "mult #%u", op0 & 15); break;
    case  (0x90): sprintf(t, "sbk"); break;
    case4 (0x91): sprintf(t, "link #%u", op0 & 15); break;
    case  (0x95): sprintf(t, "sex"); break;
    case  (0x96): sprintf(t, "asr"); break;
    case  (0x97): sprintf(t, "ror"); break;
    case6 (0x98): sprintf(t, "jmp r%u", op0 & 15); break;
    case  (0x9e): sprintf(t, "lob"); break;
    case  (0x9f): sprintf(t, "fmult"); break;
    case16(0xa0): sprintf(t, "sms r%u,(#$%.4x)", op0 & 15, op1 << 1); break;
    case16(0xb0): sprintf(t, "from r%u", op0 & 15); break;
    case  (0xc0): sprintf(t, "hib"); break;
    case15(0xc1): sprintf(t, "or #%u", op0 & 15); break;
    case15(0xd0): sprintf(t, "inc r%u", op0 & 15); break;
    case  (0xdf): sprintf(t, "ramb"); break;
    case15(0xe0): sprintf(t, "dec r%u", op0 & 15); break;
    case  (0xef): sprintf(t, "getbl"); break;
    case16(0xf0): sprintf(t, "sm r%u", op0 & 15); break;
  }
  strcat(output, t);
}

void GSU::disassembleALT3(char* output) {
  char t[256] = "";
  switch(op0) {
    case  (0x00): sprintf(t, "stop"); break;
    case  (0x01): sprintf(t, "nop"); break;
    case  (0x02): sprintf(t, "cache"); break;
    case  (0x03): sprintf(t, "lsr"); break;
    case  (0x04): sprintf(t, "rol"); break;
    case  (0x05): sprintf(t, "bra %+d", (int8_t)op1); break;
    case  (0x06): sprintf(t, "blt %+d", (int8_t)op1); break;
    case  (0x07): sprintf(t, "bge %+d", (int8_t)op1); break;
    case  (0x08): sprintf(t, "bne %+d", (int8_t)op1); break;
    case  (0x09): sprintf(t, "beq %+d", (int8_t)op1); break;
    case  (0x0a): sprintf(t, "bpl %+d", (int8_t)op1); break;
    case  (0x0b): sprintf(t, "bmi %+d", (int8_t)op1); break;
    case  (0x0c): sprintf(t, "bcc %+d", (int8_t)op1); break;
    case  (0x0d): sprintf(t, "bcs %+d", (int8_t)op1); break;
    case  (0x0e): sprintf(t, "bvc %+d", (int8_t)op1); break;
    case  (0x0f): sprintf(t, "bvs %+d", (int8_t)op1); break;
    case16(0x10): sprintf(t, "to r%u", op0 & 15); break;
    case16(0x20): sprintf(t, "with r%u", op0 & 15); break;
    case12(0x30): sprintf(t, "stb (r%u)", op0 & 15); break;
    case  (0x3c): sprintf(t, "loop"); break;
    case  (0x3d): sprintf(t, "alt1"); break;
    case  (0x3e): sprintf(t, "alt2"); break;
    case  (0x3f): sprintf(t, "alt3"); break;
    case12(0x40): sprintf(t, "ldb (r%u)", op0 & 15); break;
    case  (0x4c): sprintf(t, "rpix"); break;
    case  (0x4d): sprintf(t, "swap"); break;
    case  (0x4e): sprintf(t, "cmode"); break;
    case  (0x4f): sprintf(t, "not"); break;
    case16(0x50): sprintf(t, "adc #%u", op0 & 15); break;
    case16(0x60): sprintf(t, "cmp r%u", op0 & 15); break;
    case  (0x70): sprintf(t, "merge"); break;
    case15(0x71): sprintf(t, "bic #%u", op0 & 15); break;
    case16(0x80): sprintf(t, "umult #%u", op0 & 15); break;
    case  (0x90): sprintf(t, "sbk"); break;
    case4 (0x91): sprintf(t, "link #%u", op0 & 15); break;
    case  (0x95): sprintf(t, "sex"); break;
    case  (0x96): sprintf(t, "div2"); break;
    case  (0x97): sprintf(t, "ror"); break;
    case6 (0x98): sprintf(t, "ljmp r%u", op0 & 15); break;
    case  (0x9e): sprintf(t, "lob"); break;
    case  (0x9f): sprintf(t, "lmult"); break;
    case16(0xa0): sprintf(t, "lms r%u", op0 & 15); break;
    case16(0xb0): sprintf(t, "from r%u", op0 & 15); break;
    case  (0xc0): sprintf(t, "hib"); break;
    case15(0xc1): sprintf(t, "xor #%u", op0 & 15); break;
    case15(0xd0): sprintf(t, "inc r%u", op0 & 15); break;
    case  (0xdf): sprintf(t, "romb"); break;
    case15(0xe0): sprintf(t, "dec r%u", op0 & 15); break;
    case  (0xef): sprintf(t, "getbs"); break;
    case16(0xf0): sprintf(t, "lm r%u", op0 & 15); break;
  }
  strcat(output, t);
}

#undef case4
#undef case6
#undef case12
#undef case15
#undef case16
#undef op0
#undef op1
#undef op2

void GSU::power() {
  for(auto& r : regs.r) {
    r.data = 0x0000;
    r.modified = false;
  }

  regs.sfr.data = 0x0000;
  regs.pbr      = 0x00;
  regs.rombr    = 0x00;
  regs.rambr    = 0;
  regs.cbr      = 0x0000;
  regs.scbr     = 0x00;
  regs.scmr     = 0x00;
  regs.colr     = 0x00;
  regs.por      = 0x00;
  regs.bramr    = 0;
  regs.vcr      = 0x04;
  regs.cfgr     = 0x00;
  regs.clsr     = 0;
  regs.pipeline = 0x01;  //nop
  regs.ramaddr  = 0x0000;
  regs.reset();
}

}
