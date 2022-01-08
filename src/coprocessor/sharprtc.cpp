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

#include "../cpu.hpp"

#include "sharprtc.hpp"

namespace SuperFamicom {

uint8_t SharpRTC::rtcRead(uint8_t addr) {
  switch(addr & 0x0f) {
  case  0: return (second % 10) & 0x0f;
  case  1: return (second / 10) & 0x0f;
  case  2: return (minute % 10) & 0x0f;
  case  3: return (minute / 10) & 0x0f;
  case  4: return (hour % 10) & 0x0f;
  case  5: return (hour / 10) & 0x0f;
  case  6: return (day % 10) & 0x0f;
  case  7: return (day / 10) & 0x0f;
  case  8: return (month) & 0x0f;
  case  9: return (year % 10) & 0x0f;
  case 10: return (year / 10 % 10) & 0x0f;
  case 11: return (year / 100) & 0x0f;
  case 12: return (weekday) & 0x0f;
  default: return 0;
  }
}

void SharpRTC::rtcWrite(uint8_t addr, uint8_t data) {
  switch(addr & 0x0f) {
  case  0: second = second / 10 * 10 + (data & 0x0f); break;
  case  1: second = (data & 0x0f) * 10 + second % 10; break;
  case  2: minute = minute / 10 * 10 + (data & 0x0f); break;
  case  3: minute = (data & 0x0f) * 10 + minute % 10; break;
  case  4: hour = hour / 10 * 10 + (data & 0x0f); break;
  case  5: hour = (data & 0x0f) * 10 + hour % 10; break;
  case  6: day = day / 10 * 10 + (data & 0x0f); break;
  case  7: day = (data & 0x0f) * 10 + day % 10; break;
  case  8: month = (data & 0x0f); break;
  case  9: year = year / 10 * 10 + (data & 0x0f); break;
  case 10: year = year / 100 * 100 + (data & 0x0f) * 10 + year % 10; break;
  case 11: year = (data & 0x0f) * 100 + year % 100; break;
  case 12: weekday = (data & 0x0f); break;
  }
}

void SharpRTC::load(const uint8_t* data) {
  for(unsigned byte = 0; byte < 8; ++byte) {
    rtcWrite(byte * 2 + 0, data[byte] & 0x0f);
    rtcWrite(byte * 2 + 1, data[byte] >> 4);
  }

  uint64_t timestamp = 0;
  for(unsigned byte = 0; byte < 8; ++byte) {
    timestamp |= (uint64_t)data[8 + byte] << (byte * 8);
  }

  uint64_t diff = (uint64_t)time(0) - timestamp;
  while(diff >= 60 * 60 * 24) { tickDay(); diff -= 60 * 60 * 24; }
  while(diff >= 60 * 60) { tickHour(); diff -= 60 * 60; }
  while(diff >= 60) { tickMinute(); diff -= 60; }
  while(diff--) tickSecond();
}

void SharpRTC::save(uint8_t* data) {
  for(unsigned byte = 0; byte < 8; ++byte) {
    data[byte]  = rtcRead(byte * 2 + 0) << 0;
    data[byte] |= rtcRead(byte * 2 + 1) << 4;
  }

  uint64_t timestamp = (uint64_t)time(nullptr);
  for(unsigned byte = 0; byte < 8; ++byte) {
    data[8 + byte] = timestamp;
    timestamp >>= 8;
  }
}

const unsigned SharpRTC::daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void SharpRTC::tickSecond() {
  if(++second < 60) return;
  second = 0;
  tickMinute();
}

void SharpRTC::tickMinute() {
  if(++minute < 60) return;
  minute = 0;
  tickHour();
}

void SharpRTC::tickHour() {
  if(++hour < 24) return;
  hour = 0;
  tickDay();
}

void SharpRTC::tickDay() {
  unsigned days = daysInMonth[(month - 1) % 12];

  //add one day in February for leap years
  if(month == 2) {
         if(year % 400 == 0) days++;
    else if(year % 100 == 0);
    else if(year %   4 == 0) days++;
  }

  if(day++ < days) return;
  day = 1;
  tickMonth();
}

void SharpRTC::tickMonth() {
  if(month++ < 12) return;
  month = 1;
  tickYear();
}

void SharpRTC::tickYear() {
  ++year;
  year = year & 0xfff;
}

//returns day of week for specified date
//eg 0 = Sunday, 1 = Monday, ... 6 = Saturday
//usage: calculate_weekday(2008, 1, 1) returns weekday of January 1st, 2008
unsigned SharpRTC::calculateWeekday(int year, int month, int day) {
  int y = 1000, m = 1;  //SharpRTC epoch is 1000-01-01
  unsigned sum = 0;          //number of days passed since epoch

  year = std::max(1000, year);
  month = std::max(1, std::min(12, month));
  day = std::max(1, std::min(31, day));

  while(y < year) {
    bool leapyear = false;
    if(y % 4 == 0) {
      leapyear = true;
      if(y % 100 == 0 && y % 400 != 0) leapyear = false;
    }
    sum += 365 + leapyear;
    y++;
  }

  while(m < month) {
    unsigned days = daysInMonth[(m - 1) % 12];
    bool leapyearmonth = false;
    if(days == 28) {
      if(y % 4 == 0) {
        leapyearmonth = true;
        if(y % 100 == 0 && y % 400 != 0) leapyearmonth = false;
      }
    }
    sum += days + leapyearmonth;
    m++;
  }

  sum += day - 1;
  return (sum + 3) % 7;  //1000-01-01 was a Wednesday
}

void SharpRTC::serialize(serializer& s) {
  Thread::serialize(s);

  unsigned st = (unsigned)state;
  s.integer((unsigned&)st);
  s.integer(index);

  s.integer(second);
  s.integer(minute);
  s.integer(hour);
  s.integer(day);
  s.integer(month);
  s.integer(year);
  s.integer(weekday);
}

SharpRTC sharprtc;

void SharpRTC::synchronizeCPU() {
  if(clock >= 0) scheduler.resume(cpu.thread);
}

void SharpRTC::Enter() {
  while(true) {
    scheduler.synchronize();
    sharprtc.main();
  }
}

void SharpRTC::main() {
  tickSecond();

  step(1);
  synchronizeCPU();
}

void SharpRTC::step(unsigned clocks) {
  clock += clocks * (uint64_t)cpu.frequency;
}

void SharpRTC::initialize() {
  second = 0;
  minute = 0;
  hour = 0;
  day = 0;
  month = 0;
  year = 0;
  weekday = 0;
}

void SharpRTC::power() {
  create(SharpRTC::Enter, 1);

  state = State::Read;
  index = -1;
}

void SharpRTC::synchronize(uint64_t timestamp) {
  time_t systime = timestamp;
  tm* timeinfo = localtime(&systime);

  second = std::min(59, timeinfo->tm_sec);
  minute = timeinfo->tm_min;
  hour = timeinfo->tm_hour;
  day = timeinfo->tm_mday;
  month = 1 + timeinfo->tm_mon;
  year = 900 + timeinfo->tm_year;
  weekday = timeinfo->tm_wday;
}

uint8_t SharpRTC::read(unsigned addr, uint8_t data) {
  addr &= 1;

  if(addr == 0) {
    if(state != State::Read) return 0;

    if(index < 0) {
      ++index;
      return 15;
    } else if(index > 12) {
      index = -1;
      return 15;
    } else {
      return rtcRead(index++);
    }
  }

  return data;
}

void SharpRTC::write(unsigned addr, uint8_t data) {
  addr &= 1, data &= 15;

  if(addr == 1) {
    if(data == 0x0d) {
      state = State::Read;
      index = -1;
      return;
    }

    if(data == 0x0e) {
      state = State::Command;
      return;
    }

    if(data == 0x0f) return;  //unknown behavior

    if(state == State::Command) {
      if(data == 0) {
        state = State::Write;
        index = 0;
      } else if(data == 4) {
        state = State::Ready;
        index = -1;
        //reset time
        second = 0;
        minute = 0;
        hour = 0;
        day = 0;
        month = 0;
        year = 0;
        weekday = 0;
      } else {
        //unknown behavior
        state = State::Ready;
      }
      return;
    }

    if(state == State::Write) {
      if(index >= 0 && index < 12) {
        rtcWrite(index++, data & 0x0f);
        if(index == 12) {
          //day of week is automatically calculated and written
          weekday = calculateWeekday(1000 + year, month, day);
        }
      }
      return;
    }
  }
}

}
