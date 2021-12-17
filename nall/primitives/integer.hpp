#pragma once

namespace nall {

template<unsigned Precision> struct Integer {
  static inline constexpr auto bits() -> unsigned { return Precision; }
  using stype =
    std::conditional_t<bits() <=  8,  int8_t,
    std::conditional_t<bits() <= 16, int16_t,
    std::conditional_t<bits() <= 32, int32_t,
    std::conditional_t<bits() <= 64, int64_t,
    void>>>>;
  static inline constexpr auto mask() -> typename Natural<Precision>::utype { return ~0ull >> 64 - Precision; }
  static inline constexpr auto sign() -> typename Natural<Precision>::utype { return 1ull << Precision - 1; }

  template<typename T> inline Integer(const T& value) { data = cast(value); }

  inline operator stype() const { return data; }

private:
  inline auto cast(stype value) const -> stype {
    return (value & mask() ^ sign()) - sign();
  }

  stype data;
};

}
