/*
 * bsnes-jg - Super Nintendo emulator
 *
 * Copyright (C) 2004-2020 byuu
 * Copyright (C) 2020-2022 Rupert Carmichael
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, specifically version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "../sfc.hpp"

#include "dsp4emu.hpp"

namespace SuperFamicom {

DSP4 dsp4;

void DSP4::serialize(serializer& s) {
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

void DSP4::power() {
  DSP4i::InitDSP4();
}

uint8_t DSP4::read(unsigned addr, uint8_t data) {
  if(addr & 1) return 0x80;

  DSP4i::dsp4_address = addr;
  DSP4i::DSP4GetByte();
  return DSP4i::dsp4_byte;
}

void DSP4::write(unsigned addr, uint8_t data) {
  if(addr & 1) return;

  DSP4i::dsp4_address = addr;
  DSP4i::dsp4_byte = data;
  DSP4i::DSP4SetByte();
}

}
