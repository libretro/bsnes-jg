#include "../sfc.hpp"

namespace SuperFamicom {

//note: timings are completely unverified
//due to the ST018 chip design (on-die ROM), testing is nearly impossible

void ArmDSP::sleep() {
  step(1);
}

uint32_t ArmDSP::get(unsigned mode, uint32_t addr) {
  step(1);

  static auto memory = [&](const uint8_t* memory, unsigned mode, uint32_t addr) -> uint32_t {
    if(mode & Word) {
      memory += addr & ~3;
      return memory[0] << 0 | memory[1] << 8 | memory[2] << 16 | memory[3] << 24;
    } else if(mode & Byte) {
      return memory[addr];
    } else {
      return 0;  //should never occur
    }
  };

  switch(addr & 0xe000'0000) {
  case 0x0000'0000: return memory(programROM, mode, addr & 0x1ffff);
  case 0x2000'0000: return pipeline.fetch.instruction;
  case 0x4000'0000: break;
  case 0x6000'0000: return 0x40404001;
  case 0x8000'0000: return pipeline.fetch.instruction;
  case 0xa000'0000: return memory(dataROM, mode, addr & 0x7fff);
  case 0xc000'0000: return pipeline.fetch.instruction;
  case 0xe000'0000: return memory(programRAM, mode, addr & 0x3fff);
  }

  addr &= 0xe000'003f;

  if(addr == 0x4000'0010) {
    if(bridge.cputoarm.ready) {
      bridge.cputoarm.ready = false;
      return bridge.cputoarm.data;
    }
  }

  if(addr == 0x4000'0020) {
    return bridge.status();
  }

  return 0;
}

void ArmDSP::set(unsigned mode, uint32_t addr, uint32_t word) {
  step(1);

  static auto memory = [](uint8_t *memory, unsigned mode, uint32_t addr, uint32_t word) {
    if(mode & Word) {
      memory += addr & ~3;
      *memory++ = word >>  0;
      *memory++ = word >>  8;
      *memory++ = word >> 16;
      *memory++ = word >> 24;
    } else if(mode & Byte) {
      memory += addr;
      *memory++ = word >>  0;
    }
  };

  switch(addr & 0xe000'0000) {
  case 0x0000'0000: return;
  case 0x2000'0000: return;
  case 0x4000'0000: break;
  case 0x6000'0000: return;
  case 0x8000'0000: return;
  case 0xa000'0000: return;
  case 0xc000'0000: return;
  case 0xe000'0000: return memory(programRAM, mode, addr & 0x3fff, word);
  }

  addr &= 0xe000'003f;
  word &= 0x0000'00ff;

  if(addr == 0x4000'0000) {
    bridge.armtocpu.ready = true;
    bridge.armtocpu.data = word;
  }

  if(addr == 0x4000'0010) bridge.signal = true;

  if(addr == 0x4000'0020) bridge.timerlatch = bridge.timerlatch & 0xffff00 | word <<  0;
  if(addr == 0x4000'0024) bridge.timerlatch = bridge.timerlatch & 0xff00ff | word <<  8;
  if(addr == 0x4000'0028) bridge.timerlatch = bridge.timerlatch & 0x00ffff | word << 16;

  if(addr == 0x4000'002c) bridge.timer = bridge.timerlatch;
}

std::vector<uint8_t> ArmDSP::firmware() const {
  std::vector<uint8_t> buffer;
  if(!cartridge.has.ARMDSP) return buffer;
  buffer.reserve(128 * 1024 + 32 * 1024);
  for(auto n : nall::range(128 * 1024)) buffer.push_back(programROM[n]);
  for(auto n : nall::range( 32 * 1024)) buffer.push_back(dataROM[n]);
  return buffer;
}

void ArmDSP::serialize(serializer& s) {
  ARM7TDMI::serialize(s);
  Thread::serialize(s);

  s.array(programRAM, 16 * 1024);

  s.integer(bridge.cputoarm.ready);
  s.integer(bridge.cputoarm.data);
  s.integer(bridge.armtocpu.ready);
  s.integer(bridge.armtocpu.data);
  s.integer(bridge.timer);
  s.integer(bridge.timerlatch);
  s.integer(bridge.reset);
  s.integer(bridge.ready);
  s.integer(bridge.signal);
}

ArmDSP armdsp;

void ArmDSP::synchronizeCPU() {
  if(clock >= 0) scheduler.resume(cpu.thread);
}

void ArmDSP::Enter() {
  armdsp.boot();
  while(true) {
    scheduler.synchronize();
    armdsp.main();
  }
}

void ArmDSP::boot() {
  //reset hold delay
  while(bridge.reset) {
    step(1);
    continue;
  }

  //reset sequence delay
  if(bridge.ready == false) {
    step(65'536);
    bridge.ready = true;
  }
}

void ArmDSP::main() {
  processor.cpsr.t = 0;  //force ARM mode
  instruction();
}

void ArmDSP::step(unsigned clocks) {
  if(bridge.timer && --bridge.timer == 0);
  clock += clocks * (uint64_t)cpu.frequency;
  synchronizeCPU();
}

//MMIO: 00-3f,80-bf:3800-38ff
//3800-3807 mirrored throughout
//a0 ignored

uint8_t ArmDSP::read(unsigned addr, uint8_t) {
  cpu.synchronizeCoprocessors();

  uint8_t data = 0x00;
  addr &= 0xff06;

  if(addr == 0x3800) {
    if(bridge.armtocpu.ready) {
      bridge.armtocpu.ready = false;
      data = bridge.armtocpu.data;
    }
  }

  if(addr == 0x3802) {
    bridge.signal = false;
  }

  if(addr == 0x3804) {
    data = bridge.status();
  }

  return data;
}

void ArmDSP::write(unsigned addr, uint8_t data) {
  cpu.synchronizeCoprocessors();

  addr &= 0xff06;

  if(addr == 0x3802) {
    bridge.cputoarm.ready = true;
    bridge.cputoarm.data = data;
  }

  if(addr == 0x3804) {
    data &= 1;
    if(!bridge.reset && data) reset();
    bridge.reset = data;
  }
}

void ArmDSP::power() {
  random.array((uint8_t*)programRAM, sizeof(programRAM));
  bridge.reset = false;
  reset();
}

void ArmDSP::reset() {
  ARM7TDMI::power();
  create(ArmDSP::Enter, Frequency);

  bridge.ready = false;
  bridge.signal = false;
  bridge.timer = 0;
  bridge.timerlatch = 0;
  bridge.cputoarm.ready = false;
  bridge.armtocpu.ready = false;
}

}
