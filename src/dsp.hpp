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

#include <memory>
#include "dsp/SPC_DSP.h"

struct DSP {
  Emulator::Stream *stream;
  uint8_t apuram[64 * 1024] = {};

  void main();
  uint8_t read(uint8_t address);
  void write(uint8_t address, uint8_t data);

  bool load();
  void power(bool reset);
  bool mute();

  void serialize(serializer&);

  int64_t clock = 0;

private:
  SPC_DSP spc_dsp;
  int16_t samplebuffer[8192];
};

extern DSP dsp;
