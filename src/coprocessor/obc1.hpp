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

struct OBC1 {
  void unload();
  void power();

  uint8_t read(unsigned, uint8_t);
  void write(unsigned, uint8_t);

  void serialize(serializer&);

  WritableMemory ram;

private:
  uint8_t ramRead(unsigned);
  void ramWrite(unsigned, uint8_t);

  struct {
    uint16_t address;
    uint16_t baseptr;
    uint16_t shift;
  } status;
};

extern OBC1 obc1;

}
