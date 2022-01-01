#include <sstream>
#include <string>
#include <vector>

#include "sfc.hpp"

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
  const nall::function<uint8_t (unsigned, uint8_t)>& read,
  const nall::function<void  (unsigned, uint8_t)>& write,
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

  for (auto& bank : banks) {
    ss.clear(); ss.str(bank);
    std::vector<unsigned> bankRange;
    for (std::string i; std::getline(ss, i, '-');
      bankRange.push_back(std::stoul(i, nullptr, 16)));
    if (bankRange.size() == 1) bankRange.push_back(bankRange[0]);

    for (auto& addr : addrs) {
      ss.clear(); ss.str(addr);
      std::vector<unsigned> addrRange;
      for (std::string i; std::getline(ss, i, '-');
        addrRange.push_back(std::stoul(i, nullptr, 16)));
      if (addrRange.size() == 1) addrRange.push_back(addrRange[0]);

      for(unsigned bank = bankRange[0]; bank <= bankRange[1]; bank++) {
        for(unsigned addr = addrRange[0]; addr <= addrRange[1]; addr++) {
          unsigned pid = lookup[bank << 16 | addr];
          if(pid && --counter[pid] == 0) {
            reader[pid].reset();
            writer[pid].reset();
          }

          unsigned offset = reduce(bank << 16 | addr, mask);
          if(size) base = mirror(base, size);
          if(size) offset = base + mirror(offset, size - base);
          lookup[bank << 16 | addr] = id;
          target[bank << 16 | addr] = offset;
          counter[id]++;
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

  for (auto& bank : banks) {
    ss.clear(); ss.str(bank);
    std::vector<unsigned> bankRange;
    for (std::string i; std::getline(ss, i, '-');
      bankRange.push_back(std::stoul(i, nullptr, 16)));

    for (auto& addr : addrs) {
      ss.clear(); ss.str(addr);
      std::vector<unsigned> addrRange;
      for (std::string i; std::getline(ss, i, '-');
        addrRange.push_back(std::stoul(i, nullptr, 16)));

      for(unsigned bank = bankRange[0]; bank <= bankRange[1]; bank++) {
        for(unsigned addr = addrRange[0]; addr <= addrRange[1]; addr++) {
          unsigned pid = lookup[bank << 16 | addr];
          if(pid && --counter[pid] == 0) {
            reader[pid].reset();
            writer[pid].reset();
          }

          lookup[bank << 16 | addr] = 0;
          target[bank << 16 | addr] = 0;
        }
      }
    }
  }
}

}
