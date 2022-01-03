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

//note: (u)intmax actually mean it: use as many bits as is possible
#if defined(__SIZEOF_INT128__)
  using uintmax = unsigned __int128;
#else
  using uintmax = uintmax_t;
#endif

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

constexpr inline auto test(const char* s, uintmax sum = 0) -> uintmax {
  return (
    *s == '0' || *s == '1' ? test(s + 1, (sum << 1) | (*s - '0')) :
    *s == ' ' || *s == '_' ? test(s + 1, sum) :
    *s ? test(s + 1, sum << 1) :
    sum
  );
}

//count number of bits set in a byte
constexpr inline auto count(uintmax x) -> unsigned {
  unsigned count = 0;
  while(x) x &= x - 1, count++;  //clear the least significant bit
  return count;
}

//round up to next highest single bit:
//round(15) == 16, round(16) == 16, round(17) == 32
constexpr inline auto round(uintmax x) -> uintmax {
  if((x & (x - 1)) == 0) return x;
  while(x & (x - 1)) x &= x - 1;
  return x << 1;
}

}
