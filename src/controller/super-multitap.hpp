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

struct SuperMultitap : Controller {
  enum : unsigned {
    Up, Down, Left, Right, B, A, Y, X, L, R, Select, Start,
  };

  SuperMultitap(unsigned port);

  uint8_t data();
  void latch(bool data);

private:
  bool latched;
  unsigned counter1;
  unsigned counter2;

  struct Gamepad {
    bool b, y, select, start;
    bool up, down, left, right;
    bool a, x, l, r;
  } gamepads[4];
};
