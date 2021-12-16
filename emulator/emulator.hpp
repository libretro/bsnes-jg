#pragma once

#include <libco/libco.h>

#include <any>
using std::any;

#include <nall/stdint.hpp>
#include <nall/intrinsics.hpp>
#include <nall/platform.hpp>
#include <nall/algorithm.hpp>
#include <nall/atoi.hpp>
#include <nall/bit.hpp>
#include <nall/endian.hpp>
#include <nall/function.hpp>
#include <nall/iterator.hpp>
#include <nall/array.hpp>
#include <nall/maybe.hpp>
#include <nall/memory.hpp>
#include <nall/serializer.hpp>
#include <nall/primitives.hpp>
#include <nall/shared-pointer.hpp>
#include <nall/varint.hpp>
#include <nall/vector.hpp>
#include <nall/string.hpp>
#include <nall/inode.hpp>
#include <nall/queue.hpp>
#include <nall/arithmetic.hpp>
#include <nall/dsp.hpp>
#include <nall/hash.hpp>
#include <nall/markup.hpp>
#include <nall/file-buffer.hpp>
#include <nall/vfs.hpp>
using namespace nall;

#include "types.hpp"
#include "memory.hpp"
#include "audio.hpp"

namespace Emulator {
  //incremented only when serialization format changes
  static const string SerializerVersion = "115";

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
    Load(uint pathID, string option = "") : valid(true), pathID(pathID), option(option) {}
    explicit operator bool() const { return valid; }

    bool valid = false;
    uint pathID = 0;
    string option;
  };

  virtual auto path(uint id) -> string { return ""; }
  virtual auto open(uint id, string name, vfs::file::mode mode, bool required = false) -> shared_pointer<vfs::file> { return {}; }
  virtual auto load(uint id, string name, string type, vector<string> options = {}) -> Load { return {}; }
  virtual auto videoFrame(const uint16_t *data, uint pitch, uint width, uint height, uint scale) -> void {}
  virtual auto audioFrame(const double* samples, uint channels) -> void {}
  virtual auto inputPoll(uint port, uint device, uint input) -> int16_t { return 0; }
  virtual auto inputRumble(uint port, uint device, uint input, bool enable) -> void {}
  virtual auto dipSettings(Markup::Node node) -> uint { return 0; }
  virtual auto notify(string text) -> void {}
};

extern Platform* platform;

struct Interface {
  struct Information {
    string manufacturer;
    string name;
    string extension;
    bool resettable = false;
  };

  struct Display {
    struct Type { enum : uint {
      CRT,
      LCD,
    };};
    uint id = 0;
    string name;
    uint type = 0;
    uint colors = 0;
    uint width = 0;
    uint height = 0;
    uint internalWidth = 0;
    uint internalHeight = 0;
    double aspectCorrection = 0;
  };

  struct Port {
    uint id;
    string name;
  };

  struct Device {
    uint id;
    string name;
  };

  struct Input {
    struct Type { enum : uint {
      Hat,
      Button,
      Trigger,
      Control,
      Axis,
      Rumble,
    };};

    uint type;
    string name;
  };

  //information
  virtual auto information() -> Information { return {}; }

  virtual auto display() -> Display { return {}; }
  virtual auto color(uint32_t color) -> uint64_t { return 0; }

  //game interface
  virtual auto loaded() -> bool { return false; }
  virtual auto hashes() -> vector<string> { return {}; }
  virtual auto manifests() -> vector<string> { return {}; }
  virtual auto titles() -> vector<string> { return {}; }
  virtual auto title() -> string { return {}; }
  virtual auto load() -> bool { return false; }
  virtual auto save() -> void {}
  virtual auto unload() -> void {}

  //system interface
  virtual auto ports() -> vector<Port> { return {}; }
  virtual auto devices(uint port) -> vector<Device> { return {}; }
  virtual auto inputs(uint device) -> vector<Input> { return {}; }
  virtual auto connected(uint port) -> uint { return 0; }
  virtual auto connect(uint port, uint device) -> void {}
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
  virtual auto read(uint24 address) -> uint8_t { return 0; }
  virtual auto cheats(const vector<string>& = {}) -> void {}

  //configuration
  virtual auto configuration() -> string { return {}; }
  virtual auto configuration(string name) -> string { return {}; }
  virtual auto configure(string configuration = "") -> bool { return false; }
  virtual auto configure(string name, string value) -> bool { return false; }

  //settings
  virtual auto cap(const string& name) -> bool { return false; }
  virtual auto get(const string& name) -> any { return {}; }
  virtual auto set(const string& name, const any& value) -> bool { return false; }

  virtual auto frameSkip() -> uint { return 0; }
  virtual auto setFrameSkip(uint frameSkip) -> void {}

  virtual auto runAhead() -> bool { return false; }
  virtual auto setRunAhead(bool runAhead) -> void {}
};

struct Game {
  struct Memory;
  struct Oscillator;

  inline auto load(string) -> void;
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
  string name;
  string title;
  string region;
  string revision;
  string board;
  vector<Memory> memoryList;
  vector<Oscillator> oscillatorList;
};

auto Game::load(string text) -> void {
  document = BML::unserialize(text);

  sha256 = document["game/sha256"].text();
  label = document["game/label"].text();
  name = document["game/name"].text();
  title = document["game/title"].text();
  region = document["game/region"].text();
  revision = document["game/revision"].text();
  board = document["game/board"].text();

  for(auto node : document.find("game/board/memory")) {
    memoryList.append(Memory{node});
  }

  for(auto node : document.find("game/board/oscillator")) {
    oscillatorList.append(Oscillator{node});
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
