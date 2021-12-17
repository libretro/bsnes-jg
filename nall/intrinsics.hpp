#pragma once

//note: (u)intmax actually mean it: use as many bits as is possible
#if defined(__SIZEOF_INT128__)
  using int128_t = signed __int128;
  using uint128_t = unsigned __int128;

  using intmax = int128_t;
  using uintmax = uint128_t;
#else
  using intmax = intmax_t;
  using uintmax = uintmax_t;
#endif

/* Compiler detection */

namespace nall {

#if defined(__clang__)
  #pragma clang diagnostic ignored "-Winconsistent-missing-override"
  #pragma clang diagnostic ignored "-Wparentheses"
  #pragma clang diagnostic ignored "-Wshift-count-overflow"
  #pragma clang diagnostic ignored "-Wswitch"
#endif

}

#if defined(__clang__) || defined(__GNUC__)
  #define noinline   __attribute__((noinline))
  #define alwaysinline  inline __attribute__((always_inline))
#else
  #define noinline
  #define alwaysinline  inline
#endif
