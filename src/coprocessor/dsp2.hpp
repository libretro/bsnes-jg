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

struct DSP2 {
  void power();

  uint8_t read(unsigned addr, uint8_t data);
  void write(unsigned addr, uint8_t data);

  void serialize(serializer&);

  struct {
    bool waiting_for_command;
    unsigned command;
    unsigned in_count,  in_index;
    unsigned out_count, out_index;

    uint8_t  parameters[512];
    uint8_t  output[512];

    uint8_t  op05transparent;
    bool   op05haslen;
    int    op05len;
    bool   op06haslen;
    int    op06len;
    uint16_t op09word1;
    uint16_t op09word2;
    bool   op0dhaslen;
    int    op0doutlen;
    int    op0dinlen;
  } status;

  void op01();
  void op03();
  void op05();
  void op06();
  void op09();
  void op0d();
};

extern DSP2 dsp2;

}
