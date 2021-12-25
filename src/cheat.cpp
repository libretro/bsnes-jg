#include <iostream>
#include <string>
#include <vector>

#include "emulator.hpp"
#include "cheat.hpp"

namespace Emulator {

void Cheat::reset() {
  codes.clear();
}

void Cheat::append(unsigned address, unsigned data, nall::maybe<unsigned> compare) {
  codes.push_back({address, data, compare});
}

void Cheat::assign(const std::vector<nall::string>& list) {
  reset();
  for(auto& entry : list) {
    for(auto code : entry.split("+")) {
      auto part = code.transform("=?", "//").split("/");
      if(part.size() == 2) append(part[0].hex(), part[1].hex());
      if(part.size() == 3) append(part[0].hex(), part[2].hex(), part[1].hex());
    }
  }
}

nall::maybe<unsigned> Cheat::find(unsigned address, unsigned compare) {
  for(auto& code : codes) {
    if(code.address == address && (!code.compare || code.compare() == compare)) {
      return code.data;
    }
  }
  return nall::nothing;
}

}
