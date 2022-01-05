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

//The Super Scope is a light-gun: it detects the CRT beam cannon position,
//and latches the counters by toggling iobit. This only works on controller
//port 2, as iobit there is connected to the PPU H/V counter latch.
//(PIO $4201.d7)

//It is obviously not possible to perfectly simulate an IR light detecting
//a CRT beam cannon, hence this class will read the PPU raster counters.

//A Super Scope can still technically be used in port 1, however it would
//require manual polling of PIO ($4201.d6) to determine when iobit was written.
//Note that no commercial game ever utilizes a Super Scope in port 1.

#include "controller.hpp"
#include "../interface.hpp"
#include "../ppu.hpp"

#include "super-scope.hpp"

namespace SuperFamicom {

SuperScope::SuperScope(unsigned port) : Controller(port) {
  latched = 0;
  counter = 0;

  //center cursor onscreen
  x = 256 / 2;
  y = 240 / 2;

  trigger   = false;
  cursor    = false;
  turbo     = false;
  pause     = false;
  offscreen = false;

  oldturbo    = false;
  triggerlock = false;
  pauselock   = false;

  prev = 0;
}

uint8_t SuperScope::data() {
  if(counter >= 8) return 1;

  if(counter == 0) {
    //turbo is a switch; toggle is edge sensitive
    bool newturbo = Emulator::platform->inputPoll(port, ID::Device::SuperScope, Turbo);
    if(newturbo && !oldturbo) {
      turbo = !turbo;  //toggle state
    }
    oldturbo = newturbo;

    //trigger is a button
    //if turbo is active, trigger is level sensitive; otherwise, it is edge sensitive
    trigger = false;
    bool newtrigger = Emulator::platform->inputPoll(port, ID::Device::SuperScope, Trigger);
    if(newtrigger && (turbo || !triggerlock)) {
      trigger = true;
      triggerlock = true;
    } else if(!newtrigger) {
      triggerlock = false;
    }

    //cursor is a button; it is always level sensitive
    cursor = Emulator::platform->inputPoll(port, ID::Device::SuperScope, Cursor);

    //pause is a button; it is always edge sensitive
    pause = false;
    bool newpause = Emulator::platform->inputPoll(port, ID::Device::SuperScope, Pause);
    if(newpause && !pauselock) {
      pause = true;
      pauselock = true;
    } else if(!newpause) {
      pauselock = false;
    }

    // Changes were made here because some games need their coords offset JOLLYGOOD
    offscreen = (x < -16 || y < -16 || x >= 512 || y >= 480);
  }

  switch(counter++) {
  case 0: return offscreen ? 0 : trigger;
  case 1: return cursor;
  case 2: return turbo;
  case 3: return pause;
  case 4: return 0;
  case 5: return 0;
  case 6: return offscreen;
  case 7: return 0;  //noise (1 = yes)
  }

  return 0; // unreachable
}

void SuperScope::latch(bool data) {
  if(latched == data) return;
  latched = data;
  counter = 0;
}

void SuperScope::latch() {
  x = Emulator::platform->inputPoll(port, ID::Device::SuperScope, X);
  y = Emulator::platform->inputPoll(port, ID::Device::SuperScope, Y);
  offscreen = (x < 0 || y < 0 || x >= 512 || y >= 480);
  if(!offscreen) ppu.latchCounters(x, y);
}

}
