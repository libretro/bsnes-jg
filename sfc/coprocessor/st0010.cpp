#include <sfc/sfc.hpp>

namespace SuperFamicom {

const int16_t ST0010::sin_table[256] = {
   0x0000,  0x0324,  0x0648,  0x096a,  0x0c8c,  0x0fab,  0x12c8,  0x15e2,
   0x18f9,  0x1c0b,  0x1f1a,  0x2223,  0x2528,  0x2826,  0x2b1f,  0x2e11,
   0x30fb,  0x33df,  0x36ba,  0x398c,  0x3c56,  0x3f17,  0x41ce,  0x447a,
   0x471c,  0x49b4,  0x4c3f,  0x4ebf,  0x5133,  0x539b,  0x55f5,  0x5842,
   0x5a82,  0x5cb3,  0x5ed7,  0x60eb,  0x62f1,  0x64e8,  0x66cf,  0x68a6,
   0x6a6d,  0x6c23,  0x6dc9,  0x6f5e,  0x70e2,  0x7254,  0x73b5,  0x7504,
   0x7641,  0x776b,  0x7884,  0x7989,  0x7a7c,  0x7b5c,  0x7c29,  0x7ce3,
   0x7d89,  0x7e1d,  0x7e9c,  0x7f09,  0x7f61,  0x7fa6,  0x7fd8,  0x7ff5,
   0x7fff,  0x7ff5,  0x7fd8,  0x7fa6,  0x7f61,  0x7f09,  0x7e9c,  0x7e1d,
   0x7d89,  0x7ce3,  0x7c29,  0x7b5c,  0x7a7c,  0x7989,  0x7884,  0x776b,
   0x7641,  0x7504,  0x73b5,  0x7254,  0x70e2,  0x6f5e,  0x6dc9,  0x6c23,
   0x6a6d,  0x68a6,  0x66cf,  0x64e8,  0x62f1,  0x60eb,  0x5ed7,  0x5cb3,
   0x5a82,  0x5842,  0x55f5,  0x539b,  0x5133,  0x4ebf,  0x4c3f,  0x49b4,
   0x471c,  0x447a,  0x41ce,  0x3f17,  0x3c56,  0x398c,  0x36ba,  0x33df,
   0x30fb,  0x2e11,  0x2b1f,  0x2826,  0x2528,  0x2223,  0x1f1a,  0x1c0b,
   0x18f8,  0x15e2,  0x12c8,  0x0fab,  0x0c8c,  0x096a,  0x0648,  0x0324,
   0x0000, -0x0324, -0x0648, -0x096b, -0x0c8c, -0x0fab, -0x12c8, -0x15e2,
  -0x18f9, -0x1c0b, -0x1f1a, -0x2223, -0x2528, -0x2826, -0x2b1f, -0x2e11,
  -0x30fb, -0x33df, -0x36ba, -0x398d, -0x3c56, -0x3f17, -0x41ce, -0x447a,
  -0x471c, -0x49b4, -0x4c3f, -0x4ebf, -0x5133, -0x539b, -0x55f5, -0x5842,
  -0x5a82, -0x5cb3, -0x5ed7, -0x60ec, -0x62f1, -0x64e8, -0x66cf, -0x68a6,
  -0x6a6d, -0x6c23, -0x6dc9, -0x6f5e, -0x70e2, -0x7254, -0x73b5, -0x7504,
  -0x7641, -0x776b, -0x7884, -0x7989, -0x7a7c, -0x7b5c, -0x7c29, -0x7ce3,
  -0x7d89, -0x7e1d, -0x7e9c, -0x7f09, -0x7f61, -0x7fa6, -0x7fd8, -0x7ff5,
  -0x7fff, -0x7ff5, -0x7fd8, -0x7fa6, -0x7f61, -0x7f09, -0x7e9c, -0x7e1d,
  -0x7d89, -0x7ce3, -0x7c29, -0x7b5c, -0x7a7c, -0x7989, -0x7883, -0x776b,
  -0x7641, -0x7504, -0x73b5, -0x7254, -0x70e2, -0x6f5e, -0x6dc9, -0x6c23,
  -0x6a6d, -0x68a6, -0x66cf, -0x64e8, -0x62f1, -0x60eb, -0x5ed7, -0x5cb3,
  -0x5a82, -0x5842, -0x55f5, -0x539a, -0x5133, -0x4ebf, -0x4c3f, -0x49b3,
  -0x471c, -0x447a, -0x41cd, -0x3f17, -0x3c56, -0x398c, -0x36b9, -0x33de,
  -0x30fb, -0x2e10, -0x2b1f, -0x2826, -0x2527, -0x2223, -0x1f19, -0x1c0b,
  -0x18f8, -0x15e2, -0x12c8, -0x0fab, -0x0c8b, -0x096a, -0x0647, -0x0324
};

const int16_t ST0010::mode7_scale[176] = {
  0x0380,  0x0325,  0x02da,  0x029c,  0x0268,  0x023b,  0x0215,  0x01f3,
  0x01d5,  0x01bb,  0x01a3,  0x018e,  0x017b,  0x016a,  0x015a,  0x014b,
  0x013e,  0x0132,  0x0126,  0x011c,  0x0112,  0x0109,  0x0100,  0x00f8,
  0x00f0,  0x00e9,  0x00e3,  0x00dc,  0x00d6,  0x00d1,  0x00cb,  0x00c6,
  0x00c1,  0x00bd,  0x00b8,  0x00b4,  0x00b0,  0x00ac,  0x00a8,  0x00a5,
  0x00a2,  0x009e,  0x009b,  0x0098,  0x0095,  0x0093,  0x0090,  0x008d,
  0x008b,  0x0088,  0x0086,  0x0084,  0x0082,  0x0080,  0x007e,  0x007c,
  0x007a,  0x0078,  0x0076,  0x0074,  0x0073,  0x0071,  0x006f,  0x006e,
  0x006c,  0x006b,  0x0069,  0x0068,  0x0067,  0x0065,  0x0064,  0x0063,
  0x0062,  0x0060,  0x005f,  0x005e,  0x005d,  0x005c,  0x005b,  0x005a,
  0x0059,  0x0058,  0x0057,  0x0056,  0x0055,  0x0054,  0x0053,  0x0052,
  0x0051,  0x0051,  0x0050,  0x004f,  0x004e,  0x004d,  0x004d,  0x004c,
  0x004b,  0x004b,  0x004a,  0x0049,  0x0048,  0x0048,  0x0047,  0x0047,
  0x0046,  0x0045,  0x0045,  0x0044,  0x0044,  0x0043,  0x0042,  0x0042,
  0x0041,  0x0041,  0x0040,  0x0040,  0x003f,  0x003f,  0x003e,  0x003e,
  0x003d,  0x003d,  0x003c,  0x003c,  0x003b,  0x003b,  0x003a,  0x003a,
  0x003a,  0x0039,  0x0039,  0x0038,  0x0038,  0x0038,  0x0037,  0x0037,
  0x0036,  0x0036,  0x0036,  0x0035,  0x0035,  0x0035,  0x0034,  0x0034,
  0x0034,  0x0033,  0x0033,  0x0033,  0x0032,  0x0032,  0x0032,  0x0031,
  0x0031,  0x0031,  0x0030,  0x0030,  0x0030,  0x0030,  0x002f,  0x002f,
  0x002f,  0x002e,  0x002e,  0x002e,  0x002e,  0x002d,  0x002d,  0x002d,
  0x002d,  0x002c,  0x002c,  0x002c,  0x002c,  0x002b,  0x002b,  0x002b
};

const uint8_t ST0010::arctan[32][32] = {
  { 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80 },
  { 0x80, 0xa0, 0xad, 0xb3, 0xb6, 0xb8, 0xb9, 0xba, 0xbb, 0xbb, 0xbc, 0xbc, 0xbd, 0xbd, 0xbd, 0xbd,
    0xbd, 0xbe, 0xbe, 0xbe, 0xbe, 0xbe, 0xbe, 0xbe, 0xbe, 0xbe, 0xbe, 0xbe, 0xbf, 0xbf, 0xbf, 0xbf },
  { 0x80, 0x93, 0xa0, 0xa8, 0xad, 0xb0, 0xb3, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xb9, 0xba, 0xba, 0xbb,
    0xbb, 0xbb, 0xbb, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd, 0xbd },
  { 0x80, 0x8d, 0x98, 0xa0, 0xa6, 0xaa, 0xad, 0xb0, 0xb1, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb7, 0xb8,
    0xb8, 0xb9, 0xb9, 0xba, 0xba, 0xba, 0xba, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbc, 0xbc, 0xbc, 0xbc },
  { 0x80, 0x8a, 0x93, 0x9a, 0xa0, 0xa5, 0xa8, 0xab, 0xad, 0xaf, 0xb0, 0xb2, 0xb3, 0xb4, 0xb5, 0xb5,
    0xb6, 0xb7, 0xb7, 0xb8, 0xb8, 0xb8, 0xb9, 0xb9, 0xb9, 0xba, 0xba, 0xba, 0xba, 0xba, 0xbb, 0xbb },
  { 0x80, 0x88, 0x90, 0x96, 0x9b, 0xa0, 0xa4, 0xa7, 0xa9, 0xab, 0xad, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3,
    0xb4, 0xb4, 0xb5, 0xb6, 0xb6, 0xb6, 0xb7, 0xb7, 0xb8, 0xb8, 0xb8, 0xb9, 0xb9, 0xb9, 0xb9, 0xb9 },
  { 0x80, 0x87, 0x8d, 0x93, 0x98, 0x9c, 0xa0, 0xa3, 0xa6, 0xa8, 0xaa, 0xac, 0xad, 0xae, 0xb0, 0xb0,
    0xb1, 0xb2, 0xb3, 0xb4, 0xb4, 0xb5, 0xb5, 0xb6, 0xb6, 0xb6, 0xb7, 0xb7, 0xb7, 0xb8, 0xb8, 0xb8 },
  { 0x80, 0x86, 0x8b, 0x90, 0x95, 0x99, 0x9d, 0xa0, 0xa3, 0xa5, 0xa7, 0xa9, 0xaa, 0xac, 0xad, 0xae,
    0xaf, 0xb0, 0xb1, 0xb2, 0xb2, 0xb3, 0xb3, 0xb4, 0xb4, 0xb5, 0xb5, 0xb6, 0xb6, 0xb6, 0xb7, 0xb7 },
  { 0x80, 0x85, 0x8a, 0x8f, 0x93, 0x97, 0x9a, 0x9d, 0xa0, 0xa2, 0xa5, 0xa6, 0xa8, 0xaa, 0xab, 0xac,
    0xad, 0xae, 0xaf, 0xb0, 0xb0, 0xb1, 0xb2, 0xb2, 0xb3, 0xb3, 0xb4, 0xb4, 0xb5, 0xb5, 0xb5, 0xb5 },
  { 0x80, 0x85, 0x89, 0x8d, 0x91, 0x95, 0x98, 0x9b, 0x9e, 0xa0, 0xa0, 0xa4, 0xa6, 0xa7, 0xa9, 0xaa,
    0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb0, 0xb1, 0xb1, 0xb2, 0xb2, 0xb3, 0xb3, 0xb4, 0xb4, 0xb4 },
  { 0x80, 0x84, 0x88, 0x8c, 0x90, 0x93, 0x96, 0x99, 0x9b, 0x9e, 0xa0, 0xa2, 0xa4, 0xa5, 0xa7, 0xa8,
    0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xaf, 0xb0, 0xb0, 0xb1, 0xb2, 0xb2, 0xb2, 0xb3, 0xb3 },
  { 0x80, 0x84, 0x87, 0x8b, 0x8e, 0x91, 0x94, 0x97, 0x9a, 0x9c, 0x9e, 0xa0, 0xa2, 0xa3, 0xa5, 0xa6,
    0xa7, 0xa9, 0xaa, 0xab, 0xac, 0xac, 0xad, 0xae, 0xae, 0xaf, 0xb0, 0xb0, 0xb1, 0xb1, 0xb2, 0xb2 },
  { 0x80, 0x83, 0x87, 0x8a, 0x8d, 0x90, 0x93, 0x96, 0x98, 0x9a, 0x9c, 0x9e, 0xa0, 0xa2, 0xa3, 0xa5,
    0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xac, 0xad, 0xae, 0xae, 0xaf, 0xb0, 0xb0, 0xb0, 0xb1 },
  { 0x80, 0x83, 0x86, 0x89, 0x8c, 0x8f, 0x92, 0x94, 0x96, 0x99, 0x9b, 0x9d, 0x9e, 0xa0, 0xa2, 0xa3,
    0xa4, 0xa5, 0xa7, 0xa8, 0xa9, 0xa9, 0xaa, 0xab, 0xac, 0xac, 0xad, 0xae, 0xae, 0xaf, 0xaf, 0xb0 },
  { 0x80, 0x83, 0x86, 0x89, 0x8b, 0x8e, 0x90, 0x93, 0x95, 0x97, 0x99, 0x9b, 0x9d, 0x9e, 0xa0, 0xa1,
    0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xaa, 0xab, 0xac, 0xad, 0xad, 0xae, 0xae, 0xaf },
  { 0x80, 0x83, 0x85, 0x88, 0x8b, 0x8d, 0x90, 0x92, 0x94, 0x96, 0x98, 0x9a, 0x9b, 0x9d, 0x9f, 0xa0,
    0xa1, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa8, 0xa9, 0xaa, 0xab, 0xab, 0xac, 0xad, 0xad, 0xae },
  { 0x80, 0x83, 0x85, 0x88, 0x8a, 0x8c, 0x8f, 0x91, 0x93, 0x95, 0x97, 0x99, 0x9a, 0x9c, 0x9d, 0x9f,
    0xa0, 0xa1, 0xa2, 0xa3, 0xa5, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xaa, 0xab, 0xab, 0xac, 0xad },
  { 0x80, 0x82, 0x85, 0x87, 0x89, 0x8c, 0x8e, 0x90, 0x92, 0x94, 0x96, 0x97, 0x99, 0x9b, 0x9c, 0x9d,
    0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa8, 0xa9, 0xaa, 0xaa, 0xab, 0xac },
  { 0x80, 0x82, 0x85, 0x87, 0x89, 0x8b, 0x8d, 0x8f, 0x91, 0x93, 0x95, 0x96, 0x98, 0x99, 0x9b, 0x9c,
    0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa7, 0xa8, 0xa9, 0xa9, 0xaa, 0xab },
  { 0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8c, 0x8e, 0x90, 0x92, 0x94, 0x95, 0x97, 0x98, 0x9a, 0x9b,
    0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa6, 0xa7, 0xa8, 0xa8, 0xa9, 0xaa },
  { 0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8c, 0x8e, 0x90, 0x91, 0x93, 0x94, 0x96, 0x97, 0x99, 0x9a,
    0x9b, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa5, 0xa6, 0xa7, 0xa7, 0xa8, 0xa9 },
  { 0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8b, 0x8d, 0x8f, 0x90, 0x92, 0x94, 0x95, 0x97, 0x98, 0x99,
    0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa4, 0xa5, 0xa6, 0xa6, 0xa7, 0xa8 },
  { 0x80, 0x82, 0x84, 0x86, 0x87, 0x89, 0x8b, 0x8d, 0x8e, 0x90, 0x91, 0x93, 0x94, 0x96, 0x97, 0x98,
    0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa3, 0xa3, 0xa4, 0xa5, 0xa6, 0xa6, 0xa7 },
  { 0x80, 0x82, 0x84, 0x85, 0x87, 0x89, 0x8a, 0x8c, 0x8e, 0x8f, 0x91, 0x92, 0x94, 0x95, 0x96, 0x98,
    0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa2, 0xa3, 0xa4, 0xa5, 0xa5, 0xa6 },
  { 0x80, 0x82, 0x83, 0x85, 0x87, 0x88, 0x8a, 0x8c, 0x8d, 0x8f, 0x90, 0x92, 0x93, 0x94, 0x96, 0x97,
    0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa2, 0xa3, 0xa4, 0xa5, 0xa5 },
  { 0x80, 0x82, 0x83, 0x85, 0x86, 0x88, 0x8a, 0x8b, 0x8d, 0x8e, 0x90, 0x91, 0x92, 0x94, 0x95, 0x96,
    0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa2, 0xa3, 0xa4, 0xa4 },
  { 0x80, 0x82, 0x83, 0x85, 0x86, 0x88, 0x89, 0x8b, 0x8c, 0x8e, 0x8f, 0x90, 0x92, 0x93, 0x94, 0x95,
    0x96, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9e, 0x9f, 0xa0, 0xa1, 0xa2, 0xa2, 0xa3, 0xa4 },
  { 0x80, 0x82, 0x83, 0x85, 0x86, 0x87, 0x89, 0x8a, 0x8c, 0x8d, 0x8e, 0x90, 0x91, 0x92, 0x93, 0x95,
    0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9e, 0x9f, 0xa0, 0xa1, 0xa1, 0xa2, 0xa3 },
  { 0x80, 0x81, 0x83, 0x84, 0x86, 0x87, 0x89, 0x8a, 0x8b, 0x8d, 0x8e, 0x8f, 0x90, 0x92, 0x93, 0x94,
    0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9e, 0x9f, 0xa0, 0xa1, 0xa1, 0xa2 },
  { 0x80, 0x81, 0x83, 0x84, 0x86, 0x87, 0x88, 0x8a, 0x8b, 0x8c, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93,
    0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0x9f, 0xa0, 0xa1, 0xa1 },
  { 0x80, 0x81, 0x83, 0x84, 0x85, 0x87, 0x88, 0x89, 0x8b, 0x8c, 0x8d, 0x8e, 0x90, 0x91, 0x92, 0x93,
    0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0x9f, 0xa0, 0xa1 },
  { 0x80, 0x81, 0x83, 0x84, 0x85, 0x87, 0x88, 0x89, 0x8a, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92,
    0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9c, 0x9d, 0x9e, 0x9f, 0x9f, 0xa0 }
};

int16_t ST0010::sin(int16_t theta) {
  return sin_table[(theta >> 8) & 0xff];
}

int16_t ST0010::cos(int16_t theta) {
  return sin_table[((theta + 0x4000) >> 8) & 0xff];
}

uint8_t ST0010::readb(uint16_t addr) {
  return ram[addr & 0xfff];
}

uint16_t ST0010::readw(uint16_t addr) {
  return (readb(addr + 0) <<  0) |
         (readb(addr + 1) <<  8);
}

uint32_t ST0010::readd(uint16_t addr) {
  return (readb(addr + 0) <<  0) |
         (readb(addr + 1) <<  8) |
         (readb(addr + 2) << 16) |
         (readb(addr + 3) << 24);
}

void ST0010::writeb(uint16_t addr, uint8_t data) {
  ram[addr & 0xfff] = data;
}

void ST0010::writew(uint16_t addr, uint16_t data) {
  writeb(addr + 0, data >> 0);
  writeb(addr + 1, data >> 8);
}

void ST0010::writed(uint16_t addr, uint32_t data) {
  writeb(addr + 0, data >>  0);
  writeb(addr + 1, data >>  8);
  writeb(addr + 2, data >> 16);
  writeb(addr + 3, data >> 24);
}

//ST-0010 emulation code - Copyright (C) 2003 The Dumper, Matthew Kendora, Overload, Feather
//bsnes port - Copyright (C) 2007 byuu

void ST0010::op_01(int16_t x0, int16_t y0, int16_t &x1, int16_t &y1, int16_t &quadrant, int16_t &theta) {
  if((x0 < 0) && (y0 < 0)) {
    x1 = -x0;
    y1 = -y0;
    quadrant = -0x8000;
  } else if(x0 < 0) {
    x1 = y0;
    y1 = -x0;
    quadrant = -0x4000;
  } else if(y0 < 0) {
    x1 = -y0;
    y1 = x0;
    quadrant = 0x4000;
  } else {
    x1 = x0;
    y1 = y0;
    quadrant = 0x0000;
  }

  while((x1 > 0x1f) || (y1 > 0x1f)) {
    if(x1 > 1) { x1 >>= 1; }
    if(y1 > 1) { y1 >>= 1; }
  }

  if(y1 == 0) { quadrant += 0x4000; }

  theta = (arctan[y1][x1] << 8) ^ quadrant;
}

//

void ST0010::op_01() {
  int16_t x0 = readw(0x0000);
  int16_t y0 = readw(0x0002);
  int16_t x1, y1, quadrant, theta;

  op_01(x0, y0, x1, y1, quadrant, theta);

  writew(0x0000, x1);
  writew(0x0002, y1);
  writew(0x0004, quadrant);
//writew(0x0006, y0);  //Overload's docs note this write occurs, SNES9x disagrees
  writew(0x0010, theta);
}

void ST0010::op_02() {
  int16_t positions = readw(0x0024);
  uint16_t *places  = (uint16_t*)(ram + 0x0040);
  uint16_t *drivers = (uint16_t*)(ram + 0x0080);

  bool sorted;
  uint16_t temp;
  if(positions > 1) {
    do {
      sorted = true;
      for(int i = 0; i < positions - 1; i++) {
        if(places[i] < places[i + 1]) {
          temp = places[i + 1];
          places[i + 1] = places[i];
          places[i] = temp;

          temp = drivers[i + 1];
          drivers[i + 1] = drivers[i];
          drivers[i] = temp;

          sorted = false;
        }
      }
      positions--;
    } while(!sorted);
  }
}

void ST0010::op_03() {
  int16_t x0 = readw(0x0000);
  int16_t y0 = readw(0x0002);
  int16_t multiplier = readw(0x0004);
  int32_t x1, y1;

  x1 = x0 * multiplier << 1;
  y1 = y0 * multiplier << 1;

  writed(0x0010, x1);
  writed(0x0014, y1);
}

void ST0010::op_04() {
  int16_t x = readw(0x0000);
  int16_t y = readw(0x0002);
  int16_t square;
  //calculate the vector length of (x,y)
  square = (int16_t)sqrt((double)(y * y + x * x));

  writew(0x0010, square);
}

void ST0010::op_05() {
  int32_t dx, dy;
  int16_t a1, b1, c1;
  uint16_t o1;
  bool wrap = false;

  //target (x,y) coordinates
  int16_t ypos_max = readw(0x00c0);
  int16_t xpos_max = readw(0x00c2);

  //current coordinates and direction
  int32_t ypos = readd(0x00c4);
  int32_t xpos = readd(0x00c8);
  uint16_t rot = readw(0x00cc);

  //physics
  uint16_t speed = readw(0x00d4);
  uint16_t accel = readw(0x00d6);
  uint16_t speed_max = readw(0x00d8);

  //special condition acknowledgement
  int16_t system = readw(0x00da);
  int16_t flags = readw(0x00dc);

  //new target coordinates
  int16_t ypos_new = readw(0x00de);
  int16_t xpos_new = readw(0x00e0);

  //mask upper bit
  xpos_new &= 0x7fff;

  //get the current distance
  dx = xpos_max - (xpos >> 16);
  dy = ypos_max - (ypos >> 16);

  //quirk: clear and move in9
  writew(0x00d2, 0xffff);
  writew(0x00da, 0x0000);

  //grab the target angle
  op_01(dy, dx, a1, b1, c1, (int16_t&)o1);

  //check for wrapping
  if(abs(o1 - rot) > 0x8000) {
    o1 += 0x8000;
    rot += 0x8000;
    wrap = true;
  }

  uint16_t old_speed = speed;

  //special case
  if(abs(o1 - rot) == 0x8000) {
    speed = 0x100;
  }

  //slow down for sharp curves
  else if(abs(o1 - rot) >= 0x1000) {
  uint32_t slow = abs(o1 - rot);
    slow >>= 4;  //scaling
    speed -= slow;
  }

  //otherwise accelerate
  else {
    speed += accel;
    if(speed > speed_max) {
      speed = speed_max;  //clip speed
    }
  }

  //prevent negative/positive overflow
  if(abs(old_speed - speed) > 0x8000) {
    if(old_speed < speed) { speed = 0; }
    else speed = 0xff00;
  }

  //adjust direction by so many degrees
  //be careful of negative adjustments
  if((o1 > rot && (o1 - rot) > 0x80) || (o1 < rot && (rot - o1) >= 0x80)) {
    if(o1 < rot) { rot -= 0x280; }
    else if(o1 > rot) { rot += 0x280; }
  }

  //turn off wrapping
  if(wrap) { rot -= 0x8000; }

  //now check the distances (store for later)
  dx = (xpos_max << 16) - xpos;
  dy = (ypos_max << 16) - ypos;
  dx >>= 16;
  dy >>= 16;

  //if we're in so many units of the target, signal it
  if((system && (dy <= 6 && dy >= -8) && (dx <= 126 && dx >= -128)) || (!system && (dx <= 6 && dx >= -8) && (dy <= 126 && dy >= -128))) {
    //announce our new destination and flag it
    xpos_max = xpos_new & 0x7fff;
    ypos_max = ypos_new;
    flags |= 0x08;
  }

  //update position
  xpos -= (cos(rot) * 0x400 >> 15) * (speed >> 8) << 1;
  ypos -= (sin(rot) * 0x400 >> 15) * (speed >> 8) << 1;

  //quirk: mask upper byte
  xpos &= 0x1fffffff;
  ypos &= 0x1fffffff;

  writew(0x00c0, ypos_max);
  writew(0x00c2, xpos_max);
  writed(0x00c4, ypos);
  writed(0x00c8, xpos);
  writew(0x00cc, rot);
  writew(0x00d4, speed);
  writew(0x00dc, flags);
}

void ST0010::op_06() {
  int16_t multiplicand = readw(0x0000);
  int16_t multiplier = readw(0x0002);
  int32_t product;

  product = multiplicand * multiplier << 1;

  writed(0x0010, product);
}

void ST0010::op_07() {
  int16_t theta = readw(0x0000);

  int16_t data;
  for(int i = 0, offset = 0; i < 176; i++) {
    data = mode7_scale[i] * cos(theta) >> 15;
    writew(0x00f0 + offset, data);
    writew(0x0510 + offset, data);

    data = mode7_scale[i] * sin(theta) >> 15;
    writew(0x0250 + offset, data);
    if(data) { data = ~data; }
    writew(0x03b0 + offset, data);

    offset += 2;
  }
}

void ST0010::op_08() {
  int16_t x0 = readw(0x0000);
  int16_t y0 = readw(0x0002);
  int16_t theta = readw(0x0004);
  int16_t x1, y1;

  x1 = (y0 * sin(theta) >> 15) + (x0 * cos(theta) >> 15);
  y1 = (y0 * cos(theta) >> 15) - (x0 * sin(theta) >> 15);

  writew(0x0010, x1);
  writew(0x0012, y1);
}

ST0010 st0010;

auto ST0010::serialize(serializer& s) -> void {
  s.array(ram);
}

auto ST0010::power() -> void {
  memset(ram, 0x00, sizeof ram);
}

auto ST0010::read(unsigned addr, uint8_t data) -> uint8_t {
  return readb(addr);
}

auto ST0010::write(unsigned addr, uint8_t data) -> void {
  writeb(addr, data);

  if((addr & 0xfff) == 0x0021 && (data & 0x80)) {
    switch(ram[0x0020]) {
      case 0x01: op_01(); break;
      case 0x02: op_02(); break;
      case 0x03: op_03(); break;
      case 0x04: op_04(); break;
      case 0x05: op_05(); break;
      case 0x06: op_06(); break;
      case 0x07: op_07(); break;
      case 0x08: op_08(); break;
    }

    ram[0x0021] &= ~0x80;
  }
}

}
