#include <memory>
#include "dsp/SPC_DSP.h"

struct DSP {
  Emulator::Stream *stream;
  uint8_t apuram[64 * 1024] = {};

  auto main() -> void;
  auto read(uint8_t address) -> uint8_t;
  auto write(uint8_t address, uint8_t data) -> void;

  auto load() -> bool;
  auto power(bool reset) -> void;
  auto mute() -> bool;

  auto serialize(serializer&) -> void;

  int64_t clock = 0;

private:
  SPC_DSP spc_dsp;
  int16_t samplebuffer[8192];

//unserialized:
  uint8_t echoram[64 * 1024] = {};
};

extern DSP dsp;
