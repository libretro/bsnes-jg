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

#include "expansion.hpp"

#include "21fx.hpp"

namespace SuperFamicom {

S21FX::S21FX() {
  create(S21FX::Enter, 10'000'000);

  resetVector.byte(0) = bus.read(0xfffc, 0x00);
  resetVector.byte(1) = bus.read(0xfffd, 0x00);

  bus.map({&S21FX::read, this}, {&S21FX::write, this}, "00-3f,80-bf:2184-21ff");
  bus.map({&S21FX::read, this}, {&S21FX::write, this}, "00:fffc-fffd");

  booted = false;

  for(auto& byte : ram) byte = 0xdb;  //stp
  ram[0] = 0x6c;  //jmp ($fffc)
  ram[1] = 0xfc;
  ram[2] = 0xff;

  if(auto buffer = file::read({platform->path(ID::System), "21fx.rom"})) {
    memory::copy(ram, sizeof(ram), buffer.data(), buffer.size());
  }

  string filename{platform->path(ID::SuperFamicom), "21fx.so"};
  if(link.openAbsolute(filename)) {
    linkInit = link.sym("fx_init");
    linkMain = link.sym("fx_main");
  }
}

S21FX::~S21FX() {
  scheduler.remove(*this);
  bus.unmap("00-3f,80-bf:2184-21ff");
  bus.unmap("00:fffc-fffd");

  //note: this is an awful hack ...
  //since the bus maps are lambdas, we can't safely restore the original reset vector handler
  //as such, we install a basic read-only lambda that simply returns the known reset vector
  //the downside is that if 00:fffc-fffd were anything but ROM; it will now only act as ROM
  //given that this is the only device that hooks the reset vector like this,
  //it's not worth the added complexity to support some form of reversible bus mapping hooks
  unsigned vector = resetVector;
  bus.map([vector](nall::Natural<24> addr, uint8_t) -> uint8_t {
    return vector >> addr * 8;
  }, [](nall::Natural<24>, uint8_t) -> void {
  }, "00:fffc-fffd", 2);

  if(link.open()) link.close();
  linkInit.reset();
  linkMain.reset();
}

auto S21FX::Enter() -> void {
  while(true) scheduler.synchronize(), expansionPort.device->main();
}

auto S21FX::step(unsigned clocks) -> void {
  Thread::step(clocks);
  synchronize(cpu);
}

auto S21FX::main() -> void {
  if(linkInit) linkInit(
    {&S21FX::quit, this},
    {&S21FX::usleep, this},
    {&S21FX::readable, this},
    {&S21FX::writable, this},
    {&S21FX::read, this},
    {&S21FX::write, this}
  );
  if(linkMain) linkMain({});
  while(true) step(10'000'000);
}

auto S21FX::read(unsigned addr, uint8_t data) -> uint8_t {
  addr &= 0x40ffff;

  if(addr == 0xfffc) return booted ? resetVector.byte(0) : (uint8_t)0x84;
  if(addr == 0xfffd) return booted ? resetVector.byte(1) : (booted = true, (uint8_t)0x21);

  if(addr >= 0x2184 && addr <= 0x21fd) return ram[addr - 0x2184];

  if(addr == 0x21fe) return !link.open() ? 0 : (
    (linkBuffer.size() >    0) << 7  //1 = readable
  | (snesBuffer.size() < 1024) << 6  //1 = writable
  | (link.open())              << 5  //1 = connected
  );

  if(addr == 0x21ff) {
    if(linkBuffer.size() > 0) {
      return linkBuffer.takeLeft();
    }
  }

  return data;
}

auto S21FX::write(unsigned addr, uint8_t data) -> void {
  addr &= 0x40ffff;

  if(addr == 0x21ff) {
    if(snesBuffer.size() < 1024) {
      snesBuffer.append(data);
    }
  }
}

auto S21FX::quit() -> bool {
  step(1);
  return false;
}

auto S21FX::usleep(unsigned microseconds) -> void {
  step(10 * microseconds);
}

auto S21FX::readable() -> bool {
  step(1);
  return snesBuffer.size() > 0;
}

auto S21FX::writable() -> bool {
  step(1);
  return linkBuffer.size() < 1024;
}

//SNES -> Link
auto S21FX::read() -> uint8_t {
  step(1);
  if(snesBuffer.size() > 0) {
    return snesBuffer.takeLeft();
  }
  return 0x00;
}

//Link -> SNES
auto S21FX::write(uint8_t data) -> void {
  step(1);
  if(linkBuffer.size() < 1024) {
    linkBuffer.append(data);
  }
}

}
