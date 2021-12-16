#pragma once

//multi-precision arithmetic
//warning: each size is quadratically more expensive than the size before it!

namespace nall {
  template<typename T, std::enable_if_t<std::is_unsigned<T>::value>>
  inline auto lower(T value) -> T {
    static const T Mask = ~T(0) >> sizeof(T) * 4;
    return value & Mask;
  }

  template<typename T, typename U, std::enable_if_t<std::is_unsigned<T>::value>, std::enable_if_t<std::is_unsigned<U>::value>>
  inline auto mul(T lhs, U rhs) -> uintmax {
    return lhs * rhs;
  }

  template<typename T, std::enable_if_t<std::is_unsigned<T>::value>>
  inline auto square(T value) -> uintmax {
    return value * value;
  }

  template<typename T, typename U>
  inline auto ror(T lhs, U rhs, std::enable_if_t<std::is_unsigned<T>::value>* = 0) -> T {
    return lhs >> rhs | lhs << sizeof(T) * 8 - rhs;
  }
}
