#pragma once

/* Compiler detection */

namespace nall {

#if defined(__clang__)
  #pragma clang diagnostic warning "-Wreturn-type"
  #pragma clang diagnostic ignored "-Wunused-result"
  #pragma clang diagnostic ignored "-Wunknown-pragmas"
  #pragma clang diagnostic ignored "-Wempty-body"
  #pragma clang diagnostic ignored "-Wparentheses"
  #pragma clang diagnostic ignored "-Wswitch"
  #pragma clang diagnostic ignored "-Wswitch-bool"
  #pragma clang diagnostic ignored "-Wtautological-compare"
  #pragma clang diagnostic ignored "-Wabsolute-value"
  #pragma clang diagnostic ignored "-Wshift-count-overflow"
  #pragma clang diagnostic ignored "-Wtrigraphs"
#elif defined(__GNUC__)
  #pragma GCC diagnostic warning "-Wreturn-type"
  #pragma GCC diagnostic ignored "-Wunused-result"
  #pragma GCC diagnostic ignored "-Wunknown-pragmas"
  #pragma GCC diagnostic ignored "-Wpragmas"
  #pragma GCC diagnostic ignored "-Wswitch-bool"
  #pragma GCC diagnostic ignored "-Wtrigraphs"
#elif defined(_MSC_VER)
  #pragma warning(disable:4996)  //libc "deprecation" warnings
#endif

}

/* Endian detection */

#if defined(__APPLE__)
  #include <machine/endian.h>
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined (__DragonFly__)
  #include <sys/endian.h>
#endif

namespace nall {

// A note on endian constants: Traditional UNIX provides a header that defines
// constants LITTLE_ENDIAN, BIG_ENDIAN, and BYTE_ORDER (set to LITTLE_ENDIAN or
// BIG_ENDIAN as appropriate). However, C89 says that the compiler/libc should
// not introduce any names unless they start with an underscore, so when you're
// compiling in standards-compilant mode, those constants are named
// __LITTLE_ENDIAN, or sometimes _LITTLE_ENDIAN, or sometimes even LITTLE_ENDIAN
// on platforms that care more about tradition than standards. The platforms
// that rename the constants usually provide some other name you can #define to
// say, "forget C89, yes I really want traditional constant names", but *that*
// name also differs from platform to platform, and it affects more than just
// the endian header.
//
// Rather than wade into that mess, let's just test for all the constants we
// know about.

#if  (defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && __BYTE_ORDER == __LITTLE_ENDIAN) \
  || (defined( _BYTE_ORDER) && defined( _LITTLE_ENDIAN) &&  _BYTE_ORDER ==  _LITTLE_ENDIAN) \
  || (defined(  BYTE_ORDER) && defined(  LITTLE_ENDIAN) &&   BYTE_ORDER ==   LITTLE_ENDIAN) \
  || defined(__LITTLE_ENDIAN__) \
  || defined(__i386__) || defined(__amd64__) \
  || defined(_M_IX86) || defined(_M_AMD64)
  #define ENDIAN_LSB
#elif(defined(__BYTE_ORDER) && defined(__BIG_ENDIAN) && __BYTE_ORDER == __BIG_ENDIAN) \
  || (defined( _BYTE_ORDER) && defined( _BIG_ENDIAN) &&  _BYTE_ORDER ==  _BIG_ENDIAN) \
  || (defined(  BYTE_ORDER) && defined(  BIG_ENDIAN) &&   BYTE_ORDER ==   BIG_ENDIAN) \
  || defined(__BIG_ENDIAN__) \
  || defined(__powerpc__) || defined(_M_PPC)
  #define ENDIAN_MSB
#else
  #warning "unable to detect endian"
  #define ENDIAN_UNKNOWN
#endif

}
