#pragma once

namespace Emulator {

struct Interface;
struct Audio;
struct Stream;

struct Audio {
  ~Audio();
  void reset(Interface* interface);
  void setFrequency(double frequency);
  shared_pointer<Stream> createStream(unsigned channels, double frequency);

private:
  void process();

  Interface* _interface = nullptr;
  std::vector<shared_pointer<Stream>> _streams;

  unsigned _channels = 0;
  double _frequency = 48000.0;

  friend class Stream;
};

struct Stream {
  void reset(unsigned channels, double inputFrequency, double outputFrequency);
  void setFrequency(double inputFrequency, maybe<double> outputFrequency = nothing);
  unsigned pending() const;
  unsigned read(double samples[]);
  void write(const double samples[]);

  template<typename... P> void sample(P&&... p) {
    double samples[sizeof...(P)] = {std::forward<P>(p)...};
    write(samples);
  }

private:
  struct Channel {
    DSP::Resampler::Cubic resampler;
  };
  std::vector<Channel> channels;
  double inputFrequency;
  double outputFrequency;

  friend class Audio;
};

extern Audio audio;

}

#undef double
