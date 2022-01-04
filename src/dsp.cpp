/*
 * bsnes-jg - Super Nintendo emulator
 *
 * Copyright (C) 2004-2020 byuu
 * Copyright (C) 2020-2022 Rupert Carmichael
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, specifically version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "interface.hpp"
#include "cartridge.hpp"
#include "dsp.hpp"

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
    for(int n = 0; n < count; n += 2) {
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
