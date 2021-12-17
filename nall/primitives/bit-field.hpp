#pragma once

namespace nall {

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

}
