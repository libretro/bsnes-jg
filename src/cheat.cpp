#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include "emulator.hpp"
#include "cheat.hpp"

namespace Emulator {

void Cheat::reset() {
  codes.clear();
}

void Cheat::append(unsigned address, unsigned data, std::optional<unsigned> compare) {
  codes.push_back({address, data, compare});
}

void Cheat::assign(const std::vector<std::string>& list) {
  reset();

  for (std::string entry : list) {
      std::replace(entry.begin(), entry.end(), '?', '=');
      std::stringstream ss(entry);
      std::vector<unsigned> part;
      while(std::getline(ss, entry, '=')) {
          part.push_back(std::stoul(entry, nullptr, 16));
      }
      if (part.size() == 2) append(part[0], part[1]);
      if (part.size() == 3) append(part[0], part[2], part[1]);
  }
}

bool Cheat::find(uint8_t *val, unsigned address, unsigned compare) {
  for(auto& code : codes) {
    if(code.address == address && (!code.compare || code.compare == compare)) {
      *val = code.data;
      return true;
    }
  }
  return false;
}

}
