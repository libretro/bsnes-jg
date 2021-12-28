#pragma once

namespace Emulator {

struct Cheat {
  struct Code {
    bool operator==(const Code& code) const {
      if(address != code.address) return false;
      if(data != code.data) return false;
      if((bool)compare != (bool)code.compare) return false;
      if(compare && code.compare && compare != code.compare) return false;
      return true;
    }

    unsigned address;
    unsigned data;
    std::optional<unsigned> compare;
    bool enable;
    unsigned restore;
  };

  explicit operator bool() const {
    return codes.size() > 0;
  }

  void reset();
  void append(unsigned address, unsigned data, std::optional<unsigned> compare = {});
  void assign(const std::vector<std::string>& list);
  bool find(uint8_t *val, unsigned address, unsigned compare);

  std::vector<Code> codes;
};

}

namespace CheatDecoder {
  bool gb(std::string& code);
  bool snes(std::string& code);
}
