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

#include <string>
#include <vector>

#include "serializer.hpp"

namespace SuperFamicom {

struct Interface {
  bool loaded();
  bool load();
  void save();
  void unload();

  unsigned connected(unsigned);
  void connect(unsigned, unsigned);
  void power();
  void reset();
  void run();

  bool rtc();

  serializer serialize(bool = true);
  bool unserialize(serializer&);

  uint8_t read(uint32_t);
  void cheats(const std::vector<std::string>&);

  unsigned frameSkip();
  void setFrameSkip(unsigned);

  bool runAhead();
  void setRunAhead(bool);

private:
  Interface* interface = nullptr;
};

}
