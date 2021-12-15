#include <sfc/sfc.hpp>

namespace SuperFamicom {

auto SufamiTurboCartridge::serialize(serializer& s) -> void {
  s.array(ram.data(), ram.size());
}

SufamiTurboCartridge sufamiturboA;
SufamiTurboCartridge sufamiturboB;

auto SufamiTurboCartridge::unload() -> void {
  rom.reset();
  ram.reset();
}

auto SufamiTurboCartridge::power() -> void {
}

}
