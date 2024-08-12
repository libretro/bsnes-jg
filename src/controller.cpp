/*
 * bsnes-jg - Super Nintendo emulator
 *
 * Copyright (C) 2004-2020 byuu
 * Copyright (C) 2020-2024 Rupert Carmichael
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

#include "serializer.hpp"
#include "cpu.hpp"
#include "memory.hpp"
#include "settings.hpp"

#include "controller.hpp"

static unsigned (*inputPollGamepad)(unsigned);
static int (*inputPollMouse)(unsigned, unsigned);
static int (*inputPollLightgun)(unsigned, unsigned);

namespace SuperFamicom {

void setInputPollGamepad(unsigned (*cb)(unsigned)) {
  inputPollGamepad = cb;
}

void setInputPollMouse(int (*cb)(unsigned, unsigned)) {
  inputPollMouse = cb;
}

void setInputPollLightgun(int (*cb)(unsigned, unsigned)) {
  inputPollLightgun = cb;
}

struct Gamepad : Controller {
  Gamepad(unsigned);

  uint8_t data();
  void latch(bool);

private:
  using Controller::latch;

  bool latched;
  unsigned bits;
};

struct Justifier : Controller {
  enum : unsigned {
    X, Y, Trigger, Start,
  };

  Justifier(unsigned, bool);

  uint8_t data() override;
  void latch(bool) override;
  void latch() override;

private:
  const bool chained;  //true if the second justifier is attached to the first
  const unsigned device;
  bool latched;
  unsigned counter;

  bool active;
  struct Player {
    int x;
    int y;
    bool trigger;
    bool start;
  } player1, player2;
};

struct Mouse : Controller {
  Mouse(unsigned);

  uint8_t data();
  void latch(bool);

private:
  using Controller::latch;

  bool latched;
  unsigned bits;
  unsigned speed;  //0 = slow, 1 = normal, 2 = fast

  /* It seems that speed 0 is a linear value from 0-127, and speeds 1 and 2
     use a LUT where values above 7 are capped to the value of the last item in
     the LUT for the respective speed. Further research may be required.
     https://snes.nesdev.org/wiki/Mouse
  */
  /*unsigned speedlut[2][8]{
    {0, 1, 2, 3, 8, 10, 12, 21},
    {0, 1, 4, 9, 12, 20, 24, 28}
  };*/
};

struct SuperMultitap : Controller {
  SuperMultitap(unsigned);

  uint8_t data();
  void latch(bool);

private:
  using Controller::latch;

  bool latched;
  unsigned counter1;
  unsigned counter2;

  struct Gamepad {
    unsigned bits;
  } gamepads[4];
};

struct SuperScope : Controller {
  enum : unsigned {
    X, Y, Trigger, Cursor, Turbo, Pause,
  };

  SuperScope(unsigned);

  uint8_t data() override;
  void latch(bool) override;
  void latch() override;

private:
  bool latched;
  unsigned counter;
  unsigned bits;

  int x;
  int y;

  bool trigger;
  bool cursor;
  bool turbo;
  bool pause;
  bool offscreen;

  bool oldturbo;
  bool triggerlock;
  bool pauselock;
};

ControllerPort controllerPort1;
ControllerPort controllerPort2;

Controller::Controller(unsigned deviceID) : port(deviceID) {
}

Controller::~Controller() {
}

bool Controller::iobit() {
  switch(port) {
    case ID::Port::Controller1: return cpu.pio() & 0x40;
    case ID::Port::Controller2: return cpu.pio() & 0x80;
  }
  return 0; // unreachable
}

void Controller::iobit(bool data) {
  switch(port) {
    case ID::Port::Controller1: bus.write(0x4201, (cpu.pio() & ~0x40) | (data << 6)); break;
    case ID::Port::Controller2: bus.write(0x4201, (cpu.pio() & ~0x80) | (data << 7)); break;
  }
}

void ControllerPort::connect(unsigned deviceID) {
  if(system.loaded()) {
    delete device;

    switch(deviceID) { default:
      case ID::Device::None: device = new Controller(port); break;
      case ID::Device::Gamepad: device = new Gamepad(port); break;
      case ID::Device::Mouse: device = new Mouse(port); break;
      case ID::Device::SuperMultitap: device = new SuperMultitap(port); break;
      case ID::Device::SuperScope: device = new SuperScope(port); break;
      case ID::Device::Justifier: device = new Justifier(port, false); break;
      case ID::Device::Justifiers: device = new Justifier(port, true); break;
    }
  }
}

void ControllerPort::power(unsigned deviceID) {
  this->port = deviceID;
}

void ControllerPort::unload() {
  delete device;
  device = nullptr;
}

void ControllerPort::serialize(serializer& s) {
  if (s.mode()) {}
}

Gamepad::Gamepad(unsigned deviceID) : Controller(deviceID) {
  latched = 0;
}

uint8_t Gamepad::data() {
  if (latched) {
    bits = inputPollGamepad(port);
  }

  /* Additional reads past the first 16 return 1s on official controllers.
     Third party controllers may return 0s instead.
     Shift the MSB out and return it, replace the LSB with 1.
  */
  bits = (bits << 1) | 1;
  return (bits >> 16) & 1;
}

void Gamepad::latch(bool data) {
  if(latched != data) {
    latched = data;

    if(latched == 0) {
      /* JOY1H       JOY1L
         $4219       $4218
      15  bit  8   7  bit  0
       ---- ----   ---- ----
       BYsS UDLR   AXlr 0000
       |||| ||||   |||| ||||
       |||| ||||   |||| ++++- Signature
       |||| ||||   ||++------ L/R shoulder buttons
       |||| ||||   ++-------- A/X buttons
       |||| ++++------------- D-pad
       ||++------------------ Select (s) and Start (S)
       ++-------------------- B/Y buttons
      */
      bits = inputPollGamepad(port);
    }
  }
}

Justifier::Justifier(unsigned deviceID, bool chain):
Controller(deviceID),
chained(chain),
device(!chained ? ID::Device::Justifier : ID::Device::Justifiers)
{
  latched = 0;
  counter = 0;
  active = 0;

  player1.x = 256 / 2;
  player1.y = 240 / 2;
  player1.trigger = false;
  player1.start = false;

  player2.x = 256 / 2;
  player2.y = 240 / 2;
  player2.trigger = false;
  player2.start = false;

  if(chained == false) {
    player2.x = -1;
    player2.y = -1;
  } else {
    player1.x -= 16;
    player2.x += 16;
  }
}

uint8_t Justifier::data() {
  if(counter >= 32) return 1;

  if(counter == 0) {
    player1.trigger = inputPollLightgun(port, Trigger);
    player1.start   = inputPollLightgun(port, Start);
  }

  /*if(counter == 0 && chained) {
    player2.trigger = inputPoll(port, device, 4 + Trigger);
    player2.start   = inputPoll(port, device, 4 + Start);
  }*/

  switch(counter++) {
    case  0: return 0;
    case  1: return 0;
    case  2: return 0;
    case  3: return 0;
    case  4: return 0;
    case  5: return 0;
    case  6: return 0;
    case  7: return 0;
    case  8: return 0;
    case  9: return 0;
    case 10: return 0;
    case 11: return 0;

    case 12: return 1;  //signature
    case 13: return 1;  // ||
    case 14: return 1;  // ||
    case 15: return 0;  // ||

    case 16: return 0;
    case 17: return 1;
    case 18: return 0;
    case 19: return 1;
    case 20: return 0;
    case 21: return 1;
    case 22: return 0;
    case 23: return 1;

    case 24: return player1.trigger;
    case 25: return player2.trigger;
    case 26: return player1.start;
    case 27: return player2.start;
    case 28: return active;

    case 29: return 0;
    case 30: return 0;
    case 31: return 0;
  }

  return 0; // unreachable
}

void Justifier::latch(bool data) {
  if(latched != data) {
    latched = data;
    counter = 0;
    //toggle between both controllers, even when unchained
    if(latched == 0)
      active = !active;
  }
}

void Justifier::latch() {
  if(!active) {
    player1.x = inputPollLightgun(port, X);
    player1.y = inputPollLightgun(port, Y);
    bool offscreen = (player1.x < 0 || player1.y < 0 || player1.x >= 256 || player1.y >= (int)ppu.vdisp());
    if(!offscreen) ppu.latchCounters(player1.x, player1.y);
  }
  /*else {
    player2.x = inputPoll(port, device, 4 + X);
    player2.y = inputPoll(port, device, 4 + Y);
    bool offscreen = (player2.x < 0 || player2.y < 0 || player2.x >= 256 || player2.y >= (int)ppu.vdisp());
    if(!offscreen) ppu.latchCounters(player2.x, player2.y);
  }*/
}

Mouse::Mouse(unsigned deviceID) : Controller(deviceID) {
  latched = 0;
  speed = 0;
}

uint8_t Mouse::data() {
  if(latched == 1) {
    speed = (speed + 1) % 3;
    return 0;
  }

  unsigned ret = bits >> 31;
  bits = (bits << 1) | 1;
  return ret;
}

void Mouse::latch(bool data) {
  if(latched != data) {
    latched = data;

    int x = inputPollMouse(port, 0); // X relative motion
    int y = inputPollMouse(port, 1); // Y relative motion
    int b = inputPollMouse(port, 2); // Buttons

    /* 76543210  First byte
       ++++++++- Always zero: 00000000

       76543210  Second byte
       ||||++++- Signature: 0001
       ||++----- Current sensitivity (0: low; 1: medium; 2: high)
       |+------- Left button (1: pressed)
       +-------- Right button (1: pressed)

       76543210  Third byte
       |+++++++- Vertical displacement since last read
       +-------- Direction (1: up; 0: down)

       76543210  Fourth byte
       |+++++++- Horizontal displacement since last read
       +-------- Direction (1: left; 0: right)
    */
    bits = 0x10000; // Signature
    bits |= (speed << 20);
    bits |= b;

    if (x < 0) {
        bits |= (1 << 7); // Left
        x = -x; // abs
    }

    if (y < 0) {
        bits |= (1 << 15); // Up
        y = -y; // abs
    }

    /* Do not use the speed LUT because though it is more accurate, it leads to
       a bad experience in the context of emulation.
    */
    /*if (speed) {
      x = speedlut[speed - 1][std::min(7, x)];
      y = speedlut[speed - 1][std::min(7, y)];
    }
    else {
      x = std::min(127, x);
      y = std::min(127, y);
    }*/
    if (speed) {
      x *= (speed + 0.5);
      y *= (speed + 0.5);
    }

    x = std::min(127, x);
    y = std::min(127, y);

    bits |= x;
    bits |= (y << 8);
  }
}

SuperMultitap::SuperMultitap(unsigned deviceID) : Controller(deviceID) {
  latched = 0;
  counter1 = 0;
  counter2 = 0;
}

uint8_t SuperMultitap::data() {
  if(latched) return 2;  //device detection

  unsigned a = 0, b = 1;

  if(iobit()) {
    a = 0;  //controller 2
    b = 1;  //controller 3
  } else {
    a = 2;  //controller 4
    b = 3;  //controller 5
  }

  Gamepad& padA = gamepads[a];
  Gamepad& padB = gamepads[b];

  padA.bits = (padA.bits << 1) | 1;
  padB.bits = (padB.bits << 1) | 1;

  return ((padA.bits >> 16) & 1) | ((padB.bits >> 15) & 2);
}

void SuperMultitap::latch(bool data) {
  if(latched != data) {
    latched = data;
    counter1 = 0;
    counter2 = 0;

    if(latched == 0) {
      for(unsigned id = 0; id < 4; ++id) {
        gamepads[id].bits = inputPollGamepad(id + 1); // Start from gamepad 2
      }
    }
  }
}

//The Super Scope is a light-gun: it detects the CRT beam cannon position,
//and latches the counters by toggling iobit. This only works on controller
//port 2, as iobit there is connected to the PPU H/V counter latch.
//(PIO $4201.d7)

//It is obviously not possible to perfectly simulate an IR light detecting
//a CRT beam cannon, hence this class will read the PPU raster counters.

//A Super Scope can still technically be used in port 1, however it would
//require manual polling of PIO ($4201.d6) to determine when iobit was written.
//Note that no commercial game ever utilizes a Super Scope in port 1.

// TODO: Actually make this accurate. It is a hacked up mess right now.

SuperScope::SuperScope(unsigned deviceID) : Controller(deviceID) {
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
}

uint8_t SuperScope::data() {
  if(counter >= 8) return 1;

  if(counter == 0) {
    counter = 1;
    //turbo is a switch; toggle is edge sensitive
    bool newturbo = inputPollLightgun(port, Turbo);
    if(newturbo && !oldturbo) {
      turbo = !turbo;  //toggle state
    }
    oldturbo = newturbo;

    //trigger is a button
    //if turbo is active, trigger is level sensitive; otherwise, it is edge sensitive
    trigger = false;
    bool newtrigger = inputPollLightgun(port, Trigger);
    if(newtrigger && (turbo || !triggerlock)) {
      trigger = true;
      triggerlock = true;
    } else if(!newtrigger) {
      triggerlock = false;
    }

    //cursor is a button; it is always level sensitive
    cursor = inputPollLightgun(port, Cursor);

    //pause is a button; it is always edge sensitive
    pause = false;
    bool newpause = inputPollLightgun(port, Pause);
    if(newpause && !pauselock) {
      pause = true;
      pauselock = true;
    } else if(!newpause) {
      pauselock = false;
    }

    // Changes were made here because some games need their coords offset JOLLYGOOD
    offscreen = (x < -16 || y < -16 || x >= 512 || y >= 480);

    bits = (offscreen ? 0 : (trigger << 15)) | 0xff;
    bits |= (cursor << 14);
    bits |= (turbo << 13);
    bits |= (pause << 12);
    bits |= (offscreen << 9);
  }

  bits = (bits << 1) | 1;
  return (bits >> 16) & 1;
}

void SuperScope::latch(bool data) {
  if(latched != data) {
    latched = data;
    counter = 0;
  }
}

void SuperScope::latch() {
  x = inputPollLightgun(port, X);
  y = inputPollLightgun(port, Y);
  offscreen = (x < 0 || y < 0 || x >= 512 || y >= 480);
  if(!offscreen) ppu.latchCounters(x, y);
}

}
