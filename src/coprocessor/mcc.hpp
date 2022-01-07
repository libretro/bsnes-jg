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
    uint8_t flag;    //bit 0
    uint8_t enable;  //bit 1
  } irq;

  struct Registers {
    uint8_t mapping;             //bit  2 (0 = ignore A15; 1 = use A15)
    uint8_t psramEnableLo;       //bit  3
    uint8_t psramEnableHi;       //bit  4
    uint8_t psramMapping;        //bits 5-6
    uint8_t romEnableLo;         //bit  7
    uint8_t romEnableHi;         //bit  8
    uint8_t exEnableLo;          //bit  9
    uint8_t exEnableHi;          //bit 10
    uint8_t exMapping;           //bit 11
    uint8_t internallyWritable;  //bit 12 (1 = MCC allows writes to BS Memory Cassette)
    uint8_t externallyWritable;  //bit 13 (1 = BS Memory Cassette allows writes to flash memory)
  } r, w;

  //bit 14 = commit
  //bit 15 = unknown (test register interface?)
};

extern MCC mcc;

}
