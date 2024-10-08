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

#include "processor/spc700.hpp"

namespace SuperFamicom {

//Sony CXP1100Q-1

struct SMP : Processor::SPC700, Thread {
  inline bool synchronizing() const override;
  inline bool raise(bool&, bool);

  uint8_t portRead(uint8_t) const;
  void portWrite(uint8_t, uint8_t);

  void synchronizeCPU();
  void synchronizeDSP();
  void main();
  bool load();
  void power();

  void serialize(serializer&);

  static const uint8_t iplrom[64];

private:
  struct IO {
    //timing
    unsigned clockCounter = 0;
    unsigned dspCounter = 0;

    //external
    uint8_t apu0 = 0;
    uint8_t apu1 = 0;
    uint8_t apu2 = 0;
    uint8_t apu3 = 0;

    //$00f0
    uint8_t timersDisable = 0;
    uint8_t ramWritable = 1;
    uint8_t ramDisable = 0;
    uint8_t timersEnable = 1;
    uint8_t externalWaitStates = 0;
    uint8_t internalWaitStates = 0;

    //$00f1
    bool iplromEnable = true;

    //$00f2
    uint8_t dspAddr = 0;

    //$00f4-00f7
    uint8_t cpu0 = 0;
    uint8_t cpu1 = 0;
    uint8_t cpu2 = 0;
    uint8_t cpu3 = 0;

    //$00f8-00f9
    uint8_t aux4 = 0;
    uint8_t aux5 = 0;
  } io;

  inline uint8_t readRAM(uint16_t);
  inline void writeRAM(uint16_t, uint8_t);

  void idle() override;
  uint8_t read(uint16_t) override;
  void write(uint16_t, uint8_t) override;

  uint8_t readDisassembler(uint16_t);

  inline uint8_t readIO(uint16_t);
  inline void writeIO(uint16_t, uint8_t);

  template<unsigned>
  struct Timer {
    inline bool lower(bool&, bool);
    uint8_t stage0 = 0;
    uint8_t stage1 = 0;
    uint8_t stage2 = 0;
    uint8_t stage3 = 0;
    bool line = 0;
    bool enable = 0;
    uint8_t target = 0;

    void step(unsigned);
    void synchronizeStage1();
  };

  Timer<128> timer0;
  Timer<128> timer1;
  Timer< 16> timer2;

  inline void wait(uint16_t, bool = false);
  inline void waitIdle();
  inline void step(unsigned);
  inline void stepIdle(unsigned);
  inline void stepTimers(unsigned);
};

extern SMP smp;

}
