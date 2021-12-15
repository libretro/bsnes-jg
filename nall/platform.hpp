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

#if defined(COMPILER_MICROSOFT)
  #define va_copy(dest, src) ((dest) = (src))
#endif

#if defined(PLATFORM_WINDOWS)
  #undef  IN
  #undef  OUT
  #undef  interface
  #define dllexport __declspec(dllexport)
  #define MSG_NOSIGNAL 0

  inline auto access(const char* path, int amode) -> int { return _waccess(nall::utf16_t(path), amode); }
  inline auto getcwd(char* buf, size_t size) -> char* { wchar_t wpath[PATH_MAX] = L""; if(!_wgetcwd(wpath, size)) return nullptr; strcpy(buf, nall::utf8_t(wpath)); return buf; }
  inline auto mkdir(const char* path, int mode) -> int { return _wmkdir(nall::utf16_t(path)); }
  inline auto putenv(const char* value) -> int { return _wputenv(nall::utf16_t(value)); }
  inline auto realpath(const char* file_name, char* resolved_name) -> char* { wchar_t wfile_name[PATH_MAX] = L""; if(!_wfullpath(wfile_name, nall::utf16_t(file_name), PATH_MAX)) return nullptr; strcpy(resolved_name, nall::utf8_t(wfile_name)); return resolved_name; }
  inline auto rename(const char* oldname, const char* newname) -> int { return _wrename(nall::utf16_t(oldname), nall::utf16_t(newname)); }

#else
  #define dllexport
#endif

#if defined(PLATFORM_MACOS)
  #define MSG_NOSIGNAL 0
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
