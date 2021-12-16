#include <sfc/sfc.hpp>

namespace SuperFamicom {

namespace DSP4i {
  #define  bool8  uint8_t
  #define   int8   int8_t
  #define  int16  int16_t
  #define  int32  int32_t
  #define  int64  int64_t
  #define  uint8_t  uint8_t
  #define uint16 uint16_t
  #define uint32 uint32_t
  #define uint64 uint64_t
  #define DSP4_CPP
  inline uint16 READ_WORD(uint8_t *addr) {
    return (addr[0]) + (addr[1] << 8);
  }
  inline uint32 READ_DWORD(uint8_t *addr) {
    return (addr[0]) + (addr[1] << 8) + (addr[2] << 16) + (addr[3] << 24);
  }
  inline void WRITE_WORD(uint8_t *addr, uint16 data) {
    addr[0] = data;
    addr[1] = data >> 8;
  }
  #include "dsp4/dsp4emu.c"
  #undef  bool8
  #undef   int8
  #undef  int16
  #undef  int32
  #undef  int64
  #undef  uint8
  #undef uint16
  #undef uint32
  #undef uint64
}

DSP4 dsp4;

auto DSP4::serialize(serializer& s) -> void {
  s.integer(DSP4i::DSP4.waiting4command);
  s.integer(DSP4i::DSP4.half_command);
  s.integer(DSP4i::DSP4.command);
  s.integer(DSP4i::DSP4.in_count);
  s.integer(DSP4i::DSP4.in_index);
  s.integer(DSP4i::DSP4.out_count);
  s.integer(DSP4i::DSP4.out_index);
  s.array(DSP4i::DSP4.parameters);
  s.array(DSP4i::DSP4.output);

  s.integer(DSP4i::DSP4_vars.DSP4_Logic);
  s.integer(DSP4i::DSP4_vars.lcv);
  s.integer(DSP4i::DSP4_vars.distance);
  s.integer(DSP4i::DSP4_vars.raster);
  s.integer(DSP4i::DSP4_vars.segments);
  s.integer(DSP4i::DSP4_vars.world_x);
  s.integer(DSP4i::DSP4_vars.world_y);
  s.integer(DSP4i::DSP4_vars.world_dx);
  s.integer(DSP4i::DSP4_vars.world_dy);
  s.integer(DSP4i::DSP4_vars.world_ddx);
  s.integer(DSP4i::DSP4_vars.world_ddy);
  s.integer(DSP4i::DSP4_vars.world_xenv);
  s.integer(DSP4i::DSP4_vars.world_yofs);
  s.integer(DSP4i::DSP4_vars.view_x1);
  s.integer(DSP4i::DSP4_vars.view_y1);
  s.integer(DSP4i::DSP4_vars.view_x2);
  s.integer(DSP4i::DSP4_vars.view_y2);
  s.integer(DSP4i::DSP4_vars.view_dx);
  s.integer(DSP4i::DSP4_vars.view_dy);
  s.integer(DSP4i::DSP4_vars.view_xofs1);
  s.integer(DSP4i::DSP4_vars.view_yofs1);
  s.integer(DSP4i::DSP4_vars.view_xofs2);
  s.integer(DSP4i::DSP4_vars.view_yofs2);
  s.integer(DSP4i::DSP4_vars.view_yofsenv);
  s.integer(DSP4i::DSP4_vars.view_turnoff_x);
  s.integer(DSP4i::DSP4_vars.view_turnoff_dx);
  s.integer(DSP4i::DSP4_vars.viewport_cx);
  s.integer(DSP4i::DSP4_vars.viewport_cy);
  s.integer(DSP4i::DSP4_vars.viewport_left);
  s.integer(DSP4i::DSP4_vars.viewport_right);
  s.integer(DSP4i::DSP4_vars.viewport_top);
  s.integer(DSP4i::DSP4_vars.viewport_bottom);
  s.integer(DSP4i::DSP4_vars.sprite_x);
  s.integer(DSP4i::DSP4_vars.sprite_y);
  s.integer(DSP4i::DSP4_vars.sprite_attr);
  s.integer(DSP4i::DSP4_vars.sprite_size);
  s.integer(DSP4i::DSP4_vars.sprite_clipy);
  s.integer(DSP4i::DSP4_vars.sprite_count);

  s.array(DSP4i::DSP4_vars.poly_clipLf[0]);
  s.array(DSP4i::DSP4_vars.poly_clipLf[1]);
  s.array(DSP4i::DSP4_vars.poly_clipRt[0]);
  s.array(DSP4i::DSP4_vars.poly_clipRt[1]);
  s.array(DSP4i::DSP4_vars.poly_ptr[0]);
  s.array(DSP4i::DSP4_vars.poly_ptr[1]);
  s.array(DSP4i::DSP4_vars.poly_raster[0]);
  s.array(DSP4i::DSP4_vars.poly_raster[1]);
  s.array(DSP4i::DSP4_vars.poly_top[0]);
  s.array(DSP4i::DSP4_vars.poly_top[1]);
  s.array(DSP4i::DSP4_vars.poly_bottom[0]);
  s.array(DSP4i::DSP4_vars.poly_bottom[1]);
  s.array(DSP4i::DSP4_vars.poly_cx[0]);
  s.array(DSP4i::DSP4_vars.poly_cx[1]);
  s.array(DSP4i::DSP4_vars.poly_start);
  s.array(DSP4i::DSP4_vars.poly_plane);
  s.array(DSP4i::DSP4_vars.OAM_attr);
  s.integer(DSP4i::DSP4_vars.OAM_index);
  s.integer(DSP4i::DSP4_vars.OAM_bits);
  s.integer(DSP4i::DSP4_vars.OAM_RowMax);
  s.array(DSP4i::DSP4_vars.OAM_Row);
}

auto DSP4::power() -> void {
  DSP4i::InitDSP4();
}

auto DSP4::read(unsigned addr, uint8_t data) -> uint8_t {
  if(addr & 1) return 0x80;

  DSP4i::dsp4_address = addr;
  DSP4i::DSP4GetByte();
  return DSP4i::dsp4_byte;
}

auto DSP4::write(unsigned addr, uint8_t data) -> void {
  if(addr & 1) return;

  DSP4i::dsp4_address = addr;
  DSP4i::dsp4_byte = data;
  DSP4i::DSP4SetByte();
}

}
