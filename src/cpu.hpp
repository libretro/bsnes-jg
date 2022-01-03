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

struct CPU : Processor::WDC65816, Thread, PPUcounter {
  inline bool interruptPending() const override { return status.interruptPending; }
  inline uint8_t pio() const { return io.pio; }
  inline bool refresh() const { return status.dramRefresh == 1; }
  inline bool synchronizing() const override { return scheduler.synchronizing(); }

  //cpu.cpp
  void synchronizeSMP();
  void synchronizePPU();
  void synchronizeCoprocessors();
  static void Enter();
  void main();
  bool load();
  void power(bool reset);

  //dma.cpp
  inline bool dmaEnable();
  inline bool hdmaEnable();
  inline bool hdmaActive();

  void dmaRun();
  void hdmaReset();
  void hdmaSetup();
  void hdmaRun();

  //memory.cpp
  void idle() override;
  uint8_t read(unsigned addr) override;
  void write(unsigned addr, uint8_t data) override;
  uint8_t readDisassembler(unsigned addr) override;

  //io.cpp
  uint8_t readRAM(unsigned address, uint8_t data);
  uint8_t readAPU(unsigned address, uint8_t data);
  uint8_t readCPU(unsigned address, uint8_t data);
  uint8_t readDMA(unsigned address, uint8_t data);
  void writeRAM(unsigned address, uint8_t data);
  void writeAPU(unsigned address, uint8_t data);
  void writeCPU(unsigned address, uint8_t data);
  void writeDMA(unsigned address, uint8_t data);

  //timing.cpp
  inline unsigned dmaCounter() const;
  inline unsigned joypadCounter() const;

  inline void stepOnce();
  inline void step(unsigned clocks);
  template<unsigned Clocks, bool Synchronize> void step();
  void scanline();

  inline void aluEdge();
  inline void dmaEdge();

  //irq.cpp
  inline void nmiPoll();
  inline void irqPoll();
  void nmitimenUpdate(uint8_t data);
  bool rdnmi();
  bool timeup();

  inline bool nmiTest();
  inline bool irqTest();
  inline void lastCycle() override;

  //joypad.cpp
  void joypadEdge();

  //serialization.cpp
  void serialize(serializer&);

  uint8_t wram[128 * 1024];
  std::vector<Thread*> coprocessors;

  struct Overclocking {
    unsigned counter = 0;
    unsigned target = 0;
  } overclocking;

private:
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

    nall::Boolean nmiValid = 0;
    nall::Boolean nmiLine = 0;
    nall::Boolean nmiTransition = 0;
    nall::Boolean nmiPending = 0;
    nall::Boolean nmiHold = 0;

    nall::Boolean irqValid = 0;
    nall::Boolean irqLine = 0;
    nall::Boolean irqTransition = 0;
    nall::Boolean irqPending = 0;
    nall::Boolean irqHold = 0;

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
    nall::Natural<17> wramAddress = 0;

    //$4200
    nall::Boolean hirqEnable = 0;
    nall::Boolean virqEnable = 0;
    nall::Boolean irqEnable = 0;
    nall::Boolean nmiEnable = 0;
    nall::Boolean autoJoypadPoll = 0;

    //$4201
    uint8_t pio = 0xff;

    //$4202-$4203
    uint8_t wrmpya = 0xff;
    uint8_t wrmpyb = 0xff;

    //$4204-$4206
    uint16_t wrdiva = 0xffff;
    uint8_t wrdivb = 0xff;

    //$4207-$420a
    nall::Natural<12> htime = (0x1ff + 1) << 2;
    nall::Natural< 9>  vtime = 0x1ff;

    //$420d
    nall::Natural< 1> fastROM = 0;

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
    //dma.cpp
    template<unsigned Clocks, bool Synchronize> inline void step();
    inline void edge();

    inline bool validA(nall::Natural<24> address);
    inline uint8_t readA(nall::Natural<24> address);
    inline uint8_t readB(uint8_t address, bool valid);
    inline void writeA(nall::Natural<24> address, uint8_t data);
    inline void writeB(uint8_t address, uint8_t data, bool valid);
    inline void transfer(nall::Natural<24> address, nall::Natural< 2> index);

    inline void dmaRun();
    inline bool hdmaActive();
    inline bool hdmaFinished();
    inline void hdmaReset();
    inline void hdmaSetup();
    inline void hdmaReload();
    inline void hdmaTransfer();
    inline void hdmaAdvance();

    //$420b
    nall::Natural< 1> dmaEnable = 0;

    //$420c
    nall::Natural< 1> hdmaEnable = 0;

    //$43x0
    nall::Natural< 3> transferMode = 7;
    nall::Natural< 1> fixedTransfer = 1;
    nall::Natural< 1> reverseTransfer = 1;
    nall::Natural< 1> unused = 1;
    nall::Natural< 1> indirect = 1;
    nall::Natural< 1> direction = 1;

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
    nall::Natural< 1> hdmaCompleted = 0;
    nall::Natural< 1> hdmaDoTransfer = 0;

    Channel *next;

    Channel() : transferSize(0xffff) {}
  } channels[8];
};

extern CPU cpu;
