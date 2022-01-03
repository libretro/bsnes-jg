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
