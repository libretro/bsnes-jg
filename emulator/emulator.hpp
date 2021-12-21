#pragma once

#include <libco/libco.h>

#include <string>
#include <vector>

#include <nall/bit.hpp>
#include <nall/function.hpp>
#include <nall/iterator.hpp>
#include <nall/array.hpp>
#include <nall/maybe.hpp>
#include <nall/memory.hpp>
#include "../src/serializer.hpp"
#include <nall/primitives.hpp>
#include <nall/shared-pointer.hpp>
#include <nall/varint.hpp>
#include <nall/vector.hpp>
#include <nall/string.hpp>
#include <nall/markup.hpp>
#include <nall/file-buffer.hpp>
#include <nall/vfs.hpp>

#include "audio.hpp"

namespace Emulator {
  //incremented only when serialization format changes
  static const std::string SerializerVersion = "115";

  namespace Constants {
    namespace Colorburst {
      static constexpr double NTSC = 315.0 / 88.0 * 1'000'000.0;
      static constexpr double PAL  = 283.75 * 15'625.0 + 25.0;
    }
  }

  //nall/vfs shorthand constants for open(), load()
  namespace File {
    static const auto Read  = vfs::file::mode::read;
    static const auto Write = vfs::file::mode::write;
    static const auto Optional = false;
    static const auto Required = true;
  };

struct Platform {
  struct Load {
    Load() = default;
    Load(unsigned pathID, string option = "") : valid(true), pathID(pathID), option(option) {}
    explicit operator bool() const { return valid; }

    bool valid = false;
    unsigned pathID = 0;
    string option;
  };

  virtual auto open(unsigned id, std::string name, vfs::file::mode mode, bool required = false) -> vfs::file* { return {}; }
  virtual auto load(unsigned id, std::string name, std::string type, std::vector<std::string> options = {}) -> Load { return {}; }
  virtual auto videoFrame(const uint16_t *data, unsigned pitch, unsigned width, unsigned height, unsigned scale) -> void {}
  virtual auto audioFrame(unsigned numsamps) -> void {}
  virtual auto inputPoll(unsigned port, unsigned device, unsigned input) -> int16_t { return 0; }
};

extern Platform* platform;

struct Interface {
  struct Information {
    std::string manufacturer;
    std::string name;
    std::string extension;
    bool resettable = false;
  };

  struct Display {
    struct Type { enum : unsigned {
      CRT,
      LCD,
    };};
    unsigned id = 0;
    string name;
    unsigned type = 0;
    unsigned colors = 0;
    unsigned width = 0;
    unsigned height = 0;
    unsigned internalWidth = 0;
    unsigned internalHeight = 0;
    double aspectCorrection = 0;
  };

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

  //information
  virtual auto information() -> Information { return {}; }

  virtual auto display() -> Display { return {}; }
  virtual auto color(uint32_t color) -> uint64_t { return 0; }

  //game interface
  virtual auto loaded() -> bool { return false; }
  virtual auto hashes() -> std::vector<string> { return {}; }
  virtual auto manifests() -> std::vector<string> { return {}; }
  virtual auto titles() -> std::vector<string> { return {}; }
  virtual auto title() -> string { return {}; }
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
  virtual auto synchronize(uint64_t timestamp = 0) -> void {}

  //state functions
  virtual auto serialize(bool synchronize = true) -> serializer { return {}; }
  virtual auto unserialize(serializer&) -> bool { return false; }

  //cheat functions
  virtual auto read(nall::Natural<24> address) -> uint8_t { return 0; }
  virtual auto cheats(const std::vector<string>& = {}) -> void {}

  //configuration
  virtual auto configuration() -> string { return {}; }
  virtual auto configuration(string name) -> string { return {}; }
  virtual auto configure(string configuration = "") -> bool { return false; }
  virtual auto configure(string name, string value) -> bool { return false; }

  virtual auto frameSkip() -> unsigned { return 0; }
  virtual auto setFrameSkip(unsigned frameSkip) -> void {}

  virtual auto runAhead() -> bool { return false; }
  virtual auto setRunAhead(bool runAhead) -> void {}
};

struct Game {
  struct Memory;
  struct Oscillator;

  inline auto load(std::string) -> void;
  inline auto memory(Markup::Node) -> maybe<Memory>;
  inline auto oscillator(Natural<> = 0) -> maybe<Oscillator>;

  struct Memory {
    Memory() = default;
    inline Memory(Markup::Node);
    explicit operator bool() const { return (bool)type; }
    inline auto name() const -> string;

    string type;
    Natural<> size;
    string content;
    string manufacturer;
    string architecture;
    string identifier;
    Boolean nonVolatile;
  };

  struct Oscillator {
    Oscillator() = default;
    inline Oscillator(Markup::Node);
    explicit operator bool() const { return frequency; }

    Natural<> frequency;
  };

  Markup::Node document;
  string sha256;
  string label;
  std::string name;
  string title;
  string region;
  string revision;
  string board;
  std::vector<Memory> memoryList;
  std::vector<Oscillator> oscillatorList;
};

auto Game::load(std::string text) -> void {
  document = BML::unserialize(text.c_str());

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

auto Game::memory(Markup::Node node) -> maybe<Memory> {
  if(!node) return nothing;
  for(auto& memory : memoryList) {
    auto type = node["type"].text();
    auto size = node["size"].natural();
    auto content = node["content"].text();
    auto manufacturer = node["manufacturer"].text();
    auto architecture = node["architecture"].text();
    auto identifier = node["identifier"].text();
    if(type && type != memory.type) continue;
    if(size && size != memory.size) continue;
    if(content && content != memory.content) continue;
    if(manufacturer && manufacturer != memory.manufacturer) continue;
    if(architecture && architecture != memory.architecture) continue;
    if(identifier && identifier != memory.identifier) continue;
    return memory;
  }
  return nothing;
}

auto Game::oscillator(Natural<> index) -> maybe<Oscillator> {
  if(index < oscillatorList.size()) return oscillatorList[index];
  return nothing;
}

Game::Memory::Memory(Markup::Node node) {
  type = node["type"].text();
  size = node["size"].natural();
  content = node["content"].text();
  manufacturer = node["manufacturer"].text();
  architecture = node["architecture"].text();
  identifier = node["identifier"].text();
  nonVolatile = !(bool)node["volatile"];
}

auto Game::Memory::name() const -> string {
  if(architecture) return string{architecture, ".", content, ".", type}.downcase();
  return string{content, ".", type}.downcase();
}

Game::Oscillator::Oscillator(Markup::Node node) {
  frequency = node["frequency"].natural();
}

}
