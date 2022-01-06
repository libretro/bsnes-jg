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

#include "../emulator.hpp"
#include "../settings.hpp"

#include "controller.hpp"

#include "mouse.hpp"

namespace SuperFamicom {

Mouse::Mouse(unsigned port) : Controller(port) {
  latched = 0;
  counter = 0;

  speed = 0;
  x = 0;
  y = 0;
  dx = 0;
  dy = 0;
  l = 0;
  r = 0;
}

uint8_t Mouse::data() {
  if(latched == 1) {
    speed = (speed + 1) % 3;
    return 0;
  }

  if(counter >= 32) return 1;

  switch(counter++) { default:
  case  0: return 0;
  case  1: return 0;
  case  2: return 0;
  case  3: return 0;
  case  4: return 0;
  case  5: return 0;
  case  6: return 0;
  case  7: return 0;

  case  8: return r;
  case  9: return l;
  case 10: return (speed >> 1) & 1;
  case 11: return (speed >> 0) & 1;

  case 12: return 0;  //signature
  case 13: return 0;  // ||
  case 14: return 0;  // ||
  case 15: return 1;  // ||

  case 16: return dy;
  case 17: return (y >> 6) & 1;
  case 18: return (y >> 5) & 1;
  case 19: return (y >> 4) & 1;
  case 20: return (y >> 3) & 1;
  case 21: return (y >> 2) & 1;
  case 22: return (y >> 1) & 1;
  case 23: return (y >> 0) & 1;

  case 24: return dx;
  case 25: return (x >> 6) & 1;
  case 26: return (x >> 5) & 1;
  case 27: return (x >> 4) & 1;
  case 28: return (x >> 3) & 1;
  case 29: return (x >> 2) & 1;
  case 30: return (x >> 1) & 1;
  case 31: return (x >> 0) & 1;
  }
}

void Mouse::latch(bool data) {
  if(latched == data) return;
  latched = data;
  counter = 0;

  x = Emulator::platform->inputPoll(port, ID::Device::Mouse, X);  //-n = left, 0 = center, +n = right
  y = Emulator::platform->inputPoll(port, ID::Device::Mouse, Y);  //-n = up,   0 = center, +n = down
  l = Emulator::platform->inputPoll(port, ID::Device::Mouse, Left);
  r = Emulator::platform->inputPoll(port, ID::Device::Mouse, Right);

  dx = x < 0;  //0 = right, 1 = left
  dy = y < 0;  //0 = down,  1 = up

  if(x < 0) x = -x;  //abs(position_x)
  if(y < 0) y = -y;  //abs(position_y)

  double multiplier = 1.0;
  if(speed == 1) multiplier = 1.5;
  if(speed == 2) multiplier = 2.0;
  x = (double)x * multiplier;
  y = (double)y * multiplier;

  x = std::min(127, x);
  y = std::min(127, y);
}

}
