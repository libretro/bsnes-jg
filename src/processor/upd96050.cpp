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

#include "upd96050.hpp"

namespace Processor {

void uPD96050::exec() {
  uint32_t opcode = programROM[regs.pc++] & 0xffffff;
  switch(opcode >> 22) {
  case 0: execOP(opcode); break;
  case 1: execRT(opcode); break;
  case 2: execJP(opcode); break;
  case 3: execLD(opcode); break;
  }

  int32_t result = (int32_t)regs.k * regs.l;  //sign + 30-bit result
  regs.m = result >> 15;  //store sign + top 15-bits
  regs.n = result <<  1;  //store low 15-bits + zero
}

void uPD96050::execOP(uint32_t opcode) {
  uint8_t pselect = (opcode >> 20) & 0x03;  //P select
  uint8_t alu     = (opcode >> 16) & 0x0f;  //ALU operation mode
  uint8_t asl     = (opcode >> 15) & 0x01;  //accumulator select
  uint8_t dpl     = (opcode >> 13) & 0x03;  //DP low modify
  uint8_t dphm    = (opcode >>  9) & 0x0f;  //DP high XOR modify
  uint8_t rpdcr   = (opcode >>  8) & 0x01;  //RP decrement
  uint8_t src     = (opcode >>  4) & 0x0f;  //move source
  uint8_t dst     = (opcode >>  0) & 0x0f;  //move destination

  uint16_t idb = 0;
  switch(src) {
  case  0: idb = regs.trb; break;
  case  1: idb = regs.a; break;
  case  2: idb = regs.b; break;
  case  3: idb = regs.tr; break;
  case  4: idb = regs.dp; break;
  case  5: idb = regs.rp; break;
  case  6: idb = dataROM[regs.rp]; break;
  case  7: idb = 0x8000 - flags.a.s1; break;  //ASL ignored; always SA1
  case  8: idb = regs.dr; regs.sr.rqm = 1; break;
  case  9: idb = regs.dr; break;
  case 10: idb = regs.sr; break;
  case 11: idb = regs.si; break;  //MSB
  case 12: idb = regs.si; break;  //LSB
  case 13: idb = regs.k; break;
  case 14: idb = regs.l; break;
  case 15: idb = dataRAM[regs.dp]; break;
  }

  if(alu) {
    uint16_t p = 0, q = 0, r = 0;
    Flag flag;
    bool c;

    switch(pselect) {
    case 0: p = dataRAM[regs.dp]; break;
    case 1: p = idb; break;
    case 2: p = regs.m; break;
    case 3: p = regs.n; break;
    }

    switch(asl) {
    case 0: q = regs.a; flag = flags.a; c = flags.b.c; break;
    case 1: q = regs.b; flag = flags.b; c = flags.a.c; break;
    }

    switch(alu) {
    case  1: r = q | p; break;                  //OR
    case  2: r = q & p; break;                  //AND
    case  3: r = q ^ p; break;                  //XOR
    case  4: r = q - p; break;                  //SUB
    case  5: r = q + p; break;                  //ADD
    case  6: r = q - p - c; break;              //SBB
    case  7: r = q + p + c; break;              //ADC
    case  8: r = q - 1; p = 1; break;           //DEC
    case  9: r = q + 1; p = 1; break;           //INC
    case 10: r = ~q; break;                     //CMP
    case 11: r = q >> 1 | (q & 0x8000); break;  //SHR1 (ASR)
    case 12: r = q << 1 | c; break;             //SHL1 (ROL)
    case 13: r = q << 2 | 3; break;             //SHL2
    case 14: r = q << 4 | 15; break;            //SHL4
    case 15: r = q << 8 | q >> 8; break;        //XCHG
    }

    flag.z = r == 0;
    flag.s0 = r & 0x8000;
    if(!flag.ov1) flag.s1 = flag.s0;

    switch(alu) {

    case  1:    //OR
    case  2:    //AND
    case  3:    //XOR
    case 10:    //CMP
    case 13:    //SHL2
    case 14:    //SHL4
    case 15: {  //XCHG
      flag.ov0 = 0;
      flag.ov1 = 0;
      flag.c = 0;
      break;
    }

    case  4:    //SUB
    case  5:    //ADD
    case  6:    //SBB
    case  7:    //ADC
    case  8:    //DEC
    case  9: {  //INC
      if(alu & 1) {
        //addition
        flag.ov0 = (q ^ r) & ~(q ^ p) & 0x8000;
        flag.c = r < q;
      } else {
        //subtraction
        flag.ov0 = (q ^ r) &  (q ^ p) & 0x8000;
        flag.c = r > q;
      }
      flag.ov1 = (flag.ov0 & flag.ov1) ? flag.s0 == flag.s1 : flag.ov0 | flag.ov1;
      break;
    }

    case 11: {  //SHR1 (ASR)
      flag.ov0 = 0;
      flag.ov1 = 0;
      flag.c = q & 1;
      break;
    }

    case 12: {  //SHL1 (ROL)
      flag.ov0 = 0;
      flag.ov1 = 0;
      flag.c = q >> 15;
      break;
    }

    }

    switch(asl) {
    case 0: regs.a = r; flags.a = flag; break;
    case 1: regs.b = r; flags.b = flag; break;
    }
  }

  execLD(idb << 6 | dst);

  if(dst != 4) {  //if LD does not write to DP
    switch(dpl) {
    case 1: regs.dp = (regs.dp & 0xf0) + ((regs.dp + 1) & 0x0f); break;  //DPINC
    case 2: regs.dp = (regs.dp & 0xf0) + ((regs.dp - 1) & 0x0f); break;  //DPDEC
    case 3: regs.dp = (regs.dp & 0xf0); break;  //DPCLR
    }
    regs.dp ^= dphm << 4;
  }

  if(dst != 5) {  //if LD does not write to RP
    if(rpdcr) regs.rp--;
  }
}

void uPD96050::execRT(uint32_t opcode) {
  execOP(opcode);
  regs.sp = (regs.sp - 1) & 0x0f;
  regs.pc = regs.stack[regs.sp];
}

void uPD96050::execJP(uint32_t opcode) {
  uint16_t brch = (opcode >> 13) & 0x1ff; //branch
  uint16_t na   = (opcode >> 2) & 0x7ff;  //next address
  uint8_t bank  = opcode & 0x03;         //bank address

  uint16_t jp = ((regs.pc & 0x2000) | bank << 11 | na) & 0x3fff;

  switch(brch) {
  case 0x000: regs.pc = regs.so; return;  //JMPSO

  case 0x080: if(flags.a.c == 0) regs.pc = jp; return;  //JNCA
  case 0x082: if(flags.a.c == 1) regs.pc = jp; return;  //JCA
  case 0x084: if(flags.b.c == 0) regs.pc = jp; return;  //JNCB
  case 0x086: if(flags.b.c == 1) regs.pc = jp; return;  //JCB

  case 0x088: if(flags.a.z == 0) regs.pc = jp; return;  //JNZA
  case 0x08a: if(flags.a.z == 1) regs.pc = jp; return;  //JZA
  case 0x08c: if(flags.b.z == 0) regs.pc = jp; return;  //JNZB
  case 0x08e: if(flags.b.z == 1) regs.pc = jp; return;  //JZB

  case 0x090: if(flags.a.ov0 == 0) regs.pc = jp; return;  //JNOVA0
  case 0x092: if(flags.a.ov0 == 1) regs.pc = jp; return;  //JOVA0
  case 0x094: if(flags.b.ov0 == 0) regs.pc = jp; return;  //JNOVB0
  case 0x096: if(flags.b.ov0 == 1) regs.pc = jp; return;  //JOVB0

  case 0x098: if(flags.a.ov1 == 0) regs.pc = jp; return;  //JNOVA1
  case 0x09a: if(flags.a.ov1 == 1) regs.pc = jp; return;  //JOVA1
  case 0x09c: if(flags.b.ov1 == 0) regs.pc = jp; return;  //JNOVB1
  case 0x09e: if(flags.b.ov1 == 1) regs.pc = jp; return;  //JOVB1

  case 0x0a0: if(flags.a.s0 == 0) regs.pc = jp; return;  //JNSA0
  case 0x0a2: if(flags.a.s0 == 1) regs.pc = jp; return;  //JSA0
  case 0x0a4: if(flags.b.s0 == 0) regs.pc = jp; return;  //JNSB0
  case 0x0a6: if(flags.b.s0 == 1) regs.pc = jp; return;  //JSB0

  case 0x0a8: if(flags.a.s1 == 0) regs.pc = jp; return;  //JNSA1
  case 0x0aa: if(flags.a.s1 == 1) regs.pc = jp; return;  //JSA1
  case 0x0ac: if(flags.b.s1 == 0) regs.pc = jp; return;  //JNSB1
  case 0x0ae: if(flags.b.s1 == 1) regs.pc = jp; return;  //JSB1

  case 0x0b0: if((regs.dp & 0x0f) == 0x00) regs.pc = jp; return;  //JDPL0
  case 0x0b1: if((regs.dp & 0x0f) != 0x00) regs.pc = jp; return;  //JDPLN0
  case 0x0b2: if((regs.dp & 0x0f) == 0x0f) regs.pc = jp; return;  //JDPLF
  case 0x0b3: if((regs.dp & 0x0f) != 0x0f) regs.pc = jp; return;  //JDPLNF

  //serial input/output acknowledge not emulated
  case 0x0b4: if(regs.sr.siack == 0) regs.pc = jp; return;  //JNSIAK
  case 0x0b6: if(regs.sr.siack == 1) regs.pc = jp; return;  //JSIAK
  case 0x0b8: if(regs.sr.soack == 0) regs.pc = jp; return;  //JNSOAK
  case 0x0ba: if(regs.sr.soack == 1) regs.pc = jp; return;  //JSOAK

  case 0x0bc: if(regs.sr.rqm == 0) regs.pc = jp; return;  //JNRQM
  case 0x0be: if(regs.sr.rqm == 1) regs.pc = jp; return;  //JRQM

  case 0x100: regs.pc = jp & ~0x2000; return;  //LJMP
  case 0x101: regs.pc = jp |  0x2000; return;  //HJMP

  case 0x140: {  //LCALL
    regs.stack[regs.sp] = regs.pc;
    regs.sp = (regs.sp + 1) & 0x0f;
    regs.pc = jp & ~0x2000;
    return;
  }
  case 0x141: {  //HCALL
    regs.stack[regs.sp] = regs.pc;
    regs.sp = (regs.sp + 1) & 0x0f;
    regs.pc = jp |  0x2000;
    return;
  }
  }
}

void uPD96050::execLD(uint32_t opcode) {
  uint16_t id = opcode >> 6;  //immediate data
  uint8_t dst = opcode & 0x0f;  //destination

  switch(dst) {
  case  0: break;
  case  1: regs.a = id; break;
  case  2: regs.b = id; break;
  case  3: regs.tr = id; break;
  case  4: regs.dp = id; break;
  case  5: regs.rp = id; break;
  case  6: regs.dr = id; regs.sr.rqm = 1; break;
  case  7: regs.sr = (regs.sr & 0x907c) | (id & ~0x907c); break;
  case  8: regs.so = id; break;  //LSB
  case  9: regs.so = id; break;  //MSB
  case 10: regs.k = id; break;
  case 11: regs.k = id; regs.l = dataROM[regs.rp]; break;
  case 12: regs.l = id; regs.k = dataRAM[regs.dp | 0x40]; break;
  case 13: regs.l = id; break;
  case 14: regs.trb = id; break;
  case 15: dataRAM[regs.dp] = id; break;
  }
}

uint8_t uPD96050::readSR() {
  return regs.sr >> 8;
}

void uPD96050::writeSR(uint8_t data) {
  if (data) {}
}

uint8_t uPD96050::readDR() {
  if(regs.sr.drc == 0) {
    //16-bit
    if(regs.sr.drs == 0) {
      regs.sr.drs = 1;
      return regs.dr >> 0;
    } else {
      regs.sr.rqm = 0;
      regs.sr.drs = 0;
      return regs.dr >> 8;
    }
  } else {
    //8-bit
    regs.sr.rqm = 0;
    return regs.dr >> 0;
  }
}

void uPD96050::writeDR(uint8_t data) {
  if(regs.sr.drc == 0) {
    //16-bit
    if(regs.sr.drs == 0) {
      regs.sr.drs = 1;
      regs.dr = (regs.dr & 0xff00) | (data << 0);
    } else {
      regs.sr.rqm = 0;
      regs.sr.drs = 0;
      regs.dr = (data << 8) | (regs.dr & 0x00ff);
    }
  } else {
    //8-bit
    regs.sr.rqm = 0;
    regs.dr = (regs.dr & 0xff00) | (data << 0);
  }
}

uint8_t uPD96050::readDP(uint16_t addr) {
  bool hi = addr & 1;
  addr = (addr >> 1) & 0x7ff;

  if(hi == false) {
    return dataRAM[addr] >> 0;
  } else {
    return dataRAM[addr] >> 8;
  }
}

void uPD96050::writeDP(uint16_t addr, uint8_t data) {
  bool hi = addr & 1;
  addr = (addr >> 1) & 0x7ff;

  if(hi == false) {
    dataRAM[addr] = (dataRAM[addr] & 0xff00) | (data << 0);
  } else {
    dataRAM[addr] = (dataRAM[addr] & 0x00ff) | (data << 8);
  }
}

void uPD96050::serialize(serializer& s) {
  s.array(dataRAM);
  regs.serialize(s);
  flags.a.serialize(s);
  flags.b.serialize(s);
}

void uPD96050::Flag::serialize(serializer& s) {
  s.boolean(ov0);
  s.boolean(ov1);
  s.boolean(z);
  s.boolean(c);
  s.boolean(s0);
  s.boolean(s1);
}

void uPD96050::Status::serialize(serializer& s) {
  s.boolean(p0);
  s.boolean(p1);
  s.boolean(ei);
  s.boolean(sic);
  s.boolean(soc);
  s.boolean(drc);
  s.boolean(dma);
  s.boolean(drs);
  s.boolean(usf0);
  s.boolean(usf1);
  s.boolean(rqm);
  s.boolean(siack);
  s.boolean(soack);
}

void uPD96050::Registers::serialize(serializer& s) {
  s.array(stack);
  s.integer(pc);
  s.integer(rp);
  s.integer(dp);
  s.integer(sp);
  s.integer(si);
  s.integer(so);
  s.integer(k);
  s.integer(l);
  s.integer(m);
  s.integer(n);
  s.integer(a);
  s.integer(b);
  s.integer(tr);
  s.integer(trb);
  s.integer(dr);
  sr.serialize(s);
}

void uPD96050::power() {
  // Take a very close look at this when fixing NEC DSP LLE support
  /*if(revision == Revision::uPD7725) {
    regs.pc.resize(11);
    regs.rp.resize(10);
    regs.dp.resize( 8);
  }

  if(revision == Revision::uPD96050) {
    regs.pc.resize(14);
    regs.rp.resize(11);
    regs.dp.resize(11);
  }*/

  for(unsigned n = 0; n < 16; ++n) regs.stack[n] = 0x0000;
  regs.pc = 0x0000;
  regs.rp = 0x0000;
  regs.dp = 0x0000;
  regs.sp = 0x0;
  regs.si = 0x0000;
  regs.so = 0x0000;
  regs.k = 0x0000;
  regs.l = 0x0000;
  regs.m = 0x0000;
  regs.n = 0x0000;
  regs.a = 0x0000;
  regs.b = 0x0000;
  regs.tr = 0x0000;
  regs.trb = 0x0000;
  regs.dr = 0x0000;
  regs.sr = 0x0000;
  regs.sr.siack = 0;
  regs.sr.soack = 0;

  flags.a = 0x0000;
  flags.b = 0x0000;
}

}
