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

#include "processor/wdc65816.hpp"

namespace SuperFamicom {

//Super Accelerator (SA-1)

struct SA1 : Processor::WDC65816, Thread {
  inline bool synchronizing() const override { return scheduler.synchronizing(); }

  //sa1.cpp
  void synchronizeCPU();
  static void Enter();
  void main();
  void step();
  void interrupt() override;

  inline void triggerIRQ();
  inline void lastCycle() override;
  inline bool interruptPending() const override;

  void unload();
  void power();

  //dma.cpp
  struct DMA {
    enum CDEN : unsigned { DmaNormal = 0, DmaCharConversion = 1 };
    enum SD : unsigned { SourceROM = 0, SourceBWRAM = 1, SourceIRAM = 2 };
    enum DD : unsigned { DestIRAM = 0, DestBWRAM = 1 };
    unsigned line;
  };

  void dmaNormal();
  void dmaCC1();
  uint8_t dmaCC1Read(unsigned);
  void dmaCC2();

  //memory.cpp
  inline bool conflictROM() const;
  inline bool conflictBWRAM() const;
  inline bool conflictIRAM() const;

  inline void idle() override;
  inline void idleJump() override;
  inline void idleBranch() override;
  inline uint8_t read(unsigned) override;
  inline void write(unsigned, uint8_t) override;
  uint8_t readVBR(unsigned, uint8_t = 0);
  uint8_t readDisassembler(unsigned);

  //io.cpp
  uint8_t readIOCPU(unsigned, uint8_t);
  uint8_t readIOSA1(unsigned, uint8_t);
  void writeIOCPU(unsigned, uint8_t);
  void writeIOSA1(unsigned, uint8_t);
  void writeIOShared(unsigned, uint8_t);

  //serialization.cpp
  void serialize(serializer&);

  struct ROM : ReadableMemory {
    //rom.cpp
    inline bool conflict() const;

    inline uint8_t read(unsigned, uint8_t = 0) override;
    inline void write(unsigned, uint8_t) override;

    uint8_t readCPU(unsigned, uint8_t = 0);
    void writeCPU(unsigned, uint8_t);

    uint8_t readSA1(unsigned, uint8_t = 0);
    void writeSA1(unsigned, uint8_t);
  } rom;

  struct BWRAM : WritableMemory {
    //bwram.cpp
    inline bool conflict() const;

    inline uint8_t read(unsigned, uint8_t = 0) override;
    inline void write(unsigned, uint8_t) override;

    uint8_t readCPU(unsigned, uint8_t = 0);
    void writeCPU(unsigned, uint8_t);

    uint8_t readSA1(unsigned, uint8_t = 0);
    void writeSA1(unsigned, uint8_t);

    uint8_t readLinear(unsigned, uint8_t = 0);
    void writeLinear(unsigned, uint8_t);

    uint8_t readBitmap(uint32_t, uint8_t = 0);
    void writeBitmap(uint32_t, uint8_t);

    bool dma;
  } bwram;

  struct IRAM : WritableMemory {
    //iram.cpp
    inline bool conflict() const;

    inline uint8_t read(unsigned, uint8_t = 0) override;
    inline void write(unsigned, uint8_t) override;

    uint8_t readCPU(unsigned, uint8_t);
    void writeCPU(unsigned, uint8_t);

    uint8_t readSA1(unsigned, uint8_t = 0);
    void writeSA1(unsigned, uint8_t);
  } iram;

private:
  DMA dma;

  struct Status {
    uint8_t counter;

    bool interruptPending;

    uint16_t scanlines;
    uint16_t vcounter;
    uint16_t hcounter;
  } status;

  struct MMIO {
    //$2200 CCNT
    bool sa1_irq;
    bool sa1_rdyb;
    bool sa1_resb;
    bool sa1_nmi;
    uint8_t smeg;

    //$2201 SIE
    bool cpu_irqen;
    bool chdma_irqen;

    //$2202 SIC
    bool cpu_irqcl;
    bool chdma_irqcl;

    //$2203,$2204 CRV
    uint16_t crv;

    //$2205,$2206 CNV
    uint16_t cnv;

    //$2207,$2208 CIV
    uint16_t civ;

    //$2209 SCNT
    bool cpu_irq;
    bool cpu_ivsw;
    bool cpu_nvsw;
    uint8_t cmeg;

    //$220a CIE
    bool sa1_irqen;
    bool timer_irqen;
    bool dma_irqen;
    bool sa1_nmien;

    //$220b CIC
    bool sa1_irqcl;
    bool timer_irqcl;
    bool dma_irqcl;
    bool sa1_nmicl;

    //$220c,$220d SNV
    uint16_t snv;

    //$220e,$220f SIV
    uint16_t siv;

    //$2210 TMC
    bool hvselb;
    bool ven;
    bool hen;

    //$2212,$2213
    uint16_t hcnt;

    //$2214,$2215
    uint16_t vcnt;

    //$2220 CXB
    bool cbmode;
    unsigned cb;

    //$2221 DXB
    bool dbmode;
    unsigned db;

    //$2222 EXB
    bool ebmode;
    unsigned eb;

    //$2223 FXB
    bool fbmode;
    unsigned fb;

    //$2224 BMAPS
    uint8_t sbm;

    //$2225 BMAP
    bool sw46;
    uint8_t cbm;

    //$2226 SBWE
    bool swen;

    //$2227 CBWE
    bool cwen;

    //$2228 BWPA
    uint8_t bwp;

    //$2229 SIWP
    uint8_t siwp;

    //$222a CIWP
    uint8_t ciwp;

    //$2230 DCNT
    bool dmaen;
    bool dprio;
    bool cden;
    bool cdsel;
    bool dd;
    uint8_t sd;

    //$2231 CDMA
    bool chdend;
    uint8_t dmasize;
    uint8_t dmacb;

    //$2232-$2234 SDA
    uint32_t dsa;

    //$2235-$2237 DDA
    uint32_t dda;

    //$2238,$2239 DTC
    uint16_t dtc;

    //$223f BBF
    bool bbf;

    //$2240-224f BRF
    uint8_t brf[16];

    //$2250 MCNT
    bool acm;
    bool md;

    //$2251,$2252 MA
    uint16_t ma;

    //$2253,$2254 MB
    uint16_t mb;

    //$2258 VBD
    bool hl;
    uint8_t vb;

    //$2259-$225b VDA
    uint32_t va;
    uint8_t vbit;

    //$2300 SFR
    bool cpu_irqfl;
    bool chdma_irqfl;

    //$2301 CFR
    bool sa1_irqfl;
    bool timer_irqfl;
    bool dma_irqfl;
    bool sa1_nmifl;

    //$2302,$2303 HCR
    uint16_t hcr;

    //$2304,$2305 VCR
    uint16_t vcr;

    //$2306-230a MR
    uint64_t mr;

    //$230b OF
    bool overflow;
  } mmio;
};

extern SA1 sa1;

}
