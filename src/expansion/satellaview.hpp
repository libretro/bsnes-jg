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

struct Satellaview : Expansion {
  Satellaview();
  ~Satellaview();

  uint8_t read(unsigned addr, uint8_t data);
  void write(unsigned addr, uint8_t data);

private:
  struct {
    uint8_t r2188, r2189, r218a, r218b;
    uint8_t r218c, r218d, r218e, r218f;
    uint8_t r2190, r2191, r2192, r2193;
    uint8_t r2194, r2195, r2196, r2197;
    uint8_t r2198, r2199, r219a, r219b;
    uint8_t r219c, r219d, r219e, r219f;

    uint8_t rtcCounter;
    uint8_t rtcHour;
    uint8_t rtcMinute;
    uint8_t rtcSecond;
  } regs;
};

}
