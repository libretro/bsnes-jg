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
#include "../../nall/iterator.hpp"

#include "hg51b.hpp"

namespace Processor {

uint32_t HG51B::readRegister(uint8_t address) {
  switch(address & 0x7f) {
  case 0x01: return r.mul >> 24 & 0xffffff;
  case 0x02: return r.mul >>  0 & 0xffffff;
  case 0x03: return r.mdr;
  case 0x08: return r.rom;
  case 0x0c: return r.ram;
  case 0x13: return r.mar;
  case 0x1c: return r.dpr;
  case 0x20: return r.pc;
  case 0x28: return r.p;
  case 0x2e:
    io.bus.enable  = 1;
    io.bus.reading = 1;
    io.bus.pending = (1 + io.wait.rom) & 0x0f;
    io.bus.address = r.mar;
    return 0x000000;
  case 0x2f:
    io.bus.enable  = 1;
    io.bus.reading = 1;
    io.bus.pending = (1 + io.wait.ram) & 0x0f;
    io.bus.address = r.mar;
    return 0x000000;

  //constant registers
  case 0x50: return 0x000000;
  case 0x51: return 0xffffff;
  case 0x52: return 0x00ff00;
  case 0x53: return 0xff0000;
  case 0x54: return 0x00ffff;
  case 0x55: return 0xffff00;
  case 0x56: return 0x800000;
  case 0x57: return 0x7fffff;
  case 0x58: return 0x008000;
  case 0x59: return 0x007fff;
  case 0x5a: return 0xff7fff;
  case 0x5b: return 0xffff7f;
  case 0x5c: return 0x010000;
  case 0x5d: return 0xfeffff;
  case 0x5e: return 0x000100;
  case 0x5f: return 0x00feff;

  //general purpose registers
  case 0x60: case 0x70: return r.gpr[ 0];
  case 0x61: case 0x71: return r.gpr[ 1];
  case 0x62: case 0x72: return r.gpr[ 2];
  case 0x63: case 0x73: return r.gpr[ 3];
  case 0x64: case 0x74: return r.gpr[ 4];
  case 0x65: case 0x75: return r.gpr[ 5];
  case 0x66: case 0x76: return r.gpr[ 6];
  case 0x67: case 0x77: return r.gpr[ 7];
  case 0x68: case 0x78: return r.gpr[ 8];
  case 0x69: case 0x79: return r.gpr[ 9];
  case 0x6a: case 0x7a: return r.gpr[10];
  case 0x6b: case 0x7b: return r.gpr[11];
  case 0x6c: case 0x7c: return r.gpr[12];
  case 0x6d: case 0x7d: return r.gpr[13];
  case 0x6e: case 0x7e: return r.gpr[14];
  case 0x6f: case 0x7f: return r.gpr[15];
  }

  return 0x000000;  //verified
}

void HG51B::writeRegister(uint8_t address, uint32_t data) {
  switch(address & 0x7f) {
  case 0x01: r.mul = (r.mul &  0xffffffull) | (data & 0xffffff) << 24; return;
  case 0x02: r.mul = (r.mul & ~0xffffffull) | (data & 0xffffff); return;
  case 0x03: r.mdr = data & 0xffffff; return;
  case 0x08: r.rom = data & 0xffffff; return;
  case 0x0c: r.ram = data & 0xffffff; return;
  case 0x13: r.mar = data & 0xffffff; return;
  case 0x1c: r.dpr = data & 0xffffff; return;
  case 0x20: r.pc = data & 0xffffff; return;
  case 0x28: r.p = data & 0xffffff; return;
  case 0x2e:
    io.bus.enable  = 1;
    io.bus.writing = 1;
    io.bus.pending = (1 + io.wait.rom) & 0x0f;
    io.bus.address = r.mar;
    return;
  case 0x2f:
    io.bus.enable  = 1;
    io.bus.writing = 1;
    io.bus.pending = (1 + io.wait.ram) & 0x0f;
    io.bus.address = r.mar;
    return;

  case 0x60: case 0x70: r.gpr[ 0] = data & 0xffffff; return;
  case 0x61: case 0x71: r.gpr[ 1] = data & 0xffffff; return;
  case 0x62: case 0x72: r.gpr[ 2] = data & 0xffffff; return;
  case 0x63: case 0x73: r.gpr[ 3] = data & 0xffffff; return;
  case 0x64: case 0x74: r.gpr[ 4] = data & 0xffffff; return;
  case 0x65: case 0x75: r.gpr[ 5] = data & 0xffffff; return;
  case 0x66: case 0x76: r.gpr[ 6] = data & 0xffffff; return;
  case 0x67: case 0x77: r.gpr[ 7] = data & 0xffffff; return;
  case 0x68: case 0x78: r.gpr[ 8] = data & 0xffffff; return;
  case 0x69: case 0x79: r.gpr[ 9] = data & 0xffffff; return;
  case 0x6a: case 0x7a: r.gpr[10] = data & 0xffffff; return;
  case 0x6b: case 0x7b: r.gpr[11] = data & 0xffffff; return;
  case 0x6c: case 0x7c: r.gpr[12] = data & 0xffffff; return;
  case 0x6d: case 0x7d: r.gpr[13] = data & 0xffffff; return;
  case 0x6e: case 0x7e: r.gpr[14] = data & 0xffffff; return;
  case 0x6f: case 0x7f: r.gpr[15] = data & 0xffffff; return;
  }
}

HG51B::HG51B() {
  #define bind(id, name, ...) { \
    if(instructionTable[id]) throw; \
    instructionTable[id] = [=] { return instruction##name(__VA_ARGS__); }; \
  }

  #define pattern(s) \
    std::integral_constant<uint16_t, s>::value
    //std::integral_constant<uint16_t, nall::test(s)>::value

  static const nall::Natural< 5> shifts[] = {0, 1, 8, 16};

  //NOP
  for(nall::Natural<10> null : nall::range(1024)) {
    //auto opcode = pattern("0000 00.. .... ....");
    auto opcode = pattern(0x0000);
    bind(opcode | null << 0, NOP);
  }

  //???
  for(nall::Natural<10> null : nall::range(1024)) {
    //auto opcode = pattern("0000 01.. .... ....");
    auto opcode = pattern(0x0400);
    bind(opcode | null << 0, NOP);
  }

  //JMP imm
  for(uint8_t data : nall::range(256))
  for(nall::Natural< 1> null : nall::range(  2))
  for(nall::Natural< 1> far  : nall::range(  2)) {
    //auto opcode = pattern("0000 10f. dddd dddd");
    auto opcode = pattern(0x0800);
    bind(opcode | data << 0 | null << 8 | far << 9, JMP, data, far, 1);
  }

  //JMP EQ,imm
  for(uint8_t data : nall::range(256))
  for(nall::Natural< 1> null : nall::range(  2))
  for(nall::Natural< 1> far  : nall::range(  2)) {
    //auto opcode = pattern("0000 11f. dddd dddd");
    auto opcode = pattern(0x0c00);
    bind(opcode | data << 0 | null << 8 | far << 9, JMP, data, far, r.z);
  }

  //JMP GE,imm
  for(uint8_t data : nall::range(256))
  for(nall::Natural< 1> null : nall::range(  2))
  for(nall::Natural< 1> far  : nall::range(  2)) {
    //auto opcode = pattern("0001 00f. dddd dddd");
    auto opcode = pattern(0x1000);
    bind(opcode | data << 0 | null << 8 | far << 9, JMP, data, far, r.c);
  }

  //JMP MI,imm
  for(uint8_t data : nall::range(256))
  for(nall::Natural< 1> null : nall::range(  2))
  for(nall::Natural< 1> far  : nall::range(  2)) {
    //auto opcode = pattern("0001 01f. dddd dddd");
    auto opcode = pattern(0x1400);
    bind(opcode | data << 0 | null << 8 | far << 9, JMP, data, far, r.n);
  }

  //JMP VS,imm
  for(uint8_t data : nall::range(256))
  for(nall::Natural< 1> null : nall::range(  2))
  for(nall::Natural< 1> far  : nall::range(  2)) {
    //auto opcode = pattern("0001 10f. dddd dddd");
    auto opcode = pattern(0x1800);
    bind(opcode | data << 0 | null << 8 | far << 9, JMP, data, far, r.v);
  }

  //WAIT
  for(nall::Natural<10> null : nall::range(1024)) {
    //auto opcode = pattern("0001 11.. .... ....");
    auto opcode = pattern(0x1c00);
    bind(opcode | null << 0, WAIT);
  }

  //???
  for(nall::Natural<10> null : nall::range(1024)) {
    //auto opcode = pattern("0010 00.. .... ....");
    auto opcode = pattern(0x2000);
    bind(opcode | null << 0, NOP);
  }

  //SKIP V
  for(nall::Natural< 1> take : nall::range(  2))
  for(nall::Natural< 7> null : nall::range(128)) {
    //auto opcode = pattern("0010 0100 .... ...t");
    auto opcode = pattern(0x2400);
    bind(opcode | take << 0 | null << 1, SKIP, take, r.v);
  }

  //SKIP C
  for(nall::Natural< 1> take : nall::range(  2))
  for(nall::Natural< 7> null : nall::range(128)) {
    //auto opcode = pattern("0010 0101 .... ...t");
    auto opcode = pattern(0x2500);
    bind(opcode | take << 0 | null << 1, SKIP, take, r.c);
  }

  //SKIP Z
  for(nall::Natural< 1> take : nall::range(  2))
  for(nall::Natural< 7> null : nall::range(128)) {
    //auto opcode = pattern("0010 0110 .... ...t");
    auto opcode = pattern(0x2600);
    bind(opcode | take << 0 | null << 1, SKIP, take, r.z);
  }

  //SKIP N
  for(nall::Natural< 1> take : nall::range(  2))
  for(nall::Natural< 7> null : nall::range(128)) {
    //auto opcode = pattern("0010 0111 .... ...t");
    auto opcode = pattern(0x2700);
    bind(opcode | take << 0 | null << 1, SKIP, take, r.n);
  }

  //JSR
  for(uint8_t data : nall::range(256))
  for(nall::Natural< 1> null : nall::range(  2))
  for(nall::Natural< 1> far  : nall::range(  2)) {
    //auto opcode = pattern("0010 10f. dddd dddd");
    auto opcode = pattern(0x2800);
    bind(opcode | data << 0 | null << 8 | far << 9, JSR, data, far, 1);
  }

  //JSR EQ,imm
  for(uint8_t data : nall::range(256))
  for(nall::Natural< 1> null : nall::range(  2))
  for(nall::Natural< 1> far  : nall::range(  2)) {
    //auto opcode = pattern("0010 11f. dddd dddd");
    auto opcode = pattern(0x2c00);
    bind(opcode | data << 0 | null << 8 | far << 9, JSR, data, far, r.z);
  }

  //JSR GE,imm
  for(uint8_t data : nall::range(256))
  for(nall::Natural< 1> null : nall::range(  2))
  for(nall::Natural< 1> far  : nall::range(  2)) {
    //auto opcode = pattern("0011 00f. dddd dddd");
    auto opcode = pattern(0x3000);
    bind(opcode | data << 0 | null << 8 | far << 9, JSR, data, far, r.c);
  }

  //JSR MI,imm
  for(uint8_t data : nall::range(256))
  for(nall::Natural< 1> null : nall::range(  2))
  for(nall::Natural< 1> far  : nall::range(  2)) {
    //auto opcode = pattern("0011 01f. dddd dddd");
    auto opcode = pattern(0x3400);
    bind(opcode | data << 0 | null << 8 | far << 9, JSR, data, far, r.n);
  }

  //JSR VS,imm
  for(uint8_t data : nall::range(256))
  for(nall::Natural< 1> null : nall::range(  2))
  for(nall::Natural< 1> far  : nall::range(  2)) {
    //auto opcode = pattern("0011 10f. dddd dddd");
    auto opcode = pattern(0x3800);
    bind(opcode | data << 0 | null << 8 | far << 9, JSR, data, far, r.v);
  }

  //RTS
  for(nall::Natural<10> null : nall::range(1024)) {
    //auto opcode = pattern("0011 11.. .... ....");
    auto opcode = pattern(0x3c00);
    bind(opcode | null << 0, RTS);
  }

  //INC MAR
  for(nall::Natural<10> null : nall::range(1024)) {
    //auto opcode = pattern("0100 00.. .... ....");
    auto opcode = pattern(0x4000);
    bind(opcode | null << 0, INC, r.mar);
  }

  //???
  for(nall::Natural<10> null : nall::range(1024)) {
    //auto opcode = pattern("0100 01.. .... ....");
    auto opcode = pattern(0x4400);
    bind(opcode | null << 0, NOP);
  }

  //CMPR A<<s,reg
  for(nall::Natural< 7> reg   : nall::range(128))
  for(nall::Natural< 1> null  : nall::range(  2))
  for(nall::Natural< 2> shift : nall::range(  4)) {
    //auto opcode = pattern("0100 10ss .rrr rrrr");
    auto opcode = pattern(0x4800);
    bind(opcode | reg << 0 | null << 7 | shift << 8, CMPR, reg, shifts[shift]);
  }

  //CMPR A<<s,imm
  for(uint8_t imm   : nall::range(256))
  for(nall::Natural< 2> shift : nall::range(  4)) {
    //auto opcode = pattern("0100 11ss iiii iiii");
    auto opcode = pattern(0x4c00);
    bind(opcode | imm << 0 | shift << 8, CMPR, imm, shifts[shift]);
  }

  //CMP A<<s,reg
  for(nall::Natural< 7> reg   : nall::range(128))
  for(nall::Natural< 1> null  : nall::range(  2))
  for(nall::Natural< 2> shift : nall::range(  4)) {
    //auto opcode = pattern("0101 00ss .rrr rrrr");
    auto opcode = pattern(0x5000);
    bind(opcode | reg << 0 | null << 7 | shift << 8, CMP, reg, shifts[shift]);
  }

  //CMP A<<s,imm
  for(uint8_t imm   : nall::range(256))
  for(nall::Natural< 2> shift : nall::range(  4)) {
    //auto opcode = pattern("0101 01ss iiii iiii");
    auto opcode = pattern(0x5400);
    bind(opcode | imm << 0 | shift << 8, CMP, imm, shifts[shift]);
  }

  //???
  for(uint8_t null : nall::range(256)) {
    //auto opcode = pattern("0101 1000 .... ....");
    auto opcode = pattern(0x5800);
    bind(opcode | null << 0, NOP);
  }

  //SXB A
  for(uint8_t null : nall::range(256)) {
    //auto opcode = pattern("0101 1001 .... ....");
    auto opcode = pattern(0x5900);
    bind(opcode | null << 0, SXB);
  }

  //SXW A
  for(uint8_t null : nall::range(256)) {
    //auto opcode = pattern("0101 1010 .... ....");
    auto opcode = pattern(0x5a00);
    bind(opcode | null << 0, SXW);
  }

  //???
  for(uint8_t null : nall::range(256)) {
    //auto opcode = pattern("0101 1011 .... ....");
    auto opcode = pattern(0x5b00);
    bind(opcode | null << 0, NOP);
  }

  //???
  for(nall::Natural<10> null : nall::range(1024)) {
    //auto opcode = pattern("0101 11.. .... ....");
    auto opcode = pattern(0x5c00);
    bind(opcode | null << 0, NOP);
  }

  //LD A,reg
  for(nall::Natural< 7> reg  : nall::range(128))
  for(nall::Natural< 1> null : nall::range(  2)) {
    //auto opcode = pattern("0110 0000 .rrr rrrr");
    auto opcode = pattern(0x6000);
    bind(opcode | reg << 0 | null << 7, LD, r.a, reg);
  }

  //LD MDR,reg
  for(nall::Natural< 7> reg  : nall::range(128))
  for(nall::Natural< 1> null : nall::range(  2)) {
    //auto opcode = pattern("0110 0001 .rrr rrrr");
    auto opcode = pattern(0x6100);
    bind(opcode | reg << 0 | null << 7, LD, r.mdr, reg);
  }

  //LD MAR,reg
  for(nall::Natural< 7> reg  : nall::range(128))
  for(nall::Natural< 1> null : nall::range(  2)) {
    //auto opcode = pattern("0110 0010 .rrr rrrr");
    auto opcode = pattern(0x6200);
    bind(opcode | reg << 0 | null << 7, LD, r.mar, reg);
  }

  //LD P,reg
  for(nall::Natural< 4> reg  : nall::range(16))
  for(nall::Natural< 4> null : nall::range(16)) {
    //auto opcode = pattern("0110 0011 .... rrrr");
    auto opcode = pattern(0x6300);
    bind(opcode | reg << 0 | null << 4, LD, r.p, reg);
  }

  //LD A,imm
  for(uint8_t imm : nall::range(256)) {
    //auto opcode = pattern("0110 0100 iiii iiii");
    auto opcode = pattern(0x6400);
    bind(opcode | imm << 0, LD, r.a, imm);
  }

  //LD MDR,imm
  for(uint8_t imm : nall::range(256)) {
    //auto opcode = pattern("0110 0101 iiii iiii");
    auto opcode = pattern(0x6500);
    bind(opcode | imm << 0, LD, r.mdr, imm);
  }

  //LD MAR,imm
  for(uint8_t imm : nall::range(256)) {
    //auto opcode = pattern("0110 0110 iiii iiii");
    auto opcode = pattern(0x6600);
    bind(opcode | imm << 0, LD, r.mar, imm);
  }

  //LD P,imm
  for(uint8_t imm : nall::range(256)) {
    //auto opcode = pattern("0110 0111 iiii iiii");
    auto opcode = pattern(0x6700);
    bind(opcode | imm << 0, LD, r.p, imm);
  }

  //RDRAM 0,A
  for(uint8_t null : nall::range(256)) {
    //auto opcode = pattern("0110 1000 .... ....");
    auto opcode = pattern(0x6800);
    bind(opcode | null << 0, RDRAM, 0, r.a);
  }

  //RDRAM 1,A
  for(uint8_t null : nall::range(256)) {
    //auto opcode = pattern("0110 1001 .... ....");
    auto opcode = pattern(0x6900);
    bind(opcode | null << 0, RDRAM, 1, r.a);
  }

  //RDRAM 2,A
  for(uint8_t null : nall::range(256)) {
    //auto opcode = pattern("0110 1010 .... ....");
    auto opcode = pattern(0x6a00);
    bind(opcode | null << 0, RDRAM, 2, r.a);
  }

  //???
  for(uint8_t null : nall::range(256)) {
    //auto opcode = pattern("0110 1011 .... ....");
    auto opcode = pattern(0x6b00);
    bind(opcode | null << 0, NOP);
  }

  //RDRAM 0,imm
  for(uint8_t imm : nall::range(256)) {
    //auto opcode = pattern("0110 1100 iiii iiii");
    auto opcode = pattern(0x6c00);
    bind(opcode | imm << 0, RDRAM, 0, imm);
  }

  //RDRAM 1,imm
  for(uint8_t imm : nall::range(256)) {
    //auto opcode = pattern("0110 1101 iiii iiii");
    auto opcode = pattern(0x6d00);
    bind(opcode | imm << 0, RDRAM, 1, imm);
  }

  //RDRAM 2,imm
  for(uint8_t imm : nall::range(256)) {
    //auto opcode = pattern("0110 1110 iiii iiii");
    auto opcode = pattern(0x6e00);
    bind(opcode | imm << 0, RDRAM, 2, imm);
  }

  //???
  for(uint8_t null : nall::range(256)) {
    //auto opcode = pattern("0110 1111 .... ....");
    auto opcode = pattern(0x6f00);
    bind(opcode | null << 0, NOP);
  }

  //RDROM A
  for(nall::Natural<10> null : nall::range(1024)) {
    //auto opcode = pattern("0111 00.. .... ....");
    auto opcode = pattern(0x7000);
    bind(opcode | null << 0, RDROM, r.a);
  }

  //RDROM imm
  for(nall::Natural<10> imm : nall::range(1024)) {
    //auto opcode = pattern("0111 01ii iiii iiii");
    auto opcode = pattern(0x7400);
    bind(opcode | imm << 0, RDROM, imm);
  }

  //???
  for(nall::Natural<10> null : nall::range(1024)) {
    //auto opcode = pattern("0111 10.. .... ....");
    auto opcode = pattern(0x7800);
    bind(opcode | null << 0, NOP);
  }

  //LD PL,imm
  for(uint8_t imm : nall::range(256)) {
    //auto opcode = pattern("0111 1100 iiii iiii");
    auto opcode = pattern(0x7c00);
    bind(opcode | imm << 0, LDL, r.p, imm);
  }

  //LD PH,imm
  for(nall::Natural< 7> imm  : nall::range(128))
  for(nall::Natural< 1> null : nall::range(  2)) {
    //auto opcode = pattern("0111 1101 .iii iiii");
    auto opcode = pattern(0x7d00);
    bind(opcode | imm << 0 | null << 7, LDH, r.p, imm);
  }

  //???
  for(nall::Natural< 9> null : nall::range(512)) {
    //auto opcode = pattern("0111 111. .... ....");
    auto opcode = pattern(0x7e00);
    bind(opcode | null << 0, NOP);
  }

  //ADD A<<s,reg
  for(nall::Natural< 7> reg   : nall::range(128))
  for(nall::Natural< 1> null  : nall::range(  2))
  for(nall::Natural< 2> shift : nall::range(  4)) {
    //auto opcode = pattern("1000 00ss .rrr rrrr");
    auto opcode = pattern(0x8000);
    bind(opcode | reg << 0 | null << 7 | shift << 8, ADD, reg, shifts[shift]);
  }

  //ADD A<<s,imm
  for(uint8_t imm   : nall::range(256))
  for(nall::Natural< 2> shift : nall::range(  4)) {
    //auto opcode = pattern("1000 01ss iiii iiii");
    auto opcode = pattern(0x8400);
    bind(opcode | imm << 0 | shift << 8, ADD, imm, shifts[shift]);
  }

  //SUBR A<<s,reg
  for(nall::Natural< 7> reg   : nall::range(128))
  for(nall::Natural< 1> null  : nall::range(  2))
  for(nall::Natural< 2> shift : nall::range(  4)) {
    //auto opcode = pattern("1000 10ss .rrr rrrr");
    auto opcode = pattern(0x8800);
    bind(opcode | reg << 0 | null << 7 | shift << 8, SUBR, reg, shifts[shift]);
  }

  //SUBR A<<s,imm
  for(uint8_t imm   : nall::range(256))
  for(nall::Natural< 2> shift : nall::range(  4)) {
    //auto opcode = pattern("1000 11ss iiii iiii");
    auto opcode = pattern(0x8c00);
    bind(opcode | imm << 0 | shift << 8, SUBR, imm, shifts[shift]);
  }

  //SUB A<<s,reg
  for(nall::Natural< 7> reg   : nall::range(128))
  for(nall::Natural< 1> null  : nall::range(  2))
  for(nall::Natural< 2> shift : nall::range(  4)) {
    //auto opcode = pattern("1001 00ss .rrr rrrr");
    auto opcode = pattern(0x9000);
    bind(opcode | reg << 0 | null << 7 | shift << 8, SUB, reg, shifts[shift]);
  }

  //SUB A<<s,imm
  for(uint8_t imm   : nall::range(256))
  for(nall::Natural< 2> shift : nall::range(  4)) {
    //auto opcode = pattern("1001 01ss iiii iiii");
    auto opcode = pattern(0x9400);
    bind(opcode | imm << 0 | shift << 8, SUB, imm, shifts[shift]);
  }

  //MUL reg
  for(nall::Natural< 7> reg  : nall::range(128))
  for(nall::Natural< 3> null : nall::range(  8)) {
    //auto opcode = pattern("1001 10.. .rrr rrrr");
    auto opcode = pattern(0x9800);
    bind(opcode | reg << 0 | null << 7, MUL, reg);
  }

  //MUL imm
  for(uint8_t imm  : nall::range(256))
  for(nall::Natural< 2> null : nall::range(  4)) {
    //auto opcode = pattern("1001 11.. iiii iiii");
    auto opcode = pattern(0x9c00);
    bind(opcode | imm << 0 | null << 8, MUL, imm);
  }

  //XNOR A<<s,reg
  for(nall::Natural< 7> reg   : nall::range(128))
  for(nall::Natural< 1> null  : nall::range(  2))
  for(nall::Natural< 2> shift : nall::range(  4)) {
    //auto opcode = pattern("1010 00ss .rrr rrrr");
    auto opcode = pattern(0xa000);
    bind(opcode | reg << 0 | null << 7 | shift << 8, XNOR, reg, shifts[shift]);
  }

  //XNOR A<<s,imm
  for(uint8_t imm   : nall::range(256))
  for(nall::Natural< 2> shift : nall::range(  4)) {
    //auto opcode = pattern("1010 01ss iiii iiii");
    auto opcode = pattern(0xa400);
    bind(opcode | imm << 0 | shift << 8, XNOR, imm, shifts[shift]);
  }

  //XOR A<<s,reg
  for(nall::Natural< 7> reg   : nall::range(128))
  for(nall::Natural< 1> null  : nall::range(  2))
  for(nall::Natural< 2> shift : nall::range(  4)) {
    //auto opcode = pattern("1010 10ss .rrr rrrr");
    auto opcode = pattern(0xa800);
    bind(opcode | reg << 0 | null << 7 | shift << 8, XOR, reg, shifts[shift]);
  }

  //XOR A<<s,imm
  for(uint8_t imm   : nall::range(256))
  for(nall::Natural< 2> shift : nall::range(  4)) {
    //auto opcode = pattern("1010 11ss iiii iiii");
    auto opcode = pattern(0xac00);
    bind(opcode | imm << 0 | shift << 8, XOR, imm, shifts[shift]);
  }

  //AND A<<s,reg
  for(nall::Natural< 7> reg   : nall::range(128))
  for(nall::Natural< 1> null  : nall::range(  2))
  for(nall::Natural< 2> shift : nall::range(  4)) {
    //auto opcode = pattern("1011 00ss .rrr rrrr");
    auto opcode = pattern(0xb000);
    bind(opcode | reg << 0 | null << 7 | shift << 8, AND, reg, shifts[shift]);
  }

  //AND A<<s,imm
  for(uint8_t imm   : nall::range(256))
  for(nall::Natural< 2> shift : nall::range(  4)) {
    //auto opcode = pattern("1011 01ss iiii iiii");
    auto opcode = pattern(0xb400);
    bind(opcode | imm << 0 | shift << 8, AND, imm, shifts[shift]);
  }

  //OR A<<s,reg
  for(nall::Natural< 7> reg   : nall::range(128))
  for(nall::Natural< 1> null  : nall::range(  2))
  for(nall::Natural< 2> shift : nall::range(  4)) {
    //auto opcode = pattern("1011 10ss .rrr rrrr");
    auto opcode = pattern(0xb800);
    bind(opcode | reg << 0 | null << 7 | shift << 8, OR, reg, shifts[shift]);
  }

  //OR A<<s,imm
  for(uint8_t imm   : nall::range(256))
  for(nall::Natural< 2> shift : nall::range(  4)) {
    //auto opcode = pattern("1011 11ss iiii iiii");
    auto opcode = pattern(0xbc00);
    bind(opcode | imm << 0 | shift << 8, OR, imm, shifts[shift]);
  }

  //SHR A,reg
  for(nall::Natural< 7> reg  : nall::range(128))
  for(nall::Natural< 3> null : nall::range(  8)) {
    //auto opcode = pattern("1100 00.. .rrr rrrr");
    auto opcode = pattern(0xc000);
    bind(opcode | reg << 0 | null << 7, SHR, reg);
  }

  //SHR A,imm
  for(nall::Natural< 5> imm  : nall::range(32))
  for(nall::Natural< 5> null : nall::range(32)) {
    //auto opcode = pattern("1100 01.. ...i iiii");
    auto opcode = pattern(0xc400);
    bind(opcode | imm << 0 | null << 5, SHR, imm);
  }

  //ASR A,reg
  for(nall::Natural< 7> reg  : nall::range(128))
  for(nall::Natural< 3> null : nall::range(  8)) {
    //auto opcode = pattern("1100 10.. .rrr rrrr");
    auto opcode = pattern(0xc800);
    bind(opcode | reg << 0 | null << 7, ASR, reg);
  }

  //ASR A,imm
  for(nall::Natural< 5> imm  : nall::range(32))
  for(nall::Natural< 5> null : nall::range(32)) {
    //auto opcode = pattern("1100 11.. ...i iiii");
    auto opcode = pattern(0xcc00);
    bind(opcode | imm << 0 | null << 5, ASR, imm);
  }

  //ROR A,reg
  for(nall::Natural< 7> reg  : nall::range(128))
  for(nall::Natural< 3> null : nall::range(  8)) {
    //auto opcode = pattern("1101 00.. .rrr rrrr");
    auto opcode = pattern(0xd000);
    bind(opcode | reg << 0 | null << 7, ROR, reg);
  }

  //ROR A,imm
  for(nall::Natural< 5> imm  : nall::range(32))
  for(nall::Natural< 5> null : nall::range(32)) {
    //auto opcode = pattern("1101 01.. ...i iiii");
    auto opcode = pattern(0xd400);
    bind(opcode | imm << 0 | null << 5, ROR, imm);
  }

  //SHL A,reg
  for(nall::Natural< 7> reg  : nall::range(128))
  for(nall::Natural< 3> null : nall::range(  8)) {
    //auto opcode = pattern("1101 10.. .rrr rrrr");
    auto opcode = pattern(0xd800);
    bind(opcode | reg << 0 | null << 7, SHL, reg);
  }

  //SHL A,imm
  for(nall::Natural< 5> imm  : nall::range(32))
  for(nall::Natural< 5> null : nall::range(32)) {
    //auto opcode = pattern("1101 11.. ...i iiii");
    auto opcode = pattern(0xdc00);
    bind(opcode | imm << 0 | null << 5, SHL, imm);
  }

  //ST reg,A
  for(nall::Natural< 7> reg  : nall::range(128))
  for(nall::Natural< 1> null : nall::range(  2)) {
    //auto opcode = pattern("1110 0000 .rrr rrrr");
    auto opcode = pattern(0xe000);
    bind(opcode | reg << 0 | null << 7, ST, reg, r.a);
  }

  //ST reg,MDR
  for(nall::Natural< 7> reg  : nall::range(128))
  for(nall::Natural< 1> null : nall::range(  2)) {
    //auto opcode = pattern("1110 0001 .rrr rrrr");
    auto opcode = pattern(0xe100);
    bind(opcode | reg << 0 | null << 7, ST, reg, r.mdr);
  }

  //???
  for(nall::Natural< 9> null : nall::range(512)) {
    //auto opcode = pattern("1110 001. .... ....");
    auto opcode = pattern(0xe200);
    bind(opcode | null << 0, NOP);
  }

  //???
  for(nall::Natural<10> null : nall::range(1024)) {
    //auto opcode = pattern("1110 01.. .... ....");
    auto opcode = pattern(0xe400);
    bind(opcode | null << 0, NOP);
  }

  //WRRAM 0,A
  for(uint8_t null : nall::range(256)) {
    //auto opcode = pattern("1110 1000 .... ....");
    auto opcode = pattern(0xe800);
    bind(opcode | null << 0, WRRAM, 0, r.a);
  }

  //WRRAM 1,A
  for(uint8_t null : nall::range(256)) {
    //auto opcode = pattern("1110 1001 .... ....");
    auto opcode = pattern(0xe900);
    bind(opcode | null << 0, WRRAM, 1, r.a);
  }

  //WRRAM 2,A
  for(uint8_t null : nall::range(256)) {
    //auto opcode = pattern("1110 1010 .... ....");
    auto opcode = pattern(0xea00);
    bind(opcode | null << 0, WRRAM, 2, r.a);
  }

  //???
  for(uint8_t null : nall::range(256)) {
    //auto opcode = pattern("1110 1011 .... ....");
    auto opcode = pattern(0xeb00);
    bind(opcode | null << 0, NOP);
  }

  //WRRAM 0,imm
  for(uint8_t imm : nall::range(256)) {
    //auto opcode = pattern("1110 1100 iiii iiii");
    auto opcode = pattern(0xec00);
    bind(opcode | imm << 0, WRRAM, 0, imm);
  }

  //WRRAM 1,imm
  for(uint8_t imm : nall::range(256)) {
    //auto opcode = pattern("1110 1101 iiii iiii");
    auto opcode = pattern(0xed00);
    bind(opcode | imm << 0, WRRAM, 1, imm);
  }

  //WRRAM 2,imm
  for(uint8_t imm : nall::range(256)) {
    //auto opcode = pattern("1110 1110 iiii iiii");
    auto opcode = pattern(0xee00);
    bind(opcode | imm << 0, WRRAM, 2, imm);
  }

  //???
  for(uint8_t null : nall::range(256)) {
    //auto opcode = pattern("1110 1111 .... ....");
    auto opcode = pattern(0xef00);
    bind(opcode | null << 0, NOP);
  }

  //SWAP A,Rn
  for(nall::Natural< 4> reg  : nall::range(16))
  for(nall::Natural< 6> null : nall::range(64)) {
    //auto opcode = pattern("1111 00.. .... rrrr");
    auto opcode = pattern(0xf000);
    bind(opcode | reg << 0 | null << 4, SWAP, r.a, reg);
  }

  //???
  for(nall::Natural<10> null : nall::range(1024)) {
    //auto opcode = pattern("1111 01.. .... ....");
    auto opcode = pattern(0xf400);
    bind(opcode | null << 0, NOP);
  }

  //CLEAR
  for(nall::Natural<10> null : nall::range(1024)) {
    //auto opcode = pattern("1111 10.. .... ....");
    auto opcode = pattern(0xf800);
    bind(opcode | null << 0, CLEAR);
  }

  //HALT
  for(nall::Natural<10> null : nall::range(1024)) {
    //auto opcode = pattern("1111 11.. .... ....");
    auto opcode = pattern(0xfc00);
    bind(opcode | null << 0, HALT);
  }

  #undef bind
  #undef pattern

  for(unsigned opcode : nall::range(65536)) {
    if(!instructionTable[opcode]) throw;
  }
}

void HG51B::push() {
  stack[7] = stack[6];
  stack[6] = stack[5];
  stack[5] = stack[4];
  stack[4] = stack[3];
  stack[3] = stack[2];
  stack[2] = stack[1];
  stack[1] = stack[0];
  stack[0] = r.pb << 8 | r.pc << 0;
}

void HG51B::pull() {
  auto pc  = stack[0];
  stack[0] = stack[1];
  stack[1] = stack[2];
  stack[2] = stack[3];
  stack[3] = stack[4];
  stack[4] = stack[5];
  stack[5] = stack[6];
  stack[6] = stack[7];
  stack[7] = 0x0000;

  r.pb = pc >> 8;
  r.pc = pc >> 0;
}

nall::Natural<24> HG51B::algorithmADD(nall::Natural<24> x, nall::Natural<24> y) {
  int z = x + y;
  r.n = z & 0x800000;
  r.z = (nall::Natural<24>)z == 0;
  r.c = z > 0xffffff;
  r.v = ~(x ^ y) & (x ^ z) & 0x800000;
  return z;
}

nall::Natural<24> HG51B::algorithmAND(nall::Natural<24> x, nall::Natural<24> y) {
  x = x & y;
  r.n = x & 0x800000;
  r.z = x == 0;
  return x;
}

nall::Natural<24> HG51B::algorithmASR(nall::Natural<24> a, nall::Natural< 5> s) {
  if(s > 24) s = 0;
  a = signextend<int32_t,24>(a) >> s;
  r.n = a & 0x800000;
  r.z = a == 0;
  return a;
}

nall::Natural<48> HG51B::algorithmMUL(uint32_t x, uint32_t y) {
  return signextend<int64_t,24>(x) * signextend<int64_t,24>(y);
}

nall::Natural<24> HG51B::algorithmOR(nall::Natural<24> x, nall::Natural<24> y) {
  x = x | y;
  r.n = x & 0x800000;
  r.z = x == 0;
  return x;
}

nall::Natural<24> HG51B::algorithmROR(nall::Natural<24> a, nall::Natural< 5> s) {
  if(s > 24) s = 0;
  a = (a >> s) | (a << 24 - s);
  r.n = a & 0x800000;
  r.z = a == 0;
  return a;
}

nall::Natural<24> HG51B::algorithmSHL(nall::Natural<24> a, nall::Natural< 5> s) {
  if(s > 24) s = 0;
  a = a << s;
  r.n = a & 0x800000;
  r.z = a == 0;
  return a;
}

nall::Natural<24> HG51B::algorithmSHR(nall::Natural<24> a, nall::Natural< 5> s) {
  if(s > 24) s = 0;
  a = a >> s;
  r.n = a & 0x800000;
  r.z = a == 0;
  return a;
}

nall::Natural<24> HG51B::algorithmSUB(nall::Natural<24> x, nall::Natural<24> y) {
  int z = x - y;
  r.n = z & 0x800000;
  r.z = (nall::Natural<24>)z == 0;
  r.c = z >= 0;
  r.v = ~(x ^ y) & (x ^ z) & 0x800000;
  return z;
}

nall::Natural<24> HG51B::algorithmSX(nall::Natural<24> x) {
  r.n = x & 0x800000;
  r.z = x == 0;
  return x;
}

nall::Natural<24> HG51B::algorithmXNOR(nall::Natural<24> x, nall::Natural<24> y) {
  x = ~x ^ y;
  r.n = x & 0x800000;
  r.z = x == 0;
  return x;
}

nall::Natural<24> HG51B::algorithmXOR(nall::Natural<24> x, nall::Natural<24> y) {
  x = x ^ y;
  r.n = x & 0x800000;
  r.z = x == 0;
  return x;
}

void HG51B::instructionADD(nall::Natural< 7> reg, nall::Natural< 5> shift) {
  r.a = algorithmADD(r.a << shift, readRegister(reg));
}

void HG51B::instructionADD(uint8_t imm, nall::Natural< 5> shift) {
  r.a = algorithmADD(r.a << shift, imm);
}

void HG51B::instructionAND(nall::Natural< 7> reg, nall::Natural< 5> shift) {
  r.a = algorithmAND(r.a << shift, readRegister(reg));
}

void HG51B::instructionAND(uint8_t imm, nall::Natural< 5> shift) {
  r.a = algorithmAND(r.a << shift, imm);
}

void HG51B::instructionASR(nall::Natural< 7> reg) {
  r.a = algorithmASR(r.a, readRegister(reg));
}

void HG51B::instructionASR(nall::Natural< 5> imm) {
  r.a = algorithmASR(r.a, imm);
}

void HG51B::instructionCLEAR() {
  r.a = 0;
  r.p = 0;
  r.ram = 0;
  r.dpr = 0;
}

void HG51B::instructionCMP(nall::Natural< 7> reg, nall::Natural< 5> shift) {
  algorithmSUB(r.a << shift, readRegister(reg));
}

void HG51B::instructionCMP(uint8_t imm, nall::Natural< 5> shift) {
  algorithmSUB(r.a << shift, imm);
}

void HG51B::instructionCMPR(nall::Natural< 7> reg, nall::Natural< 5> shift) {
  algorithmSUB(readRegister(reg), r.a << shift);
}

void HG51B::instructionCMPR(uint8_t imm, nall::Natural< 5> shift) {
  algorithmSUB(imm, r.a << shift);
}

void HG51B::instructionHALT() {
  halt();
}

void HG51B::instructionINC(nall::Natural<24>& reg) {
  reg++;
}

void HG51B::instructionJMP(uint8_t data, nall::Natural< 1> far, const nall::Natural< 1>& take) {
  if(!take) return;
  if(far) r.pb = r.p;
  r.pc = data;
  step(2);
}

void HG51B::instructionJSR(uint8_t data, nall::Natural< 1> far, const nall::Natural< 1>& take) {
  if(!take) return;
  push();
  if(far) r.pb = r.p;
  r.pc = data;
  step(2);
}

void HG51B::instructionLD(nall::Natural<24>& out, nall::Natural< 7> reg) {
  out = readRegister(reg);
}

void HG51B::instructionLD(nall::Natural<15>& out, nall::Natural< 4> reg) {
  out = r.gpr[reg];
}

void HG51B::instructionLD(nall::Natural<24>& out, uint8_t imm) {
  out = imm;
}

void HG51B::instructionLD(nall::Natural<15>& out, uint8_t imm) {
  out = imm;
}

void HG51B::instructionLDL(nall::Natural<15>& out, uint8_t imm) {
  out = (out & 0x7f00) | imm << 0;
}

void HG51B::instructionLDH(nall::Natural<15>& out, nall::Natural< 7> imm) {
  out = (out & 0x00ff) | (imm & 0x7f) << 8;
}

void HG51B::instructionMUL(nall::Natural< 7> reg) {
  r.mul = algorithmMUL(r.a, readRegister(reg));
}

void HG51B::instructionMUL(uint8_t imm) {
  r.mul = algorithmMUL(r.a, imm);
}

void HG51B::instructionNOP() {
}

void HG51B::instructionOR(nall::Natural< 7> reg, nall::Natural< 5> shift) {
  r.a = algorithmOR(r.a << shift, readRegister(reg));
}

void HG51B::instructionOR(uint8_t imm, nall::Natural< 5> shift) {
  r.a = algorithmOR(r.a << shift, imm);
}

void HG51B::instructionRDRAM(nall::Natural< 2> byte, nall::Natural<24>& a) {
  nall::Natural<12> address = a;
  if(address >= 0xc00) address -= 0x400;
  r.ram.byte(byte) = dataRAM[address];
}

void HG51B::instructionRDRAM(nall::Natural< 2> byte, uint8_t imm) {
  nall::Natural<12> address = r.dpr + imm;
  if(address >= 0xc00) address -= 0x400;
  r.ram.byte(byte) = dataRAM[address];
}

void HG51B::instructionRDROM(nall::Natural<24>& reg) {
  r.rom = dataROM[(nall::Natural<10>)reg];
}

void HG51B::instructionRDROM(nall::Natural<10> imm) {
  r.rom = dataROM[imm];
}

void HG51B::instructionROR(nall::Natural< 7> reg) {
  r.a = algorithmROR(r.a, readRegister(reg));
}

void HG51B::instructionROR(nall::Natural< 5> imm) {
  r.a = algorithmROR(r.a, imm);
}

void HG51B::instructionRTS() {
  pull();
  step(2);
}

void HG51B::instructionSKIP(nall::Natural< 1> take, const nall::Natural< 1>& flag) {
  if(flag != take) return;
  advance();
  step(1);
}

void HG51B::instructionSHL(nall::Natural< 7> reg) {
  r.a = algorithmSHL(r.a, readRegister(reg));
}

void HG51B::instructionSHL(nall::Natural< 5> imm) {
  r.a = algorithmSHL(r.a, imm);
}

void HG51B::instructionSHR(nall::Natural< 7> reg) {
  r.a = algorithmSHR(r.a, readRegister(reg));
}

void HG51B::instructionSHR(nall::Natural< 5> imm) {
  r.a = algorithmSHR(r.a, imm);
}

void HG51B::instructionST(nall::Natural< 7> reg, nall::Natural<24>& in) {
  writeRegister(reg, in);
}

void HG51B::instructionSUB(nall::Natural< 7> reg, nall::Natural< 5> shift) {
  r.a = algorithmSUB(r.a << shift, readRegister(reg));
}

void HG51B::instructionSUB(uint8_t imm, nall::Natural< 5> shift) {
  r.a = algorithmSUB(r.a << shift, imm);
}

void HG51B::instructionSUBR(nall::Natural< 7> reg, nall::Natural< 5> shift) {
  r.a = algorithmSUB(readRegister(reg), r.a << shift);
}

void HG51B::instructionSUBR(uint8_t imm, nall::Natural< 5> shift) {
  r.a = algorithmSUB(imm, r.a << shift);
}

void HG51B::instructionSWAP(nall::Natural<24>& a, nall::Natural< 4> reg) {
  std::swap(a, r.gpr[reg]);
}

void HG51B::instructionSXB() {
  r.a = algorithmSX((int8_t)r.a) & 0xffffff;
}

void HG51B::instructionSXW() {
  r.a = algorithmSX((int16_t)r.a) & 0xffffff;
}

void HG51B::instructionWAIT() {
  if(!io.bus.enable) return;
  return step(io.bus.pending);
}

void HG51B::instructionWRRAM(nall::Natural< 2> byte, nall::Natural<24>& a) {
  nall::Natural<12> address = a;
  if(address >= 0xc00) address -= 0x400;
  dataRAM[address] = r.ram.byte(byte);
}

void HG51B::instructionWRRAM(nall::Natural< 2> byte, uint8_t imm) {
  nall::Natural<12> address = r.dpr + imm;
  if(address >= 0xc00) address -= 0x400;
  dataRAM[address] = r.ram.byte(byte);
}

void HG51B::instructionXNOR(nall::Natural< 7> reg, nall::Natural< 5> shift) {
  r.a = algorithmXNOR(r.a << shift, readRegister(reg)) & 0xffffff;
}

void HG51B::instructionXNOR(uint8_t imm, nall::Natural< 5> shift) {
  r.a = algorithmXNOR(r.a << shift, imm) & 0xffffff;
}

void HG51B::instructionXOR(nall::Natural< 7> reg, nall::Natural< 5> shift) {
  r.a = algorithmXOR(r.a << shift, readRegister(reg)) & 0xffffff;
}

void HG51B::instructionXOR(uint8_t imm, nall::Natural< 5> shift) {
  r.a = algorithmXOR(r.a << shift, imm) & 0xffffff;
}

void HG51B::serialize(serializer& s) {
  s.array(programRAM[0]);
  s.array(programRAM[1]);
  s.array(dataRAM);

  s.integer(r.pb);
  s.integer(r.pc);

  s.boolean(r.n);
  s.boolean(r.z);
  s.boolean(r.c);
  s.boolean(r.v);
  s.boolean(r.i);

  s.integer(r.a);
  s.integer(r.p);
  s.integer(r.mul);
  s.integer(r.mdr);
  s.integer(r.rom);
  s.integer(r.ram);
  s.integer(r.mar);
  s.integer(r.dpr);
  s.array(r.gpr);

  s.integer(io.lock);
  s.integer(io.halt);
  s.integer(io.irq);
  s.integer(io.rom);
  s.array(io.vector);

  s.integer(io.wait.rom);
  s.integer(io.wait.ram);

  s.integer(io.suspend.enable);
  s.integer(io.suspend.duration);

  s.integer(io.cache.enable);
  s.integer(io.cache.page);
  s.array(io.cache.lock);
  s.array(io.cache.address);
  s.integer(io.cache.base);
  s.integer(io.cache.pb);
  s.integer(io.cache.pc);

  s.integer(io.dma.enable);
  s.integer(io.dma.source);
  s.integer(io.dma.target);
  s.integer(io.dma.length);

  s.integer(io.bus.enable);
  s.integer(io.bus.reading);
  s.integer(io.bus.writing);
  s.integer(io.bus.pending);
  s.integer(io.bus.address);

  s.array(stack);
}

void HG51B::lock() {
  io.lock = 1;
}

void HG51B::halt() {
  io.halt = 1;
}

unsigned HG51B::wait(nall::Natural<24> address) {
  if(isROM(address)) return 1 + io.wait.rom;
  if(isRAM(address)) return 1 + io.wait.ram;
  return 1;
}

void HG51B::main() {
  if(io.lock) return step(1);
  if(io.suspend.enable) return suspend();
  if(io.cache.enable) return cache(), void();
  if(io.dma.enable) return dma();
  if(io.halt) return step(1);
  return execute();
}

void HG51B::step(unsigned clocks) {
  if(io.bus.enable) {
    if(io.bus.pending > clocks) {
      io.bus.pending = (io.bus.pending - clocks) & 0x0f;
    } else {
      io.bus.enable = 0;
      io.bus.pending = 0;
      if(io.bus.reading) io.bus.reading = 0, r.mdr = read(io.bus.address);
      if(io.bus.writing) io.bus.writing = 0, write(io.bus.address, r.mdr);
    }
  }
}

void HG51B::execute() {
  if(!cache()) return halt();
  auto opcode = programRAM[io.cache.page][r.pc];
  advance();
  step(1);
  instructionTable[opcode]();
}

void HG51B::advance() {
  if(++r.pc == 0) {
    if(io.cache.page == 1) return halt();
    io.cache.page = 1;
    if(io.cache.lock[io.cache.page]) return halt();
    r.pb = r.p;
    if(!cache()) return halt();
  }
}

void HG51B::suspend() {
  if(!io.suspend.duration) return step(1);  //indefinite
  step(io.suspend.duration);
  io.suspend.duration = 0;
  io.suspend.enable = 0;
}

bool HG51B::cache() {
  uint32_t address = (io.cache.base + r.pb * 512) & 0xffffff;

  //try to use the current page ...
  if(io.cache.address[io.cache.page] == address) return io.cache.enable = 0, true;
  //if it's not valid, try to use the other page ...
  io.cache.page ^= 1;
  if(io.cache.address[io.cache.page] == address) return io.cache.enable = 0, true;
  //if it's not valid, try to load into the other page ...
  if(io.cache.lock[io.cache.page]) io.cache.page ^= 1;
  //if it's locked, try to load into the first page ...
  if(io.cache.lock[io.cache.page]) return io.cache.enable = 0, false;

  io.cache.address[io.cache.page] = address;
  for(unsigned offset : nall::range(256)) {
    step(wait(address));
    programRAM[io.cache.page][offset]  = read(address++) << 0;
    programRAM[io.cache.page][offset] |= read(address++) << 8;
  }
  return io.cache.enable = 0, true;
}

void HG51B::dma() {
  for(unsigned offset : nall::range(io.dma.length)) {
    uint32_t source = (io.dma.source + offset) & 0xffffff;
    uint32_t target = (io.dma.target + offset) & 0xffffff;

    if(isROM(source) && isROM(target)) return lock();
    if(isRAM(source) && isRAM(target)) return lock();

    step(wait(source));
    auto data = read(source);

    step(wait(target));
    write(target, data);
  }

  io.dma.enable = 0;
}

bool HG51B::running() const {
  return io.cache.enable || io.dma.enable || io.bus.pending || !io.halt;
}

bool HG51B::busy() const {
  return io.cache.enable || io.dma.enable || io.bus.pending;
}

void HG51B::power() {
  r = {};
  io = {};
}

}
