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

#include "../interface.hpp"

#include "gamepad.hpp"

namespace SuperFamicom {

Gamepad::Gamepad(unsigned port) : Controller(port) {
  latched = 0;
  counter = 0;
}

uint8_t Gamepad::data() {
  if(counter >= 16) return 1;
  if(latched == 1) return Emulator::platform->inputPoll(port, ID::Device::Gamepad, B);

  //note: D-pad physically prevents up+down and left+right from being pressed at the same time
  switch(counter++) {
  case  0: return b;
  case  1: return y;
  case  2: return select;
  case  3: return start;
  case  4: return up & !down;
  case  5: return down & !up;
  case  6: return left & !right;
  case  7: return right & !left;
  case  8: return a;
  case  9: return x;
  case 10: return l;
  case 11: return r;
  }

  return 0;  //12-15: signature
}

void Gamepad::latch(bool data) {
  if(latched == data) return;
  latched = data;
  counter = 0;

  if(latched == 0) {
    b      = Emulator::platform->inputPoll(port, ID::Device::Gamepad, B);
    y      = Emulator::platform->inputPoll(port, ID::Device::Gamepad, Y);
    select = Emulator::platform->inputPoll(port, ID::Device::Gamepad, Select);
    start  = Emulator::platform->inputPoll(port, ID::Device::Gamepad, Start);
    up     = Emulator::platform->inputPoll(port, ID::Device::Gamepad, Up);
    down   = Emulator::platform->inputPoll(port, ID::Device::Gamepad, Down);
    left   = Emulator::platform->inputPoll(port, ID::Device::Gamepad, Left);
    right  = Emulator::platform->inputPoll(port, ID::Device::Gamepad, Right);
    a      = Emulator::platform->inputPoll(port, ID::Device::Gamepad, A);
    x      = Emulator::platform->inputPoll(port, ID::Device::Gamepad, X);
    l      = Emulator::platform->inputPoll(port, ID::Device::Gamepad, L);
    r      = Emulator::platform->inputPoll(port, ID::Device::Gamepad, R);
  }
}

}
