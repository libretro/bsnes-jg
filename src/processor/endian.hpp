#pragma once

// Reference: https://sourceforge.net/p/predef/wiki/Endianness/

#if defined(__APPLE__)
  #include <machine/endian.h>
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined (__DragonFly__)
  #include <sys/endian.h>
#endif

#if (defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN) \
    || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) \
    || (defined(__FLOAT_WORD_ORDER__) && __FLOAT_WORD_ORDER__ == __ORDER_LITTLE_ENDIAN__) \
    || defined(__LITTLE_ENDIAN__) || defined(__ARMEL__) || defined(__THUMBEL__) \
    || defined(__AARCH64EL__) || defined(_MIPSEL) || defined(__MIPSEL) \
    || defined(__MIPSEL__) || defined(_WIN32) || defined(_WIN64)
  //little-endian: uint8_t[] { 0x01, 0x02, 0x03, 0x04 } == 0x04030201
  #define order_lsb2(a,b)             a,b
  #define order_lsb4(a,b,c,d)         a,b,c,d
#elif (defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN) \
    || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__) \
    || (defined(__FLOAT_WORD_ORDER__) && __FLOAT_WORD_ORDER__ == __ORDER_BIG_ENDIAN__) \
    || defined(__BIG_ENDIAN__) || defined(__ARMEB__) || defined(__THUMBEB__) \
    || defined(__AARCH64EB__) || defined(_MIPSEB) || defined(__MIPSEB) \
    || defined(__MIPSEB__) || defined(__powerpc__) || defined(_M_PPC)
  //big-endian:    uint8_t[] { 0x01, 0x02, 0x03, 0x04 } == 0x01020304
  #define order_lsb2(a,b)             b,a
  #define order_lsb4(a,b,c,d)         d,c,b,a
#else
  #warning "Endianness is unknown, assuming little endian."
  #define order_lsb2(a,b)             a,b
  #define order_lsb4(a,b,c,d)         a,b,c,d
#endif
