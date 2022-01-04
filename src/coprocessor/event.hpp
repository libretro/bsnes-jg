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

//HLE of the NEC uPD78P214GC processor found on SNES-EVENT PCBs, used by:
//* Campus Challenge '92
//* PowerFest '94

//The NEC uPD78214 family are 8-bit microprocessors containing:
//* UART/CSI serial interface
//* ALU (MUL, DIV, BCD)
//* interrupts (12 internal; 7 external; 2 priority levels)
//* 16384 x 8-bit ROM
//*   512 x 8-bit RAM
//*     4 x timer/counters

//None of the SNES-EVENT games have had their uPD78214 firmware dumped.
//As such, our only option is very basic high-level emulation, provided here.

struct Event : Thread {
  //event.cpp
  void synchronizeCPU();
  static void Enter();
  void main();
  void step(unsigned clocks);
  void unload();
  void power();

  uint8_t mcuRead(unsigned addr, uint8_t);
  void mcuWrite(unsigned addr, uint8_t);

  uint8_t read(unsigned addr, uint8_t data);
  void write(unsigned addr, uint8_t data);

  //serialization.cpp
  void serialize(serializer&);

public:
  ReadableMemory rom[4];

  enum class Board : unsigned { Unknown, CampusChallenge92, PowerFest94 } board;
  unsigned timer;

private:
  uint8_t status;
  uint8_t select;

  bool timerActive;
  bool scoreActive;

  unsigned timerSecondsRemaining;
  unsigned scoreSecondsRemaining;
};

extern Event event;

}
