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

#include "serializer.hpp"
#include "cpu.hpp"
#include "emulator.hpp"
#include "memory.hpp"
#include "settings.hpp"

#include "controller.hpp"

static int16_t (*inputPoll)(unsigned, unsigned, unsigned);

namespace SuperFamicom {

void setInputPoll(int16_t (*cb)(unsigned, unsigned, unsigned)) {
  inputPoll = cb;
}

struct Gamepad : Controller {
  enum : unsigned {
    Up, Down, Left, Right, B, A, Y, X, L, R, Select, Start,
  };

  Gamepad(unsigned);

  uint8_t data();
  void latch(bool);

private:
  bool latched;
  unsigned counter;

  bool b, y, select, start;
  bool up, down, left, right;
  bool a, x, l, r;
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
  unsigned prev;

  bool active;
  struct Player {
    int x;
    int y;
    bool trigger;
    bool start;
  } player1, player2;
};

struct Mouse : Controller {
  enum : unsigned {
    X, Y, Left, Right,
  };

  Mouse(unsigned);

  uint8_t data();
  void latch(bool);

private:
  bool latched;
  unsigned counter;

  unsigned speed;  //0 = slow, 1 = normal, 2 = fast
  int  x;      //x-coordinate
  int  y;      //y-coordinate
  bool dx;     //x-direction
  bool dy;     //y-direction
  bool l;      //left button
  bool r;      //right button
};

struct SuperMultitap : Controller {
  enum : unsigned {
    Up, Down, Left, Right, B, A, Y, X, L, R, Select, Start,
  };

  SuperMultitap(unsigned);

  uint8_t data();
  void latch(bool);

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

  unsigned prev;
};

ControllerPort controllerPort1;
ControllerPort controllerPort2;

Controller::Controller(unsigned port) : port(port) {
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
  if(!system.loaded()) return;
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

void ControllerPort::power(unsigned deviceID) {
  this->port = deviceID;
}

void ControllerPort::unload() {
  delete device;
  device = nullptr;
}

void ControllerPort::serialize(serializer& s) {
}

Gamepad::Gamepad(unsigned port) : Controller(port) {
  latched = 0;
  counter = 0;
}

uint8_t Gamepad::data() {
  if(counter >= 16) return 1;
  if(latched == 1) return inputPoll(port, ID::Device::Gamepad, B);

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
    b      = inputPoll(port, ID::Device::Gamepad, B);
    y      = inputPoll(port, ID::Device::Gamepad, Y);
    select = inputPoll(port, ID::Device::Gamepad, Select);
    start  = inputPoll(port, ID::Device::Gamepad, Start);
    up     = inputPoll(port, ID::Device::Gamepad, Up);
    down   = inputPoll(port, ID::Device::Gamepad, Down);
    left   = inputPoll(port, ID::Device::Gamepad, Left);
    right  = inputPoll(port, ID::Device::Gamepad, Right);
    a      = inputPoll(port, ID::Device::Gamepad, A);
    x      = inputPoll(port, ID::Device::Gamepad, X);
    l      = inputPoll(port, ID::Device::Gamepad, L);
    r      = inputPoll(port, ID::Device::Gamepad, R);
  }
}

Justifier::Justifier(unsigned port, bool chained):
Controller(port),
chained(chained),
device(!chained ? ID::Device::Justifier : ID::Device::Justifiers)
{
  latched = 0;
  counter = 0;
  active = 0;
  prev = 0;

  player1.x = 256 / 2;
  player1.y = 240 / 2;
  player1.trigger = false;
  player2.start = false;

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
    player1.trigger = inputPoll(port, device, 0 + Trigger);
    player1.start   = inputPoll(port, device, 0 + Start);
  }

  if(counter == 0 && chained) {
    player2.trigger = inputPoll(port, device, 4 + Trigger);
    player2.start   = inputPoll(port, device, 4 + Start);
  }

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
  if(latched == data) return;
  latched = data;
  counter = 0;
  if(latched == 0) active = !active;  //toggle between both controllers, even when unchained
}

void Justifier::latch() {
  if(!active) {
    player1.x = inputPoll(port, device, 0 + X);
    player1.y = inputPoll(port, device, 0 + Y);
    bool offscreen = (player1.x < 0 || player1.y < 0 || player1.x >= 256 || player1.y >= (int)ppu.vdisp());
    if(!offscreen) ppu.latchCounters(player1.x, player1.y);
  }
  else {
    player2.x = inputPoll(port, device, 4 + X);
    player2.y = inputPoll(port, device, 4 + Y);
    bool offscreen = (player2.x < 0 || player2.y < 0 || player2.x >= 256 || player2.y >= (int)ppu.vdisp());
    if(!offscreen) ppu.latchCounters(player2.x, player2.y);
  }
}

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

  x = inputPoll(port, ID::Device::Mouse, X);  //-n = left, 0 = center, +n = right
  y = inputPoll(port, ID::Device::Mouse, Y);  //-n = up,   0 = center, +n = down
  l = inputPoll(port, ID::Device::Mouse, Left);
  r = inputPoll(port, ID::Device::Mouse, Right);

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

SuperMultitap::SuperMultitap(unsigned port) : Controller(port) {
  latched = 0;
  counter1 = 0;
  counter2 = 0;
}

uint8_t SuperMultitap::data() {
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
      gamepad.b      = inputPoll(port, ID::Device::SuperMultitap, id * 12 + B);
      gamepad.y      = inputPoll(port, ID::Device::SuperMultitap, id * 12 + Y);
      gamepad.select = inputPoll(port, ID::Device::SuperMultitap, id * 12 + Select);
      gamepad.start  = inputPoll(port, ID::Device::SuperMultitap, id * 12 + Start);
      gamepad.up     = inputPoll(port, ID::Device::SuperMultitap, id * 12 + Up);
      gamepad.down   = inputPoll(port, ID::Device::SuperMultitap, id * 12 + Down);
      gamepad.left   = inputPoll(port, ID::Device::SuperMultitap, id * 12 + Left);
      gamepad.right  = inputPoll(port, ID::Device::SuperMultitap, id * 12 + Right);
      gamepad.a      = inputPoll(port, ID::Device::SuperMultitap, id * 12 + A);
      gamepad.x      = inputPoll(port, ID::Device::SuperMultitap, id * 12 + X);
      gamepad.l      = inputPoll(port, ID::Device::SuperMultitap, id * 12 + L);
      gamepad.r      = inputPoll(port, ID::Device::SuperMultitap, id * 12 + R);
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
    bool newturbo = inputPoll(port, ID::Device::SuperScope, Turbo);
    if(newturbo && !oldturbo) {
      turbo = !turbo;  //toggle state
    }
    oldturbo = newturbo;

    //trigger is a button
    //if turbo is active, trigger is level sensitive; otherwise, it is edge sensitive
    trigger = false;
    bool newtrigger = inputPoll(port, ID::Device::SuperScope, Trigger);
    if(newtrigger && (turbo || !triggerlock)) {
      trigger = true;
      triggerlock = true;
    } else if(!newtrigger) {
      triggerlock = false;
    }

    //cursor is a button; it is always level sensitive
    cursor = inputPoll(port, ID::Device::SuperScope, Cursor);

    //pause is a button; it is always edge sensitive
    pause = false;
    bool newpause = inputPoll(port, ID::Device::SuperScope, Pause);
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
  x = inputPoll(port, ID::Device::SuperScope, X);
  y = inputPoll(port, ID::Device::SuperScope, Y);
  offscreen = (x < 0 || y < 0 || x >= 512 || y >= 480);
  if(!offscreen) ppu.latchCounters(x, y);
}

}
