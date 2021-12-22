#include "emulator.hpp"

namespace Emulator {

Stream* Audio::createStream(double frequency) {
  Stream *stream = new Stream;
  stream->reset(frequency, _frequency);
  _streams.push_back(stream);
  return stream;
}

void Stream::reset(double inputFrequency, double outputFrequency) {
  if (srcstate == nullptr) {
    int err;
    srcstate = src_new(audio._rsqual, 2, &err); // 2 channels
    srcdata = { 0 }; // end_of_input MUST be zero initialized
  }

  src_reset(srcstate);
  setFrequency(inputFrequency, this->outputFrequency);
  queue_in.reserve(audio._spf << 1);
  queue_out.reserve(audio._spf << 1);
  resamp_out.reserve(audio._spf << 1);
}

void Stream::setFrequency(double inputFrequency, double outputFrequency) {
  this->inputFrequency = inputFrequency;
  if (outputFrequency) {
    this->outputFrequency = outputFrequency;
  }
  srcdata.src_ratio = this->outputFrequency / inputFrequency;
  spf_in = (unsigned)(inputFrequency / ((this->outputFrequency / audio._spf)));
  if (spf_in & 1) --spf_in; // 2 channels means samples per frame must be even
}

void Stream::write(const int16_t samples[]) {
  queue_in.push_back(samples[0] / 32768.0f);
  queue_in.push_back(samples[1] / 32768.0f);

  if (queue_in.size() == spf_in) {
    srcdata.data_in = queue_in.data();
    srcdata.data_out = resamp_out.data();
    srcdata.input_frames = spf_in >> 1;
    srcdata.output_frames = audio._spf;
    src_process(srcstate, &srcdata);
    queue_in.clear();

    for (int i = 0; i < srcdata.output_frames_gen << 1; ++i) {
      queue_out.push_back(resamp_out[i]);
    }

    audio.process();
  }
}

Audio audio;

Audio::~Audio() {
  for (auto& stream : _streams) {
    stream->srcstate = src_delete(stream->srcstate);
    stream->srcstate = nullptr;
  }
  reset(nullptr);
}

void Audio::reset(Interface* interface) {
  _interface = interface;
  _streams.clear();
}

void Audio::process() {
  for (auto& stream : _streams) {
      if (stream->queue_out.size() < _spf) return;
  }

  for (auto& stream : _streams) {
    for (int i = 0; i < _spf; ++i) {
      buffer[i] += stream->queue_out[i];
    }
    stream->queue_out.erase(stream->queue_out.begin(), stream->queue_out.begin() + _spf);
  }

  platform->audioFrame(_spf);
  memset(&buffer[0], 0.0, _spf * sizeof(float));
}

}
