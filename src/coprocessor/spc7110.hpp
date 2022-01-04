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

namespace SuperFamicom {

struct Decompressor;

struct SPC7110 : Thread {
  SPC7110();
  ~SPC7110();

  void synchronizeCPU();
  static void Enter();
  void main();
  void step(unsigned clocks);
  void unload();
  void power();

  void addClocks(unsigned clocks);

  uint8_t read(unsigned addr, uint8_t data);
  void write(unsigned addr, uint8_t data);

  uint8_t mcuromRead(unsigned addr, uint8_t data);
  void mcuromWrite(unsigned addr, uint8_t data);

  uint8_t mcuramRead(unsigned addr, uint8_t data);
  void mcuramWrite(unsigned addr, uint8_t data);

  void serialize(serializer&);

  //dcu.cpp
  void dcuLoadAddress();
  void dcuBeginTransfer();
  uint8_t dcuRead();

  void deinterleave1bpp(unsigned length);
  void deinterleave2bpp(unsigned length);
  void deinterleave4bpp(unsigned length);

  //data.cpp
  uint8_t dataromRead(unsigned addr);

  unsigned dataOffset();
  unsigned dataAdjust();
  unsigned dataStride();

  void setDataOffset(unsigned addr);
  void setDataAdjust(unsigned addr);

  void dataPortRead();

  void dataPortIncrement4810();
  void dataPortIncrement4814();
  void dataPortIncrement4815();
  void dataPortIncrement481a();

  //alu.cpp
  void aluMultiply();
  void aluDivide();

  ReadableMemory prom;  //program ROM
  ReadableMemory drom;  //data ROM
  WritableMemory ram;

private:
  //decompression unit
  uint8_t r4801;  //compression table B0
  uint8_t r4802;  //compression table B1
  nall::Natural< 7> r4803;  //compression table B2
  uint8_t r4804;  //compression table index
  uint8_t r4805;  //adjust length B0
  uint8_t r4806;  //adjust length B1
  uint8_t r4807;  //stride length
  uint8_t r4809;  //compression counter B0
  uint8_t r480a;  //compression counter B1
  uint8_t r480b;  //decompression settings
  uint8_t r480c;  //decompression status

  bool dcuPending;
  nall::Natural< 2> dcuMode;
  nall::Natural<23> dcuAddress;
  unsigned dcuOffset;
  uint8_t dcuTile[32];
  Decompressor* decompressor;

  //data port unit
  uint8_t r4810;  //data port read + seek
  uint8_t r4811;  //data offset B0
  uint8_t r4812;  //data offset B1
  nall::Natural< 7> r4813;  //data offset B2
  uint8_t r4814;  //data adjust B0
  uint8_t r4815;  //data adjust B1
  uint8_t r4816;  //data stride B0
  uint8_t r4817;  //data stride B1
  uint8_t r4818;  //data port settings
  uint8_t r481a;  //data port seek

  //arithmetic logic unit
  uint8_t r4820;  //16-bit multiplicand B0, 32-bit dividend B0
  uint8_t r4821;  //16-bit multiplicand B1, 32-bit dividend B1
  uint8_t r4822;  //32-bit dividend B2
  uint8_t r4823;  //32-bit dividend B3
  uint8_t r4824;  //16-bit multiplier B0
  uint8_t r4825;  //16-bit multiplier B1
  uint8_t r4826;  //16-bit divisor B0
  uint8_t r4827;  //16-bit divisor B1
  uint8_t r4828;  //32-bit product B0, 32-bit quotient B0
  uint8_t r4829;  //32-bit product B1, 32-bit quotient B1
  uint8_t r482a;  //32-bit product B2, 32-bit quotient B2
  uint8_t r482b;  //32-bit product B3, 32-bit quotient B3
  uint8_t r482c;  //16-bit remainder B0
  uint8_t r482d;  //16-bit remainder B1
  uint8_t r482e;  //math settings
  uint8_t r482f;  //math status

  bool mulPending;
  bool divPending;

  //memory control unit
  uint8_t r4830;  //bank 0 mapping + SRAM write enable
  uint8_t r4831;  //bank 1 mapping
  uint8_t r4832;  //bank 2 mapping
  uint8_t r4833;  //bank 3 mapping
  uint8_t r4834;  //bank mapping settings
};

extern SPC7110 spc7110;

}
