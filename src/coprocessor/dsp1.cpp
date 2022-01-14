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

#include "dsp1emu.hpp"

#include "dsp1.hpp"

namespace SuperFamicom {

Dsp1 dsp1emu;
DSP1 dsp1;

void DSP1::serialize(serializer& s) {
  dsp1emu.serialize(s);
}

void Dsp1::serialize(serializer &s) {
  for(unsigned i = 0; i < 3; i++) {
    s.array(shared.MatrixA[i]);
    s.array(shared.MatrixB[i]);
    s.array(shared.MatrixC[i]);
  }

  s.integer(shared.CentreX);
  s.integer(shared.CentreY);
  s.integer(shared.CentreZ);
  s.integer(shared.CentreZ_C);
  s.integer(shared.CentreZ_E);
  s.integer(shared.VOffset);
  s.integer(shared.Les);
  s.integer(shared.C_Les);
  s.integer(shared.E_Les);
  s.integer(shared.SinAas);
  s.integer(shared.CosAas);
  s.integer(shared.SinAzs);
  s.integer(shared.CosAzs);
  s.integer(shared.SinAZS);
  s.integer(shared.CosAZS);
  s.integer(shared.SecAZS_C1);
  s.integer(shared.SecAZS_E1);
  s.integer(shared.SecAZS_C2);
  s.integer(shared.SecAZS_E2);
  s.integer(shared.Nx);
  s.integer(shared.Ny);
  s.integer(shared.Nz);
  s.integer(shared.Gx);
  s.integer(shared.Gy);
  s.integer(shared.Gz);
  s.integer(shared.Hx);
  s.integer(shared.Hy);
  s.integer(shared.Vx);
  s.integer(shared.Vy);
  s.integer(shared.Vz);

  s.integer(mSr);
  s.integer(mSrLowByteAccess);
  s.integer(mDr);
  s.integer(mFsmMajorState);
  s.integer(mCommand);
  s.integer(mDataCounter);
  s.array(mReadBuffer);
  s.array(mWriteBuffer);
  s.integer(mFreeze);
}

void DSP1::power() {
  dsp1emu.reset();
}

uint8_t DSP1::read(unsigned addr, uint8_t data) {
  if (data) {}
  if(addr & 1) {
    return dsp1emu.getSr();
  } else {
    return dsp1emu.getDr();
  }
}

void DSP1::write(unsigned addr, uint8_t data) {
  if(addr & 1) {
  } else {
    return dsp1emu.setDr(data);
  }
}

}
