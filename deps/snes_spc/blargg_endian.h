// CPU Byte Order Utilities

// snes_spc 0.9.0
#ifndef BLARGG_ENDIAN
#define BLARGG_ENDIAN

inline unsigned get_le16( void const* );
inline unsigned get_be16( void const* );
inline blargg_ulong get_le32( void const* );
inline blargg_ulong get_be32( void const* );
inline void set_le16( void*, unsigned );
inline void set_be16( void*, unsigned );
inline void set_le32( void*, blargg_ulong );
inline void set_be32( void*, blargg_ulong );

// auto-selecting versions

inline void set_le( BOOST::uint16_t*, unsigned     );
inline void set_le( BOOST::uint32_t*, blargg_ulong );
inline void set_be( BOOST::uint16_t*, unsigned     );
inline void set_be( BOOST::uint32_t*, blargg_ulong );
inline unsigned     get_le( BOOST::uint16_t* );
inline blargg_ulong get_le( BOOST::uint32_t* );
inline unsigned     get_be( BOOST::uint16_t* );
inline blargg_ulong get_be( BOOST::uint32_t* );

#endif
