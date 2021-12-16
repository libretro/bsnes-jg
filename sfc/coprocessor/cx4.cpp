#include <math.h>
#include <sfc/sfc.hpp>

namespace SuperFamicom {

Cx4 cx4;

const uint8_t Cx4::immediate_data[48] = {
  0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff,
  0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x80, 0xff, 0xff, 0x7f,
  0x00, 0x80, 0x00, 0xff, 0x7f, 0x00, 0xff, 0x7f, 0xff, 0x7f, 0xff, 0xff,
  0x00, 0x00, 0x01, 0xff, 0xff, 0xfe, 0x00, 0x01, 0x00, 0xff, 0xfe, 0x00
};

const uint16_t Cx4::wave_data[40] = {
  0x0000, 0x0002, 0x0004, 0x0006, 0x0008, 0x000a, 0x000c, 0x000e,
  0x0200, 0x0202, 0x0204, 0x0206, 0x0208, 0x020a, 0x020c, 0x020e,
  0x0400, 0x0402, 0x0404, 0x0406, 0x0408, 0x040a, 0x040c, 0x040e,
  0x0600, 0x0602, 0x0604, 0x0606, 0x0608, 0x060a, 0x060c, 0x060e,
  0x0800, 0x0802, 0x0804, 0x0806, 0x0808, 0x080a, 0x080c, 0x080e
};

const uint32_t Cx4::sin_table[256] = {
  0x000000, 0x000324, 0x000648, 0x00096c, 0x000c8f, 0x000fb2, 0x0012d5, 0x0015f6,
  0x001917, 0x001c37, 0x001f56, 0x002273, 0x002590, 0x0028aa, 0x002bc4, 0x002edb,
  0x0031f1, 0x003505, 0x003817, 0x003b26, 0x003e33, 0x00413e, 0x004447, 0x00474d,
  0x004a50, 0x004d50, 0x00504d, 0x005347, 0x00563e, 0x005931, 0x005c22, 0x005f0e,
  0x0061f7, 0x0064dc, 0x0067bd, 0x006a9b, 0x006d74, 0x007049, 0x007319, 0x0075e5,
  0x0078ad, 0x007b70, 0x007e2e, 0x0080e7, 0x00839c, 0x00864b, 0x0088f5, 0x008b9a,
  0x008e39, 0x0090d3, 0x009368, 0x0095f6, 0x00987f, 0x009b02, 0x009d7f, 0x009ff6,
  0x00a267, 0x00a4d2, 0x00a736, 0x00a994, 0x00abeb, 0x00ae3b, 0x00b085, 0x00b2c8,
  0x00b504, 0x00b73a, 0x00b968, 0x00bb8f, 0x00bdae, 0x00bfc7, 0x00c1d8, 0x00c3e2,
  0x00c5e4, 0x00c7de, 0x00c9d1, 0x00cbbb, 0x00cd9f, 0x00cf7a, 0x00d14d, 0x00d318,
  0x00d4db, 0x00d695, 0x00d848, 0x00d9f2, 0x00db94, 0x00dd2d, 0x00debe, 0x00e046,
  0x00e1c5, 0x00e33c, 0x00e4aa, 0x00e60f, 0x00e76b, 0x00e8bf, 0x00ea09, 0x00eb4b,
  0x00ec83, 0x00edb2, 0x00eed8, 0x00eff5, 0x00f109, 0x00f213, 0x00f314, 0x00f40b,
  0x00f4fa, 0x00f5de, 0x00f6ba, 0x00f78b, 0x00f853, 0x00f912, 0x00f9c7, 0x00fa73,
  0x00fb14, 0x00fbac, 0x00fc3b, 0x00fcbf, 0x00fd3a, 0x00fdab, 0x00fe13, 0x00fe70,
  0x00fec4, 0x00ff0e, 0x00ff4e, 0x00ff84, 0x00ffb1, 0x00ffd3, 0x00ffec, 0x00fffb,
  0x000000, 0xfffcdb, 0xfff9b7, 0xfff693, 0xfff370, 0xfff04d, 0xffed2a, 0xffea09,
  0xffe6e8, 0xffe3c8, 0xffe0a9, 0xffdd8c, 0xffda6f, 0xffd755, 0xffd43b, 0xffd124,
  0xffce0e, 0xffcafa, 0xffc7e8, 0xffc4d9, 0xffc1cc, 0xffbec1, 0xffbbb8, 0xffb8b2,
  0xffb5af, 0xffb2af, 0xffafb2, 0xffacb8, 0xffa9c1, 0xffa6ce, 0xffa3dd, 0xffa0f1,
  0xff9e08, 0xff9b23, 0xff9842, 0xff9564, 0xff928b, 0xff8fb6, 0xff8ce6, 0xff8a1a,
  0xff8752, 0xff848f, 0xff81d1, 0xff7f18, 0xff7c63, 0xff79b4, 0xff770a, 0xff7465,
  0xff71c6, 0xff6f2c, 0xff6c97, 0xff6a09, 0xff6780, 0xff64fd, 0xff6280, 0xff6009,
  0xff5d98, 0xff5b2d, 0xff58c9, 0xff566b, 0xff5414, 0xff51c4, 0xff4f7a, 0xff4d37,
  0xff4afb, 0xff48c5, 0xff4697, 0xff4470, 0xff4251, 0xff4038, 0xff3e27, 0xff3c1e,
  0xff3a1b, 0xff3821, 0xff362e, 0xff3444, 0xff3260, 0xff3085, 0xff2eb2, 0xff2ce7,
  0xff2b24, 0xff296a, 0xff27b7, 0xff260d, 0xff246b, 0xff22d2, 0xff2141, 0xff1fb9,
  0xff1e3a, 0xff1cc3, 0xff1b55, 0xff19f0, 0xff1894, 0xff1740, 0xff15f6, 0xff14b4,
  0xff137c, 0xff124d, 0xff1127, 0xff100a, 0xff0ef6, 0xff0dec, 0xff0ceb, 0xff0bf4,
  0xff0b05, 0xff0a21, 0xff0945, 0xff0874, 0xff07ac, 0xff06ed, 0xff0638, 0xff058d,
  0xff04eb, 0xff0453, 0xff03c4, 0xff0340, 0xff02c5, 0xff0254, 0xff01ec, 0xff018f,
  0xff013b, 0xff00f1, 0xff00b1, 0xff007b, 0xff004e, 0xff002c, 0xff0013, 0xff0004
};

const int16_t Cx4::SinTable[512] = {
       0,    402,    804,   1206,   1607,   2009,   2410,   2811,
    3211,   3611,   4011,   4409,   4808,   5205,   5602,   5997,
    6392,   6786,   7179,   7571,   7961,   8351,   8739,   9126,
    9512,   9896,  10278,  10659,  11039,  11416,  11793,  12167,
   12539,  12910,  13278,  13645,  14010,  14372,  14732,  15090,
   15446,  15800,  16151,  16499,  16846,  17189,  17530,  17869,
   18204,  18537,  18868,  19195,  19519,  19841,  20159,  20475,
   20787,  21097,  21403,  21706,  22005,  22301,  22594,  22884,
   23170,  23453,  23732,  24007,  24279,  24547,  24812,  25073,
   25330,  25583,  25832,  26077,  26319,  26557,  26790,  27020,
   27245,  27466,  27684,  27897,  28106,  28310,  28511,  28707,
   28898,  29086,  29269,  29447,  29621,  29791,  29956,  30117,
   30273,  30425,  30572,  30714,  30852,  30985,  31114,  31237,
   31357,  31471,  31581,  31685,  31785,  31881,  31971,  32057,
   32138,  32214,  32285,  32351,  32413,  32469,  32521,  32568,
   32610,  32647,  32679,  32706,  32728,  32745,  32758,  32765,
   32767,  32765,  32758,  32745,  32728,  32706,  32679,  32647,
   32610,  32568,  32521,  32469,  32413,  32351,  32285,  32214,
   32138,  32057,  31971,  31881,  31785,  31685,  31581,  31471,
   31357,  31237,  31114,  30985,  30852,  30714,  30572,  30425,
   30273,  30117,  29956,  29791,  29621,  29447,  29269,  29086,
   28898,  28707,  28511,  28310,  28106,  27897,  27684,  27466,
   27245,  27020,  26790,  26557,  26319,  26077,  25832,  25583,
   25330,  25073,  24812,  24547,  24279,  24007,  23732,  23453,
   23170,  22884,  22594,  22301,  22005,  21706,  21403,  21097,
   20787,  20475,  20159,  19841,  19519,  19195,  18868,  18537,
   18204,  17869,  17530,  17189,  16846,  16499,  16151,  15800,
   15446,  15090,  14732,  14372,  14010,  13645,  13278,  12910,
   12539,  12167,  11793,  11416,  11039,  10659,  10278,   9896,
    9512,   9126,   8739,   8351,   7961,   7571,   7179,   6786,
    6392,   5997,   5602,   5205,   4808,   4409,   4011,   3611,
    3211,   2811,   2410,   2009,   1607,   1206,    804,    402,
       0,   -402,   -804,  -1206,  -1607,  -2009,  -2410,  -2811,
   -3211,  -3611,  -4011,  -4409,  -4808,  -5205,  -5602,  -5997,
   -6392,  -6786,  -7179,  -7571,  -7961,  -8351,  -8739,  -9126,
   -9512,  -9896, -10278, -10659, -11039, -11416, -11793, -12167,
  -12539, -12910, -13278, -13645, -14010, -14372, -14732, -15090,
  -15446, -15800, -16151, -16499, -16846, -17189, -17530, -17869,
  -18204, -18537, -18868, -19195, -19519, -19841, -20159, -20475,
  -20787, -21097, -21403, -21706, -22005, -22301, -22594, -22884,
  -23170, -23453, -23732, -24007, -24279, -24547, -24812, -25073,
  -25330, -25583, -25832, -26077, -26319, -26557, -26790, -27020,
  -27245, -27466, -27684, -27897, -28106, -28310, -28511, -28707,
  -28898, -29086, -29269, -29447, -29621, -29791, -29956, -30117,
  -30273, -30425, -30572, -30714, -30852, -30985, -31114, -31237,
  -31357, -31471, -31581, -31685, -31785, -31881, -31971, -32057,
  -32138, -32214, -32285, -32351, -32413, -32469, -32521, -32568,
  -32610, -32647, -32679, -32706, -32728, -32745, -32758, -32765,
  -32767, -32765, -32758, -32745, -32728, -32706, -32679, -32647,
  -32610, -32568, -32521, -32469, -32413, -32351, -32285, -32214,
  -32138, -32057, -31971, -31881, -31785, -31685, -31581, -31471,
  -31357, -31237, -31114, -30985, -30852, -30714, -30572, -30425,
  -30273, -30117, -29956, -29791, -29621, -29447, -29269, -29086,
  -28898, -28707, -28511, -28310, -28106, -27897, -27684, -27466,
  -27245, -27020, -26790, -26557, -26319, -26077, -25832, -25583,
  -25330, -25073, -24812, -24547, -24279, -24007, -23732, -23453,
  -23170, -22884, -22594, -22301, -22005, -21706, -21403, -21097,
  -20787, -20475, -20159, -19841, -19519, -19195, -18868, -18537,
  -18204, -17869, -17530, -17189, -16846, -16499, -16151, -15800,
  -15446, -15090, -14732, -14372, -14010, -13645, -13278, -12910,
  -12539, -12167, -11793, -11416, -11039, -10659, -10278,  -9896,
   -9512,  -9126,  -8739,  -8351,  -7961,  -7571,  -7179,  -6786,
   -6392,  -5997,  -5602,  -5205,  -4808,  -4409,  -4011,  -3611,
   -3211,  -2811,  -2410,  -2009,  -1607,  -1206,   -804,   -402
};

const int16_t Cx4::CosTable[512] = {
   32767,  32765,  32758,  32745,  32728,  32706,  32679,  32647,
   32610,  32568,  32521,  32469,  32413,  32351,  32285,  32214,
   32138,  32057,  31971,  31881,  31785,  31685,  31581,  31471,
   31357,  31237,  31114,  30985,  30852,  30714,  30572,  30425,
   30273,  30117,  29956,  29791,  29621,  29447,  29269,  29086,
   28898,  28707,  28511,  28310,  28106,  27897,  27684,  27466,
   27245,  27020,  26790,  26557,  26319,  26077,  25832,  25583,
   25330,  25073,  24812,  24547,  24279,  24007,  23732,  23453,
   23170,  22884,  22594,  22301,  22005,  21706,  21403,  21097,
   20787,  20475,  20159,  19841,  19519,  19195,  18868,  18537,
   18204,  17869,  17530,  17189,  16846,  16499,  16151,  15800,
   15446,  15090,  14732,  14372,  14010,  13645,  13278,  12910,
   12539,  12167,  11793,  11416,  11039,  10659,  10278,   9896,
    9512,   9126,   8739,   8351,   7961,   7571,   7179,   6786,
    6392,   5997,   5602,   5205,   4808,   4409,   4011,   3611,
    3211,   2811,   2410,   2009,   1607,   1206,    804,    402,
       0,   -402,   -804,  -1206,  -1607,  -2009,  -2410,  -2811,
   -3211,  -3611,  -4011,  -4409,  -4808,  -5205,  -5602,  -5997,
   -6392,  -6786,  -7179,  -7571,  -7961,  -8351,  -8739,  -9126,
   -9512,  -9896, -10278, -10659, -11039, -11416, -11793, -12167,
  -12539, -12910, -13278, -13645, -14010, -14372, -14732, -15090,
  -15446, -15800, -16151, -16499, -16846, -17189, -17530, -17869,
  -18204, -18537, -18868, -19195, -19519, -19841, -20159, -20475,
  -20787, -21097, -21403, -21706, -22005, -22301, -22594, -22884,
  -23170, -23453, -23732, -24007, -24279, -24547, -24812, -25073,
  -25330, -25583, -25832, -26077, -26319, -26557, -26790, -27020,
  -27245, -27466, -27684, -27897, -28106, -28310, -28511, -28707,
  -28898, -29086, -29269, -29447, -29621, -29791, -29956, -30117,
  -30273, -30425, -30572, -30714, -30852, -30985, -31114, -31237,
  -31357, -31471, -31581, -31685, -31785, -31881, -31971, -32057,
  -32138, -32214, -32285, -32351, -32413, -32469, -32521, -32568,
  -32610, -32647, -32679, -32706, -32728, -32745, -32758, -32765,
  -32767, -32765, -32758, -32745, -32728, -32706, -32679, -32647,
  -32610, -32568, -32521, -32469, -32413, -32351, -32285, -32214,
  -32138, -32057, -31971, -31881, -31785, -31685, -31581, -31471,
  -31357, -31237, -31114, -30985, -30852, -30714, -30572, -30425,
  -30273, -30117, -29956, -29791, -29621, -29447, -29269, -29086,
  -28898, -28707, -28511, -28310, -28106, -27897, -27684, -27466,
  -27245, -27020, -26790, -26557, -26319, -26077, -25832, -25583,
  -25330, -25073, -24812, -24547, -24279, -24007, -23732, -23453,
  -23170, -22884, -22594, -22301, -22005, -21706, -21403, -21097,
  -20787, -20475, -20159, -19841, -19519, -19195, -18868, -18537,
  -18204, -17869, -17530, -17189, -16846, -16499, -16151, -15800,
  -15446, -15090, -14732, -14372, -14010, -13645, -13278, -12910,
  -12539, -12167, -11793, -11416, -11039, -10659, -10278,  -9896,
   -9512,  -9126,  -8739,  -8351,  -7961,  -7571,  -7179,  -6786,
   -6392,  -5997,  -5602,  -5205,  -4808,  -4409,  -4011,  -3611,
   -3211,  -2811,  -2410,  -2009,  -1607,  -1206,   -804,   -402,
       0,    402,    804,   1206,   1607,   2009,   2410,   2811,
    3211,   3611,   4011,   4409,   4808,   5205,   5602,   5997,
    6392,   6786,   7179,   7571,   7961,   8351,   8739,   9126,
    9512,   9896,  10278,  10659,  11039,  11416,  11793,  12167,
   12539,  12910,  13278,  13645,  14010,  14372,  14732,  15090,
   15446,  15800,  16151,  16499,  16846,  17189,  17530,  17869,
   18204,  18537,  18868,  19195,  19519,  19841,  20159,  20475,
   20787,  21097,  21403,  21706,  22005,  22301,  22594,  22884,
   23170,  23453,  23732,  24007,  24279,  24547,  24812,  25073,
   25330,  25583,  25832,  26077,  26319,  26557,  26790,  27020,
   27245,  27466,  27684,  27897,  28106,  28310,  28511,  28707,
   28898,  29086,  29269,  29447,  29621,  29791,  29956,  30117,
   30273,  30425,  30572,  30714,  30852,  30985,  31114,  31237,
   31357,  31471,  31581,  31685,  31785,  31881,  31971,  32057,
   32138,  32214,  32285,  32351,  32413,  32469,  32521,  32568,
   32610,  32647,  32679,  32706,  32728,  32745,  32758,  32765
};

#define Tan(a) (CosTable[a] ? ((((int32_t)SinTable[a]) << 16) / CosTable[a]) : 0x80000000)
#define sar(b, n) ((b) >> (n))
#ifdef PI
#undef PI
#endif
#define PI 3.1415926535897932384626433832795

//Wireframe Helpers
void Cx4::C4TransfWireFrame() {
  double c4x = (double)C4WFXVal;
  double c4y = (double)C4WFYVal;
  double c4z = (double)C4WFZVal - 0x95;
  double tanval, c4x2, c4y2, c4z2;

  //Rotate X
  tanval = -(double)C4WFX2Val * PI * 2 / 128;
  c4y2   = c4y * ::cos(tanval) - c4z * ::sin(tanval);
  c4z2   = c4y * ::sin(tanval) + c4z * ::cos(tanval);

  //Rotate Y
  tanval = -(double)C4WFY2Val * PI * 2 / 128;
  c4x2   = c4x * ::cos(tanval)  + c4z2 * ::sin(tanval);
  c4z    = c4x * -::sin(tanval) + c4z2 * ::cos(tanval);

  //Rotate Z
  tanval = -(double)C4WFDist * PI * 2 / 128;
  c4x    = c4x2 * ::cos(tanval) - c4y2 * ::sin(tanval);
  c4y    = c4x2 * ::sin(tanval) + c4y2 * ::cos(tanval);

  //Scale
  C4WFXVal = (int16_t)(c4x * C4WFScale / (0x90 * (c4z + 0x95)) * 0x95);
  C4WFYVal = (int16_t)(c4y * C4WFScale / (0x90 * (c4z + 0x95)) * 0x95);
}

void Cx4::C4CalcWireFrame() {
  C4WFXVal = C4WFX2Val - C4WFXVal;
  C4WFYVal = C4WFY2Val - C4WFYVal;

  if(abs(C4WFXVal) > abs(C4WFYVal)) {
    C4WFDist = abs(C4WFXVal) + 1;
    C4WFYVal = (256 * (long)C4WFYVal) / abs(C4WFXVal);
    C4WFXVal = (C4WFXVal < 0) ? -256 : 256;
  } else if(C4WFYVal != 0) {
    C4WFDist = abs(C4WFYVal) + 1;
    C4WFXVal = (256 * (long)C4WFXVal) / abs(C4WFYVal);
    C4WFYVal = (C4WFYVal < 0) ? -256 : 256;
  } else {
    C4WFDist = 0;
  }
}

void Cx4::C4TransfWireFrame2() {
  double c4x = (double)C4WFXVal;
  double c4y = (double)C4WFYVal;
  double c4z = (double)C4WFZVal;
  double tanval, c4x2, c4y2, c4z2;

  //Rotate X
  tanval = -(double)C4WFX2Val * PI * 2 / 128;
  c4y2   = c4y * ::cos(tanval) - c4z * ::sin(tanval);
  c4z2   = c4y * ::sin(tanval) + c4z * ::cos(tanval);

  //Rotate Y
  tanval = -(double)C4WFY2Val * PI * 2 / 128;
  c4x2   = c4x * ::cos(tanval)  + c4z2 * ::sin(tanval);
  c4z    = c4x * -::sin(tanval) + c4z2 * ::cos(tanval);

  //Rotate Z
  tanval = -(double)C4WFDist * PI * 2 / 128;
  c4x    = c4x2 * ::cos(tanval) - c4y2 * ::sin(tanval);
  c4y    = c4x2 * ::sin(tanval) + c4y2 * ::cos(tanval);

  //Scale
  C4WFXVal = (int16_t)(c4x * C4WFScale / 0x100);
  C4WFYVal = (int16_t)(c4y * C4WFScale / 0x100);
}

void Cx4::C4DrawWireFrame() {
  uint32_t line = readl(0x1f80);
  uint32_t point1, point2;
  int16_t X1, Y1, Z1;
  int16_t X2, Y2, Z2;
  uint8_t Color;

  for(int32_t i = ram[0x0295]; i > 0; i--, line += 5) {
    if(bus.read(line) == 0xff && bus.read(line + 1) == 0xff) {
      int32_t tmp = line - 5;
      while(bus.read(tmp + 2) == 0xff && bus.read(tmp + 3) == 0xff && (tmp + 2) >= 0) { tmp -= 5; }
      point1 = (read(0x1f82) << 16) | (bus.read(tmp + 2) << 8) | bus.read(tmp + 3);
    } else {
      point1 = (read(0x1f82) << 16) | (bus.read(line) << 8) | bus.read(line + 1);
    }
    point2 = (read(0x1f82) << 16) | (bus.read(line + 2) << 8) | bus.read(line + 3);

    X1=(bus.read(point1 + 0) << 8) | bus.read(point1 + 1);
    Y1=(bus.read(point1 + 2) << 8) | bus.read(point1 + 3);
    Z1=(bus.read(point1 + 4) << 8) | bus.read(point1 + 5);
    X2=(bus.read(point2 + 0) << 8) | bus.read(point2 + 1);
    Y2=(bus.read(point2 + 2) << 8) | bus.read(point2 + 3);
    Z2=(bus.read(point2 + 4) << 8) | bus.read(point2 + 5);
    Color = bus.read(line + 4);
    C4DrawLine(X1, Y1, Z1, X2, Y2, Z2, Color);
  }
}

void Cx4::C4DrawLine(int32_t X1, int32_t Y1, int16_t Z1, int32_t X2, int32_t Y2, int16_t Z2, uint8_t Color) {
  //Transform coordinates
  C4WFXVal  = (int16_t)X1;
  C4WFYVal  = (int16_t)Y1;
  C4WFZVal  = Z1;
  C4WFScale = read(0x1f90);
  C4WFX2Val = read(0x1f86);
  C4WFY2Val = read(0x1f87);
  C4WFDist  = read(0x1f88);
  C4TransfWireFrame2();
  X1 = (C4WFXVal + 48) << 8;
  Y1 = (C4WFYVal + 48) << 8;

  C4WFXVal = (int16_t)X2;
  C4WFYVal = (int16_t)Y2;
  C4WFZVal = Z2;
  C4TransfWireFrame2();
  X2 = (C4WFXVal + 48) << 8;
  Y2 = (C4WFYVal + 48) << 8;

  //Get line info
  C4WFXVal  = (int16_t)(X1 >> 8);
  C4WFYVal  = (int16_t)(Y1 >> 8);
  C4WFX2Val = (int16_t)(X2 >> 8);
  C4WFY2Val = (int16_t)(Y2 >> 8);
  C4CalcWireFrame();
  X2 = (int16_t)C4WFXVal;
  Y2 = (int16_t)C4WFYVal;

  //Render line
  for(int32_t i = C4WFDist ? C4WFDist : (int16_t)1; i > 0; i--) {
    if(X1 > 0xff && Y1 > 0xff && X1 < 0x6000 && Y1 < 0x6000) {
      uint16_t addr = (((Y1 >> 8) >> 3) << 8) - (((Y1 >> 8) >> 3) << 6) + (((X1 >> 8) >> 3) << 4) + ((Y1 >> 8) & 7) * 2;
      uint8_t bit = 0x80 >> ((X1 >> 8) & 7);
      ram[addr + 0x300] &= ~bit;
      ram[addr + 0x301] &= ~bit;
      if(Color & 1) ram[addr + 0x300] |= bit;
      if(Color & 2) ram[addr + 0x301] |= bit;
    }
    X1 += X2;
    Y1 += Y2;
  }
}

void Cx4::C4DoScaleRotate(int row_padding) {
  int16_t A, B, C, D;

  //Calculate matrix
  int32_t XScale = readw(0x1f8f);
  int32_t YScale = readw(0x1f92);

  if(XScale & 0x8000)XScale = 0x7fff;
  if(YScale & 0x8000)YScale = 0x7fff;

  if(readw(0x1f80) == 0) {  //no rotation
    A = (int16_t)XScale;
    B = 0;
    C = 0;
    D = (int16_t)YScale;
  } else if(readw(0x1f80) == 128) {  //90 degree rotation
    A = 0;
    B = (int16_t)(-YScale);
    C = (int16_t)XScale;
    D = 0;
  } else if(readw(0x1f80) == 256) {  //180 degree rotation
    A = (int16_t)(-XScale);
    B = 0;
    C = 0;
    D = (int16_t)(-YScale);
  } else if(readw(0x1f80) == 384) {  //270 degree rotation
    A = 0;
    B = (int16_t)YScale;
    C = (int16_t)(-XScale);
    D = 0;
  } else {
    A = (int16_t)  sar(CosTable[readw(0x1f80) & 0x1ff] * XScale, 15);
    B = (int16_t)(-sar(SinTable[readw(0x1f80) & 0x1ff] * YScale, 15));
    C = (int16_t)  sar(SinTable[readw(0x1f80) & 0x1ff] * XScale, 15);
    D = (int16_t)  sar(CosTable[readw(0x1f80) & 0x1ff] * YScale, 15);
  }

  //Calculate Pixel Resolution
  uint8_t w = read(0x1f89) & ~7;
  uint8_t h = read(0x1f8c) & ~7;

  //Clear the output RAM
  memset(ram, 0, (w + row_padding / 4) * h / 2);

  int32_t Cx = (int16_t)readw(0x1f83);
  int32_t Cy = (int16_t)readw(0x1f86);

  //Calculate start position (i.e. (Ox, Oy) = (0, 0))
  //The low 12 bits are fractional, so (Cx<<12) gives us the Cx we want in
  //the function. We do Cx*A etc normally because the matrix parameters
  //already have the fractional parts.
  int32_t LineX = (Cx << 12) - Cx * A - Cx * B;
  int32_t LineY = (Cy << 12) - Cy * C - Cy * D;

  //Start loop
  uint32_t X, Y;
  uint8_t byte;
  int32_t outidx = 0;
  uint8_t bit    = 0x80;

  for(int32_t y = 0; y < h; y++) {
    X = LineX;
    Y = LineY;
    for(int32_t x = 0; x < w; x++) {
      if((X >> 12) >= w || (Y >> 12) >= h) {
        byte = 0;
      } else {
        uint32_t addr = (Y >> 12) * w + (X >> 12);
        byte = read(0x600 + (addr >> 1));
        if(addr & 1) { byte >>= 4; }
      }

      //De-bitplanify
      if(byte & 1) ram[outidx     ] |= bit;
      if(byte & 2) ram[outidx +  1] |= bit;
      if(byte & 4) ram[outidx + 16] |= bit;
      if(byte & 8) ram[outidx + 17] |= bit;

      bit >>= 1;
      if(!bit) {
        bit     = 0x80;
        outidx += 32;
      }

      X += A;  //Add 1 to output x => add an A and a C
      Y += C;
    }
    outidx += 2 + row_padding;
    if(outidx & 0x10) {
      outidx &= ~0x10;
    } else {
      outidx -= w * 4 + row_padding;
    }
    LineX += B;  //Add 1 to output y => add a B and a D
    LineY += D;
  }
}

//Build OAM
void Cx4::op00_00() {
  uint32_t oamptr = ram[0x626] << 2;
  for(int32_t i = 0x1fd; i > oamptr && i >= 0; i -= 4) {
    //clear oam-to-be
    if(i >= 0) ram[i] = 0xe0;
  }

  uint16_t globalx, globaly;
  uint32_t oamptr2;
  int16_t  sprx, spry;
  uint8_t  sprname, sprattr;
  uint8_t  sprcount;

  globalx = readw(0x621);
  globaly = readw(0x623);
  oamptr2 = 0x200 + (ram[0x626] >> 2);

  if(!ram[0x620]) return;

  sprcount = 128 - ram[0x626];
  uint8_t offset = (ram[0x626] & 3) * 2;
  uint32_t srcptr = 0x220;

  for(int i = ram[0x620]; i > 0 && sprcount > 0; i--, srcptr += 16) {
    sprx = readw(srcptr)     - globalx;
    spry = readw(srcptr + 2) - globaly;
    sprname = ram[srcptr + 5];
    sprattr = ram[srcptr + 4] | ram[srcptr + 6];

    uint32_t spraddr = readl(srcptr + 7);
    if(bus.read(spraddr)) {
      int16_t x, y;
      for(int sprcnt = bus.read(spraddr++); sprcnt > 0 && sprcount > 0; sprcnt--, spraddr += 4) {
        x = (int8_t)bus.read(spraddr + 1);
        if(sprattr & 0x40) {
          x = -x - ((bus.read(spraddr) & 0x20) ? 16 : 8);
        }
        x += sprx;
        if(x >= -16 && x <= 272) {
          y = (int8_t)bus.read(spraddr + 2);
          if(sprattr & 0x80) {
            y = -y - ((bus.read(spraddr) & 0x20) ? 16 : 8);
          }
          y += spry;
          if(y >= -16 && y <= 224) {
            ram[oamptr    ] = (uint8_t)x;
            ram[oamptr + 1] = (uint8_t)y;
            ram[oamptr + 2] = sprname + bus.read(spraddr + 3);
            ram[oamptr + 3] = sprattr ^ (bus.read(spraddr) & 0xc0);
            ram[oamptr2] &= ~(3 << offset);
            if(x & 0x100) ram[oamptr2] |= 1 << offset;
            if(bus.read(spraddr) & 0x20) ram[oamptr2] |= 2 << offset;
            oamptr += 4;
            sprcount--;
            offset = (offset + 2) & 6;
            if(!offset)oamptr2++;
          }
        }
      }
    } else if(sprcount > 0) {
      ram[oamptr    ] = (uint8_t)sprx;
      ram[oamptr + 1] = (uint8_t)spry;
      ram[oamptr + 2] = sprname;
      ram[oamptr + 3] = sprattr;
      ram[oamptr2] &= ~(3 << offset);
      if(sprx & 0x100) ram[oamptr2] |= 3 << offset;
      else ram[oamptr2] |= 2 << offset;
      oamptr += 4;
      sprcount--;
      offset = (offset + 2) & 6;
      if(!offset) oamptr2++;
    }
  }
}

//Scale and Rotate
void Cx4::op00_03() {
  C4DoScaleRotate(0);
}

//Transform Lines
void Cx4::op00_05() {
  C4WFX2Val = read(0x1f83);
  C4WFY2Val = read(0x1f86);
  C4WFDist  = read(0x1f89);
  C4WFScale = read(0x1f8c);

//Transform Vertices
uint32_t ptr = 0;
  for(int32_t i = readw(0x1f80); i > 0; i--, ptr += 0x10) {
    C4WFXVal = readw(ptr + 1);
    C4WFYVal = readw(ptr + 5);
    C4WFZVal = readw(ptr + 9);
    C4TransfWireFrame();

    //Displace
    writew(ptr + 1, C4WFXVal + 0x80);
    writew(ptr + 5, C4WFYVal + 0x50);
  }

  writew(0x600,     23);
  writew(0x602,     0x60);
  writew(0x605,     0x40);
  writew(0x600 + 8, 23);
  writew(0x602 + 8, 0x60);
  writew(0x605 + 8, 0x40);

  ptr = 0xb02;
  uint32_t ptr2 = 0;

  for(int32_t i = readw(0xb00); i > 0; i--, ptr += 2, ptr2 += 8) {
    C4WFXVal  = readw((read(ptr + 0) << 4) + 1);
    C4WFYVal  = readw((read(ptr + 0) << 4) + 5);
    C4WFX2Val = readw((read(ptr + 1) << 4) + 1);
    C4WFY2Val = readw((read(ptr + 1) << 4) + 5);
    C4CalcWireFrame();
    writew(ptr2 + 0x600, C4WFDist ? C4WFDist : (int16_t)1);
    writew(ptr2 + 0x602, C4WFXVal);
    writew(ptr2 + 0x605, C4WFYVal);
  }
}

//Scale and Rotate
void Cx4::op00_07() {
  C4DoScaleRotate(64);
}

//Draw Wireframe
void Cx4::op00_08() {
  C4DrawWireFrame();
}

//Disintegrate
void Cx4::op00_0b() {
  uint8_t  width, height;
  uint32_t startx, starty;
  uint32_t srcptr;
  uint32_t x, y;
  int32_t  scalex, scaley;
  int32_t cx, cy;
  int32_t  i, j;

  width  = read(0x1f89);
  height = read(0x1f8c);
  cx     = readw(0x1f80);
  cy     = readw(0x1f83);

  scalex = (int16_t)readw(0x1f86);
  scaley = (int16_t)readw(0x1f8f);
  startx = -cx * scalex + (cx << 8);
  starty = -cy * scaley + (cy << 8);
  srcptr = 0x600;

  for(i = 0; i < (width * height) >> 1; i++) {
    write(i, 0);
  }

  for(y = starty, i = 0;i < height; i++, y += scaley) {
    for(x = startx, j = 0;j < width; j++, x += scalex) {
      if((x >> 8) < width && (y >> 8) < height && (y >> 8) * width + (x >> 8) < 0x2000) {
        uint8_t pixel = (j & 1) ? (uint8_t)(ram[srcptr] >> 4) : (ram[srcptr]);
        int32_t index = (y >> 11) * width * 4 + (x >> 11) * 32 + ((y >> 8) & 7) * 2;
        uint8_t mask = 0x80 >> ((x >> 8) & 7);

        if(pixel & 1) ram[index     ] |= mask;
        if(pixel & 2) ram[index +  1] |= mask;
        if(pixel & 4) ram[index + 16] |= mask;
        if(pixel & 8) ram[index + 17] |= mask;
      }
      if(j & 1) srcptr++;
    }
  }
}

//Bitplane Wave
void Cx4::op00_0c() {
  uint32_t destptr = 0;
  uint32_t waveptr = read(0x1f83);
  uint16_t mask1   = 0xc0c0;
  uint16_t mask2   = 0x3f3f;

  for(int j = 0; j < 0x10; j++) {
    do {
      int16_t height = -((int8_t)read(waveptr + 0xb00)) - 16;
      for(int i = 0; i < 40; i++) {
        uint16_t temp = readw(destptr + wave_data[i]) & mask2;
        if(height >= 0) {
          if(height < 8) {
            temp |= mask1 & readw(0xa00 + height * 2);
          } else {
            temp |= mask1 & 0xff00;
          }
        }
        writew(destptr + wave_data[i], temp);
        height++;
      }
      waveptr = (waveptr + 1) & 0x7f;
      mask1   = (mask1 >> 2) | (mask1 << 6);
      mask2   = (mask2 >> 2) | (mask2 << 6);
    } while(mask1 != 0xc0c0);
    destptr += 16;

    do {
      int16_t height = -((int8_t)read(waveptr + 0xb00)) - 16;
      for(int i = 0; i < 40; i++) {
        uint16_t temp = readw(destptr + wave_data[i]) & mask2;
        if(height >= 0) {
          if(height < 8) {
            temp |= mask1 & readw(0xa10 + height * 2);
          } else {
            temp |= mask1 & 0xff00;
          }
        }
        writew(destptr + wave_data[i], temp);
        height++;
      }
      waveptr = (waveptr + 1) & 0x7f;
      mask1   = (mask1 >> 2) | (mask1 << 6);
      mask2   = (mask2 >> 2) | (mask2 << 6);
    } while(mask1 != 0xc0c0);
    destptr += 16;
  }
}

//Sprite Functions
void Cx4::op00() {
  switch(reg[0x4d]) {
    case 0x00: op00_00(); break;
    case 0x03: op00_03(); break;
    case 0x05: op00_05(); break;
    case 0x07: op00_07(); break;
    case 0x08: op00_08(); break;
    case 0x0b: op00_0b(); break;
    case 0x0c: op00_0c(); break;
  }
}

//Draw Wireframe
void Cx4::op01() {
  memset(ram + 0x300, 0, 2304);
  C4DrawWireFrame();
}

//Propulsion
void Cx4::op05() {
  int32_t temp = 0x10000;
  if(readw(0x1f83)) {
    temp = sar((temp / readw(0x1f83)) * readw(0x1f81), 8);
  }
  writew(0x1f80, temp);
}

//Set Vector length
void Cx4::op0d() {
  C41FXVal    = readw(0x1f80);
  C41FYVal    = readw(0x1f83);
  C41FDistVal = readw(0x1f86);
  double tanval = sqrt(((double)C41FYVal) * ((double)C41FYVal) + ((double)C41FXVal) * ((double)C41FXVal));
  tanval = (double)C41FDistVal / tanval;
  C41FYVal = (int16_t)(((double)C41FYVal * tanval) * 0.99);
  C41FXVal = (int16_t)(((double)C41FXVal * tanval) * 0.98);
  writew(0x1f89, C41FXVal);
  writew(0x1f8c, C41FYVal);
}

//Triangle
void Cx4::op10() {
  r0 = ldr(0);
  r1 = ldr(1);

  r4 = r0 & 0x1ff;
  if(r1 & 0x8000)r1 |= ~0x7fff;
  else r1 &= 0x7fff;

  mul(cos(r4), r1, r5, r2);
  r5 = (r5 >> 16) & 0xff;
  r2 = (r2 << 8) + r5;

  mul(sin(r4), r1, r5, r3);
  r5 = (r5 >> 16) & 0xff;
  r3 = (r3 << 8) + r5;

  str(0, r0);
  str(1, r1);
  str(2, r2);
  str(3, r3);
  str(4, r4);
  str(5, r5);
}

//Triangle
void Cx4::op13() {
  r0 = ldr(0);
  r1 = ldr(1);

  r4 = r0 & 0x1ff;

  mul(cos(r4), r1, r5, r2);
  r5 = (r5 >> 8) & 0xffff;
  r2 = (r2 << 16) + r5;

  mul(sin(r4), r1, r5, r3);
  r5 = (r5 >> 8) & 0xffff;
  r3 = (r3 << 16) + r5;

  str(0, r0);
  str(1, r1);
  str(2, r2);
  str(3, r3);
  str(4, r4);
  str(5, r5);
}

//Pythagorean
void Cx4::op15() {
  C41FXVal = readw(0x1f80);
  C41FYVal = readw(0x1f83);
  C41FDist = (int16_t)sqrt((double)C41FXVal * (double)C41FXVal + (double)C41FYVal * (double)C41FYVal);
  writew(0x1f80, C41FDist);
}

//Calculate distance
void Cx4::op1f() {
  C41FXVal = readw(0x1f80);
  C41FYVal = readw(0x1f83);
  if(!C41FXVal) {
    C41FAngleRes = (C41FYVal > 0) ? 0x080 : 0x180;
  } else {
    double tanval = ((double)C41FYVal) / ((double)C41FXVal);
    C41FAngleRes = (short)(atan(tanval) / (PI * 2) * 512);
    C41FAngleRes = C41FAngleRes;
    if(C41FXVal < 0) {
      C41FAngleRes += 0x100;
    }
    C41FAngleRes &= 0x1ff;
  }
  writew(0x1f86, C41FAngleRes);
}

//Trapezoid
void Cx4::op22() {
  int16_t angle1 = readw(0x1f8c) & 0x1ff;
  int16_t angle2 = readw(0x1f8f) & 0x1ff;
  int32_t tan1 = Tan(angle1);
  int32_t tan2 = Tan(angle2);
  int16_t y = readw(0x1f83) - readw(0x1f89);
  int16_t left, right;

  for(int32_t j = 0; j < 225; j++, y++) {
    if(y >= 0) {
      left  = sar((int32_t)tan1 * y, 16) - readw(0x1f80) + readw(0x1f86);
      right = sar((int32_t)tan2 * y, 16) - readw(0x1f80) + readw(0x1f86) + readw(0x1f93);

      if(left < 0 && right < 0) {
        left  = 1;
        right = 0;
      } else if(left < 0) {
        left  = 0;
      } else if(right < 0) {
        right = 0;
      }

      if(left > 255 && right > 255) {
        left  = 255;
        right = 254;
      } else if(left > 255) {
        left  = 255;
      } else if(right > 255) {
        right = 255;
      }
    } else {
      left  = 1;
      right = 0;
    }
    ram[j + 0x800] = (uint8_t)left;
    ram[j + 0x900] = (uint8_t)right;
  }
}

//Multiply
void Cx4::op25() {
  r0 = ldr(0);
  r1 = ldr(1);
  mul(r0, r1, r0, r1);
  str(0, r0);
  str(1, r1);
}

//Transform Coords
void Cx4::op2d() {
  C4WFXVal  = readw(0x1f81);
  C4WFYVal  = readw(0x1f84);
  C4WFZVal  = readw(0x1f87);
  C4WFX2Val = read (0x1f89);
  C4WFY2Val = read (0x1f8a);
  C4WFDist  = read (0x1f8b);
  C4WFScale = readw(0x1f90);
  C4TransfWireFrame2();
  writew(0x1f80, C4WFXVal);
  writew(0x1f83, C4WFYVal);
}

//Sum
void Cx4::op40() {
  r0 = 0;
  for(uint32_t i=0;i<0x800;i++) {
    r0 += ram[i];
  }
  str(0, r0);
}

//Square
void Cx4::op54() {
  r0 = ldr(0);
  mul(r0, r0, r1, r2);
  str(1, r1);
  str(2, r2);
}

//Immediate Register
void Cx4::op5c() {
  str(0, 0x000000);
  immediate_reg(0);
}

//Immediate Register (Multiple)
void Cx4::op5e() { immediate_reg( 0); }
void Cx4::op60() { immediate_reg( 3); }
void Cx4::op62() { immediate_reg( 6); }
void Cx4::op64() { immediate_reg( 9); }
void Cx4::op66() { immediate_reg(12); }
void Cx4::op68() { immediate_reg(15); }
void Cx4::op6a() { immediate_reg(18); }
void Cx4::op6c() { immediate_reg(21); }
void Cx4::op6e() { immediate_reg(24); }
void Cx4::op70() { immediate_reg(27); }
void Cx4::op72() { immediate_reg(30); }
void Cx4::op74() { immediate_reg(33); }
void Cx4::op76() { immediate_reg(36); }
void Cx4::op78() { immediate_reg(39); }
void Cx4::op7a() { immediate_reg(42); }
void Cx4::op7c() { immediate_reg(45); }

//Immediate ROM
void Cx4::op89() {
  str(0, 0x054336);
  str(1, 0xffffff);
}

auto Cx4::serialize(serializer& s) -> void {
  s.array(ram);
  s.array(reg);

  s.integer(r0);
  s.integer(r1);
  s.integer(r2);
  s.integer(r3);
  s.integer(r4);
  s.integer(r5);
  s.integer(r6);
  s.integer(r7);
  s.integer(r8);
  s.integer(r9);
  s.integer(r10);
  s.integer(r11);
  s.integer(r12);
  s.integer(r13);
  s.integer(r14);
  s.integer(r15);

  s.integer(C4WFXVal);
  s.integer(C4WFYVal);
  s.integer(C4WFZVal);
  s.integer(C4WFX2Val);
  s.integer(C4WFY2Val);
  s.integer(C4WFDist);
  s.integer(C4WFScale);

  s.integer(C41FXVal);
  s.integer(C41FYVal);
  s.integer(C41FAngleRes);
  s.integer(C41FDist);
  s.integer(C41FDistVal);
}

auto Cx4::power() -> void {
  memset(ram, 0, 0x0c00);
  memset(reg, 0, 0x0100);
}

uint32_t Cx4::ldr(uint8_t r) {
  uint16_t addr = 0x0080 + (r * 3);
  return (reg[addr + 0] <<  0)
       | (reg[addr + 1] <<  8)
       | (reg[addr + 2] << 16);
}

void Cx4::str(uint8_t r, uint32_t data) {
  uint16_t addr = 0x0080 + (r * 3);
  reg[addr + 0] = (data >>  0);
  reg[addr + 1] = (data >>  8);
  reg[addr + 2] = (data >> 16);
}

void Cx4::mul(uint32_t x, uint32_t y, uint32_t &rl, uint32_t &rh) {
  int64_t rx = x & 0xffffff;
  int64_t ry = y & 0xffffff;
  if(rx & 0x800000)rx |= ~0x7fffff;
  if(ry & 0x800000)ry |= ~0x7fffff;

  rx *= ry;

  rl = (rx)       & 0xffffff;
  rh = (rx >> 24) & 0xffffff;
}

uint32_t Cx4::sin(uint32_t rx) {
  r0 = rx & 0x1ff;
  if(r0 & 0x100)r0 ^= 0x1ff;
  if(r0 & 0x080)r0 ^= 0x0ff;
  if(rx & 0x100) {
    return sin_table[r0 + 0x80];
  } else {
    return sin_table[r0];
  }
}

uint32_t Cx4::cos(uint32_t rx) {
  return sin(rx + 0x080);
}

void Cx4::immediate_reg(uint32_t start) {
  r0 = ldr(0);
  for(uint32_t i = start; i < 48; i++) {
    if((r0 & 0x0fff) < 0x0c00) {
      ram[r0 & 0x0fff] = immediate_data[i];
    }
    r0++;
  }
  str(0, r0);
}

void Cx4::transfer_data() {
  uint32_t src;
  uint16_t dest, count;

  src   = (reg[0x40]) | (reg[0x41] << 8) | (reg[0x42] << 16);
  count = (reg[0x43]) | (reg[0x44] << 8);
  dest  = (reg[0x45]) | (reg[0x46] << 8);

  for(uint32_t i=0;i<count;i++) {
    write(dest++, bus.read(src++));
  }
}

void Cx4::write(uint addr, uint8_t data) {
  addr &= 0x1fff;

  if(addr < 0x0c00) {
    //ram
    ram[addr] = data;
    return;
  }

  if(addr < 0x1f00) {
    //unmapped
    return;
  }

  //command register
  reg[addr & 0xff] = data;

  if(addr == 0x1f47) {
    //memory transfer
    transfer_data();
    return;
  }

  if(addr == 0x1f4f) {
    //c4 command
    if(reg[0x4d] == 0x0e && !(data & 0xc3)) {
      //c4 test command
      reg[0x80] = data >> 2;
      return;
    }

    switch(data) {
      case 0x00: op00(); break;
      case 0x01: op01(); break;
      case 0x05: op05(); break;
      case 0x0d: op0d(); break;
      case 0x10: op10(); break;
      case 0x13: op13(); break;
      case 0x15: op15(); break;
      case 0x1f: op1f(); break;
      case 0x22: op22(); break;
      case 0x25: op25(); break;
      case 0x2d: op2d(); break;
      case 0x40: op40(); break;
      case 0x54: op54(); break;
      case 0x5c: op5c(); break;
      case 0x5e: op5e(); break;
      case 0x60: op60(); break;
      case 0x62: op62(); break;
      case 0x64: op64(); break;
      case 0x66: op66(); break;
      case 0x68: op68(); break;
      case 0x6a: op6a(); break;
      case 0x6c: op6c(); break;
      case 0x6e: op6e(); break;
      case 0x70: op70(); break;
      case 0x72: op72(); break;
      case 0x74: op74(); break;
      case 0x76: op76(); break;
      case 0x78: op78(); break;
      case 0x7a: op7a(); break;
      case 0x7c: op7c(); break;
      case 0x89: op89(); break;
    }
  }
}

void Cx4::writeb(uint16_t addr, uint8_t data) {
  write(addr,     data);
}

void Cx4::writew(uint16_t addr, uint16_t data) {
  write(addr + 0, data >> 0);
  write(addr + 1, data >> 8);
}

void Cx4::writel(uint16_t addr, uint32_t data) {
  write(addr + 0, data >>  0);
  write(addr + 1, data >>  8);
  write(addr + 2, data >> 16);
}

uint8_t Cx4::read(uint addr, uint8_t data) {
  addr &= 0x1fff;

  if(addr < 0x0c00) {
    return ram[addr];
  }

  if(addr >= 0x1f00) {
    return reg[addr & 0xff];
  }

  return cpu.r.mdr;
}

uint8_t Cx4::readb(uint16_t addr) {
  return read(addr);
}

uint16_t Cx4::readw(uint16_t addr) {
  return read(addr) | (read(addr + 1) << 8);
}

uint32_t Cx4::readl(uint16_t addr) {
  return read(addr) | (read(addr + 1) << 8) + (read(addr + 2) << 16);
}

}
