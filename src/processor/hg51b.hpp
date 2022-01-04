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

#pragma once

//Hitachi HG51B S169

namespace Processor {

struct HG51B {
  //instruction.cpp
  HG51B();

  //hg51b.cpp
  virtual void step(unsigned clocks);
  virtual bool isROM(unsigned address) = 0;
  virtual bool isRAM(unsigned address) = 0;
  virtual uint8_t read(unsigned address) = 0;
  virtual void write(unsigned address, uint8_t data) = 0;
  virtual void lock();
  virtual void halt();
  unsigned wait(nall::Natural<24> address);
  void main();
  void execute();
  void advance();
  void suspend();
  bool cache();
  void dma();
  bool running() const;
  bool busy() const;

  void power();

  //instructions.cpp
  void push();
  void pull();

  nall::Natural<24> algorithmADD(nall::Natural<24> x, nall::Natural<24> y);
  nall::Natural<24> algorithmAND(nall::Natural<24> x, nall::Natural<24> y);
  nall::Natural<24> algorithmASR(nall::Natural<24> a, nall::Natural< 5> s);
  nall::Natural<48> algorithmMUL(nall::Integer<24> x, nall::Integer<24> y);
  nall::Natural<24> algorithmOR(nall::Natural<24> x, nall::Natural<24> y);
  nall::Natural<24> algorithmROR(nall::Natural<24> a, nall::Natural< 5> s);
  nall::Natural<24> algorithmSHL(nall::Natural<24> a, nall::Natural< 5> s);
  nall::Natural<24> algorithmSHR(nall::Natural<24> a, nall::Natural< 5> s);
  nall::Natural<24> algorithmSUB(nall::Natural<24> x, nall::Natural<24> y);
  nall::Natural<24> algorithmSX(nall::Natural<24> x);
  nall::Natural<24> algorithmXNOR(nall::Natural<24> x, nall::Natural<24> y);
  nall::Natural<24> algorithmXOR(nall::Natural<24> x, nall::Natural<24> y);

  void instructionADD(nall::Natural< 7> reg, nall::Natural< 5> shift);
  void instructionADD(uint8_t imm, nall::Natural< 5> shift);
  void instructionAND(nall::Natural< 7> reg, nall::Natural< 5> shift);
  void instructionAND(uint8_t imm, nall::Natural< 5> shift);
  void instructionASR(nall::Natural< 7> reg);
  void instructionASR(nall::Natural< 5> imm);
  void instructionCLEAR();
  void instructionCMP(nall::Natural< 7> reg, nall::Natural< 5> shift);
  void instructionCMP(uint8_t imm, nall::Natural< 5> shift);
  void instructionCMPR(nall::Natural< 7> reg, nall::Natural< 5> shift);
  void instructionCMPR(uint8_t imm, nall::Natural< 5> shift);
  void instructionHALT();
  void instructionINC(nall::Natural<24>& reg);
  void instructionJMP(uint8_t data, nall::Natural< 1> far, const nall::Natural< 1>& take);
  void instructionJSR(uint8_t data, nall::Natural< 1> far, const nall::Natural< 1>& take);
  void instructionLD(nall::Natural<24>& out, nall::Natural< 7> reg);
  void instructionLD(nall::Natural<15>& out, nall::Natural< 4> reg);
  void instructionLD(nall::Natural<24>& out, uint8_t imm);
  void instructionLD(nall::Natural<15>& out, uint8_t imm);
  void instructionLDL(nall::Natural<15>& out, uint8_t imm);
  void instructionLDH(nall::Natural<15>& out, nall::Natural< 7> imm);
  void instructionMUL(nall::Natural< 7> reg);
  void instructionMUL(uint8_t imm);
  void instructionNOP();
  void instructionOR(nall::Natural< 7> reg, nall::Natural< 5> shift);
  void instructionOR(uint8_t imm, nall::Natural< 5> shift);
  void instructionRDRAM(nall::Natural< 2> byte, nall::Natural<24>& a);
  void instructionRDRAM(nall::Natural< 2> byte, uint8_t imm);
  void instructionRDROM(nall::Natural<24>& reg);
  void instructionRDROM(nall::Natural<10> imm);
  void instructionROR(nall::Natural< 7> reg);
  void instructionROR(nall::Natural< 5> imm);
  void instructionRTS();
  void instructionSHL(nall::Natural< 7> reg);
  void instructionSHL(nall::Natural< 5> imm);
  void instructionSHR(nall::Natural< 7> reg);
  void instructionSHR(nall::Natural< 5> imm);
  void instructionSKIP(nall::Natural< 1> take, const nall::Natural< 1>& flag);
  void instructionST(nall::Natural< 7> reg, nall::Natural<24>& in);
  void instructionSUB(nall::Natural< 7> reg, nall::Natural< 5> shift);
  void instructionSUB(uint8_t imm, nall::Natural< 5> shift);
  void instructionSUBR(nall::Natural< 7> reg, nall::Natural< 5> shift);
  void instructionSUBR(uint8_t imm, nall::Natural< 5> shift);
  void instructionSWAP(nall::Natural<24>& a, nall::Natural< 4> reg);
  void instructionSXB();
  void instructionSXW();
  void instructionWAIT();
  void instructionWRRAM(nall::Natural< 2> byte, nall::Natural<24>& a);
  void instructionWRRAM(nall::Natural< 2> byte, uint8_t imm);
  void instructionXNOR(nall::Natural< 7> reg, nall::Natural< 5> shift);
  void instructionXNOR(uint8_t imm, nall::Natural< 5> shift);
  void instructionXOR(nall::Natural< 7> reg, nall::Natural< 5> shift);
  void instructionXOR(uint8_t imm, nall::Natural< 5> shift);

  //serialization.cpp
  void serialize(serializer&);

  uint16_t programRAM[2][256];  //instruction cache
  nall::Natural<24> dataROM[1024];
  uint8_t dataRAM[3072];

  //registers.cpp
  nall::Natural<24> readRegister(nall::Natural< 7> address);
  void writeRegister(nall::Natural< 7> address, nall::Natural<24> data);

protected:
  struct Registers {
    nall::Natural<15> pb;    //program bank
    uint8_t  pc;  //program counter

    bool n;  //negative
    bool z;  //zero
    bool c;  //carry
    bool v;  //overflow
    bool i;  //interrupt

    nall::Natural<24> a;              //accumulator
    nall::Natural<15> p;              //page register
    nall::Natural<48> mul;            //multiplier
    nall::Natural<24> mdr;            //bus memory data register
    nall::Natural<24> rom;            //data ROM data buffer
    nall::Natural<24> ram;            //data RAM data buffer
    nall::Natural<24> mar;            //bus memory address register
    nall::Natural<24> dpr;            //data RAM address pointer
    nall::Natural<24> gpr[16];        //general purpose registers
  } r;

  struct IO {
    nall::Natural< 1> lock;
    nall::Natural< 1> halt = 1;
    nall::Natural< 1> irq;      //0 = enable, 1 = disable
    nall::Natural< 1> rom = 1;  //0 = 2 ROMs, 1 = 1 ROM
    uint8_t vector[32];

    struct Wait {
      nall::Natural< 3> rom = 3;
      nall::Natural< 3> ram = 3;
    } wait;

    struct Suspend {
      nall::Natural< 1> enable;
      uint8_t duration;
    } suspend;

    struct Cache {
      nall::Natural< 1>  enable;
      nall::Natural< 1>  page;
      nall::Natural< 1>  lock[2];
      nall::Natural<24> address[2];  //cache address is in bytes; so 24-bit
      nall::Natural<24> base;        //base address is also in bytes
      nall::Natural<15> pb;
      uint8_t  pc;
    } cache;

    struct DMA {
      nall::Natural< 1>  enable;
      nall::Natural<24> source;
      nall::Natural<24> target;
      uint16_t length;
    } dma;

    struct Bus {
      nall::Natural< 1>  enable;
      nall::Natural< 1>  reading;
      nall::Natural< 1>  writing;
      nall::Natural< 4>  pending;
      nall::Natural<24> address;
    } bus;
  } io;

  nall::Natural<23> stack[8];
  bfunction<void ()> instructionTable[65536];
};

}
