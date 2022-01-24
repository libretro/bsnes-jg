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

#include "processor/gsu.hpp"

namespace SuperFamicom {

struct SuperFX : Processor::GSU, Thread {
  ReadableMemory rom;
  WritableMemory ram;

  inline bool synchronizing() const;

  //superfx.cpp
  void synchronizeCPU();
  static void Enter();
  void main();
  void unload();
  void power();

  //bus.cpp
  struct CPUROM : Memory {
    uint8_t* data() override;
    unsigned size() const override;
    uint8_t read(unsigned, uint8_t) override;
    void write(unsigned, uint8_t) override;
  };

  struct CPURAM : Memory {
    uint8_t* data() override;
    unsigned size() const override;
    uint8_t read(unsigned, uint8_t) override;
    void write(unsigned, uint8_t) override;
  };

  //core.cpp
  void stop() override;
  uint8_t color(uint8_t) override;
  void plot(uint8_t, uint8_t) override;
  uint8_t rpix(uint8_t, uint8_t) override;

  void flushPixelCache(PixelCache&);

  //memory.cpp
  uint8_t read(unsigned, uint8_t = 0x00) override;
  void write(unsigned, uint8_t) override;

  uint8_t readOpcode(uint16_t);
  inline uint8_t peekpipe();
  inline uint8_t pipe() override;

  void flushCache() override;
  uint8_t readCache(uint16_t);
  void writeCache(uint16_t, uint8_t);

  //io.cpp
  uint8_t readIO(unsigned, uint8_t);
  void writeIO(unsigned, uint8_t);

  //timing.cpp
  void step(unsigned) override;

  void syncROMBuffer() override;
  uint8_t readROMBuffer() override;
  void updateROMBuffer();

  void syncRAMBuffer() override;
  uint8_t readRAMBuffer(uint16_t) override;
  void writeRAMBuffer(uint16_t, uint8_t) override;

  //serialization.cpp
  void serialize(serializer&);

  unsigned Frequency;

  CPUROM cpurom;
  CPURAM cpuram;

private:
  unsigned romMask;
  unsigned ramMask;
};

extern SuperFX superfx;

}
