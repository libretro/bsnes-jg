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

#include <iostream>

#include "cpu.hpp"
#include "random.hpp"
#include "settings.hpp"

namespace SuperFamicom {

extern Emulator::Random random;

PPU ppu;

void PPU::main() {
  if(vcounter() == 0) {
    if(display.overscan && !io.overscan) {
      //when disabling overscan, clear the overscan area that won't be rendered to:
      for(unsigned y = 1; y <= 240; y++) {
        if(y >= 8 && y <= 231) continue;
        auto output = ppu.output + y * 1024;
        std::memset(output, 0, 1024 * sizeof(uint16_t));
      }
    }
    display.interlace = io.interlace;
    display.overscan = io.overscan;
    bg1.frame();
    bg2.frame();
    bg3.frame();
    bg4.frame();
    obj.frame();
  }

  mosaic.scanline();
  bg1.scanline();
  bg2.scanline();
  bg3.scanline();
  bg4.scanline();
  obj.scanline();
  window.scanline();
  screen.scanline();

  if(vcounter() > 240) {
    step(hperiod());
    return;
  }

  #define cycles02(index) cycle<index>()
  #define cycles04(index) cycles02(index); cycles02(index +  2)
  #define cycles08(index) cycles04(index); cycles04(index +  4)
  #define cycles16(index) cycles08(index); cycles08(index +  8)
  #define cycles32(index) cycles16(index); cycles16(index + 16)
  #define cycles64(index) cycles32(index); cycles32(index + 32)
  cycles16(   0);
  cycles04(  16);
  //H =   20
  cycles04(  20);
  cycles04(  24);
  //H =   28
  cycles04(  28);
  cycles32(  32);
  cycles64(  64);
  cycles64( 128);
  cycles64( 192);
  cycles64( 256);
  cycles64( 320);
  cycles64( 384);
  cycles64( 448);
  cycles64( 512);
  cycles64( 576);
  cycles64( 640);
  cycles64( 704);
  cycles64( 768);
  cycles64( 832);
  cycles64( 896);
  cycles64( 960);
  cycles32(1024);
  cycles16(1056);
  cycles08(1072);
  //H = 1080
  obj.fetch();
  //H = 1352 (max)
  step(hperiod() - hcounter());
}

//it would be lovely if we could put these functions inside cycle(),
//but due to the multiple template instantiations, that destroys L1 cache.
//it's a performance penalty of about 25% for the entire(!!) emulator.

void PPU::cycleObjectEvaluate() {
  obj.evaluate((hcounter() >> 3) & 0x7f);
}

template<unsigned Cycle>
void PPU::cycleBackgroundFetch() {
  switch(io.bgMode) {
  case 0:
    if constexpr(Cycle == 0) bg4.fetchNameTable();
    if constexpr(Cycle == 1) bg3.fetchNameTable();
    if constexpr(Cycle == 2) bg2.fetchNameTable();
    if constexpr(Cycle == 3) bg1.fetchNameTable();
    if constexpr(Cycle == 4) bg4.fetchCharacter(0);
    if constexpr(Cycle == 5) bg3.fetchCharacter(0);
    if constexpr(Cycle == 6) bg2.fetchCharacter(0);
    if constexpr(Cycle == 7) bg1.fetchCharacter(0);
    break;
  case 1:
    if constexpr(Cycle == 0) bg3.fetchNameTable();
    if constexpr(Cycle == 1) bg2.fetchNameTable();
    if constexpr(Cycle == 2) bg1.fetchNameTable();
    if constexpr(Cycle == 3) bg3.fetchCharacter(0);
    if constexpr(Cycle == 4) bg2.fetchCharacter(0);
    if constexpr(Cycle == 5) bg2.fetchCharacter(1);
    if constexpr(Cycle == 6) bg1.fetchCharacter(0);
    if constexpr(Cycle == 7) bg1.fetchCharacter(1);
    break;
  case 2:
    if constexpr(Cycle == 0) bg2.fetchNameTable();
    if constexpr(Cycle == 1) bg1.fetchNameTable();
    if constexpr(Cycle == 2) bg3.fetchOffset(0);
    if constexpr(Cycle == 3) bg3.fetchOffset(8);
    if constexpr(Cycle == 4) bg2.fetchCharacter(0);
    if constexpr(Cycle == 5) bg2.fetchCharacter(1);
    if constexpr(Cycle == 6) bg1.fetchCharacter(0);
    if constexpr(Cycle == 7) bg1.fetchCharacter(1);
    break;
  case 3:
    if constexpr(Cycle == 0) bg2.fetchNameTable();
    if constexpr(Cycle == 1) bg1.fetchNameTable();
    if constexpr(Cycle == 2) bg2.fetchCharacter(0);
    if constexpr(Cycle == 3) bg2.fetchCharacter(1);
    if constexpr(Cycle == 4) bg1.fetchCharacter(0);
    if constexpr(Cycle == 5) bg1.fetchCharacter(1);
    if constexpr(Cycle == 6) bg1.fetchCharacter(2);
    if constexpr(Cycle == 7) bg1.fetchCharacter(3);
    break;
  case 4:
    if constexpr(Cycle == 0) bg2.fetchNameTable();
    if constexpr(Cycle == 1) bg1.fetchNameTable();
    if constexpr(Cycle == 2) bg3.fetchOffset(0);
    if constexpr(Cycle == 3) bg2.fetchCharacter(0);
    if constexpr(Cycle == 4) bg1.fetchCharacter(0);
    if constexpr(Cycle == 5) bg1.fetchCharacter(1);
    if constexpr(Cycle == 6) bg1.fetchCharacter(2);
    if constexpr(Cycle == 7) bg1.fetchCharacter(3);
    break;
  case 5:
    if constexpr(Cycle == 0) bg2.fetchNameTable();
    if constexpr(Cycle == 1) bg1.fetchNameTable();
    if constexpr(Cycle == 2) bg2.fetchCharacter(0, 0);
    if constexpr(Cycle == 3) bg2.fetchCharacter(0, 1);
    if constexpr(Cycle == 4) bg1.fetchCharacter(0, 0);
    if constexpr(Cycle == 5) bg1.fetchCharacter(1, 0);
    if constexpr(Cycle == 6) bg1.fetchCharacter(0, 1);
    if constexpr(Cycle == 7) bg1.fetchCharacter(1, 1);
    break;
  case 6:
    if constexpr(Cycle == 0) bg2.fetchNameTable();
    if constexpr(Cycle == 1) bg1.fetchNameTable();
    if constexpr(Cycle == 2) bg3.fetchOffset(0);
    if constexpr(Cycle == 3) bg3.fetchOffset(8);
    if constexpr(Cycle == 4) bg1.fetchCharacter(0, 0);
    if constexpr(Cycle == 5) bg1.fetchCharacter(1, 0);
    if constexpr(Cycle == 6) bg1.fetchCharacter(0, 1);
    if constexpr(Cycle == 7) bg1.fetchCharacter(1, 1);
    break;
  case 7:
    //handled separately by mode7.cpp
    break;
  }
}

void PPU::cycleBackgroundBegin() {
  bg1.begin();
  bg2.begin();
  bg3.begin();
  bg4.begin();
}

void PPU::cycleBackgroundBelow() {
  bg1.run(1);
  bg2.run(1);
  bg3.run(1);
  bg4.run(1);
}

void PPU::cycleBackgroundAbove() {
  bg1.run(0);
  bg2.run(0);
  bg3.run(0);
  bg4.run(0);
}

void PPU::cycleRenderPixel() {
  obj.run();
  window.run();
  screen.run();
}

template<unsigned Cycle>
void PPU::cycle() {
  if constexpr(Cycle >=  0 && Cycle <= 1016 && (Cycle -  0) % 8 == 0) cycleObjectEvaluate();
  if constexpr(Cycle >=  0 && Cycle <= 1054 && (Cycle -  0) % 4 == 0) cycleBackgroundFetch<(Cycle - 0) / 4 & 7>();
  if constexpr(Cycle == 56                                          ) cycleBackgroundBegin();
  if constexpr(Cycle >= 56 && Cycle <= 1078 && (Cycle - 56) % 4 == 0) cycleBackgroundBelow();
  if constexpr(Cycle >= 56 && Cycle <= 1078 && (Cycle - 56) % 4 == 2) cycleBackgroundAbove();
  if constexpr(Cycle >= 56 && Cycle <= 1078 && (Cycle - 56) % 4 == 2) cycleRenderPixel();
  step();
}

void PPU::latchCounters(unsigned hcounter, unsigned vcounter) {
  io.hcounter = hcounter;
  io.vcounter = vcounter;
  latch.counters = 1;
}

void PPU::latchCounters() {
  cpu.synchronizePPU();
  io.hcounter = hdot();
  io.vcounter = vcounter();
  latch.counters = 1;
}

uint16_t PPU::addressVRAM() const {
  uint16_t address = io.vramAddress;
  switch(io.vramMapping) {
  case 0: return address;
  case 1: return (address & 0xff00) | (address << 3 & 0x00f8) | (address >> 5 & 7);
  case 2: return (address & 0xfe00) | (address << 3 & 0x01f8) | (address >> 6 & 7);
  case 3: return (address & 0xfc00) | (address << 3 & 0x03f8) | (address >> 7 & 7);
  }
  return 0; // unreachable
}

uint16_t PPU::readVRAM() {
  if(!io.displayDisable && vcounter() < vdisp()) return 0x0000;
  auto address = addressVRAM();
  return vram[address];
}

void PPU::writeVRAM(bool byte, uint8_t data) {
  if(!io.displayDisable && vcounter() < vdisp()) return;
  auto address = addressVRAM();
  if(byte == 0) vram[address] = (vram[address] & 0xff00) | data << 0;
  if(byte == 1) vram[address] = (vram[address] & 0x00ff) | data << 8;
}

uint8_t PPU::readOAM(nall::Natural<10> addr) {
  if(!io.displayDisable && vcounter() < vdisp()) addr = latch.oamAddress;
  return obj.oam.read(addr);
}

void PPU::writeOAM(nall::Natural<10> addr, uint8_t data) {
  if(!io.displayDisable && vcounter() < vdisp()) addr = latch.oamAddress;
  obj.oam.write(addr, data);
}

uint8_t PPU::readCGRAM(bool byte, uint8_t addr) {
  if(!io.displayDisable
  && vcounter() > 0 && vcounter() < vdisp()
  && hcounter() >= 88 && hcounter() < 1096
  ) addr = latch.cgramAddress;
  return byte ? (screen.cgram[addr] & 0x7f00) >> 8 : screen.cgram[addr] & 0xff;
}

void PPU::writeCGRAM(uint8_t addr, uint16_t data) {
  if(!io.displayDisable
  && vcounter() > 0 && vcounter() < vdisp()
  && hcounter() >= 88 && hcounter() < 1096
  ) addr = latch.cgramAddress;
  screen.cgram[addr] = data;
}

uint8_t PPU::readIO(unsigned addr, uint8_t data) {
  cpu.synchronizePPU();

  switch(addr & 0xffff) {

  case 0x2104: case 0x2105: case 0x2106: case 0x2108:
  case 0x2109: case 0x210a: case 0x2114: case 0x2115:
  case 0x2116: case 0x2118: case 0x2119: case 0x211a:
  case 0x2124: case 0x2125: case 0x2126: case 0x2128:
  case 0x2129: case 0x212a: {
    return ppu1.mdr;
  }

  //MPYL
  case 0x2134: {
    uint32_t result = (int16_t)io.m7a * (int8_t)(io.m7b >> 8);
    return ppu1.mdr = result & 0xff;
  }

  //MPYM
  case 0x2135: {
    uint32_t result = (int16_t)io.m7a * (int8_t)(io.m7b >> 8);
    return ppu1.mdr = (result >> 8) & 0xff;
  }

  //MPYH
  case 0x2136: {
    uint32_t result = (int16_t)io.m7a * (int8_t)(io.m7b >> 8);
    return ppu1.mdr = (result >> 16) & 0xff;
  }

  //SLHV
  case 0x2137: {
    if(cpu.pio() & 0x80) latchCounters();
    return data;  //CPU MDR
  }

  //OAMDATAREAD
  case 0x2138: {
    ppu1.mdr = readOAM(io.oamAddress++);
    obj.setFirstSprite();
    return ppu1.mdr;
  }

  //VMDATALREAD
  case 0x2139: {
    ppu1.mdr = latch.vram >> 0;
    if(io.vramIncrementMode == 0) {
      latch.vram = readVRAM();
      io.vramAddress += io.vramIncrementSize;
    }
    return ppu1.mdr;
  }

  //VMDATAHREAD
  case 0x213a: {
    ppu1.mdr = latch.vram >> 8;
    if(io.vramIncrementMode == 1) {
      latch.vram = readVRAM();
      io.vramAddress += io.vramIncrementSize;
    }
    return ppu1.mdr;
  }

  //CGDATAREAD
  case 0x213b: {
    if(io.cgramAddressLatch) {
      ppu2.mdr &= 0x80;
      ppu2.mdr |= readCGRAM(1, io.cgramAddress++) & 0x7f;
    } else {
      ppu2.mdr = readCGRAM(0, io.cgramAddress);
    }
    io.cgramAddressLatch ^= 1;
    return ppu2.mdr;
  }

  //OPHCT
  case 0x213c: {
    if(latch.hcounter) {
      ppu2.mdr &= 0xfe;
      ppu2.mdr |= io.hcounter >> 8 & 1;
    } else {
      ppu2.mdr = io.hcounter >> 0;
    }
    latch.hcounter ^= 1;
    return ppu2.mdr;
  }

  //OPVCT
  case 0x213d: {
    if(latch.vcounter) {
      ppu2.mdr &= 0xfe;
      ppu2.mdr |= io.vcounter >> 8 & 1;
    } else {
      ppu2.mdr = io.vcounter >> 0;
    }
    latch.vcounter ^= 1;
    return ppu2.mdr;
  }

  //STAT77
  case 0x213e: {
    ppu1.mdr &= 1 << 4;
    ppu1.mdr |= ppu1.version << 0;
    ppu1.mdr |= obj.io.rangeOver << 6;
    ppu1.mdr |= obj.io.timeOver << 7;
    return ppu1.mdr;
  }

  //STAT78
  case 0x213f: {
    latch.hcounter = 0;
    latch.vcounter = 0;
    ppu2.mdr &= 1 << 5;
    ppu2.mdr |= ppu2.version;
    ppu2.mdr |= Region::PAL() << 4;  //0 = NTSC, 1 = PAL
    if(!(cpu.pio() & 0x80)) {
      ppu2.mdr |= 1 << 6;
    } else {
      ppu2.mdr |= latch.counters << 6;
      latch.counters = 0;
    }
    ppu2.mdr |= field() << 7;
    return ppu2.mdr;
  }

  }

  return data;
}

void PPU::writeIO(unsigned addr, uint8_t data) {
  cpu.synchronizePPU();

  switch(addr & 0xffff) {

  //INIDISP
  case 0x2100: {
    if(io.displayDisable && vcounter() == vdisp()) obj.addressReset();
    io.displayBrightness = data >> 0 & 15;
    io.displayDisable    = data >> 7 & 1;
    return;
  }

  //OBSEL
  case 0x2101: {
    obj.io.tiledataAddress = (data & 7) << 13;
    obj.io.nameselect      = data >> 3 & 3;
    obj.io.baseSize        = data >> 5 & 7;
    return;
  }

  //OAMADDL
  case 0x2102: {
    io.oamBaseAddress = (io.oamBaseAddress & 0x0200) | data << 1;
    obj.addressReset();
    return;
  }

  //OAMADDH
  case 0x2103: {
    io.oamBaseAddress = (data & 1) << 9 | (io.oamBaseAddress & 0x01fe);
    io.oamPriority    = (data & 0x80) ? 1 : 0;
    obj.addressReset();
    return;
  }

  //OAMDATA
  case 0x2104: {
    uint8_t latchBit = io.oamAddress & 1;
    nall::Natural<10> address = io.oamAddress++;
    if(latchBit == 0) latch.oam = data;
    if(address & 0x200) {
      writeOAM(address, data);
    } else if(latchBit == 1) {
      writeOAM((address & ~1) + 0, latch.oam);
      writeOAM((address & ~1) + 1, data);
    }
    obj.setFirstSprite();
    return;
  }

  //BGMODE
  case 0x2105: {
    io.bgMode       = data >> 0 & 7;
    io.bgPriority   = data >> 3 & 1;
    bg1.io.tileSize = data >> 4 & 1;
    bg2.io.tileSize = data >> 5 & 1;
    bg3.io.tileSize = data >> 6 & 1;
    bg4.io.tileSize = data >> 7 & 1;
    updateVideoMode();
    return;
  }

  //MOSAIC
  case 0x2106: {
    bool mosaicEnable = mosaic.enable();
    bg1.mosaic.enable = data >> 0 & 1;
    bg2.mosaic.enable = data >> 1 & 1;
    bg3.mosaic.enable = data >> 2 & 1;
    bg4.mosaic.enable = data >> 3 & 1;
    mosaic.size       = ((data >> 4 & 15) + 1) & 0x1f;
    if(!mosaicEnable && mosaic.enable()) {
      //mosaic vcounter is reloaded when mosaic becomes enabled
      mosaic.vcounter = (mosaic.size + 1) & 0x1f;
    }
    return;
  }

  //BG1SC
  case 0x2107: {
    bg1.io.screenSize    = data & 3;
    bg1.io.screenAddress = data >> 2 << 10;
    return;
  }

  //BG2SC
  case 0x2108: {
    bg2.io.screenSize    = data & 3;
    bg2.io.screenAddress = data >> 2 << 10;
    return;
  }

  //BG3SC
  case 0x2109: {
    bg3.io.screenSize    = data & 3;
    bg3.io.screenAddress = data >> 2 << 10;
    return;
  }

  //BG4SC
  case 0x210a: {
    bg4.io.screenSize    = data & 3;
    bg4.io.screenAddress = data >> 2 << 10;
    return;
  }

  //BG12NBA
  case 0x210b: {
    bg1.io.tiledataAddress = (data >> 0 & 15) << 12;
    bg2.io.tiledataAddress = (data >> 4 & 15) << 12;
    return;
  }

  //BG34NBA
  case 0x210c: {
    bg3.io.tiledataAddress = (data >> 0 & 15) << 12;
    bg4.io.tiledataAddress = (data >> 4 & 15) << 12;
    return;
  }

  //BG1HOFS
  case 0x210d: {
    io.hoffsetMode7 = data << 8 | latch.mode7;
    latch.mode7 = data;

    bg1.io.hoffset = data << 8 | (latch.bgofsPPU1 & ~7) | (latch.bgofsPPU2 & 7);
    latch.bgofsPPU1 = data;
    latch.bgofsPPU2 = data & 7;
    return;
  }

  //BG1VOFS
  case 0x210e: {
    io.voffsetMode7 = data << 8 | latch.mode7;
    latch.mode7 = data;

    bg1.io.voffset = data << 8 | latch.bgofsPPU1;
    latch.bgofsPPU1 = data;
    return;
  }

  //BG2HOFS
  case 0x210f: {
    bg2.io.hoffset = data << 8 | (latch.bgofsPPU1 & ~7) | (latch.bgofsPPU2 & 7);
    latch.bgofsPPU1 = data;
    latch.bgofsPPU2 = data & 7;
    return;
  }

  //BG2VOFS
  case 0x2110: {
    bg2.io.voffset = data << 8 | latch.bgofsPPU1;
    latch.bgofsPPU1 = data;
    return;
  }

  //BG3HOFS
  case 0x2111: {
    bg3.io.hoffset = data << 8 | (latch.bgofsPPU1 & ~7) | (latch.bgofsPPU2 & 7);
    latch.bgofsPPU1 = data;
    latch.bgofsPPU2 = data & 7;
    return;
  }

  //BG3VOFS
  case 0x2112: {
    bg3.io.voffset = data << 8 | latch.bgofsPPU1;
    latch.bgofsPPU1 = data;
    return;
  }

  //BG4HOFS
  case 0x2113: {
    bg4.io.hoffset = data << 8 | (latch.bgofsPPU1 & ~7) | (latch.bgofsPPU2 & 7);
    latch.bgofsPPU1 = data;
    latch.bgofsPPU2 = data & 7;
    return;
  }

  //BG4VOFS
  case 0x2114: {
    bg4.io.voffset = data << 8 | latch.bgofsPPU1;
    latch.bgofsPPU1 = data;
    return;
  }

  //VMAIN
  case 0x2115: {
    static const unsigned size[4] = {1, 32, 128, 128};
    io.vramIncrementSize = size[data & 3];
    io.vramMapping       = data >> 2 & 3;
    io.vramIncrementMode = data >> 7 & 1;
    return;
  }

  //VMADDL
  case 0x2116: {
    io.vramAddress = (io.vramAddress & 0xff00) | data << 0;
    latch.vram = readVRAM();
    return;
  }

  //VMADDH
  case 0x2117: {
    io.vramAddress = (io.vramAddress & 0x00ff) | data << 8;
    latch.vram = readVRAM();
    return;
  }

  //VMDATAL
  case 0x2118: {
    writeVRAM(0, data);
    if(io.vramIncrementMode == 0) io.vramAddress += io.vramIncrementSize;
    return;
  }

  //VMDATAH
  case 0x2119: {
    writeVRAM(1, data);
    if(io.vramIncrementMode == 1) io.vramAddress += io.vramIncrementSize;
    return;
  }

  //M7SEL
  case 0x211a: {
    io.hflipMode7  = data >> 0 & 1;
    io.vflipMode7  = data >> 1 & 1;
    io.repeatMode7 = data >> 6 & 3;
    return;
  }

  //M7A
  case 0x211b: {
    io.m7a = data << 8 | latch.mode7;
    latch.mode7 = data;
    return;
  }

  //M7B
  case 0x211c: {
    io.m7b = data << 8 | latch.mode7;
    latch.mode7 = data;
    return;
  }

  //M7C
  case 0x211d: {
    io.m7c = data << 8 | latch.mode7;
    latch.mode7 = data;
    return;
  }

  //M7D
  case 0x211e: {
    io.m7d = data << 8 | latch.mode7;
    latch.mode7 = data;
    return;
  }

  //M7X
  case 0x211f: {
    io.m7x = data << 8 | latch.mode7;
    latch.mode7 = data;
    return;
  }

  //M7Y
  case 0x2120: {
    io.m7y = data << 8 | latch.mode7;
    latch.mode7 = data;
    return;
  }

  //CGADD
  case 0x2121: {
    io.cgramAddress = data;
    io.cgramAddressLatch = 0;
    return;
  }

  //CGDATA
  case 0x2122: {
    if(io.cgramAddressLatch) {
      writeCGRAM(io.cgramAddress++, (data & 0x7f) << 8 | latch.cgram);
    } else {
      latch.cgram = data;
    }
    io.cgramAddressLatch ^= 1;
    return;
  }

  //W12SEL
  case 0x2123: {
    window.io.bg1.oneInvert = data >> 0 & 1;
    window.io.bg1.oneEnable = data >> 1 & 1;
    window.io.bg1.twoInvert = data >> 2 & 1;
    window.io.bg1.twoEnable = data >> 3 & 1;
    window.io.bg2.oneInvert = data >> 4 & 1;
    window.io.bg2.oneEnable = data >> 5 & 1;
    window.io.bg2.twoInvert = data >> 6 & 1;
    window.io.bg2.twoEnable = data >> 7 & 1;
    return;
  }

  //W34SEL
  case 0x2124: {
    window.io.bg3.oneInvert = data >> 0 & 1;
    window.io.bg3.oneEnable = data >> 1 & 1;
    window.io.bg3.twoInvert = data >> 2 & 1;
    window.io.bg3.twoEnable = data >> 3 & 1;
    window.io.bg4.oneInvert = data >> 4 & 1;
    window.io.bg4.oneEnable = data >> 5 & 1;
    window.io.bg4.twoInvert = data >> 6 & 1;
    window.io.bg4.twoEnable = data >> 7 & 1;
    return;
  }

  //WOBJSEL
  case 0x2125: {
    window.io.obj.oneInvert = data >> 0 & 1;
    window.io.obj.oneEnable = data >> 1 & 1;
    window.io.obj.twoInvert = data >> 2 & 1;
    window.io.obj.twoEnable = data >> 3 & 1;
    window.io.col.oneInvert = data >> 4 & 1;
    window.io.col.oneEnable = data >> 5 & 1;
    window.io.col.twoInvert = data >> 6 & 1;
    window.io.col.twoEnable = data >> 7 & 1;
    return;
  }

  //WH0
  case 0x2126: {
    window.io.oneLeft = data;
    return;
  }

  //WH1
  case 0x2127: {
    window.io.oneRight = data;
    return;
  }

  //WH2
  case 0x2128: {
    window.io.twoLeft = data;
    return;
  }

  //WH3
  case 0x2129: {
    window.io.twoRight = data;
    return;
  }

  //WBGLOG
  case 0x212a: {
    window.io.bg1.mask = data >> 0 & 3;
    window.io.bg2.mask = data >> 2 & 3;
    window.io.bg3.mask = data >> 4 & 3;
    window.io.bg4.mask = data >> 6 & 3;
    return;
  }

  //WOBJLOG
  case 0x212b: {
    window.io.obj.mask = data >> 0 & 3;
    window.io.col.mask = data >> 2 & 3;
    return;
  }

  //TM
  case 0x212c: {
    bg1.io.aboveEnable = data >> 0 & 1;
    bg2.io.aboveEnable = data >> 1 & 1;
    bg3.io.aboveEnable = data >> 2 & 1;
    bg4.io.aboveEnable = data >> 3 & 1;
    obj.io.aboveEnable = data >> 4 & 1;
    return;
  }

  //TS
  case 0x212d: {
    bg1.io.belowEnable = data >> 0 & 1;
    bg2.io.belowEnable = data >> 1 & 1;
    bg3.io.belowEnable = data >> 2 & 1;
    bg4.io.belowEnable = data >> 3 & 1;
    obj.io.belowEnable = data >> 4 & 1;
    return;
  }

  //TMW
  case 0x212e: {
    window.io.bg1.aboveEnable = data >> 0 & 1;
    window.io.bg2.aboveEnable = data >> 1 & 1;
    window.io.bg3.aboveEnable = data >> 2 & 1;
    window.io.bg4.aboveEnable = data >> 3 & 1;
    window.io.obj.aboveEnable = data >> 4 & 1;
    return;
  }

  //TSW
  case 0x212f: {
    window.io.bg1.belowEnable = data >> 0 & 1;
    window.io.bg2.belowEnable = data >> 1 & 1;
    window.io.bg3.belowEnable = data >> 2 & 1;
    window.io.bg4.belowEnable = data >> 3 & 1;
    window.io.obj.belowEnable = data >> 4 & 1;
    return;
  }

  //CGWSEL
  case 0x2130: {
    screen.io.directColor   = data >> 0 & 1;
    screen.io.blendMode     = data >> 1 & 1;
    window.io.col.belowMask = data >> 4 & 3;
    window.io.col.aboveMask = data >> 6 & 3;
    return;
  }

  //CGADDSUB
  case 0x2131: {
    screen.io.bg1.colorEnable  = data >> 0 & 1;
    screen.io.bg2.colorEnable  = data >> 1 & 1;
    screen.io.bg3.colorEnable  = data >> 2 & 1;
    screen.io.bg4.colorEnable  = data >> 3 & 1;
    screen.io.obj.colorEnable  = data >> 4 & 1;
    screen.io.back.colorEnable = data >> 5 & 1;
    screen.io.colorHalve       = data >> 6 & 1;
    screen.io.colorMode        = data >> 7 & 1;
    return;
  }

  //COLDATA
  case 0x2132: {
    if(data & 0x20) screen.io.colorRed   = data & 0x1f;
    if(data & 0x40) screen.io.colorGreen = data & 0x1f;
    if(data & 0x80) screen.io.colorBlue  = data & 0x1f;
    return;
  }

  //SETINI
  case 0x2133: {
    io.interlace     = data >> 0 & 1;
    obj.io.interlace = data >> 1 & 1;
    io.overscan      = data >> 2 & 1;
    io.pseudoHires   = data >> 3 & 1;
    io.extbg         = data >> 6 & 1;
    updateVideoMode();
    return;
  }

  }
}

void PPU::updateVideoMode() {
  display.vdisp = !io.overscan ? 225 : 240;

  switch(io.bgMode) {
  case 0:
    bg1.io.mode = Background::Mode::BPP2;
    bg2.io.mode = Background::Mode::BPP2;
    bg3.io.mode = Background::Mode::BPP2;
    bg4.io.mode = Background::Mode::BPP2;
    bg1.io.priority[0] = 8; bg1.io.priority[1] = 11;
    bg2.io.priority[0] = 7; bg2.io.priority[1] = 10;
    bg3.io.priority[0] = 2; bg3.io.priority[1] = 5;
    bg4.io.priority[0] = 1; bg4.io.priority[1] = 4;
    obj.io.priority[0] = 3; obj.io.priority[1] = 6;
    obj.io.priority[2] = 9; obj.io.priority[3] = 12;
    break;

  case 1:
    bg1.io.mode = Background::Mode::BPP4;
    bg2.io.mode = Background::Mode::BPP4;
    bg3.io.mode = Background::Mode::BPP2;
    bg4.io.mode = Background::Mode::Inactive;
    if(io.bgPriority) {
      bg1.io.priority[0] = 5; bg1.io.priority[1] = 8;
      bg2.io.priority[0] = 4; bg2.io.priority[1] = 7;
      bg3.io.priority[0] = 1; bg3.io.priority[1] = 10;
      obj.io.priority[0] = 2; obj.io.priority[1] = 3;
      obj.io.priority[2] = 6; obj.io.priority[3] = 9;
    } else {
      bg1.io.priority[0] = 6; bg1.io.priority[1] = 9;
      bg2.io.priority[0] = 5; bg2.io.priority[1] = 8;
      bg3.io.priority[0] = 1; bg3.io.priority[1] = 3;
      obj.io.priority[0] = 2; obj.io.priority[1] = 4;
      obj.io.priority[2] = 7; obj.io.priority[3] = 10;
    }
    break;

  case 2:
    bg1.io.mode = Background::Mode::BPP4;
    bg2.io.mode = Background::Mode::BPP4;
    bg3.io.mode = Background::Mode::Inactive;
    bg4.io.mode = Background::Mode::Inactive;
    bg1.io.priority[0] = 3; bg1.io.priority[1] = 7;
    bg2.io.priority[0] = 1; bg2.io.priority[1] = 5;
    obj.io.priority[0] = 2; obj.io.priority[1] = 4;
    obj.io.priority[2] = 6; obj.io.priority[3] = 8;
    break;

  case 3:
    bg1.io.mode = Background::Mode::BPP8;
    bg2.io.mode = Background::Mode::BPP4;
    bg3.io.mode = Background::Mode::Inactive;
    bg4.io.mode = Background::Mode::Inactive;
    bg1.io.priority[0] = 3; bg1.io.priority[1] = 7;
    bg2.io.priority[0] = 1; bg2.io.priority[1] = 5;
    obj.io.priority[0] = 2; obj.io.priority[1] = 4;
    obj.io.priority[2] = 6; obj.io.priority[3] = 8;
    break;

  case 4:
    bg1.io.mode = Background::Mode::BPP8;
    bg2.io.mode = Background::Mode::BPP2;
    bg3.io.mode = Background::Mode::Inactive;
    bg4.io.mode = Background::Mode::Inactive;
    bg1.io.priority[0] = 3; bg1.io.priority[1] = 7;
    bg2.io.priority[0] = 1; bg2.io.priority[1] = 5;
    obj.io.priority[0] = 2; obj.io.priority[1] = 4;
    obj.io.priority[2] = 6; obj.io.priority[3] = 8;
    break;

  case 5:
    bg1.io.mode = Background::Mode::BPP4;
    bg2.io.mode = Background::Mode::BPP2;
    bg3.io.mode = Background::Mode::Inactive;
    bg4.io.mode = Background::Mode::Inactive;
    bg1.io.priority[0] = 3; bg1.io.priority[1] = 7;
    bg2.io.priority[0] = 1; bg2.io.priority[1] = 5;
    obj.io.priority[0] = 2; obj.io.priority[1] = 4;
    obj.io.priority[2] = 6; obj.io.priority[3] = 8;
    break;

  case 6:
    bg1.io.mode = Background::Mode::BPP4;
    bg2.io.mode = Background::Mode::Inactive;
    bg3.io.mode = Background::Mode::Inactive;
    bg4.io.mode = Background::Mode::Inactive;
    bg1.io.priority[0] = 2; bg1.io.priority[1] = 5;
    obj.io.priority[0] = 1; obj.io.priority[1] = 3;
    obj.io.priority[2] = 4; obj.io.priority[3] = 6;
    break;

  case 7:
    if(!io.extbg) {
      bg1.io.mode = Background::Mode::Mode7;
      bg2.io.mode = Background::Mode::Inactive;
      bg3.io.mode = Background::Mode::Inactive;
      bg4.io.mode = Background::Mode::Inactive;
      bg1.io.priority[0] = 2;
      obj.io.priority[0] = 1; obj.io.priority[1] = 3;
      obj.io.priority[2] = 4; obj.io.priority[3] = 5;
    } else {
      bg1.io.mode = Background::Mode::Mode7;
      bg2.io.mode = Background::Mode::Mode7;
      bg3.io.mode = Background::Mode::Inactive;
      bg4.io.mode = Background::Mode::Inactive;
      bg1.io.priority[0] = 3;
      bg2.io.priority[0] = 1; bg2.io.priority[1] = 5;
      obj.io.priority[0] = 2; obj.io.priority[1] = 4;
      obj.io.priority[2] = 6; obj.io.priority[3] = 7;
    }
    break;
  }
}

bool PPU::Mosaic::enable() const {
  if(ppu.bg1.mosaic.enable) return true;
  if(ppu.bg2.mosaic.enable) return true;
  if(ppu.bg3.mosaic.enable) return true;
  if(ppu.bg4.mosaic.enable) return true;
  return false;
}

unsigned PPU::Mosaic::voffset() const {
  return size - vcounter;
}

//H = 0
void PPU::Mosaic::scanline() {
  if(ppu.vcounter() == 1) {
    vcounter = enable() ? ((size + 1) & 0x1f) : 0;
  }
  if(vcounter && !--vcounter) {
    vcounter = enable() ? size + 0 : 0;
  }
}

void PPU::Mosaic::power() {
  size = (random() & 15) + 1;
  vcounter = 0;
}

int PPU::Background::clip(int n) {
  //13-bit sign extend: --s---nnnnnnnnnn -> ssssssnnnnnnnnnn
  return n & 0x2000 ? (n | ~1023) : (n & 1023);
}

void PPU::Background::runMode7() {
  int a = (int16_t)ppu.io.m7a;
  int b = (int16_t)ppu.io.m7b;
  int c = (int16_t)ppu.io.m7c;
  int d = (int16_t)ppu.io.m7d;

  /* Note: These 4 variables were originally cast to nall::Integer<13> before
     assignment to ints.
  */
  int hcenter = signextend<int16_t,13>(ppu.io.m7x);
  int vcenter = signextend<int16_t,13>(ppu.io.m7y);
  int hoffset = signextend<int16_t,13>(ppu.io.hoffsetMode7);
  int voffset = signextend<int16_t,13>(ppu.io.voffsetMode7);

  unsigned x = mosaic.hoffset;
  unsigned y = ppu.vcounter();
  if(ppu.bg1.mosaic.enable) y -= ppu.mosaic.voffset();  //BG2 vertical mosaic uses BG1 mosaic enable

  if(!mosaic.enable) {
    mosaic.hoffset += 1;
  } else if(--mosaic.hcounter == 0) {
    mosaic.hcounter = ppu.mosaic.size;
    mosaic.hoffset += ppu.mosaic.size;
  }

  if(ppu.io.hflipMode7) x = 255 - x;
  if(ppu.io.vflipMode7) y = 255 - y;

  int originX = (a * clip(hoffset - hcenter) & ~63) + (b * clip(voffset - vcenter) & ~63) + (b * y & ~63) + (hcenter << 8);
  int originY = (c * clip(hoffset - hcenter) & ~63) + (d * clip(voffset - vcenter) & ~63) + (d * y & ~63) + (vcenter << 8);

  int pixelX = (originX + a * x) >> 8;
  int pixelY = (originY + c * x) >> 8;
  uint16_t paletteAddress = ((pixelY & 0x07) << 3) | (pixelX & 0x07);

  uint8_t tileX = (pixelX >> 3) & 0x7f;
  uint8_t tileY = (pixelY >> 3) & 0x7f;
  uint16_t tileAddress = tileY << 7 | tileX;

  bool outOfBounds = (pixelX | pixelY) & ~1023;

  uint8_t tile = ppu.io.repeatMode7 == 3 && outOfBounds ? 0 : ppu.vram[tileAddress] >> 0;
  uint8_t palette = ppu.io.repeatMode7 == 2 && outOfBounds ? 0 : ppu.vram[tile << 6 | paletteAddress] >> 8;

  unsigned priority = 0;
  if(id == ID::BG1) {
    priority = io.priority[0];
  } else if(id == ID::BG2) {
    priority = io.priority[palette >> 7];
    palette &= 0x7f;
  }

  if(palette == 0) return;

  if(io.aboveEnable) {
    output.above.priority = priority;
    output.above.palette = palette;
    output.above.paletteGroup = 0;
  }

  if(io.belowEnable) {
    output.below.priority = priority;
    output.below.palette = palette;
    output.below.paletteGroup = 0;
  }
}

inline bool PPU::Background::hires() const {
  return ppu.io.bgMode == 5 || ppu.io.bgMode == 6;
}

//V = 0, H = 0
inline void PPU::Background::frame() {
}

//H = 0
inline void PPU::Background::scanline() {
  mosaic.hcounter = ppu.mosaic.size;
  mosaic.hoffset = 0;

  renderingIndex = 0;
  pixelCounter = ((io.hoffset & 7) << hires()) & 7;

  opt.hoffset = 0;
  opt.voffset = 0;
}

//H = 56
void PPU::Background::begin() {
  //remove partial tile columns that have been scrolled offscreen
  for(auto& data : tiles[0].data) data >>= pixelCounter << 1;
}

void PPU::Background::fetchNameTable() {
  if(ppu.vcounter() == 0) return;

  unsigned nameTableIndex = ppu.hcounter() >> 5 << hires();
  int x = (ppu.hcounter() & ~31) >> 2;

  unsigned hpixel = x << hires();
  unsigned vpixel = ppu.vcounter();
  unsigned hscroll = io.hoffset;
  unsigned vscroll = io.voffset;

  if(hires()) {
    hscroll <<= 1;
    if(ppu.io.interlace) vpixel = vpixel << 1 | (ppu.field() && !mosaic.enable);
  }
  if(mosaic.enable) {
    vpixel -= ppu.mosaic.voffset() << (hires() && ppu.io.interlace);
  }

  bool repeated = false;
  repeat:

  unsigned hoffset = hpixel + hscroll;
  unsigned voffset = vpixel + vscroll;

  if(ppu.io.bgMode == 2 || ppu.io.bgMode == 4 || ppu.io.bgMode == 6) {
    auto hlookup = ppu.bg3.opt.hoffset;
    auto vlookup = ppu.bg3.opt.voffset;
    unsigned valid = 1 << (13 + id);

    if(ppu.io.bgMode == 4) {
      if(hlookup & valid) {
        if(!(hlookup & 0x8000)) {
          hoffset = hpixel + (hlookup & ~7) + (hscroll & 7);
        } else {
          voffset = vpixel + (vlookup);
        }
      }
    } else {
      if(hlookup & valid) hoffset = hpixel + (hlookup & ~7) + (hscroll & 7);
      if(vlookup & valid) voffset = vpixel + (vlookup);
    }
  }

  unsigned width = 256 << hires();
  unsigned hsize = width << io.tileSize << io.screenSize.bit(0);
  unsigned vsize = width << io.tileSize << io.screenSize.bit(1);

  hoffset &= hsize - 1;
  voffset &= vsize - 1;

  unsigned vtiles = 3 + io.tileSize;
  unsigned htiles = !hires() ? vtiles : 4;

  unsigned htile = hoffset >> htiles;
  unsigned vtile = voffset >> vtiles;

  unsigned hscreen = io.screenSize.bit(0) ? 32 << 5 : 0;
  unsigned vscreen = io.screenSize.bit(1) ? 32 << (5 + io.screenSize.bit(0)) : 0;

  uint16_t offset = (htile & 0x1f) << 0 | (vtile & 0x1f) << 5;
  if(htile & 0x20) offset += hscreen;
  if(vtile & 0x20) offset += vscreen;

  uint16_t address = io.screenAddress + offset;
  uint16_t attributes = ppu.vram[address];

  auto& tile = tiles[nameTableIndex];
  tile.character = attributes & 0x03ff;
  tile.paletteGroup = attributes >> 10 & 7;
  tile.priority = io.priority[attributes >> 13 & 1];
  tile.hmirror = attributes & 0x4000 ? 1 : 0;
  tile.vmirror = attributes & 0x8000 ? 1 : 0;

  if(htiles == 4 && bool(hoffset & 8) != tile.hmirror) tile.character +=  1;
  if(vtiles == 4 && bool(voffset & 8) != tile.vmirror) tile.character += 16;

  unsigned characterMask = ppu.vram.mask >> (3 + io.mode);
  unsigned characterIndex = io.tiledataAddress >> (3 + io.mode);
  uint16_t origin = tile.character + characterIndex & characterMask;

  if(tile.vmirror) voffset ^= 7;
  tile.address = (origin << (3 + io.mode)) + (voffset & 7);

  unsigned paletteOffset = ppu.io.bgMode == 0 ? id << 5 : 0;
  unsigned paletteSize = 2 << io.mode;
  tile.palette = paletteOffset + (tile.paletteGroup << paletteSize);

  nameTableIndex++;
  if(hires() && !repeated) {
    repeated = true;
    hpixel += 8;
    goto repeat;
  }
}

void PPU::Background::fetchOffset(unsigned y) {
  if(ppu.vcounter() == 0) return;

  unsigned characterIndex = ppu.hcounter() >> 5 << hires();
  unsigned x = characterIndex << 3;

  unsigned hoffset = x + (io.hoffset & ~7);
  unsigned voffset = y + (io.voffset);

  unsigned vtiles = 3 + io.tileSize;
  unsigned htiles = !hires() ? vtiles : 4;

  unsigned htile = hoffset >> htiles;
  unsigned vtile = voffset >> vtiles;

  unsigned hscreen = io.screenSize.bit(0) ? 32 << 5 : 0;
  unsigned vscreen = io.screenSize.bit(1) ? 32 << (5 + io.screenSize.bit(0)) : 0;

  uint16_t offset = (htile & 0x1f) << 0 | (vtile & 0x1f) << 5;
  if(htile & 0x20) offset += hscreen;
  if(vtile & 0x20) offset += vscreen;

  uint16_t address = io.screenAddress + offset;
  if(y == 0) opt.hoffset = ppu.vram[address];
  if(y == 8) opt.voffset = ppu.vram[address];
}

void PPU::Background::fetchCharacter(unsigned index, bool half) {
  if(ppu.vcounter() == 0) return;

  unsigned characterIndex = (ppu.hcounter() >> 5 << hires()) + half;

  auto& tile = tiles[characterIndex];
  uint16_t data = ppu.vram[tile.address + (index << 3)];

  //reverse bits so that the lowest bit is the left-most pixel
  if(!tile.hmirror) {
    data = (data >> 4 & 0x0f0f) | (data << 4 & 0xf0f0);
    data = (data >> 2 & 0x3333) | (data << 2 & 0xcccc);
    data = (data >> 1 & 0x5555) | (data << 1 & 0xaaaa);
  }

  //interleave two bitplanes for faster planar decoding later
  tile.data[index] = (
    (((uint8_t(data >> 0) * 0x0101010101010101ull & 0x8040201008040201ull) * 0x0102040810204081ull >> 49) & 0x5555)
  | (((uint8_t(data >> 8) * 0x0101010101010101ull & 0x8040201008040201ull) * 0x0102040810204081ull >> 48) & 0xaaaa)
  );
}

inline void PPU::Background::run(bool screen) {
  if(ppu.vcounter() == 0) return;

  if(screen == Screen::Below) {
    output.above.priority = 0;
    output.below.priority = 0;
    if(!hires()) return;
  }

  if(io.mode == Mode::Mode7) return runMode7();

  auto& tile = tiles[renderingIndex];
  uint8_t color = 0;

  if(io.mode >= Mode::BPP2)
    color |= (tile.data[0] & 3) << 0;

  tile.data[0] >>= 2;

  if(io.mode >= Mode::BPP4)
    color |= (tile.data[1] & 3) << 2;

  tile.data[1] >>= 2;

  if(io.mode >= Mode::BPP8)
    color |= (tile.data[2] & 3) << 4;

  tile.data[2] >>= 2;

  if(io.mode >= Mode::BPP8)
    color |= (tile.data[3] & 3) << 6;

  tile.data[3] >>= 2;

  Pixel pixel;
  pixel.priority = tile.priority;
  pixel.palette = color ? unsigned(tile.palette + color) : 0;
  pixel.paletteGroup = tile.paletteGroup;
  pixelCounter = (pixelCounter + 1) & 7;
  if(!pixelCounter) renderingIndex = (renderingIndex + 1) & 0x7f;

  unsigned x = (ppu.hcounter() - 56) >> 2;

  if(x == 0 && (!hires() || screen == Screen::Below)) {
    mosaic.hcounter = ppu.mosaic.size;
    mosaic.pixel = pixel;
  } else if((!hires() || screen == Screen::Below) && --mosaic.hcounter == 0) {
    mosaic.hcounter = ppu.mosaic.size;
    mosaic.pixel = pixel;
  } else if(mosaic.enable) {
    pixel = mosaic.pixel;
  }
  if(screen == Screen::Above) ++x;
  if(pixel.palette == 0) return;

  if(!hires() || screen == Screen::Above) if(io.aboveEnable) output.above = pixel;
  if(!hires() || screen == Screen::Below) if(io.belowEnable) output.below = pixel;
}

void PPU::Background::power() {
  io = {};
  io.tiledataAddress = (random() & 0x0f) << 12;
  io.screenAddress = (random() & 0xfc) << 8;
  io.screenSize = random();
  io.tileSize = random() & 1;
  io.aboveEnable = random() & 1;
  io.belowEnable = random() & 1;
  io.hoffset = random();
  io.voffset = random();

  output.above = {};
  output.below = {};

  mosaic = {};
  mosaic.enable = random() & 1;
}

uint8_t PPU::OAM::read(nall::Natural<10> address) {
  if(!(address & 0x200)) {
    unsigned n = address >> 2;  //object#
    address &= 3;
    if(address == 0) return object[n].x & 0xff;
    if(address == 1) return object[n].y;
    if(address == 2) return object[n].character;
    return (
      object[n].nameselect << 0
    | object[n].palette    << 1
    | object[n].priority   << 4
    | object[n].hflip      << 6
    | object[n].vflip      << 7
    );
  } else {
    unsigned n = (address & 0x1f) << 2;  //object#
    return (
      object[n + 0].x >> 8 << 0
    | object[n + 0].size   << 1
    | object[n + 1].x >> 8 << 2
    | object[n + 1].size   << 3
    | object[n + 2].x >> 8 << 4
    | object[n + 2].size   << 5
    | object[n + 3].x >> 8 << 6
    | object[n + 3].size   << 7
    );
  }
}

void PPU::OAM::write(nall::Natural<10> address, uint8_t data) {
  if(!(address & 0x200)) {
    unsigned n = address >> 2;  //object#
    address &= 3;
    if(address == 0) { object[n].x = (object[n].x & 0x100) | data; return; }
    if(address == 1) { object[n].y = data; return; }
    if(address == 2) { object[n].character = data; return; }
    object[n].nameselect = data >> 0 & 1;
    object[n].palette    = data >> 1 & 7;
    object[n].priority   = data >> 4 & 3;
    object[n].hflip      = data >> 6 & 1;
    object[n].vflip      = data >> 7 & 1;
  } else {
    unsigned n = (address & 0x1f) << 2;  //object#
    object[n + 0].x    = (object[n + 0].x & 0xff) | bool(data & 0x01) << 8;
    object[n + 0].size = data & 0x02 ? 1 : 0;
    object[n + 1].x    = (object[n + 1].x & 0xff) | bool(data & 0x04) << 8;
    object[n + 1].size = data & 0x08 ? 1 : 0;
    object[n + 2].x    = (object[n + 2].x & 0xff) | bool(data & 0x10) << 8;
    object[n + 2].size = data & 0x20 ? 1 : 0;
    object[n + 3].x    = (object[n + 3].x & 0xff) | bool(data & 0x40) << 8;
    object[n + 3].size = data & 0x80 ? 1 : 0;
  }
}

unsigned PPU::OAM::Object::width() const {
  if(size == 0) {
    static const unsigned width[] = { 8,  8,  8, 16, 16, 32, 16, 16};
    return width[ppu.obj.io.baseSize];
  } else {
    static const unsigned width[] = {16, 32, 64, 32, 64, 64, 32, 32};
    return width[ppu.obj.io.baseSize];
  }
}

unsigned PPU::OAM::Object::height() const {
  if(size == 0) {
    if(ppu.obj.io.interlace && ppu.obj.io.baseSize >= 6) return 16;  //hardware quirk
    static const unsigned height[] = { 8,  8,  8, 16, 16, 32, 32, 32};
    return height[ppu.obj.io.baseSize];
  } else {
    static const unsigned height[] = {16, 32, 64, 32, 64, 64, 64, 32};
    return height[ppu.obj.io.baseSize];
  }
}

void PPU::Object::addressReset() {
  ppu.io.oamAddress = ppu.io.oamBaseAddress;
  setFirstSprite();
}

void PPU::Object::setFirstSprite() {
  io.firstSprite = !ppu.io.oamPriority ? 0 : (ppu.io.oamAddress >> 2) & 0x7f;
}

void PPU::Object::frame() {
  io.timeOver = 0;
  io.rangeOver = 0;
}

void PPU::Object::scanline() {
  latch.firstSprite = io.firstSprite;

  t.x = 0;
  t.y = ppu.vcounter();
  t.itemCount = 0;
  t.tileCount = 0;

  t.active = !t.active;
  auto oamItem = t.item[t.active];
  auto oamTile = t.tile[t.active];

  for(unsigned n = 0; n < 32; ++n) oamItem[n].valid = 0;
  for(unsigned n = 0; n < 34; ++n) oamTile[n].valid = 0;

  if(t.y == ppu.vdisp() && !ppu.io.displayDisable) addressReset();
  if(t.y >= ppu.vdisp() - 1 || ppu.io.displayDisable) return;
}

void PPU::Object::evaluate(uint8_t index) {
  if(ppu.io.displayDisable) return;
  if(t.itemCount > 32) return;

  auto oamItem = t.item[t.active];

  uint8_t sprite = (latch.firstSprite + index) & 0x7f;
  if(!onScanline(oam.object[sprite])) return;
  ppu.latch.oamAddress = sprite;

  if(t.itemCount++ < 32) {
    oamItem[t.itemCount - 1] = {true, sprite};
  }
}

bool PPU::Object::onScanline(PPU::OAM::Object& sprite) {
  if(sprite.x > 256 && sprite.x + sprite.width() - 1 < 512) return false;
  unsigned height = sprite.height() >> io.interlace;
  if(t.y >= sprite.y && t.y < sprite.y + height) return true;
  if(sprite.y + height >= 256 && t.y < (sprite.y + height & 255)) return true;
  return false;
}

void PPU::Object::run() {
  output.above.priority = 0;
  output.below.priority = 0;

  auto oamTile = t.tile[!t.active];
  unsigned x = t.x++;

  for(unsigned n = 0; n < 34; ++n) {
    const auto& tile = oamTile[n];
    if(!tile.valid) break;

    int px = x - signextend<int16_t,9>(tile.x);
    if(px & ~7) continue;

    unsigned color = 0, shift = tile.hflip ? px : 7 - px;
    color += tile.data >> (shift +  0) & 1;
    color += tile.data >> (shift +  7) & 2;
    color += tile.data >> (shift + 14) & 4;
    color += tile.data >> (shift + 21) & 8;

    if(color) {
      if(io.aboveEnable) {
        output.above.palette = tile.palette + color;
        output.above.priority = io.priority[tile.priority];
      }

      if(io.belowEnable) {
        output.below.palette = tile.palette + color;
        output.below.priority = io.priority[tile.priority];
      }
    }
  }
}

void PPU::Object::fetch() {
  auto oamItem = t.item[t.active];
  auto oamTile = t.tile[t.active];

  for(unsigned i = 32; i--;) {
    if(!oamItem[i].valid) continue;

    if(ppu.io.displayDisable || ppu.vcounter() >= ppu.vdisp() - 1) {
      ppu.step(8);
      continue;
    }

    ppu.latch.oamAddress = 0x0200 + (oamItem[i].index >> 2);
    const auto& sprite = oam.object[oamItem[i].index];

    unsigned tileWidth = sprite.width() >> 3;
    int x = sprite.x;
    int y = t.y - sprite.y & 0xff;
    if(io.interlace) y <<= 1;

    if(sprite.vflip) {
      if(sprite.width() == sprite.height()) {
        y = sprite.height() - 1 - y;
      } else if(y < (int)sprite.width()) {
        y = sprite.width() - 1 - y;
      } else {
        y = sprite.width() + (sprite.width() - 1) - (y - sprite.width());
      }
    }

    if(io.interlace) {
      y = !sprite.vflip ? y + ppu.field() : y - ppu.field();
    }

    x &= 511;
    y &= 255;

    uint16_t tiledataAddress = io.tiledataAddress;
    if(sprite.nameselect) tiledataAddress += (1 + io.nameselect) << 12;
    uint16_t chrx =  (sprite.character & 15);
    uint16_t chry = ((sprite.character >> 4) + (y >> 3) & 15) << 4;

    for(unsigned tx = 0; tx < tileWidth; ++tx) {
      unsigned sx = x + (tx << 3) & 511;
      if(x != 256 && sx >= 256 && sx + 7 < 512) continue;
      if(t.tileCount++ >= 34) break;

      unsigned n = t.tileCount - 1;
      oamTile[n].valid = 1;
      oamTile[n].x = sx;
      oamTile[n].priority = sprite.priority;
      oamTile[n].palette = 128 + (sprite.palette << 4);
      oamTile[n].hflip = sprite.hflip;

      unsigned mx = !sprite.hflip ? tx : tileWidth - 1 - tx;
      unsigned pos = tiledataAddress + ((chry + (chrx + mx & 15)) << 4);
      uint16_t address = (pos & 0xfff0) + (y & 7);

      if(!ppu.io.displayDisable)
      oamTile[n].data  = ppu.vram[address + 0] <<  0;
      ppu.step(4);

      if(!ppu.io.displayDisable)
      oamTile[n].data |= ppu.vram[address + 8] << 16;
      ppu.step(4);
    }
  }

  io.timeOver  |= (t.tileCount > 34) & 1;
  io.rangeOver |= (t.itemCount > 32) & 1;
}

void PPU::Object::power() {
  for(auto& object : oam.object) {
    object.x = 0;
    object.y = 0;
    object.character = 0;
    object.nameselect = 0;
    object.vflip = 0;
    object.hflip = 0;
    object.priority = 0;
    object.palette = 0;
    object.size = 0;
  }

  t.x = 0;
  t.y = 0;

  t.itemCount = 0;
  t.tileCount = 0;

  t.active = 0;
  for(unsigned p = 0; p < 2; ++p) {
    for(unsigned n = 0; n < 32; ++n) {
      t.item[p][n].valid = 0;
      t.item[p][n].index = 0;
    }
    for(unsigned n = 0; n < 34; ++n) {
      t.tile[p][n].valid = 0;
      t.tile[p][n].x = 0;
      t.tile[p][n].priority = 0;
      t.tile[p][n].palette = 0;
      t.tile[p][n].hflip = 0;
      t.tile[p][n].data = 0;
    }
  }

  io.aboveEnable = random() & 1;
  io.belowEnable = random() & 1;
  io.interlace = random() & 1;

  io.baseSize = random() & 7;
  io.nameselect = random();
  io.tiledataAddress = (random() & 7) << 13;
  io.firstSprite = 0;

  for(auto& p : io.priority) p = 0;

  io.timeOver = 0;
  io.rangeOver = 0;

  latch = {};

  output.above.palette = 0;
  output.above.priority = 0;
  output.below.palette = 0;
  output.below.priority = 0;
}

void PPU::Window::scanline() {
  x = 0;
}

void PPU::Window::run() {
  bool one = (x >= io.oneLeft && x <= io.oneRight);
  bool two = (x >= io.twoLeft && x <= io.twoRight);
  x++;

  if(test(io.bg1.oneEnable, one ^ io.bg1.oneInvert, io.bg1.twoEnable, two ^ io.bg1.twoInvert, io.bg1.mask)) {
    if(io.bg1.aboveEnable) ppu.bg1.output.above.priority = 0;
    if(io.bg1.belowEnable) ppu.bg1.output.below.priority = 0;
  }

  if(test(io.bg2.oneEnable, one ^ io.bg2.oneInvert, io.bg2.twoEnable, two ^ io.bg2.twoInvert, io.bg2.mask)) {
    if(io.bg2.aboveEnable) ppu.bg2.output.above.priority = 0;
    if(io.bg2.belowEnable) ppu.bg2.output.below.priority = 0;
  }

  if(test(io.bg3.oneEnable, one ^ io.bg3.oneInvert, io.bg3.twoEnable, two ^ io.bg3.twoInvert, io.bg3.mask)) {
    if(io.bg3.aboveEnable) ppu.bg3.output.above.priority = 0;
    if(io.bg3.belowEnable) ppu.bg3.output.below.priority = 0;
  }

  if(test(io.bg4.oneEnable, one ^ io.bg4.oneInvert, io.bg4.twoEnable, two ^ io.bg4.twoInvert, io.bg4.mask)) {
    if(io.bg4.aboveEnable) ppu.bg4.output.above.priority = 0;
    if(io.bg4.belowEnable) ppu.bg4.output.below.priority = 0;
  }

  if(test(io.obj.oneEnable, one ^ io.obj.oneInvert, io.obj.twoEnable, two ^ io.obj.twoInvert, io.obj.mask)) {
    if(io.obj.aboveEnable) ppu.obj.output.above.priority = 0;
    if(io.obj.belowEnable) ppu.obj.output.below.priority = 0;
  }

  bool value = test(io.col.oneEnable, one ^ io.col.oneInvert, io.col.twoEnable, two ^ io.col.twoInvert, io.col.mask);
  bool array[] = {true, value, !value, false};
  output.above.colorEnable = array[io.col.aboveMask];
  output.below.colorEnable = array[io.col.belowMask];
}

bool PPU::Window::test(bool oneEnable, bool one, bool twoEnable, bool two, unsigned mask) {
  if(!oneEnable) return two && twoEnable;
  if(!twoEnable) return one;
  if(mask == 0) return (one | two);
  if(mask == 1) return (one & two);
  return (one ^ two) == 3 - mask;
}

void PPU::Window::power() {
  io.bg1.oneEnable = random();
  io.bg1.oneInvert = random();
  io.bg1.twoEnable = random();
  io.bg1.twoInvert = random();
  io.bg1.mask = random();
  io.bg1.aboveEnable = random() & 1;
  io.bg1.belowEnable = random() & 1;

  io.bg2.oneEnable = random();
  io.bg2.oneInvert = random();
  io.bg2.twoEnable = random();
  io.bg2.twoInvert = random();
  io.bg2.mask = random();
  io.bg2.aboveEnable = random() & 1;
  io.bg2.belowEnable = random() & 1;

  io.bg3.oneEnable = random();
  io.bg3.oneInvert = random();
  io.bg3.twoEnable = random();
  io.bg3.twoInvert = random();
  io.bg3.mask = random();
  io.bg3.aboveEnable = random() & 1;
  io.bg3.belowEnable = random() & 1;

  io.bg4.oneEnable = random();
  io.bg4.oneInvert = random();
  io.bg4.twoEnable = random();
  io.bg4.twoInvert = random();
  io.bg4.mask = random();
  io.bg4.aboveEnable = random() & 1;
  io.bg4.belowEnable = random() & 1;

  io.obj.oneEnable = random();
  io.obj.oneInvert = random();
  io.obj.twoEnable = random();
  io.obj.twoInvert = random();
  io.obj.mask = random();
  io.obj.aboveEnable = random() & 1;
  io.obj.belowEnable = random() & 1;

  io.col.oneEnable = random();
  io.col.oneInvert = random();
  io.col.twoEnable = random();
  io.col.twoInvert = random();
  io.col.mask = random();
  io.col.aboveMask = random();
  io.col.belowMask = random();

  io.oneLeft = random();
  io.oneRight = random();
  io.twoLeft = random();
  io.twoRight = random();

  output.above.colorEnable = 0;
  output.below.colorEnable = 0;

  x = 0;
}

void PPU::Screen::scanline() {
  auto y = ppu.vcounter() + (!ppu.display.overscan ? 7 : 0);

  lineA = ppu.output + y * (ppu.display.interlace ? 1024 : 512);
  lineB = lineA + (ppu.display.interlace ? 0 : 512);
  if(ppu.display.interlace && ppu.field()) lineA += 512, lineB += 512;

  //the first hires pixel of each scanline is transparent
  //note: exact value initializations are not confirmed on hardware
  math.above.color = paletteColor(0);
  math.below.color = math.above.color;

  math.above.colorEnable = 0;
  math.below.colorEnable = 0;

  math.transparent = 1;
  math.blendMode   = 0;
  math.colorHalve  = io.colorHalve && !io.blendMode && math.above.colorEnable;
}

void PPU::Screen::run() {
  if(ppu.vcounter() == 0) return;

  bool hires      = ppu.io.pseudoHires || ppu.io.bgMode == 5 || ppu.io.bgMode == 6;
  auto belowColor = below(hires);
  auto aboveColor = above();

  *lineA++ = *lineB++ = ppu.lightTable[ppu.io.displayBrightness][hires ? belowColor : aboveColor];
  *lineA++ = *lineB++ = ppu.lightTable[ppu.io.displayBrightness][aboveColor];
}

uint16_t PPU::Screen::below(bool hires) {
  if(ppu.io.displayDisable || (!ppu.io.overscan && ppu.vcounter() >= 225)) return 0;

  unsigned priority = 0;
  if(ppu.bg1.output.below.priority) {
    priority = ppu.bg1.output.below.priority;
    if(io.directColor && (ppu.io.bgMode == 3 || ppu.io.bgMode == 4 || ppu.io.bgMode == 7)) {
      math.below.color = directColor(ppu.bg1.output.below.palette, ppu.bg1.output.below.paletteGroup);
    } else {
      math.below.color = paletteColor(ppu.bg1.output.below.palette);
    }
  }
  if(ppu.bg2.output.below.priority > priority) {
    priority = ppu.bg2.output.below.priority;
    math.below.color = paletteColor(ppu.bg2.output.below.palette);
  }
  if(ppu.bg3.output.below.priority > priority) {
    priority = ppu.bg3.output.below.priority;
    math.below.color = paletteColor(ppu.bg3.output.below.palette);
  }
  if(ppu.bg4.output.below.priority > priority) {
    priority = ppu.bg4.output.below.priority;
    math.below.color = paletteColor(ppu.bg4.output.below.palette);
  }
  if(ppu.obj.output.below.priority > priority) {
    priority = ppu.obj.output.below.priority;
    math.below.color = paletteColor(ppu.obj.output.below.palette);
  }
  if((math.transparent = (priority == 0))) math.below.color = paletteColor(0);

  if(!hires) return 0;
  if(!math.below.colorEnable) return math.above.colorEnable ? math.below.color : (uint16_t)0;

  return blend(
    math.above.colorEnable ? math.below.color : (uint16_t)0,
    math.blendMode ? math.above.color : fixedColor()
  );
}

uint16_t PPU::Screen::above() {
  if(ppu.io.displayDisable || (!ppu.io.overscan && ppu.vcounter() >= 225)) return 0;

  unsigned priority = 0;
  if(ppu.bg1.output.above.priority) {
    priority = ppu.bg1.output.above.priority;
    if(io.directColor && (ppu.io.bgMode == 3 || ppu.io.bgMode == 4 || ppu.io.bgMode == 7)) {
      math.above.color = directColor(ppu.bg1.output.above.palette, ppu.bg1.output.above.paletteGroup);
    } else {
      math.above.color = paletteColor(ppu.bg1.output.above.palette);
    }
    math.below.colorEnable = io.bg1.colorEnable;
  }
  if(ppu.bg2.output.above.priority > priority) {
    priority = ppu.bg2.output.above.priority;
    math.above.color = paletteColor(ppu.bg2.output.above.palette);
    math.below.colorEnable = io.bg2.colorEnable;
  }
  if(ppu.bg3.output.above.priority > priority) {
    priority = ppu.bg3.output.above.priority;
    math.above.color = paletteColor(ppu.bg3.output.above.palette);
    math.below.colorEnable = io.bg3.colorEnable;
  }
  if(ppu.bg4.output.above.priority > priority) {
    priority = ppu.bg4.output.above.priority;
    math.above.color = paletteColor(ppu.bg4.output.above.palette);
    math.below.colorEnable = io.bg4.colorEnable;
  }
  if(ppu.obj.output.above.priority > priority) {
    priority = ppu.obj.output.above.priority;
    math.above.color = paletteColor(ppu.obj.output.above.palette);
    math.below.colorEnable = io.obj.colorEnable && ppu.obj.output.above.palette >= 192;
  }
  if(priority == 0) {
    math.above.color = paletteColor(0);
    math.below.colorEnable = io.back.colorEnable;
  }

  if(!ppu.window.output.below.colorEnable) math.below.colorEnable = 0;
  math.above.colorEnable = ppu.window.output.above.colorEnable;
  if(!math.below.colorEnable) return math.above.colorEnable ? math.above.color : (uint16_t)0;

  if(io.blendMode && math.transparent) {
    math.blendMode  = 0;
    math.colorHalve = 0;
  } else {
    math.blendMode  = io.blendMode;
    math.colorHalve = io.colorHalve && math.above.colorEnable;
  }

  return blend(
    math.above.colorEnable ? math.above.color : (uint16_t)0,
    math.blendMode ? math.below.color : fixedColor()
  );
}

uint16_t PPU::Screen::blend(unsigned x, unsigned y) const {
  if(!io.colorMode) {  //add
    if(!math.colorHalve) {
      unsigned sum = x + y;
      unsigned carry = (sum - ((x ^ y) & 0x0421)) & 0x8420;
      return (sum - carry) | (carry - (carry >> 5));
    } else {
      return (x + y - ((x ^ y) & 0x0421)) >> 1;
    }
  } else {  //sub
    unsigned diff = x - y + 0x8420;
    unsigned borrow = (diff - ((x ^ y) & 0x8420)) & 0x8420;
    if(!math.colorHalve) {
      return   (diff - borrow) & (borrow - (borrow >> 5));
    } else {
      return (((diff - borrow) & (borrow - (borrow >> 5))) & 0x7bde) >> 1;
    }
  }
}

uint16_t PPU::Screen::paletteColor(uint8_t palette) const {
  ppu.latch.cgramAddress = palette;
  return cgram[palette];
}

uint16_t PPU::Screen::directColor(uint8_t palette, uint8_t paletteGroup) const {
  //palette = -------- BBGGGRRR
  //group   = -------- -----bgr
  //output  = 0BBb00GG Gg0RRRr0
  return (palette << 7 & 0x6000) + (paletteGroup << 10 & 0x1000)
       + (palette << 4 & 0x0380) + (paletteGroup <<  5 & 0x0040)
       + (palette << 2 & 0x001c) + (paletteGroup <<  1 & 0x0002);
}

uint16_t PPU::Screen::fixedColor() const {
  return io.colorBlue << 10 | io.colorGreen << 5 | io.colorRed << 0;
}

void PPU::Screen::power() {
  random.array((uint8_t*)cgram, sizeof(cgram));
  for(auto& word : cgram) word &= 0x7fff;

  io.blendMode = random() & 1;
  io.directColor = random() & 1;
  io.colorMode = random() & 1;
  io.colorHalve = random() & 1;
  io.bg1.colorEnable = random() & 1;
  io.bg2.colorEnable = random() & 1;
  io.bg3.colorEnable = random() & 1;
  io.bg4.colorEnable = random() & 1;
  io.obj.colorEnable = random() & 1;
  io.back.colorEnable = random() & 1;
  io.colorBlue = random() & 0x1f;
  io.colorGreen = random() & 0x1f;
  io.colorRed = random() & 0x1f;
}

void PPU::serialize(serializer& s) {
  s.integer(display.interlace);
  s.integer(display.overscan);
  s.integer(display.vdisp);

  Thread::serialize(s);
  PPUcounter::serialize(s);

  s.integer(vram.mask);
  s.array(vram.data, vram.mask + 1);

  s.integer(ppu1.version);
  s.integer(ppu1.mdr);

  s.integer(ppu2.version);
  s.integer(ppu2.mdr);

  s.integer(latch.vram);
  s.integer(latch.oam);
  s.integer(latch.cgram);
  s.integer(latch.bgofsPPU1);
  s.integer(latch.bgofsPPU2);
  s.integer(latch.mode7);
  s.integer(latch.counters);
  s.integer(latch.hcounter);
  s.integer(latch.vcounter);

  s.integer(latch.oamAddress);
  s.integer(latch.cgramAddress);

  s.integer(io.displayDisable);
  s.integer(io.displayBrightness);

  s.integer(io.oamBaseAddress);
  s.integer(io.oamAddress);
  s.integer(io.oamPriority);

  s.integer(io.bgMode);
  s.integer(io.bgPriority);

  s.integer(io.hoffsetMode7);
  s.integer(io.voffsetMode7);

  s.integer(io.vramIncrementMode);
  s.integer(io.vramMapping);
  s.integer(io.vramIncrementSize);

  s.integer(io.vramAddress);

  s.integer(io.repeatMode7);
  s.integer(io.vflipMode7);
  s.integer(io.hflipMode7);

  s.integer(io.m7a);
  s.integer(io.m7b);
  s.integer(io.m7c);
  s.integer(io.m7d);
  s.integer(io.m7x);
  s.integer(io.m7y);

  s.integer(io.cgramAddress);
  s.integer(io.cgramAddressLatch);

  s.integer(io.extbg);
  s.integer(io.pseudoHires);
  s.integer(io.overscan);
  s.integer(io.interlace);

  s.integer(io.hcounter);
  s.integer(io.vcounter);

  mosaic.serialize(s);
  bg1.serialize(s);
  bg2.serialize(s);
  bg3.serialize(s);
  bg4.serialize(s);
  obj.serialize(s);
  window.serialize(s);
  screen.serialize(s);
}

void PPU::Mosaic::serialize(serializer& s) {
  s.integer(size);
  s.integer(vcounter);
}

void PPU::Background::serialize(serializer& s) {
  s.integer(io.tiledataAddress);
  s.integer(io.screenAddress);
  s.integer(io.screenSize);
  s.integer(io.tileSize);
  s.integer(io.mode);
  s.array(io.priority);
  s.integer(io.aboveEnable);
  s.integer(io.belowEnable);
  s.integer(io.hoffset);
  s.integer(io.voffset);

  s.integer(output.above.priority);
  s.integer(output.above.palette);
  s.integer(output.above.paletteGroup);

  s.integer(output.below.priority);
  s.integer(output.below.palette);
  s.integer(output.below.paletteGroup);

  s.integer(mosaic.enable);
  s.integer(mosaic.hcounter);
  s.integer(mosaic.hoffset);

  s.integer(mosaic.pixel.priority);
  s.integer(mosaic.pixel.palette);
  s.integer(mosaic.pixel.paletteGroup);

  s.integer(opt.hoffset);
  s.integer(opt.voffset);

  for(auto& tile : tiles) {
    s.integer(tile.address);
    s.integer(tile.character);
    s.integer(tile.palette);
    s.integer(tile.paletteGroup);
    s.integer(tile.priority);
    s.integer(tile.hmirror);
    s.integer(tile.vmirror);
    s.array(tile.data);
  }

  s.integer(renderingIndex);
  s.integer(pixelCounter);
}

void PPU::Object::serialize(serializer& s) {
  for(auto& object : oam.object) {
    s.integer(object.x);
    s.integer(object.y);
    s.integer(object.character);
    s.integer(object.nameselect);
    s.integer(object.vflip);
    s.integer(object.hflip);
    s.integer(object.priority);
    s.integer(object.palette);
    s.integer(object.size);
  }

  s.integer(io.aboveEnable);
  s.integer(io.belowEnable);
  s.integer(io.interlace);

  s.integer(io.baseSize);
  s.integer(io.nameselect);
  s.integer(io.tiledataAddress);
  s.integer(io.firstSprite);

  s.array(io.priority);

  s.integer(io.timeOver);
  s.integer(io.rangeOver);

  s.integer(latch.firstSprite);

  s.integer(t.x);
  s.integer(t.y);

  s.integer(t.itemCount);
  s.integer(t.tileCount);

  s.integer(t.active);
  for(auto p = 0; p < 2; ++p) {
    for(auto n = 0; n < 32; ++n) {
      s.integer(t.item[p][n].valid);
      s.integer(t.item[p][n].index);
    }
    for(auto n = 0; n < 34; ++n) {
      s.integer(t.tile[p][n].valid);
      s.integer(t.tile[p][n].x);
      s.integer(t.tile[p][n].priority);
      s.integer(t.tile[p][n].palette);
      s.integer(t.tile[p][n].hflip);
      s.integer(t.tile[p][n].data);
    }
  }

  s.integer(output.above.priority);
  s.integer(output.above.palette);

  s.integer(output.below.priority);
  s.integer(output.below.palette);
}

void PPU::Window::serialize(serializer& s) {
  s.integer(io.bg1.oneEnable);
  s.integer(io.bg1.oneInvert);
  s.integer(io.bg1.twoEnable);
  s.integer(io.bg1.twoInvert);
  s.integer(io.bg1.mask);
  s.integer(io.bg1.aboveEnable);
  s.integer(io.bg1.belowEnable);

  s.integer(io.bg2.oneEnable);
  s.integer(io.bg2.oneInvert);
  s.integer(io.bg2.twoEnable);
  s.integer(io.bg2.twoInvert);
  s.integer(io.bg2.mask);
  s.integer(io.bg2.aboveEnable);
  s.integer(io.bg2.belowEnable);

  s.integer(io.bg3.oneEnable);
  s.integer(io.bg3.oneInvert);
  s.integer(io.bg3.twoEnable);
  s.integer(io.bg3.twoInvert);
  s.integer(io.bg3.mask);
  s.integer(io.bg3.aboveEnable);
  s.integer(io.bg3.belowEnable);

  s.integer(io.bg4.oneEnable);
  s.integer(io.bg4.oneInvert);
  s.integer(io.bg4.twoEnable);
  s.integer(io.bg4.twoInvert);
  s.integer(io.bg4.mask);
  s.integer(io.bg4.aboveEnable);
  s.integer(io.bg4.belowEnable);

  s.integer(io.obj.oneEnable);
  s.integer(io.obj.oneInvert);
  s.integer(io.obj.twoEnable);
  s.integer(io.obj.twoInvert);
  s.integer(io.obj.mask);
  s.integer(io.obj.aboveEnable);
  s.integer(io.obj.belowEnable);

  s.integer(io.col.oneEnable);
  s.integer(io.col.oneInvert);
  s.integer(io.col.twoEnable);
  s.integer(io.col.twoInvert);
  s.integer(io.col.mask);
  s.integer(io.col.aboveMask);
  s.integer(io.col.belowMask);

  s.integer(io.oneLeft);
  s.integer(io.oneRight);
  s.integer(io.twoLeft);
  s.integer(io.twoRight);

  s.integer(output.above.colorEnable);
  s.integer(output.below.colorEnable);

  s.integer(x);
}

void PPU::Screen::serialize(serializer& s) {
  s.array(cgram);

  s.integer(io.blendMode);
  s.integer(io.directColor);

  s.integer(io.colorMode);
  s.integer(io.colorHalve);
  s.integer(io.bg1.colorEnable);
  s.integer(io.bg2.colorEnable);
  s.integer(io.bg3.colorEnable);
  s.integer(io.bg4.colorEnable);
  s.integer(io.obj.colorEnable);
  s.integer(io.back.colorEnable);

  s.integer(io.colorBlue);
  s.integer(io.colorGreen);
  s.integer(io.colorRed);

  s.integer(math.above.color);
  s.integer(math.above.colorEnable);
  s.integer(math.below.color);
  s.integer(math.below.colorEnable);
  s.integer(math.transparent);
  s.integer(math.blendMode);
  s.integer(math.colorHalve);
}

void PPUcounter::serialize(serializer& s) {
  s.integer(time.interlace);
  s.integer(time.field);
  s.integer(time.vperiod);
  s.integer(time.hperiod);
  s.integer(time.vcounter);
  s.integer(time.hcounter);

  s.integer(last.vperiod);
  s.integer(last.hperiod);
}

PPU::PPU() :
bg1(Background::ID::BG1),
bg2(Background::ID::BG2),
bg3(Background::ID::BG3),
bg4(Background::ID::BG4) {
  ppu1.version = 1;  //allowed values: 1
  ppu2.version = 3;  //allowed values: 1, 2, 3

  for(unsigned l = 0; l < 16; l++) {
    for(unsigned r = 0; r < 32; r++) {
      for(unsigned g = 0; g < 32; g++) {
        for(unsigned b = 0; b < 32; b++) {
          double luma = (double)l / 15.0;
          unsigned ar = (luma * r + 0.5);
          unsigned ag = (luma * g + 0.5);
          unsigned ab = (luma * b + 0.5);
          lightTable[l][(r << 10) + (g << 5) + b] = (ab << 10) + (ag << 5) + ar;
        }
      }
    }
  }
}

PPU::~PPU() {
}

void PPU::synchronizeCPU() {
  if(clock >= 0) scheduler.resume(cpu.thread);
}

void PPU::step() {
  tick(2);
  clock += 2;
  synchronizeCPU();
}

void PPU::step(unsigned clocks) {
  clocks >>= 1;
  while(clocks--) {
    tick(2);
    clock += 2;
    synchronizeCPU();
  }
}

void PPU::Enter() {
  while(true) {
    scheduler.synchronize();
    ppu.main();
  }
}

bool PPU::load() {
  ppu1.version = std::max(1, std::min(1, (int)configuration.system.ppu1.version));
  ppu2.version = std::max(1, std::min(3, (int)configuration.system.ppu2.version));
  vram.mask = configuration.system.ppu1.vram.size / sizeof(uint16_t) - 1;
  if(vram.mask != 0xffff) vram.mask = 0x7fff;
  return true;
}

void PPU::power(bool reset) {
  create(Enter, system.cpuFrequency());
  PPUcounter::reset();
  std::memset(output, 0, 512 * 480 * sizeof(uint16_t));

  bfunction<uint8_t (unsigned, uint8_t)> reader{&PPU::readIO, this};
  bfunction<void  (unsigned, uint8_t)> writer{&PPU::writeIO, this};
  bus.map(reader, writer, "00-3f,80-bf:2100-213f");

  if(!reset) random.array((uint8_t*)vram.data, sizeof(vram.data));

  ppu1.mdr = random.bias(0xff);
  ppu2.mdr = random.bias(0xff);

  latch.vram = random();
  latch.oam = random();
  latch.cgram = random();
  latch.bgofsPPU1 = random();
  latch.bgofsPPU2 = random() & 7;
  latch.mode7 = random();
  latch.counters = 0;
  latch.hcounter = 0;
  latch.vcounter = 0;

  latch.oamAddress = 0x0000;
  latch.cgramAddress = 0x00;

  //$2100  INIDISP
  io.displayDisable = 1;
  io.displayBrightness = 0;

  //$2102  OAMADDL
  //$2103  OAMADDH
  io.oamBaseAddress = random() & ~1;
  io.oamAddress = random();
  io.oamPriority = random() & 1;

  //$2105  BGMODE
  io.bgPriority = 0;
  io.bgMode = 0;

  //$210d  BG1HOFS
  io.hoffsetMode7 = random();

  //$210e  BG1VOFS
  io.voffsetMode7 = random();

  //$2115  VMAIN
  io.vramIncrementMode = random.bias(1) & 1;
  io.vramMapping = random();
  io.vramIncrementSize = 1;

  //$2116  VMADDL
  //$2117  VMADDH
  io.vramAddress = random();

  //$211a  M7SEL
  io.repeatMode7 = random();
  io.vflipMode7 = random() & 1;
  io.hflipMode7 = random() & 1;

  //$211b  M7A
  io.m7a = random();

  //$211c  M7B
  io.m7b = random();

  //$211d  M7C
  io.m7c = random();

  //$211e  M7D
  io.m7d = random();

  //$211f  M7X
  io.m7x = random();

  //$2120  M7Y
  io.m7y = random();

  //$2121  CGADD
  io.cgramAddress = random();
  io.cgramAddressLatch = random() & 1;

  //$2133  SETINI
  io.extbg = random() & 1;
  io.pseudoHires = random() & 1;
  io.overscan = 0;
  io.interlace = 0;

  //$213c  OPHCT
  io.hcounter = 0;

  //$213d  OPVCT
  io.vcounter = 0;

  mosaic.power();
  bg1.power();
  bg2.power();
  bg3.power();
  bg4.power();
  obj.power();
  window.power();
  screen.power();

  updateVideoMode();
}

void PPU::refresh() {
  if(system.runAhead) return;

  auto output = this->output;
  auto pitch  = 512;
  auto width  = 512;
  auto height = ppu.display.interlace ? 480 : 240;

  Emulator::platform->videoFrame(output, pitch * sizeof(uint16_t), width, height);
}

}
