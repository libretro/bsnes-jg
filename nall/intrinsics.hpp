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
#endif

}

#if defined(__clang__) || defined(__GNUC__)
  #define noinline   __attribute__((noinline))
  #define alwaysinline  inline __attribute__((always_inline))
#else
  #define noinline
  #define alwaysinline  inline
#endif
