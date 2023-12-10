// CPU Byte Order Utilities

// snes_spc 0.9.0
#ifndef BLARGG_ENDIAN
#define BLARGG_ENDIAN

#include "blargg_common.h"

inline unsigned get_le16( void const* p )
{
	return  (unsigned) ((unsigned char const*) p) [1] << 8 |
			(unsigned) ((unsigned char const*) p) [0];
}

inline unsigned get_be16( void const* p )
{
	return  (unsigned) ((unsigned char const*) p) [0] << 8 |
			(unsigned) ((unsigned char const*) p) [1];
}

inline blargg_ulong get_le32( void const* p )
{
	return  (blargg_ulong) ((unsigned char const*) p) [3] << 24 |
			(blargg_ulong) ((unsigned char const*) p) [2] << 16 |
			(blargg_ulong) ((unsigned char const*) p) [1] <<  8 |
			(blargg_ulong) ((unsigned char const*) p) [0];
}

inline blargg_ulong get_be32( void const* p )
{
	return  (blargg_ulong) ((unsigned char const*) p) [0] << 24 |
			(blargg_ulong) ((unsigned char const*) p) [1] << 16 |
			(blargg_ulong) ((unsigned char const*) p) [2] <<  8 |
			(blargg_ulong) ((unsigned char const*) p) [3];
}

inline void set_le16( void* p, unsigned n )
{
	((unsigned char*) p) [1] = (unsigned char) (n >> 8);
	((unsigned char*) p) [0] = (unsigned char) n;
}

inline void set_be16( void* p, unsigned n )
{
	((unsigned char*) p) [0] = (unsigned char) (n >> 8);
	((unsigned char*) p) [1] = (unsigned char) n;
}

inline void set_le32( void* p, blargg_ulong n )
{
	((unsigned char*) p) [0] = (unsigned char) n;
	((unsigned char*) p) [1] = (unsigned char) (n >> 8);
	((unsigned char*) p) [2] = (unsigned char) (n >> 16);
	((unsigned char*) p) [3] = (unsigned char) (n >> 24);
}

inline void set_be32( void* p, blargg_ulong n )
{
	((unsigned char*) p) [3] = (unsigned char) n;
	((unsigned char*) p) [2] = (unsigned char) (n >> 8);
	((unsigned char*) p) [1] = (unsigned char) (n >> 16);
	((unsigned char*) p) [0] = (unsigned char) (n >> 24);
}

#ifndef GET_LE16
	#define GET_LE16( addr )        get_le16( addr )
	#define SET_LE16( addr, data )  set_le16( addr, data )
#endif

#ifndef GET_LE32
	#define GET_LE32( addr )        get_le32( addr )
	#define SET_LE32( addr, data )  set_le32( addr, data )
#endif

#ifndef GET_BE16
	#define GET_BE16( addr )        get_be16( addr )
	#define SET_BE16( addr, data )  set_be16( addr, data )
#endif

#ifndef GET_BE32
	#define GET_BE32( addr )        get_be32( addr )
	#define SET_BE32( addr, data )  set_be32( addr, data )
#endif

// auto-selecting versions

inline void set_le( BOOST::uint16_t* p, unsigned     n ) { SET_LE16( p, n ); }
inline void set_le( BOOST::uint32_t* p, blargg_ulong n ) { SET_LE32( p, n ); }
inline void set_be( BOOST::uint16_t* p, unsigned     n ) { SET_BE16( p, n ); }
inline void set_be( BOOST::uint32_t* p, blargg_ulong n ) { SET_BE32( p, n ); }
inline unsigned     get_le( BOOST::uint16_t* p ) { return GET_LE16( p ); }
inline blargg_ulong get_le( BOOST::uint32_t* p ) { return GET_LE32( p ); }
inline unsigned     get_be( BOOST::uint16_t* p ) { return GET_BE16( p ); }
inline blargg_ulong get_be( BOOST::uint32_t* p ) { return GET_BE32( p ); }

#endif
