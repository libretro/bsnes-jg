#pragma once

namespace nall {

struct VariadicNatural {
  inline VariadicNatural() : mask(~0ull) { assign(0); }
  template<typename T> inline VariadicNatural(const T& value) : mask(~0ull) { assign(value); }

  inline operator uint64_t() const { return data; }
  template<typename T> inline auto& operator=(const T& value) { return assign(value); }

  inline auto operator++(int) { auto value = data; assign(data + 1); return value; }
  inline auto operator--(int) { auto value = data; assign(data - 1); return value; }

  inline auto& operator++() { return assign(data + 1); }
  inline auto& operator--() { return assign(data - 1); }

  inline auto& operator &=(const uint64_t value) { return assign(data  & value); }
  inline auto& operator |=(const uint64_t value) { return assign(data  | value); }
  inline auto& operator ^=(const uint64_t value) { return assign(data  ^ value); }
  inline auto& operator<<=(const uint64_t value) { return assign(data << value); }
  inline auto& operator>>=(const uint64_t value) { return assign(data >> value); }
  inline auto& operator +=(const uint64_t value) { return assign(data  + value); }
  inline auto& operator -=(const uint64_t value) { return assign(data  - value); }
  inline auto& operator *=(const uint64_t value) { return assign(data  * value); }
  inline auto& operator /=(const uint64_t value) { return assign(data  / value); }
  inline auto& operator %=(const uint64_t value) { return assign(data  % value); }

  inline auto resize(unsigned bits) {
    assert(bits <= 64);
    mask = ~0ull >> (64 - bits);
    data &= mask;
  }

  inline auto serialize(serializer& s) {
    s(data);
    s(mask);
  }

  struct Reference {
    inline Reference(VariadicNatural& self, unsigned lo, unsigned hi) : self(self), Lo(lo), Hi(hi) {}

    inline operator uint64_t() const {
      const uint64_t RangeBits = Hi - Lo + 1;
      const uint64_t RangeMask = (((1ull << RangeBits) - 1) << Lo) & self.mask;
      return (self & RangeMask) >> Lo;
    }

    inline auto& operator=(const uint64_t value) {
      const uint64_t RangeBits = Hi - Lo + 1;
      const uint64_t RangeMask = (((1ull << RangeBits) - 1) << Lo) & self.mask;
      self.data = (self.data & ~RangeMask) | ((value << Lo) & RangeMask);
      return *this;
    }

  private:
    VariadicNatural& self;
    const unsigned Lo;
    const unsigned Hi;
  };

  inline auto bits(unsigned lo, unsigned hi) -> Reference { return {*this, lo < hi ? lo : hi, hi > lo ? hi : lo}; }
  inline auto bit(unsigned index) -> Reference { return {*this, index, index}; }
  inline auto byte(unsigned index) -> Reference { return {*this, index * 8 + 0, index * 8 + 7}; }

private:
  auto assign(uint64_t value) -> VariadicNatural& {
    data = value & mask;
    return *this;
  }

  uint64_t data;
  uint64_t mask;
};

}
