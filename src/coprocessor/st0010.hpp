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

  uint8_t read(unsigned addr, uint8_t data);
  void write(unsigned addr, uint8_t data);

  void serialize(serializer&);

  uint8_t ram[0x1000];
  static const int16_t sin_table[256];
  static const int16_t mode7_scale[176];
  static const uint8_t arctan[32][32];

  //interfaces to sin table
  int16_t sin(int16_t theta);
  int16_t cos(int16_t theta);

  //interfaces to ram buffer
  uint8_t readb (uint16_t addr);
  uint16_t readw (uint16_t addr);
  uint32_t readd (uint16_t addr);
  void writeb(uint16_t addr, uint8_t  data);
  void writew(uint16_t addr, uint16_t data);
  void writed(uint16_t addr, uint32_t data);

  //opcodes
  void op_01();
  void op_02();
  void op_03();
  void op_04();
  void op_05();
  void op_06();
  void op_07();
  void op_08();

  void op_01(int16_t x0, int16_t y0, int16_t &x1, int16_t &y1, int16_t &quadrant, int16_t &theta);
};

extern ST0010 st0010;

}
