// Sets up common environment for Shay Green's libraries.
// To change configuration options, modify blargg_config.h, not this file.

// snes_spc 0.9.0
#ifndef BLARGG_COMMON_H
#define BLARGG_COMMON_H

#include <stddef.h>
#include <stdlib.h>
#include <limits.h>

// blargg_err_t (0 on success, otherwise error string)
#ifndef blargg_err_t
	typedef const char* blargg_err_t;
#endif

// blargg_vector - very lightweight vector of POD types (no constructor/destructor)
template<class T>
class blargg_vector {
	T* begin_;
	size_t size_;
public:
	blargg_vector() : begin_( 0 ), size_( 0 ) { }
	~blargg_vector();
	size_t size() const;
	T* begin() const;
	T* end() const;
	blargg_err_t resize( size_t );
	void clear();
	T& operator [] ( size_t ) const;
};

#ifndef BLARGG_DISABLE_NOTHROW
	// throw spec mandatory in ISO C++ if operator new can return NULL
	#if __cplusplus >= 199711 || defined (__GNUC__)
		#define BLARGG_THROWS( spec ) throw spec
	#else
		#define BLARGG_THROWS( spec )
	#endif
	#define BLARGG_DISABLE_NOTHROW \
		void* operator new ( size_t s ) BLARGG_THROWS(()) { return malloc( s ); }\
		void operator delete ( void* p ) { free( p ); }
	#define BLARGG_NEW new
#else
	#include <new>
	#define BLARGG_NEW new (std::nothrow)
#endif

// BLARGG_4CHAR('a','b','c','d') = 'abcd' (four character integer constant)
#define BLARGG_4CHAR( a, b, c, d ) \
	((a&0xFF)*0x1000000L + (b&0xFF)*0x10000L + (c&0xFF)*0x100L + (d&0xFF))

// blargg_long/blargg_ulong = at least 32 bits, int if it's big enough

#if INT_MAX < 0x7FFFFFFF || LONG_MAX == 0x7FFFFFFF
	typedef long blargg_long;
#else
	typedef int blargg_long;
#endif

#if UINT_MAX < 0xFFFFFFFF || ULONG_MAX == 0xFFFFFFFF
	typedef unsigned long blargg_ulong;
#else
	typedef unsigned blargg_ulong;
#endif

#endif
