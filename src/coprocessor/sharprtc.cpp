#include "../sfc.hpp"

namespace SuperFamicom {

auto SharpRTC::rtcRead(nall::Natural< 4> addr) -> nall::Natural< 4> {
  switch(addr) {
  case  0: return second % 10;
  case  1: return second / 10;
  case  2: return minute % 10;
  case  3: return minute / 10;
  case  4: return hour % 10;
  case  5: return hour / 10;
  case  6: return day % 10;
  case  7: return day / 10;
  case  8: return month;
  case  9: return year % 10;
  case 10: return year / 10 % 10;
  case 11: return year / 100;
  case 12: return weekday;
  default: return 0;
  }
}

auto SharpRTC::rtcWrite(nall::Natural< 4> addr, nall::Natural< 4> data) -> void {
  switch(addr) {
  case  0: second = second / 10 * 10 + data; break;
  case  1: second = data * 10 + second % 10; break;
  case  2: minute = minute / 10 * 10 + data; break;
  case  3: minute = data * 10 + minute % 10; break;
  case  4: hour = hour / 10 * 10 + data; break;
  case  5: hour = data * 10 + hour % 10; break;
  case  6: day = day / 10 * 10 + data; break;
  case  7: day = data * 10 + day % 10; break;
  case  8: month = data; break;
  case  9: year = year / 10 * 10 + data; break;
  case 10: year = year / 100 * 100 + data * 10 + year % 10; break;
  case 11: year = data * 100 + year % 100; break;
  case 12: weekday = data; break;
  }
}

auto SharpRTC::load(const uint8_t* data) -> void {
  for(auto byte : nall::range(8)) {
    rtcWrite(byte * 2 + 0, data[byte] >> 0);
    rtcWrite(byte * 2 + 1, data[byte] >> 4);
  }

  uint64_t timestamp = 0;
  for(auto byte : nall::range(8)) {
    timestamp |= data[8 + byte] << (byte * 8);
  }

  uint64_t diff = (uint64_t)time(0) - timestamp;
  while(diff >= 60 * 60 * 24) { tickDay(); diff -= 60 * 60 * 24; }
  while(diff >= 60 * 60) { tickHour(); diff -= 60 * 60; }
  while(diff >= 60) { tickMinute(); diff -= 60; }
  while(diff--) tickSecond();
}

auto SharpRTC::save(uint8_t* data) -> void {
  for(auto byte : nall::range(8)) {
    data[byte]  = rtcRead(byte * 2 + 0) << 0;
    data[byte] |= rtcRead(byte * 2 + 1) << 4;
  }

  uint64_t timestamp = (uint64_t)time(nullptr);
  for(auto byte : nall::range(8)) {
    data[8 + byte] = timestamp;
    timestamp >>= 8;
  }
}

const unsigned SharpRTC::daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

auto SharpRTC::tickSecond() -> void {
  if(++second < 60) return;
  second = 0;
  tickMinute();
}

auto SharpRTC::tickMinute() -> void {
  if(++minute < 60) return;
  minute = 0;
  tickHour();
}

auto SharpRTC::tickHour() -> void {
  if(++hour < 24) return;
  hour = 0;
  tickDay();
}

auto SharpRTC::tickDay() -> void {
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

auto SharpRTC::tickMonth() -> void {
  if(month++ < 12) return;
  month = 1;
  tickYear();
}

auto SharpRTC::tickYear() -> void {
  year++;
  year = (nall::Natural<12>)year;
}

//returns day of week for specified date
//eg 0 = Sunday, 1 = Monday, ... 6 = Saturday
//usage: calculate_weekday(2008, 1, 1) returns weekday of January 1st, 2008
auto SharpRTC::calculateWeekday(int year, int month, int day) -> unsigned {
  unsigned y = 1000, m = 1;  //SharpRTC epoch is 1000-01-01
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

auto SharpRTC::serialize(serializer& s) -> void {
  Thread::serialize(s);

  s.integer((unsigned&)state);
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

auto SharpRTC::synchronizeCPU() -> void {
  if(clock >= 0) scheduler.resume(cpu.thread);
}

auto SharpRTC::Enter() -> void {
  while(true) {
    scheduler.synchronize();
    sharprtc.main();
  }
}

auto SharpRTC::main() -> void {
  tickSecond();

  step(1);
  synchronizeCPU();
}

auto SharpRTC::step(unsigned clocks) -> void {
  clock += clocks * (uint64_t)cpu.frequency;
}

auto SharpRTC::initialize() -> void {
  second = 0;
  minute = 0;
  hour = 0;
  day = 0;
  month = 0;
  year = 0;
  weekday = 0;
}

auto SharpRTC::power() -> void {
  create(SharpRTC::Enter, 1);

  state = State::Read;
  index = -1;
}

auto SharpRTC::synchronize(uint64_t timestamp) -> void {
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

auto SharpRTC::read(unsigned addr, uint8_t data) -> uint8_t {
  addr &= 1;

  if(addr == 0) {
    if(state != State::Read) return 0;

    if(index < 0) {
      index++;
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

auto SharpRTC::write(unsigned addr, uint8_t data) -> void {
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
        rtcWrite(index++, data);
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
