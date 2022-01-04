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

struct Justifier : Controller {
  enum : unsigned {
    X, Y, Trigger, Start,
  };

  Justifier(unsigned port, bool chained);

  uint8_t data() override;
  void latch(bool data) override;
  void latch() override;

//private:
  const bool chained;  //true if the second justifier is attached to the first
  const unsigned device;
  bool latched;
  unsigned counter;
  unsigned prev;

  bool active;
  struct Player {
    int x;
    int y;
    bool trigger;
    bool start;
  } player1, player2;
};

}
