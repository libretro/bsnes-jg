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

#include <cstdint>
#include <string>

namespace SuperFamicom {

struct Cheat {
  struct Code {
    bool operator==(const Code& code) const;

    unsigned address;
    unsigned data;
    unsigned compare;
    bool usecompare;
    bool enable;
    unsigned restore;
  };

  explicit operator bool() const;

  void reset();
  void append(unsigned, unsigned, unsigned, bool);
  void assign(const std::vector<std::string>&);
  bool find(uint8_t*, unsigned, unsigned);

  std::vector<Code> codes;
};

extern Cheat cheat;

}

namespace CheatDecoder {
  bool gb(std::string&);
  bool snes(std::string&);
}
