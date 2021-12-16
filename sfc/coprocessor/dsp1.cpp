#include <sfc/sfc.hpp>

namespace SuperFamicom {

#define   int8   int8_t
#define  int16  int16_t
#define  int32  int32_t
#define  int64  int64_t
#define  uint8_t  uint8_t
#define uint16 uint16_t
#define uint32 uint32_t
#define uint64 uint64_t
#define DSP1_CPP
#include "dsp1/dsp1emu.cpp"
Dsp1 dsp1emu;
#undef   int8
#undef  int16
#undef  int32
#undef  int64
#undef  uint8
#undef uint16
#undef uint32
#undef uint64

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

auto DSP1::read(uint addr, uint8_t data) -> uint8_t {
  if(addr & 1) {
    return dsp1emu.getSr();
  } else {
    return dsp1emu.getDr();
  }
}

auto DSP1::write(uint addr, uint8_t data) -> void {
  if(addr & 1) {
  } else {
    return dsp1emu.setDr(data);
  }
}

}
