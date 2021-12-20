#pragma once

#include <vector>
#include <samplerate.h>

namespace Emulator {

struct Interface;
struct Audio;
struct Stream;

struct Audio {
  ~Audio();
  shared_pointer<Stream> createStream(unsigned channels, double frequency);
  void reset(Interface* interface);
  void setFrequency(double frequency) { _frequency = frequency; }
  void setBuffer(float *buffer) { this->buffer = buffer; }
  void setSpf(unsigned spf) { _spf = spf; }
  void setQuality(unsigned rsqual) { _rsqual = 4 - rsqual; } // Low to High

private:
  void process();

  Interface* _interface = nullptr;
  std::vector<shared_pointer<Stream>> _streams;

  unsigned _channels = 0;
  double _frequency = 48000.0;
  unsigned _rsqual = SRC_SINC_MEDIUM_QUALITY;
  unsigned _spf = 0;
  float *buffer = nullptr;

  friend class Stream;
};

struct Stream {
  void reset(unsigned channels, double inputFrequency, double outputFrequency);
  void setFrequency(double inputFrequency, double outputFrequency);
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

  friend class Audio;
};

extern Audio audio;

}

#undef double
