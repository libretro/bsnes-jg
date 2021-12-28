#pragma once

#include <libco/libco.h>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <optional>
#include <string>
#include <vector>

#include "../nall/bit.hpp"
#include "../nall/function.hpp"
#include "../nall/iterator.hpp"
#include "../nall/array.hpp"
#include "../nall/maybe.hpp"
#include "../nall/memory.hpp"
#include "serializer.hpp"
#include "../nall/primitives.hpp"
#include "../nall/varint.hpp"
#include "../nall/shared-pointer.hpp"
#include "../nall/vector.hpp"
#include "../nall/string.hpp"

#include "byuuML/byuuML.hpp"
#include "byuuML/byuuML_query.hpp"

#include "audio.hpp"
#include "markup.hpp"

namespace Emulator {
  //incremented only when serialization format changes
  static const std::string SerializerVersion = "115";

  namespace Constants {
    namespace Colorburst {
      static constexpr double NTSC = 315.0 / 88.0 * 1'000'000.0;
      static constexpr double PAL  = 283.75 * 15'625.0 + 25.0;
    }
  }

struct Platform {
  struct Load {
    Load() = default;
    Load(unsigned pathID, std::string option = "") : valid(true), pathID(pathID), option(option) {}
    explicit operator bool() const { return valid; }

    bool valid = false;
    unsigned pathID = 0;
    std::string option;
  };

  virtual Load load(unsigned id, std::string name, std::string type, std::vector<std::string> options = {}) { return {}; }
  virtual std::ifstream fopen(unsigned id, std::string name) { return {}; }
  virtual std::vector<uint8_t> mopen(unsigned id, std::string name) { return {}; }
  virtual std::string stropen(unsigned id, std::string name) { return {}; }
  virtual void write(unsigned id, std::string name, const uint8_t *data, unsigned size) {}
  virtual void videoFrame(const uint16_t *data, unsigned pitch, unsigned width, unsigned height, unsigned scale) {}
  virtual void audioFrame(unsigned numsamps) {}
  virtual int16_t inputPoll(unsigned port, unsigned device, unsigned input) { return 0; }
};

extern Platform* platform;

struct Interface {

  struct Port {
    unsigned id;
    std::string name;
  };

  struct Device {
    unsigned id;
    std::string name;
  };

  struct Input {
    struct Type { enum : unsigned {
      Hat,
      Button,
      Trigger,
      Control,
      Axis,
      Rumble,
    };};

    unsigned type;
    std::string name;
  };

  //game interface
  virtual bool loaded() { return false; }
  virtual std::vector<std::string> hashes() { return {}; }
  virtual std::vector<std::string> manifests() { return {}; }
  virtual std::vector<std::string> titles() { return {}; }
  virtual std::string title() { return {}; }
  virtual bool load() { return false; }
  virtual void save() {}
  virtual void unload() {}

  //system interface
  virtual std::vector<Port> ports() { return {}; }
  virtual std::vector<Device> devices(unsigned port) { return {}; }
  virtual std::vector<Input> inputs(unsigned device) { return {}; }
  virtual unsigned connected(unsigned port) { return 0; }
  virtual void connect(unsigned port, unsigned device) {}
  virtual void power() {}
  virtual void reset() {}
  virtual void run() {}

  //time functions
  virtual bool rtc() { return false; }

  //state functions
  virtual serializer serialize(bool synchronize = true) { return {}; }
  virtual bool unserialize(serializer&) { return false; }

  //cheat functions
  virtual uint8_t read(nall::Natural<24> address) { return 0; }
  virtual void cheats(const std::vector<std::string>& = {}) {}

  virtual unsigned frameSkip() { return 0; }
  virtual void setFrameSkip(unsigned frameSkip) {}

  virtual bool runAhead() { return false; }
  virtual void setRunAhead(bool runAhead) {}
};

struct Game {
  struct Memory;
  struct Oscillator;

  void load(std::string);
  nall::maybe<Game::Memory> memory(Markup::Node);
  nall::maybe<Game::Oscillator> oscillator(unsigned = 0);

  struct Memory {
    Memory() = default;
    Memory(Markup::Node);
    explicit operator bool() const { return (bool)!type.empty(); }
    std::string name() const;

    std::string type;
    unsigned size;
    std::string content;
    std::string manufacturer;
    std::string architecture;
    std::string identifier;
    bool nonVolatile;
  };

  struct Oscillator {
    Oscillator() = default;
    Oscillator(Markup::Node);
    explicit operator bool() const { return frequency; }

    unsigned frequency;
  };

  Markup::Node document;
  std::string sha256;
  std::string label;
  std::string name;
  std::string title;
  std::string region;
  std::string revision;
  std::string board;
  std::vector<Game::Memory> memoryList;
  std::vector<Game::Oscillator> oscillatorList;
};

}
