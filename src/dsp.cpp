#include "sfc.hpp"

namespace SuperFamicom {

DSP dsp;

static auto dsp_state_save(unsigned char** out, void* in, size_t size) -> void {
  memcpy(*out, in, size);
  *out += size;
}

static auto dsp_state_load(unsigned char** in, void* out, size_t size) -> void {
  memcpy(out, *in, size);
  *in += size;
}

auto DSP::serialize(serializer& s) -> void {
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

auto DSP::main() -> void {
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

auto DSP::read(uint8_t address) -> uint8_t {
  return spc_dsp.read(address);
}

auto DSP::write(uint8_t address, uint8_t data) -> void {
  spc_dsp.write(address, data);
}

auto DSP::load() -> bool {
  return true;
}

auto DSP::power(bool reset) -> void {
  clock = 0;
  stream = Emulator::audio.createStream(2, system.apuFrequency() / 768.0);

  if(!reset) {
    spc_dsp.init(apuram, apuram);
    spc_dsp.reset();
    spc_dsp.set_output(samplebuffer, 8192);
  } else {
    spc_dsp.soft_reset();
    spc_dsp.set_output(samplebuffer, 8192);
  }

  if(configuration.hacks.hotfixes) {
    //Magical Drop (Japan) does not initialize the DSP registers at startup:
    //tokoton mode will hang forever in some instances even on real hardware.
    if(cartridge.headerTitle() == "MAGICAL DROP") {
      for(unsigned address : range(0x80)) spc_dsp.write(address, 0xff);
    }
  }
}

auto DSP::mute() -> bool {
  return spc_dsp.mute();
}

}
