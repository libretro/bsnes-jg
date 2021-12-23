#include "../sfc.hpp"

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
