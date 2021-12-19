#include <iostream>
#include "emulator.hpp"

namespace Emulator {

void Stream::reset(unsigned channelCount, double inputFrequency, double outputFrequency) {
  channels.clear();
  channels.resize(channelCount);
  setFrequency(inputFrequency, outputFrequency);
}

void Stream::setFrequency(double inputFrequency, maybe<double> outputFrequency) {
  this->inputFrequency = inputFrequency;
  if(outputFrequency) this->outputFrequency = outputFrequency();
  for(auto& channel : channels) {
    channel.resampler.reset(this->inputFrequency, this->outputFrequency);
  }
}

unsigned Stream::pending() const {
  if(channels.empty()) return 0;
  return channels[0].resampler.pending();
}

unsigned Stream::read(double samples[]) {
  for(unsigned c = 0; c < channels.size(); ++c) {
    samples[c] = channels[c].resampler.read();
  }
  return channels.size();
}

void Stream::write(const double samples[]) {
  for(unsigned c = 0; c < channels.size(); ++c) {
    double sample = samples[c] + 1e-25;  //constant offset used to suppress denormals
    channels[c].resampler.write(sample);
  }

  audio.process();
}

Audio audio;

Audio::~Audio() {
  reset(nullptr);
}

void Audio::reset(Interface* interface) {
  _interface = interface;
  _streams.clear();
  _channels = 0;
}

shared_pointer<Stream> Audio::createStream(unsigned channels, double frequency) {
  _channels = std::max(_channels, channels);
  shared_pointer<Stream> stream = new Stream;
  stream->reset(channels, frequency, _frequency);
  _streams.push_back(stream);
  return stream;
}

void Audio::process() {
  while(!_streams.empty()) {
    for(auto& stream : _streams) {
      if(!stream->pending()) return;
    }

    double samples[_channels];
    for(auto& sample : samples) sample = 0.0;

    for(auto& stream : _streams) {
      double buffer[_channels];
      unsigned length = stream->read(buffer), offset = 0;

      for(auto& sample : samples) {
        sample += buffer[offset];
        if(++offset >= length) offset = 0;
      }
    }

    for(auto c : range(_channels)) {
      samples[c] = std::max(-1.0, std::min(+1.0, samples[c]));
    }

    platform->audioFrame(samples, _channels);
  }
}

}
