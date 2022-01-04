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

#include "../processor/upd96050.hpp"

namespace SuperFamicom {

struct NECDSP : Processor::uPD96050, Thread {
  void synchronizeCPU();
  static void Enter();
  void main();
  void step(unsigned clocks);

  uint8_t read(unsigned addr, uint8_t data);
  void write(unsigned addr, uint8_t data);

  uint8_t readRAM(unsigned addr, uint8_t data);
  void writeRAM(unsigned addr, uint8_t data);

  void power();

  std::vector<uint8_t> firmware() const;
  void serialize(serializer&);

  unsigned Frequency = 0;
};

extern NECDSP necdsp;

}
