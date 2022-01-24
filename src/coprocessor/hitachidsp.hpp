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

#include "memory.hpp"
#include "processor/hg51b.hpp"

namespace SuperFamicom {

struct HitachiDSP : Processor::HG51B, Thread {
  ReadableMemory rom;
  WritableMemory ram;

  //hitachidsp.cpp
  void synchronizeCPU();
  static void Enter();
  void step(unsigned) override;
  void halt() override;

  void unload();
  void power();

  bool isROM(unsigned) override;
  bool isRAM(unsigned) override;

  //HG51B read/write
  uint8_t read(unsigned) override;
  void write(unsigned, uint8_t) override;

  //CPU ROM read/write
  bool addressROM(unsigned&) const;
  uint8_t readROM(unsigned, uint8_t = 0);
  void writeROM(unsigned, uint8_t);

  //CPU RAM read/write
  bool addressRAM(unsigned&) const;
  uint8_t readRAM(unsigned, uint8_t = 0);
  void writeRAM(unsigned, uint8_t);

  //HG51B data RAM read/write
  bool addressDRAM(unsigned&) const;
  uint8_t readDRAM(unsigned, uint8_t = 0);
  void writeDRAM(unsigned, uint8_t);

  //CPU IO read/write
  bool addressIO(unsigned&) const;
  uint8_t readIO(unsigned, uint8_t = 0);
  void writeIO(unsigned, uint8_t);

  std::vector<uint8_t> firmware() const;
  void serialize(serializer&);

  unsigned Frequency;
  unsigned Roms;
  bool Mapping;

  //data-rom.cpp
  static const uint8_t staticDataROM[3072];
};

extern HitachiDSP hitachidsp;

}
