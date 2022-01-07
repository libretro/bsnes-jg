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
#include <vector>

#include "audio.hpp"
#include "cpu.hpp"
#include "dsp.hpp"

#include "smp.hpp"

namespace SuperFamicom {

SMP smp;

const uint8_t SMP::iplrom[64] = {
  0xcd, 0xef, 0xbd, 0xe8, 0x00, 0xc6, 0x1d, 0xd0,
  0xfc, 0x8f, 0xaa, 0xf4, 0x8f, 0xbb, 0xf5, 0x78,
  0xcc, 0xf4, 0xd0, 0xfb, 0x2f, 0x19, 0xeb, 0xf4,
  0xd0, 0xfc, 0x7e, 0xf4, 0xd0, 0x0b, 0xe4, 0xf5,
  0xcb, 0xf4, 0xd7, 0x00, 0xfc, 0xd0, 0xf3, 0xab,
  0x01, 0x10, 0xef, 0x7e, 0xf4, 0x10, 0xeb, 0xba,
  0xf6, 0xda, 0x00, 0xba, 0xf4, 0xc4, 0xf4, 0xdd,
  0x5d, 0xd0, 0xdb, 0x1f, 0x00, 0x00, 0xc0, 0xff
};

uint8_t SMP::readRAM(uint16_t address) {
  if(address >= 0xffc0 && io.iplromEnable) return iplrom[address & 0x3f];
  if(io.ramDisable) return 0x5a;  //0xff on mini-SNES
  return dsp.apuram[address];
}

void SMP::writeRAM(uint16_t address, uint8_t data) {
  //writes to $ffc0-$ffff always go to apuram, even if the iplrom is enabled
  if(io.ramWritable && !io.ramDisable) dsp.apuram[address] = data;
}

void SMP::idle() {
  waitIdle();
}

uint8_t SMP::read(uint16_t address) {
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

void SMP::write(uint16_t address, uint8_t data) {
  wait(address);
  writeRAM(address, data);  //even IO writes affect underlying RAM
  if((address & 0xfff0) == 0x00f0) writeIO(address, data);
}

uint8_t SMP::readDisassembler(uint16_t address) {
  if((address & 0xfff0) == 0x00f0) return 0x00;
  return readRAM(address);
}

uint8_t SMP::portRead(nall::Natural< 2> port) const {
  if(port == 0) return io.cpu0;
  if(port == 1) return io.cpu1;
  if(port == 2) return io.cpu2;
  if(port == 3) return io.cpu3;
  return 0; // unreachable
}

void SMP::portWrite(nall::Natural< 2> port, uint8_t data) {
  if(port == 0) io.apu0 = data;
  if(port == 1) io.apu1 = data;
  if(port == 2) io.apu2 = data;
  if(port == 3) io.apu3 = data;
}

uint8_t SMP::readIO(uint16_t address) {
  uint8_t data = 0;

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

void SMP::writeIO(uint16_t address, uint8_t data) {
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
    if(raise(timer0.enable, data & 0x01)) {
      timer0.stage2 = 0;
      timer0.stage3 = 0;
    }

    if(raise(timer1.enable, data & 0x02)) {
      timer1.stage2 = 0;
      timer1.stage3 = 0;
    }

    if(raise(timer2.enable, data & 0x04)) {
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
void SMP::wait(std::optional<uint16_t> addr, bool half) {
  static const unsigned cycleWaitStates[4] = {2, 4, 10, 20};
  static const unsigned timerWaitStates[4] = {2, 4,  8, 16};

  unsigned waitStates = io.externalWaitStates;
  if(!addr) waitStates = io.internalWaitStates;  //idle cycles
  else if((*addr & 0xfff0) == 0x00f0) waitStates = io.internalWaitStates;  //IO registers
  else if(*addr >= 0xffc0 && io.iplromEnable) waitStates = io.internalWaitStates;  //IPLROM

  step(cycleWaitStates[waitStates] >> half);
  stepTimers(timerWaitStates[waitStates] >> half);
}

void SMP::waitIdle(std::optional<uint16_t> addr, bool half) {
  static const unsigned cycleWaitStates[4] = {2, 4, 10, 20};
  static const unsigned timerWaitStates[4] = {2, 4,  8, 16};

  unsigned waitStates = io.externalWaitStates;
  if(!addr) waitStates = io.internalWaitStates;  //idle cycles
  else if((*addr & 0xfff0) == 0x00f0) waitStates = io.internalWaitStates;  //IO registers
  else if(*addr >= 0xffc0 && io.iplromEnable) waitStates = io.internalWaitStates;  //IPLROM

  stepIdle(cycleWaitStates[waitStates] >> half);
  stepTimers(timerWaitStates[waitStates] >> half);
}

void SMP::step(unsigned clocks) {
  clock += clocks * (uint64_t)cpu.frequency;
  dsp.clock -= clocks;
  synchronizeDSP();
  //forcefully sync SMP to CPU in case chips are not communicating
  if(clock > 768 * 24 * (int64_t)24'000'000) synchronizeCPU();
}

void SMP::stepIdle(unsigned clocks) {
  clock += clocks * (uint64_t)cpu.frequency;
  dsp.clock -= clocks;
}

void SMP::stepTimers(unsigned clocks) {
  timer0.step(clocks);
  timer1.step(clocks);
  timer2.step(clocks);
}

template<unsigned Frequency> void SMP::Timer<Frequency>::step(unsigned clocks) {
  //stage 0 increment
  stage0 += clocks;
  if(stage0 < Frequency) return;
  stage0 -= Frequency;

  //stage 1 increment
  stage1 ^= 1;
  synchronizeStage1();
}

template<unsigned Frequency> void SMP::Timer<Frequency>::synchronizeStage1() {
  bool level = stage1;
  if(!smp.io.timersEnable) level = false;
  if(smp.io.timersDisable) level = false;

  if(!lower(line, level)) return;  //only pulse on 1->0 transition

  //stage 2 increment
  if(!enable) return;
  if(++stage2 != target) return;

  //stage 3 increment
  stage2 = 0;
  stage3++;
}

void SMP::serialize(serializer& s) {
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

void SMP::synchronizeCPU() {
  if(clock >= 0) scheduler.resume(cpu.thread);
}

void SMP::synchronizeDSP() {
  while(dsp.clock < 0) dsp.main();
}

void SMP::Enter() {
  while(true) {
    scheduler.synchronize();
    smp.main();
  }
}

void SMP::main() {
  if(r.wait) return instructionWait();
  if(r.stop) return instructionStop();
  instruction();
}

bool SMP::load() {
  return true;
}

void SMP::power(bool reset) {
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
