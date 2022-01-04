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

#include "../interface.hpp"

#include "superfx.hpp"

namespace SuperFamicom {

//ROM / RAM access from the S-CPU

uint8_t* SuperFX::CPUROM::data() {
  return superfx.rom.data();
}

unsigned SuperFX::CPUROM::size() const {
  return superfx.rom.size();
}

uint8_t SuperFX::CPUROM::read(unsigned addr, uint8_t data) {
  if(superfx.regs.sfr.g && superfx.regs.scmr.ron) {
    static const uint8_t vector[16] = {
      0x00, 0x01, 0x00, 0x01, 0x04, 0x01, 0x00, 0x01,
      0x00, 0x01, 0x08, 0x01, 0x00, 0x01, 0x0c, 0x01,
    };
    return vector[addr & 15];
  }
  return superfx.rom.read(addr, data);
}

void SuperFX::CPUROM::write(unsigned addr, uint8_t data) {
  superfx.rom.write(addr, data);
}

uint8_t* SuperFX::CPURAM::data() {
  return superfx.ram.data();
}

unsigned SuperFX::CPURAM::size() const {
  return superfx.ram.size();
}

uint8_t SuperFX::CPURAM::read(unsigned addr, uint8_t data) {
  if(superfx.regs.sfr.g && superfx.regs.scmr.ran) return data;
  return superfx.ram.read(addr, data);
}

void SuperFX::CPURAM::write(unsigned addr, uint8_t data) {
  superfx.ram.write(addr, data);
}

void SuperFX::stop() {
  cpu.irq(1);
}

uint8_t SuperFX::color(uint8_t source) {
  if(regs.por.highnibble) return (regs.colr & 0xf0) | (source >> 4);
  if(regs.por.freezehigh) return (regs.colr & 0xf0) | (source & 0x0f);
  return source;
}

void SuperFX::plot(uint8_t x, uint8_t y) {
  if(!regs.por.transparent) {
    if(regs.scmr.md == 3) {
      if(regs.por.freezehigh) {
        if((regs.colr & 0x0f) == 0) return;
      } else {
        if(regs.colr == 0) return;
      }
    } else {
      if((regs.colr & 0x0f) == 0) return;
    }
  }

  uint8_t color = regs.colr;
  if(regs.por.dither && regs.scmr.md != 3) {
    if((x ^ y) & 1) color >>= 4;
    color &= 0x0f;
  }

  uint16_t offset = (y << 5) + (x >> 3);
  if(offset != pixelcache[0].offset) {
    flushPixelCache(pixelcache[1]);
    pixelcache[1] = pixelcache[0];
    pixelcache[0].bitpend = 0x00;
    pixelcache[0].offset = offset;
  }

  x = (x & 7) ^ 7;
  pixelcache[0].data[x] = color;
  pixelcache[0].bitpend |= 1 << x;
  if(pixelcache[0].bitpend == 0xff) {
    flushPixelCache(pixelcache[1]);
    pixelcache[1] = pixelcache[0];
    pixelcache[0].bitpend = 0x00;
  }
}

uint8_t SuperFX::rpix(uint8_t x, uint8_t y) {
  flushPixelCache(pixelcache[1]);
  flushPixelCache(pixelcache[0]);

  unsigned cn = 0;  //character number
  switch(regs.por.obj ? 3 : regs.scmr.ht) {
  case 0: cn = ((x & 0xf8) << 1) + ((y & 0xf8) >> 3); break;
  case 1: cn = ((x & 0xf8) << 1) + ((x & 0xf8) >> 1) + ((y & 0xf8) >> 3); break;
  case 2: cn = ((x & 0xf8) << 1) + ((x & 0xf8) << 0) + ((y & 0xf8) >> 3); break;
  case 3: cn = ((y & 0x80) << 2) + ((x & 0x80) << 1) + ((y & 0x78) << 1) + ((x & 0x78) >> 3); break;
  }
  unsigned bpp = 2 << (regs.scmr.md - (regs.scmr.md >> 1));  // = [regs.scmr.md]{ 2, 4, 4, 8 };
  unsigned addr = 0x700000 + (cn * (bpp << 3)) + (regs.scbr << 10) + ((y & 0x07) * 2);
  uint8_t data = 0x00;
  x = (x & 7) ^ 7;

  for(unsigned n = 0; n < bpp; ++n) {
    unsigned byte = ((n >> 1) << 4) + (n & 1);  // = [n]{ 0, 1, 16, 17, 32, 33, 48, 49 };
    step(regs.clsr ? 5 : 6);
    data |= ((read(addr + byte) >> x) & 1) << n;
  }

  return data;
}

void SuperFX::flushPixelCache(PixelCache& cache) {
  if(cache.bitpend == 0x00) return;

  uint8_t x = cache.offset << 3;
  uint8_t y = cache.offset >> 5;

  unsigned cn = 0;  //character number
  switch(regs.por.obj ? 3 : regs.scmr.ht) {
  case 0: cn = ((x & 0xf8) << 1) + ((y & 0xf8) >> 3); break;
  case 1: cn = ((x & 0xf8) << 1) + ((x & 0xf8) >> 1) + ((y & 0xf8) >> 3); break;
  case 2: cn = ((x & 0xf8) << 1) + ((x & 0xf8) << 0) + ((y & 0xf8) >> 3); break;
  case 3: cn = ((y & 0x80) << 2) + ((x & 0x80) << 1) + ((y & 0x78) << 1) + ((x & 0x78) >> 3); break;
  }
  unsigned bpp = 2 << (regs.scmr.md - (regs.scmr.md >> 1));  // = [regs.scmr.md]{ 2, 4, 4, 8 };
  unsigned addr = 0x700000 + (cn * (bpp << 3)) + (regs.scbr << 10) + ((y & 0x07) * 2);

  for(unsigned n = 0; n < bpp; ++n) {
    unsigned byte = ((n >> 1) << 4) + (n & 1);  // = [n]{ 0, 1, 16, 17, 32, 33, 48, 49 };
    uint8_t data = 0x00;
    for(unsigned x = 0; x < 8; ++x) data |= ((cache.data[x] >> n) & 1) << x;
    if(cache.bitpend != 0xff) {
      step(regs.clsr ? 5 : 6);
      data &= cache.bitpend;
      data |= read(addr + byte) & ~cache.bitpend;
    }
    step(regs.clsr ? 5 : 6);
    write(addr + byte, data);
  }

  cache.bitpend = 0x00;
}

uint8_t SuperFX::read(unsigned addr, uint8_t data) {
  if((addr & 0xc00000) == 0x000000) {  //$00-3f:0000-7fff,:8000-ffff
    while(!regs.scmr.ron) {
      step(6);
      synchronizeCPU();
      if(synchronizing()) break;
    }
    return rom.read((((addr & 0x3f0000) >> 1) | (addr & 0x7fff)) & romMask);
  }

  if((addr & 0xe00000) == 0x400000) {  //$40-5f:0000-ffff
    while(!regs.scmr.ron) {
      step(6);
      synchronizeCPU();
      if(synchronizing()) break;
    }
    return rom.read(addr & romMask);
  }

  if((addr & 0xe00000) == 0x600000) {  //$60-7f:0000-ffff
    while(!regs.scmr.ran) {
      step(6);
      synchronizeCPU();
      if(synchronizing()) break;
    }
    return ram.read(addr & ramMask);
  }

  return data;
}

void SuperFX::write(unsigned addr, uint8_t data) {
  if((addr & 0xe00000) == 0x600000) {  //$60-7f:0000-ffff
    while(!regs.scmr.ran) {
      step(6);
      synchronizeCPU();
      if(synchronizing()) break;
    }
    return ram.write(addr & ramMask, data);
  }
}

uint8_t SuperFX::readOpcode(uint16_t addr) {
  uint16_t offset = addr - regs.cbr;
  if(offset < 512) {
    if(cache.valid[offset >> 4] == false) {
      unsigned dp = offset & 0xfff0;
      unsigned sp = (regs.pbr << 16) + ((regs.cbr + dp) & 0xfff0);
      for(unsigned n = 0; n < 16; ++n) {
        step(regs.clsr ? 5 : 6);
        cache.buffer[dp++] = read(sp++);
      }
      cache.valid[offset >> 4] = true;
    } else {
      step(regs.clsr ? 1 : 2);
    }
    return cache.buffer[offset];
  }

  if(regs.pbr <= 0x5f) {
    //$00-5f:0000-ffff ROM
    syncROMBuffer();
    step(regs.clsr ? 5 : 6);
    return read(regs.pbr << 16 | addr);
  } else {
    //$60-7f:0000-ffff RAM
    syncRAMBuffer();
    step(regs.clsr ? 5 : 6);
    return read(regs.pbr << 16 | addr);
  }
}

uint8_t SuperFX::peekpipe() {
  uint8_t result = regs.pipeline;
  regs.pipeline = readOpcode(regs.r[15]);
  regs.r[15].modified = false;
  return result;
}

uint8_t SuperFX::pipe() {
  uint8_t result = regs.pipeline;
  regs.pipeline = readOpcode(++regs.r[15]);
  regs.r[15].modified = false;
  return result;
}

void SuperFX::flushCache() {
  for(unsigned n = 0; n < 32; ++n) cache.valid[n] = false;
}

uint8_t SuperFX::readCache(uint16_t addr) {
  addr = (addr + regs.cbr) & 511;
  return cache.buffer[addr];
}

void SuperFX::writeCache(uint16_t addr, uint8_t data) {
  addr = (addr + regs.cbr) & 511;
  cache.buffer[addr] = data;
  if((addr & 15) == 15) cache.valid[addr >> 4] = true;
}

uint8_t SuperFX::readIO(unsigned addr, uint8_t) {
  cpu.synchronizeCoprocessors();
  addr = 0x3000 | (addr & 0x3ff);

  if(addr >= 0x3100 && addr <= 0x32ff) {
    return readCache(addr - 0x3100);
  }

  if(addr >= 0x3000 && addr <= 0x301f) {
    return regs.r[(addr >> 1) & 15] >> ((addr & 1) << 3);
  }

  switch(addr) {
  case 0x3030: {
    return regs.sfr >> 0;
  }

  case 0x3031: {
    uint8_t r = regs.sfr >> 8;
    regs.sfr.irq = 0;
    cpu.irq(0);
    return r;
  }

  case 0x3034: {
    return regs.pbr;
  }

  case 0x3036: {
    return regs.rombr;
  }

  case 0x303b: {
    return regs.vcr;
  }

  case 0x303c: {
    return regs.rambr;
  }

  case 0x303e: {
    return regs.cbr >> 0;
  }

  case 0x303f: {
    return regs.cbr >> 8;
  }
  }

  return 0x00;
}

void SuperFX::writeIO(unsigned addr, uint8_t data) {
  cpu.synchronizeCoprocessors();
  addr = 0x3000 | (addr & 0x3ff);

  if(addr >= 0x3100 && addr <= 0x32ff) {
    return writeCache(addr - 0x3100, data);
  }

  if(addr >= 0x3000 && addr <= 0x301f) {
    unsigned n = (addr >> 1) & 15;
    if((addr & 1) == 0) {
      regs.r[n] = (regs.r[n] & 0xff00) | data;
    } else {
      regs.r[n] = (data << 8) | (regs.r[n] & 0xff);
    }
    if(n == 14) updateROMBuffer();

    if(addr == 0x301f) regs.sfr.g = 1;
    return;
  }

  switch(addr) {
  case 0x3030: {
    bool g = regs.sfr.g;
    regs.sfr = (regs.sfr & 0xff00) | (data << 0);
    if(g == 1 && regs.sfr.g == 0) {
      regs.cbr = 0x0000;
      flushCache();
    }
  } break;

  case 0x3031: {
    regs.sfr = (data << 8) | (regs.sfr & 0x00ff);
  } break;

  case 0x3033: {
    regs.bramr = data & 0x01;
  } break;

  case 0x3034: {
    regs.pbr = data & 0x7f;
    flushCache();
  } break;

  case 0x3037: {
    regs.cfgr = data;
  } break;

  case 0x3038: {
    regs.scbr = data;
  } break;

  case 0x3039: {
    regs.clsr = data & 0x01;
  } break;

  case 0x303a: {
    regs.scmr = data;
  } break;
  }
}

void SuperFX::step(unsigned clocks) {
  if(regs.romcl) {
    regs.romcl -= std::min(clocks, regs.romcl);
    if(regs.romcl == 0) {
      regs.sfr.r = 0;
      regs.romdr = read((regs.rombr << 16) + regs.r[14]);
    }
  }

  if(regs.ramcl) {
    regs.ramcl -= std::min(clocks, regs.ramcl);
    if(regs.ramcl == 0) {
      write(0x700000 + (regs.rambr << 16) + regs.ramar, regs.ramdr);
    }
  }

  clock += clocks * (uint64_t)cpu.frequency;
  synchronizeCPU();
}

void SuperFX::syncROMBuffer() {
  if(regs.romcl) step(regs.romcl);
}

uint8_t SuperFX::readROMBuffer() {
  syncROMBuffer();
  return regs.romdr;
}

void SuperFX::updateROMBuffer() {
  regs.sfr.r = 1;
  regs.romcl = regs.clsr ? 5 : 6;
}

void SuperFX::syncRAMBuffer() {
  if(regs.ramcl) step(regs.ramcl);
}

uint8_t SuperFX::readRAMBuffer(uint16_t addr) {
  syncRAMBuffer();
  return read(0x700000 + (regs.rambr << 16) + addr);
}

void SuperFX::writeRAMBuffer(uint16_t addr, uint8_t data) {
  syncRAMBuffer();
  regs.ramcl = regs.clsr ? 5 : 6;
  regs.ramar = addr;
  regs.ramdr = data;
}

void SuperFX::serialize(serializer& s) {
  GSU::serialize(s);
  Thread::serialize(s);

  s.array(ram.data(), ram.size());
}

SuperFX superfx;

void SuperFX::synchronizeCPU() {
  if(clock >= 0) scheduler.resume(cpu.thread);
}

void SuperFX::Enter() {
  while(true) {
    scheduler.synchronize();
    superfx.main();
  }
}

void SuperFX::main() {
  if(regs.sfr.g == 0) return step(6);

  instruction(peekpipe());

  if(regs.r[14].modified) {
    regs.r[14].modified = false;
    updateROMBuffer();
  }

  if(regs.r[15].modified) {
    regs.r[15].modified = false;
  } else {
    regs.r[15]++;
  }
}

void SuperFX::unload() {
  rom.reset();
  ram.reset();
}

void SuperFX::power() {
  double overclock = std::max(1.0, std::min(8.0, configuration.superfx.overclock / 100.0));

  GSU::power();
  create(SuperFX::Enter, Frequency * overclock);

  romMask = rom.size() - 1;
  ramMask = ram.size() - 1;

  for(unsigned n = 0; n < 512; ++n) cache.buffer[n] = 0x00;
  for(unsigned n = 0; n < 32; ++n) cache.valid[n] = false;
  for(unsigned n = 0; n < 2; ++n) {
    pixelcache[n].offset = ~0;
    pixelcache[n].bitpend = 0x00;
  }

  regs.romcl = 0;
  regs.romdr = 0;

  regs.ramcl = 0;
  regs.ramar = 0;
  regs.ramdr = 0;
}

}
