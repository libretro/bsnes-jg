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

#include "../sfc.hpp"
#include "../cpu.hpp"
#include "../memory.hpp"

#include "sdd1.hpp"

namespace SuperFamicom {

SDD1 sdd1;

//S-DD1 decompression algorithm implementation
//original code written by Andreas Naive (public domain license)
//bsnes port written by byuu

//note: decompression module does not need to be serialized with bsnes
//this is because decompression only runs during DMA, and bsnes will complete
//any pending DMA transfers prior to serialization.

//input manager

void SDD1::Decompressor::IM::init(unsigned offset_) {
  offset = offset_;
  bitCount = 4;
}

uint8_t SDD1::Decompressor::IM::getCodeWord(uint8_t codeLength) {
  uint8_t codeWord;

  codeWord = sdd1.mmcRead(offset) << bitCount;
  bitCount++;

  if(codeWord & 0x80) {
    codeWord |= sdd1.mmcRead(offset + 1) >> (9 - bitCount);
    bitCount += codeLength;
  }

  if(bitCount & 0x08) {
    offset++;
    bitCount &= 0x07;
  }

  return codeWord;
}

//golomb-code decoder

const uint8_t SDD1::Decompressor::GCD::runCount[] = {
  0x00, 0x00, 0x01, 0x00, 0x03, 0x01, 0x02, 0x00,
  0x07, 0x03, 0x05, 0x01, 0x06, 0x02, 0x04, 0x00,
  0x0f, 0x07, 0x0b, 0x03, 0x0d, 0x05, 0x09, 0x01,
  0x0e, 0x06, 0x0a, 0x02, 0x0c, 0x04, 0x08, 0x00,
  0x1f, 0x0f, 0x17, 0x07, 0x1b, 0x0b, 0x13, 0x03,
  0x1d, 0x0d, 0x15, 0x05, 0x19, 0x09, 0x11, 0x01,
  0x1e, 0x0e, 0x16, 0x06, 0x1a, 0x0a, 0x12, 0x02,
  0x1c, 0x0c, 0x14, 0x04, 0x18, 0x08, 0x10, 0x00,
  0x3f, 0x1f, 0x2f, 0x0f, 0x37, 0x17, 0x27, 0x07,
  0x3b, 0x1b, 0x2b, 0x0b, 0x33, 0x13, 0x23, 0x03,
  0x3d, 0x1d, 0x2d, 0x0d, 0x35, 0x15, 0x25, 0x05,
  0x39, 0x19, 0x29, 0x09, 0x31, 0x11, 0x21, 0x01,
  0x3e, 0x1e, 0x2e, 0x0e, 0x36, 0x16, 0x26, 0x06,
  0x3a, 0x1a, 0x2a, 0x0a, 0x32, 0x12, 0x22, 0x02,
  0x3c, 0x1c, 0x2c, 0x0c, 0x34, 0x14, 0x24, 0x04,
  0x38, 0x18, 0x28, 0x08, 0x30, 0x10, 0x20, 0x00,
  0x7f, 0x3f, 0x5f, 0x1f, 0x6f, 0x2f, 0x4f, 0x0f,
  0x77, 0x37, 0x57, 0x17, 0x67, 0x27, 0x47, 0x07,
  0x7b, 0x3b, 0x5b, 0x1b, 0x6b, 0x2b, 0x4b, 0x0b,
  0x73, 0x33, 0x53, 0x13, 0x63, 0x23, 0x43, 0x03,
  0x7d, 0x3d, 0x5d, 0x1d, 0x6d, 0x2d, 0x4d, 0x0d,
  0x75, 0x35, 0x55, 0x15, 0x65, 0x25, 0x45, 0x05,
  0x79, 0x39, 0x59, 0x19, 0x69, 0x29, 0x49, 0x09,
  0x71, 0x31, 0x51, 0x11, 0x61, 0x21, 0x41, 0x01,
  0x7e, 0x3e, 0x5e, 0x1e, 0x6e, 0x2e, 0x4e, 0x0e,
  0x76, 0x36, 0x56, 0x16, 0x66, 0x26, 0x46, 0x06,
  0x7a, 0x3a, 0x5a, 0x1a, 0x6a, 0x2a, 0x4a, 0x0a,
  0x72, 0x32, 0x52, 0x12, 0x62, 0x22, 0x42, 0x02,
  0x7c, 0x3c, 0x5c, 0x1c, 0x6c, 0x2c, 0x4c, 0x0c,
  0x74, 0x34, 0x54, 0x14, 0x64, 0x24, 0x44, 0x04,
  0x78, 0x38, 0x58, 0x18, 0x68, 0x28, 0x48, 0x08,
  0x70, 0x30, 0x50, 0x10, 0x60, 0x20, 0x40, 0x00,
};

void SDD1::Decompressor::GCD::getRunCount(uint8_t codeNumber, uint8_t &mpsCount, bool &lpsIndex) {
  uint8_t codeWord = self.im.getCodeWord(codeNumber);

  if(codeWord & 0x80) {
    lpsIndex = 1;
    mpsCount = runCount[codeWord >> (codeNumber ^ 0x07)];
  } else {
    mpsCount = 1 << codeNumber;
  }
}

//bits generator

void SDD1::Decompressor::BG::init() {
  mpsCount = 0;
  lpsIndex = 0;
}

uint8_t SDD1::Decompressor::BG::getBit(bool& endOfRun) {
  if(!(mpsCount || lpsIndex)) self.gcd.getRunCount(codeNumber, mpsCount, lpsIndex);

  uint8_t bit;
  if(mpsCount) {
    bit = 0;
    mpsCount--;
  } else {
    bit = 1;
    lpsIndex = 0;
  }

  endOfRun = !(mpsCount || lpsIndex);
  return bit;
}

//probability estimation module

const SDD1::Decompressor::PEM::State SDD1::Decompressor::PEM::evolutionTable[33] = {
  {0, 25, 25},
  {0,  2,  1},
  {0,  3,  1},
  {0,  4,  2},
  {0,  5,  3},
  {1,  6,  4},
  {1,  7,  5},
  {1,  8,  6},
  {1,  9,  7},
  {2, 10,  8},
  {2, 11,  9},
  {2, 12, 10},
  {2, 13, 11},
  {3, 14, 12},
  {3, 15, 13},
  {3, 16, 14},
  {3, 17, 15},
  {4, 18, 16},
  {4, 19, 17},
  {5, 20, 18},
  {5, 21, 19},
  {6, 22, 20},
  {6, 23, 21},
  {7, 24, 22},
  {7, 24, 23},
  {0, 26,  1},
  {1, 27,  2},
  {2, 28,  4},
  {3, 29,  8},
  {4, 30, 12},
  {5, 31, 16},
  {6, 32, 18},
  {7, 24, 22},
};

void SDD1::Decompressor::PEM::init() {
  for(auto n = 0; n < 32; ++n) {
    contextInfo[n].status = 0;
    contextInfo[n].mps = 0;
  }
}

uint8_t SDD1::Decompressor::PEM::getBit(uint8_t context) {
  ContextInfo& info = contextInfo[context];
  uint8_t currentStatus = info.status;
  uint8_t currentMps = info.mps;
  const State& s = SDD1::Decompressor::PEM::evolutionTable[currentStatus];

  uint8_t bit = 0;
  bool endOfRun = false;
  switch(s.codeNumber) {
  case 0: bit = self.bg0.getBit(endOfRun); break;
  case 1: bit = self.bg1.getBit(endOfRun); break;
  case 2: bit = self.bg2.getBit(endOfRun); break;
  case 3: bit = self.bg3.getBit(endOfRun); break;
  case 4: bit = self.bg4.getBit(endOfRun); break;
  case 5: bit = self.bg5.getBit(endOfRun); break;
  case 6: bit = self.bg6.getBit(endOfRun); break;
  case 7: bit = self.bg7.getBit(endOfRun); break;
  }

  if(endOfRun) {
    if(bit) {
      if(!(currentStatus & 0xfe)) info.mps ^= 0x01;
      info.status = s.nextIfLps;
    } else {
      info.status = s.nextIfMps;
    }
  }

  return bit ^ currentMps;
}

//context model

void SDD1::Decompressor::CM::init(unsigned offset) {
  bitplanesInfo = sdd1.mmcRead(offset) & 0xc0;
  contextBitsInfo = sdd1.mmcRead(offset) & 0x30;
  bitNumber = 0;
  for(auto n = 0; n < 8; ++n) previousBitplaneBits[n] = 0;
  switch(bitplanesInfo) {
  case 0x00: currentBitplane = 1; break;
  case 0x40: currentBitplane = 7; break;
  case 0x80: currentBitplane = 3; break;
  }
}

uint8_t SDD1::Decompressor::CM::getBit() {
  switch(bitplanesInfo) {
  case 0x00:
    currentBitplane ^= 0x01;
    break;
  case 0x40:
    currentBitplane ^= 0x01;
    if(!(bitNumber & 0x7f)) currentBitplane = ((currentBitplane + 2) & 0x07);
    break;
  case 0x80:
    currentBitplane ^= 0x01;
    if(!(bitNumber & 0x7f)) currentBitplane ^= 0x02;
    break;
  case 0xc0:
    currentBitplane = bitNumber & 0x07;
    break;
  }

  uint16_t &contextBits = previousBitplaneBits[currentBitplane];
  uint8_t currentContext = (currentBitplane & 0x01) << 4;
  switch(contextBitsInfo) {
  case 0x00: currentContext |= ((contextBits & 0x01c0) >> 5) | (contextBits & 0x0001); break;
  case 0x10: currentContext |= ((contextBits & 0x0180) >> 5) | (contextBits & 0x0001); break;
  case 0x20: currentContext |= ((contextBits & 0x00c0) >> 5) | (contextBits & 0x0001); break;
  case 0x30: currentContext |= ((contextBits & 0x0180) >> 5) | (contextBits & 0x0003); break;
  }

  uint8_t bit = self.pem.getBit(currentContext);
  contextBits <<= 1;
  contextBits |= bit;
  bitNumber++;
  return bit;
}

//output logic

void SDD1::Decompressor::OL::init(unsigned offset) {
  bitplanesInfo = sdd1.mmcRead(offset) & 0xc0;
  r0 = 0x01;
}

uint8_t SDD1::Decompressor::OL::decompress() {
  switch(bitplanesInfo) {
  case 0x00: case 0x40: case 0x80:
    if(r0 == 0) {
      r0 = ~r0;
      return r2;
    }
    for(r0 = 0x80, r1 = 0, r2 = 0; r0; r0 >>= 1) {
      if(self.cm.getBit()) r1 |= r0;
      if(self.cm.getBit()) r2 |= r0;
    }
    return r1;
  case 0xc0:
    for(r0 = 0x01, r1 = 0; r0; r0 <<= 1) {
      if(self.cm.getBit()) r1 |= r0;
    }
    return r1;
  }

  return 0;  //unreachable?
}

//core

SDD1::Decompressor::Decompressor():
im(*this), gcd(*this),
bg0(*this, 0), bg1(*this, 1), bg2(*this, 2), bg3(*this, 3),
bg4(*this, 4), bg5(*this, 5), bg6(*this, 6), bg7(*this, 7),
pem(*this), cm(*this), ol(*this) {
}

void SDD1::Decompressor::init(unsigned offset) {
  im.init(offset);
  bg0.init();
  bg1.init();
  bg2.init();
  bg3.init();
  bg4.init();
  bg5.init();
  bg6.init();
  bg7.init();
  pem.init();
  cm.init(offset);
  ol.init(offset);
}

uint8_t SDD1::Decompressor::read() {
  return ol.decompress();
}

void SDD1::serialize(serializer& s) {
  s.integer(r4800);
  s.integer(r4801);
  s.integer(r4804);
  s.integer(r4805);
  s.integer(r4806);
  s.integer(r4807);

  for(auto& channel : dma) {
    s.integer(channel.addr);
    s.integer(channel.size);
  }
  s.integer(dmaReady);

  decompressor.serialize(s);
}

void SDD1::Decompressor::serialize(serializer& s) {
  im.serialize(s);
  gcd.serialize(s);
  bg0.serialize(s);
  bg1.serialize(s);
  bg2.serialize(s);
  bg3.serialize(s);
  bg4.serialize(s);
  bg5.serialize(s);
  bg6.serialize(s);
  bg7.serialize(s);
  pem.serialize(s);
  cm.serialize(s);
  ol.serialize(s);
}

void SDD1::Decompressor::IM::serialize(serializer& s) {
  s.integer(offset);
  s.integer(bitCount);
}

void SDD1::Decompressor::GCD::serialize(serializer& s) {
}

void SDD1::Decompressor::BG::serialize(serializer& s) {
  s.integer(mpsCount);
  s.integer(lpsIndex);
}

void SDD1::Decompressor::PEM::serialize(serializer& s) {
  for(auto& info : contextInfo) {
    s.integer(info.status);
    s.integer(info.mps);
  }
}

void SDD1::Decompressor::CM::serialize(serializer& s) {
  s.integer(bitplanesInfo);
  s.integer(contextBitsInfo);
  s.integer(bitNumber);
  s.integer(currentBitplane);
  s.array(previousBitplaneBits);
}

void SDD1::Decompressor::OL::serialize(serializer& s) {
  s.integer(bitplanesInfo);
  s.integer(r0);
  s.integer(r1);
  s.integer(r2);
}

void SDD1::unload() {
  rom.reset();
}

void SDD1::power() {
  //hook S-CPU DMA MMIO registers to gather information for struct dma[];
  //buffer address and transfer size information for use in SDD1::mcu_read()
  bus.map({&SDD1::dmaRead, &sdd1}, {&SDD1::dmaWrite, &sdd1}, "00-3f,80-bf:4300-437f");

  r4800 = 0x00;
  r4801 = 0x00;
  r4804 = 0x00;
  r4805 = 0x01;
  r4806 = 0x02;
  r4807 = 0x03;

  for(auto n = 0; n < 8; ++n) {
    dma[n].addr = 0;
    dma[n].size = 0;
  }
  dmaReady = false;
}

uint8_t SDD1::ioRead(unsigned addr, uint8_t data) {
  addr = 0x4800 | (addr & 0xf);

  switch(addr) {
  case 0x4800: return r4800;
  case 0x4801: return r4801;
  case 0x4804: return r4804;
  case 0x4805: return r4805;
  case 0x4806: return r4806;
  case 0x4807: return r4807;
  }

  //00-3f,80-bf:4802-4803,4808-480f falls through to ROM
  return rom.read(addr);
}

void SDD1::ioWrite(unsigned addr, uint8_t data) {
  addr = 0x4800 | (addr & 0xf);

  switch(addr) {
  case 0x4800: r4800 = data; break;
  case 0x4801: r4801 = data; break;
  case 0x4804: r4804 = data & 0x8f; break;
  case 0x4805: r4805 = data & 0x8f; break;
  case 0x4806: r4806 = data & 0x8f; break;
  case 0x4807: r4807 = data & 0x8f; break;
  }
}

uint8_t SDD1::dmaRead(unsigned addr, uint8_t data) {
  return cpu.readDMA(addr, data);
}

void SDD1::dmaWrite(unsigned addr, uint8_t data) {
  unsigned channel = addr >> 4 & 7;
  switch(addr & 15) {
  case 2: dma[channel].addr = (dma[channel].addr & 0xffff00) | data <<  0; break;
  case 3: dma[channel].addr = (dma[channel].addr & 0xff00ff) | data <<  8; break;
  case 4: dma[channel].addr = (dma[channel].addr & 0x00ffff) | data << 16; break;
  case 5: dma[channel].size = (dma[channel].size & 0xff00) | data << 0; break;
  case 6: dma[channel].size = (dma[channel].size & 0x00ff) | data << 8; break;
  }
  return cpu.writeDMA(addr, data);
}

uint8_t SDD1::mmcRead(unsigned addr) {
  switch(addr >> 20 & 3) {
  case 0: return rom.read((r4804 & 0xf) << 20 | (addr & 0xfffff));  //c0-cf:0000-ffff
  case 1: return rom.read((r4805 & 0xf) << 20 | (addr & 0xfffff));  //d0-df:0000-ffff
  case 2: return rom.read((r4806 & 0xf) << 20 | (addr & 0xfffff));  //e0-ef:0000-ffff
  case 3: return rom.read((r4807 & 0xf) << 20 | (addr & 0xfffff));  //f0-ff:0000-ffff
  }
  return 0; // unreachable
}

//map address=00-3f,80-bf:8000-ffff
//map address=c0-ff:0000-ffff
uint8_t SDD1::mcuRead(unsigned addr, uint8_t data) {
  //map address=00-3f,80-bf:8000-ffff
  if(!(addr & 1 << 22)) {
    if(!(addr & 1 << 23) && (addr & 1 << 21) && (r4805 & 0x80)) addr &= ~(1 << 21);  //20-3f:8000-ffff
    if( (addr & 1 << 23) && (addr & 1 << 21) && (r4807 & 0x80)) addr &= ~(1 << 21);  //a0-bf:8000-ffff
    addr = (addr >> 1 & 0x1f8000) | (addr & 0x7fff);
    return rom.read(addr);
  }

  //map address=c0-ff:0000-ffff
  if(r4800 & r4801) {
    //at least one channel has S-DD1 decompression enabled ...
    for(auto n = 0; n < 8; ++n) {
      if((r4800 & 1 << n) && (r4801 & 1 << n)) {
        //S-DD1 always uses fixed transfer mode, so address will not change during transfer
        if(addr == dma[n].addr) {
          if(!dmaReady) {
            //prepare streaming decompression
            decompressor.init(addr);
            dmaReady = true;
          }

          //fetch a decompressed byte; once finished, disable channel and invalidate buffer
          data = decompressor.read();
          if(--dma[n].size == 0) {
            dmaReady = false;
            r4801 &= ~(1 << n);
          }

          return data;
        }  //address matched
      }  //channel enabled
    }  //channel loop
  }  //S-DD1 decompressor enabled

  //S-DD1 decompression mode inactive; return ROM data
  return mmcRead(addr);
}

void SDD1::mcuWrite(unsigned addr, uint8_t data) {
}

}
