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

#include "../sfc.hpp"

namespace SuperFamicom {

void OBC1::serialize(serializer& s) {
  s.array(ram.data(), ram.size());

  s.integer(status.address);
  s.integer(status.baseptr);
  s.integer(status.shift);
}

OBC1 obc1;

void OBC1::unload() {
  ram.reset();
}

void OBC1::power() {
  status.baseptr = (ramRead(0x1ff5) & 1) ? 0x1800 : 0x1c00;
  status.address = (ramRead(0x1ff6) & 0x7f);
  status.shift   = (ramRead(0x1ff6) & 3) << 1;
}

uint8_t OBC1::read(unsigned addr, uint8_t) {
  addr &= 0x1fff;

  switch(addr) {
  case 0x1ff0: return ramRead(status.baseptr + (status.address << 2) + 0);
  case 0x1ff1: return ramRead(status.baseptr + (status.address << 2) + 1);
  case 0x1ff2: return ramRead(status.baseptr + (status.address << 2) + 2);
  case 0x1ff3: return ramRead(status.baseptr + (status.address << 2) + 3);
  case 0x1ff4: return ramRead(status.baseptr + (status.address >> 2) + 0x200);
  }

  return ramRead(addr);
}

void OBC1::write(unsigned addr, uint8_t data) {
  addr &= 0x1fff;

  switch(addr) {
  case 0x1ff0: ramWrite(status.baseptr + (status.address << 2) + 0, data); return;
  case 0x1ff1: ramWrite(status.baseptr + (status.address << 2) + 1, data); return;
  case 0x1ff2: ramWrite(status.baseptr + (status.address << 2) + 2, data); return;
  case 0x1ff3: ramWrite(status.baseptr + (status.address << 2) + 3, data); return;
  case 0x1ff4: {
    uint8_t temp = ramRead(status.baseptr + (status.address >> 2) + 0x200);
    temp = (temp & ~(3 << status.shift)) | ((data & 3) << status.shift);
    ramWrite(status.baseptr + (status.address >> 2) + 0x200, temp);
  } return;
  case 0x1ff5:
    status.baseptr = (data & 1) ? 0x1800 : 0x1c00;
    ramWrite(addr, data);
    return;
  case 0x1ff6:
    status.address = (data & 0x7f);
    status.shift   = (data & 3) << 1;
    ramWrite(addr, data);
    return;
  case 0x1ff7:
    ramWrite(addr, data);
    return;
  }

  return ramWrite(addr, data);
}

uint8_t OBC1::ramRead(unsigned addr) {
  return ram.read(addr & 0x1fff);
}

void OBC1::ramWrite(unsigned addr, uint8_t data) {
  ram.write(addr & 0x1fff, data);
}

}
