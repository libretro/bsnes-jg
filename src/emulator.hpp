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

#include <optional>

namespace Emulator {

//incremented only when serialization format changes
static const std::string SerializerVersion = "115";

struct Game {
  struct Memory;
  struct Oscillator;

  void load(std::string);
  std::optional<Game::Memory> memory(std::string);
  std::optional<Game::Oscillator> oscillator(unsigned = 0);

  struct Memory {
    Memory() = default;
    Memory(std::string);
    explicit operator bool() const { return (bool)!type.empty(); }
    std::string name() const;

    std::string type;
    unsigned size;
    std::string content;
    std::string manufacturer;
    std::string architecture;
    std::string identifier;
    bool nonVolatile;
  };

  struct Oscillator {
    Oscillator() = default;
    Oscillator(std::string);
    explicit operator bool() const { return frequency; }

    unsigned frequency;
  };

  std::string document;
  std::string sha256;
  std::string label;
  std::string name;
  std::string title;
  std::string region;
  std::string revision;
  std::string board;
  std::vector<uint8_t> prgrom;
  std::vector<uint8_t> datarom;
  std::vector<uint8_t> exprom;
  std::vector<Game::Memory> memoryList;
  std::vector<Game::Oscillator> oscillatorList;
};

}
