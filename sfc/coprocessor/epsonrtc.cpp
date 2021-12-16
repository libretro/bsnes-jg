#include <sfc/sfc.hpp>

namespace SuperFamicom {

auto EpsonRTC::rtcReset() -> void {
  state = State::Mode;
  offset = 0;

  resync = 0;
  pause = 0;
  test = 0;
}

auto EpsonRTC::rtcRead(uint4 addr) -> uint4 {
  switch(addr) { default:
  case  0: return secondlo;
  case  1: return secondhi | batteryfailure << 3;
  case  2: return minutelo;
  case  3: return minutehi | resync << 3;
  case  4: return hourlo;
  case  5: return hourhi | meridian << 2 | resync << 3;
  case  6: return daylo;
  case  7: return dayhi | dayram << 2 | resync << 3;
  case  8: return monthlo;
  case  9: return monthhi | monthram << 1 | resync << 3;
  case 10: return yearlo;
  case 11: return yearhi;
  case 12: return weekday | resync << 3;
  case 13: {
    uint1 readflag = irqflag & !irqmask;
    irqflag = 0;
    return hold | calendar << 1 | readflag << 2 | roundseconds << 3;
  }
  case 14: return irqmask | irqduty << 1 | irqperiod << 2;
  case 15: return pause | stop << 1 | atime << 2 | test << 3;
  }
}

auto EpsonRTC::rtcWrite(uint4 addr, uint4 data) -> void {
  switch(addr) {
  case 0:
    secondlo = data;
    break;
  case 1:
    secondhi = data;
    batteryfailure = data >> 3;
    break;
  case 2:
    minutelo = data;
    break;
  case 3:
    minutehi = data;
    break;
  case 4:
    hourlo = data;
    break;
  case 5:
    hourhi = data;
    meridian = data >> 2;
    if(atime == 1) meridian = 0;
    if(atime == 0) hourhi &= 1;
    break;
  case 6:
    daylo = data;
    break;
  case 7:
    dayhi = data;
    dayram = data >> 2;
    break;
  case 8:
    monthlo = data;
    break;
  case 9:
    monthhi = data;
    monthram = data >> 1;
    break;
  case 10:
    yearlo = data;
    break;
  case 11:
    yearhi = data;
    break;
  case 12:
    weekday = data;
    break;
  case 13: {
    bool held = hold;
    hold = data;
    calendar = data >> 1;
    roundseconds = data >> 3;
    if(held == 1 && hold == 0 && holdtick == 1) {
      //if a second has passed during hold, increment one second upon resuming
      holdtick = 0;
      tickSecond();
    }
  } break;
  case 14:
    irqmask = data;
    irqduty = data >> 1;
    irqperiod = data >> 2;
    break;
  case 15:
    pause = data;
    stop = data >> 1;
    atime = data >> 2;
    test = data >> 3;
    if(atime == 1) meridian = 0;
    if(atime == 0) hourhi &= 1;
    if(pause) {
      secondlo = 0;
      secondhi = 0;
    }
    break;
  }
}

auto EpsonRTC::load(const uint8_t* data) -> void {
  secondlo = data[0] >> 0;
  secondhi = data[0] >> 4;
  batteryfailure = data[0] >> 7;

  minutelo = data[1] >> 0;
  minutehi = data[1] >> 4;
  resync = data[1] >> 7;

  hourlo = data[2] >> 0;
  hourhi = data[2] >> 4;
  meridian = data[2] >> 6;

  daylo = data[3] >> 0;
  dayhi = data[3] >> 4;
  dayram = data[3] >> 6;

  monthlo = data[4] >> 0;
  monthhi = data[4] >> 4;
  monthram = data[4] >> 5;

  yearlo = data[5] >> 0;
  yearhi = data[5] >> 4;

  weekday = data[6] >> 0;

  hold = data[6] >> 4;
  calendar = data[6] >> 5;
  irqflag = data[6] >> 6;
  roundseconds = data[6] >> 7;

  irqmask = data[7] >> 0;
  irqduty = data[7] >> 1;
  irqperiod = data[7] >> 2;

  pause = data[7] >> 4;
  stop = data[7] >> 5;
  atime = data[7] >> 6;
  test = data[7] >> 7;

  uint64_t timestamp = 0;
  for(auto byte : range(8)) {
    timestamp |= data[8 + byte] << (byte * 8);
  }

  uint64_t diff = (uint64_t)time(0) - timestamp;
  while(diff >= 60 * 60 * 24) { tickDay(); diff -= 60 * 60 * 24; }
  while(diff >= 60 * 60) { tickHour(); diff -= 60 * 60; }
  while(diff >= 60) { tickMinute(); diff -= 60; }
  while(diff--) tickSecond();
}

auto EpsonRTC::save(uint8_t* data) -> void {
  data[0] = secondlo << 0 | secondhi << 4 | batteryfailure << 7;
  data[1] = minutelo << 0 | minutehi << 4 | resync << 7;
  data[2] = hourlo << 0 | hourhi << 4 | meridian << 6 | resync << 7;
  data[3] = daylo << 0 | dayhi << 4 | dayram << 6 | resync << 7;
  data[4] = monthlo << 0 | monthhi << 4 | monthram << 5 | resync << 7;
  data[5] = yearlo << 0 | yearhi << 4;
  data[6] = weekday << 0 | resync << 3 | hold << 4 | calendar << 5 | irqflag << 6 | roundseconds << 7;
  data[7] = irqmask << 0 | irqduty << 1 | irqperiod << 2 | pause << 4 | stop << 5 | atime << 6 | test << 7;

  uint64_t timestamp = (uint64_t)time(0);
  for(auto byte : range(8)) {
    data[8 + byte] = timestamp;
    timestamp >>= 8;
  }
}

auto EpsonRTC::irq(uint2 period) -> void {
  if(stop || pause) return;

  if(period == irqperiod) irqflag = 1;
}

auto EpsonRTC::duty() -> void {
  if(irqduty) irqflag = 0;
}

auto EpsonRTC::roundSeconds() -> void {
  if(roundseconds == 0) return;
  roundseconds = 0;

  if(secondhi >= 3) tickMinute();
  secondlo = 0;
  secondhi = 0;
}

auto EpsonRTC::tick() -> void {
  if(stop || pause) return;

  if(hold) {
    holdtick = 1;
    return;
  }

  resync = 1;
  tickSecond();
}

//below code provides bit-perfect emulation of invalid BCD values on the RTC-4513
//code makes extensive use of variable-length integers (see epsonrtc.hpp for sizes)

auto EpsonRTC::tickSecond() -> void {
  if(secondlo <= 8 || secondlo == 12) {
    secondlo++;
  } else {
    secondlo = 0;
    if(secondhi < 5) {
      secondhi++;
    } else {
      secondhi = 0;
      tickMinute();
    }
  }
}

auto EpsonRTC::tickMinute() -> void {
  if(minutelo <= 8 || minutelo == 12) {
    minutelo++;
  } else {
    minutelo = 0;
    if(minutehi < 5) {
      minutehi++;
    } else {
      minutehi = 0;
      tickHour();
    }
  }
}

auto EpsonRTC::tickHour() -> void {
  if(atime) {
    if(hourhi < 2) {
      if(hourlo <= 8 || hourlo == 12) {
        hourlo++;
      } else {
        hourlo = !(hourlo & 1);
        hourhi++;
      }
    } else {
      if(hourlo != 3 && !(hourlo & 4)) {
        if(hourlo <= 8 || hourlo >= 12) {
          hourlo++;
        } else {
          hourlo = !(hourlo & 1);
          hourhi++;
        }
      } else {
        hourlo = !(hourlo & 1);
        hourhi = 0;
        tickDay();
      }
    }
  } else {
    if(hourhi == 0) {
      if(hourlo <= 8 || hourlo == 12) {
        hourlo++;
      } else {
        hourlo = !(hourlo & 1);
        hourhi ^= 1;
      }
    } else {
      if(hourlo & 1) meridian ^= 1;
      if(hourlo < 2 || hourlo == 4 || hourlo == 5 || hourlo == 8 || hourlo == 12) {
        hourlo++;
      } else {
        hourlo = !(hourlo & 1);
        hourhi ^= 1;
      }
      if(meridian == 0 && !(hourlo & 1)) tickDay();
    }
  }
}

auto EpsonRTC::tickDay() -> void {
  if(calendar == 0) return;
  weekday = (weekday + 1) + (weekday == 6);

  //January - December = 0x01 - 0x09; 0x10 - 0x12
  static const uint daysinmonth[32] = {
    30, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 30, 31, 30,
    31, 30, 31, 30, 31, 30, 31, 30, 31, 30, 31, 30, 31, 30, 31, 30,
  };

  uint days = daysinmonth[monthhi << 4 | monthlo];
  if(days == 28) {
    //add one day for leap years
    if((yearhi & 1) == 0 && ((yearlo - 0) & 3) == 0) days++;
    if((yearhi & 1) == 1 && ((yearlo - 2) & 3) == 0) days++;
  }

  if(days == 28 && (dayhi == 3 || (dayhi == 2 && daylo >= 8))) {
    daylo = 1;
    dayhi = 0;
    return tickMonth();
  }

  if(days == 29 && (dayhi == 3 || (dayhi == 2 && (daylo > 8 && daylo != 12)))) {
    daylo = 1;
    dayhi = 0;
    return tickMonth();
  }

  if(days == 30 && (dayhi == 3 || (dayhi == 2 && (daylo == 10 || daylo == 14)))) {
    daylo = 1;
    dayhi = 0;
    return tickMonth();
  }

  if(days == 31 && (dayhi == 3 && (daylo & 3))) {
    daylo = 1;
    dayhi = 0;
    return tickMonth();
  }

  if(daylo <= 8 || daylo == 12) {
    daylo++;
  } else {
    daylo = !(daylo & 1);
    dayhi++;
  }
}

auto EpsonRTC::tickMonth() -> void {
  if(monthhi == 0 || !(monthlo & 2)) {
    if(monthlo <= 8 || monthlo == 12) {
      monthlo++;
    } else {
      monthlo = !(monthlo & 1);
      monthhi ^= 1;
    }
  } else {
    monthlo = !(monthlo & 1);
    monthhi = 0;
    tickYear();
  }
}

auto EpsonRTC::tickYear() -> void {
  if(yearlo <= 8 || yearlo == 12) {
    yearlo++;
  } else {
    yearlo = !(yearlo & 1);
    if(yearhi <= 8 || yearhi == 12) {
      yearhi++;
    } else {
      yearhi = !(yearhi & 1);
    }
  }
}

auto EpsonRTC::serialize(serializer& s) -> void {
  Thread::serialize(s);

  s.integer(clocks);
  s.integer(seconds);

  s.integer(chipselect);
  s.integer((uint&)state);
  s.integer(mdr);
  s.integer(offset);
  s.integer(wait);
  s.integer(ready);
  s.integer(holdtick);

  s.integer(secondlo);
  s.integer(secondhi);
  s.integer(batteryfailure);

  s.integer(minutelo);
  s.integer(minutehi);
  s.integer(resync);

  s.integer(hourlo);
  s.integer(hourhi);
  s.integer(meridian);

  s.integer(daylo);
  s.integer(dayhi);
  s.integer(dayram);

  s.integer(monthlo);
  s.integer(monthhi);
  s.integer(monthram);

  s.integer(yearlo);
  s.integer(yearhi);

  s.integer(weekday);

  s.integer(hold);
  s.integer(calendar);
  s.integer(irqflag);
  s.integer(roundseconds);

  s.integer(irqmask);
  s.integer(irqduty);
  s.integer(irqperiod);

  s.integer(pause);
  s.integer(stop);
  s.integer(atime);
  s.integer(test);
}

EpsonRTC epsonrtc;

auto EpsonRTC::synchronizeCPU() -> void {
  if(clock >= 0) scheduler.resume(cpu.thread);
}

auto EpsonRTC::Enter() -> void {
  while(true) {
    scheduler.synchronize();
    epsonrtc.main();
  }
}

auto EpsonRTC::main() -> void {
  if(wait) { if(--wait == 0) ready = 1; }

  clocks++;
  if((clocks & ~0x00ff) == 0) roundSeconds();  //125 microseconds
  if((clocks & ~0x3fff) == 0) duty();  //1/128th second
  if((clocks & ~0x7fff) == 0) irq(0);  //1/64th second
  if(clocks == 0) {  //1 second
    seconds++;
    irq(1);
    if(seconds %   60 == 0) irq(2);  //1 minute
    if(seconds % 1440 == 0) irq(3), seconds = 0;  //1 hour
    tick();
  }

  step(1);
  synchronizeCPU();
}

auto EpsonRTC::step(uint clocks) -> void {
  clock += clocks * (uint64_t)cpu.frequency;
}

auto EpsonRTC::initialize() -> void {
  secondlo = 0;
  secondhi = 0;
  batteryfailure = 1;

  minutelo = 0;
  minutehi = 0;
  resync = 0;

  hourlo = 0;
  hourhi = 0;
  meridian = 0;

  daylo = 0;
  dayhi = 0;
  dayram = 0;

  monthlo = 0;
  monthhi = 0;
  monthram = 0;

  yearlo = 0;
  yearhi = 0;

  weekday = 0;

  hold = 0;
  calendar = 0;
  irqflag = 0;
  roundseconds = 0;

  irqmask = 0;
  irqduty = 0;
  irqperiod = 0;

  pause = 0;
  stop = 0;
  atime = 0;
  test = 0;
}

auto EpsonRTC::power() -> void {
  create(EpsonRTC::Enter, 32'768 * 64);

  clocks = 0;
  seconds = 0;

  chipselect = 0;
  state = State::Mode;
  offset = 0;
  wait = 0;
  ready = 0;
  holdtick = 0;
}

auto EpsonRTC::synchronize(uint64_t timestamp) -> void {
  time_t systime = timestamp;
  tm* timeinfo = localtime(&systime);

  uint second = min(59, timeinfo->tm_sec);
  secondlo = second % 10;
  secondhi = second / 10;

  uint minute = timeinfo->tm_min;
  minutelo = minute % 10;
  minutehi = minute / 10;

  uint hour = timeinfo->tm_hour;
  if(atime) {
    hourlo = hour % 10;
    hourhi = hour / 10;
  } else {
    meridian = hour >= 12;
    hour %= 12;
    if(hour == 0) hour = 12;
    hourlo = hour % 10;
    hourhi = hour / 10;
  }

  uint day = timeinfo->tm_mday;
  daylo = day % 10;
  dayhi = day / 10;

  uint month = 1 + timeinfo->tm_mon;
  monthlo = month % 10;
  monthhi = month / 10;

  uint year = timeinfo->tm_year % 100;
  yearlo = year % 10;
  yearhi = year / 10;

  weekday = timeinfo->tm_wday;

  resync = true;  //alert program that time has changed
}

auto EpsonRTC::read(uint addr, uint8_t data) -> uint8_t {
  cpu.synchronizeCoprocessors();
  addr &= 3;

  if(addr == 0) {
    return chipselect;
  }

  if(addr == 1) {
    if(chipselect != 1) return 0;
    if(ready == 0) return 0;
    if(state == State::Write) return mdr;
    if(state != State::Read) return 0;
    ready = 0;
    wait = 8;
    return rtcRead(offset++);
  }

  if(addr == 2) {
    return ready << 7;
  }

  return data;
}

auto EpsonRTC::write(uint addr, uint8_t data) -> void {
  cpu.synchronizeCoprocessors();
  addr &= 3, data &= 15;

  if(addr == 0) {
    chipselect = data;
    if(chipselect != 1) rtcReset();
    ready = 1;
  }

  if(addr == 1) {
    if(chipselect != 1) return;
    if(ready == 0) return;

    if(state == State::Mode) {
      if(data != 0x03 && data != 0x0c) return;
      state = State::Seek;
      ready = 0;
      wait = 8;
      mdr = data;
    }

    else if(state == State::Seek) {
      if(mdr == 0x03) state = State::Write;
      if(mdr == 0x0c) state = State::Read;
      offset = data;
      ready = 0;
      wait = 8;
      mdr = data;
    }

    else if(state == State::Write) {
      rtcWrite(offset++, data);
      ready = 0;
      wait = 8;
      mdr = data;
    }
  }
}

}
