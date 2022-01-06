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

#include "emulator.hpp"

namespace SuperFamicom {

struct Interface : Emulator::Interface {
  bool loaded() override;
  bool load() override;
  void save() override;
  void unload() override;

  unsigned connected(unsigned port) override;
  void connect(unsigned port, unsigned device) override;
  void power() override;
  void reset() override;
  void run() override;

  bool rtc() override;

  serializer serialize(bool synchronize = true) override;
  bool unserialize(serializer&) override;

  uint8_t read(uint32_t address) override;
  void cheats(const std::vector<std::string>&) override;

  unsigned frameSkip() override;
  void setFrameSkip(unsigned frameSkip) override;

  bool runAhead() override;
  void setRunAhead(bool runAhead) override;
};

}
