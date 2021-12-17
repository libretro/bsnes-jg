#include "sfc.hpp"

namespace SuperFamicom {

SMP smp;

auto SMP::readRAM(uint16_t address) -> uint8_t {
  if(address >= 0xffc0 && io.iplromEnable) return iplrom[address & 0x3f];
  if(io.ramDisable) return 0x5a;  //0xff on mini-SNES
  return dsp.apuram[address];
}

auto SMP::writeRAM(uint16_t address, uint8_t data) -> void {
  //writes to $ffc0-$ffff always go to apuram, even if the iplrom is enabled
  if(io.ramWritable && !io.ramDisable) dsp.apuram[address] = data;
}

auto SMP::idle() -> void {
  waitIdle();
}

auto SMP::read(uint16_t address) -> uint8_t {
  //Kishin Douji Zenki - Tenchi Meidou requires bus hold delays on CPU I/O reads.
  //smp_mem_access_times requires no bus hold delays on APU RAM reads.
  if((address & 0xfffc) == 0x00f4) {
    wait(address, 1);
    uint8_t data = readRAM(address);
    if((address & 0xfff0) == 0x00f0) data = readIO(address);
    wait(address, 1);
    return data;
  } else {
    wait(address, 0);
    uint8_t data = readRAM(address);
    if((address & 0xfff0) == 0x00f0) data = readIO(address);
    return data;
  }
}

auto SMP::write(uint16_t address, uint8_t data) -> void {
  wait(address);
  writeRAM(address, data);  //even IO writes affect underlying RAM
  if((address & 0xfff0) == 0x00f0) writeIO(address, data);
}

auto SMP::readDisassembler(uint16_t address) -> uint8_t {
  if((address & 0xfff0) == 0x00f0) return 0x00;
  return readRAM(address);
}

auto SMP::portRead(nall::Natural< 2> port) const -> uint8_t {
  if(port == 0) return io.cpu0;
  if(port == 1) return io.cpu1;
  if(port == 2) return io.cpu2;
  if(port == 3) return io.cpu3;
  return 0; // unreachable
}

auto SMP::portWrite(nall::Natural< 2> port, uint8_t data) -> void {
  if(port == 0) io.apu0 = data;
  if(port == 1) io.apu1 = data;
  if(port == 2) io.apu2 = data;
  if(port == 3) io.apu3 = data;
}

auto SMP::readIO(uint16_t address) -> uint8_t {
  uint8_t data;

  switch(address) {
  case 0xf0:  //TEST (write-only register)
    return 0x00;

  case 0xf1:  //CONTROL (write-only register)
    return 0x00;

  case 0xf2:  //DSPADDR
    return io.dspAddr;

  case 0xf3:  //DSPDATA
    //0x80-0xff are read-only mirrors of 0x00-0x7f
    return dsp.read(io.dspAddr & 0x7f);

  case 0xf4:  //CPUIO0
    synchronizeCPU();
    return io.apu0;

  case 0xf5:  //CPUIO1
    synchronizeCPU();
    return io.apu1;

  case 0xf6:  //CPUIO2
    synchronizeCPU();
    return io.apu2;

  case 0xf7:  //CPUIO3
    synchronizeCPU();
    return io.apu3;

  case 0xf8:  //AUXIO4
    return io.aux4;

  case 0xf9:  //AUXIO5
    return io.aux5;

  case 0xfa:  //T0TARGET
  case 0xfb:  //T1TARGET
  case 0xfc:  //T2TARGET (write-only registers)
    return 0x00;

  case 0xfd:  //T0OUT (4-bit counter value)
    data = timer0.stage3;
    timer0.stage3 = 0;
    return data;

  case 0xfe:  //T1OUT (4-bit counter value)
    data = timer1.stage3;
    timer1.stage3 = 0;
    return data;

  case 0xff:  //T2OUT (4-bit counter value)
    data = timer2.stage3;
    timer2.stage3 = 0;
    return data;
  }

  return data;  //unreachable
}

auto SMP::writeIO(uint16_t address, uint8_t data) -> void {
  switch(address) {
  case 0xf0:  //TEST
    if(r.p.p) break;  //writes only valid when P flag is clear

    io.timersDisable      = data >> 0 & 1;
    io.ramWritable        = data >> 1 & 1;
    io.ramDisable         = data >> 2 & 1;
    io.timersEnable       = data >> 3 & 1;
    io.externalWaitStates = data >> 4 & 3;
    io.internalWaitStates = data >> 6 & 3;

    timer0.synchronizeStage1();
    timer1.synchronizeStage1();
    timer2.synchronizeStage1();
    break;

  case 0xf1:  //CONTROL
    //0->1 transistion resets timers
    if(timer0.enable.raise(data & 0x01)) {
      timer0.stage2 = 0;
      timer0.stage3 = 0;
    }

    if(timer1.enable.raise(data & 0x02)) {
      timer1.stage2 = 0;
      timer1.stage3 = 0;
    }

    if(!timer2.enable.raise(data & 0x04)) {
      timer2.stage2 = 0;
      timer2.stage3 = 0;
    }

    if(data & 0x10) {
      synchronizeCPU();
      io.apu0 = 0x00;
      io.apu1 = 0x00;
    }

    if(data & 0x20) {
      synchronizeCPU();
      io.apu2 = 0x00;
      io.apu3 = 0x00;
    }

    io.iplromEnable = bool(data & 0x80);
    break;

  case 0xf2:  //DSPADDR
    io.dspAddr = data;
    break;

  case 0xf3:  //DSPDATA
    if(io.dspAddr & 0x80) break;  //0x80-0xff are read-only mirrors of 0x00-0x7f
    dsp.write(io.dspAddr & 0x7f, data);
    break;

  case 0xf4:  //CPUIO0
    synchronizeCPU();
    io.cpu0 = data;
    break;

  case 0xf5:  //CPUIO1
    synchronizeCPU();
    io.cpu1 = data;
    break;

  case 0xf6:  //CPUIO2
    synchronizeCPU();
    io.cpu2 = data;
    break;

  case 0xf7:  //CPUIO3
    synchronizeCPU();
    io.cpu3 = data;
    break;

  case 0xf8:  //AUXIO4
    io.aux4 = data;
    break;

  case 0xf9:  //AUXIO5
    io.aux5 = data;
    break;

  case 0xfa:  //T0TARGET
    timer0.target = data;
    break;

  case 0xfb:  //T1TARGET
    timer1.target = data;
    break;

  case 0xfc:  //T2TARGET
    timer2.target = data;
    break;

  case 0xfd:  //T0OUT
  case 0xfe:  //T1OUT
  case 0xff:  //T2OUT (read-only registers)
    break;
  }
}

//DSP clock (~24576khz) / 12 (~2048khz) is fed into the SMP
//from here, the wait states value is really a clock divider of {2, 4, 8, 16}
//due to an unknown hardware issue, clock dividers of 8 and 16 are glitchy
//the SMP ends up consuming 10 and 20 clocks per opcode cycle instead
//this causes unpredictable behavior on real hardware
//sometimes the SMP will run far slower than expected
//other times (and more likely), the SMP will deadlock until the system is reset
//the timers are not affected by this and advance by their expected values
auto SMP::wait(maybe<uint16_t> addr, bool half) -> void {
  static const unsigned cycleWaitStates[4] = {2, 4, 10, 20};
  static const unsigned timerWaitStates[4] = {2, 4,  8, 16};

  unsigned waitStates = io.externalWaitStates;
  if(!addr) waitStates = io.internalWaitStates;  //idle cycles
  else if((*addr & 0xfff0) == 0x00f0) waitStates = io.internalWaitStates;  //IO registers
  else if(*addr >= 0xffc0 && io.iplromEnable) waitStates = io.internalWaitStates;  //IPLROM

  step(cycleWaitStates[waitStates] >> half);
  stepTimers(timerWaitStates[waitStates] >> half);
}

auto SMP::waitIdle(maybe<uint16_t> addr, bool half) -> void {
  static const unsigned cycleWaitStates[4] = {2, 4, 10, 20};
  static const unsigned timerWaitStates[4] = {2, 4,  8, 16};

  unsigned waitStates = io.externalWaitStates;
  if(!addr) waitStates = io.internalWaitStates;  //idle cycles
  else if((*addr & 0xfff0) == 0x00f0) waitStates = io.internalWaitStates;  //IO registers
  else if(*addr >= 0xffc0 && io.iplromEnable) waitStates = io.internalWaitStates;  //IPLROM

  stepIdle(cycleWaitStates[waitStates] >> half);
  stepTimers(timerWaitStates[waitStates] >> half);
}

auto SMP::step(unsigned clocks) -> void {
  clock += clocks * (uint64_t)cpu.frequency;
  dsp.clock -= clocks;
  synchronizeDSP();
  //forcefully sync SMP to CPU in case chips are not communicating
  if(clock > 768 * 24 * (int64_t)24'000'000) synchronizeCPU();
}

auto SMP::stepIdle(unsigned clocks) -> void {
  clock += clocks * (uint64_t)cpu.frequency;
  dsp.clock -= clocks;
}

auto SMP::stepTimers(unsigned clocks) -> void {
  timer0.step(clocks);
  timer1.step(clocks);
  timer2.step(clocks);
}

template<unsigned Frequency> auto SMP::Timer<Frequency>::step(unsigned clocks) -> void {
  //stage 0 increment
  stage0 += clocks;
  if(stage0 < Frequency) return;
  stage0 -= Frequency;

  //stage 1 increment
  stage1 ^= 1;
  synchronizeStage1();
}

template<unsigned Frequency> auto SMP::Timer<Frequency>::synchronizeStage1() -> void {
  bool level = stage1;
  if(!smp.io.timersEnable) level = false;
  if(smp.io.timersDisable) level = false;
  if(!line.lower(level)) return;  //only pulse on 1->0 transition

  //stage 2 increment
  if(!enable) return;
  if(++stage2 != target) return;

  //stage 3 increment
  stage2 = 0;
  stage3++;
}

auto SMP::serialize(serializer& s) -> void {
  SPC700::serialize(s);
  Thread::serialize(s);

  s.integer(io.clockCounter);
  s.integer(io.dspCounter);

  s.integer(io.apu0);
  s.integer(io.apu1);
  s.integer(io.apu2);
  s.integer(io.apu3);

  s.integer(io.timersDisable);
  s.integer(io.ramWritable);
  s.integer(io.ramDisable);
  s.integer(io.timersEnable);
  s.integer(io.externalWaitStates);
  s.integer(io.internalWaitStates);

  s.integer(io.iplromEnable);

  s.integer(io.dspAddr);

  s.integer(io.cpu0);
  s.integer(io.cpu1);
  s.integer(io.cpu2);
  s.integer(io.cpu3);

  s.integer(io.aux4);
  s.integer(io.aux5);

  s.integer(timer0.stage0);
  s.integer(timer0.stage1);
  s.integer(timer0.stage2);
  s.integer(timer0.stage3);
  s.boolean(timer0.line);
  s.boolean(timer0.enable);
  s.integer(timer0.target);

  s.integer(timer1.stage0);
  s.integer(timer1.stage1);
  s.integer(timer1.stage2);
  s.integer(timer1.stage3);
  s.boolean(timer1.line);
  s.boolean(timer1.enable);
  s.integer(timer1.target);

  s.integer(timer2.stage0);
  s.integer(timer2.stage1);
  s.integer(timer2.stage2);
  s.integer(timer2.stage3);
  s.boolean(timer2.line);
  s.boolean(timer2.enable);
  s.integer(timer2.target);
}

auto SMP::synchronizeCPU() -> void {
  if(clock >= 0) scheduler.resume(cpu.thread);
}

auto SMP::synchronizeDSP() -> void {
  while(dsp.clock < 0) dsp.main();
}

auto SMP::Enter() -> void {
  while(true) {
    scheduler.synchronize();
    smp.main();
  }
}

auto SMP::main() -> void {
  if(r.wait) return instructionWait();
  if(r.stop) return instructionStop();
  instruction();
}

auto SMP::load() -> bool {
  if(auto fp = platform->open(ID::System, "ipl.rom", File::Read, File::Required)) {
    fp->read(iplrom, 64);
    return true;
  }
  return false;
}

auto SMP::power(bool reset) -> void {
  SPC700::power();
  create(Enter, system.apuFrequency() / 12.0);

  r.pc.byte.l = iplrom[62];
  r.pc.byte.h = iplrom[63];

  io = {};
  timer0 = {};
  timer1 = {};
  timer2 = {};
}

}
