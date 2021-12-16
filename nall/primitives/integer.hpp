#pragma once

namespace nall {

template<unsigned Precision> struct Integer {
  static_assert(Precision >= 1 && Precision <= 64);
  static inline constexpr auto bits() -> unsigned { return Precision; }
  using stype =
    std::conditional_t<bits() <=  8,  int8_t,
    std::conditional_t<bits() <= 16, int16_t,
    std::conditional_t<bits() <= 32, int32_t,
    std::conditional_t<bits() <= 64, int64_t,
    void>>>>;
  using utype = typename Natural<Precision>::utype;
  static inline constexpr auto mask() -> utype { return ~0ull >> 64 - Precision; }
  static inline constexpr auto sign() -> utype { return 1ull << Precision - 1; }

  inline Integer() : data(0) {}
  template<unsigned Bits> inline Integer(Integer<Bits> value) { data = cast(value); }
  template<typename T> inline Integer(const T& value) { data = cast(value); }
  explicit inline Integer(const char* value) { data = cast(toInteger(value)); }

  inline operator stype() const { return data; }

  inline auto mask(int index) const -> utype {
    return data & 1 << index;
  }

  inline auto mask(int lo, int hi) const -> utype {
    return data & (~0ull >> 64 - (hi - lo + 1) << lo);
  }

private:
  inline auto cast(stype value) const -> stype {
    return (value & mask() ^ sign()) - sign();
  }

  stype data;
};

}
