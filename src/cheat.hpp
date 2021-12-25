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
    nall::maybe<unsigned> compare;
    bool enable;
    unsigned restore;
  };

  explicit operator bool() const {
    return codes.size() > 0;
  }

  void reset();
  void append(unsigned address, unsigned data, nall::maybe<unsigned> compare = {});
  void assign(const std::vector<std::string>& list);
  nall::maybe<unsigned> find(unsigned address, unsigned compare);

  std::vector<Code> codes;
};

}
