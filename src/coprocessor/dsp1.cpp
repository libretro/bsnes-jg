#include "../sfc.hpp"

namespace SuperFamicom {

#include "dsp1emu.hpp"

Dsp1 dsp1emu;
DSP1 dsp1;

auto DSP1::serialize(serializer& s) -> void {
  dsp1emu.serialize(s);
}

auto Dsp1::serialize(serializer &s) -> void {
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

auto DSP1::power() -> void {
  dsp1emu.reset();
}

auto DSP1::read(unsigned addr, uint8_t data) -> uint8_t {
  if(addr & 1) {
    return dsp1emu.getSr();
  } else {
    return dsp1emu.getDr();
  }
}

auto DSP1::write(unsigned addr, uint8_t data) -> void {
  if(addr & 1) {
  } else {
    return dsp1emu.setDr(data);
  }
}

}
