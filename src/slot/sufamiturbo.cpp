#include "../sfc.hpp"

namespace SuperFamicom {

void SufamiTurboCartridge::serialize(serializer& s) {
  s.array(ram.data(), ram.size());
}

SufamiTurboCartridge sufamiturboA;
SufamiTurboCartridge sufamiturboB;

void SufamiTurboCartridge::unload() {
  rom.reset();
  ram.reset();
}

void SufamiTurboCartridge::power() {
}

}
