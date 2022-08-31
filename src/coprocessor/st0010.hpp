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

struct ST0010 {
  void power();

  uint8_t read(unsigned, uint8_t);
  void write(unsigned, uint8_t);

  void serialize(serializer&);

  uint8_t ram[0x1000];

  //interfaces to sin table
  int16_t sin(int16_t);
  int16_t cos(int16_t);

  //interfaces to ram buffer
  uint8_t readb (uint16_t);
  uint16_t readw (uint16_t);
  uint32_t readd (uint16_t);
  void writeb(uint16_t, uint8_t);
  void writew(uint16_t, uint16_t);
  void writed(uint16_t, uint32_t);

  //opcodes
  void op_01();
  void op_02();
  void op_03();
  void op_04();
  void op_05();
  void op_06();
  void op_07();
  void op_08();

  void op_01(int16_t, int16_t, int16_t&, int16_t&, int16_t&, int16_t&);
};

extern ST0010 st0010;

}
