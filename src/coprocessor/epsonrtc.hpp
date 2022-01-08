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

  uint32_t clocks;
  unsigned seconds;

  uint8_t chipselect;
  enum class State : unsigned { Mode, Seek, Read, Write } state;
  uint8_t mdr;
  uint8_t offset;
  unsigned wait;
  uint8_t ready;
  uint8_t holdtick;

  nall::Natural< 4> secondlo;
  nall::Natural< 3> secondhi;
  uint8_t batteryfailure;

  nall::Natural< 4> minutelo;
  nall::Natural< 3> minutehi;
  uint8_t resync;

  nall::Natural< 4> hourlo;
  nall::Natural< 2> hourhi;
  uint8_t meridian;

  nall::Natural< 4> daylo;
  nall::Natural< 2> dayhi;
  uint8_t dayram;

  nall::Natural< 4> monthlo;
  uint8_t monthhi;
  nall::Natural< 2> monthram;

  nall::Natural< 4> yearlo;
  nall::Natural< 4> yearhi;

  nall::Natural< 3> weekday;

  uint8_t hold;
  uint8_t calendar;
  uint8_t irqflag;
  uint8_t roundseconds;

  uint8_t irqmask;
  uint8_t irqduty;
  nall::Natural< 2> irqperiod;

  uint8_t pause;
  uint8_t stop;
  uint8_t atime;  //astronomical time (24-hour mode)
  uint8_t test;

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
