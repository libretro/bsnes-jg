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

#include <string>

namespace SuperFamicom {

struct ID {
  enum : unsigned {
    System,
    SuperFamicom,
    GameBoy,
    BSMemory,
    SufamiTurboA,
    SufamiTurboB,
  };

  struct Port { enum : unsigned {
    Controller1,
    Controller2,
    Expansion,
  };};

  struct Device { enum : unsigned {
    None,
    Gamepad,
    Mouse,
    SuperMultitap,
    SuperScope,
    Justifier,
    Justifiers,

    Satellaview,
    S21FX,
  };};
};

struct Configuration {
  struct System {
    struct CPU {
      unsigned version = 2;
    } cpu;
    struct PPU1 {
      unsigned version = 1;
      struct VRAM {
        unsigned size = 0x10000;
      } vram;
    } ppu1;
    struct PPU2 {
      unsigned version = 3;
    } ppu2;
    struct Serialization {
      bool faststates;
    } serialization;
  } system;

  bool hotfixes = true;
  std::string entropy = "Low";
  struct CPU {
    unsigned overclock = 100;
    bool fastMath = false;
  } cpu;
  struct Coprocessor {
    bool delayedSync = true;
    bool preferHLE = false;
  } coprocessor;
  struct SA1 {
    unsigned overclock = 100;
  } sa1;
  struct SuperFX {
    unsigned overclock = 100;
  } superfx;

  unsigned controllerPort1 = ID::Device::Gamepad;
  unsigned controllerPort2 = ID::Device::Gamepad;
  unsigned expansionPort = ID::Device::None;
};

extern Configuration configuration;

}
