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

#include <cstdint>

#include "memory.hpp"
#include "sfc.hpp"
#include "bsmemory.hpp"

#include "mcc.hpp"

namespace SuperFamicom {

void MCC::serialize(serializer& s) {
  s.array(psram.data(), psram.size());

  s.integer(irq.flag);
  s.integer(irq.enable);

  s.integer(r.mapping);
  s.integer(r.psramEnableLo);
  s.integer(r.psramEnableHi);
  s.integer(r.psramMapping);
  s.integer(r.romEnableLo);
  s.integer(r.romEnableHi);
  s.integer(r.exEnableLo);
  s.integer(r.exEnableHi);
  s.integer(r.exMapping);
  s.integer(r.internallyWritable);
  s.integer(r.externallyWritable);

  s.integer(w.mapping);
  s.integer(w.psramEnableLo);
  s.integer(w.psramEnableHi);
  s.integer(w.psramMapping);
  s.integer(w.romEnableLo);
  s.integer(w.romEnableHi);
  s.integer(w.exEnableLo);
  s.integer(w.exEnableHi);
  s.integer(w.exMapping);
  s.integer(w.internallyWritable);
  s.integer(w.externallyWritable);
}

MCC mcc;

void MCC::unload() {
  rom.reset();
  psram.reset();
}

void MCC::power() {
  irq.flag = 0;
  irq.enable = 0;
  w.mapping = 1;
  w.psramEnableLo = 1;
  w.psramEnableHi = 0;
  w.psramMapping = 3;
  w.romEnableLo = 1;
  w.romEnableHi = 1;
  w.exEnableLo = 1;
  w.exEnableHi = 0;
  w.exMapping = 1;
  w.internallyWritable = 0;
  w.externallyWritable = 0;
  commit();
}

void MCC::commit() {
  r = w;
  bsmemory.writable(r.externallyWritable);
}

uint8_t MCC::read(unsigned address, uint8_t data) {
  if((address & 0xf0f000) == 0x005000) {  //$00-0f:5000-5fff
    switch(address >> 16 & 15) {
    case  0: return irq.flag << 7;
    case  1: return irq.enable << 7;
    case  2: return r.mapping << 7;
    case  3: return r.psramEnableLo << 7;
    case  4: return r.psramEnableHi << 7;
    case  5: return (r.psramMapping >> 0 & 1) << 7;
    case  6: return (r.psramMapping >> 1 & 1) << 7;
    case  7: return r.romEnableLo << 7;
    case  8: return r.romEnableHi << 7;
    case  9: return r.exEnableLo << 7;
    case 10: return r.exEnableHi << 7;
    case 11: return r.exMapping << 7;
    case 12: return r.internallyWritable << 7;
    case 13: return r.externallyWritable << 7;
    case 14: return 0;  //commit (always zero)
    case 15: return 0;  //unknown (always zero)
    }
  }

  return data;
}

void MCC::write(unsigned address, uint8_t data) {
  if((address & 0xf0f000) == 0x005000) {  //$00-0f:5000-5fff
    switch(address >> 16 & 15) {
    case  1: irq.enable = data >> 7; break;
    case  2: w.mapping = data >> 7; break;
    case  3: w.psramEnableLo = data >> 7; break;
    case  4: w.psramEnableHi = data >> 7; break;
    case  5: w.psramMapping = (w.psramMapping & 2) | data >> 7 << 0; break;
    case  6: w.psramMapping = (w.psramMapping & 1) | data >> 7 << 1; break;
    case  7: w.romEnableLo = data >> 7; break;
    case  8: w.romEnableHi = data >> 7; break;
    case  9: w.exEnableLo = data >> 7; break;
    case 10: w.exEnableHi = data >> 7; break;
    case 11: w.exMapping = data >> 7; break;
    case 12: w.internallyWritable = data >> 7; break;
    case 13: w.externallyWritable = data >> 7; break;
    case 14: if(data >> 7) commit(); break;
    }
  }
}

uint8_t MCC::mcuRead(unsigned address, uint8_t data) {
  return mcuAccess(0, address, data);
}

void MCC::mcuWrite(unsigned address, uint8_t data) {
  return mcuAccess(1, address, data), void();
}

uint8_t MCC::mcuAccess(bool mode, unsigned address, uint8_t data) {
  //[[ROM]]

  if(r.romEnableLo) {
    if((address & 0xc08000) == 0x008000) {  //00-3f:8000-ffff
      return romAccess(mode, (address & 0x3f0000) >> 1 | (address & 0x7fff), data);
    }
  }

  if(r.romEnableHi) {
    if((address & 0xc08000) == 0x808000) {  //80-bf:8000-ffff
      return romAccess(mode, (address & 0x3f0000) >> 1 | (address & 0x7fff), data);
    }
  }

  //[[PSRAM]]

  if(r.psramEnableLo && r.mapping == 0) {
    if(((address & 0xf08000) == 0x008000 && r.psramMapping == 0)  //00-0f:8000-ffff
    || ((address & 0xf08000) == 0x208000 && r.psramMapping == 1)  //20-2f:8000-ffff
    || ((address & 0xf00000) == 0x400000 && r.psramMapping == 2)  //40-4f:0000-ffff
    || ((address & 0xf00000) == 0x600000 && r.psramMapping == 3)  //60-6f:0000-ffff
    ) {
      return psramAccess(mode, (address & 0x0f0000) >> 1 | (address & 0x7fff), data);
    }

    if((address & 0xf08000) == 0x700000) {  //70-7d:0000-7fff
      return psramAccess(mode, (address & 0x0f0000) >> 1 | (address & 0x7fff), data);
    }
  }

  if(r.psramEnableHi && r.mapping == 0) {
    if(((address & 0xf08000) == 0x808000 && r.psramMapping == 0)  //80-8f:8000-ffff
    || ((address & 0xf08000) == 0xa08000 && r.psramMapping == 1)  //a0-af:8000-ffff
    || ((address & 0xf00000) == 0xc00000 && r.psramMapping == 2)  //c0-cf:0000-ffff
    || ((address & 0xf00000) == 0xe00000 && r.psramMapping == 3)  //e0-ef:0000-ffff
    ) {
      return psramAccess(mode, (address & 0x0f0000) >> 1 | (address & 0x7fff), data);
    }

    if((address & 0xf08000) == 0xf00000) {  //f0-ff:0000-7fff
      return psramAccess(mode, (address & 0x0f0000) >> 1 | (address & 0x7fff), data);
    }
  }

  if(r.psramEnableLo && r.mapping == 1) {
    if(((address & 0xf88000) == 0x008000 && r.psramMapping == 0)  //00-07:8000-ffff
    || ((address & 0xf88000) == 0x108000 && r.psramMapping == 1)  //10-17:8000-ffff
    || ((address & 0xf88000) == 0x208000 && r.psramMapping == 2)  //20-27:8000-ffff
    || ((address & 0xf88000) == 0x308000 && r.psramMapping == 3)  //30-37:8000-ffff
    || ((address & 0xf80000) == 0x400000 && r.psramMapping == 0)  //40-47:0000-ffff
    || ((address & 0xf80000) == 0x500000 && r.psramMapping == 1)  //50-57:0000-ffff
    || ((address & 0xf80000) == 0x600000 && r.psramMapping == 2)  //60-67:0000-ffff
    || ((address & 0xf80000) == 0x700000 && r.psramMapping == 3)  //70-77:0000-ffff
    ) {
      return psramAccess(mode, address & 0x07ffff, data);
    }

    if((address & 0xe0e000) == 0x206000) {  //20-3f:6000-7fff
      return psramAccess(mode, (address & 0x3f0000) >> 3 | (address & 0x1fff), data);
    }
  }

  if(r.psramEnableHi && r.mapping == 1) {
    if(((address & 0xf88000) == 0x808000 && r.psramMapping == 0)  //80-87:8000-ffff
    || ((address & 0xf88000) == 0x908000 && r.psramMapping == 1)  //90-97:8000-ffff
    || ((address & 0xf88000) == 0xa08000 && r.psramMapping == 2)  //a0-a7:8000-ffff
    || ((address & 0xf88000) == 0xb08000 && r.psramMapping == 3)  //b0-b7:8000-ffff
    || ((address & 0xf80000) == 0xc00000 && r.psramMapping == 0)  //c0-c7:0000-ffff
    || ((address & 0xf80000) == 0xd00000 && r.psramMapping == 1)  //d0-d7:0000-ffff
    || ((address & 0xf80000) == 0xe00000 && r.psramMapping == 2)  //e0-e7:0000-ffff
    || ((address & 0xf80000) == 0xf00000 && r.psramMapping == 3)  //f0-f7:0000-ffff
    ) {
      return psramAccess(mode, address & 0x07ffff, data);
    }

    if((address & 0xe0e000) == 0xa06000) {  //a0-bf:6000-7fff
      return psramAccess(mode, (address & 0x3f0000) >> 3 | (address & 0x1fff), data);
    }
  }

  //[[EXMEMORY]]

  if(r.exEnableLo && r.mapping == 0) {
    if(((address & 0xe08000) == 0x008000 && r.exMapping == 0)  //00-1f:8000-ffff
    || ((address & 0xe00000) == 0x400000 && r.exMapping == 1)  //40-5f:0000-ffff
    ) {
      return exAccess(mode, (address & 0x1f0000) >> 1 | (address & 0x7fff), data);
    }
  }

  if(r.exEnableLo && r.mapping == 1) {
    if(((address & 0xf08000) == 0x008000 && r.exMapping == 0)  //00-0f:8000-ffff
    || ((address & 0xf08000) == 0x208000 && r.exMapping == 1)  //20-2f:8000-ffff
    || ((address & 0xf00000) == 0x400000 && r.exMapping == 0)  //40-4f:0000-ffff
    || ((address & 0xf00000) == 0x600000 && r.exMapping == 1)  //60-6f:0000-ffff
    ) {
      return exAccess(mode, address & 0x0fffff, data);
    }
  }

  if(r.exEnableHi && r.mapping == 0) {
    if(((address & 0xe08000) == 0x808000 && r.exMapping == 0)  //80-9f:8000-ffff
    || ((address & 0xe00000) == 0xc00000 && r.exMapping == 1)  //c0-df:0000-ffff
    ) {
      return exAccess(mode, (address & 0x1f0000) >> 1 | (address & 0x7fff), data);
    }
  }

  if(r.exEnableHi && r.mapping == 1) {
    if(((address & 0xf08000) == 0x808000 && r.exMapping == 0)  //80-8f:8000-ffff
    || ((address & 0xf08000) == 0xa08000 && r.exMapping == 1)  //a0-af:8000-ffff
    || ((address & 0xf00000) == 0xc00000 && r.exMapping == 0)  //c0-cf:0000-ffff
    || ((address & 0xf00000) == 0xe00000 && r.exMapping == 1)  //e0-ef:0000-ffff
    ) {
      return exAccess(mode, address & 0x0fffff, data);
    }
  }

  //[[BSMEMORY]]

  if(bsmemory.size() && r.mapping == 0) {
    if(((address & 0x408000) == 0x008000)  //00-3f,80-bf:8000-ffff
    || ((address & 0x400000) == 0x400000)  //40-7d,c0-ff:0000-ffff
    ) {
      return bsAccess(mode, (address & 0x3f0000) >> 1 | (address & 0x7fff), data);
    }
  }

  if(bsmemory.size() && r.mapping == 1) {
    if(((address & 0x408000) == 0x008000)  //00-3f,80-bf:8000-ffff
    || ((address & 0x400000) == 0x400000)  //40-7d,c0-ff:0000-ffff
    ) {
      return bsAccess(mode, address & 0x3fffff, data);
    }
  }

  return data;
}

//size: 0x100000
uint8_t MCC::romAccess(bool mode, unsigned address, uint8_t data) {
  address = bus.mirror(address, rom.size());
  if(mode == 0) return rom.read(address);
  return data;
}

//size: 0x80000
uint8_t MCC::psramAccess(bool mode, unsigned address, uint8_t data) {
  address = bus.mirror(address, psram.size());
  if(mode == 0) return psram.read(address);
  return psram.write(address, data), data;
}

//size: 0x100000 (?)
uint8_t MCC::exAccess(bool mode, unsigned address, uint8_t data) {
  if (mode || address) {}
  //not physically present on BSC-1A5B9P-01
  return data;
}

//size: 0x100000, 0x200000, 0x400000
uint8_t MCC::bsAccess(bool mode, unsigned address, uint8_t data) {
  address = bus.mirror(address, bsmemory.size());
  if(mode == 0) return bsmemory.read(address, data);
  if(!r.internallyWritable) return data;
  return bsmemory.write(address, data), data;
}

}
