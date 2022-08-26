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

namespace Heuristics {

struct Memory {
  Memory& type(std::string);
  Memory& size(size_t);
  Memory& content(std::string);
  Memory& manufacturer(std::string);
  Memory& architecture(std::string);
  Memory& identifier(std::string);
  Memory& isVolatile();

  std::string text() const;

  std::string _type;
  bool _battery;
  size_t _size;
  std::string _content;
  std::string _manufacturer;
  std::string _architecture;
  std::string _identifier;
  bool _volatile;
};

struct Oscillator {
  Oscillator& frequency(unsigned);
  std::string text() const;
  unsigned _frequency;
};

struct Slot {
  Slot& type(std::string);
  std::string text() const;
  std::string _type;
};

struct BSMemory {
  BSMemory(std::vector<uint8_t>&, std::string);
  explicit operator bool() const;
  std::string manifest() const;

private:
  std::vector<uint8_t>& data;
  std::string location;
};

struct GameBoy {
  GameBoy(std::vector<uint8_t>&, std::string);
  explicit operator bool() const;
  std::string manifest() const;

private:
  std::vector<uint8_t>& data;
  std::string location;
  unsigned headerAddress = 0;

  uint8_t read(unsigned) const;
};

struct SufamiTurbo {
  SufamiTurbo(std::vector<uint8_t>&, std::string);
  explicit operator bool() const;
  std::string manifest() const;

private:
  std::vector<uint8_t>& data;
  std::string location;
};

struct SuperFamicom {
  SuperFamicom(std::vector<uint8_t>&, std::string);
  explicit operator bool() const;

  std::string manifest() const;
  std::string region() const;
  std::string videoRegion() const;
  std::string revision() const;
  std::string board() const;
  std::string title() const;
  std::string serial() const;
  unsigned romSize() const;
  unsigned programRomSize() const;
  unsigned dataRomSize() const;
  unsigned expansionRomSize() const;
  unsigned firmwareRomSize() const;
  unsigned ramSize() const;
  unsigned expansionRamSize() const;
  bool nonVolatile() const;

private:
  unsigned size() const;
  unsigned scoreHeader(unsigned);
  std::string firmwareARM() const;
  std::string firmwareEXNEC() const;
  std::string firmwareGB() const;
  std::string firmwareHITACHI() const;
  std::string firmwareNEC() const;

  std::vector<uint8_t>& data;
  std::string location;
  unsigned headerAddress = 0;
};
}
