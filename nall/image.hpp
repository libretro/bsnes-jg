#pragma once

#include <algorithm>

#include <nall/memory.hpp>
#include <nall/file-buffer.hpp>
#include <nall/stdint.hpp>

namespace nall {

struct image {
  enum class blend : uint {
    add,
    sourceAlpha,  //color = sourceColor * sourceAlpha + targetColor * (1 - sourceAlpha)
    sourceColor,  //color = sourceColor
    targetAlpha,  //color = targetColor * targetAlpha + sourceColor * (1 - targetAlpha)
    targetColor,  //color = targetColor
  };

  struct channel {
    channel(uint64_t mask, uint depth, uint shift) : _mask(mask), _depth(depth), _shift(shift) {
    }

    auto operator==(const channel& source) const -> bool {
      return _mask == source._mask && _depth == source._depth && _shift == source._shift;
    }

    auto operator!=(const channel& source) const -> bool {
      return !operator==(source);
    }

    alwaysinline auto mask() const { return _mask; }
    alwaysinline auto depth() const { return _depth; }
    alwaysinline auto shift() const { return _shift; }

  private:
    uint64_t _mask;
    uint _depth;
    uint _shift;
  };

  //core.hpp
  inline image(const image& source);
  inline image(image&& source);
  inline image(bool endian, uint depth, uint64_t alphaMask, uint64_t redMask, uint64_t greenMask, uint64_t blueMask);
  inline image(const void* data, uint size);
  inline image(const vector<uint8_t>& buffer);
  template<uint Size> inline image(const uint8_t (&Name)[Size]);
  inline image();
  inline ~image();

  inline auto operator=(const image& source) -> image&;
  inline auto operator=(image&& source) -> image&;

  inline explicit operator bool() const;
  inline auto operator==(const image& source) const -> bool;
  inline auto operator!=(const image& source) const -> bool;

  inline auto read(const uint8_t* data) const -> uint64_t;
  inline auto write(uint8_t* data, uint64_t value) const -> void;

  inline auto free() -> void;
  inline auto copy(const void* data, uint pitch, uint width, uint height) -> void;
  inline auto allocate(uint width, uint height) -> void;

  //static.hpp
  static inline auto bitDepth(uint64_t color) -> uint;
  static inline auto bitShift(uint64_t color) -> uint;
  static inline auto normalize(uint64_t color, uint sourceDepth, uint targetDepth) -> uint64_t;

  //access
  alwaysinline auto data() { return _data; }
  alwaysinline auto data() const { return _data; }
  alwaysinline auto width() const { return _width; }
  alwaysinline auto height() const { return _height; }

  alwaysinline auto endian() const { return _endian; }
  alwaysinline auto depth() const { return _depth; }
  alwaysinline auto stride() const { return (_depth + 7) >> 3; }

  alwaysinline auto pitch() const { return _width * stride(); }
  alwaysinline auto size() const { return _height * pitch(); }

  alwaysinline auto alpha() const { return _alpha; }
  alwaysinline auto red() const { return _red; }
  alwaysinline auto green() const { return _green; }
  alwaysinline auto blue() const { return _blue; }

private:
  //core.hpp
  inline auto allocate(uint width, uint height, uint stride) -> uint8_t*;

  uint8_t* _data   = nullptr;
  uint _width  = 0;
  uint _height = 0;

  bool _endian =  0;  //0 = lsb, 1 = msb
  uint _depth  = 32;

  channel _alpha{255u << 24, 8, 24};
  channel _red  {255u << 16, 8, 16};
  channel _green{255u <<  8, 8,  8};
  channel _blue {255u <<  0, 8,  0};
};

}

#include <nall/image/static.hpp>
#include <nall/image/core.hpp>
