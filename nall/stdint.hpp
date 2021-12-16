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
