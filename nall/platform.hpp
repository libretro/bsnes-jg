#pragma once

namespace Math {
  static const long double e  = 2.71828182845904523536;
  static const long double Pi = 3.14159265358979323846;
}

#if defined(_WIN32)
  #include <nall/windows/guard.hpp>
  #include <windows.h>
  #include <wchar.h>
  #include <shellapi.h>
  #include <nall/windows/guard.hpp>
  #include <nall/windows/utf8.hpp>

  inline auto mkdir(const char* path, int mode) -> int { return _wmkdir(nall::utf16_t(path)); }
#endif

#if defined(__clang__) || defined(__GNUC__)
  #define noinline   __attribute__((noinline))
  #define alwaysinline  inline __attribute__((always_inline))
#elif defined(_MSC_VER)
  #define noinline   __declspec(noinline)
  #define alwaysinline  inline __forceinline
#else
  #define noinline
  #define alwaysinline  inline
#endif
