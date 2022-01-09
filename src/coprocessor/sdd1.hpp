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

struct SDD1 {
  void unload();
  void power();

  uint8_t ioRead(unsigned addr, uint8_t data);
  void ioWrite(unsigned addr, uint8_t data);

  uint8_t dmaRead(unsigned addr, uint8_t data);
  void dmaWrite(unsigned addr, uint8_t data);

  uint8_t mmcRead(unsigned addr);

  uint8_t mcuRead(unsigned addr, uint8_t data);
  void mcuWrite(unsigned addr, uint8_t data);

  void serialize(serializer&);

  ReadableMemory rom;

private:
  uint8_t r4800;  //hard enable
  uint8_t r4801;  //soft enable
  uint8_t r4804;  //MMC bank 0
  uint8_t r4805;  //MMC bank 1
  uint8_t r4806;  //MMC bank 2
  uint8_t r4807;  //MMC bank 3

  struct DMA {
    uint32_t addr;  //$43x2-$43x4 -- DMA transfer address
    uint16_t size;  //$43x5-$43x6 -- DMA transfer size
  } dma[8];
  bool dmaReady;  //used to initialize decompression module

public:
  struct Decompressor {
    struct IM {  //input manager
      IM(SDD1::Decompressor& self) {}
      void init(unsigned offset);
      uint8_t getCodeWord(uint8_t codeLength);
      void serialize(serializer&);

    private:
      unsigned offset;
      unsigned bitCount;
    };

    struct GCD {  //golomb-code decoder
      GCD(SDD1::Decompressor& self) : self(self) {}
      void getRunCount(uint8_t codeNumber, uint8_t &mpsCount, bool& lpsIndex);
      void serialize(serializer&);

    private:
      Decompressor& self;
      static const uint8_t runCount[256];
    };

    struct BG {  //bits generator
      BG(SDD1::Decompressor& self, uint8_t codeNumber) : self(self), codeNumber(codeNumber) {}
      void init();
      uint8_t getBit(bool& endOfRun);
      void serialize(serializer&);

    private:
      Decompressor& self;
      const uint8_t codeNumber;
      uint8_t mpsCount;
      bool lpsIndex;
    };

    struct PEM {  //probability estimation module
      PEM(SDD1::Decompressor& self) : self(self) {}
      void init();
      uint8_t getBit(uint8_t context);
      void serialize(serializer&);

    private:
      Decompressor& self;
      struct State {
        uint8_t codeNumber;
        uint8_t nextIfMps;
        uint8_t nextIfLps;
      };
      static const State evolutionTable[33];
      struct ContextInfo {
        uint8_t status;
        uint8_t mps;
      } contextInfo[32];
    };

    struct CM {  //context model
      CM(SDD1::Decompressor& self) : self(self) {}
      void init(unsigned offset);
      uint8_t getBit();
      void serialize(serializer&);

    private:
      Decompressor& self;
      uint8_t bitplanesInfo;
      uint8_t contextBitsInfo;
      uint8_t bitNumber;
      uint8_t currentBitplane;
      uint16_t previousBitplaneBits[8];
    };

    struct OL {  //output logic
      OL(SDD1::Decompressor& self) : self(self) {}
      void init(unsigned offset);
      uint8_t decompress();
      void serialize(serializer&);

    private:
      Decompressor& self;
      uint8_t bitplanesInfo;
      uint8_t r0, r1, r2;
    };

    Decompressor();
    void init(unsigned offset);
    uint8_t read();
    void serialize(serializer&);

    IM  im;
    GCD gcd;
    BG  bg0, bg1, bg2, bg3, bg4, bg5, bg6, bg7;
    PEM pem;
    CM  cm;
    OL  ol;
  };

  Decompressor decompressor;
};

extern SDD1 sdd1;

}
