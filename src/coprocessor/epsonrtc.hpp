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

//Epson RTC-4513 Real-Time Clock

struct EpsonRTC : Thread {
  void synchronizeCPU();
  static void Enter();
  void main();
  void step(unsigned clocks);

  void initialize();
  void power();
  void synchronize(uint64_t timestamp);

  uint8_t read(unsigned addr, uint8_t data);
  void write(unsigned addr, uint8_t data);

  void serialize(serializer&);

  nall::Natural<21> clocks;
  unsigned seconds;

  nall::Natural< 2> chipselect;
  enum class State : unsigned { Mode, Seek, Read, Write } state;
  nall::Natural< 4> mdr;
  nall::Natural< 4> offset;
  unsigned wait;
  nall::Natural< 1> ready;
  nall::Natural< 1> holdtick;

  nall::Natural< 4> secondlo;
  nall::Natural< 3> secondhi;
  nall::Natural< 1> batteryfailure;

  nall::Natural< 4> minutelo;
  nall::Natural< 3> minutehi;
  nall::Natural< 1> resync;

  nall::Natural< 4> hourlo;
  nall::Natural< 2> hourhi;
  nall::Natural< 1> meridian;

  nall::Natural< 4> daylo;
  nall::Natural< 2> dayhi;
  nall::Natural< 1> dayram;

  nall::Natural< 4> monthlo;
  nall::Natural< 1> monthhi;
  nall::Natural< 2> monthram;

  nall::Natural< 4> yearlo;
  nall::Natural< 4> yearhi;

  nall::Natural< 3> weekday;

  nall::Natural< 1> hold;
  nall::Natural< 1> calendar;
  nall::Natural< 1> irqflag;
  nall::Natural< 1> roundseconds;

  nall::Natural< 1> irqmask;
  nall::Natural< 1> irqduty;
  nall::Natural< 2> irqperiod;

  nall::Natural< 1> pause;
  nall::Natural< 1> stop;
  nall::Natural< 1> atime;  //astronomical time (24-hour mode)
  nall::Natural< 1> test;

  //memory.cpp
  void rtcReset();
  nall::Natural< 4> rtcRead(nall::Natural< 4> addr);
  void rtcWrite(nall::Natural< 4> addr, nall::Natural< 4> data);

  void load(const uint8_t* data);
  void save(uint8_t* data);

  //time.cpp
  void irq(nall::Natural< 2> period);
  void duty();
  void roundSeconds();
  void tick();

  void tickSecond();
  void tickMinute();
  void tickHour();
  void tickDay();
  void tickMonth();
  void tickYear();
};

extern EpsonRTC epsonrtc;

}
