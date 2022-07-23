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
  [[noreturn]] static void Enter();
  void main();
  void step(unsigned);

  void initialize();
  void power();
  void synchronize(uint64_t);

  uint8_t read(unsigned, uint8_t);
  void write(unsigned, uint8_t);

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

  uint8_t secondlo;
  uint8_t secondhi;
  uint8_t batteryfailure;

  uint8_t minutelo;
  uint8_t minutehi;
  uint8_t resync;

  uint8_t hourlo;
  uint8_t hourhi;
  uint8_t meridian;

  uint8_t daylo;
  uint8_t dayhi;
  uint8_t dayram;

  uint8_t monthlo;
  uint8_t monthhi;
  uint8_t monthram;

  uint8_t yearlo;
  uint8_t yearhi;

  uint8_t weekday;

  uint8_t hold;
  uint8_t calendar;
  uint8_t irqflag;
  uint8_t roundseconds;

  uint8_t irqmask;
  uint8_t irqduty;
  uint8_t irqperiod;

  uint8_t pause;
  uint8_t stop;
  uint8_t atime;  //astronomical time (24-hour mode)
  uint8_t test;

  //memory.cpp
  void rtcReset();
  uint8_t rtcRead(uint8_t);
  void rtcWrite(uint8_t, uint8_t);

  void load(const uint8_t*);
  void save(uint8_t*);

  //time.cpp
  void irq(uint8_t);
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
