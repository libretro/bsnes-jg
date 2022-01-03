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

#include "../sfc.hpp"

namespace SuperFamicom {

SuperMultitap::SuperMultitap(unsigned port) : Controller(port) {
  latched = 0;
  counter1 = 0;
  counter2 = 0;
}

nall::Natural< 2> SuperMultitap::data() {
  if(latched) return 2;  //device detection
  unsigned counter, a, b;

  if(iobit()) {
    counter = counter1;
    if(counter >= 16) return 3;
    counter1++;
    if(counter >= 12) return 0;
    a = 0;  //controller 2
    b = 1;  //controller 3
  } else {
    counter = counter2;
    if(counter >= 16) return 3;
    counter2++;
    if(counter >= 12) return 0;
    a = 2;  //controller 4
    b = 3;  //controller 5
  }

  auto& A = gamepads[a];
  auto& B = gamepads[b];

  switch(counter) {
  case  0: return A.b << 0 | B.b << 1;
  case  1: return A.y << 0 | B.y << 1;
  case  2: return A.select << 0 | B.select << 1;
  case  3: return A.start << 0 | B.start << 1;
  case  4: return (A.up & !A.down) << 0 | (B.up & !B.down) << 1;
  case  5: return (A.down & !A.up) << 0 | (B.down & !B.up) << 1;
  case  6: return (A.left & !A.right) << 0 | (B.left & !B.right) << 1;
  case  7: return (A.right & !A.left) << 0 | (B.right & !B.left) << 1;
  case  8: return A.a << 0 | B.a << 1;
  case  9: return A.x << 0 | B.x << 1;
  case 10: return A.l << 0 | B.l << 1;
  case 11: return A.r << 0 | B.r << 1;
  }
  return 0; // unreachable
}

void SuperMultitap::latch(bool data) {
  if(latched == data) return;
  latched = data;
  counter1 = 0;
  counter2 = 0;

  if(latched == 0) {
    for(unsigned id = 0; id < 4; ++id) {
      auto& gamepad = gamepads[id];
      gamepad.b      = Emulator::platform->inputPoll(port, ID::Device::SuperMultitap, id * 12 + B);
      gamepad.y      = Emulator::platform->inputPoll(port, ID::Device::SuperMultitap, id * 12 + Y);
      gamepad.select = Emulator::platform->inputPoll(port, ID::Device::SuperMultitap, id * 12 + Select);
      gamepad.start  = Emulator::platform->inputPoll(port, ID::Device::SuperMultitap, id * 12 + Start);
      gamepad.up     = Emulator::platform->inputPoll(port, ID::Device::SuperMultitap, id * 12 + Up);
      gamepad.down   = Emulator::platform->inputPoll(port, ID::Device::SuperMultitap, id * 12 + Down);
      gamepad.left   = Emulator::platform->inputPoll(port, ID::Device::SuperMultitap, id * 12 + Left);
      gamepad.right  = Emulator::platform->inputPoll(port, ID::Device::SuperMultitap, id * 12 + Right);
      gamepad.a      = Emulator::platform->inputPoll(port, ID::Device::SuperMultitap, id * 12 + A);
      gamepad.x      = Emulator::platform->inputPoll(port, ID::Device::SuperMultitap, id * 12 + X);
      gamepad.l      = Emulator::platform->inputPoll(port, ID::Device::SuperMultitap, id * 12 + L);
      gamepad.r      = Emulator::platform->inputPoll(port, ID::Device::SuperMultitap, id * 12 + R);
    }
  }
}

}
