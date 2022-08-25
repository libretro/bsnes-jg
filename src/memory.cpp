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

#include <sstream>
#include <string>
#include <vector>

#include "memory.hpp"

namespace SuperFamicom {

bool Memory::GlobalWriteEnable = false;
Bus bus;

Bus::~Bus() {
  if(lookup) delete[] lookup;
  if(target) delete[] target;
}

void Bus::reset() {
  for(unsigned id = 0; id < 256; ++id) {
    reader[id].reset();
    writer[id].reset();
    counter[id] = 0;
  }

  if(lookup) delete[] lookup;
  if(target) delete[] target;

  lookup = new uint8_t [16 * 1024 * 1024]();
  target = new uint32_t[16 * 1024 * 1024]();

  reader[0] = [](unsigned, uint8_t data) -> uint8_t { return data; };
  writer[0] = [](unsigned, uint8_t) -> void {};
}

unsigned Bus::map(
  const bfunction<uint8_t (unsigned, uint8_t)>& read,
  const bfunction<void  (unsigned, uint8_t)>& write,
  const std::string& addr, unsigned size, unsigned base, unsigned mask
) {
  unsigned id = 1;
  while(counter[id]) {
    if(++id >= 256) return printf("SFC error: bus map exhausted\n"), 0;
  }

  reader[id] = read;
  writer[id] = write;

  std::stringstream ss(addr);
  std::vector<std::string> p;
  for (std::string i; std::getline(ss, i, ':'); p.push_back(i));

  ss.clear(); ss.str(p[0]);
  std::vector<std::string> banks;
  for (std::string i; std::getline(ss, i, ','); banks.push_back(i));

  ss.clear(); ss.str(p[1]);
  std::vector<std::string> addrs;
  for (std::string i; std::getline(ss, i, ','); addrs.push_back(i));

  for (std::string& bank : banks) {
    ss.clear(); ss.str(bank);
    std::vector<unsigned> bankRange;
    for (std::string i; std::getline(ss, i, '-');
      bankRange.push_back(std::stoul(i, nullptr, 16)));
    if (bankRange.size() == 1) bankRange.push_back(bankRange[0]);

    for (std::string& addr2 : addrs) {
      ss.clear(); ss.str(addr2);
      std::vector<unsigned> addrRange;
      for (std::string i; std::getline(ss, i, '-');
        addrRange.push_back(std::stoul(i, nullptr, 16)));
      if (addrRange.size() == 1) addrRange.push_back(addrRange[0]);

      for(unsigned bank2 = bankRange[0]; bank2 <= bankRange[1]; ++bank2) {
        for(unsigned addr3 = addrRange[0]; addr3 <= addrRange[1]; ++addr3) {
          unsigned pid = lookup[bank2 << 16 | addr3];
          if(pid && --counter[pid] == 0) {
            reader[pid].reset();
            writer[pid].reset();
          }

          unsigned offset = reduce(bank2 << 16 | addr3, mask);
          if(size) base = mirror(base, size);
          if(size) offset = base + mirror(offset, size - base);
          lookup[bank2 << 16 | addr3] = id;
          target[bank2 << 16 | addr3] = offset;
          ++counter[id];
        }
      }
    }
  }

  return id;
}

void Bus::unmap(const std::string& addr) {
  std::stringstream ss(addr);
  std::vector<std::string> p;
  for (std::string i; std::getline(ss, i, ':'); p.push_back(i));

  ss.clear(); ss.str(p[0]);
  std::vector<std::string> banks;
  for (std::string i; std::getline(ss, i, ','); banks.push_back(i));

  ss.clear(); ss.str(p[1]);
  std::vector<std::string> addrs;
  for (std::string i; std::getline(ss, i, ','); addrs.push_back(i));

  for (std::string& bank : banks) {
    ss.clear(); ss.str(bank);
    std::vector<unsigned> bankRange;
    for (std::string i; std::getline(ss, i, '-');
      bankRange.push_back(std::stoul(i, nullptr, 16)));

    for (std::string& addr2 : addrs) {
      ss.clear(); ss.str(addr2);
      std::vector<unsigned> addrRange;
      for (std::string i; std::getline(ss, i, '-');
        addrRange.push_back(std::stoul(i, nullptr, 16)));

      for(unsigned bank2 = bankRange[0]; bank2 <= bankRange[1]; ++bank2) {
        for(unsigned addr3 = addrRange[0]; addr3 <= addrRange[1]; ++addr3) {
          unsigned pid = lookup[bank2 << 16 | addr3];
          if(pid && --counter[pid] == 0) {
            reader[pid].reset();
            writer[pid].reset();
          }

          lookup[bank2 << 16 | addr3] = 0;
          target[bank2 << 16 | addr3] = 0;
        }
      }
    }
  }
}

}
