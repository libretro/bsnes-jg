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

#include <vector>

#include "../serializer.hpp"
#include "../cpu.hpp"
#include "../memory.hpp"
#include "../settings.hpp"
#include "../bsmemory.hpp"

#include "sa1.hpp"

namespace SuperFamicom {

bool SA1::ROM::conflict() const {
  if(configuration.coprocessor.delayedSync) return false;

  if((cpu.r.mar & 0x408000) == 0x008000) return true;  //00-3f,80-bf:8000-ffff
  if((cpu.r.mar & 0xc00000) == 0xc00000) return true;  //c0-ff:0000-ffff
  return false;
}

uint8_t SA1::ROM::read(unsigned address, uint8_t data) {
  address = bus.mirror(address, size());
  return ReadableMemory::read(address, data);
}

void SA1::ROM::write(unsigned address, uint8_t data) {
}

//note: addresses are translated prior to invoking this function:
//00-3f,80-bf:8000-ffff mask=0x408000 => 00-3f:0000-ffff
//c0-ff:0000-ffff => untranslated
uint8_t SA1::ROM::readCPU(unsigned address, uint8_t data) {
  //reset vector overrides
  if((address & 0xffffe0) == 0x007fe0) {  //00:ffe0-ffef
    if(address == 0x7fea && sa1.mmio.cpu_nvsw) return sa1.mmio.snv >> 0;
    if(address == 0x7feb && sa1.mmio.cpu_nvsw) return sa1.mmio.snv >> 8;
    if(address == 0x7fee && sa1.mmio.cpu_ivsw) return sa1.mmio.siv >> 0;
    if(address == 0x7fef && sa1.mmio.cpu_ivsw) return sa1.mmio.siv >> 8;
  }

  static auto read = [](unsigned addr) {
    if((addr & 0x400000) && bsmemory.size()) return bsmemory.read(addr, 0x00);
    return sa1.rom.read(addr);
  };

  bool lo = address < 0x400000;  //*bmode==0 only applies to 00-3f,80-bf:8000-ffff
  address &= 0x3fffff;

  if(address < 0x100000) {  //00-1f,8000-ffff; c0-cf:0000-ffff
    if(lo && sa1.mmio.cbmode == 0) return read(address);
    return read(sa1.mmio.cb << 20 | (address & 0x0fffff));
  }

  if(address < 0x200000) {  //20-3f,8000-ffff; d0-df:0000-ffff
    if(lo && sa1.mmio.dbmode == 0) return read(address);
    return read(sa1.mmio.db << 20 | (address & 0x0fffff));
  }

  if(address < 0x300000) {  //80-9f,8000-ffff; e0-ef:0000-ffff
    if(lo && sa1.mmio.ebmode == 0) return read(address);
    return read(sa1.mmio.eb << 20 | (address & 0x0fffff));
  }

  if(address < 0x400000) {  //a0-bf,8000-ffff; f0-ff:0000-ffff
    if(lo && sa1.mmio.fbmode == 0) return read(address);
    return read(sa1.mmio.fb << 20 | (address & 0x0fffff));
  }

  return data;  //unreachable
}

void SA1::ROM::writeCPU(unsigned address, uint8_t data) {
}

uint8_t SA1::ROM::readSA1(unsigned address, uint8_t data) {
  if((address & 0x408000) == 0x008000) {
    address = (address & 0x800000) >> 2 | (address & 0x3f0000) >> 1 | (address & 0x007fff);
  }
  return readCPU(address, data);
}

void SA1::ROM::writeSA1(unsigned address, uint8_t data) {
}

bool SA1::BWRAM::conflict() const {
  if(configuration.coprocessor.delayedSync) return false;

  if((cpu.r.mar & 0x40e000) == 0x006000) return true;  //00-3f,80-bf:6000-7fff
  if((cpu.r.mar & 0xf00000) == 0x400000) return true;  //40-4f:0000-ffff
  return false;
}

uint8_t SA1::BWRAM::read(unsigned address, uint8_t data) {
  if(!size()) return data;
  address = bus.mirror(address, size());
  return WritableMemory::read(address, data);
}

void SA1::BWRAM::write(unsigned address, uint8_t data) {
  if(!size()) return;
  address = bus.mirror(address, size());
  return WritableMemory::write(address, data);
}

//note: addresses are translated prior to invoking this function:
//00-3f,80-bf:6000-7fff size=0x2000 => 00:0000-1fff
//40-4f:0000-ffff => untranslated
uint8_t SA1::BWRAM::readCPU(unsigned address, uint8_t data) {
  cpu.synchronizeCoprocessors();

  if(address < 0x2000) {  //$00-3f,80-bf:6000-7fff
    address = sa1.mmio.sbm * 0x2000 + (address & 0x1fff);
  }

  if(dma) return sa1.dmaCC1Read(address);
  return read(address, data);
}

void SA1::BWRAM::writeCPU(unsigned address, uint8_t data) {
  cpu.synchronizeCoprocessors();

  if(address < 0x2000) {  //$00-3f,80-bf:6000-7fff
    address = sa1.mmio.sbm * 0x2000 + (address & 0x1fff);
  }

  return write(address, data);
}

uint8_t SA1::BWRAM::readSA1(unsigned address, uint8_t data) {
  if(sa1.mmio.sw46 == 0) {
    //$40-43:0000-ffff x  32 projection
    address = (sa1.mmio.cbm & 0x1f) * 0x2000 + (address & 0x1fff);
    return readLinear(address, data);
  } else {
    //$60-6f:0000-ffff x 128 projection
    address = sa1.mmio.cbm * 0x2000 + (address & 0x1fff);
    return readBitmap(address & 0xfffff, data);
  }
}

void SA1::BWRAM::writeSA1(unsigned address, uint8_t data) {
  if(sa1.mmio.sw46 == 0) {
    //$40-43:0000-ffff x  32 projection
    address = (sa1.mmio.cbm & 0x1f) * 0x2000 + (address & 0x1fff);
    return writeLinear(address, data);
  } else {
    //$60-6f:0000-ffff x 128 projection
    address = sa1.mmio.cbm * 0x2000 + (address & 0x1fff);
    return writeBitmap(address & 0xfffff, data);
  }
}

uint8_t SA1::BWRAM::readLinear(unsigned address, uint8_t data) {
  return read(address, data);
}

void SA1::BWRAM::writeLinear(unsigned address, uint8_t data) {
  return write(address, data);
}

uint8_t SA1::BWRAM::readBitmap(uint32_t address, uint8_t data) {
  if(sa1.mmio.bbf == 0) {
    //4bpp
    unsigned shift = address & 1;
    address >>= 1;
    switch(shift) {
    case 0: return read(address) >> 0 & 15;
    case 1: return read(address) >> 4 & 15;
    }
  } else {
    //2bpp
    unsigned shift = address & 3;
    address >>= 2;
    switch(shift) {
    case 0: return read(address) >> 0 & 3;
    case 1: return read(address) >> 2 & 3;
    case 2: return read(address) >> 4 & 3;
    case 3: return read(address) >> 6 & 3;
    }
  }
  return 0; // unreachable
}

void SA1::BWRAM::writeBitmap(uint32_t address, uint8_t data) {
  if(sa1.mmio.bbf == 0) {
    //4bpp
    unsigned shift = address & 1;
    address >>= 1;
    switch(shift) {
    case 0: data = (read(address) & 0xf0) | (data & 0x0f) << 0; break;
    case 1: data = (read(address) & 0x0f) | (data & 0x0f) << 4; break;
    }
  } else {
    //2bpp
    unsigned shift = address & 3;
    address >>= 2;
    switch(shift) {
    case 0: data = (read(address) & 0xfc) | (data & 0x03) << 0; break;
    case 1: data = (read(address) & 0xf3) | (data & 0x03) << 2; break;
    case 2: data = (read(address) & 0xcf) | (data & 0x03) << 4; break;
    case 3: data = (read(address) & 0x3f) | (data & 0x03) << 6; break;
    }
  }
  write(address, data);
}

bool SA1::IRAM::conflict() const {
  if(configuration.coprocessor.delayedSync) return false;

  if((cpu.r.mar & 0x40f800) == 0x003000) return cpu.refresh() == 0;  //00-3f,80-bf:3000-37ff
  return false;
}

uint8_t SA1::IRAM::read(unsigned address, uint8_t data) {
  if(!size()) return data;
  address = bus.mirror(address, size());
  return WritableMemory::read(address, data);
}

void SA1::IRAM::write(unsigned address, uint8_t data) {
  if(!size()) return;
  address = bus.mirror(address, size());
  return WritableMemory::write(address, data);
}

uint8_t SA1::IRAM::readCPU(unsigned address, uint8_t data) {
  cpu.synchronizeCoprocessors();
  return read(address, data);
}

void SA1::IRAM::writeCPU(unsigned address, uint8_t data) {
  cpu.synchronizeCoprocessors();
  return write(address, data);
}

uint8_t SA1::IRAM::readSA1(unsigned address, uint8_t data) {
  return read(address, data);
}

void SA1::IRAM::writeSA1(unsigned address, uint8_t data) {
  return write(address, data);
}

//direct data transfer
void SA1::dmaNormal() {
  while(mmio.dtc--) {
    uint8_t data = r.mdr;
    uint32_t source = mmio.dsa;
    mmio.dsa = (mmio.dsa + 1) & 0xffffff;
    uint16_t target = mmio.dda++;

    if(mmio.sd == DMA::SourceROM && mmio.dd == DMA::DestBWRAM) {
      step();
      step();
      if(bwram.conflict()) step();
      if(bwram.conflict()) step();
      data = rom.readSA1(source, data);
      bwram.write(target, data);
    }

    if(mmio.sd == DMA::SourceROM && mmio.dd == DMA::DestIRAM) {
      step();
      if(iram.conflict() || rom.conflict()) step();
      if(iram.conflict()) step();
      data = rom.readSA1(source, data);
      iram.write(target, data);
    }

    if(mmio.sd == DMA::SourceBWRAM && mmio.dd == DMA::DestIRAM) {
      step();
      step();
      if(bwram.conflict() || iram.conflict()) step();
      if(bwram.conflict()) step();
      data = bwram.read(source, data);
      iram.write(target, data);
    }

    if(mmio.sd == DMA::SourceIRAM && mmio.dd == DMA::DestBWRAM) {
      step();
      step();
      if(bwram.conflict() || iram.conflict()) step();
      if(bwram.conflict()) step();
      data = iram.read(source, data);
      bwram.write(target, data);
    }
  }

  mmio.dma_irqfl = true;
  if(mmio.dma_irqen) mmio.dma_irqcl = 0;
}

//type-1 character conversion
void SA1::dmaCC1() {
  bwram.dma = true;
  mmio.chdma_irqfl = true;
  if(mmio.chdma_irqen) {
    mmio.chdma_irqcl = 0;
    cpu.irq(1);
  }
}

//((byte & 6) << 3) + (byte & 1) explanation:
//transforms a byte index (0-7) into a planar index:
//result[] = {0, 1, 16, 17, 32, 33, 48, 49};
//works for 2bpp, 4bpp and 8bpp modes

//type-1 character conversion
uint8_t SA1::dmaCC1Read(unsigned addr) {
  //16 bytes/char (2bpp); 32 bytes/char (4bpp); 64 bytes/char (8bpp)
  unsigned charmask = (1 << (6 - mmio.dmacb)) - 1;

  if((addr & charmask) == 0) {
    //buffer next character to I-RAM
    unsigned bpp = 2 << (2 - mmio.dmacb);
    unsigned bpl = (8 << mmio.dmasize) >> mmio.dmacb;
    unsigned bwmask = bwram.size() - 1;
    unsigned tile = ((addr - mmio.dsa) & bwmask) >> (6 - mmio.dmacb);
    unsigned ty = (tile >> mmio.dmasize);
    unsigned tx = tile & ((1 << mmio.dmasize) - 1);
    unsigned bwaddr = mmio.dsa + ty * 8 * bpl + tx * bpp;

    for(unsigned y = 0; y < 8; ++y) {
      uint64_t data = 0;
      for(unsigned byte = 0; byte < bpp; ++byte) {
        data |= (uint64_t)bwram.read((bwaddr + byte) & bwmask) << (byte << 3);
      }
      bwaddr += bpl;

      uint8_t out[] = {0, 0, 0, 0, 0, 0, 0, 0};
      for(unsigned x = 0; x < 8; ++x) {
        out[0] |= (data & 1) << (7 - x); data >>= 1;
        out[1] |= (data & 1) << (7 - x); data >>= 1;
        if(mmio.dmacb == 2) continue;
        out[2] |= (data & 1) << (7 - x); data >>= 1;
        out[3] |= (data & 1) << (7 - x); data >>= 1;
        if(mmio.dmacb == 1) continue;
        out[4] |= (data & 1) << (7 - x); data >>= 1;
        out[5] |= (data & 1) << (7 - x); data >>= 1;
        out[6] |= (data & 1) << (7 - x); data >>= 1;
        out[7] |= (data & 1) << (7 - x); data >>= 1;
      }

      for(unsigned byte = 0; byte < bpp; ++byte) {
        unsigned p = mmio.dda + (y << 1) + ((byte & 6) << 3) + (byte & 1);
        iram.write(p & 0x07ff, out[byte]);
      }
    }
  }

  return iram.read((mmio.dda + (addr & charmask)) & 0x07ff);
}

//type-2 character conversion
void SA1::dmaCC2() {
  //select register file index (0-7 or 8-15)
  const uint8_t *brf = &mmio.brf[(dma.line & 1) << 3];
  unsigned bpp = 2 << (2 - mmio.dmacb);
  unsigned addr = mmio.dda & 0x07ff;
  addr &= ~((1 << (7 - mmio.dmacb)) - 1);
  addr += (dma.line & 8) * bpp;
  addr += (dma.line & 7) * 2;

  for(unsigned byte = 0; byte < bpp; ++bpp) {
    uint8_t output = 0;
    for(unsigned bit = 0; bit < 8; ++bit) {
      output |= ((brf[bit] >> byte) & 1) << (7 - bit);
    }
    iram.write(addr + ((byte & 6) << 3) + (byte & 1), output);
  }

  dma.line = (dma.line + 1) & 15;
}

void SA1::idle() {
  step();
}

//RTx, JMx, JSx
void SA1::idleJump() {
  //ROM access penalty cycle: does not apply to BWRAM or IRAM
  if((r.pc.d & 0x408000) == 0x008000  //00-3f,80-bf:8000-ffff
  || (r.pc.d & 0xc00000) == 0xc00000  //c0-ff:0000-ffff
  ) {
    step();
    if(rom.conflict()) step();
  }
}

//Bxx
void SA1::idleBranch() {
  if(r.pc.d & 1) idleJump();
}

uint8_t SA1::read(unsigned address) {
  r.mar = address & 0xffffff;
  uint8_t data = r.mdr;

  if((address & 0x40fe00) == 0x002200  //00-3f,80-bf:2200-23ff
  ) {
    step();
    return r.mdr = readIOSA1(address, data);
  }

  if((address & 0x408000) == 0x008000  //00-3f,80-bf:8000-ffff
  || (address & 0xc00000) == 0xc00000  //c0-ff:0000-ffff
  ) {
    step();
    if(rom.conflict()) step();
    return r.mdr = rom.readSA1(address, data);
  }

  if((address & 0x40e000) == 0x006000  //00-3f,80-bf:6000-7fff
  || (address & 0xf00000) == 0x400000  //40-4f:0000-ffff
  || (address & 0xf00000) == 0x600000  //60-6f:0000-ffff
  ) {
    step();
    step();
    if(bwram.conflict()) step();
    if(bwram.conflict()) step();
    if((address & 1 << 22) && (address & 1 << 21)) return r.mdr = bwram.readBitmap(address & 0xfffff, data);
    if((address & 1 << 22)) return r.mdr = bwram.readLinear(address, data);
    return r.mdr = bwram.readSA1(address, data);
  }

  if((address & 0x40f800) == 0x000000  //00-3f,80-bf:0000-07ff
  || (address & 0x40f800) == 0x003000  //00-3f,80-bf:3000-37ff
  ) {
    step();
    if(iram.conflict()) step();
    if(iram.conflict()) step();
    return r.mdr = iram.readSA1(address, data);
  }

  step();
  return data;
}

void SA1::write(unsigned address, uint8_t data) {
  r.mar = address & 0xffffff;
  r.mdr = data;

  if((address & 0x40fe00) == 0x002200  //00-3f,80-bf:2200-23ff
  ) {
    step();
    return writeIOSA1(address, data);
  }

  if((address & 0x408000) == 0x008000  //00-3f,80-bf:8000-ffff
  || (address & 0xc00000) == 0xc00000  //c0-ff:0000-ffff
  ) {
    step();
    if(rom.conflict()) step();
    return rom.writeSA1(address, data);
  }

  if((address & 0x40e000) == 0x006000  //00-3f,80-bf:6000-7fff
  || (address & 0xf00000) == 0x400000  //40-4f:0000-ffff
  || (address & 0xf00000) == 0x600000  //60-6f:0000-ffff
  ) {
    step();
    step();
    if(bwram.conflict()) step();
    if(bwram.conflict()) step();
    if((address & 1 << 22) && (address & 1 << 21)) return bwram.writeBitmap(address & 0xfffff, data);
    if((address & 1 << 22)) return bwram.writeLinear(address, data);
    return bwram.writeSA1(address, data);
  }

  if((address & 0x40f800) == 0x000000  //00-3f,80-bf:0000-07ff
  || (address & 0x40f800) == 0x003000  //00-3f,80-bf:3000-37ff
  ) {
    step();
    if(iram.conflict()) step();
    if(iram.conflict()) step();
    return iram.writeSA1(address, data);
  }

  step();
  return;
}

//$230c (VDPL), $230d (VDPH) use this bus to read variable-length data.
//this is used both to keep VBR-reads from accessing MMIO registers, and
//to avoid syncing the S-CPU and SA-1*; as both chips are able to access
//these ports.
uint8_t SA1::readVBR(unsigned address, uint8_t data) {
  if((address & 0x408000) == 0x008000  //00-3f,80-bf:8000-ffff
  || (address & 0xc00000) == 0xc00000  //c0-ff:0000-ffff
  ) {
    return rom.readSA1(address, data);
  }

  if((address & 0x40e000) == 0x006000  //00-3f,80-bf:6000-7fff
  || (address & 0xf00000) == 0x400000  //40-4f:0000-ffff
  ) {
    return bwram.read(address, data);
  }

  if((address & 0x40f800) == 0x000000  //00-3f,80-bf:0000-07ff
  || (address & 0x40f800) == 0x003000  //00-3f,80-bf:3000-37ff
  ) {
    return iram.read(address, data);
  }

  return 0xff;
}

uint8_t SA1::readDisassembler(unsigned address) {
  //TODO: this is a hack; SA1::read() advances the clock; whereas Bus::read() does not
  //the CPU and SA1 bus are identical for ROM, but have differences in BWRAM and IRAM
  return bus.read(address, r.mdr);
}

uint8_t SA1::readIOCPU(unsigned address, uint8_t data) {
  cpu.synchronizeCoprocessors();

  switch(0x2200 | (address & 0x1ff)) {

  //(SFR) S-CPU flag read
  case 0x2300: {
    uint8_t _data;
    _data  = mmio.cpu_irqfl   << 7;
    _data |= mmio.cpu_ivsw    << 6;
    _data |= mmio.chdma_irqfl << 5;
    _data |= mmio.cpu_nvsw    << 4;
    _data |= mmio.cmeg;
    return _data;
  }

  //(VC) version code register
  case 0x230e: {
    break;  //does not actually exist on real hardware ... always returns open bus
  }

  }

  return data;
}

uint8_t SA1::readIOSA1(unsigned address, uint8_t) {
  synchronizeCPU();

  switch(0x2200 | (address & 0x1ff)) {

  //(CFR) SA-1 flag read
  case 0x2301: {
    uint8_t data;
    data  = mmio.sa1_irqfl   << 7;
    data |= mmio.timer_irqfl << 6;
    data |= mmio.dma_irqfl   << 5;
    data |= mmio.sa1_nmifl   << 4;
    data |= mmio.smeg;
    return data;
  }

  //(HCR) hcounter read
  case 0x2302: {
    //latch counters
    mmio.hcr = status.hcounter >> 2;
    mmio.vcr = status.vcounter;
    return mmio.hcr >> 0;
  }

  case 0x2303: {
    return mmio.hcr >> 8;
  }

  //(VCR) vcounter read
  case 0x2304: return mmio.vcr >> 0;
  case 0x2305: return mmio.vcr >> 8;

  //(MR) arithmetic result
  case 0x2306: return mmio.mr >>  0;
  case 0x2307: return mmio.mr >>  8;
  case 0x2308: return mmio.mr >> 16;
  case 0x2309: return mmio.mr >> 24;
  case 0x230a: return mmio.mr >> 32;

  //(OF) arithmetic overflow flag
  case 0x230b: return mmio.overflow << 7;

  //(VDPL) variable-length data read port low
  case 0x230c: {
    uint32_t data = readVBR(mmio.va + 0);
    data |= (readVBR(mmio.va + 1) << 8);
    data |= (readVBR(mmio.va + 2) << 16);
    data >>= mmio.vbit;

    return data >> 0;
  }

  //(VDPH) variable-length data read port high
  case 0x230d: {
    uint32_t data = readVBR(mmio.va + 0);
    data |= (readVBR(mmio.va + 1) << 8);
    data |= (readVBR(mmio.va + 2) << 16);
    data >>= mmio.vbit;

    if(mmio.hl == 1) {
      //auto-increment mode
      mmio.vbit += mmio.vb;
      mmio.va += (mmio.vbit >> 3);
      mmio.vbit &= 7;
    }

    return data >> 8;
  }

  }

  return 0xff;
}

void SA1::writeIOCPU(unsigned address, uint8_t data) {
  cpu.synchronizeCoprocessors();

  switch(0x2200 | (address & 0x1ff)) {

  //(CCNT) SA-1 control
  case 0x2200: {
    if(mmio.sa1_resb && !(data & 0x20)) {
      //reset SA-1 CPU (PC bank set to 0x00)
      r.pc.d = mmio.crv;
    }

    mmio.sa1_irq  = (data & 0x80);
    mmio.sa1_rdyb = (data & 0x40);
    mmio.sa1_resb = (data & 0x20);
    mmio.sa1_nmi  = (data & 0x10);
    mmio.smeg     = (data & 0x0f);

    if(mmio.sa1_irq) {
      mmio.sa1_irqfl = true;
      if(mmio.sa1_irqen) mmio.sa1_irqcl = 0;
    }

    if(mmio.sa1_nmi) {
      mmio.sa1_nmifl = true;
      if(mmio.sa1_nmien) mmio.sa1_nmicl = 0;
    }

    return;
  }

  //(SIE) S-CPU interrupt enable
  case 0x2201: {
    if(!mmio.cpu_irqen && (data & 0x80)) {
      if(mmio.cpu_irqfl) {
        mmio.cpu_irqcl = 0;
        cpu.irq(1);
      }
    }

    if(!mmio.chdma_irqen && (data & 0x20)) {
      if(mmio.chdma_irqfl) {
        mmio.chdma_irqcl = 0;
        cpu.irq(1);
      }
    }

    mmio.cpu_irqen   = (data & 0x80);
    mmio.chdma_irqen = (data & 0x20);
    return;
  }

  //(SIC) S-CPU interrupt clear
  case 0x2202: {
    mmio.cpu_irqcl   = (data & 0x80);
    mmio.chdma_irqcl = (data & 0x20);

    if(mmio.cpu_irqcl  ) mmio.cpu_irqfl   = false;
    if(mmio.chdma_irqcl) mmio.chdma_irqfl = false;

    if(!mmio.cpu_irqfl && !mmio.chdma_irqfl) cpu.irq(0);
    return;
  }

  //(CRV) SA-1 reset vector
  case 0x2203: { mmio.crv = (mmio.crv & 0xff00) | data; return; }
  case 0x2204: { mmio.crv = (data << 8) | (mmio.crv & 0xff); return; }

  //(CNV) SA-1 NMI vector
  case 0x2205: { mmio.cnv = (mmio.cnv & 0xff00) | data; return; }
  case 0x2206: { mmio.cnv = (data << 8) | (mmio.cnv & 0xff); return; }

  //(CIV) SA-1 IRQ vector
  case 0x2207: { mmio.civ = (mmio.civ & 0xff00) | data; return; }
  case 0x2208: { mmio.civ = (data << 8) | (mmio.civ & 0xff); return; }

  //(CXB) Super MMC bank C
  case 0x2220: {
    mmio.cbmode = (data & 0x80);
    mmio.cb     = (data & 0x07);
    return;
  }

  //(DXB) Super MMC bank D
  case 0x2221: {
    mmio.dbmode = (data & 0x80);
    mmio.db     = (data & 0x07);
    return;
  }

  //(EXB) Super MMC bank E
  case 0x2222: {
    mmio.ebmode = (data & 0x80);
    mmio.eb     = (data & 0x07);
    return;
  }

  //(FXB) Super MMC bank F
  case 0x2223: {
    mmio.fbmode = (data & 0x80);
    mmio.fb     = (data & 0x07);
    return;
  }

  //(BMAPS) S-CPU BW-RAM address mapping
  case 0x2224: {
    mmio.sbm = (data & 0x1f);
    return;
  }

  //(SWBE) S-CPU BW-RAM write enable
  case 0x2226: {
    mmio.swen = (data & 0x80);
    return;
  }

  //(BWPA) BW-RAM write-protected area
  case 0x2228: {
    mmio.bwp = (data & 0x0f);
    return;
  }

  //(SIWP) S-CPU I-RAM write protection
  case 0x2229: {
    mmio.siwp = data;
    return;
  }

  case 0x2231: case 0x2232: case 0x2233: case 0x2234: case 0x2235: case 0x2236: case 0x2237: {
    return writeIOShared(address, data);
  }

  }
}

void SA1::writeIOSA1(unsigned address, uint8_t data) {
  synchronizeCPU();

  switch(0x2200 | (address & 0x1ff)) {

  //(SCNT) S-CPU control
  case 0x2209: {
    mmio.cpu_irq  = (data & 0x80);
    mmio.cpu_ivsw = (data & 0x40);
    mmio.cpu_nvsw = (data & 0x10);
    mmio.cmeg     = (data & 0x0f);

    if(mmio.cpu_irq) {
      mmio.cpu_irqfl = true;
      if(mmio.cpu_irqen) {
        mmio.cpu_irqcl = 0;
        cpu.irq(1);
      }
    }

    return;
  }

  //(CIE) SA-1 interrupt enable
  case 0x220a: {
    if(!mmio.sa1_irqen   && (data & 0x80) && mmio.sa1_irqfl  ) mmio.sa1_irqcl   = 0;
    if(!mmio.timer_irqen && (data & 0x40) && mmio.timer_irqfl) mmio.timer_irqcl = 0;
    if(!mmio.dma_irqen   && (data & 0x20) && mmio.dma_irqfl  ) mmio.dma_irqcl   = 0;
    if(!mmio.sa1_nmien   && (data & 0x10) && mmio.sa1_nmifl  ) mmio.sa1_nmicl   = 0;

    mmio.sa1_irqen   = (data & 0x80);
    mmio.timer_irqen = (data & 0x40);
    mmio.dma_irqen   = (data & 0x20);
    mmio.sa1_nmien   = (data & 0x10);
    return;
  }

  //(CIC) SA-1 interrupt clear
  case 0x220b: {
    mmio.sa1_irqcl   = (data & 0x80);
    mmio.timer_irqcl = (data & 0x40);
    mmio.dma_irqcl   = (data & 0x20);
    mmio.sa1_nmicl   = (data & 0x10);

    if(mmio.sa1_irqcl)   mmio.sa1_irqfl   = false;
    if(mmio.timer_irqcl) mmio.timer_irqfl = false;
    if(mmio.dma_irqcl)   mmio.dma_irqfl   = false;
    if(mmio.sa1_nmicl)   mmio.sa1_nmifl   = false;
    return;
  }

  //(SNV) S-CPU NMI vector
  case 0x220c: { mmio.snv = (mmio.snv & 0xff00) | data; return; }
  case 0x220d: { mmio.snv = (data << 8) | (mmio.snv & 0xff); return; }

  //(SIV) S-CPU IRQ vector
  case 0x220e: { mmio.siv = (mmio.siv & 0xff00) | data; return; }
  case 0x220f: { mmio.siv = (data << 8) | (mmio.siv & 0xff); return; }

  //(TMC) H/V timer control
  case 0x2210: {
    mmio.hvselb = (data & 0x80);
    mmio.ven    = (data & 0x02);
    mmio.hen    = (data & 0x01);
    return;
  }

  //(CTR) SA-1 timer restart
  case 0x2211: {
    status.vcounter = 0;
    status.hcounter = 0;
    return;
  }

  //(HCNT) H-count
  case 0x2212: { mmio.hcnt = (mmio.hcnt & 0xff00) | (data << 0); return; }
  case 0x2213: { mmio.hcnt = (mmio.hcnt & 0x00ff) | (data << 8); return; }

  //(VCNT) V-count
  case 0x2214: { mmio.vcnt = (mmio.vcnt & 0xff00) | (data << 0); return; }
  case 0x2215: { mmio.vcnt = (mmio.vcnt & 0x00ff) | (data << 8); return; }

  //(BMAP) SA-1 BW-RAM address mapping
  case 0x2225: {
    mmio.sw46 = (data & 0x80);
    mmio.cbm  = (data & 0x7f);
    return;
  }

  //(CWBE) SA-1 BW-RAM write enable
  case 0x2227: {
    mmio.cwen = (data & 0x80);
    return;
  }

  //(CIWP) SA-1 I-RAM write protection
  case 0x222a: {
    mmio.ciwp = data;
    return;
  }

  //(DCNT) DMA control
  case 0x2230: {
    mmio.dmaen = (data & 0x80);
    mmio.dprio = (data & 0x40);
    mmio.cden  = (data & 0x20);
    mmio.cdsel = (data & 0x10);
    mmio.dd    = (data & 0x04);
    mmio.sd    = (data & 0x03);

    if(mmio.dmaen == 0) dma.line = 0;
    return;
  }

  case 0x2231: case 0x2232: case 0x2233: case 0x2234: case 0x2235: case 0x2236: case 0x2237: {
    return writeIOShared(address, data);
  }

  //(DTC) DMA terminal counter
  case 0x2238: { mmio.dtc = (mmio.dtc & 0xff00) | (data << 0); return; }
  case 0x2239: { mmio.dtc = (mmio.dtc & 0x00ff) | (data << 8); return; }

  //(BBF) BW-RAM bitmap format
  case 0x223f: { mmio.bbf = (data & 0x80); return; }

  //(BRF) bitmap register files
  case 0x2240: { mmio.brf[ 0] = data; return; }
  case 0x2241: { mmio.brf[ 1] = data; return; }
  case 0x2242: { mmio.brf[ 2] = data; return; }
  case 0x2243: { mmio.brf[ 3] = data; return; }
  case 0x2244: { mmio.brf[ 4] = data; return; }
  case 0x2245: { mmio.brf[ 5] = data; return; }
  case 0x2246: { mmio.brf[ 6] = data; return; }
  case 0x2247: { mmio.brf[ 7] = data;
    if(mmio.dmaen) {
      if(mmio.cden == 1 && mmio.cdsel == 0) {
        dmaCC2();
      }
    }
    return;
  }
  case 0x2248: { mmio.brf[ 8] = data; return; }
  case 0x2249: { mmio.brf[ 9] = data; return; }
  case 0x224a: { mmio.brf[10] = data; return; }
  case 0x224b: { mmio.brf[11] = data; return; }
  case 0x224c: { mmio.brf[12] = data; return; }
  case 0x224d: { mmio.brf[13] = data; return; }
  case 0x224e: { mmio.brf[14] = data; return; }
  case 0x224f: { mmio.brf[15] = data;
    if(mmio.dmaen) {
      if(mmio.cden == 1 && mmio.cdsel == 0) {
        dmaCC2();
      }
    }
    return;
  }

  //(MCNT) arithmetic control
  case 0x2250: {
    mmio.acm = (data & 0x02);
    mmio.md  = (data & 0x01);

    if(mmio.acm) mmio.mr = 0;
    return;
  }

  //(MAL) multiplicand / dividend low
  case 0x2251: {
    mmio.ma = (mmio.ma & ~0x00ff) | data << 0;
    return;
  }

  //(MAH) multiplicand / dividend high
  case 0x2252: {
    mmio.ma = (mmio.ma & ~0xff00) | data << 8;
    return;
  }

  //(MBL) multiplier / divisor low
  case 0x2253: {
    mmio.mb = (mmio.mb & ~0x00ff) | data << 0;
    return;
  }

  //(MBH) multiplier / divisor high
  //multiplication / cumulative sum only resets MB
  //division resets both MA and MB
  case 0x2254: {
    mmio.mb = (mmio.mb & ~0xff00) | data << 8;

    if(mmio.acm == 0) {
      if(mmio.md == 0) {
        //signed multiplication
        mmio.mr = (uint32_t)((int16_t)mmio.ma * (int16_t)mmio.mb);
        mmio.mb = 0;
      } else {
        //unsigned division
        if(mmio.mb == 0) {
          mmio.mr = 0;
        } else {
          int16_t dividend = mmio.ma;
          uint16_t divisor = mmio.mb;
          uint16_t remainder = dividend >= 0 ? uint16_t(dividend % divisor) : uint16_t((dividend % divisor + divisor) % divisor);
          uint16_t quotient = (dividend - remainder) / divisor;
          mmio.mr = remainder << 16 | quotient;
        }
        mmio.ma = 0;
        mmio.mb = 0;
      }
    } else {
      //sigma (accumulative multiplication)
      mmio.mr += (int16_t)mmio.ma * (int16_t)mmio.mb;
      mmio.overflow = mmio.mr >> 40;
      mmio.mr &= 0xffffffffff;
      mmio.mb = 0;
    }
    return;
  }

  //(VBD) variable-length bit processing
  case 0x2258: {
    mmio.hl = (data & 0x80);
    mmio.vb = (data & 0x0f);
    if(mmio.vb == 0) mmio.vb = 16;

    if(mmio.hl == 0) {
      //fixed mode
      mmio.vbit += mmio.vb;
      mmio.va += (mmio.vbit >> 3);
      mmio.vbit &= 7;
    }
    return;
  }

  //(VDA) variable-length bit game pak ROM start address
  case 0x2259: { mmio.va = (mmio.va & 0xffff00) | (data <<  0); return; }
  case 0x225a: { mmio.va = (mmio.va & 0xff00ff) | (data <<  8); return; }
  case 0x225b: { mmio.va = (mmio.va & 0x00ffff) | (data << 16); mmio.vbit = 0; return; }

  }
}

void SA1::writeIOShared(unsigned address, uint8_t data) {
  switch(0x2200 | (address & 0x1ff)) {

  //(CDMA) character conversion DMA parameters
  case 0x2231: {
    mmio.chdend  = (data & 0x80);
    mmio.dmasize = (data >> 2) & 7;
    mmio.dmacb   = (data & 0x03);

    if(mmio.chdend) bwram.dma = false;
    if(mmio.dmasize > 5) mmio.dmasize = 5;
    if(mmio.dmacb   > 2) mmio.dmacb   = 2;
    return;
  }

  //(SDA) DMA source device start address
  case 0x2232: { mmio.dsa = (mmio.dsa & 0xffff00) | (data <<  0); return; }
  case 0x2233: { mmio.dsa = (mmio.dsa & 0xff00ff) | (data <<  8); return; }
  case 0x2234: { mmio.dsa = (mmio.dsa & 0x00ffff) | (data << 16); return; }

  //(DDA) DMA destination start address
  case 0x2235: { mmio.dda = (mmio.dda & 0xffff00) | (data <<  0); return; }
  case 0x2236: { mmio.dda = (mmio.dda & 0xff00ff) | (data <<  8);
    if(mmio.dmaen) {
      if(mmio.cden == 0 && mmio.dd == DMA::DestIRAM) {
        dmaNormal();
      } else if(mmio.cden == 1 && mmio.cdsel == 1) {
        dmaCC1();
      }
    }
    return;
  }
  case 0x2237: { mmio.dda = (mmio.dda & 0x00ffff) | (data << 16);
    if(mmio.dmaen) {
      if(mmio.cden == 0 && mmio.dd == DMA::DestBWRAM) {
        dmaNormal();
      }
    }
    return;
  }

  }
}

void SA1::serialize(serializer& s) {
  WDC65816::serialize(s);
  Thread::serialize(s);

  s.array(iram.data(), iram.size());
  s.array(bwram.data(), bwram.size());
  s.integer(bwram.dma);

  //sa1.hpp
  s.integer(status.counter);

  s.integer(status.interruptPending);

  s.integer(status.scanlines);
  s.integer(status.vcounter);
  s.integer(status.hcounter);

  //dma/dma.hpp
  s.integer(dma.line);

  //mmio/mmio.hpp
  s.integer(mmio.sa1_irq);
  s.integer(mmio.sa1_rdyb);
  s.integer(mmio.sa1_resb);
  s.integer(mmio.sa1_nmi);
  s.integer(mmio.smeg);

  s.integer(mmio.cpu_irqen);
  s.integer(mmio.chdma_irqen);

  s.integer(mmio.cpu_irqcl);
  s.integer(mmio.chdma_irqcl);

  s.integer(mmio.crv);

  s.integer(mmio.cnv);

  s.integer(mmio.civ);

  s.integer(mmio.cpu_irq);
  s.integer(mmio.cpu_ivsw);
  s.integer(mmio.cpu_nvsw);
  s.integer(mmio.cmeg);

  s.integer(mmio.sa1_irqen);
  s.integer(mmio.timer_irqen);
  s.integer(mmio.dma_irqen);
  s.integer(mmio.sa1_nmien);

  s.integer(mmio.sa1_irqcl);
  s.integer(mmio.timer_irqcl);
  s.integer(mmio.dma_irqcl);
  s.integer(mmio.sa1_nmicl);

  s.integer(mmio.snv);

  s.integer(mmio.siv);

  s.integer(mmio.hvselb);
  s.integer(mmio.ven);
  s.integer(mmio.hen);

  s.integer(mmio.hcnt);

  s.integer(mmio.vcnt);

  s.integer(mmio.cbmode);
  s.integer(mmio.cb);

  s.integer(mmio.dbmode);
  s.integer(mmio.db);

  s.integer(mmio.ebmode);
  s.integer(mmio.eb);

  s.integer(mmio.fbmode);
  s.integer(mmio.fb);

  s.integer(mmio.sbm);

  s.integer(mmio.sw46);
  s.integer(mmio.cbm);

  s.integer(mmio.swen);

  s.integer(mmio.cwen);

  s.integer(mmio.bwp);

  s.integer(mmio.siwp);

  s.integer(mmio.ciwp);

  s.integer(mmio.dmaen);
  s.integer(mmio.dprio);
  s.integer(mmio.cden);
  s.integer(mmio.cdsel);
  s.integer(mmio.dd);
  s.integer(mmio.sd);

  s.integer(mmio.chdend);
  s.integer(mmio.dmasize);
  s.integer(mmio.dmacb);

  s.integer(mmio.dsa);

  s.integer(mmio.dda);

  s.integer(mmio.dtc);

  s.integer(mmio.bbf);

  s.array(mmio.brf);

  s.integer(mmio.acm);
  s.integer(mmio.md);

  s.integer(mmio.ma);

  s.integer(mmio.mb);

  s.integer(mmio.hl);
  s.integer(mmio.vb);

  s.integer(mmio.va);
  s.integer(mmio.vbit);

  s.integer(mmio.cpu_irqfl);
  s.integer(mmio.chdma_irqfl);

  s.integer(mmio.sa1_irqfl);
  s.integer(mmio.timer_irqfl);
  s.integer(mmio.dma_irqfl);
  s.integer(mmio.sa1_nmifl);

  s.integer(mmio.hcr);

  s.integer(mmio.vcr);

  s.integer(mmio.mr);

  s.integer(mmio.overflow);
}

SA1 sa1;

void SA1::synchronizeCPU() {
  if(clock >= 0) scheduler.resume(cpu.thread);
}

void SA1::Enter() {
  while(true) {
    scheduler.synchronize();
    sa1.main();
  }
}

void SA1::main() {
  if(r.wai) return instructionWait();
  if(r.stp) return instructionStop();

  if(mmio.sa1_rdyb || mmio.sa1_resb) {
    //SA-1 co-processor is asleep
    step();
    return;
  }

  if(status.interruptPending) {
    status.interruptPending = false;
    interrupt();
    return;
  }

  instruction();
}

//override R65816::interrupt() to support SA-1 vector location IO registers
void SA1::interrupt() {
  read(r.pc.d);
  idle();
  if(!r.e) push(r.pc.b);
  push(r.pc.h);
  push(r.pc.l);
  push(r.e ? r.p & ~0x10 : r.p);
  r.p.i = 1;
  r.p.d = 0;
  r.pc.d = r.vector;  //PC bank set to 0x00
}

void SA1::lastCycle() {
  if(mmio.sa1_nmi && !mmio.sa1_nmicl) {
    status.interruptPending = true;
    r.vector = mmio.cnv;
    mmio.sa1_nmifl = true;
    mmio.sa1_nmicl = 1;
    r.wai = false;
  } else if(!r.p.i) {
    if(mmio.timer_irqen && !mmio.timer_irqcl) {
      status.interruptPending = true;
      r.vector = mmio.civ;
      mmio.timer_irqfl = true;
      r.wai = false;
    } else if(mmio.dma_irqen && !mmio.dma_irqcl) {
      status.interruptPending = true;
      r.vector = mmio.civ;
      mmio.dma_irqfl = true;
      r.wai = false;
    } else if(mmio.sa1_irq && !mmio.sa1_irqcl) {
      status.interruptPending = true;
      r.vector = mmio.civ;
      mmio.sa1_irqfl = true;
      r.wai = false;
    }
  }
}

bool SA1::interruptPending() const {
  return status.interruptPending;
}

void SA1::step() {
  clock += (uint64_t)cpu.frequency << 1;
  synchronizeCPU();

  //adjust counters:
  //note that internally, status counters are in clocks;
  //whereas MMIO register counters are in dots (4 clocks = 1 dot)
  if(mmio.hvselb == 0) {
    //HV timer
    status.hcounter += 2;
    if(status.hcounter >= 1364) {
      status.hcounter = 0;
      if(++status.vcounter >= status.scanlines) {
        status.vcounter = 0;
      }
    }
  } else {
    //linear timer
    status.hcounter += 2;
    status.vcounter += status.hcounter >> 11;
    status.hcounter &= 0x07ff;
    status.vcounter &= 0x01ff;
  }

  //test counters for timer IRQ
  switch(mmio.hen << 0 | mmio.ven << 1) {
  case 0: break;
  case 1: if(status.hcounter == mmio.hcnt << 2) triggerIRQ(); break;
  case 2: if(status.vcounter == mmio.vcnt && status.hcounter == 0) triggerIRQ(); break;
  case 3: if(status.vcounter == mmio.vcnt && status.hcounter == mmio.hcnt << 2) triggerIRQ(); break;
  }
}

void SA1::triggerIRQ() {
  mmio.timer_irqfl = true;
  if(mmio.timer_irqen) mmio.timer_irqcl = 0;
}

void SA1::unload() {
  rom.reset();
  iram.reset();
  bwram.reset();
}

void SA1::power() {
  double overclock = std::max(1.0, std::min(4.0, configuration.sa1.overclock / 100.0));

  WDC65816::power();
  create(SA1::Enter, system.cpuFrequency() * overclock);

  bwram.dma = false;
  for(unsigned address = 0; address < iram.size(); ++address) {
    iram.write(address, 0x00);
  }

  status.counter = 0;

  status.interruptPending = false;

  status.scanlines = Region::PAL() ? 312 : 262;
  status.vcounter  = 0;
  status.hcounter  = 0;

  dma.line = 0;

  //$2200 CCNT
  mmio.sa1_irq  = false;
  mmio.sa1_rdyb = false;
  mmio.sa1_resb = true;
  mmio.sa1_nmi  = false;
  mmio.smeg     = 0;

  //$2201 SIE
  mmio.cpu_irqen   = false;
  mmio.chdma_irqen = false;

  //$2202 SIC
  mmio.cpu_irqcl   = false;
  mmio.chdma_irqcl = false;

  //$2203,$2204 CRV
  mmio.crv = 0x0000;

  //$2205,$2206 CNV
  mmio.cnv = 0x0000;

  //$2207,$2208 CIV
  mmio.civ = 0x0000;

  //$2209 SCNT
  mmio.cpu_irq  = false;
  mmio.cpu_ivsw = false;
  mmio.cpu_nvsw = false;
  mmio.cmeg     = 0;

  //$220a CIE
  mmio.sa1_irqen   = false;
  mmio.timer_irqen = false;
  mmio.dma_irqen   = false;
  mmio.sa1_nmien   = false;

  //$220b CIC
  mmio.sa1_irqcl   = false;
  mmio.timer_irqcl = false;
  mmio.dma_irqcl   = false;
  mmio.sa1_nmicl   = false;

  //$220c,$220d SNV
  mmio.snv = 0x0000;

  //$220e,$220f SIV
  mmio.siv = 0x0000;

  //$2210
  mmio.hvselb = false;
  mmio.ven    = false;
  mmio.hen    = false;

  //$2212,$2213 HCNT
  mmio.hcnt = 0x0000;

  //$2214,$2215 VCNT
  mmio.vcnt = 0x0000;

  //$2220-2223 CXB, DXB, EXB, FXB
  mmio.cbmode = 0;
  mmio.dbmode = 0;
  mmio.ebmode = 0;
  mmio.fbmode = 0;

  mmio.cb = 0x00;
  mmio.db = 0x01;
  mmio.eb = 0x02;
  mmio.fb = 0x03;

  //$2224 BMAPS
  mmio.sbm = 0x00;

  //$2225 BMAP
  mmio.sw46 = false;
  mmio.cbm  = 0x00;

  //$2226 SWBE
  mmio.swen = false;

  //$2227 CWBE
  mmio.cwen = false;

  //$2228 BWPA
  mmio.bwp = 0x0f;

  //$2229 SIWP
  mmio.siwp = 0x00;

  //$222a CIWP
  mmio.ciwp = 0x00;

  //$2230 DCNT
  mmio.dmaen = false;
  mmio.dprio = false;
  mmio.cden  = false;
  mmio.cdsel = false;
  mmio.dd    = 0;
  mmio.sd    = 0;

  //$2231 CDMA
  mmio.chdend  = false;
  mmio.dmasize = 0;
  mmio.dmacb   = 0;

  //$2232-$2234 SDA
  mmio.dsa = 0x000000;

  //$2235-$2237 DDA
  mmio.dda = 0x000000;

  //$2238,$2239 DTC
  mmio.dtc = 0x0000;

  //$223f BBF
  mmio.bbf = 0;

  //$2240-$224f BRF
  for(auto& n : mmio.brf) n = 0x00;

  //$2250 MCNT
  mmio.acm = 0;
  mmio.md  = 0;

  //$2251,$2252 MA
  mmio.ma = 0x0000;

  //$2253,$2254 MB
  mmio.mb = 0x0000;

  //$2258 VBD
  mmio.hl = false;
  mmio.vb = 16;

  //$2259-$225b
  mmio.va   = 0x000000;
  mmio.vbit = 0;

  //$2300 SFR
  mmio.cpu_irqfl   = false;
  mmio.chdma_irqfl = false;

  //$2301 CFR
  mmio.sa1_irqfl   = false;
  mmio.timer_irqfl = false;
  mmio.dma_irqfl   = false;
  mmio.sa1_nmifl   = false;

  //$2302,$2303 HCR
  mmio.hcr = 0x0000;

  //$2304,$2305 VCR
  mmio.vcr = 0x0000;

  //$2306-$230a MR
  mmio.mr = 0;

  //$230b
  mmio.overflow = false;
}

}
