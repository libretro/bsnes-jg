#include <memory>
#include "dsp/SPC_DSP.h"

struct DSP {
  Emulator::Stream *stream;
  uint8_t apuram[64 * 1024] = {};

  void main();
  uint8_t read(uint8_t address);
  void write(uint8_t address, uint8_t data);

  bool load();
  void power(bool reset);
  bool mute();

  void serialize(serializer&);

  int64_t clock = 0;

private:
  SPC_DSP spc_dsp;
  int16_t samplebuffer[8192];

//unserialized:
  uint8_t echoram[64 * 1024] = {};
};

extern DSP dsp;
