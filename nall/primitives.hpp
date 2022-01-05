/*
 * nall - C++ template library
 *
 * Copyright © 2006-2020 byuu et al
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#pragma once

#include <cstdint>

template <typename T, unsigned B>
inline T signextend(const T x) {
  struct { T x:B; } s;
  return s.x = x;
}

namespace nall {

#ifdef __clang__
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wparentheses"
#elif __GNUC__
  #pragma GCC push_options
  #pragma GCC diagnostic ignored "-Wparentheses"
#endif

struct Boolean;
template<unsigned Precision = 64> struct Natural;
template<unsigned Precision = 64> struct Integer;
template<unsigned Precision = 64> struct Real;

template<int...> struct BitField;

/* static BitField */

template<int Precision, int Index> struct BitField<Precision, Index> {
  enum : unsigned { bits = Precision };
  using type =
    std::conditional_t<bits <=  8,  uint8_t,
    std::conditional_t<bits <= 16, uint16_t,
    std::conditional_t<bits <= 32, uint32_t,
    std::conditional_t<bits <= 64, uint64_t,
    void>>>>;
  enum : unsigned { shift = Index < 0 ? Precision + Index : Index };
  enum : type { mask = 1ull << shift };

  BitField(const BitField&) = delete;

  template<typename T> inline BitField(T* source) : target((type&)*source) {
    static_assert(sizeof(T) == sizeof(type));
  }

  inline operator bool() const {
    return target & mask;
  }

  inline auto& operator=(bool source) {
    target = target & ~mask | source << shift;
    return *this;
  }

private:
  type& target;
};

template<int...> struct BitRange;

/* static BitRange */

template<int Precision, int Lo, int Hi> struct BitRange<Precision, Lo, Hi> {
  enum : unsigned { bits = Precision };
  using type =
    std::conditional_t<bits <=  8,  uint8_t,
    std::conditional_t<bits <= 16, uint16_t,
    std::conditional_t<bits <= 32, uint32_t,
    std::conditional_t<bits <= 64, uint64_t,
    void>>>>;
  enum : unsigned { lo = Lo < 0 ? Precision + Lo : Lo };
  enum : unsigned { hi = Hi < 0 ? Precision + Hi : Hi };
  enum : type { mask = ~0ull >> 64 - (hi - lo + 1) << lo };
  enum : unsigned { shift = lo };

  BitRange(const BitRange& source) = delete;

  inline auto& operator=(const BitRange& source) {
    target = target & ~mask | ((source.target & source.mask) >> source.shift) << shift & mask;
    return *this;
  }

  template<typename T> inline BitRange(T* source) : target((type&)*source) {
    static_assert(sizeof(T) == sizeof(type));
  }

private:
  type& target;
};

/* dynamic BitRange */

template<int Precision> struct BitRange<Precision> {
  enum : unsigned { bits = Precision };
  using type =
    std::conditional_t<bits <=  8,  uint8_t,
    std::conditional_t<bits <= 16, uint16_t,
    std::conditional_t<bits <= 32, uint32_t,
    std::conditional_t<bits <= 64, uint64_t,
    void>>>>;

  BitRange(const BitRange& source) = delete;

  template<typename T> inline BitRange(T* source, int index) : target((type&)*source) {
    static_assert(sizeof(T) == sizeof(type));
    if(index < 0) index = Precision + index;
    mask = 1ull << index;
    shift = index;
  }

  template<typename T> inline BitRange(T* source, int lo, int hi) : target((type&)*source) {
    static_assert(sizeof(T) == sizeof(type));
    if(lo < 0) lo = Precision + lo;
    if(hi < 0) hi = Precision + hi;
    if(lo > hi) std::swap(lo, hi);
    mask = ~0ull >> 64 - (hi - lo + 1) << lo;
    shift = lo;
  }

  inline operator type() const {
    return (target & mask) >> shift;
  }

  inline auto operator++(int) {
    auto value = (target & mask) >> shift;
    target = target & ~mask | target + (1 << shift) & mask;
    return value;
  }

  template<typename T> inline auto& operator=(const T& source) {
    type value = source;
    target = target & ~mask | value << shift & mask;
    return *this;
  }

private:
  type& target;
  type mask;
  unsigned shift;
};

template<unsigned Precision> struct Natural {
  static inline constexpr auto bits() -> unsigned { return Precision; }
  using utype =
    std::conditional_t<bits() <=  8,  uint8_t,
    std::conditional_t<bits() <= 16, uint16_t,
    std::conditional_t<bits() <= 32, uint32_t,
    std::conditional_t<bits() <= 64, uint64_t,
    void>>>>;
  static inline constexpr auto mask() -> utype { return ~0ull >> 64 - Precision; }

  inline Natural() : data(0) {}
  template<typename T> inline Natural(const T& value) { data = cast(value); }

  inline operator utype() const { return data; }

  inline auto operator++(int) { auto value = *this; data = cast(data + 1); return value; }
  inline auto operator--(int) { auto value = *this; data = cast(data - 1); return value; }

  inline auto& operator++() { data = cast(data + 1); return *this; }
  inline auto& operator--() { data = cast(data - 1); return *this; }

  template<typename T> inline auto& operator /=(const T& value) { data = cast(data  / value); return *this; }
  template<typename T> inline auto& operator +=(const T& value) { data = cast(data  + value); return *this; }
  template<typename T> inline auto& operator -=(const T& value) { data = cast(data  - value); return *this; }
  template<typename T> inline auto& operator>>=(const T& value) { data = cast(data >> value); return *this; }
  template<typename T> inline auto& operator &=(const T& value) { data = cast(data  & value); return *this; }
  template<typename T> inline auto& operator ^=(const T& value) { data = cast(data  ^ value); return *this; }
  template<typename T> inline auto& operator |=(const T& value) { data = cast(data  | value); return *this; }

  inline auto bit(int index) -> BitRange<Precision> { return {&data, index}; }
  inline auto bit(int lo, int hi) -> BitRange<Precision> { return {&data, lo, hi}; }

  inline auto byte(int index) -> BitRange<Precision> { return {&data, index * 8 + 0, index * 8 + 7}; }
  inline auto serialize(serializer& s) { s(data); }

private:
  inline auto cast(utype value) const -> utype {
    return value & mask();
  }

  utype data;
};

#ifdef __clang__
  #pragma clang diagnostic pop
#elif __GNUC__
  #pragma GCC pop_options
#endif

}
