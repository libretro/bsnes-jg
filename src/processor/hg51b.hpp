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

#include "../function.hpp"

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
  unsigned wait(uint32_t address);
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

  uint32_t algorithmADD(uint32_t x, uint32_t y);
  uint32_t algorithmAND(uint32_t x, uint32_t y);
  uint32_t algorithmASR(uint32_t a, uint8_t s);
  uint64_t algorithmMUL(uint32_t x, uint32_t y);
  uint32_t algorithmOR(uint32_t x, uint32_t y);
  uint32_t algorithmROR(uint32_t a, uint8_t s);
  uint32_t algorithmSHL(uint32_t a, uint8_t s);
  uint32_t algorithmSHR(uint32_t a, uint8_t s);
  uint32_t algorithmSUB(uint32_t x, uint32_t y);
  uint32_t algorithmSX(uint32_t x);
  uint32_t algorithmXNOR(uint32_t x, uint32_t y);
  uint32_t algorithmXOR(uint32_t x, uint32_t y);

  void instructionADDr(uint8_t reg, uint8_t shift);
  void instructionADD(uint8_t imm, uint8_t shift);
  void instructionANDr(uint8_t reg, uint8_t shift);
  void instructionAND(uint8_t imm, uint8_t shift);
  void instructionASRr(uint8_t reg);
  void instructionASR(uint8_t imm);
  void instructionCLEAR();
  void instructionCMPr(uint8_t reg, uint8_t shift);
  void instructionCMP(uint8_t imm, uint8_t shift);
  void instructionCMPRr(uint8_t reg, uint8_t shift);
  void instructionCMPR(uint8_t imm, uint8_t shift);
  void instructionHALT();
  void instructionINC(uint32_t& reg);
  void instructionJMP(uint8_t data, uint8_t far, const uint8_t& take);
  void instructionJSR(uint8_t data, uint8_t far, const uint8_t& take);
  void instructionLDr(uint32_t& out, uint8_t reg);
  void instructionLDr(uint16_t& out, uint8_t reg);
  void instructionLD(uint32_t& out, uint8_t imm);
  void instructionLD(uint16_t& out, uint8_t imm);
  void instructionLDL(uint16_t& out, uint8_t imm);
  void instructionLDHr(uint16_t& out, uint8_t imm);
  void instructionMULr(uint8_t reg);
  void instructionMUL(uint8_t imm);
  void instructionNOP();
  void instructionORr(uint8_t reg, uint8_t shift);
  void instructionOR(uint8_t imm, uint8_t shift);
  void instructionRDRAM(uint8_t byte, uint32_t& a);
  void instructionRDRAM(uint8_t byte, uint8_t imm);
  void instructionRDROM(uint32_t& reg);
  void instructionRDROM(uint16_t imm);
  void instructionRORr(uint8_t reg);
  void instructionROR(uint8_t imm);
  void instructionRTS();
  void instructionSHLr(uint8_t reg);
  void instructionSHL(uint8_t imm);
  void instructionSHRr(uint8_t reg);
  void instructionSHR(uint8_t imm);
  void instructionSKIP(uint8_t take, const uint8_t& flag);
  void instructionSTr(uint8_t reg, uint32_t& in);
  void instructionSUBr(uint8_t reg, uint8_t shift);
  void instructionSUB(uint8_t imm, uint8_t shift);
  void instructionSUBRr(uint8_t reg, uint8_t shift);
  void instructionSUBR(uint8_t imm, uint8_t shift);
  void instructionSWAPr(uint32_t& a, uint8_t reg);
  void instructionSXB();
  void instructionSXW();
  void instructionWAIT();
  void instructionWRRAM(uint8_t byte, uint32_t& a);
  void instructionWRRAM(uint8_t byte, uint8_t imm);
  void instructionXNORr(uint8_t reg, uint8_t shift);
  void instructionXNOR(uint8_t imm, uint8_t shift);
  void instructionXORr(uint8_t reg, uint8_t shift);
  void instructionXOR(uint8_t imm, uint8_t shift);

  //serialization.cpp
  void serialize(serializer&);

  uint16_t programRAM[2][256];  //instruction cache
  uint32_t dataROM[1024];
  uint8_t dataRAM[3072];

  //registers.cpp
  uint32_t readRegister(uint8_t address);
  void writeRegister(uint8_t address, uint32_t data);

protected:
  struct Registers {
    uint16_t pb;    //program bank
    uint8_t  pc;  //program counter

    bool n;  //negative
    bool z;  //zero
    bool c;  //carry
    bool v;  //overflow
    bool i;  //interrupt

    uint32_t a;              //accumulator
    uint16_t p;              //page register
    uint64_t mul;            //multiplier
    uint32_t mdr;            //bus memory data register
    uint32_t rom;            //data ROM data buffer
    uint32_t ram;            //data RAM data buffer
    uint32_t mar;            //bus memory address register
    uint32_t dpr;            //data RAM address pointer
    uint32_t gpr[16];        //general purpose registers
  } r;

  struct IO {
    uint8_t lock;
    uint8_t halt = 1;
    uint8_t irq;      //0 = enable, 1 = disable
    uint8_t rom = 1;  //0 = 2 ROMs, 1 = 1 ROM
    uint8_t vector[32];

    struct Wait {
      uint8_t rom = 3;
      uint8_t ram = 3;
    } wait;

    struct Suspend {
      uint8_t enable;
      uint8_t duration;
    } suspend;

    struct Cache {
      uint8_t enable;
      uint8_t page;
      uint8_t lock[2];
      uint32_t address[2];  //cache address is in bytes; so 24-bit
      uint32_t base;        //base address is also in bytes
      uint16_t pb;
      uint8_t pc;
    } cache;

    struct DMA {
      uint8_t enable;
      uint32_t source;
      uint32_t target;
      uint16_t length;
    } dma;

    struct Bus {
      uint8_t enable;
      uint8_t reading;
      uint8_t writing;
      uint8_t pending;
      uint32_t address;
    } bus;
  } io;

  uint32_t stack[8];
  bfunction<void ()> instructionTable[65536];
};

}
