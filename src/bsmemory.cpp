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

#include <cmath>

#include "serializer.hpp"
#include "memory.hpp"
#include "sfc.hpp"
#include "cpu.hpp"

#include "random.hpp"
#include "bsmemory.hpp"

namespace SuperFamicom {

BSMemory bsmemory;

void BSMemory::serialize(serializer& s) {
  if(ROM) return;
  Thread::serialize(s);

  s.array(memory.data(), memory.size());

  s.integer(pin.writable);

  s.integer(chip.vendor);
  s.integer(chip.device);
  s.integer(chip.serial);

  s.array(page.buffer[0]);
  s.array(page.buffer[1]);

  for(auto& _block : blocks) {
    s.integer(_block.id);
    s.integer(_block.erased);
    s.integer(_block.locked);
    s.integer(_block.erasing);
    s.integer(_block.status.vppLow);
    s.integer(_block.status.queueFull);
    s.integer(_block.status.aborted);
    s.integer(_block.status.failed);
    s.integer(_block.status.locked);
    s.integer(_block.status.ready);
  }

  s.integer(compatible.status.vppLow);
  s.integer(compatible.status.writeFailed);
  s.integer(compatible.status.eraseFailed);
  s.integer(compatible.status.eraseSuspended);
  s.integer(compatible.status.ready);

  s.integer(global.status.page);
  s.integer(global.status.pageReady);
  s.integer(global.status.pageAvailable);
  s.integer(global.status.queueFull);
  s.integer(global.status.sleeping);
  s.integer(global.status.failed);
  s.integer(global.status.suspended);
  s.integer(global.status.ready);

  s.integer(mode);

  s.integer(readyBusyMode);

  queue.serialize(s);
}

void BSMemory::Queue::serialize(serializer& s) {
  s.integer(history[0].valid);
  s.integer(history[0].address);
  s.integer(history[0].data);

  s.integer(history[1].valid);
  s.integer(history[1].address);
  s.integer(history[1].data);

  s.integer(history[2].valid);
  s.integer(history[2].address);
  s.integer(history[2].data);

  s.integer(history[3].valid);
  s.integer(history[3].address);
  s.integer(history[3].data);
}

bool BSMemory::writable() const {
  return pin.writable;
}

void BSMemory::writable(bool writable) {
  pin.writable = !ROM && writable;
}

BSMemory::BSMemory() {
  page.self = this;
  unsigned blockID = 0;
  for(auto& _block : blocks) _block.self = this, _block.id = blockID++;
  block.self = this;
}

void BSMemory::synchronizeCPU() {
  if(clock >= 0) scheduler.resume(cpu.thread);
}

void BSMemory::Enter() {
  while(true) {
    scheduler.synchronize();
    bsmemory.main();
  }
}

void BSMemory::main() {
  if(ROM) return step(1'000'000);  //1 second

  for(unsigned n = 0; n < block.count(); ++n) {
    if(block(n).erasing) return block(n).erase();
    block(n).status.ready = 1;
  }

  compatible.status.ready = 1;
  global.status.ready = 1;
  step(10'000);  //10 milliseconds
}

void BSMemory::step(unsigned clocks) {
  clock += clocks * (uint64_t)cpu.frequency;
  synchronizeCPU();
}

bool BSMemory::load() {
  if(ROM) return true;

  if(size() != 0x100000 && size() != 0x200000 && size() != 0x400000) {
    memory.reset();
    return false;
  }

  chip.vendor = 0x00'b0;  //Sharp
  if(size() == 0x100000) chip.device = 0x66'a8;  //LH28F800SU
  if(size() == 0x200000) chip.device = 0x66'88;  //LH28F016SU
  if(size() == 0x400000) chip.device = 0x66'88;  //LH28F032SU (same device ID as LH28F016SU per datasheet)
  chip.serial = 0x00'01'23'45'67'89ull;  //serial# should be unique for every cartridge ...

  //page buffer values decay to random noise upon losing power to the flash chip
  //the randomness is high entropy (at least compared to SNES SRAM/DRAM chips)
  for(auto& byte : page.buffer[0]) byte = random();
  for(auto& byte : page.buffer[1]) byte = random();

  for(auto& _block : blocks) {
    _block.erased = 1;
    _block.locked = 1;
  }

  /*if(auto fp = platform->open(pathID, "metadata.bml", File::Read, File::Optional)) {
    auto document = BML::unserialize(fp->reads());
    if(auto node = document["flash/vendor"]) {
      chip.vendor = node.natural();
    }
    if(auto node = document["flash/device"]) {
      chip.device = node.natural();
    }
    if(auto node = document["flash/serial"]) {
      chip.serial = node.natural();
    }
    for(unsigned id : nall::range(block.count())) {
      if(auto node = document[{"flash/block(id=", id, ")"}]) {
        if(auto erased = node["erased"]) {
          block(id).erased = erased.natural();
        }
        if(auto locked = node["locked"]) {
          block(id).locked = locked.boolean();
        }
      }
    }
  }*/

  return true;
}

void BSMemory::unload() {
  if(ROM) return memory.reset();

  /*if(auto fp = platform->open(pathID, "metadata.bml", File::Write, File::Optional)) {
    nall::string manifest;
    manifest.append("flash\n");
    manifest.append("  vendor: 0x", nall::hex(chip.vendor,  4L), "\n");
    manifest.append("  device: 0x", nall::hex(chip.device,  4L), "\n");
    manifest.append("  serial: 0x", nall::hex(chip.serial, 12L), "\n");
    for(nall::Natural< 6> id : nall::range(block.count())) {
      manifest.append("  block\n");
      manifest.append("    id: ", id, "\n");
      manifest.append("    erased: ", (unsigned)block(id).erased, "\n");
      manifest.append("    locked: ", (bool)block(id).locked, "\n");
    }
    fp->writes(manifest);
  }*/

  memory.reset();
}

void BSMemory::power() {
  create(Enter, 1'000'000);  //microseconds

  for(auto& _block : blocks) {
    _block.erasing = 0;
    _block.status = {};
  }
  compatible.status = {};
  global.status = {};
  mode = Mode::Flash;
  readyBusyMode = ReadyBusyMode::Disable;
  queue.flush();
}

uint8_t* BSMemory::data() {
  return memory.data();
}

unsigned BSMemory::size() const {
  return memory.size();
}

uint8_t BSMemory::read(unsigned address, uint8_t data) {
  if(!size()) return data;
  if(ROM) return memory.read(bus.mirror(address, size()));

  if(mode == Mode::Chip) {
    if(address == 0) return chip.vendor;  //only appears once
    if(address == 1) return chip.device;  //only appears once
    if((address & 0x07) == 2) return 0x63;  //unknown constant: repeats every eight bytes
    return 0x20;  //unknown constant: fills in all remaining bytes
  }

  if(mode == Mode::Page) {
    return page.read(address);
  }

  if(mode == Mode::CompatibleStatus) {
    return compatible.status();
  }

  if(mode == Mode::ExtendedStatus) {
    if((uint16_t)address == 0x0002) return block(address >> block.bitCount()).status();
    if((uint16_t)address == 0x0004) return global.status();
    return 0x00;  //reserved: always zero
  }

  return block(address >> block.bitCount()).read(address);  //Mode::Flash
}

void BSMemory::write(unsigned address, uint8_t data) {
  if(!size() || ROM) return;
  queue.push(address, data);

  //write page to flash
  if(queue.data(0) == 0x0c) {
  if(queue.size() < 3) return;
    uint16_t count;  //1 - 65536
    count  = queue.data(!(queue.address(1) & 1) ? 1 : 2) << 0;
    count |= queue.data(!(queue.address(1) & 1) ? 2 : 1) << 8;
    unsigned addr = queue.address(2);
    do {
      block(addr >> block.bitCount()).write(addr, page.read(addr));
      ++addr;
    } while(count--);
    page.swap();
    mode = Mode::CompatibleStatus;
    return queue.flush();
  }

  //write byte
  if(queue.data(0) == 0x10) {
  if(queue.size() < 2) return;
    block(queue.address(1) >> block.bitCount()).write(queue.address(1), queue.data(1));
    mode = Mode::CompatibleStatus;
    return queue.flush();
  }

  //erase block
  if(queue.data(0) == 0x20) {
  if(queue.size() < 2) return;
  if(queue.data(1) != 0xd0) return failed(), queue.flush();
    block(queue.address(1) >> block.bitCount()).erase();
    mode = Mode::CompatibleStatus;
    return queue.flush();
  }

  //LH28F800SUT-ZI specific? (undocumented / unavailable? for the LH28F800SU)
  //write signature, identifier, serial# into current page buffer, then swap page buffers
  if(queue.data(0) == 0x38) {
  if(queue.size() < 2) return;
  if(queue.data(1) != 0xd0) return failed(), queue.flush();
    page.write(0x00, 0x4d);  //'M' (memory)
    page.write(0x02, 0x50);  //'P' (pack)
    page.write(0x04, 0x04);  //unknown constant (maybe block count? eg 1<<4 = 16 blocks)
    page.write(0x06, 0x10 | (uint8_t(log2(size() >> 10) + 0.5) & 0x0f));  //d0-d3 = size; d4-d7 = type (1)
    page.write(0x08, (chip.serial >> 40) & 0xff);  //serial# (big endian; BCD format)
    page.write(0x0a, (chip.serial >> 32) & 0xff);  //smallest observed value:
    page.write(0x0c, (chip.serial >> 24) & 0xff);  //  0x00'00'10'62'62'39
    page.write(0x0e, (chip.serial >> 16) & 0xff);  //largest observed value:
    page.write(0x10, (chip.serial >> 8) & 0xff);  //  0x00'91'90'70'31'03
    page.write(0x12, chip.serial & 0xff);  //most values are: 0x00'0x'xx'xx'xx'xx
    page.swap();
    return queue.flush();
  }

  //write byte
  if(queue.data(0) == 0x40) {
  if(queue.size() < 2) return;
    block(queue.address(1) >> block.bitCount()).write(queue.address(1), queue.data(1));
    mode = Mode::CompatibleStatus;
    return queue.flush();
  }

  //clear status register
  if(queue.data(0) == 0x50) {
    for(unsigned n = 0; n < block.count(); ++n) {
      block(n).status.vppLow = 0;
      block(n).status.failed = 0;
    }
    compatible.status.vppLow = 0;
    compatible.status.writeFailed = 0;
    compatible.status.eraseFailed = 0;
    global.status.failed = 0;
    return queue.flush();
  }

  //read compatible status register
  if(queue.data(0) == 0x70) {
    mode = Mode::CompatibleStatus;
    return queue.flush();
  }

  //read extended status registers
  if(queue.data(0) == 0x71) {
    mode = Mode::ExtendedStatus;
    return queue.flush();
  }

  //page buffer swap
  if(queue.data(0) == 0x72) {
    page.swap();
    return queue.flush();
  }

  //single load to page buffer
  if(queue.data(0) == 0x74) {
  if(queue.size() < 2) return;
    page.write(queue.address(1), queue.data(1));
    return queue.flush();
  }

  //read page buffer
  if(queue.data(0) == 0x75) {
    mode = Mode::Page;
    return queue.flush();
  }

  //lock block
  if(queue.data(0) == 0x77) {
  if(queue.size() < 2) return;
  if(queue.data(1) != 0xd0) return failed(), queue.flush();
    block(queue.address(1) >> block.bitCount()).lock();
    return queue.flush();
  }

  //abort
  //(unsupported)
  if(queue.data(0) == 0x80) {
    global.status.sleeping = 1;  //abort seems to put the chip into sleep mode
    return queue.flush();
  }

  //read chip identifiers
  if(queue.data(0) == 0x90) {
    mode = Mode::Chip;
    return queue.flush();
  }

  //update ry/by mode
  //(unsupported)
  if(queue.data(0) == 0x96) {
  if(queue.size() < 2) return;
    if(queue.data(1) == 0x01) readyBusyMode = ReadyBusyMode::EnableToLevelMode;
    if(queue.data(1) == 0x02) readyBusyMode = ReadyBusyMode::PulseOnWrite;
    if(queue.data(1) == 0x03) readyBusyMode = ReadyBusyMode::PulseOnErase;
    if(queue.data(1) == 0x04) readyBusyMode = ReadyBusyMode::Disable;
    return queue.flush();
  }

  //upload lock status bits
  if(queue.data(0) == 0x97) {
  if(queue.size() < 2) return;
  if(queue.data(1) != 0xd0) return failed(), queue.flush();
    for(unsigned n = 0; n < block.count(); ++n) block(n).update();
    return queue.flush();
  }

  //upload device information (number of erase cycles per block)
  if(queue.data(0) == 0x99) {
  if(queue.size() < 2) return;
  if(queue.data(1) != 0xd0) return failed(), queue.flush();
    page.write(0x06, 0x06);  //unknown constant
    page.write(0x07, 0x00);  //unknown constant
    for(uint8_t n = 0; n < block.count(); ++n) {
      uint8_t addr2 = 0;
      addr2 += (n >> 0 & 3) * 0x08;  //verified for LH28F800SUT-ZI
      addr2 += (n >> 2 & 3) * 0x40;  //verified for LH28F800SUT-ZI
      addr2 += (n >> 4 & 1) * 0x20;  //guessed for LH28F016SU
      addr2 += (n >> 5 & 1) * 0x04;  //guessed for LH28F032SU; will overwrite unknown constants
      uint32_t erased = 1 << 31 | block(n).erased;  //unknown if d31 is set when erased == 0
      for(unsigned byte = 0; byte < 4; ++byte) {
        page.write(addr2 + byte, erased >> byte * 8);  //little endian
      }
    }
    page.swap();
    return queue.flush();
  }

  //erase all blocks
  if(queue.data(0) == 0xa7) {
  if(queue.size() < 2) return;
  if(queue.data(1) != 0xd0) return failed(), queue.flush();
    for(unsigned n = 0; n < block.count(); ++n) block(n & 0x3f).erase();
    mode = Mode::CompatibleStatus;
    return queue.flush();
  }

  //erase suspend/resume
  //(unsupported)
  if(queue.data(0) == 0xb0) {
  if(queue.size() < 2) return;
  if(queue.data(1) != 0xd0) return failed(), queue.flush();
    mode = Mode::CompatibleStatus;
    return queue.flush();
  }

  //sequential load to page buffer
  if(queue.data(0) == 0xe0) {
  if(queue.size() < 4) return;  //command length = 3 + count
    uint16_t count;  //1 - 65536
    count  = queue.data(1) << 0;  //endian order not affected by queue.address(1).bit(0)
    count |= queue.data(2) << 8;
    page.write(queue.address(3), queue.data(3));
    if(count--) {
      queue.history[1].data = count >> 0;
      queue.history[2].data = count >> 8;
      return queue.pop();  //hack to avoid needing a 65539-entry queue
    } else {
      return queue.flush();
    }
  }

  //sleep
  //(unsupported)
  if(queue.data(0) == 0xf0) {
    //it is currently unknown how to exit sleep mode; other than via chip reset
    global.status.sleeping = 1;
    return queue.flush();
  }

  //write word
  if(queue.data(0) == 0xfb) {
  if(queue.size() < 3) return;
    uint16_t value;
    value  = queue.data(!(queue.address(1) & 1) ? 1 : 2) << 0;
    value |= queue.data(!(queue.address(1) & 1) ? 2 : 1) << 8;
    //writes are always word-aligned: a0 toggles, rather than increments
    block(queue.address(2) >> block.bitCount()).write(queue.address(2) ^ 0, value >> 0);
    block(queue.address(2) >> block.bitCount()).write(queue.address(2) ^ 1, value >> 8);
    mode = Mode::CompatibleStatus;
    return queue.flush();
  }

  //read flash memory
  if(queue.data(0) == 0xff) {
    mode = Mode::Flash;
    return queue.flush();
  }

  //unknown command
  return queue.flush();
}

void BSMemory::failed() {
  compatible.status.writeFailed = 1;  //datasheet specifies these are for write/erase failures
  compatible.status.eraseFailed = 1;  //yet all errors seem to set both of these bits ...
  global.status.failed = 1;
}

void BSMemory::Page::swap() {
  self->global.status.page ^= 1;
}

uint8_t BSMemory::Page::read(uint8_t address) {
  return buffer[self->global.status.page][address];
}

void BSMemory::Page::write(uint8_t address, uint8_t data) {
  buffer[self->global.status.page][address] = data;
}

unsigned BSMemory::BlockInformation::bitCount() const { return 16; }
unsigned BSMemory::BlockInformation::byteCount() const { return 1 << bitCount(); }
unsigned BSMemory::BlockInformation::count() const { return self->size() >> bitCount(); }

uint8_t BSMemory::Block::read(unsigned address) {
  address &= byteCount() - 1;
  return self->memory.read(id << bitCount() | address);
}

void BSMemory::Block::write(unsigned address, uint8_t data) {
  if(!self->writable() && status.locked) {
    status.failed = 1;
    return self->failed();
  }

  //writes to flash can only clear bits
  address &= byteCount() - 1;
  data &= self->memory.read(id << bitCount() | address);
  self->memory.write(id << bitCount() | address, data);
}

void BSMemory::Block::erase() {
  if(cpu.active()) {
    //erase command runs even if the block is not currently writable
    erasing = 1;
    status.ready = 0;
    self->compatible.status.ready = 0;
    self->global.status.ready = 0;
    return;
  }

  self->step(300'000);  //300 milliseconds are required to erase one block
  erasing = 0;

  if(!self->writable() && status.locked) {
    //does not set any failure bits when unsuccessful ...
    return;
  }

  for(unsigned address = 0; address < byteCount(); ++address) {
    self->memory.write(id << bitCount() | address, 0xff);
  }

  erased++;
  locked = 0;
  status.locked = 0;
}

void BSMemory::Block::lock() {
  if(!self->writable()) {
    //produces a failure result even if the page was already locked
    status.failed = 1;
    return self->failed();
  }

  locked = 1;
  status.locked = 1;
}

//at reset, the locked status bit is set
//this command refreshes the true locked status bit from the device
void BSMemory::Block::update() {
  status.locked = locked;
}

auto BSMemory::Blocks::operator()(uint8_t id) -> Block& {
  return self->blocks[id & (count() - 1)];
}

uint8_t BSMemory::Block::Status::operator()() {
  return (  //d0-d1 are reserved; always return zero
    vppLow    << 2
  | queueFull << 3
  | aborted   << 4
  | failed    << 5
  |!locked    << 6  //note: technically the unlocked flag; so locked is inverted here
  | ready     << 7
  );
}

uint8_t BSMemory::Compatible::Status::operator()() {
  return (  //d0-d2 are reserved; always return zero
    vppLow         << 3
  | writeFailed    << 4
  | eraseFailed    << 5
  | eraseSuspended << 6
  | ready          << 7
  );
}

uint8_t BSMemory::Global::Status::operator()() {
  return (
    page          << 0
  | pageReady     << 1
  | pageAvailable << 2
  | queueFull     << 3
  | sleeping      << 4
  | failed        << 5
  | suspended     << 6
  | ready         << 7
  );
}

void BSMemory::Queue::flush() {
  history[0] = {};
  history[1] = {};
  history[2] = {};
  history[3] = {};
}

void BSMemory::Queue::pop() {
  if(history[3].valid) { history[3] = {}; return; }
  if(history[2].valid) { history[2] = {}; return; }
  if(history[1].valid) { history[1] = {}; return; }
  if(history[0].valid) { history[0] = {}; return; }
}

void BSMemory::Queue::push(uint32_t address, uint8_t data) {
  if(!history[0].valid) { history[0] = {true, address, data}; return; }
  if(!history[1].valid) { history[1] = {true, address, data}; return; }
  if(!history[2].valid) { history[2] = {true, address, data}; return; }
  if(!history[3].valid) { history[3] = {true, address, data}; return; }
}

unsigned BSMemory::Queue::size() {
  if(history[3].valid) return 4;
  if(history[2].valid) return 3;
  if(history[1].valid) return 2;
  if(history[0].valid) return 1;
  return 0;
}

uint32_t BSMemory::Queue::address(unsigned index) {
  if(index > 3 || !history[index].valid) return 0;
  return history[index].address;
}

uint8_t BSMemory::Queue::data(unsigned index) {
  if(index > 3 || !history[index].valid) return 0;
  return history[index].data;
}

}
