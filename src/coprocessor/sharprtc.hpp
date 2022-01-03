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

struct SharpRTC : Thread {
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

  enum class State : unsigned { Ready, Command, Read, Write } state;
  int index;

  unsigned second;
  unsigned minute;
  unsigned hour;
  unsigned day;
  unsigned month;
  unsigned year;
  unsigned weekday;

  //memory.cpp
  nall::Natural< 4> rtcRead(nall::Natural< 4> addr);
  void rtcWrite(nall::Natural< 4> addr, nall::Natural< 4> data);

  void load(const uint8_t* data);
  void save(uint8_t* data);

  //time.cpp
  static const unsigned daysInMonth[12];
  void tickSecond();
  void tickMinute();
  void tickHour();
  void tickDay();
  void tickMonth();
  void tickYear();

  unsigned calculateWeekday(int year, int month, int day);
};

extern SharpRTC sharprtc;
