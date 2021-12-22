#pragma once

namespace Emulator {

struct Cheat {
  struct Code {
    auto operator==(const Code& code) const -> bool {
      if(address != code.address) return false;
      if(data != code.data) return false;
      if((bool)compare != (bool)code.compare) return false;
      if(compare && code.compare && compare() != code.compare()) return false;
      return true;
    }

    unsigned address;
    unsigned data;
    maybe<unsigned> compare;
    bool enable;
    unsigned restore;
  };

  explicit operator bool() const {
    return codes.size() > 0;
  }

  auto reset() -> void {
    codes.clear();
  }

  auto append(unsigned address, unsigned data, maybe<unsigned> compare = {}) -> void {
    codes.push_back({address, data, compare});
  }

  auto assign(const std::vector<nall::string>& list) -> void {
    reset();
    for(auto& entry : list) {
      for(auto code : entry.split("+")) {
        auto part = code.transform("=?", "//").split("/");
        if(part.size() == 2) append(part[0].hex(), part[1].hex());
        if(part.size() == 3) append(part[0].hex(), part[2].hex(), part[1].hex());
      }
    }
  }

  auto find(unsigned address, unsigned compare) -> maybe<unsigned> {
    for(auto& code : codes) {
      if(code.address == address && (!code.compare || code.compare() == compare)) {
        return code.data;
      }
    }
    return nothing;
  }

  std::vector<Code> codes;
};

}
