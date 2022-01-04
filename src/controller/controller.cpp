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

#include "controller.hpp"
#include "../interface.hpp"
#include "../cpu.hpp"

namespace SuperFamicom {

ControllerPort controllerPort1;
ControllerPort controllerPort2;

Controller::Controller(unsigned port) : port(port) {
}

Controller::~Controller() {
}

bool Controller::iobit() {
  switch(port) {
  case ID::Port::Controller1: return cpu.pio() & 0x40;
  case ID::Port::Controller2: return cpu.pio() & 0x80;
  }
  return 0; // unreachable
}

void Controller::iobit(bool data) {
  switch(port) {
  case ID::Port::Controller1: bus.write(0x4201, (cpu.pio() & ~0x40) | (data << 6)); break;
  case ID::Port::Controller2: bus.write(0x4201, (cpu.pio() & ~0x80) | (data << 7)); break;
  }
}

void ControllerPort::connect(unsigned deviceID) {
  if(!system.loaded()) return;
  delete device;

  switch(deviceID) { default:
  case ID::Device::None: device = new Controller(port); break;
  case ID::Device::Gamepad: device = new Gamepad(port); break;
  case ID::Device::Mouse: device = new Mouse(port); break;
  case ID::Device::SuperMultitap: device = new SuperMultitap(port); break;
  case ID::Device::SuperScope: device = new SuperScope(port); break;
  case ID::Device::Justifier: device = new Justifier(port, false); break;
  case ID::Device::Justifiers: device = new Justifier(port, true); break;
  }
}

void ControllerPort::power(unsigned port) {
  this->port = port;
}

void ControllerPort::unload() {
  delete device;
  device = nullptr;
}

void ControllerPort::serialize(serializer& s) {
}

}
