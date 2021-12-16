#pragma once

#if defined(_MSC_VER)
  typedef signed char int8_t;
  typedef signed short int16_t;
  typedef signed int int32_t;
  typedef signed long long int64_t;
  typedef int64_t intmax_t;
  #if defined(_WIN64)
  typedef int64_t intptr_t;
  #else
  typedef int32_t intptr_t;
  #endif

  typedef unsigned char uint8_t;
  typedef unsigned short uint16_t;
  typedef unsigned int uint32_t;
  typedef unsigned long long uint64_t;
  typedef uint64_t uintmax_t;
  #if defined(_WIN64)
  typedef uint64_t uintptr_t;
  #else
  typedef uint32_t uintptr_t;
  #endif
#endif

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

using uint = unsigned int;
