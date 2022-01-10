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

#pragma once

#include <samplerate.h>

namespace SuperFamicom {

struct Interface;
struct Audio;
struct Stream;

struct Audio {
  ~Audio();
  Stream* createStream(double);
  void reset();
  void setFrequency(double);
  void setBuffer(float*);
  void setSpf(unsigned);
  void setQuality(unsigned);

private:
  void process();

  std::vector<Stream*> _streams;

  double _frequency = 48000.0;
  unsigned _rsqual = SRC_SINC_FASTEST;
  unsigned _spf = 0;
  float *buffer = nullptr;

  friend struct Stream;
};

struct Stream {
  void reset(double, double);
  void setFrequency(double, double);
  void write(const int16_t samples[]);

  template<typename... P> void sample(P&&... p) {
    int16_t samples[sizeof...(P)] = {std::forward<P>(p)...};
    write(samples);
  }

  SRC_STATE *srcstate = nullptr;
  SRC_DATA srcdata;
  std::vector<float> queue_in;
  std::vector<float> queue_out;
  std::vector<float> resamp_out;
  unsigned spf_in = 0;

private:
  double inputFrequency;
  double outputFrequency = 48000.0;

  friend struct Audio;
};

extern Audio audio;

}
