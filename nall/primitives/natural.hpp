#pragma once

namespace nall {

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

}
