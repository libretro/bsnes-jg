#include "../sfc.hpp"

namespace SuperFamicom {

ExpansionPort expansionPort;

Expansion::Expansion() {
}

Expansion::~Expansion() {
}

void ExpansionPort::connect(unsigned deviceID) {
  if(!system.loaded()) return;
  delete device;

  switch(deviceID) { default:
  case ID::Device::None: device = new Expansion; break;
  case ID::Device::Satellaview: device = new Satellaview; break;
//case ID::Device::S21FX: device = new S21FX; break;
  }
}

void ExpansionPort::power() {
}

void ExpansionPort::unload() {
  delete device;
  device = nullptr;
}

void ExpansionPort::serialize(serializer& s) {
}

}
