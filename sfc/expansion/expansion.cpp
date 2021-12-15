#include <sfc/sfc.hpp>

namespace SuperFamicom {

ExpansionPort expansionPort;

Expansion::Expansion() {
}

Expansion::~Expansion() {
}

auto ExpansionPort::connect(uint deviceID) -> void {
  if(!system.loaded()) return;
  delete device;

  switch(deviceID) { default:
  case ID::Device::None: device = new Expansion; break;
  case ID::Device::Satellaview: device = new Satellaview; break;
//case ID::Device::S21FX: device = new S21FX; break;
  }
}

auto ExpansionPort::power() -> void {
}

auto ExpansionPort::unload() -> void {
  delete device;
  device = nullptr;
}

auto ExpansionPort::serialize(serializer& s) -> void {
}

}
