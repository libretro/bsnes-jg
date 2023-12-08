#pragma once

#if defined(__APPLE__)
  #include <machine/endian.h>
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined (__DragonFly__)
  #include <sys/endian.h>
#endif

#if  (defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && __BYTE_ORDER == __LITTLE_ENDIAN) \
  || (defined( _BYTE_ORDER) && defined( _LITTLE_ENDIAN) &&  _BYTE_ORDER ==  _LITTLE_ENDIAN) \
  || (defined(  BYTE_ORDER) && defined(  LITTLE_ENDIAN) &&   BYTE_ORDER ==   LITTLE_ENDIAN) \
  || defined(__LITTLE_ENDIAN__) \
  || defined(__i386__) || defined(__amd64__) || defined(__aarch64__) \
  || defined(_M_IX86) || defined(_M_AMD64)
  //little-endian: uint8_t[] { 0x01, 0x02, 0x03, 0x04 } == 0x04030201
  #define order_lsb2(a,b)             a,b
  #define order_lsb4(a,b,c,d)         a,b,c,d
#elif(defined(__BYTE_ORDER) && defined(__BIG_ENDIAN) && __BYTE_ORDER == __BIG_ENDIAN) \
  || (defined( _BYTE_ORDER) && defined( _BIG_ENDIAN) &&  _BYTE_ORDER ==  _BIG_ENDIAN) \
  || (defined(  BYTE_ORDER) && defined(  BIG_ENDIAN) &&   BYTE_ORDER ==   BIG_ENDIAN) \
  || defined(__BIG_ENDIAN__) \
  || defined(__powerpc__) || defined(_M_PPC)
  //big-endian:    uint8_t[] { 0x01, 0x02, 0x03, 0x04 } == 0x01020304
  #define order_lsb2(a,b)             b,a
  #define order_lsb4(a,b,c,d)         d,c,b,a
#else
  #warning "Endianness is unknown, assuming little endian."
  #define order_lsb2(a,b)             a,b
  #define order_lsb4(a,b,c,d)         a,b,c,d
#endif
