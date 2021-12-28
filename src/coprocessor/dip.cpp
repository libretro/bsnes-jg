//DIP switch
//used for Nintendo Super System emulation

#include "../sfc.hpp"

namespace SuperFamicom {

void DIP::serialize(serializer& s) {
  s.integer(value);
}

DIP dip;

void DIP::power() {
}

uint8_t DIP::read(unsigned, uint8_t) {
  return value;
}

void DIP::write(unsigned, uint8_t) {
}

}
