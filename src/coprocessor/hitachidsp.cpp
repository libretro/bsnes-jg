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

#include "serializer.hpp"
#include "cpu.hpp"

#include "hitachidsp.hpp"

namespace SuperFamicom {

bool HitachiDSP::isROM(unsigned address) {
  return (bool)addressROM(address);
}

bool HitachiDSP::isRAM(unsigned address) {
  return (bool)addressRAM(address);
}

uint8_t HitachiDSP::read(unsigned address) {
  unsigned addr = address;
  if(addressROM(addr)) return readROM(addr);

  addr = address;
  if(addressRAM(addr)) return readRAM(addr);

  addr = address;
  if(addressDRAM(addr)) return readDRAM(addr);

  addr = address;
  if(addressIO(addr)) return readIO(addr);
  return 0x00;
}

void HitachiDSP::write(unsigned address, uint8_t data) {
  unsigned addr = address;
  if(addressROM(addr)) return writeROM (addr, data);

  addr = address;
  if(addressRAM(addr)) return writeRAM(addr, data);

  addr = address;
  if(addressDRAM(addr)) return writeDRAM(addr, data);

  addr = address;
  if(addressIO(addr)) return writeIO(addr, data);
}

bool HitachiDSP::addressROM(unsigned& address) const {
  if(Mapping == 0) {
    //00-3f,80-bf:8000-ffff; c0-ff:0000-ffff
    if((address & 0x408000) == 0x008000 || (address & 0xc00000) == 0xc00000) {
      address = (address & 0x3f0000) >> 1 | (address & 0x7fff);
      address &= 0x1fffff;
      return true;
    }
  } else {
    //00-3f,80-bf:8000-ffff; c0-ff:0000-ffff
    if((address & 0x408000) == 0x008000 || (address & 0xc00000) == 0xc00000) {
      address &= 0x3fffff;
      return true;
    }
  }
  return false;
}

uint8_t HitachiDSP::readROM(unsigned address, uint8_t data) {
  if(hitachidsp.active() || !busy()) {
    address = bus.mirror(address, rom.size());
  //if(Roms == 2 && mmio.r1f52 == 1 && address >= (nall::round(rom.size()) >> 1)) return 0x00;
    return rom.read(address, data);
  }
  //DSP has the bus acquired: CPU reads from 00:ffc0-ffff return IO registers (including reset vector overrides)
  if(Mapping == 0 && (address & 0xbfffc0) == 0x007fc0) return readIO(0x7f40 | (address & 0x3f));
  if(Mapping == 1 && (address & 0xbfffc0) == 0x00ffc0) return readIO(0x7f40 | (address & 0x3f));
  return data;
}

void HitachiDSP::writeROM(unsigned address, uint8_t data) {
    if (address || data) {}
}

bool HitachiDSP::addressRAM(unsigned& address) const {
  if(Mapping == 0) {
    //70-77:0000-7fff
    if((address & 0xf88000) == 0x700000) {
      address = (address & 0x070000) >> 1 | (address & 0x7fff);
      address &= 0x03ffff;
      return true;
    }
  } else {
    //30-3f,b0-bf:6000-7fff
    if((address & 0x70e000) == 0x306000) {
      address = (address & 0x0f0000) >> 3 | (address & 0x1fff);
      address &= 0x01ffff;
      return true;
    }
  }
  return false;
}

uint8_t HitachiDSP::readRAM(unsigned address, uint8_t data) {
  if(ram.size() == 0) return 0x00;  //not open bus
  return ram.read(bus.mirror(address, ram.size()), data);
}

void HitachiDSP::writeRAM(unsigned address, uint8_t data) {
  if(ram.size() == 0) return;
  return ram.write(bus.mirror(address, ram.size()), data);
}

bool HitachiDSP::addressDRAM(unsigned& address) const {
  if(Mapping == 0) {
    //00-3f,80-bf:6000-6bff,7000-7bff
    if((address & 0x40e000) == 0x006000 && (address & 0x0c00) != 0x0c00) {
      address &= 0x0fff;
      return true;
    }
  } else {
    //00-2f,80-af:6000-6bff,7000-7bff
    if((address & 0x40e000) == 0x006000 && (address & 0x0c00) != 0x0c00 && (address & 0x300000) != 0x300000) {
      address &= 0x0fff;
      return true;
    }
  }
  return false;
}

uint8_t HitachiDSP::readDRAM(unsigned address, uint8_t data) {
  address &= 0xfff;
  if(address >= 0xc00) return data;
  return dataRAM[address];
}

void HitachiDSP::writeDRAM(unsigned address, uint8_t data) {
  address &= 0xfff;
  if(address >= 0xc00) return;
  dataRAM[address] = data;
}

bool HitachiDSP::addressIO(unsigned& address) const {
  if(Mapping == 0) {
    //00-3f,80-bf:6c00-6fff,7c00-7fff
    if((address & 0x40ec00) == 0x006c00) {
      address &= 0x03ff;
      return true;
    }
  } else {
    //00-2f,80-af:6c00-6fff,7c00-7fff
    if((address & 0x40ec00) == 0x006c00 && (address & 0x300000) != 0x300000) {
      address &= 0x03ff;
      return true;
    }
  }
  return false;
}

uint8_t HitachiDSP::readIO(unsigned address, uint8_t data) {
  if (data) {}
  address = 0x7c00 | (address & 0x03ff);

  //IO
  switch(address) {
  case 0x7f40: return io.dma.source >>  0;
  case 0x7f41: return io.dma.source >>  8;
  case 0x7f42: return io.dma.source >> 16;
  case 0x7f43: return io.dma.length >>  0;
  case 0x7f44: return io.dma.length >>  8;
  case 0x7f45: return io.dma.target >>  0;
  case 0x7f46: return io.dma.target >>  8;
  case 0x7f47: return io.dma.target >> 16;
  case 0x7f48: return io.cache.page;
  case 0x7f49: return io.cache.base >>  0;
  case 0x7f4a: return io.cache.base >>  8;
  case 0x7f4b: return io.cache.base >> 16;
  case 0x7f4c: return io.cache.lock[0] << 0 | io.cache.lock[1] << 1;
  case 0x7f4d: return io.cache.pb >> 0;
  case 0x7f4e: return io.cache.pb >> 8;
  case 0x7f4f: return io.cache.pc;
  case 0x7f50: return io.wait.ram << 0 | io.wait.rom << 4;
  case 0x7f51: return io.irq;
  case 0x7f52: return io.rom;
  case 0x7f53: case 0x7f54: case 0x7f55: case 0x7f56: case 0x7f57:
  case 0x7f59: case 0x7f5b: case 0x7f5c: case 0x7f5d: case 0x7f5e:
  case 0x7f5f: return io.suspend.enable << 0 | r.i << 1 | running() << 6 | busy() << 7;
  }

  //vectors
  if(address >= 0x7f60 && address <= 0x7f7f) {
    return io.vector[address & 0x1f];
  }

  //registers
  if((address >= 0x7f80 && address <= 0x7faf) || (address >= 0x7fc0 && address <= 0x7fef)) {
    address &= 0x3f;
    switch(address % 3) {
    case 0: return r.gpr[address / 3] >>  0;
    case 1: return r.gpr[address / 3] >>  8;
    case 2: return r.gpr[address / 3] >> 16;
    }
  }

  return 0x00;
}

void HitachiDSP::writeIO(unsigned address, uint8_t data) {
  address = 0x7c00 | (address & 0x03ff);

  //IO
  switch(address) {
  case 0x7f40: io.dma.source = (io.dma.source & 0xffff00) | data <<  0; return;
  case 0x7f41: io.dma.source = (io.dma.source & 0xff00ff) | data <<  8; return;
  case 0x7f42: io.dma.source = (io.dma.source & 0x00ffff) | data << 16; return;

  case 0x7f43: io.dma.length = (io.dma.length & 0xff00) | data << 0; return;
  case 0x7f44: io.dma.length = (io.dma.length & 0x00ff) | data << 8; return;

  case 0x7f45: io.dma.target = (io.dma.target & 0xffff00) | data <<  0; return;
  case 0x7f46: io.dma.target = (io.dma.target & 0xff00ff) | data <<  8; return;
  case 0x7f47: io.dma.target = (io.dma.target & 0x00ffff) | data << 16;
    if(io.halt) io.dma.enable = 1;
    return;

  case 0x7f48:
    io.cache.page = data & 1;
    if(io.halt) io.cache.enable = 1;
    return;

  case 0x7f49: io.cache.base = (io.cache.base & 0xffff00) | data <<  0; return;
  case 0x7f4a: io.cache.base = (io.cache.base & 0xff00ff) | data <<  8; return;
  case 0x7f4b: io.cache.base = (io.cache.base & 0x00ffff) | data << 16; return;

  case 0x7f4c:
    io.cache.lock[0] = bool(data & 1);
    io.cache.lock[1] = bool(data & 2);
    return;

  case 0x7f4d: io.cache.pb = (io.cache.pb & 0xff00) | data << 0; return;
  case 0x7f4e: io.cache.pb = ((io.cache.pb & 0x00ff) | data << 8) & 0x7fff; return;

  case 0x7f4f:
    io.cache.pc = data;
    if(io.halt) {
      io.halt = 0;
      r.pb = io.cache.pb;
      r.pc = io.cache.pc;
    }
    return;

  case 0x7f50:
    io.wait.ram = data >> 0 & 7;
    io.wait.rom = data >> 4 & 7;
    return;

  case 0x7f51:
    io.irq = data & 1;
    if(io.irq == 1) cpu.irq(r.i = 0);
    return;

  case 0x7f52:
    io.rom = data & 1;
    return;

  case 0x7f53:
    io.lock = 0;
    io.halt = 1;
    return;

  case 0x7f55: io.suspend.enable = 1; io.suspend.duration =   0; return;  //indefinite
  case 0x7f56: io.suspend.enable = 1; io.suspend.duration =  32; return;
  case 0x7f57: io.suspend.enable = 1; io.suspend.duration =  64; return;
  case 0x7f58: io.suspend.enable = 1; io.suspend.duration =  96; return;
  case 0x7f59: io.suspend.enable = 1; io.suspend.duration = 128; return;
  case 0x7f5a: io.suspend.enable = 1; io.suspend.duration = 160; return;
  case 0x7f5b: io.suspend.enable = 1; io.suspend.duration = 192; return;
  case 0x7f5c: io.suspend.enable = 1; io.suspend.duration = 224; return;
  case 0x7f5d: io.suspend.enable = 0; return;  //resume

  case 0x7f5e:
    r.i = 0;  //does not deassert CPU IRQ line
    return;
  }

  //vectors
  if(address >= 0x7f60 && address <= 0x7f7f) {
    io.vector[address & 0x1f] = data;
    return;
  }

  //registers
  if((address >= 0x7f80 && address <= 0x7faf) || (address >= 0x7fc0 && address <= 0x7fef)) {
    address &= 0x3f;
    switch(address % 3) {
    case 0: r.gpr[address / 3] = (r.gpr[address / 3] & 0xffff00) | data <<  0; break;
    case 1: r.gpr[address / 3] = (r.gpr[address / 3] & 0xff00ff) | data <<  8; break;
    case 2: r.gpr[address / 3] = (r.gpr[address / 3] & 0x00ffff) | data << 16; break;
    }
    return;
  }
}

std::vector<uint8_t> HitachiDSP::firmware() const {
  std::vector<uint8_t> buffer;
  buffer.reserve(1024 * 3);
  for(unsigned n = 0; n < 1024; ++n) {
    buffer.push_back(dataROM[n] >>  0);
    buffer.push_back(dataROM[n] >>  8);
    buffer.push_back(dataROM[n] >> 16);
  }
  return buffer;
}

void HitachiDSP::serialize(serializer& s) {
  HG51B::serialize(s);
  Thread::serialize(s);
}

/* note: this is not copyrightable, as it is purely math tables such as sin and cos */

const uint8_t HitachiDSP::staticDataROM[3072] = {
  255,255,255,0,0,128,0,0,64,170,170,42,0,0,32,153,153,25,85,85,21,36,73,18,0,0,16,227,56,14,204,204,
  12,232,162,11,170,170,10,157,216,9,146,36,9,136,136,8,0,0,8,135,135,7,113,28,7,161,188,6,102,102,6,97,
  24,6,116,209,5,178,144,5,85,85,5,184,30,5,78,236,4,161,189,4,73,146,4,238,105,4,68,68,4,8,33,4,
  0,0,4,248,224,3,195,195,3,58,168,3,56,142,3,159,117,3,80,94,3,52,72,3,51,51,3,56,31,3,48,12,
  3,11,250,2,186,232,2,45,216,2,89,200,2,49,185,2,170,170,2,188,156,2,92,143,2,130,130,2,39,118,2,67,
  106,2,208,94,2,200,83,2,36,73,2,224,62,2,247,52,2,99,43,2,34,34,2,46,25,2,132,16,2,32,8,2,
  0,0,2,31,248,1,124,240,1,19,233,1,225,225,1,230,218,1,29,212,1,133,205,1,28,199,1,224,192,1,207,186,
  1,232,180,1,40,175,1,142,169,1,26,164,1,200,158,1,153,153,1,139,148,1,156,143,1,203,138,1,24,134,1,129,
  129,1,5,125,1,164,120,1,93,116,1,46,112,1,22,108,1,22,104,1,44,100,1,88,96,1,152,92,1,237,88,1,
  85,85,1,208,81,1,94,78,1,253,74,1,174,71,1,111,68,1,65,65,1,34,62,1,19,59,1,19,56,1,33,53,
  1,62,50,1,104,47,1,159,44,1,228,41,1,53,39,1,146,36,1,251,33,1,112,31,1,240,28,1,123,26,1,17,
  24,1,177,21,1,92,19,1,17,17,1,207,14,1,151,12,1,104,10,1,66,8,1,36,6,1,16,4,1,4,2,1,
  0,0,1,3,254,0,15,252,0,35,250,0,62,248,0,96,246,0,137,244,0,185,242,0,240,240,0,46,239,0,115,237,
  0,189,235,0,14,234,0,101,232,0,194,230,0,37,229,0,142,227,0,252,225,0,112,224,0,233,222,0,103,221,0,235,
  219,0,116,218,0,1,217,0,148,215,0,43,214,0,199,212,0,104,211,0,13,210,0,182,208,0,100,207,0,22,206,0,
  204,204,0,135,203,0,69,202,0,7,201,0,206,199,0,152,198,0,101,197,0,55,196,0,12,195,0,228,193,0,192,192,
  0,160,191,0,130,190,0,105,189,0,82,188,0,62,187,0,46,186,0,33,185,0,23,184,0,15,183,0,11,182,0,9,
  181,0,11,180,0,15,179,0,22,178,0,31,177,0,44,176,0,58,175,0,76,174,0,96,173,0,118,172,0,143,171,0,
  170,170,0,200,169,0,232,168,0,10,168,0,47,167,0,85,166,0,126,165,0,169,164,0,215,163,0,6,163,0,55,162,
  0,107,161,0,160,160,0,216,159,0,17,159,0,76,158,0,137,157,0,200,156,0,9,156,0,76,155,0,144,154,0,215,
  153,0,31,153,0,104,152,0,180,151,0,1,151,0,79,150,0,160,149,0,242,148,0,69,148,0,154,147,0,241,146,0,
  73,146,0,162,145,0,253,144,0,90,144,0,184,143,0,23,143,0,120,142,0,218,141,0,61,141,0,162,140,0,8,140,
  0,112,139,0,216,138,0,66,138,0,174,137,0,26,137,0,136,136,0,247,135,0,103,135,0,217,134,0,75,134,0,191,
  133,0,52,133,0,169,132,0,33,132,0,153,131,0,18,131,0,140,130,0,8,130,0,132,129,0,2,129,0,128,128,0,
  0,0,0,0,0,16,158,160,22,122,182,27,0,0,32,239,198,35,28,49,39,255,84,42,60,65,45,0,0,48,176,152,
  50,229,16,53,245,108,55,86,176,57,212,221,59,189,247,61,0,0,64,61,248,65,219,225,67,12,190,69,222,141,71,58,
  82,73,241,11,75,185,187,76,56,98,78,0,0,80,149,149,81,112,35,83,254,169,84,162,41,86,183,162,87,144,21,89,
  121,130,90,186,233,91,148,75,93,67,168,94,0,0,96,254,82,97,112,161,98,131,235,99,96,49,101,50,115,102,29,177,
  103,68,235,104,202,33,106,205,84,107,108,132,108,194,176,109,235,217,110,0,0,112,24,35,113,74,67,114,173,96,115,84,
  123,116,84,147,117,191,168,118,168,187,119,31,204,120,52,218,121,249,229,122,122,239,123,200,246,124,239,251,125,253,254,126,
  0,0,128,1,255,128,15,252,129,52,247,130,123,240,131,238,231,132,152,221,133,130,209,134,182,195,135,61,180,136,31,163,
  137,102,144,138,25,124,139,65,102,140,228,78,141,11,54,142,188,27,143,0,0,144,219,226,144,86,196,145,117,164,146,65,
  131,147,189,96,148,241,60,149,226,23,150,150,241,150,17,202,151,89,161,152,115,119,153,100,76,154,49,32,155,222,242,155,
  112,196,156,235,148,157,84,100,158,175,50,159,0,0,160,74,204,160,146,151,161,220,97,162,42,43,163,130,243,163,230,186,
  164,90,129,165,225,70,166,126,11,167,53,207,167,9,146,168,253,83,169,19,21,170,80,213,170,180,148,171,69,83,172,3,
  17,173,242,205,173,21,138,174,110,69,175,0,0,176,204,185,176,214,114,177,32,43,178,172,226,178,124,153,179,147,79,180,
  243,4,181,157,185,181,149,109,182,220,32,183,117,211,183,96,133,184,160,54,185,56,231,185,40,151,186,115,70,187,26,245,
  187,32,163,188,134,80,189,78,253,189,121,169,190,9,85,191,0,0,192,95,170,192,40,84,193,92,253,193,253,165,194,13,
  78,195,140,245,195,125,156,196,225,66,197,184,232,197,5,142,198,201,50,199,6,215,199,187,122,200,235,29,201,152,192,201,
  193,98,202,105,4,203,145,165,203,58,70,204,100,230,204,18,134,205,68,37,206,252,195,206,58,98,207,0,0,208,78,157,
  208,38,58,209,137,214,209,119,114,210,243,13,211,252,168,211,148,67,212,188,221,212,116,119,213,190,16,214,155,169,214,11,
  66,215,15,218,215,169,113,216,216,8,217,159,159,217,254,53,218,245,203,218,133,97,219,176,246,219,118,139,220,216,31,221,
  215,179,221,115,71,222,173,218,222,134,109,223,0,0,224,25,146,224,212,35,225,48,181,225,48,70,226,210,214,226,25,103,
  227,4,247,227,148,134,228,203,21,229,168,164,229,45,51,230,90,193,230,47,79,231,173,220,231,214,105,232,169,246,232,38,
  131,233,80,15,234,38,155,234,168,38,235,217,177,235,183,60,236,67,199,236,127,81,237,106,219,237,6,101,238,82,238,238,
  80,119,239,0,0,240,97,136,240,118,16,241,62,152,241,186,31,242,234,166,242,207,45,243,105,180,243,185,58,244,192,192,
  244,125,70,245,242,203,245,30,81,246,2,214,246,159,90,247,245,222,247,5,99,248,206,230,248,82,106,249,144,237,249,138,
  112,250,63,243,250,177,117,251,223,247,251,202,121,252,114,251,252,216,124,253,251,253,253,222,126,254,127,255,254,223,127,255,
  0,0,0,58,36,3,85,72,6,50,108,9,178,143,12,183,178,15,32,213,18,208,246,21,166,23,25,133,55,28,78,86,
  31,225,115,34,32,144,37,237,170,40,40,196,43,179,219,46,112,241,49,64,5,53,4,23,56,159,38,59,242,51,62,224,
  62,65,73,71,68,16,77,71,24,80,74,67,80,77,114,77,80,137,71,83,105,62,86,247,49,89,20,34,92,164,14,95,
  138,247,97,169,220,100,229,189,103,32,155,106,64,116,109,39,73,112,186,25,115,221,229,117,116,173,120,101,112,123,147,46,
  126,228,231,128,60,156,131,130,75,134,154,245,136,107,154,139,217,57,142,204,211,144,42,104,147,217,246,149,191,127,152,197,
  2,155,209,127,157,202,246,159,153,103,162,36,210,164,85,54,167,20,148,169,73,235,171,221,59,174,186,133,176,201,200,178,
  243,4,181,34,58,183,65,104,185,58,143,187,249,174,189,103,199,191,112,216,193,0,226,195,3,228,197,101,222,199,18,209,
  201,247,187,203,2,159,205,31,122,207,61,77,209,72,24,211,49,219,212,228,149,214,82,72,216,105,242,217,26,148,219,83,
  45,221,5,190,222,33,70,224,151,197,225,89,60,227,89,170,228,135,15,230,215,107,231,59,191,232,166,9,234,11,75,235,
  94,131,236,147,178,237,157,216,238,115,245,239,8,9,241,82,19,242,71,20,243,221,11,244,10,250,244,198,222,245,7,186,
  246,197,139,247,247,83,248,151,18,249,157,199,249,1,115,250,190,20,251,205,172,251,39,59,252,201,191,252,171,58,253,203,
  171,253,35,19,254,175,112,254,109,196,254,87,14,255,109,78,255,171,132,255,15,177,255,151,211,255,67,236,255,16,251,255,
  0,0,0,249,162,0,246,69,1,248,232,1,1,140,2,20,47,3,52,210,3,100,117,4,165,24,5,251,187,5,104,95,
  6,239,2,7,146,166,7,84,74,8,56,238,8,64,146,9,110,54,10,199,218,10,76,127,11,1,36,12,231,200,12,2,
  110,13,85,19,14,227,184,14,174,94,15,185,4,16,8,171,16,158,81,17,125,248,17,169,159,18,37,71,19,244,238,19,
  25,151,20,153,63,21,117,232,21,178,145,22,83,59,23,92,229,23,209,143,24,180,58,25,10,230,25,216,145,26,32,62,
  27,231,234,27,49,152,28,2,70,29,95,244,29,76,163,30,206,82,31,234,2,32,163,179,32,0,101,33,5,23,34,184,
  201,34,30,125,35,60,49,36,24,230,36,185,155,37,36,82,38,95,9,39,113,193,39,97,122,40,54,52,41,246,238,41,
  170,170,42,89,103,43,10,37,44,199,227,44,152,163,45,133,100,46,153,38,47,220,233,47,89,174,48,27,116,49,44,59,
  50,152,3,51,107,205,51,177,152,52,120,101,53,206,51,54,193,3,55,96,213,55,187,168,56,228,125,57,236,84,58,230,
  45,59,230,8,60,1,230,60,77,197,61,227,166,62,220,138,63,82,113,64,98,90,65,44,70,66,208,52,67,113,38,68,
  55,27,69,74,19,70,214,14,71,12,14,72,32,17,73,76,24,74,205,35,75,234,51,76,236,72,77,39,99,78,249,130,
  79,201,168,80,10,213,81,63,8,83,252,66,84,234,133,85,204,209,86,130,39,88,21,136,89,188,244,90,237,110,92,108,
  248,93,105,147,95,163,66,97,165,9,99,30,237,100,129,243,102,23,38,105,34,147,107,165,82,110,124,147,113,180,206,117,
  0,0,0,36,3,0,72,6,0,109,9,0,147,12,0,186,15,0,226,18,0,11,22,0,54,25,0,99,28,0,147,31,
  0,196,34,0,249,37,0,48,41,0,107,44,0,169,47,0,235,50,0,50,54,0,124,57,0,203,60,0,31,64,0,121,
  67,0,216,70,0,61,74,0,168,77,0,25,81,0,146,84,0,17,88,0,153,91,0,40,95,0,192,98,0,96,102,0,
  9,106,0,188,109,0,122,113,0,65,117,0,20,121,0,242,124,0,220,128,0,210,132,0,213,136,0,230,140,0,5,145,
  0,51,149,0,112,153,0,190,157,0,28,162,0,139,166,0,13,171,0,162,175,0,75,180,0,9,185,0,220,189,0,198,
  194,0,200,199,0,227,204,0,24,210,0,103,215,0,211,220,0,93,226,0,6,232,0,207,237,0,187,243,0,202,249,0,
  0,0,1,92,6,1,226,12,1,148,19,1,115,26,1,131,33,1,198,40,1,62,48,1,239,55,1,220,63,1,8,72,
  1,119,80,1,45,89,1,45,98,1,125,107,1,34,117,1,33,127,1,128,137,1,68,148,1,118,159,1,28,171,1,62,
  183,1,231,195,1,31,209,1,241,222,1,105,237,1,149,252,1,131,12,2,68,29,2,233,46,2,134,65,2,51,85,2,
  9,106,2,37,128,2,167,151,2,181,176,2,120,203,2,35,232,2,236,6,3,21,40,3,235,75,3,198,114,3,16,157,
  3,71,203,3,2,254,3,247,53,4,5,116,4,63,185,4,255,6,5,249,94,5,93,195,5,9,55,6,207,189,6,230,
  92,7,151,27,8,109,4,9,54,39,10,198,156,11,129,142,13,233,70,16,255,90,20,113,38,27,72,188,40,181,123,81,
  255,255,255,16,251,255,67,236,255,151,211,255,15,177,255,171,132,255,109,78,255,87,14,255,109,196,254,175,112,254,35,19,
  254,203,171,253,171,58,253,201,191,252,39,59,252,205,172,251,190,20,251,1,115,250,157,199,249,151,18,249,247,83,248,197,
  139,247,7,186,246,198,222,245,10,250,244,221,11,244,71,20,243,82,19,242,8,9,241,115,245,239,157,216,238,147,178,237,
  94,131,236,11,75,235,166,9,234,59,191,232,215,107,231,135,15,230,89,170,228,89,60,227,151,197,225,33,70,224,5,190,
  222,83,45,221,26,148,219,105,242,217,82,72,216,228,149,214,49,219,212,72,24,211,61,77,209,31,122,207,2,159,205,247,
  187,203,18,209,201,101,222,199,3,228,197,0,226,195,112,216,193,103,199,191,249,174,189,58,143,187,65,104,185,34,58,183,
  243,4,181,201,200,178,186,133,176,221,59,174,73,235,171,20,148,169,85,54,167,36,210,164,153,103,162,202,246,159,209,127,
  157,197,2,155,191,127,152,217,246,149,42,104,147,204,211,144,217,57,142,107,154,139,154,245,136,130,75,134,60,156,131,228,
  231,128,147,46,126,101,112,123,116,173,120,221,229,117,186,25,115,39,73,112,64,116,109,32,155,106,229,189,103,169,220,100,
  138,247,97,164,14,95,20,34,92,247,49,89,105,62,86,137,71,83,114,77,80,67,80,77,24,80,74,16,77,71,73,71,
  68,224,62,65,242,51,62,159,38,59,4,23,56,64,5,53,112,241,49,179,219,46,40,196,43,237,170,40,32,144,37,225,
  115,34,78,86,31,133,55,28,166,23,25,208,246,21,32,213,18,183,178,15,178,143,12,50,108,9,85,72,6,58,36,3,
};

HitachiDSP hitachidsp;

void HitachiDSP::synchronizeCPU() {
  if(clock >= 0) scheduler.resume(cpu.thread);
}

[[noreturn]] static void Enter() {
  while(true) {
    scheduler.synchronize();
    hitachidsp.main();
  }
}

void HitachiDSP::step(unsigned clocks) {
  HG51B::step(clocks);
  clock += clocks * (uint64_t)cpu.frequency;
  synchronizeCPU();
}

void HitachiDSP::halt() {
  HG51B::halt();
  if(io.irq == 0) cpu.irq(r.i = 1);
}

void HitachiDSP::unload() {
  rom.reset();
  ram.reset();
}

void HitachiDSP::power() {
  HG51B::power();
  create(Enter, Frequency);
}

}
