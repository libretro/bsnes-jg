#include <sfc/sfc.hpp>

namespace SuperFamicom {

CPU cpu;

auto CPU::dmaEnable() -> bool {
  for(auto& channel : channels) if(channel.dmaEnable) return true;
  return false;
}

auto CPU::hdmaEnable() -> bool {
  for(auto& channel : channels) if(channel.hdmaEnable) return true;
  return false;
}

auto CPU::hdmaActive() -> bool {
  for(auto& channel : channels) if(channel.hdmaActive()) return true;
  return false;
}

auto CPU::dmaRun() -> void {
  counter.dma += 8;
  step<8,0>();
  dmaEdge();
  for(auto& channel : channels) channel.dmaRun();
  status.irqLock = true;
}

auto CPU::hdmaReset() -> void {
  for(auto& channel : channels) channel.hdmaReset();
}

auto CPU::hdmaSetup() -> void {
  counter.dma += 8;
  step<8,0>();
  for(auto& channel : channels) channel.hdmaSetup();
  status.irqLock = true;
}

auto CPU::hdmaRun() -> void {
  counter.dma += 8;
  step<8,0>();
  for(auto& channel : channels) channel.hdmaTransfer();
  for(auto& channel : channels) channel.hdmaAdvance();
  status.irqLock = true;
}

template<unsigned Clocks, bool Synchronize>
auto CPU::Channel::step() -> void {
  cpu.counter.dma += Clocks;
  cpu.step<Clocks, Synchronize>();
}

auto CPU::Channel::edge() -> void {
  cpu.dmaEdge();
}

auto CPU::Channel::validA(nall::Natural<24> address) -> bool {
  //A-bus cannot access the B-bus or CPU I/O registers
  if((address & 0x40ff00) == 0x2100) return false;  //00-3f,80-bf:2100-21ff
  if((address & 0x40fe00) == 0x4000) return false;  //00-3f,80-bf:4000-41ff
  if((address & 0x40ffe0) == 0x4200) return false;  //00-3f,80-bf:4200-421f
  if((address & 0x40ff80) == 0x4300) return false;  //00-3f,80-bf:4300-437f
  return true;
}

auto CPU::Channel::readA(nall::Natural<24> address) -> uint8_t {
  step<4,1>();
  cpu.r.mdr = validA(address) ? bus.read(address, cpu.r.mdr) : (uint8_t)0x00;
  step<4,1>();
  return cpu.r.mdr;
}

auto CPU::Channel::readB(uint8_t address, bool valid) -> uint8_t {
  step<4,1>();
  cpu.r.mdr = valid ? bus.read(0x2100 | address, cpu.r.mdr) : (uint8_t)0x00;
  step<4,1>();
  return cpu.r.mdr;
}

auto CPU::Channel::writeA(nall::Natural<24> address, uint8_t data) -> void {
  if(validA(address)) bus.write(address, data);
}

auto CPU::Channel::writeB(uint8_t address, uint8_t data, bool valid) -> void {
  if(valid) bus.write(0x2100 | address, data);
}

auto CPU::Channel::transfer(nall::Natural<24> addressA, uint2 index) -> void {
  uint8_t addressB = targetAddress;
  switch(transferMode) {
  case 1: case 5: addressB += index.bit(0); break;
  case 3: case 7: addressB += index.bit(1); break;
  case 4: addressB += index; break;
  }

  //transfers from WRAM to WRAM are invalid
  bool valid = addressB != 0x80 || ((addressA & 0xfe0000) != 0x7e0000 && (addressA & 0x40e000) != 0x0000);

  cpu.r.mar = addressA;
  if(direction == 0) {
    auto data = readA(addressA);
    writeB(addressB, data, valid);
  } else {
    auto data = readB(addressB, valid);
    writeA(addressA, data);
  }
}

auto CPU::Channel::dmaRun() -> void {
  if(!dmaEnable) return;

  step<8,0>();
  edge();

  uint2 index = 0;
  do {
    transfer(sourceBank << 16 | sourceAddress, index++);
    if(!fixedTransfer) !reverseTransfer ? sourceAddress++ : sourceAddress--;
    edge();
  } while(dmaEnable && --transferSize);

  dmaEnable = false;
}

auto CPU::Channel::hdmaActive() -> bool {
  return hdmaEnable && !hdmaCompleted;
}

auto CPU::Channel::hdmaFinished() -> bool {
  auto channel = next;
  while(channel) {
    if(channel->hdmaActive()) return false;
    channel = channel->next;
  }
  return true;
}

auto CPU::Channel::hdmaReset() -> void {
  hdmaCompleted = false;
  hdmaDoTransfer = false;
}

auto CPU::Channel::hdmaSetup() -> void {
  hdmaDoTransfer = true;  //note: needs hardware verification
  if(!hdmaEnable) return;

  dmaEnable = false;  //HDMA will stop active DMA mid-transfer
  hdmaAddress = sourceAddress;
  lineCounter = 0;
  hdmaReload();
}

auto CPU::Channel::hdmaReload() -> void {
  auto data = readA(cpu.r.mar = sourceBank << 16 | hdmaAddress);

  if((nall::Natural< 7>)lineCounter == 0) {
    lineCounter = data;
    hdmaAddress++;

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

auto CPU::Channel::hdmaTransfer() -> void {
  if(!hdmaActive()) return;
  dmaEnable = false;  //HDMA will stop active DMA mid-transfer
  if(!hdmaDoTransfer) return;

  static const unsigned lengths[8] = {1, 2, 2, 4, 4, 4, 2, 4};
  for(uint2 index : range(lengths[transferMode])) {
    nall::Natural<24> address = !indirect ? sourceBank << 16 | hdmaAddress++ : indirectBank << 16 | indirectAddress++;
    transfer(address, index);
  }
}

auto CPU::Channel::hdmaAdvance() -> void {
  if(!hdmaActive()) return;
  lineCounter--;
  hdmaDoTransfer = bool(lineCounter & 0x80);
  hdmaReload();
}

auto CPU::idle() -> void {
  status.clockCount = 6;
  dmaEdge();
  step<6,0>();
  status.irqLock = 0;
  aluEdge();
}

auto CPU::read(unsigned address) -> uint8_t {
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
  } else if(address + 0x6000 & 0x4000) {
    status.clockCount = 8;
    dmaEdge();
    r.mar = address;
    step<4,1>();
  } else if(address - 0x4000 & 0x7e00) {
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
  auto data = bus.read(address, r.mdr);
  step<4,0>();
  aluEdge();
  //$00-3f,80-bf:4000-43ff reads are internal to CPU, and do not update the MDR
  if((address & 0x40fc00) != 0x4000) r.mdr = data;
  return data;
}

auto CPU::write(unsigned address, uint8_t data) -> void {
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
  } else if(address + 0x6000 & 0x4000) {
    status.clockCount = 8;
    dmaEdge();
    r.mar = address;
    step<8,1>();
  } else if(address - 0x4000 & 0x7e00) {
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

auto CPU::readDisassembler(unsigned address) -> uint8_t {
  return bus.read(address, r.mdr);
}

auto CPU::readRAM(unsigned addr, uint8_t data) -> uint8_t {
  return wram[addr];
}

auto CPU::readAPU(unsigned addr, uint8_t data) -> uint8_t {
  synchronizeSMP();
  return smp.portRead(addr & 3);
}

auto CPU::readCPU(unsigned addr, uint8_t data) -> uint8_t {
  switch(addr & 0xffff) {
  case 0x2180:  //WMDATA
    return bus.read(0x7e0000 | io.wramAddress++, data);

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
    data |= (uint4)version;
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

  }

  return data;
}

auto CPU::readDMA(unsigned addr, uint8_t data) -> uint8_t {
  auto& channel = this->channels[addr >> 4 & 7];

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

  }

  return data;
}

auto CPU::writeRAM(unsigned addr, uint8_t data) -> void {
  wram[addr] = data;
}

auto CPU::writeAPU(unsigned addr, uint8_t data) -> void {
  synchronizeSMP();
  return smp.portWrite(addr & 3, data);
}

auto CPU::writeCPU(unsigned addr, uint8_t data) -> void {
  switch(addr & 0xffff) {

  case 0x2180:  //WMDATA
    return bus.write(0x7e0000 | io.wramAddress++, data);

  case 0x2181:  //WMADDL
    io.wramAddress = io.wramAddress & 0x1ff00 | data << 0;
    return;

  case 0x2182:  //WMADDM
    io.wramAddress = io.wramAddress & 0x100ff | data << 8;
    return;

  case 0x2183:  //WMADDH
    io.wramAddress = io.wramAddress & 0x0ffff | (data & 1) << 16;
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

    if(!configuration.hacks.cpu.fastMath) {
      alu.mpyctr = 8;  //perform multiplication over the next eight cycles
      alu.shift = io.wrmpyb;
    } else {
      io.rdmpy = io.wrmpya * io.wrmpyb;
    }
    return;

  case 0x4204:  //WRDIVL
    io.wrdiva = io.wrdiva & 0xff00 | data << 0;
    return;

  case 0x4205:  //WRDIVH
    io.wrdiva = io.wrdiva & 0x00ff | data << 8;
    return;

  case 0x4206:  //WRDIVB
    io.rdmpy = io.wrdiva;
    if(alu.mpyctr || alu.divctr) return;

    io.wrdivb = data;

    if(!configuration.hacks.cpu.fastMath) {
      alu.divctr = 16;  //perform division over the next sixteen cycles
      alu.shift = io.wrdivb << 16;
    } else {
      if(io.wrdivb) {
        io.rddiv = io.wrdiva / io.wrdivb;
        io.rdmpy = io.wrdiva % io.wrdivb;
      } else {
        io.rddiv = 0xffff;
        io.rdmpy = io.wrdiva;
      }
    }
    return;

  case 0x4207:  //HTIMEL
    io.htime = (io.htime >> 2) - 1;
    io.htime = io.htime & 0x100 | data << 0;
    io.htime = (io.htime + 1) << 2;
    irqPoll();  //unverified
    return;

  case 0x4208:  //HTIMEH
    io.htime = (io.htime >> 2) - 1;
    io.htime = io.htime & 0x0ff | (data & 1) << 8;
    io.htime = (io.htime + 1) << 2;
    irqPoll();  //unverified
    return;

  case 0x4209:  //VTIMEL
    io.vtime = io.vtime & 0x100 | data << 0;
    irqPoll();  //unverified
    return;

  case 0x420a:  //VTIMEH
    io.vtime = io.vtime & 0x0ff | (data & 1) << 8;
    irqPoll();  //unverified
    return;

  case 0x420b:  //DMAEN
    for(auto n : range(8)) channels[n].dmaEnable = bool(data & 1 << n);
    if(data) status.dmaPending = true;
    return;

  case 0x420c:  //HDMAEN
    for(auto n : range(8)) channels[n].hdmaEnable = bool(data & 1 << n);
    return;

  case 0x420d:  //MEMSEL
    io.fastROM = data & 1;
    return;

  }
}

auto CPU::writeDMA(unsigned addr, uint8_t data) -> void {
  auto& channel = this->channels[addr >> 4 & 7];

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
    channel.sourceAddress = channel.sourceAddress & 0xff00 | data << 0;
    return;

  case 0x4303:  //A1TxH
    channel.sourceAddress = channel.sourceAddress & 0x00ff | data << 8;
    return;

  case 0x4304:  //A1Bx
    channel.sourceBank = data;
    return;

  case 0x4305:  //DASxL
    channel.transferSize = channel.transferSize & 0xff00 | data << 0;
    return;

  case 0x4306:  //DASxH
    channel.transferSize = channel.transferSize & 0x00ff | data << 8;
    return;

  case 0x4307:  //DASBx
    channel.indirectBank = data;
    return;

  case 0x4308:  //A2AxL
    channel.hdmaAddress = channel.hdmaAddress & 0xff00 | data << 0;
    return;

  case 0x4309:  //A2AxH
    channel.hdmaAddress = channel.hdmaAddress & 0x00ff | data << 8;
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
auto CPU::dmaCounter() const -> unsigned {
  return counter.cpu & 7;
}

//joypad auto-poll clock divider
auto CPU::joypadCounter() const -> unsigned {
  return counter.cpu & 127;
}

auto CPU::stepOnce() -> void {
  counter.cpu += 2;
  tick();
  if(hcounter() & 2) nmiPoll(), irqPoll();
  if(joypadCounter() == 0) joypadEdge();
}

template<unsigned Clocks, bool Synchronize>
auto CPU::step() -> void {
  static_assert(Clocks == 2 || Clocks == 4 || Clocks == 6 || Clocks == 8 || Clocks == 10 || Clocks == 12);

  for(auto coprocessor : coprocessors) {
    if(coprocessor == &icd || coprocessor == &msu1) continue;
    coprocessor->clock -= Clocks * (uint64_t)coprocessor->frequency;
  }

  if(overclocking.target) {
    overclocking.counter += Clocks;
    if(overclocking.counter < overclocking.target) {
      if constexpr(Synchronize) {
        if(configuration.hacks.coprocessor.delayedSync) return;
        synchronizeCoprocessors();
      }
      return;
    }
  }

  if constexpr(Clocks >=  2) stepOnce();
  if constexpr(Clocks >=  4) stepOnce();
  if constexpr(Clocks >=  6) stepOnce();
  if constexpr(Clocks >=  8) stepOnce();
  if constexpr(Clocks >= 10) stepOnce();
  if constexpr(Clocks >= 12) stepOnce();

  smp.clock -= Clocks * (uint64_t)smp.frequency;
  ppu.clock -= Clocks;
  for(auto coprocessor : coprocessors) {
    if(coprocessor != &icd && coprocessor != &msu1) continue;
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

  if constexpr(Synchronize) {
    if(configuration.hacks.coprocessor.delayedSync) return;
    synchronizeCoprocessors();
  }
}

auto CPU::step(unsigned clocks) -> void {
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
auto CPU::scanline() -> void {
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

  //overclocking
  if(vcounter() == (Region::NTSC() ? 261 : 311)) {
    overclocking.counter = 0;
    overclocking.target = 0;
    double overclock = configuration.hacks.cpu.overclock / 100.0;
    if(overclock > 1.0) {
      int clocks = (Region::NTSC() ? 262 : 312) * 1364;
      overclocking.target = clocks * overclock - clocks;
    }
  }

  //handle video frame events from the CPU core to prevent a race condition between
  //games polling inputs during NMI and the PPU thread not being 100% synchronized.
  if(vcounter() == ppu.vdisp()) {
    if(auto device = controllerPort2.device) device->latch();  //light guns
    synchronizePPU();
    scheduler.leave(Scheduler::Event::Frame);
  }
}

auto CPU::aluEdge() -> void {
  if(alu.mpyctr) {
    alu.mpyctr--;
    if(io.rddiv & 1) io.rdmpy += alu.shift;
    io.rddiv >>= 1;
    alu.shift <<= 1;
  }

  if(alu.divctr) {
    alu.divctr--;
    io.rddiv <<= 1;
    alu.shift >>= 1;
    if(io.rdmpy >= alu.shift) {
      io.rdmpy -= alu.shift;
      io.rddiv |= 1;
    }
  }
}

auto CPU::dmaEdge() -> void {
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

  if(!status.dmaActive) {
    if(status.dmaPending || status.hdmaPending) {
      status.dmaActive = true;
    }
  }
}

//called every 128 clocks from inside the CPU::stepOnce() function
auto CPU::joypadEdge() -> void {
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
    uint2 port0 = controllerPort1.device->data();
    uint2 port1 = controllerPort2.device->data();

    io.joy1 = io.joy1 << 1 | port0.bit(0);
    io.joy2 = io.joy2 << 1 | port1.bit(0);
    io.joy3 = io.joy3 << 1 | port0.bit(1);
    io.joy4 = io.joy4 << 1 | port1.bit(1);
  }

  status.autoJoypadCounter++;
}

//nmiPoll() and irqPoll() are called once every four clock cycles;
//as NMI steps by scanlines (divisible by 4) and IRQ by PPU 4-cycle dots.
//
//ppu.(vh)counter(n) returns the value of said counters n-clocks before current time;
//it is used to emulate hardware communication delay between opcode and interrupt units.

auto CPU::nmiPoll() -> void {
  //NMI hold
  if(status.nmiHold.lower() && io.nmiEnable) {
    status.nmiTransition = 1;
  }

  //NMI test
  if(status.nmiValid.flip(vcounter(2) >= ppu.vdisp())) {
    if(status.nmiLine = status.nmiValid) status.nmiHold = 1;  //hold /NMI for four cycles
  }
}

auto CPU::irqPoll() -> void {
  //IRQ hold
  status.irqHold = 0;
  if(status.irqLine && io.irqEnable) {
    status.irqTransition = 1;
  }

  //IRQ test
  if(status.irqValid.raise(io.irqEnable
  && (!io.virqEnable || vcounter(10) == io.vtime)
  && (!io.hirqEnable || hcounter(10) == io.htime)
  && (vcounter(6) || hcounter(6))  //IRQs cannot trigger on last dot of fields
  )) status.irqLine = status.irqHold = 1;  //hold /IRQ for four cycles
}

auto CPU::nmitimenUpdate(uint8_t data) -> void {
  io.hirqEnable = data & 0x10;
  io.virqEnable = data & 0x20;
  io.irqEnable = io.hirqEnable || io.virqEnable;

  if(io.virqEnable && !io.hirqEnable && status.irqLine) {
    status.irqTransition = 1;
  } else if(!io.irqEnable) {
    status.irqLine = 0;
    status.irqTransition = 0;
  }

  if(io.nmiEnable.raise(data & 0x80) && status.nmiLine) {
    status.nmiTransition = 1;
  }

  status.irqLock = 1;
}

auto CPU::rdnmi() -> bool {
  bool result = status.nmiLine;
  if(!status.nmiHold) {
    status.nmiLine = 0;
  }
  return result;
}

auto CPU::timeup() -> bool {
  bool result = status.irqLine;
  if(!status.irqHold) {
    status.irqLine = 0;
    status.irqTransition = 0;
  }
  return result;
}

auto CPU::nmiTest() -> bool {
  if(!status.nmiTransition) return 0;
  status.nmiTransition = 0;
  r.wai = 0;
  return 1;
}

auto CPU::irqTest() -> bool {
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
auto CPU::lastCycle() -> void {
  if(!status.irqLock) {
    if(nmiTest()) status.nmiPending = 1, status.interruptPending = 1;
    if(irqTest()) status.irqPending = 1, status.interruptPending = 1;
  }
}

auto CPU::serialize(serializer& s) -> void {
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

  for(auto& channel : channels) {
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

auto CPU::synchronizeSMP() -> void {
  if(smp.clock < 0) scheduler.resume(smp.thread);
}

auto CPU::synchronizePPU() -> void {
  if(ppu.clock < 0) scheduler.resume(ppu.thread);
}

auto CPU::synchronizeCoprocessors() -> void {
  for(auto coprocessor : coprocessors) {
    if(coprocessor->clock < 0) scheduler.resume(coprocessor->thread);
  }
}

auto CPU::Enter() -> void {
  while(true) {
    scheduler.synchronize();
    cpu.main();
  }
}

auto CPU::main() -> void {
  if(r.wai) return instructionWait();
  if(r.stp) return instructionStop();
  if(!status.interruptPending) return instruction();

  if(status.nmiPending) {
    status.nmiPending = 0;
    r.vector = r.e ? 0xfffa : 0xffea;
    return interrupt();
  }

  if(status.irqPending) {
    status.irqPending = 0;
    r.vector = r.e ? 0xfffe : 0xffee;
    return interrupt();
  }

  if(status.resetPending) {
    status.resetPending = 0;
    for(unsigned repeat : range(22)) step<6,0>();  //step(132);
    r.vector = 0xfffc;
    return interrupt();
  }

  status.interruptPending = 0;
}

auto CPU::load() -> bool {
  version = configuration.system.cpu.version;
  if(version < 1) version = 1;
  if(version > 2) version = 2;
  return true;
}

auto CPU::power(bool reset) -> void {
  WDC65816::power();
  Thread::create(Enter, system.cpuFrequency());
  coprocessors.reset();
  PPUcounter::reset();
  PPUcounter::scanline = {&CPU::scanline, this};

  function<uint8_t (unsigned, uint8_t)> reader;
  function<void  (unsigned, uint8_t)> writer;

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

  if(configuration.hacks.hotfixes) {
    //Dirt Racer (Europe) relies on uninitialized memory containing certain values to boot without freezing.
    //the game itself is broken and will fail to run sometimes on real hardware, but for the sake of expedience,
    //WRAM is initialized to a constant value that will allow this game to always boot in successfully.
    if(cartridge.headerTitle() == "DIRT RACER") {
      for(auto& byte : wram) byte = 0xff;
    }
  }

  for(unsigned n : range(8)) {
    channels[n] = {};
    if(n != 7) channels[n].next = channels[n + 1];
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
