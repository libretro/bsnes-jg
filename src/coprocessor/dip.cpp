//DIP switch
//used for Nintendo Super System emulation

#include "../sfc.hpp"

namespace SuperFamicom {

auto DIP::serialize(serializer& s) -> void {
  s.integer(value);
}

DIP dip;

auto DIP::power() -> void {
}

auto DIP::read(unsigned, uint8_t) -> uint8_t {
  return value;
}

auto DIP::write(unsigned, uint8_t) -> void {
}

}
