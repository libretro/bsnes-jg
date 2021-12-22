#include "sfc.hpp"

namespace SuperFamicom {

bool Memory::GlobalWriteEnable = false;
Bus bus;

Bus::~Bus() {
  if(lookup) delete[] lookup;
  if(target) delete[] target;
}

void Bus::reset() {
  for(unsigned id : nall::range(256)) {
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
  const nall::string& addr, unsigned size, unsigned base, unsigned mask
) {
  unsigned id = 1;
  while(counter[id]) {
    if(++id >= 256) return printf("SFC error: bus map exhausted\n"), 0;
  }

  reader[id] = read;
  writer[id] = write;

  auto p = addr.split(":", 1L);
  auto banks = p(0).split(",");
  auto addrs = p(1).split(",");
  for(auto& bank : banks) {
    for(auto& addr : addrs) {
      auto bankRange = bank.split("-", 1L);
      auto addrRange = addr.split("-", 1L);
      unsigned bankLo = bankRange(0).hex();
      unsigned bankHi = bankRange(1, bankRange(0)).hex();
      unsigned addrLo = addrRange(0).hex();
      unsigned addrHi = addrRange(1, addrRange(0)).hex();

      for(unsigned bank = bankLo; bank <= bankHi; bank++) {
        for(unsigned addr = addrLo; addr <= addrHi; addr++) {
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

void Bus::unmap(const nall::string& addr) {
  auto p = addr.split(":", 1L);
  auto banks = p(0).split(",");
  auto addrs = p(1).split(",");
  for(auto& bank : banks) {
    for(auto& addr : addrs) {
      auto bankRange = bank.split("-", 1L);
      auto addrRange = addr.split("-", 1L);
      unsigned bankLo = bankRange(0).hex();
      unsigned bankHi = bankRange(1, bankRange(0)).hex();
      unsigned addrLo = addrRange(0).hex();
      unsigned addrHi = addrRange(1, addrRange(1)).hex();

      for(unsigned bank = bankLo; bank <= bankHi; bank++) {
        for(unsigned addr = addrLo; addr <= addrHi; addr++) {
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
