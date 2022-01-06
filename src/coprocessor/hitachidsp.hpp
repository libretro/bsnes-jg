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

#include <optional>

#include "../memory.hpp"
#include "../processor/hg51b.hpp"

namespace SuperFamicom {

struct HitachiDSP : Processor::HG51B, Thread {
  ReadableMemory rom;
  WritableMemory ram;

  //hitachidsp.cpp
  void synchronizeCPU();
  static void Enter();
  void step(unsigned clocks) override;
  void halt() override;

  void unload();
  void power();

  bool isROM(unsigned address) override;
  bool isRAM(unsigned address) override;

  //HG51B read/write
  uint8_t read(unsigned address) override;
  void write(unsigned address, uint8_t data) override;

  //CPU ROM read/write
  std::optional<unsigned> addressROM(unsigned address) const;
  uint8_t readROM(unsigned address, uint8_t data = 0);
  void writeROM(unsigned address, uint8_t data);

  //CPU RAM read/write
  std::optional<unsigned> addressRAM(unsigned address) const;
  uint8_t readRAM(unsigned address, uint8_t data = 0);
  void writeRAM(unsigned address, uint8_t data);

  //HG51B data RAM read/write
  std::optional<unsigned> addressDRAM(unsigned address) const;
  uint8_t readDRAM(unsigned address, uint8_t data = 0);
  void writeDRAM(unsigned address, uint8_t data);

  //CPU IO read/write
  std::optional<unsigned> addressIO(unsigned address) const;
  uint8_t readIO(unsigned address, uint8_t data = 0);
  void writeIO(unsigned address, uint8_t data);

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
