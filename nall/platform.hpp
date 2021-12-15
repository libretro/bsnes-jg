#pragma once

namespace Math {
  static const long double e  = 2.71828182845904523536;
  static const long double Pi = 3.14159265358979323846;
}

#if defined(PLATFORM_WINDOWS)
  #include <nall/windows/guard.hpp>
  #include <initguid.h>
  #include <cguid.h>
  #include <windows.h>
  #include <direct.h>
  #include <io.h>
  #include <wchar.h>
  #include <shlobj.h>
  #include <shellapi.h>
  #include <nall/windows/guard.hpp>
  #include <nall/windows/utf8.hpp>
#endif

#include <atomic>
#include <limits>
#include <mutex>
#include <utility>

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <utime.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#if !defined(PLATFORM_WINDOWS)
  #include <dlfcn.h>
  #include <unistd.h>
  #include <pwd.h>
  #include <grp.h>
#endif

#if defined(PLATFORM_WINDOWS)
  inline auto mkdir(const char* path, int mode) -> int { return _wmkdir(nall::utf16_t(path)); }
#endif

#if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
  #define noinline   __attribute__((noinline))
  #define alwaysinline  inline __attribute__((always_inline))
#elif defined(COMPILER_MICROSOFT)
  #define noinline   __declspec(noinline)
  #define alwaysinline  inline __forceinline
#else
  #define noinline
  #define alwaysinline  inline
#endif

//P0627: [[unreachable]] -- impossible to simulate with identical syntax, must omit brackets ...
#if defined(COMPILER_CLANG) || defined(COMPILER_GCC)
  #define unreachable __builtin_unreachable()
#else
  #define unreachable throw
#endif

#define export $export
#define register $register
