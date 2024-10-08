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

#include <cstring>

#include "audio.hpp"
#include "serializer.hpp"
#include "system.hpp"

#include "dsp.hpp"

#include "snes_spc/spc_dsp.h"

namespace SuperFamicom {

static Stream *stream;

DSP dsp;

static void dsp_state_save(unsigned char** out, void* in, size_t size) {
  memcpy(*out, in, size);
  *out += size;
}

static void dsp_state_load(unsigned char** in, void* out, size_t size) {
  memcpy(out, *in, size);
  *in += size;
}

void DSP::quirk() {
  init = true;
}

void DSP::serialize(serializer& s) {
  s.array(apuram);
  s.array(samplebuffer);
  s.integer(clock);

  unsigned char state[SPC_STATE_SIZE];
  unsigned char* p = state;
  memset(&state, 0, SPC_STATE_SIZE);
  if(s.mode() == serializer::Save) {
    spc_dsp_copy_state(&p, dsp_state_save);
    s.array(state);
  } else if(s.mode() == serializer::Load) {
    s.array(state);
    spc_dsp_copy_state(&p, dsp_state_load);
  } else {
    s.array(state);
  }
}

void DSP::main() {
  spc_dsp_run(1);
  clock += 2;

  int count = spc_dsp_sample_count();
  if(count > 0) {
    if(!system.runAhead) {
      for(int n = 0; n < count; n += 2) {
        int16_t left  = samplebuffer[n + 0];
        int16_t right = samplebuffer[n + 1];
        stream->sample(left, right);
      }
    }
    spc_dsp_set_output(samplebuffer, 8192);
  }
}

uint8_t DSP::read(uint8_t address) {
  return spc_dsp_read(address);
}

void DSP::write(uint8_t address, uint8_t data) {
  spc_dsp_write(address, data);
}

bool DSP::load() {
  return true;
}

void DSP::power(bool reset) {
  clock = 0;
  stream = audio.createStream(system.apuFrequency() / 768.0);

  if(!reset) {
    spc_dsp_init(apuram);
    spc_dsp_reset();
    spc_dsp_set_output(samplebuffer, 8192);
  } else {
    spc_dsp_soft_reset();
    spc_dsp_set_output(samplebuffer, 8192);
  }

  if(init)
    for(unsigned address = 0; address < 0x80; ++address) spc_dsp_write(address, 0xff);
}

bool DSP::mute() {
  return spc_dsp_mute();
}

void DSP::setInterpolation(int interp) {
  spc_dsp_set_interpolation(interp);
}

}
