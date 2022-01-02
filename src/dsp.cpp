#include "sfc.hpp"

namespace SuperFamicom {

DSP dsp;

static void dsp_state_save(unsigned char** out, void* in, size_t size) {
  memcpy(*out, in, size);
  *out += size;
}

static void dsp_state_load(unsigned char** in, void* out, size_t size) {
  memcpy(out, *in, size);
  *in += size;
}

void DSP::serialize(serializer& s) {
  s.array(apuram);
  s.array(samplebuffer);
  s.integer(clock);

  unsigned char state[SPC_DSP::state_size];
  unsigned char* p = state;
  memset(&state, 0, SPC_DSP::state_size);
  if(s.mode() == serializer::Save) {
    spc_dsp.copy_state(&p, dsp_state_save);
    s.array(state);
  } else if(s.mode() == serializer::Load) {
    s.array(state);
    spc_dsp.copy_state(&p, dsp_state_load);
  } else {
    s.array(state);
  }
}

void DSP::main() {
  spc_dsp.run(1);
  clock += 2;

  int count = spc_dsp.sample_count();
  if(count > 0) {
    if(!system.runAhead)
    for(unsigned n = 0; n < count; n += 2) {
      int16_t left  = samplebuffer[n + 0];
      int16_t right = samplebuffer[n + 1];
      stream->sample(left, right);
    }
    spc_dsp.set_output(samplebuffer, 8192);
  }
}

uint8_t DSP::read(uint8_t address) {
  return spc_dsp.read(address);
}

void DSP::write(uint8_t address, uint8_t data) {
  spc_dsp.write(address, data);
}

bool DSP::load() {
  return true;
}

void DSP::power(bool reset) {
  clock = 0;
  stream = Emulator::audio.createStream(system.apuFrequency() / 768.0);

  if(!reset) {
    spc_dsp.init(apuram, apuram);
    spc_dsp.reset();
    spc_dsp.set_output(samplebuffer, 8192);
  } else {
    spc_dsp.soft_reset();
    spc_dsp.set_output(samplebuffer, 8192);
  }

  if(configuration.hotfixes) {
    //Magical Drop (Japan) does not initialize the DSP registers at startup:
    //tokoton mode will hang forever in some instances even on real hardware.
    if(cartridge.headerTitle() == "MAGICAL DROP") {
      for(unsigned address = 0; address < 0x80; ++address) spc_dsp.write(address, 0xff);
    }
  }
}

bool DSP::mute() {
  return spc_dsp.mute();
}

}
