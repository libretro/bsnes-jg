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
  case 0x28: r.p = data & 0x7fff; return;
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
  #define pattern(s) \
    std::integral_constant<uint16_t, s>::value
    //std::integral_constant<uint16_t, nall::test(s)>::value

  static const uint8_t shifts[] = {0, 1, 8, 16};

  //NOP
  for(unsigned null = 0; null < 1024; ++null) {
    //auto opcode = pattern("0000 00.. .... ....");
    auto opcode = pattern(0x0000) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionNOP(); };
  }

  //???
  for(unsigned null = 0; null < 1024; ++null) {
    //auto opcode = pattern("0000 01.. .... ....");
    auto opcode = pattern(0x0400) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionNOP(); };
  }

  //JMP imm
  for(unsigned data = 0; data < 256; ++data)
  for(unsigned null = 0; null < 2; ++null)
  for(unsigned far = 0; far < 2; ++far) {
    //auto opcode = pattern("0000 10f. dddd dddd");
    auto opcode = pattern(0x0800) | data << 0 | null << 8 | far << 9;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionJMP(data, far, 1); };
  }

  //JMP EQ,imm
  for(unsigned data = 0; data < 256; ++data)
  for(unsigned null = 0; null < 2; ++null)
  for(unsigned far = 0; far < 2; ++far) {
    //auto opcode = pattern("0000 11f. dddd dddd");
    auto opcode = pattern(0x0c00) | data << 0 | null << 8 | far << 9;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionJMP(data, far, r.z); };
  }

  //JMP GE,imm
  for(unsigned data = 0; data < 256; ++data)
  for(unsigned null = 0; null < 2; ++null)
  for(unsigned far = 0; far < 2; ++far) {
    //auto opcode = pattern("0001 00f. dddd dddd");
    auto opcode = pattern(0x1000) | data << 0 | null << 8 | far << 9;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionJMP(data, far, r.c); };
  }

  //JMP MI,imm
  for(unsigned data = 0; data < 256; ++data)
  for(unsigned null = 0; null < 2; ++null)
  for(unsigned far = 0; far < 2; ++far) {
    //auto opcode = pattern("0001 01f. dddd dddd");
    auto opcode = pattern(0x1400) | data << 0 | null << 8 | far << 9;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionJMP(data, far, r.n); };
  }

  //JMP VS,imm
  for(unsigned data = 0; data < 256; ++data)
  for(unsigned null = 0; null < 2; ++null)
  for(unsigned far = 0; far < 2; ++far) {
    //auto opcode = pattern("0001 10f. dddd dddd");
    auto opcode = pattern(0x1800) | data << 0 | null << 8 | far << 9;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionJMP(data, far, r.v); };
  }

  //WAIT
  for(unsigned null = 0; null < 1024; ++null) {
    //auto opcode = pattern("0001 11.. .... ....");
    auto opcode = pattern(0x1c00) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionWAIT(); };
  }

  //???
  for(unsigned null = 0; null < 1024; ++null) {
    //auto opcode = pattern("0010 00.. .... ....");
    auto opcode = pattern(0x2000) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionNOP(); };
  }

  //SKIP V
  for(unsigned take = 0; take < 2; ++take)
  for(unsigned null = 0; null < 128; ++null) {
    //auto opcode = pattern("0010 0100 .... ...t");
    auto opcode = pattern(0x2400) | take << 0 | null << 1;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionSKIP(take, r.v); };
  }

  //SKIP C
  for(unsigned take = 0; take < 2; ++take)
  for(unsigned null = 0; null < 128; ++null) {
    //auto opcode = pattern("0010 0101 .... ...t");
    auto opcode = pattern(0x2500) | take << 0 | null << 1;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionSKIP(take, r.c); };
  }

  //SKIP Z
  for(unsigned take = 0; take < 2; ++take)
  for(unsigned null = 0; null < 128; ++null) {
    //auto opcode = pattern("0010 0110 .... ...t");
    auto opcode = pattern(0x2600) | take << 0 | null << 1;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionSKIP(take, r.z); };
  }

  //SKIP N
  for(unsigned take = 0; take < 2; ++take)
  for(unsigned null = 0; null < 128; ++null) {
    //auto opcode = pattern("0010 0111 .... ...t");
    auto opcode = pattern(0x2700) | take << 0 | null << 1;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionSKIP(take, r.n); };
  }

  //JSR
  for(unsigned data = 0; data < 256; ++data)
  for(unsigned null = 0; null < 2; ++null)
  for(unsigned far = 0; far < 2; ++far) {
    //auto opcode = pattern("0010 10f. dddd dddd");
    auto opcode = pattern(0x2800) | data << 0 | null << 8 | far << 9;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionJSR(data, far, 1); };
  }

  //JSR EQ,imm
  for(unsigned data = 0; data < 256; ++data)
  for(unsigned null = 0; null < 2; ++null)
  for(unsigned far = 0; far < 2; ++far) {
    //auto opcode = pattern("0010 11f. dddd dddd");
    auto opcode = pattern(0x2c00) | data << 0 | null << 8 | far << 9;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionJSR(data, far, r.z); };
  }

  //JSR GE,imm
  for(unsigned data = 0; data < 256; ++data)
  for(unsigned null = 0; null < 2; ++null)
  for(unsigned far = 0; far < 2; ++far) {
    //auto opcode = pattern("0011 00f. dddd dddd");
    auto opcode = pattern(0x3000) | data << 0 | null << 8 | far << 9;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionJSR(data, far, r.c); };
  }

  //JSR MI,imm
  for(unsigned data = 0; data < 256; ++data)
  for(unsigned null = 0; null < 2; ++null)
  for(unsigned far = 0; far < 2; ++far) {
    //auto opcode = pattern("0011 01f. dddd dddd");
    auto opcode = pattern(0x3400) | data << 0 | null << 8 | far << 9;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionJSR(data, far, r.n); };
  }

  //JSR VS,imm
  for(unsigned data = 0; data < 256; ++data)
  for(unsigned null = 0; null < 2; ++null)
  for(unsigned far = 0; far < 2; ++far) {
    //auto opcode = pattern("0011 10f. dddd dddd");
    auto opcode = pattern(0x3800) | data << 0 | null << 8 | far << 9;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionJSR(data, far, r.v); };
  }

  //RTS
  for(unsigned null = 0; null < 1024; ++null) {
    //auto opcode = pattern("0011 11.. .... ....");
    auto opcode = pattern(0x3c00) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionRTS(); };
  }

  //INC MAR
  for(unsigned null = 0; null < 1024; ++null) {
    //auto opcode = pattern("0100 00.. .... ....");
    auto opcode = pattern(0x4000) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionINC(r.mar); };
  }

  //???
  for(unsigned null = 0; null < 1024; ++null) {
    //auto opcode = pattern("0100 01.. .... ....");
    auto opcode = pattern(0x4400) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionNOP(); };
  }

  //CMPR A<<s,reg
  for(unsigned reg = 0; reg < 128; ++reg)
  for(unsigned null = 0; null < 2; ++null)
  for(unsigned shift = 0; shift < 4; ++shift) {
    //auto opcode = pattern("0100 10ss .rrr rrrr");
    auto opcode = pattern(0x4800) | reg << 0 | null << 7 | shift << 8;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionCMPRr(reg, shifts[shift]); };
  }

  //CMPR A<<s,imm
  for(unsigned imm = 0; imm < 256; ++imm)
  for(unsigned shift = 0; shift < 4; ++shift) {
    //auto opcode = pattern("0100 11ss iiii iiii");
    auto opcode = pattern(0x4c00) | imm << 0 | shift << 8;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionCMPR(imm, shifts[shift]); };
  }

  //CMP A<<s,reg
  for(unsigned reg = 0; reg < 128; ++reg)
  for(unsigned null = 0; null < 2; ++null)
  for(unsigned shift = 0; shift < 4; ++shift) {
    //auto opcode = pattern("0101 00ss .rrr rrrr");
    auto opcode = pattern(0x5000) | reg << 0 | null << 7 | shift << 8;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionCMPr(reg, shifts[shift]); };
  }

  //CMP A<<s,imm
  for(unsigned imm = 0; imm < 256; ++imm)
  for(unsigned shift = 0; shift < 4; ++shift) {
    //auto opcode = pattern("0101 01ss iiii iiii");
    auto opcode = pattern(0x5400) | imm << 0 | shift << 8;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionCMP(imm, shifts[shift]); };
  }

  //???
  for(unsigned null = 0; null < 256; ++null) {
    //auto opcode = pattern("0101 1000 .... ....");
    auto opcode = pattern(0x5800) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionNOP(); };
  }

  //SXB A
  for(unsigned null = 0; null < 256; ++null) {
    //auto opcode = pattern("0101 1001 .... ....");
    auto opcode = pattern(0x5900) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionSXB(); };
  }

  //SXW A
  for(unsigned null = 0; null < 256; ++null) {
    //auto opcode = pattern("0101 1010 .... ....");
    auto opcode = pattern(0x5a00) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionSXW(); };
  }

  //???
  for(unsigned null = 0; null < 256; ++null) {
    //auto opcode = pattern("0101 1011 .... ....");
    auto opcode = pattern(0x5b00) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionNOP(); };
  }

  //???
  for(unsigned null = 0; null < 1024; ++null) {
    //auto opcode = pattern("0101 11.. .... ....");
    auto opcode = pattern(0x5c00) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionNOP(); };
  }

  //LD A,reg
  for(unsigned reg = 0; reg < 128; ++reg)
  for(unsigned null = 0; null < 2; ++null) {
    //auto opcode = pattern("0110 0000 .rrr rrrr");
    auto opcode = pattern(0x6000) | reg << 0 | null << 7;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionLDr(r.a, reg); };
  }

  //LD MDR,reg
  for(unsigned reg = 0; reg < 128; ++reg)
  for(unsigned null = 0; null < 2; ++null) {
    //auto opcode = pattern("0110 0001 .rrr rrrr");
    auto opcode = pattern(0x6100) | reg << 0 | null << 7;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionLDr(r.mdr, reg); };
  }

  //LD MAR,reg
  for(unsigned reg = 0; reg < 128; ++reg)
  for(unsigned null = 0; null < 2; ++null) {
    //auto opcode = pattern("0110 0010 .rrr rrrr");
    auto opcode = pattern(0x6200) | reg << 0 | null << 7;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionLDr(r.mar, reg); };
  }

  //LD P,reg
  for(unsigned reg = 0; reg < 16; ++reg)
  for(unsigned null = 0; null < 16; ++null) {
    //auto opcode = pattern("0110 0011 .... rrrr");
    auto opcode = pattern(0x6300) | reg << 0 | null << 4;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionLDr(r.p, reg); };
  }

  //LD A,imm
  for(unsigned imm = 0; imm < 256; ++imm) {
    //auto opcode = pattern("0110 0100 iiii iiii");
    auto opcode = pattern(0x6400) | imm << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionLD(r.a, imm); };
  }

  //LD MDR,imm
  for(unsigned imm = 0; imm < 256; ++imm) {
    //auto opcode = pattern("0110 0101 iiii iiii");
    auto opcode = pattern(0x6500) | imm << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionLD(r.mdr, imm); };
  }

  //LD MAR,imm
  for(unsigned imm = 0; imm < 256; ++imm) {
    //auto opcode = pattern("0110 0110 iiii iiii");
    auto opcode = pattern(0x6600) | imm << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionLD(r.mar, imm); };
  }

  //LD P,imm
  for(unsigned imm = 0; imm < 256; ++imm) {
    //auto opcode = pattern("0110 0111 iiii iiii");
    auto opcode = pattern(0x6700) | imm << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionLD(r.p, imm); };
  }

  //RDRAM 0,A
  for(unsigned null = 0; null < 256; ++null) {
    //auto opcode = pattern("0110 1000 .... ....");
    auto opcode = pattern(0x6800) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionRDRAM(0, r.a); };
  }

  //RDRAM 1,A
  for(unsigned null = 0; null < 256; ++null) {
    //auto opcode = pattern("0110 1001 .... ....");
    auto opcode = pattern(0x6900) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionRDRAM(1, r.a); };
  }

  //RDRAM 2,A
  for(unsigned null = 0; null < 256; ++null) {
    //auto opcode = pattern("0110 1010 .... ....");
    auto opcode = pattern(0x6a00) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionRDRAM(2, r.a); };
  }

  //???
  for(unsigned null = 0; null < 256; ++null) {
    //auto opcode = pattern("0110 1011 .... ....");
    auto opcode = pattern(0x6b00) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionNOP(); };
  }

  //RDRAM 0,imm
  for(unsigned imm = 0; imm < 256; ++imm) {
    //auto opcode = pattern("0110 1100 iiii iiii");
    auto opcode = pattern(0x6c00) | imm << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionRDRAM(0, imm); };
  }

  //RDRAM 1,imm
  for(unsigned imm = 0; imm < 256; ++imm) {
    //auto opcode = pattern("0110 1101 iiii iiii");
    auto opcode = pattern(0x6d00) | imm << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionRDRAM(1, imm); };
  }

  //RDRAM 2,imm
  for(unsigned imm = 0; imm < 256; ++imm) {
    //auto opcode = pattern("0110 1110 iiii iiii");
    auto opcode = pattern(0x6e00) | imm << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionRDRAM(2, imm); };
  }

  //???
  for(unsigned null = 0; null < 256; ++null) {
    //auto opcode = pattern("0110 1111 .... ....");
    auto opcode = pattern(0x6f00) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionNOP(); };
  }

  //RDROM A
  for(unsigned null = 0; null < 1024; ++null) {
    //auto opcode = pattern("0111 00.. .... ....");
    auto opcode = pattern(0x7000) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionRDROM(r.a); };
  }

  //RDROM imm
  for(unsigned imm = 0; imm < 1024; ++imm) {
    //auto opcode = pattern("0111 01ii iiii iiii");
    auto opcode = pattern(0x7400) | imm << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionRDROM(imm); };
  }

  //???
  for(unsigned null = 0; null < 1024; ++null) {
    //auto opcode = pattern("0111 10.. .... ....");
    auto opcode = pattern(0x7800) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionNOP(); };
  }

  //LD PL,imm
  for(unsigned imm = 0; imm < 256; ++imm) {
    //auto opcode = pattern("0111 1100 iiii iiii");
    auto opcode = pattern(0x7c00) | imm << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionLDL(r.p, imm); };
  }

  //LD PH,imm
  for(unsigned imm = 0; imm < 128; ++imm)
  for(unsigned null = 0; null < 2; ++null) {
    //auto opcode = pattern("0111 1101 .iii iiii");
    auto opcode = pattern(0x7d00) | imm << 0 | null << 7;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionLDHr(r.p, imm); };
  }

  //???
  for(unsigned null = 0; null < 512; ++null) {
    //auto opcode = pattern("0111 111. .... ....");
    auto opcode = pattern(0x7e00) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionNOP(); };
  }

  //ADD A<<s,reg
  for(unsigned reg = 0; reg < 128; ++reg)
  for(unsigned null = 0; null < 2; ++null)
  for(unsigned shift = 0; shift < 4; ++shift) {
    //auto opcode = pattern("1000 00ss .rrr rrrr");
    auto opcode = pattern(0x8000) | reg << 0 | null << 7 | shift << 8;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionADDr(reg, shifts[shift]); };
  }

  //ADD A<<s,imm
  for(unsigned imm = 0; imm < 256; ++imm)
  for(unsigned shift = 0; shift < 4; ++shift) {
    //auto opcode = pattern("1000 01ss iiii iiii");
    auto opcode = pattern(0x8400) | imm << 0 | shift << 8;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionADD(imm, shifts[shift]); };
  }

  //SUBR A<<s,reg
  for(unsigned reg = 0; reg < 128; ++reg)
  for(unsigned null = 0; null < 2; ++null)
  for(unsigned shift = 0; shift < 4; ++shift) {
    //auto opcode = pattern("1000 10ss .rrr rrrr");
    auto opcode = pattern(0x8800) | reg << 0 | null << 7 | shift << 8;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionSUBRr(reg, shifts[shift]); };
  }

  //SUBR A<<s,imm
  for(unsigned imm = 0; imm < 256; ++imm)
  for(unsigned shift = 0; shift < 4; ++shift) {
    //auto opcode = pattern("1000 11ss iiii iiii");
    auto opcode = pattern(0x8c00) | imm << 0 | shift << 8;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionSUBR(imm, shifts[shift]); };
  }

  //SUB A<<s,reg
  for(unsigned reg = 0; reg < 128; ++reg)
  for(unsigned null = 0; null < 2; ++null)
  for(unsigned shift = 0; shift < 4; ++shift) {
    //auto opcode = pattern("1001 00ss .rrr rrrr");
    auto opcode = pattern(0x9000) | reg << 0 | null << 7 | shift << 8;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionSUBr(reg, shifts[shift]); };
  }

  //SUB A<<s,imm
  for(unsigned imm = 0; imm < 256; ++imm)
  for(unsigned shift = 0; shift < 4; ++shift) {
    //auto opcode = pattern("1001 01ss iiii iiii");
    auto opcode = pattern(0x9400) | imm << 0 | shift << 8;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionSUB(imm, shifts[shift]); };
  }

  //MUL reg
  for(unsigned reg = 0; reg < 128; ++reg)
  for(unsigned null = 0; null < 8; ++null) {
    //auto opcode = pattern("1001 10.. .rrr rrrr");
    auto opcode = pattern(0x9800) | reg << 0 | null << 7;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionMULr(reg); };
  }

  //MUL imm
  for(unsigned imm = 0; imm < 256; ++imm)
  for(unsigned null = 0; null < 4; ++null) {
    //auto opcode = pattern("1001 11.. iiii iiii");
    auto opcode = pattern(0x9c00) | imm << 0 | null << 8;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionMUL(imm); };
  }

  //XNOR A<<s,reg
  for(unsigned reg = 0; reg < 128; ++reg)
  for(unsigned null = 0; null < 2; ++null)
  for(unsigned shift = 0; shift < 4; ++shift) {
    //auto opcode = pattern("1010 00ss .rrr rrrr");
    auto opcode = pattern(0xa000) | reg << 0 | null << 7 | shift << 8;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionXNORr(reg, shifts[shift]); };
  }

  //XNOR A<<s,imm
  for(unsigned imm = 0; imm < 256; ++imm)
  for(unsigned shift = 0; shift < 4; ++shift) {
    //auto opcode = pattern("1010 01ss iiii iiii");
    auto opcode = pattern(0xa400) | imm << 0 | shift << 8;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionXNOR(imm, shifts[shift]); };
  }

  //XOR A<<s,reg
  for(unsigned reg = 0; reg < 128; ++reg)
  for(unsigned null = 0; null < 2; ++null)
  for(unsigned shift = 0; shift < 4; ++shift) {
    //auto opcode = pattern("1010 10ss .rrr rrrr");
    auto opcode = pattern(0xa800) | reg << 0 | null << 7 | shift << 8;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionXORr(reg, shifts[shift]); };
  }

  //XOR A<<s,imm
  for(unsigned imm = 0; imm < 256; ++imm)
  for(unsigned shift = 0; shift < 4; ++shift) {
    //auto opcode = pattern("1010 11ss iiii iiii");
    auto opcode = pattern(0xac00) | imm << 0 | shift << 8;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionXOR(imm, shifts[shift]); };
  }

  //AND A<<s,reg
  for(unsigned reg = 0; reg < 128; ++reg)
  for(unsigned null = 0; null < 2; ++null)
  for(unsigned shift = 0; shift < 4; ++shift) {
    //auto opcode = pattern("1011 00ss .rrr rrrr");
    auto opcode = pattern(0xb000) | reg << 0 | null << 7 | shift << 8;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionANDr(reg, shifts[shift]); };
  }

  //AND A<<s,imm
  for(unsigned imm = 0; imm < 256; ++imm)
  for(unsigned shift = 0; shift < 4; ++shift) {
    //auto opcode = pattern("1011 01ss iiii iiii");
    auto opcode = pattern(0xb400) | imm << 0 | shift << 8;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionAND(imm, shifts[shift]); };
  }

  //OR A<<s,reg
  for(unsigned reg = 0; reg < 128; ++reg)
  for(unsigned null = 0; null < 2; ++null)
  for(unsigned shift = 0; shift < 4; ++shift) {
    //auto opcode = pattern("1011 10ss .rrr rrrr");
    auto opcode = pattern(0xb800) | reg << 0 | null << 7 | shift << 8;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionORr(reg, shifts[shift]); };
  }

  //OR A<<s,imm
  for(unsigned imm = 0; imm < 256; ++imm)
  for(unsigned shift = 0; shift < 4; ++shift) {
    //auto opcode = pattern("1011 11ss iiii iiii");
    auto opcode = pattern(0xbc00) | imm << 0 | shift << 8;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionOR(imm, shifts[shift]); };
  }

  //SHR A,reg
  for(unsigned reg = 0; reg < 128; ++reg)
  for(unsigned null = 0; null < 8; ++null) {
    //auto opcode = pattern("1100 00.. .rrr rrrr");
    auto opcode = pattern(0xc000) | reg << 0 | null << 7;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionSHRr(reg); };
  }

  //SHR A,imm
  for(unsigned imm = 0; imm < 32; ++imm)
  for(unsigned null = 0; null < 32; ++null) {
    //auto opcode = pattern("1100 01.. ...i iiii");
    auto opcode = pattern(0xc400) | imm << 0 | null << 5;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionSHR(imm); };
  }

  //ASR A,reg
  for(unsigned reg = 0; reg < 128; ++reg)
  for(unsigned null = 0; null < 8; ++null) {
    //auto opcode = pattern("1100 10.. .rrr rrrr");
    auto opcode = pattern(0xc800) | reg << 0 | null << 7;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionASRr(reg); };
  }

  //ASR A,imm
  for(unsigned imm = 0; imm < 32; ++imm)
  for(unsigned null = 0; null < 32; ++null) {
    //auto opcode = pattern("1100 11.. ...i iiii");
    auto opcode = pattern(0xcc00) | imm << 0 | null << 5;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionASR(imm); };
  }

  //ROR A,reg
  for(unsigned reg = 0; reg < 128; ++reg)
  for(unsigned null = 0; null < 8; ++null) {
    //auto opcode = pattern("1101 00.. .rrr rrrr");
    auto opcode = pattern(0xd000) | reg << 0 | null << 7;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionRORr(reg); };
  }

  //ROR A,imm
  for(unsigned imm = 0; imm < 32; ++imm)
  for(unsigned null = 0; null < 32; ++null) {
    //auto opcode = pattern("1101 01.. ...i iiii");
    auto opcode = pattern(0xd400) | imm << 0 | null << 5;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionROR(imm); };
  }

  //SHL A,reg
  for(unsigned reg = 0; reg < 128; ++reg)
  for(unsigned null = 0; null < 8; ++null) {
    //auto opcode = pattern("1101 10.. .rrr rrrr");
    auto opcode = pattern(0xd800) | reg << 0 | null << 7;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionSHLr(reg); };
  }

  //SHL A,imm
  for(unsigned imm = 0; imm < 32; ++imm)
  for(unsigned null = 0; null < 32; ++null) {
    //auto opcode = pattern("1101 11.. ...i iiii");
    auto opcode = pattern(0xdc00) | imm << 0 | null << 5;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionSHL(imm); };
  }

  //ST reg,A
  for(unsigned reg = 0; reg < 128; ++reg)
  for(unsigned null = 0; null < 2; ++null) {
    //auto opcode = pattern("1110 0000 .rrr rrrr");
    auto opcode = pattern(0xe000) | reg << 0 | null << 7;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionSTr(reg, r.a); };
  }

  //ST reg,MDR
  for(unsigned reg = 0; reg < 128; ++reg)
  for(unsigned null = 0; null < 2; ++null) {
    //auto opcode = pattern("1110 0001 .rrr rrrr");
    auto opcode = pattern(0xe100) | reg << 0 | null << 7;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionSTr(reg, r.mdr); };
  }

  //???
  for(unsigned null = 0; null < 512; ++null) {
    //auto opcode = pattern("1110 001. .... ....");
    auto opcode = pattern(0xe200) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionNOP(); };
  }

  //???
  for(unsigned null = 0; null < 1024; ++null) {
    //auto opcode = pattern("1110 01.. .... ....");
    auto opcode = pattern(0xe400) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionNOP(); };
  }

  //WRRAM 0,A
  for(unsigned null = 0; null < 256; ++null) {
    //auto opcode = pattern("1110 1000 .... ....");
    auto opcode = pattern(0xe800) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionWRRAM(0, r.a); };
  }

  //WRRAM 1,A
  for(unsigned null = 0; null < 256; ++null) {
    //auto opcode = pattern("1110 1001 .... ....");
    auto opcode = pattern(0xe900) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionWRRAM(1, r.a); };
  }

  //WRRAM 2,A
  for(unsigned null = 0; null < 256; ++null) {
    //auto opcode = pattern("1110 1010 .... ....");
    auto opcode = pattern(0xea00) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionWRRAM(2, r.a); };
  }

  //???
  for(unsigned null = 0; null < 256; ++null) {
    //auto opcode = pattern("1110 1011 .... ....");
    auto opcode = pattern(0xeb00) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionNOP(); };
  }

  //WRRAM 0,imm
  for(unsigned imm = 0; imm < 256; ++imm) {
    //auto opcode = pattern("1110 1100 iiii iiii");
    auto opcode = pattern(0xec00) | imm << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionWRRAM(0, imm); };
  }

  //WRRAM 1,imm
  for(unsigned imm = 0; imm < 256; ++imm) {
    //auto opcode = pattern("1110 1101 iiii iiii");
    auto opcode = pattern(0xed00) | imm << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionWRRAM(1, imm); };
  }

  //WRRAM 2,imm
  for(unsigned imm = 0; imm < 256; ++imm) {
    //auto opcode = pattern("1110 1110 iiii iiii");
    auto opcode = pattern(0xee00) | imm << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionWRRAM(2, imm); };
  }

  //???
  for(unsigned null = 0; null < 256; ++null) {
    //auto opcode = pattern("1110 1111 .... ....");
    auto opcode = pattern(0xef00) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionNOP(); };
  }

  //SWAP A,Rn
  for(unsigned reg = 0; reg < 16; ++reg)
  for(unsigned null = 0; null < 64; ++null) {
    //auto opcode = pattern("1111 00.. .... rrrr");
    auto opcode = pattern(0xf000) | reg << 0 | null << 4;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionSWAPr(r.a, reg); };
  }

  //???
  for(unsigned null = 0; null < 1024; ++null) {
    //auto opcode = pattern("1111 01.. .... ....");
    auto opcode = pattern(0xf400) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionNOP(); };
  }

  //CLEAR
  for(unsigned null = 0; null < 1024; ++null) {
    //auto opcode = pattern("1111 10.. .... ....");
    auto opcode = pattern(0xf800) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionCLEAR(); };
  }

  //HALT
  for(unsigned null = 0; null < 1024; ++null) {
    //auto opcode = pattern("1111 11.. .... ....");
    auto opcode = pattern(0xfc00) | null << 0;
    if(instructionTable[opcode]) throw;
    instructionTable[opcode] = [=] { return instructionHALT(); };
  }

  #undef pattern

  for(unsigned opcode = 0; opcode < 65536; ++opcode) {
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

  r.pb = (pc >> 8) & 0x7fff;
  r.pc = pc >> 0;
}

uint32_t HG51B::algorithmADD(uint32_t x, uint32_t y) {
  int z = x + y;
  r.n = z & 0x800000;
  r.z = (z & 0xffffff) == 0;
  r.c = z > 0xffffff;
  r.v = ~(x ^ y) & (x ^ z) & 0x800000;
  return z & 0xffffff;
}

uint32_t HG51B::algorithmAND(uint32_t x, uint32_t y) {
  x = x & y;
  r.n = x & 0x800000;
  r.z = x == 0;
  return x & 0xffffff;
}

uint32_t HG51B::algorithmASR(uint32_t a, uint8_t s) {
  if(s > 24) s = 0;
  a = signextend<int32_t,24>(a) >> s;
  r.n = a & 0x800000;
  r.z = a == 0;
  return a & 0xffffff;
}

uint64_t HG51B::algorithmMUL(uint32_t x, uint32_t y) {
  return (signextend<int64_t,24>(x) * signextend<int64_t,24>(y)) & 0xffffffffffff;
}

uint32_t HG51B::algorithmOR(uint32_t x, uint32_t y) {
  x = x | y;
  r.n = x & 0x800000;
  r.z = x == 0;
  return x & 0xffffff;
}

uint32_t HG51B::algorithmROR(uint32_t a, uint8_t s) {
  if(s > 24) s = 0;
  a = (a >> s) | (a << (24 - s));
  r.n = a & 0x800000;
  r.z = a == 0;
  return a & 0xffffff;
}

uint32_t HG51B::algorithmSHL(uint32_t a, uint8_t s) {
  if(s > 24) s = 0;
  a = a << s;
  r.n = a & 0x800000;
  r.z = a == 0;
  return a & 0xffffff;
}

uint32_t HG51B::algorithmSHR(uint32_t a, uint8_t s) {
  if(s > 24) s = 0;
  a = a >> s;
  r.n = a & 0x800000;
  r.z = a == 0;
  return a & 0xffffff;
}

uint32_t HG51B::algorithmSUB(uint32_t x, uint32_t y) {
  int z = x - y;
  r.n = z & 0x800000;
  r.z = (z & 0xffffff) == 0;
  r.c = z >= 0;
  r.v = ~(x ^ y) & (x ^ z) & 0x800000;
  return z & 0xffffff;
}

uint32_t HG51B::algorithmSX(uint32_t x) {
  r.n = x & 0x800000;
  r.z = x == 0;
  return x & 0xffffff;
}

uint32_t HG51B::algorithmXNOR(uint32_t x, uint32_t y) {
  x = ~x ^ y;
  r.n = x & 0x800000;
  r.z = x == 0;
  return x & 0xffffff;
}

uint32_t HG51B::algorithmXOR(uint32_t x, uint32_t y) {
  x = x ^ y;
  r.n = x & 0x800000;
  r.z = x == 0;
  return x & 0xffffff;
}

void HG51B::instructionADDr(uint8_t reg, uint8_t shift) {
  r.a = algorithmADD(r.a << shift, readRegister(reg));
}

void HG51B::instructionADD(uint8_t imm, uint8_t shift) {
  r.a = algorithmADD(r.a << shift, imm);
}

void HG51B::instructionANDr(uint8_t reg, uint8_t shift) {
  r.a = algorithmAND(r.a << shift, readRegister(reg));
}

void HG51B::instructionAND(uint8_t imm, uint8_t shift) {
  r.a = algorithmAND(r.a << shift, imm);
}

void HG51B::instructionASRr(uint8_t reg) {
  r.a = algorithmASR(r.a, readRegister(reg));
}

void HG51B::instructionASR(uint8_t imm) {
  r.a = algorithmASR(r.a, imm);
}

void HG51B::instructionCLEAR() {
  r.a = 0;
  r.p = 0;
  r.ram = 0;
  r.dpr = 0;
}

void HG51B::instructionCMPr(uint8_t reg, uint8_t shift) {
  algorithmSUB(r.a << shift, readRegister(reg));
}

void HG51B::instructionCMP(uint8_t imm, uint8_t shift) {
  algorithmSUB(r.a << shift, imm);
}

void HG51B::instructionCMPRr(uint8_t reg, uint8_t shift) {
  algorithmSUB(readRegister(reg), r.a << shift);
}

void HG51B::instructionCMPR(uint8_t imm, uint8_t shift) {
  algorithmSUB(imm, r.a << shift);
}

void HG51B::instructionHALT() {
  halt();
}

void HG51B::instructionINC(uint32_t& reg) {
  reg++;
}

void HG51B::instructionJMP(uint8_t data, uint8_t far, const uint8_t& take) {
  if(!take) return;
  if(far) r.pb = r.p;
  r.pc = data;
  step(2);
}

void HG51B::instructionJSR(uint8_t data, uint8_t far, const uint8_t& take) {
  if(!take) return;
  push();
  if(far) r.pb = r.p;
  r.pc = data;
  step(2);
}

void HG51B::instructionLDr(uint32_t& out, uint8_t reg) {
  out = readRegister(reg);
}

void HG51B::instructionLDr(uint16_t& out, uint8_t reg) {
  out = r.gpr[reg];
}

void HG51B::instructionLD(uint32_t& out, uint8_t imm) {
  out = imm;
}

void HG51B::instructionLD(uint16_t& out, uint8_t imm) {
  out = imm;
}

void HG51B::instructionLDL(uint16_t& out, uint8_t imm) {
  out = (out & 0x7f00) | imm << 0;
}

void HG51B::instructionLDHr(uint16_t& out, uint8_t imm) {
  out = (out & 0x00ff) | (imm & 0x7f) << 8;
}

void HG51B::instructionMULr(uint8_t reg) {
  r.mul = algorithmMUL(r.a, readRegister(reg));
}

void HG51B::instructionMUL(uint8_t imm) {
  r.mul = algorithmMUL(r.a, imm);
}

void HG51B::instructionNOP() {
}

void HG51B::instructionORr(uint8_t reg, uint8_t shift) {
  r.a = algorithmOR(r.a << shift, readRegister(reg));
}

void HG51B::instructionOR(uint8_t imm, uint8_t shift) {
  r.a = algorithmOR(r.a << shift, imm);
}

void HG51B::instructionRDRAM(uint8_t byte, uint32_t& a) {
  uint16_t address = a & 0xfff;
  if(address >= 0xc00) address -= 0x400;
  uint8_t shift = byte << 3;
  uint32_t mask = 0xff << shift;
  r.ram &= ~mask;
  r.ram |= (dataRAM[address] << shift);
}

void HG51B::instructionRDRAM(uint8_t byte, uint8_t imm) {
  uint16_t address = (r.dpr + imm) & 0xfff;
  if(address >= 0xc00) address -= 0x400;
  uint8_t shift = byte << 3;
  uint32_t mask = 0xff << shift;
  r.ram &= ~mask;
  r.ram |= (dataRAM[address] << shift);
}

void HG51B::instructionRDROM(uint32_t& reg) {
  r.rom = dataROM[reg & 0x3ff];
}

void HG51B::instructionRDROM(uint16_t imm) {
  r.rom = dataROM[imm];
}

void HG51B::instructionRORr(uint8_t reg) {
  r.a = algorithmROR(r.a, readRegister(reg));
}

void HG51B::instructionROR(uint8_t imm) {
  r.a = algorithmROR(r.a, imm);
}

void HG51B::instructionRTS() {
  pull();
  step(2);
}

void HG51B::instructionSKIP(uint8_t take, const uint8_t& flag) {
  if(flag != take) return;
  advance();
  step(1);
}

void HG51B::instructionSHLr(uint8_t reg) {
  r.a = algorithmSHL(r.a, readRegister(reg));
}

void HG51B::instructionSHL(uint8_t imm) {
  r.a = algorithmSHL(r.a, imm);
}

void HG51B::instructionSHRr(uint8_t reg) {
  r.a = algorithmSHR(r.a, readRegister(reg));
}

void HG51B::instructionSHR(uint8_t imm) {
  r.a = algorithmSHR(r.a, imm);
}

void HG51B::instructionSTr(uint8_t reg, uint32_t& in) {
  writeRegister(reg, in);
}

void HG51B::instructionSUBr(uint8_t reg, uint8_t shift) {
  r.a = algorithmSUB(r.a << shift, readRegister(reg));
}

void HG51B::instructionSUB(uint8_t imm, uint8_t shift) {
  r.a = algorithmSUB(r.a << shift, imm);
}

void HG51B::instructionSUBRr(uint8_t reg, uint8_t shift) {
  r.a = algorithmSUB(readRegister(reg), r.a << shift);
}

void HG51B::instructionSUBR(uint8_t imm, uint8_t shift) {
  r.a = algorithmSUB(imm, r.a << shift);
}

void HG51B::instructionSWAPr(uint32_t& a, uint8_t reg) {
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

void HG51B::instructionWRRAM(uint8_t byte, uint32_t& a) {
  uint16_t address = a & 0xfff;
  if(address >= 0xc00) address -= 0x400;
  dataRAM[address] = (r.ram >> ((uint32_t)byte << 3)) & 0xff;
}

void HG51B::instructionWRRAM(uint8_t byte, uint8_t imm) {
  uint16_t address = (r.dpr + imm) & 0xfff;
  if(address >= 0xc00) address -= 0x400;
  dataRAM[address] = (r.ram >> ((uint32_t)byte << 3)) & 0xff;
}

void HG51B::instructionXNORr(uint8_t reg, uint8_t shift) {
  r.a = algorithmXNOR(r.a << shift, readRegister(reg)) & 0xffffff;
}

void HG51B::instructionXNOR(uint8_t imm, uint8_t shift) {
  r.a = algorithmXNOR(r.a << shift, imm) & 0xffffff;
}

void HG51B::instructionXORr(uint8_t reg, uint8_t shift) {
  r.a = algorithmXOR(r.a << shift, readRegister(reg)) & 0xffffff;
}

void HG51B::instructionXOR(uint8_t imm, uint8_t shift) {
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

unsigned HG51B::wait(uint32_t address) {
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
  for(unsigned offset = 0; offset < 256; ++offset) {
    step(wait(address));
    programRAM[io.cache.page][offset]  = read(address++) << 0;
    programRAM[io.cache.page][offset] |= read(address++) << 8;
  }
  return io.cache.enable = 0, true;
}

void HG51B::dma() {
  for(unsigned offset = 0; offset < io.dma.length; ++offset) {
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
