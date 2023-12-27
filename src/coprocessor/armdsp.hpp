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

#include <vector>

#include "processor/arm7tdmi.hpp"
#include "sfc.hpp"

namespace SuperFamicom {

//ARMv3 (ARM60)

//note: this coprocessor uses the ARMv4 (ARM7TDMI) core as its base
//instruction execution forces ARM mode to remove ARMv4 THUMB access
//there is a possibility the ARMv3 supports 26-bit mode; but cannot be verified

struct ArmDSP : Processor::ARM7TDMI, Thread {
  struct Bridge {
    struct Buffer {
      bool ready;
      uint8_t data;
    };
    Buffer cputoarm;
    Buffer armtocpu;
    uint32_t timer;
    uint32_t timerlatch;
    bool reset;
    bool ready;
    bool signal;

    uint8_t status() const;
  } bridge;

  void synchronizeCPU();
  void unload();
  void boot();
  void main();

  void step(unsigned) override;
  void sleep() override;
  uint32_t get(unsigned, uint32_t) override;
  void set(unsigned, uint32_t, uint32_t) override;

  uint8_t read(unsigned, uint8_t);
  void write(unsigned, uint8_t);

  void power();
  void reset(); //soft reset

  std::vector<uint8_t> firmware() const;
  void serialize(serializer&);

  unsigned Frequency;

  uint8_t programROM[128 * 1024];
  uint8_t dataROM[32 * 1024];
  uint8_t programRAM[16 * 1024];
};

extern ArmDSP armdsp;

}
