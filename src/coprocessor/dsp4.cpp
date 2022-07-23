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

#include "serializer.hpp"

#include "dsp4emu.hpp"

#include "dsp4.hpp"

namespace SuperFamicom {

DSP4 dsp4;
static DSP4_t dsp4emu;
static DSP4_vars_t dsp4emu_vars;

void DSP4::serialize(serializer& s) {
  s.integer(dsp4emu.waiting4command);
  s.integer(dsp4emu.half_command);
  s.integer(dsp4emu.command);
  s.integer(dsp4emu.in_count);
  s.integer(dsp4emu.in_index);
  s.integer(dsp4emu.out_count);
  s.integer(dsp4emu.out_index);
  s.array(dsp4emu.parameters);
  s.array(dsp4emu.output);

  s.integer(dsp4emu_vars.DSP4_Logic);
  s.integer(dsp4emu_vars.lcv);
  s.integer(dsp4emu_vars.distance);
  s.integer(dsp4emu_vars.raster);
  s.integer(dsp4emu_vars.segments);
  s.integer(dsp4emu_vars.world_x);
  s.integer(dsp4emu_vars.world_y);
  s.integer(dsp4emu_vars.world_dx);
  s.integer(dsp4emu_vars.world_dy);
  s.integer(dsp4emu_vars.world_ddx);
  s.integer(dsp4emu_vars.world_ddy);
  s.integer(dsp4emu_vars.world_xenv);
  s.integer(dsp4emu_vars.world_yofs);
  s.integer(dsp4emu_vars.view_x1);
  s.integer(dsp4emu_vars.view_y1);
  s.integer(dsp4emu_vars.view_x2);
  s.integer(dsp4emu_vars.view_y2);
  s.integer(dsp4emu_vars.view_dx);
  s.integer(dsp4emu_vars.view_dy);
  s.integer(dsp4emu_vars.view_xofs1);
  s.integer(dsp4emu_vars.view_yofs1);
  s.integer(dsp4emu_vars.view_xofs2);
  s.integer(dsp4emu_vars.view_yofs2);
  s.integer(dsp4emu_vars.view_yofsenv);
  s.integer(dsp4emu_vars.view_turnoff_x);
  s.integer(dsp4emu_vars.view_turnoff_dx);
  s.integer(dsp4emu_vars.viewport_cx);
  s.integer(dsp4emu_vars.viewport_cy);
  s.integer(dsp4emu_vars.viewport_left);
  s.integer(dsp4emu_vars.viewport_right);
  s.integer(dsp4emu_vars.viewport_top);
  s.integer(dsp4emu_vars.viewport_bottom);
  s.integer(dsp4emu_vars.sprite_x);
  s.integer(dsp4emu_vars.sprite_y);
  s.integer(dsp4emu_vars.sprite_attr);
  s.integer(dsp4emu_vars.sprite_size);
  s.integer(dsp4emu_vars.sprite_clipy);
  s.integer(dsp4emu_vars.sprite_count);

  s.array(dsp4emu_vars.poly_clipLf[0]);
  s.array(dsp4emu_vars.poly_clipLf[1]);
  s.array(dsp4emu_vars.poly_clipRt[0]);
  s.array(dsp4emu_vars.poly_clipRt[1]);
  s.array(dsp4emu_vars.poly_ptr[0]);
  s.array(dsp4emu_vars.poly_ptr[1]);
  s.array(dsp4emu_vars.poly_raster[0]);
  s.array(dsp4emu_vars.poly_raster[1]);
  s.array(dsp4emu_vars.poly_top[0]);
  s.array(dsp4emu_vars.poly_top[1]);
  s.array(dsp4emu_vars.poly_bottom[0]);
  s.array(dsp4emu_vars.poly_bottom[1]);
  s.array(dsp4emu_vars.poly_cx[0]);
  s.array(dsp4emu_vars.poly_cx[1]);
  s.array(dsp4emu_vars.poly_start);
  s.array(dsp4emu_vars.poly_plane);
  s.array(dsp4emu_vars.OAM_attr);
  s.integer(dsp4emu_vars.OAM_index);
  s.integer(dsp4emu_vars.OAM_bits);
  s.integer(dsp4emu_vars.OAM_RowMax);
  s.array(dsp4emu_vars.OAM_Row);
}

void DSP4::power() {
  InitDSP4();
}

uint8_t DSP4::read(unsigned addr, uint8_t data) {
  if (data) {}
  if(addr & 1) return 0x80;

  dsp4_address = addr;
  DSP4GetByte();
  return dsp4_byte;
}

void DSP4::write(unsigned addr, uint8_t data) {
  if(addr & 1) return;

  dsp4_address = addr;
  dsp4_byte = data;
  DSP4SetByte();
}

}
