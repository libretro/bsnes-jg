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

#include <string>

#include "serializer.hpp"
#include "controller.hpp"
#include "coprocessor/icd.hpp"
#include "coprocessor/msu1.hpp"
#include "memory.hpp"
#include "random.hpp"
#include "settings.hpp"
#include "smp.hpp"

#include "cpu.hpp"

namespace SuperFamicom {

CPU cpu;

static const unsigned dmaLengths[8] = {1, 2, 2, 4, 4, 4, 2, 4};

bool CPU::interruptPending() const {
  return status.interruptPending;
}

bool CPU::synchronizing() const {
  return scheduler.synchronizing();
}

bool CPU::flip(bool& data, bool value) {
  return data != value ? (data = value, true) : false;
}

bool CPU::lower(bool& data) {
  return data == 1 ? data = 0, true : false;
}

bool CPU::raise(bool& data, bool value) {
  return !data && value ? (data = value, true) : (data = value, false);
}

bool CPU::dmaEnable() {
  for(Channel& channel : channels) if(channel.dmaEnable) return true;
  return false;
}

bool CPU::hdmaEnable() {
  for(Channel& channel : channels) if(channel.hdmaEnable) return true;
  return false;
}

bool CPU::hdmaActive() {
  for(Channel& channel : channels) if(channel.hdmaActive()) return true;
  return false;
}

void CPU::quirk() {
  init = true;
}

void CPU::dmaRun() {
  counter.dma += 8;
  step<8,0>();
  dmaEdge();
  for(Channel& channel : channels) channel.dmaRun();
  status.irqLock = true;
}

void CPU::hdmaReset() {
  for(Channel& channel : channels) channel.hdmaReset();
}

void CPU::hdmaSetup() {
  counter.dma += 8;
  step<8,0>();
  for(Channel& channel : channels) channel.hdmaSetup();
  status.irqLock = true;
}

void CPU::hdmaRun() {
  counter.dma += 8;
  step<8,0>();
  for(Channel& channel : channels) channel.hdmaTransfer();
  for(Channel& channel : channels) channel.hdmaAdvance();
  status.irqLock = true;
}

template<unsigned Clocks, bool Synchronize>
void CPU::Channel::step() {
  cpu.counter.dma += Clocks;
  cpu.step<Clocks, Synchronize>();
}

void CPU::Channel::edge() {
  cpu.dmaEdge();
}

bool CPU::Channel::validA(uint32_t address) {
  //A-bus cannot access the B-bus or CPU I/O registers
  if(((address & 0x40ff00) == 0x2100)      //00-3f,80-bf:2100-21ff
      || ((address & 0x40fe00) == 0x4000)  //00-3f,80-bf:4000-41ff
      || ((address & 0x40ffe0) == 0x4200)  //00-3f,80-bf:4200-421f
      || ((address & 0x40ff80) == 0x4300)) //00-3f,80-bf:4300-437f
    return false;
  return true;
}

uint8_t CPU::Channel::readA(uint32_t address) {
  step<4,1>();
  cpu.r.mdr = validA(address) ? bus.read(address, cpu.r.mdr) : (uint8_t)0x00;
  step<4,1>();
  return cpu.r.mdr;
}

uint8_t CPU::Channel::readB(uint8_t address, bool valid) {
  step<4,1>();
  cpu.r.mdr = valid ? bus.read(0x2100 | address, cpu.r.mdr) : (uint8_t)0x00;
  step<4,1>();
  return cpu.r.mdr;
}

void CPU::Channel::writeA(uint32_t address, uint8_t data) {
  if(validA(address)) bus.write(address, data);
}

void CPU::Channel::writeB(uint8_t address, uint8_t data, bool valid) {
  if(valid) bus.write(0x2100 | address, data);
}

void CPU::Channel::transfer(uint32_t addressA, uint8_t index) {
  uint8_t addressB = targetAddress;
  switch(transferMode) {
  case 1: case 5: addressB += (index & 1); break;
  case 3: case 7: addressB += ((index & 2) >> 1); break;
  case 4: addressB += index; break;
  }

  //transfers from WRAM to WRAM are invalid
  bool valid = addressB != 0x80 || ((addressA & 0xfe0000) != 0x7e0000 && (addressA & 0x40e000) != 0x0000);

  cpu.r.mar = addressA;
  if(direction == 0) {
    uint8_t data = readA(addressA);
    writeB(addressB, data, valid);
  } else {
    uint8_t data = readB(addressB, valid);
    writeA(addressA, data);
  }
}

void CPU::Channel::dmaRun() {
  if(!dmaEnable) return;

  step<8,0>();
  edge();

  uint8_t index = 0;
  do {
    transfer(sourceBank << 16 | sourceAddress, index);
    index = (index + 1) & 0x03;
    if(!fixedTransfer) !reverseTransfer ? ++sourceAddress : --sourceAddress;
    edge();
  } while(dmaEnable && --transferSize);

  dmaEnable = false;
}

bool CPU::Channel::hdmaActive() {
  return hdmaEnable && !hdmaCompleted;
}

bool CPU::Channel::hdmaFinished() {
  Channel *channel = next;
  while(channel != nullptr) {
    if(channel->hdmaActive()) return false;
    channel = channel->next;
  }
  return true;
}

void CPU::Channel::hdmaReset() {
  hdmaCompleted = false;
  hdmaDoTransfer = false;
}

void CPU::Channel::hdmaSetup() {
  hdmaDoTransfer = true;  //note: needs hardware verification

  if(hdmaEnable) {
    dmaEnable = false;  //HDMA will stop active DMA mid-transfer
    hdmaAddress = sourceAddress;
    lineCounter = 0;
    hdmaReload();
  }
}

void CPU::Channel::hdmaReload() {
  uint8_t data = readA(cpu.r.mar = sourceBank << 16 | hdmaAddress);

  if((lineCounter & 0x7f) == 0) {
    lineCounter = data;
    ++hdmaAddress;

    hdmaCompleted = lineCounter == 0;
    hdmaDoTransfer = !hdmaCompleted;

    if(indirect) {
      data = readA(cpu.r.mar = sourceBank << 16 | hdmaAddress++);
      indirectAddress = data << 8 | 0x00;  //todo: should 0x00 be indirectAddress >> 8 ?
      if(hdmaCompleted && hdmaFinished()) return;

      data = readA(cpu.r.mar = sourceBank << 16 | hdmaAddress++);
      indirectAddress = data << 8 | indirectAddress >> 8;
    }
  }
}

void CPU::Channel::hdmaTransfer() {
  if(hdmaActive()) {
    dmaEnable = false;  //HDMA will stop active DMA mid-transfer
    if(hdmaDoTransfer) {
      for(uint8_t index = 0; index < dmaLengths[transferMode]; ++index) {
        uint32_t address = !indirect ? sourceBank << 16 | hdmaAddress++ : indirectBank << 16 | indirectAddress++;
        transfer(address, index);
      }
    }
  }
}

void CPU::Channel::hdmaAdvance() {
  if(hdmaActive()) {
    --lineCounter;
    hdmaDoTransfer = bool(lineCounter & 0x80);
    hdmaReload();
  }
}

void CPU::idle() {
  status.clockCount = 6;
  dmaEdge();
  step<6,0>();
  status.irqLock = 0;
  aluEdge();
}

uint8_t CPU::read(unsigned address) {
  if(address & 0x408000) {
    if(address & 0x800000 && io.fastROM) {
      status.clockCount = 6;
      dmaEdge();
      r.mar = address;
      step<2,1>();
    } else {
      status.clockCount = 8;
      dmaEdge();
      r.mar = address;
      step<4,1>();
    }
  } else if((address + 0x6000) & 0x4000) {
    status.clockCount = 8;
    dmaEdge();
    r.mar = address;
    step<4,1>();
  } else if((address - 0x4000) & 0x7e00) {
    status.clockCount = 6;
    dmaEdge();
    r.mar = address;
    step<2,1>();
  } else {
    status.clockCount = 12;
    dmaEdge();
    r.mar = address;
    step<8,1>();
  }

  status.irqLock = 0;
  uint8_t data = bus.read(address, r.mdr);
  step<4,0>();
  aluEdge();
  //$00-3f,80-bf:4000-43ff reads are internal to CPU, and do not update the MDR
  if((address & 0x40fc00) != 0x4000) r.mdr = data;
  return data;
}

void CPU::write(unsigned address, uint8_t data) {
  aluEdge();

  if(address & 0x408000) {
    if(address & 0x800000 && io.fastROM) {
      status.clockCount = 6;
      dmaEdge();
      r.mar = address;
      step<6,1>();
    } else {
      status.clockCount = 8;
      dmaEdge();
      r.mar = address;
      step<8,1>();
    }
  } else if((address + 0x6000) & 0x4000) {
    status.clockCount = 8;
    dmaEdge();
    r.mar = address;
    step<8,1>();
  } else if((address - 0x4000) & 0x7e00) {
    status.clockCount = 6;
    dmaEdge();
    r.mar = address;
    step<6,1>();
  } else {
    status.clockCount = 12;
    dmaEdge();
    r.mar = address;
    step<12,1>();
  }

  status.irqLock = 0;
  bus.write(address, r.mdr = data);
}

uint8_t CPU::readDisassembler(unsigned address) {
  return bus.read(address, r.mdr);
}

uint8_t CPU::readRAM(unsigned addr, uint8_t data) {
  if (data) {}
  return wram[addr];
}

uint8_t CPU::readAPU(unsigned addr, uint8_t data) {
  if (data) {}
  synchronizeSMP();
  return smp.portRead(addr & 3);
}

uint8_t CPU::readCPU(unsigned addr, uint8_t data) {
  switch(addr & 0xffff) {
  case 0x2180: {  //WMDATA
    uint8_t temp = bus.read(0x7e0000 | io.wramAddress, data);
    io.wramAddress = (io.wramAddress + 1) & 0x1ffff;
    return temp;
  }

  //todo: it is not known what happens when reading from this register during auto-joypad polling
  case 0x4016:  //JOYSER0
    data &= 0xfc;
    data |= controllerPort1.device->data();
    return data;

  //todo: it is not known what happens when reading from this register during auto-joypad polling
  case 0x4017:  //JOYSER1
    data &= 0xe0;
    data |= 0x1c;  //pins are connected to GND
    data |= controllerPort2.device->data();
    return data;

  case 0x4210:  //RDNMI
    data &= 0x70;
    data |= rdnmi() << 7;
    data |= version & 0x0f;
    return data;

  case 0x4211:  //TIMEUP
    data &= 0x7f;
    data |= timeup() << 7;
    return data;

  case 0x4212:  //HVBJOY
    data &= 0x3e;
    data |= io.autoJoypadPoll && status.autoJoypadCounter < 33;
    data |= (hcounter() <= 2 || hcounter() >= 1096) << 6;  //hblank
    data |= (vcounter() >= ppu.vdisp()) << 7;              //vblank
    return data;

  case 0x4213: return io.pio;  //RDIO

  case 0x4214: return io.rddiv >> 0;  //RDDIVL
  case 0x4215: return io.rddiv >> 8;  //RDDIVH
  case 0x4216: return io.rdmpy >> 0;  //RDMPYL
  case 0x4217: return io.rdmpy >> 8;  //RDMPYH

  //todo: it is not known what happens when reading from these registers during auto-joypad polling
  case 0x4218: return io.joy1 >> 0;   //JOY1L
  case 0x4219: return io.joy1 >> 8;   //JOY1H
  case 0x421a: return io.joy2 >> 0;   //JOY2L
  case 0x421b: return io.joy2 >> 8;   //JOY2H
  case 0x421c: return io.joy3 >> 0;   //JOY3L
  case 0x421d: return io.joy3 >> 8;   //JOY3H
  case 0x421e: return io.joy4 >> 0;   //JOY4L
  case 0x421f: return io.joy4 >> 8;   //JOY4H

  default: return data;
  }
}

uint8_t CPU::readDMA(unsigned addr, uint8_t data) {
  Channel& channel = this->channels[addr >> 4 & 7];

  switch(addr & 0xff8f) {

  case 0x4300:  //DMAPx
    return (
      channel.transferMode    << 0
    | channel.fixedTransfer   << 3
    | channel.reverseTransfer << 4
    | channel.unused          << 5
    | channel.indirect        << 6
    | channel.direction       << 7
    );

  case 0x4301: return channel.targetAddress;       //BBADx
  case 0x4302: return channel.sourceAddress >> 0;  //A1TxL
  case 0x4303: return channel.sourceAddress >> 8;  //A1TxH
  case 0x4304: return channel.sourceBank;          //A1Bx
  case 0x4305: return channel.transferSize >> 0;   //DASxL
  case 0x4306: return channel.transferSize >> 8;   //DASxH
  case 0x4307: return channel.indirectBank;        //DASBx
  case 0x4308: return channel.hdmaAddress >> 0;    //A2AxL
  case 0x4309: return channel.hdmaAddress >> 8;    //A2AxH
  case 0x430a: return channel.lineCounter;         //NTRLx
  case 0x430b: return channel.unknown;             //???x
  case 0x430f: return channel.unknown;             //???x ($43xb mirror)

  default: return data;
  }
}

void CPU::writeRAM(unsigned addr, uint8_t data) {
  wram[addr] = data;
}

void CPU::writeAPU(unsigned addr, uint8_t data) {
  synchronizeSMP();
  return smp.portWrite(addr & 3, data);
}

void CPU::writeCPU(unsigned addr, uint8_t data) {
  switch(addr & 0xffff) {

  case 0x2180: { //WMDATA
    uint32_t temp = io.wramAddress;
    io.wramAddress = (io.wramAddress + 1) & 0x1ffff;
    return bus.write(0x7e0000 | temp, data);
  }

  case 0x2181:  //WMADDL
    io.wramAddress = (io.wramAddress & 0x1ff00) | data << 0;
    return;

  case 0x2182:  //WMADDM
    io.wramAddress = (io.wramAddress & 0x100ff) | data << 8;
    return;

  case 0x2183:  //WMADDH
    io.wramAddress = (io.wramAddress & 0x0ffff) | (data & 1) << 16;
    return;

  //todo: it is not known what happens when writing to this register during auto-joypad polling
  case 0x4016:  //JOYSER0
    //bit 0 is shared between JOYSER0 and JOYSER1:
    //strobing $4016.d0 affects both controller port latches.
    //$4017 bit 0 writes are ignored.
    controllerPort1.device->latch(data & 1);
    controllerPort2.device->latch(data & 1);
    return;

  case 0x4200:  //NMITIMEN
    io.autoJoypadPoll = data & 1;
    if(!io.autoJoypadPoll) status.autoJoypadCounter = 33; // Disable auto-joypad read
    nmitimenUpdate(data);
    return;

  case 0x4201:  //WRIO
    if((io.pio & 0x80) && !(data & 0x80)) ppu.latchCounters();
    io.pio = data;
    return;

  case 0x4202:  //WRMPYA
    io.wrmpya = data;
    return;

  case 0x4203:  //WRMPYB
    io.rdmpy = 0;
    if(alu.mpyctr || alu.divctr) return;

    io.wrmpyb = data;
    io.rddiv = io.wrmpyb << 8 | io.wrmpya;

    if (!alu.mpylast) {
      alu.mpyctr = 8;  //perform multiplication over the next eight cycles
      alu.shift = io.wrmpyb;
    }

    return;

  case 0x4204:  //WRDIVL
    io.wrdiva = (io.wrdiva & 0xff00) | data << 0;
    return;

  case 0x4205:  //WRDIVH
    io.wrdiva = (io.wrdiva & 0x00ff) | data << 8;
    return;

  case 0x4206:  //WRDIVB
    io.rdmpy = io.wrdiva;
    if(alu.mpyctr || alu.divctr) return;

    if (!alu.divlast) {
      io.wrdivb = data;
      alu.divctr = 16;  //perform division over the next sixteen cycles
      alu.shift = io.wrdivb << 16;
    }
    return;

  case 0x4207:  //HTIMEL
    io.htime = ((io.htime >> 2) - 1) & 0xfff;
    io.htime = (io.htime & 0x100) | data << 0;
    io.htime = ((io.htime + 1) << 2) & 0xfff;
    irqPoll();  //unverified
    return;

  case 0x4208:  //HTIMEH
    io.htime = ((io.htime >> 2) - 1) & 0xfff;
    io.htime = (io.htime & 0x0ff) | (data & 1) << 8;
    io.htime = ((io.htime + 1) << 2) & 0xfff;
    irqPoll();  //unverified
    return;

  case 0x4209:  //VTIMEL
    io.vtime = (io.vtime & 0x100) | data << 0;
    irqPoll();  //unverified
    return;

  case 0x420a:  //VTIMEH
    io.vtime = (io.vtime & 0x0ff) | (data & 1) << 8;
    irqPoll();  //unverified
    return;

  case 0x420b:  //DMAEN
    for(unsigned n = 0; n < 8; ++n) channels[n].dmaEnable = bool(data & 1 << n);
    if(data) status.dmaPending = true;
    return;

  case 0x420c:  //HDMAEN
    for(unsigned n = 0; n < 8; ++n) channels[n].hdmaEnable = bool(data & 1 << n);
    return;

  case 0x420d:  //MEMSEL
    io.fastROM = data & 1;
    return;
  }
}

void CPU::writeDMA(unsigned addr, uint8_t data) {
  Channel& channel = this->channels[addr >> 4 & 7];

  switch(addr & 0xff8f) {

  case 0x4300:  //DMAPx
    channel.transferMode    = data >> 0 & 7;
    channel.fixedTransfer   = data >> 3 & 1;
    channel.reverseTransfer = data >> 4 & 1;
    channel.unused          = data >> 5 & 1;
    channel.indirect        = data >> 6 & 1;
    channel.direction       = data >> 7 & 1;
    return;

  case 0x4301:  //BBADx
    channel.targetAddress = data;
    return;

  case 0x4302:  //A1TxL
    channel.sourceAddress = (channel.sourceAddress & 0xff00) | data << 0;
    return;

  case 0x4303:  //A1TxH
    channel.sourceAddress = (channel.sourceAddress & 0x00ff) | data << 8;
    return;

  case 0x4304:  //A1Bx
    channel.sourceBank = data;
    return;

  case 0x4305:  //DASxL
    channel.transferSize = (channel.transferSize & 0xff00) | data << 0;
    return;

  case 0x4306:  //DASxH
    channel.transferSize = (channel.transferSize & 0x00ff) | data << 8;
    return;

  case 0x4307:  //DASBx
    channel.indirectBank = data;
    return;

  case 0x4308:  //A2AxL
    channel.hdmaAddress = (channel.hdmaAddress & 0xff00) | data << 0;
    return;

  case 0x4309:  //A2AxH
    channel.hdmaAddress = (channel.hdmaAddress & 0x00ff) | data << 8;
    return;

  case 0x430a:  //NTRLx
    channel.lineCounter = data;
    return;

  case 0x430b:  //???x
    channel.unknown = data;
    return;

  case 0x430f:  //???x ($43xb mirror)
    channel.unknown = data;
    return;
  }
}

//DMA clock divider
unsigned CPU::dmaCounter() const {
  return counter.cpu & 7;
}

//joypad auto-poll clock divider
unsigned CPU::joypadCounter() const {
  return counter.cpu & 127;
}

void CPU::stepOnce() {
  counter.cpu += 2;
  tick();
  if(hcounter() & 2) nmiPoll(), irqPoll();
  if(joypadCounter() == 0) joypadEdge();
}

template<unsigned Clocks, bool Synchronize>
void CPU::step() {
  static_assert(Clocks == 2 || Clocks == 4 || Clocks == 6 || Clocks == 8 || Clocks == 10 || Clocks == 12, "invalid number of clock cycles");

  for(Thread* coprocessor : coprocessors) {
    if(coprocessor == &icd || coprocessor == &msu1) continue;
    coprocessor->clock -= Clocks * (uint64_t)coprocessor->frequency;
  }

  if(Clocks >=  2) stepOnce();
  if(Clocks >=  4) stepOnce();
  if(Clocks >=  6) stepOnce();
  if(Clocks >=  8) stepOnce();
  if(Clocks >= 10) stepOnce();
  if(Clocks >= 12) stepOnce();

  smp.clock -= Clocks * (uint64_t)smp.frequency;
  ppu.clock -= Clocks;
  for(Thread* coprocessor : coprocessors) {
    if(coprocessor == &icd || coprocessor == &msu1)
      coprocessor->clock -= Clocks * (uint64_t)coprocessor->frequency;
  }

  if(!status.dramRefresh && hcounter() >= status.dramRefreshPosition) {
    //note: pattern should technically be 5-3, 5-3, 5-3, 5-3, 5-3 per logic analyzer
    //result averages out the same as no coprocessor polls refresh() at > frequency()/2
    status.dramRefresh = 1; step<6,0>(); status.dramRefresh = 2; step<2,0>(); aluEdge();
    status.dramRefresh = 1; step<6,0>(); status.dramRefresh = 2; step<2,0>(); aluEdge();
    status.dramRefresh = 1; step<6,0>(); status.dramRefresh = 2; step<2,0>(); aluEdge();
    status.dramRefresh = 1; step<6,0>(); status.dramRefresh = 2; step<2,0>(); aluEdge();
    status.dramRefresh = 1; step<6,0>(); status.dramRefresh = 2; step<2,0>(); aluEdge();
  }

  if(!status.hdmaSetupTriggered && hcounter() >= status.hdmaSetupPosition) {
    status.hdmaSetupTriggered = true;
    hdmaReset();
    if(hdmaEnable()) {
      status.hdmaPending = true;
      status.hdmaMode = 0;
    }
  }

  if(!status.hdmaTriggered && hcounter() >= status.hdmaPosition) {
    status.hdmaTriggered = true;
    if(hdmaActive()) {
      status.hdmaPending = true;
      status.hdmaMode = 1;
    }
  }

  if (Synchronize && !configuration.coprocessor.delayedSync) {
    synchronizeCoprocessors();
  }
}

void CPU::step(unsigned clocks) {
  switch(clocks) {
  case  2: return step< 2,1>();
  case  4: return step< 4,1>();
  case  6: return step< 6,1>();
  case  8: return step< 8,1>();
  case 10: return step<10,1>();
  case 12: return step<12,1>();
  }
}

//called by ppu.tick() when Hcounter=0
void CPU::scanline() {
  //forcefully sync S-CPU to other processors, in case chips are not communicating
  synchronizeSMP();
  synchronizePPU();
  synchronizeCoprocessors();

  if(vcounter() == 0) {
    //HDMA setup triggers once every frame
    status.hdmaSetupPosition = (version == 1 ? 12 + 8 - dmaCounter() : 12 + dmaCounter());
    status.hdmaSetupTriggered = false;

    status.autoJoypadCounter = 33;  //33 = inactive
  }

  //DRAM refresh occurs once every scanline
  if(version == 2) status.dramRefreshPosition = 530 + 8 - dmaCounter();
  status.dramRefresh = 0;

  //HDMA triggers once every visible scanline
  if(vcounter() < ppu.vdisp()) {
    status.hdmaPosition = 1104;
    status.hdmaTriggered = false;
  }

  //handle video frame events from the CPU core to prevent a race condition between
  //games polling inputs during NMI and the PPU thread not being 100% synchronized.
  if(vcounter() == ppu.vdisp()) {
    if(Controller* device = controllerPort2.device) device->latch();  //light guns
    synchronizePPU();
    scheduler.leave(Scheduler::Event::Frame);
  }
}

void CPU::aluEdge() {
  if(alu.mpyctr) {
    if (!--alu.mpyctr)
      alu.mpylast = 1;
    if(io.rddiv & 1)
      io.rdmpy += alu.shift;
    io.rddiv >>= 1;
    alu.shift <<= 1;
  }
  else {
    alu.mpylast = 0;
  }

  if(alu.divctr) {
    if (!--alu.divctr)
      alu.divlast = 1;
    io.rddiv <<= 1;
    alu.shift >>= 1;
    if(io.rdmpy >= alu.shift) {
      io.rdmpy -= alu.shift;
      io.rddiv |= 1;
    }
  }
  else {
    alu.divlast = 0;
  }
}

void CPU::dmaEdge() {
  //H/DMA pending && DMA inactive?
  //.. Run one full CPU cycle
  //.. HDMA pending && HDMA enabled ? DMA sync + HDMA run
  //.. DMA pending && DMA enabled ? DMA sync + DMA run
  //.... HDMA during DMA && HDMA enabled ? DMA sync + HDMA run
  //.. Run one bus CPU cycle
  //.. CPU sync

  if(status.dmaActive) {
    if(status.hdmaPending) {
      status.hdmaPending = false;
      if(hdmaEnable()) {
        if(!dmaEnable()) {
          step(counter.dma = 8 - dmaCounter());
        }
        status.hdmaMode == 0 ? hdmaSetup() : hdmaRun();
        if(!dmaEnable()) {
          step(status.clockCount - counter.dma % status.clockCount);
          status.dmaActive = false;
        }
      }
    }

    if(status.dmaPending) {
      status.dmaPending = false;
      if(dmaEnable()) {
        step(counter.dma = 8 - dmaCounter());
        dmaRun();
        step(status.clockCount - counter.dma % status.clockCount);
        status.dmaActive = false;
      }
    }
  }

  if(!status.dmaActive && (status.dmaPending || status.hdmaPending)) {
    status.dmaActive = true;
  }
}

//called every 128 clocks from inside the CPU::stepOnce() function
void CPU::joypadEdge() {
  //it is not yet confirmed if polling can be stopped early and/or (re)started later
  if(!io.autoJoypadPoll) return;

  if(vcounter() == ppu.vdisp() && hcounter() >= 130 && hcounter() <= 256) {
    //begin new polling sequence
    status.autoJoypadCounter = 0;
  }

  //stop after polling has been completed for this frame
  if(status.autoJoypadCounter >= 33) return;

  if(status.autoJoypadCounter == 0) {
    //latch controller states on the first polling cycle
    controllerPort1.device->latch(1);
    controllerPort2.device->latch(1);
  }

  if(status.autoJoypadCounter == 1) {
    //release latch and begin reading on the second cycle
    controllerPort1.device->latch(0);
    controllerPort2.device->latch(0);

    //shift registers are cleared to zero at start of auto-joypad polling
    io.joy1 = 0;
    io.joy2 = 0;
    io.joy3 = 0;
    io.joy4 = 0;
  }

  if(status.autoJoypadCounter >= 2 && !(status.autoJoypadCounter & 1)) {
    //sixteen bits are shifted into joy{1-4}, one bit per 256 clocks
    uint8_t port0 = controllerPort1.device->data();
    uint8_t port1 = controllerPort2.device->data();

    io.joy1 = io.joy1 << 1 | (port0 & 1);
    io.joy2 = io.joy2 << 1 | (port1 & 1);
    io.joy3 = io.joy3 << 1 | ((port0 & 2) >> 1);
    io.joy4 = io.joy4 << 1 | ((port1 & 2) >> 1);
  }

  ++status.autoJoypadCounter;
}

//nmiPoll() and irqPoll() are called once every four clock cycles;
//as NMI steps by scanlines (divisible by 4) and IRQ by PPU 4-cycle dots.
//
//ppu.(vh)counter(n) returns the value of said counters n-clocks before current time;
//it is used to emulate hardware communication delay between opcode and interrupt units.

void CPU::nmiPoll() {
  //NMI hold
  if(lower(status.nmiHold) && io.nmiEnable) {
    status.nmiTransition = 1;
  }

  //NMI test
  if(flip(status.nmiValid, vcounter(2) >= ppu.vdisp())) {
    if((status.nmiLine = status.nmiValid)) status.nmiHold = 1;  //hold /NMI for four cycles
  }
}

void CPU::irqPoll() {
  //IRQ hold
  status.irqHold = 0;
  if(status.irqLine && io.irqEnable) {
    status.irqTransition = 1;
  }

  //IRQ test
  if(raise(status.irqValid, io.irqEnable
  && (!io.virqEnable || vcounter(10) == io.vtime)
  && (!io.hirqEnable || hcounter(10) == io.htime)
  && (vcounter(6) || hcounter(6))  //IRQs cannot trigger on last dot of fields
  )) status.irqLine = status.irqHold = 1;  //hold /IRQ for four cycles
}

void CPU::nmitimenUpdate(uint8_t data) {
  io.hirqEnable = data & 0x10;
  io.virqEnable = data & 0x20;
  io.irqEnable = io.hirqEnable || io.virqEnable;

  if(io.virqEnable && !io.hirqEnable && status.irqLine) {
    status.irqTransition = 1;
  } else if(!io.irqEnable) {
    status.irqLine = 0;
    status.irqTransition = 0;
  }

  if(raise(io.nmiEnable, data & 0x80) && status.nmiLine) {
    status.nmiTransition = 1;
  }

  status.irqLock = 1;
}

bool CPU::rdnmi() {
  bool result = status.nmiLine;
  if(!status.nmiHold) {
    status.nmiLine = 0;
  }
  return result;
}

bool CPU::timeup() {
  bool result = status.irqLine;
  if(!status.irqHold) {
    status.irqLine = 0;
    status.irqTransition = 0;
  }
  return result;
}

bool CPU::nmiTest() {
  if(!status.nmiTransition) return 0;
  status.nmiTransition = 0;
  r.wai = 0;
  return 1;
}

bool CPU::irqTest() {
  if(!status.irqTransition && !r.irq) return 0;
  status.irqTransition = 0;
  r.wai = 0;
  return !r.p.i;
}

//used to test for NMI/IRQ, which can trigger on the edge of every opcode.
//test one cycle early to simulate two-stage pipeline of the 65816 CPU.
//
//status.irqLock is used to simulate hardware delay before interrupts can
//trigger during certain events (immediately after DMA, writes to $4200, etc)
void CPU::lastCycle() {
  if(!status.irqLock) {
    if(nmiTest()) status.nmiPending = 1, status.interruptPending = 1;
    if(irqTest()) status.irqPending = 1, status.interruptPending = 1;
  }
}

void CPU::serialize(serializer& s) {
  WDC65816::serialize(s);
  Thread::serialize(s);
  PPUcounter::serialize(s);

  s.array(wram);

  s.integer(version);

  s.integer(counter.cpu);
  s.integer(counter.dma);

  s.integer(status.clockCount);

  s.integer(status.irqLock);

  s.integer(status.dramRefreshPosition);
  s.integer(status.dramRefresh);

  s.integer(status.hdmaSetupPosition);
  s.integer(status.hdmaSetupTriggered);

  s.integer(status.hdmaPosition);
  s.integer(status.hdmaTriggered);

  s.boolean(status.nmiValid);
  s.boolean(status.nmiLine);
  s.boolean(status.nmiTransition);
  s.boolean(status.nmiPending);
  s.boolean(status.nmiHold);

  s.boolean(status.irqValid);
  s.boolean(status.irqLine);
  s.boolean(status.irqTransition);
  s.boolean(status.irqPending);
  s.boolean(status.irqHold);

  s.integer(status.resetPending);
  s.integer(status.interruptPending);

  s.integer(status.dmaActive);
  s.integer(status.dmaPending);
  s.integer(status.hdmaPending);
  s.integer(status.hdmaMode);

  s.integer(status.autoJoypadCounter);

  s.integer(io.wramAddress);

  s.boolean(io.hirqEnable);
  s.boolean(io.virqEnable);
  s.boolean(io.irqEnable);
  s.boolean(io.nmiEnable);
  s.boolean(io.autoJoypadPoll);

  s.integer(io.pio);

  s.integer(io.wrmpya);
  s.integer(io.wrmpyb);

  s.integer(io.wrdiva);
  s.integer(io.wrdivb);

  s.integer(io.htime);
  s.integer(io.vtime);

  s.integer(io.fastROM);

  s.integer(io.rddiv);
  s.integer(io.rdmpy);

  s.integer(io.joy1);
  s.integer(io.joy2);
  s.integer(io.joy3);
  s.integer(io.joy4);

  s.integer(alu.mpyctr);
  s.integer(alu.divctr);
  s.integer(alu.shift);

  for(Channel& channel : channels) {
    s.integer(channel.dmaEnable);
    s.integer(channel.hdmaEnable);
    s.integer(channel.direction);
    s.integer(channel.indirect);
    s.integer(channel.unused);
    s.integer(channel.reverseTransfer);
    s.integer(channel.fixedTransfer);
    s.integer(channel.transferMode);
    s.integer(channel.targetAddress);
    s.integer(channel.sourceAddress);
    s.integer(channel.sourceBank);
    s.integer(channel.transferSize);
    s.integer(channel.indirectBank);
    s.integer(channel.hdmaAddress);
    s.integer(channel.lineCounter);
    s.integer(channel.unknown);
    s.integer(channel.hdmaCompleted);
    s.integer(channel.hdmaDoTransfer);
  }
}

void CPU::synchronizeSMP() {
  if(smp.clock < 0) scheduler.resume(smp.thread);
}

void CPU::synchronizePPU() {
  if(ppu.clock < 0) scheduler.resume(ppu.thread);
}

void CPU::synchronizeCoprocessors() {
  for(Thread* coprocessor : coprocessors) {
    if(coprocessor->clock < 0) scheduler.resume(coprocessor->thread);
  }
}

[[noreturn]] static void Enter() {
  while(true) {
    scheduler.synchronize();
    cpu.main();
  }
}

void CPU::main() {
  if(r.wai) return instructionWait();
  else if(r.stp) return instructionStop();
  else if(!status.interruptPending) return instruction();

  else if(status.nmiPending) {
    status.nmiPending = 0;
    r.vector = r.e ? 0xfffa : 0xffea;
    return interrupt();
  }

  else if(status.irqPending) {
    status.irqPending = 0;
    r.vector = r.e ? 0xfffe : 0xffee;
    return interrupt();
  }

  else if(status.resetPending) {
    status.resetPending = 0;
    for(unsigned repeat = 0; repeat < 22; ++repeat) step<6,0>(); //step(132);
    r.vector = 0xfffc;
    return interrupt();
  }

  status.interruptPending = 0;
}

bool CPU::load() {
  if(configuration.system.cpu.version <= 1) version = 1;
  else version = 2;
  return true;
}

void CPU::power(bool reset) {
  WDC65816::power();
  Thread::create(Enter, system.cpuFrequency());
  coprocessors.clear();
  PPUcounter::reset();
  PPUcounter::scanline = {&CPU::scanline, this};

  bfunction<uint8_t (unsigned, uint8_t)> reader;
  bfunction<void  (unsigned, uint8_t)> writer;

  reader = {&CPU::readRAM, this};
  writer = {&CPU::writeRAM, this};
  bus.map(reader, writer, "00-3f,80-bf:0000-1fff", 0x2000);
  bus.map(reader, writer, "7e-7f:0000-ffff", 0x20000);

  reader = {&CPU::readAPU, this};
  writer = {&CPU::writeAPU, this};
  bus.map(reader, writer, "00-3f,80-bf:2140-217f");

  reader = {&CPU::readCPU, this};
  writer = {&CPU::writeCPU, this};
  bus.map(reader, writer, "00-3f,80-bf:2180-2183,4016-4017,4200-421f");

  reader = {&CPU::readDMA, this};
  writer = {&CPU::writeDMA, this};
  bus.map(reader, writer, "00-3f,80-bf:4300-437f");

  if(!reset) random.array(wram, sizeof(wram));

  if(init)
    for(uint8_t& byte : wram) byte = 0xff;

  for(unsigned n = 0; n < 8; ++n) {
    channels[n] = {};
    if(n != 7) channels[n].next = &channels[n + 1];
    else channels[n].next = nullptr;
  }

  counter = {};
  io = {};
  alu = {};

  status = {};
  status.dramRefreshPosition = (version == 1 ? 530 : 538);
  status.hdmaSetupPosition = (version == 1 ? 12 + 8 - dmaCounter() : 12 + dmaCounter());
  status.hdmaPosition = 1104;
  status.resetPending = 1;
  status.interruptPending = 1;
}

}
