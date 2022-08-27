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

#include "ppu.hpp"
#include "processor/wdc65816.hpp"

#if defined(__clang__) || defined(__GNUC__)
  #define alwaysinline inline __attribute__((always_inline))
#else
  #define alwaysinline inline
#endif

namespace SuperFamicom {

struct CPU : Processor::WDC65816, Thread, PPUcounter {
  inline bool interruptPending() const override;
  inline bool refresh() const;
  inline uint8_t pio() const;
  inline bool synchronizing() const override;
  inline bool flip(bool&, bool);
  inline bool lower(bool&);
  inline bool raise(bool&, bool);

  void synchronizeSMP();
  void synchronizePPU();
  void synchronizeCoprocessors();
  [[noreturn]] static void Enter();
  void main();
  bool load();
  void power(bool);
  void quirk();

  inline bool dmaEnable();
  inline bool hdmaEnable();
  inline bool hdmaActive();

  void dmaRun();
  void hdmaReset();
  void hdmaSetup();
  void hdmaRun();

  void idle() override;
  uint8_t read(unsigned) override;
  void write(unsigned, uint8_t) override;
  uint8_t readDisassembler(unsigned);

  uint8_t readRAM(unsigned, uint8_t);
  uint8_t readAPU(unsigned, uint8_t);
  uint8_t readCPU(unsigned, uint8_t);
  uint8_t readDMA(unsigned, uint8_t);
  void writeRAM(unsigned, uint8_t);
  void writeAPU(unsigned, uint8_t);
  void writeCPU(unsigned, uint8_t);
  void writeDMA(unsigned, uint8_t);

  inline unsigned dmaCounter() const;
  inline unsigned joypadCounter() const;

  alwaysinline void stepOnce();
  inline void step(unsigned);
  template<unsigned, bool> void step();
  void scanline();

  inline void aluEdge();
  inline void dmaEdge();

  inline void nmiPoll();
  inline void irqPoll();
  void nmitimenUpdate(uint8_t);
  bool rdnmi();
  bool timeup();

  inline bool nmiTest();
  inline bool irqTest();
  inline void lastCycle() override;

  void joypadEdge();

  void serialize(serializer&);

  uint8_t wram[128 * 1024];
  std::vector<Thread*> coprocessors;

  struct Overclocking {
    unsigned counter = 0;
    unsigned target = 0;
  } overclocking;

private:
  bool init = false;
  unsigned version = 2;  //allowed: 1, 2

  struct Counter {
    unsigned cpu = 0;
    unsigned dma = 0;
  } counter;

  struct Status {
    unsigned clockCount = 0;

    bool irqLock = 0;

    unsigned dramRefreshPosition = 0;
    unsigned dramRefresh = 0;  //0 = not refreshed; 1 = refresh active; 2 = refresh inactive

    unsigned hdmaSetupPosition = 0;
    bool hdmaSetupTriggered = 0;

    unsigned hdmaPosition = 0;
    bool hdmaTriggered = 0;

    bool nmiValid = 0;
    bool nmiLine = 0;
    bool nmiTransition = 0;
    bool nmiPending = 0;
    bool nmiHold = 0;

    bool irqValid = 0;
    bool irqLine = 0;
    bool irqTransition = 0;
    bool irqPending = 0;
    bool irqHold = 0;

    bool resetPending = 0;
    bool interruptPending = 0;

    bool dmaActive = 0;
    bool dmaPending = 0;
    bool hdmaPending = 0;
    bool hdmaMode = 0;  //0 = init, 1 = run

    unsigned autoJoypadCounter = 33;  //state machine; 4224 / 128 = 33 (inactive)
  } status;

  struct IO {
    //$2181-$2183
    uint32_t wramAddress = 0;

    //$4200
    bool hirqEnable = 0;
    bool virqEnable = 0;
    bool irqEnable = 0;
    bool nmiEnable = 0;
    bool autoJoypadPoll = 0;

    //$4201
    uint8_t pio = 0xff;

    //$4202-$4203
    uint8_t wrmpya = 0xff;
    uint8_t wrmpyb = 0xff;

    //$4204-$4206
    uint16_t wrdiva = 0xffff;
    uint8_t wrdivb = 0xff;

    //$4207-$420a
    uint16_t htime = (0x1ff + 1) << 2;
    uint16_t vtime = 0x1ff;

    //$420d
    uint8_t fastROM = 0;

    //$4214-$4217
    uint16_t rddiv = 0;
    uint16_t rdmpy = 0;

    //$4218-$421f
    uint16_t joy1 = 0;
    uint16_t joy2 = 0;
    uint16_t joy3 = 0;
    uint16_t joy4 = 0;
  } io;

  struct ALU {
    unsigned mpyctr = 0;
    unsigned divctr = 0;
    unsigned shift = 0;
  } alu;

  struct Channel {
    template<unsigned, bool> inline void step();
    inline void edge();

    inline bool validA(uint32_t);
    inline uint8_t readA(uint32_t);
    inline uint8_t readB(uint8_t, bool);
    inline void writeA(uint32_t, uint8_t);
    inline void writeB(uint8_t, uint8_t, bool);
    inline void transfer(uint32_t, uint8_t);

    inline void dmaRun();
    inline bool hdmaActive();
    inline bool hdmaFinished();
    inline void hdmaReset();
    inline void hdmaSetup();
    inline void hdmaReload();
    inline void hdmaTransfer();
    inline void hdmaAdvance();

    //$420b
    bool dmaEnable = 0;

    //$420c
    bool hdmaEnable = 0;

    //$43x0
    uint8_t transferMode = 7;
    uint8_t fixedTransfer = 1;
    uint8_t reverseTransfer = 1;
    uint8_t unused = 1;
    uint8_t indirect = 1;
    uint8_t direction = 1;

    //$43x1
    uint8_t targetAddress = 0xff;

    //$43x2-$43x3
    uint16_t sourceAddress = 0xffff;

    //$43x4
    uint8_t sourceBank = 0xff;

    //$43x5-$43x6
    union {
      uint16_t transferSize;
      uint16_t indirectAddress;
    };

    //$43x7
    uint8_t indirectBank = 0xff;

    //$43x8-$43x9
    uint16_t hdmaAddress = 0xffff;

    //$43xa
    uint8_t lineCounter = 0xff;

    //$43xb/$43xf
    uint8_t unknown = 0xff;

    //internal state
    bool hdmaCompleted = 0;
    bool hdmaDoTransfer = 0;

    Channel *next;

    Channel() : transferSize(0xffff) {}
  } channels[8];
};

extern CPU cpu;

bool CPU::refresh() const {
  return status.dramRefresh == 1;
}

uint8_t CPU::pio() const {
  return io.pio;
}

}

#undef alwaysinline
