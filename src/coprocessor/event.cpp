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

#include "../serializer.hpp"
#include "../cpu.hpp"
#include "../memory.hpp"

#include "dip.hpp"

#include "event.hpp"

namespace SuperFamicom {

void Event::serialize(serializer& s) {
  Thread::serialize(s);
  s.integer(status);
  s.integer(select);
  s.integer(timerActive);
  s.integer(scoreActive);
  s.integer(timerSecondsRemaining);
  s.integer(scoreSecondsRemaining);
}

Event event;

void Event::synchronizeCPU() {
  if(clock >= 0) scheduler.resume(cpu.thread);
}

void Event::Enter() {
  while(true) {
    scheduler.synchronize();
    event.main();
  }
}

void Event::main() {
  if(scoreActive && scoreSecondsRemaining) {
    if(--scoreSecondsRemaining == 0) {
      scoreActive = false;
    }
  }

  if(timerActive && timerSecondsRemaining) {
    if(--timerSecondsRemaining == 0) {
      timerActive = false;
      status |= 0x02;  //time over
      scoreActive = true;
      scoreSecondsRemaining = 5;
    }
  }

  step(1);
  synchronizeCPU();
}

void Event::step(unsigned clocks) {
  clock += clocks * (uint64_t)cpu.frequency;
}

void Event::unload() {
  rom[0].reset();
  rom[1].reset();
  rom[2].reset();
  rom[3].reset();
}

void Event::power() {
  create(Event::Enter, 1);

  //DIP switches 0-3 control the time: 3 minutes + 0-15 extra minutes
  timer = (3 + (dip.value & 15)) * 60;  //in seconds
  //DIP switches 4-5 serve an unknown purpose
  //DIP switches 6-7 are not connected

  status = 0x00;
  select = 0x00;
  timerActive = false;
  scoreActive = false;
  timerSecondsRemaining = 0;
  scoreSecondsRemaining = 0;
}

uint8_t Event::mcuRead(unsigned addr, uint8_t data) {
  if(board == Board::CampusChallenge92) {
    unsigned id = 0;
    if(select == 0x09) id = 1;
    if(select == 0x05) id = 2;
    if(select == 0x03) id = 3;
    if((addr & 0x808000) == 0x808000) id = 0;

    if(addr & 0x008000) {
      addr = ((addr & 0x7f0000) >> 1) | (addr & 0x7fff);
      return rom[id].read(bus.mirror(addr, rom[id].size()), data);
    }
  }

  if(board == Board::PowerFest94) {
    unsigned id = 0;
    if(select == 0x09) id = 1;
    if(select == 0x0c) id = 2;
    if(select == 0x0a) id = 3;
    if((addr & 0x208000) == 0x208000) id = 0;

    if(addr & 0x400000) {
      addr &= 0x3fffff;
      return rom[id].read(bus.mirror(addr, rom[id].size()), data);
    }

    if(addr & 0x008000) {
      addr &= 0x1fffff;
      if(id != 2) addr = ((addr & 0x1f0000) >> 1) | (addr & 0x7fff);
      return rom[id].read(bus.mirror(addr, rom[id].size()), data);
    }
  }

  return data;
}

void Event::mcuWrite(unsigned addr, uint8_t data) {
}

uint8_t Event::read(unsigned addr, uint8_t data) {
  if(addr == 0x106000 || addr == 0xc00000) {
    return status;
  }
  return data;
}

void Event::write(unsigned addr, uint8_t data) {
  if(addr == 0x206000 || addr == 0xe00000) {
    select = data;
    if(timer && data == 0x09) {
      timerActive = true;
      timerSecondsRemaining = timer;
    }
  }
}

}
