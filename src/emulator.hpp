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
#include "../src/serializer.hpp"
#include "../nall/primitives.hpp"
#include "../nall/shared-pointer.hpp"
#include "../nall/varint.hpp"
#include "../nall/vector.hpp"
#include "../nall/string.hpp"

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

  virtual auto load(unsigned id, std::string name, std::string type, std::vector<std::string> options = {}) -> Load { return {}; }
  virtual auto fopen(unsigned id, std::string name) -> std::ifstream { return {}; }
  virtual auto mopen(unsigned id, std::string name) -> std::vector<uint8_t> { return {}; }
  virtual auto stropen(unsigned id, std::string name) -> std::string { return {}; }
  virtual auto write(unsigned id, std::string name, const uint8_t *data, unsigned size) -> void {}
  virtual auto videoFrame(const uint16_t *data, unsigned pitch, unsigned width, unsigned height, unsigned scale) -> void {}
  virtual auto audioFrame(unsigned numsamps) -> void {}
  virtual auto inputPoll(unsigned port, unsigned device, unsigned input) -> int16_t { return 0; }
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
  virtual auto loaded() -> bool { return false; }
  virtual auto hashes() -> std::vector<std::string> { return {}; }
  virtual auto manifests() -> std::vector<std::string> { return {}; }
  virtual auto titles() -> std::vector<std::string> { return {}; }
  virtual auto title() -> std::string { return {}; }
  virtual auto load() -> bool { return false; }
  virtual auto save() -> void {}
  virtual auto unload() -> void {}

  //system interface
  virtual auto ports() -> std::vector<Port> { return {}; }
  virtual auto devices(unsigned port) -> std::vector<Device> { return {}; }
  virtual auto inputs(unsigned device) -> std::vector<Input> { return {}; }
  virtual auto connected(unsigned port) -> unsigned { return 0; }
  virtual auto connect(unsigned port, unsigned device) -> void {}
  virtual auto power() -> void {}
  virtual auto reset() -> void {}
  virtual auto run() -> void {}

  //time functions
  virtual auto rtc() -> bool { return false; }

  //state functions
  virtual auto serialize(bool synchronize = true) -> serializer { return {}; }
  virtual auto unserialize(serializer&) -> bool { return false; }

  //cheat functions
  virtual auto read(nall::Natural<24> address) -> uint8_t { return 0; }
  virtual auto cheats(const std::vector<std::string>& = {}) -> void {}

  virtual auto frameSkip() -> unsigned { return 0; }
  virtual auto setFrameSkip(unsigned frameSkip) -> void {}

  virtual auto runAhead() -> bool { return false; }
  virtual auto setRunAhead(bool runAhead) -> void {}
};

struct Game {
  struct Memory;
  struct Oscillator;

  inline auto load(std::string) -> void;
  inline auto memory(nall::Markup::Node) -> nall::maybe<Memory>;
  inline auto oscillator(nall::Natural<> = 0) -> nall::maybe<Oscillator>;

  struct Memory {
    Memory() = default;
    inline Memory(nall::Markup::Node);
    explicit operator bool() const { return (bool)!type.empty(); }
    inline std::string name() const;

    std::string type;
    nall::Natural<> size;
    std::string content;
    std::string manufacturer;
    std::string architecture;
    std::string identifier;
    bool nonVolatile;
  };

  struct Oscillator {
    Oscillator() = default;
    inline Oscillator(nall::Markup::Node);
    explicit operator bool() const { return frequency; }

    nall::Natural<> frequency;
  };

  nall::Markup::Node document;
  std::string sha256;
  std::string label;
  std::string name;
  std::string title;
  std::string region;
  std::string revision;
  nall::string board;
  std::vector<Memory> memoryList;
  std::vector<Oscillator> oscillatorList;
};

auto Game::load(std::string text) -> void {
  document = nall::BML::unserialize(text.c_str());

  sha256 = document["game/sha256"].text();
  label = document["game/label"].text();
  name = document["game/name"].text();
  title = document["game/title"].text();
  region = document["game/region"].text();
  revision = document["game/revision"].text();
  board = document["game/board"].text();

  for(auto node : document.find("game/board/memory")) {
    memoryList.push_back(Memory{node});
  }

  for(auto node : document.find("game/board/oscillator")) {
    oscillatorList.push_back(Oscillator{node});
  }
}

auto Game::memory(nall::Markup::Node node) -> nall::maybe<Memory> {
  if(!node) return nall::nothing;
  for(auto& memory : memoryList) {
    auto type = std::string(node["type"].text());
    auto size = node["size"].natural();
    auto content = std::string(node["content"].text());
    auto manufacturer = std::string(node["manufacturer"].text());
    auto architecture = std::string(node["architecture"].text());
    auto identifier = std::string(node["identifier"].text());
    if(!type.empty() && type != memory.type) continue;
    if(size && size != memory.size) continue;
    if(!content.empty() && content != memory.content) continue;
    if(!manufacturer.empty() && manufacturer != memory.manufacturer) continue;
    if(!architecture.empty() && architecture != memory.architecture) continue;
    if(!identifier.empty() && identifier != memory.identifier) continue;
    return memory;
  }
  return nall::nothing;
}

auto Game::oscillator(nall::Natural<> index) -> nall::maybe<Oscillator> {
  if(index < oscillatorList.size()) return oscillatorList[index];
  return nall::nothing;
}

Game::Memory::Memory(nall::Markup::Node node) {
  type = node["type"].text();
  size = node["size"].natural();
  content = node["content"].text();
  manufacturer = node["manufacturer"].text();
  architecture = node["architecture"].text();
  identifier = node["identifier"].text();
  nonVolatile = !(bool)node["volatile"];
}

std::string Game::Memory::name() const {
  std::string ret;
  if(!architecture.empty()) {
    ret = architecture + "." + content + "." + type;
  }
  else {
    ret = content + "." + type;
  }

  std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
  return ret;
}

Game::Oscillator::Oscillator(nall::Markup::Node node) {
  frequency = node["frequency"].natural();
}

}
