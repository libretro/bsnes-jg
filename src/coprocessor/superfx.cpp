#include "../sfc.hpp"

namespace SuperFamicom {

//ROM / RAM access from the S-CPU

auto SuperFX::CPUROM::data() -> uint8_t* {
  return superfx.rom.data();
}

auto SuperFX::CPUROM::size() const -> unsigned {
  return superfx.rom.size();
}

auto SuperFX::CPUROM::read(unsigned addr, uint8_t data) -> uint8_t {
  if(superfx.regs.sfr.g && superfx.regs.scmr.ron) {
    static const uint8_t vector[16] = {
      0x00, 0x01, 0x00, 0x01, 0x04, 0x01, 0x00, 0x01,
      0x00, 0x01, 0x08, 0x01, 0x00, 0x01, 0x0c, 0x01,
    };
    return vector[addr & 15];
  }
  return superfx.rom.read(addr, data);
}

auto SuperFX::CPUROM::write(unsigned addr, uint8_t data) -> void {
  superfx.rom.write(addr, data);
}

auto SuperFX::CPURAM::data() -> uint8_t* {
  return superfx.ram.data();
}

auto SuperFX::CPURAM::size() const -> unsigned {
  return superfx.ram.size();
}

auto SuperFX::CPURAM::read(unsigned addr, uint8_t data) -> uint8_t {
  if(superfx.regs.sfr.g && superfx.regs.scmr.ran) return data;
  return superfx.ram.read(addr, data);
}

auto SuperFX::CPURAM::write(unsigned addr, uint8_t data) -> void {
  superfx.ram.write(addr, data);
}

auto SuperFX::stop() -> void {
  cpu.irq(1);
}

auto SuperFX::color(uint8_t source) -> uint8_t {
  if(regs.por.highnibble) return (regs.colr & 0xf0) | (source >> 4);
  if(regs.por.freezehigh) return (regs.colr & 0xf0) | (source & 0x0f);
  return source;
}

auto SuperFX::plot(uint8_t x, uint8_t y) -> void {
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

auto SuperFX::rpix(uint8_t x, uint8_t y) -> uint8_t {
  flushPixelCache(pixelcache[1]);
  flushPixelCache(pixelcache[0]);

  unsigned cn;  //character number
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

  for(unsigned n : range(bpp)) {
    unsigned byte = ((n >> 1) << 4) + (n & 1);  // = [n]{ 0, 1, 16, 17, 32, 33, 48, 49 };
    step(regs.clsr ? 5 : 6);
    data |= ((read(addr + byte) >> x) & 1) << n;
  }

  return data;
}

auto SuperFX::flushPixelCache(PixelCache& cache) -> void {
  if(cache.bitpend == 0x00) return;

  uint8_t x = cache.offset << 3;
  uint8_t y = cache.offset >> 5;

  unsigned cn;  //character number
  switch(regs.por.obj ? 3 : regs.scmr.ht) {
  case 0: cn = ((x & 0xf8) << 1) + ((y & 0xf8) >> 3); break;
  case 1: cn = ((x & 0xf8) << 1) + ((x & 0xf8) >> 1) + ((y & 0xf8) >> 3); break;
  case 2: cn = ((x & 0xf8) << 1) + ((x & 0xf8) << 0) + ((y & 0xf8) >> 3); break;
  case 3: cn = ((y & 0x80) << 2) + ((x & 0x80) << 1) + ((y & 0x78) << 1) + ((x & 0x78) >> 3); break;
  }
  unsigned bpp = 2 << (regs.scmr.md - (regs.scmr.md >> 1));  // = [regs.scmr.md]{ 2, 4, 4, 8 };
  unsigned addr = 0x700000 + (cn * (bpp << 3)) + (regs.scbr << 10) + ((y & 0x07) * 2);

  for(unsigned n : range(bpp)) {
    unsigned byte = ((n >> 1) << 4) + (n & 1);  // = [n]{ 0, 1, 16, 17, 32, 33, 48, 49 };
    uint8_t data = 0x00;
    for(unsigned x : range(8)) data |= ((cache.data[x] >> n) & 1) << x;
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

auto SuperFX::read(unsigned addr, uint8_t data) -> uint8_t {
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

auto SuperFX::write(unsigned addr, uint8_t data) -> void {
  if((addr & 0xe00000) == 0x600000) {  //$60-7f:0000-ffff
    while(!regs.scmr.ran) {
      step(6);
      synchronizeCPU();
      if(synchronizing()) break;
    }
    return ram.write(addr & ramMask, data);
  }
}

auto SuperFX::readOpcode(uint16_t addr) -> uint8_t {
  uint16_t offset = addr - regs.cbr;
  if(offset < 512) {
    if(cache.valid[offset >> 4] == false) {
      unsigned dp = offset & 0xfff0;
      unsigned sp = (regs.pbr << 16) + ((regs.cbr + dp) & 0xfff0);
      for(unsigned n : range(16)) {
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

auto SuperFX::peekpipe() -> uint8_t {
  uint8_t result = regs.pipeline;
  regs.pipeline = readOpcode(regs.r[15]);
  regs.r[15].modified = false;
  return result;
}

auto SuperFX::pipe() -> uint8_t {
  uint8_t result = regs.pipeline;
  regs.pipeline = readOpcode(++regs.r[15]);
  regs.r[15].modified = false;
  return result;
}

auto SuperFX::flushCache() -> void {
  for(unsigned n : range(32)) cache.valid[n] = false;
}

auto SuperFX::readCache(uint16_t addr) -> uint8_t {
  addr = (addr + regs.cbr) & 511;
  return cache.buffer[addr];
}

auto SuperFX::writeCache(uint16_t addr, uint8_t data) -> void {
  addr = (addr + regs.cbr) & 511;
  cache.buffer[addr] = data;
  if((addr & 15) == 15) cache.valid[addr >> 4] = true;
}

auto SuperFX::readIO(unsigned addr, uint8_t) -> uint8_t {
  cpu.synchronizeCoprocessors();
  addr = 0x3000 | addr & 0x3ff;

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

auto SuperFX::writeIO(unsigned addr, uint8_t data) -> void {
  cpu.synchronizeCoprocessors();
  addr = 0x3000 | addr & 0x3ff;

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

auto SuperFX::step(unsigned clocks) -> void {
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

auto SuperFX::syncROMBuffer() -> void {
  if(regs.romcl) step(regs.romcl);
}

auto SuperFX::readROMBuffer() -> uint8_t {
  syncROMBuffer();
  return regs.romdr;
}

auto SuperFX::updateROMBuffer() -> void {
  regs.sfr.r = 1;
  regs.romcl = regs.clsr ? 5 : 6;
}

auto SuperFX::syncRAMBuffer() -> void {
  if(regs.ramcl) step(regs.ramcl);
}

auto SuperFX::readRAMBuffer(uint16_t addr) -> uint8_t {
  syncRAMBuffer();
  return read(0x700000 + (regs.rambr << 16) + addr);
}

auto SuperFX::writeRAMBuffer(uint16_t addr, uint8_t data) -> void {
  syncRAMBuffer();
  regs.ramcl = regs.clsr ? 5 : 6;
  regs.ramar = addr;
  regs.ramdr = data;
}

auto SuperFX::serialize(serializer& s) -> void {
  GSU::serialize(s);
  Thread::serialize(s);

  s.array(ram.data(), ram.size());
}

SuperFX superfx;

auto SuperFX::synchronizeCPU() -> void {
  if(clock >= 0) scheduler.resume(cpu.thread);
}

auto SuperFX::Enter() -> void {
  while(true) {
    scheduler.synchronize();
    superfx.main();
  }
}

auto SuperFX::main() -> void {
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

auto SuperFX::unload() -> void {
  rom.reset();
  ram.reset();
}

auto SuperFX::power() -> void {
  double overclock = std::max(1.0, std::min(8.0, configuration.hacks.superfx.overclock / 100.0));

  GSU::power();
  create(SuperFX::Enter, Frequency * overclock);

  romMask = rom.size() - 1;
  ramMask = ram.size() - 1;

  for(unsigned n : range(512)) cache.buffer[n] = 0x00;
  for(unsigned n : range(32)) cache.valid[n] = false;
  for(unsigned n : range(2)) {
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