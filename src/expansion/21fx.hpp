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

struct S21FX : Expansion {
  S21FX();
  ~S21FX();

  static auto Enter() -> void;
  auto step(unsigned clocks) -> void;
  auto main() -> void;

  auto read(unsigned addr, uint8_t data) -> uint8_t;
  auto write(unsigned addr, uint8_t data) -> void;

private:
  auto quit() -> bool;
  auto usleep(unsigned) -> void;
  auto readable() -> bool;
  auto writable() -> bool;
  auto read() -> uint8_t;
  auto write(uint8_t) -> void;

  bool booted = false;
  uint16 resetVector;
  uint8_t ram[122];

  nall::library link;
  nall::function<void (
    nall::function<bool ()>,      //quit
    nall::function<void (unsigned)>,  //usleep
    nall::function<bool ()>,      //readable
    nall::function<bool ()>,      //writable
    nall::function<uint8_t ()>,     //read
    nall::function<void (uint8_t)>  //write
  )> linkInit;
  nall::function<void (vector<nall::string>)> linkMain;

  vector<uint8_t> snesBuffer;  //SNES -> Link
  vector<uint8_t> linkBuffer;  //Link -> SNES
};

}
