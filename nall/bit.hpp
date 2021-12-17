#pragma once

namespace nall {

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
