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

//MCC - Memory Controller Chip
//Custom logic chip inside the BS-X Satellaview base cartridge

namespace SuperFamicom {

struct MCC {
  ReadableMemory rom;
  WritableMemory psram;

  //mcc.cpp
  void unload();
  void power();
  void commit();

  uint8_t read(unsigned address, uint8_t data);
  void write(unsigned address, uint8_t data);

  uint8_t mcuRead(unsigned address, uint8_t data);
  void mcuWrite(unsigned address, uint8_t data);

  uint8_t mcuAccess(bool mode, unsigned address, uint8_t data);
  uint8_t romAccess(bool mode, unsigned address, uint8_t data);
  uint8_t psramAccess(bool mode, unsigned address, uint8_t data);
  uint8_t exAccess(bool mode, unsigned address, uint8_t data);
  uint8_t bsAccess(bool mode, unsigned address, uint8_t data);

  //serialization.cpp
  void serialize(serializer&);

private:
  struct IRQ {
    nall::Natural< 1> flag;    //bit 0
    nall::Natural< 1> enable;  //bit 1
  } irq;

  struct Registers {
    nall::Natural< 1> mapping;             //bit  2 (0 = ignore A15; 1 = use A15)
    nall::Natural< 1> psramEnableLo;       //bit  3
    nall::Natural< 1> psramEnableHi;       //bit  4
    nall::Natural< 2> psramMapping;        //bits 5-6
    nall::Natural< 1> romEnableLo;         //bit  7
    nall::Natural< 1> romEnableHi;         //bit  8
    nall::Natural< 1> exEnableLo;          //bit  9
    nall::Natural< 1> exEnableHi;          //bit 10
    nall::Natural< 1> exMapping;           //bit 11
    nall::Natural< 1> internallyWritable;  //bit 12 (1 = MCC allows writes to BS Memory Cassette)
    nall::Natural< 1> externallyWritable;  //bit 13 (1 = BS Memory Cassette allows writes to flash memory)
  } r, w;

  //bit 14 = commit
  //bit 15 = unknown (test register interface?)
};

extern MCC mcc;

}
