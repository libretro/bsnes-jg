#include <sfc/sfc.hpp>

namespace SuperFamicom {

//SPC7110 decompressor
//original implementation: neviksti
//optimized implementation: talarubi

struct Decompressor {
  SPC7110& spc7110;

  Decompressor(SPC7110& spc7110) : spc7110(spc7110) {}

  auto read() -> uint8 {
    return spc7110.dataromRead(offset++);
  }

  //inverse morton code transform: unpack big-endian packed pixels
  //returns odd bits in lower half; even bits in upper half
  auto deinterleave(uint64 data, uint bits) -> uint32 {
    data = data & (1ull << bits) - 1;
    data = 0x5555555555555555ull & (data << bits | data >> 1);
    data = 0x3333333333333333ull & (data | data >> 1);
    data = 0x0f0f0f0f0f0f0f0full & (data | data >> 2);
    data = 0x00ff00ff00ff00ffull & (data | data >> 4);
    data = 0x0000ffff0000ffffull & (data | data >> 8);
    return data | data >> 16;
  }

  //extract a nibble and move it to the low four bits
  auto moveToFront(uint64 list, uint nibble) -> uint64 {
    for(uint64 n = 0, mask = ~15; n < 64; n += 4, mask <<= 4) {
      if((list >> n & 15) != nibble) continue;
      return list = (list & mask) + (list << 4 & ~mask) + nibble;
    }
    return list;
  }

  auto initialize(uint mode, uint origin) -> void {
    for(auto& root : context) for(auto& node : root) node = {0, 0};
    bpp = 1 << mode;
    offset = origin;
    bits = 8;
    range = Max + 1;
    input = read();
    input = input << 8 | read();
    output = 0;
    pixels = 0;
    colormap = 0xfedcba9876543210ull;
  }

  auto decode() -> void {
    for(uint pixel = 0; pixel < 8; pixel++) {
      uint64 map = colormap;
      uint diff = 0;

      if(bpp > 1) {
        uint pa = (bpp == 2 ? pixels >>  2 & 3 : pixels >>  0 & 15);
        uint pb = (bpp == 2 ? pixels >> 14 & 3 : pixels >> 28 & 15);
        uint pc = (bpp == 2 ? pixels >> 16 & 3 : pixels >> 32 & 15);

        if(pa != pb || pb != pc) {
          uint match = pa ^ pb ^ pc;
          diff = 4;                        //no match; all pixels differ
          if((match ^ pc) == 0) diff = 3;  //a == b; pixel c differs
          if((match ^ pb) == 0) diff = 2;  //c == a; pixel b differs
          if((match ^ pa) == 0) diff = 1;  //b == c; pixel a differs
        }

        colormap = moveToFront(colormap, pa);

        map = moveToFront(map, pc);
        map = moveToFront(map, pb);
        map = moveToFront(map, pa);
      }

      for(uint plane = 0; plane < bpp; plane++) {
        uint bit = bpp > 1 ? 1 << plane : 1 << (pixel & 3);
        uint history = bit - 1 & output;
        uint set = 0;

        if(bpp == 1) set = pixel >= 4;
        if(bpp == 2) set = diff;
        if(plane >= 2 && history <= 1) set = diff;

        auto& ctx = context[set][bit + history - 1];
        auto& model = evolution[ctx.prediction];
        uint8 lps_offset = range - model.probability;
        bool symbol = input >= (lps_offset << 8);  //test only the MSB

        output = output << 1 | (symbol ^ ctx.swap);

        if(symbol == MPS) {          //[0 ... range-p]
          range = lps_offset;        //range = range-p
        } else {                     //[range-p+1 ... range]
          range -= lps_offset;       //range = p-1, with p < 0.75
          input -= lps_offset << 8;  //therefore, always rescale
        }

        while(range <= Max / 2) {    //scale back into [0.75 ... 1.5]
          ctx.prediction = model.next[symbol];

          range <<= 1;
          input <<= 1;

          if(--bits == 0) {
            bits = 8;
            input += read();
          }
        }

        if(symbol == LPS && model.probability > Half) ctx.swap ^= 1;
      }

      uint index = output & (1 << bpp) - 1;
      if(bpp == 1) index ^= pixels >> 15 & 1;

      pixels = pixels << bpp | (map >> 4 * index & 15);
    }

    if(bpp == 1) result = pixels;
    if(bpp == 2) result = deinterleave(pixels, 16);
    if(bpp == 4) result = deinterleave(deinterleave(pixels, 32), 32);
  }

  auto serialize(serializer& s) -> void {
    for(auto& root : context) {
      for(auto& node : root) {
        s.integer(node.prediction);
        s.integer(node.swap);
      }
    }

    s.integer(bpp);
    s.integer(offset);
    s.integer(bits);
    s.integer(range);
    s.integer(input);
    s.integer(output);
    s.integer(pixels);
    s.integer(colormap);
    s.integer(result);
  }

  enum : uint { MPS = 0, LPS = 1 };
  enum : uint { One = 0xaa, Half = 0x55, Max = 0xff };

  struct ModelState {
    uint8 probability;  //of the more probable symbol (MPS)
    uint8 next[2];      //next state after output {MPS, LPS}
  };
  static ModelState evolution[53];

  struct Context {
    uint8 prediction;   //current model state
    uint8 swap;         //if 1, exchange the role of MPS and LPS
  } context[5][15];     //not all 75 contexts exists; this simplifies the code

  uint bpp;             //bits per pixel (1bpp = 1; 2bpp = 2; 4bpp = 4)
  uint offset;          //SPC7110 data ROM read offset
  uint bits;            //bits remaining in input
  uint16 range;         //arithmetic range: technically 8-bits, but Max+1 = 256
  uint16 input;         //input data from SPC7110 data ROM
  uint8 output;
  uint64 pixels;
  uint64 colormap;      //most recently used list
  uint32 result;        //decompressed word after calling decode()
};

Decompressor::ModelState Decompressor::evolution[53] = {
  {0x5a, { 1, 1}}, {0x25, { 2, 6}}, {0x11, { 3, 8}},
  {0x08, { 4,10}}, {0x03, { 5,12}}, {0x01, { 5,15}},

  {0x5a, { 7, 7}}, {0x3f, { 8,19}}, {0x2c, { 9,21}},
  {0x20, {10,22}}, {0x17, {11,23}}, {0x11, {12,25}},
  {0x0c, {13,26}}, {0x09, {14,28}}, {0x07, {15,29}},
  {0x05, {16,31}}, {0x04, {17,32}}, {0x03, {18,34}},
  {0x02, { 5,35}},

  {0x5a, {20,20}}, {0x48, {21,39}}, {0x3a, {22,40}},
  {0x2e, {23,42}}, {0x26, {24,44}}, {0x1f, {25,45}},
  {0x19, {26,46}}, {0x15, {27,25}}, {0x11, {28,26}},
  {0x0e, {29,26}}, {0x0b, {30,27}}, {0x09, {31,28}},
  {0x08, {32,29}}, {0x07, {33,30}}, {0x05, {34,31}},
  {0x04, {35,33}}, {0x04, {36,33}}, {0x03, {37,34}},
  {0x02, {38,35}}, {0x02, { 5,36}},

  {0x58, {40,39}}, {0x4d, {41,47}}, {0x43, {42,48}},
  {0x3b, {43,49}}, {0x34, {44,50}}, {0x2e, {45,51}},
  {0x29, {46,44}}, {0x25, {24,45}},

  {0x56, {48,47}}, {0x4f, {49,47}}, {0x47, {50,48}},
  {0x41, {51,49}}, {0x3c, {52,50}}, {0x37, {43,51}},
};

auto SPC7110::dcuLoadAddress() -> void {
  uint table = r4801 | r4802 << 8 | r4803 << 16;
  uint index = r4804 << 2;

  uint address = table + index;
  dcuMode     = dataromRead(address + 0);
  dcuAddress  = dataromRead(address + 1) << 16;
  dcuAddress |= dataromRead(address + 2) <<  8;
  dcuAddress |= dataromRead(address + 3) <<  0;
}

auto SPC7110::dcuBeginTransfer() -> void {
  if(dcuMode == 3) return;  //invalid mode

  addClocks(20);
  decompressor->initialize(dcuMode, dcuAddress);
  decompressor->decode();

  uint seek = r480b & 2 ? r4805 | r4806 << 8 : 0;
  while(seek--) decompressor->decode();

  r480c |= 0x80;
  dcuOffset = 0;
}

auto SPC7110::dcuRead() -> uint8 {
  if((r480c & 0x80) == 0) return 0x00;

  if(dcuOffset == 0) {
    for(auto row : range(8)) {
      switch(decompressor->bpp) {
      case 1:
        dcuTile[row] = decompressor->result;
        break;
      case 2:
        dcuTile[row * 2 + 0] = decompressor->result >> 0;
        dcuTile[row * 2 + 1] = decompressor->result >> 8;
        break;
      case 4:
        dcuTile[row * 2 +  0] = decompressor->result >>  0;
        dcuTile[row * 2 +  1] = decompressor->result >>  8;
        dcuTile[row * 2 + 16] = decompressor->result >> 16;
        dcuTile[row * 2 + 17] = decompressor->result >> 24;
        break;
      }

      uint seek = r480b & 1 ? r4807 : (uint8)1;
      while(seek--) decompressor->decode();
    }
  }

  uint8 data = dcuTile[dcuOffset++];
  dcuOffset &= 8 * decompressor->bpp - 1;
  return data;
}

auto SPC7110::dataromRead(uint addr) -> uint8 {
  uint size = 1 << (r4834 & 3);  //size in MB
  uint mask = 0x100000 * size - 1;
  uint offset = addr & mask;
  if((r4834 & 3) != 3 && (addr & 0x400000)) return 0x00;
  return drom.read(Bus::mirror(offset, drom.size()));
}

auto SPC7110::dataOffset() -> uint { return r4811 | r4812 << 8 | r4813 << 16; }
auto SPC7110::dataAdjust() -> uint { return r4814 | r4815 << 8; }
auto SPC7110::dataStride() -> uint { return r4816 | r4817 << 8; }
auto SPC7110::setDataOffset(uint addr) -> void { r4811 = addr; r4812 = addr >> 8; r4813 = addr >> 16; }
auto SPC7110::setDataAdjust(uint addr) -> void { r4814 = addr; r4815 = addr >> 8; }

auto SPC7110::dataPortRead() -> void {
  uint offset = dataOffset();
  uint adjust = r4818 & 2 ? dataAdjust() : 0;
  if(r4818 & 8) adjust = (int16)adjust;
  r4810 = dataromRead(offset + adjust);
}

auto SPC7110::dataPortIncrement4810() -> void {
  uint offset = dataOffset();
  uint stride = r4818 & 1 ? dataStride() : 1;
  uint adjust = dataAdjust();
  if(r4818 & 4) stride = (int16)stride;
  if(r4818 & 8) adjust = (int16)adjust;
  if((r4818 & 16) == 0) setDataOffset(offset + stride);
  if((r4818 & 16) != 0) setDataAdjust(adjust + stride);
  dataPortRead();
}

auto SPC7110::dataPortIncrement4814() -> void {
  if(r4818 >> 5 != 1) return;
  uint offset = dataOffset();
  uint adjust = dataAdjust();
  if(r4818 & 8) adjust = (int16)adjust;
  setDataOffset(offset + adjust);
  dataPortRead();
}

auto SPC7110::dataPortIncrement4815() -> void {
  if(r4818 >> 5 != 2) return;
  uint offset = dataOffset();
  uint adjust = dataAdjust();
  if(r4818 & 8) adjust = (int16)adjust;
  setDataOffset(offset + adjust);
  dataPortRead();
}

auto SPC7110::dataPortIncrement481a() -> void {
  if(r4818 >> 5 != 3) return;
  uint offset = dataOffset();
  uint adjust = dataAdjust();
  if(r4818 & 8) adjust = (int16)adjust;
  setDataOffset(offset + adjust);
  dataPortRead();
}

auto SPC7110::aluMultiply() -> void {
  addClocks(30);

  if(r482e & 1) {
    //signed 16-bit x 16-bit multiplication
    int16 r0 = (int16)(r4824 | r4825 << 8);
    int16 r1 = (int16)(r4820 | r4821 << 8);

    int result = r0 * r1;
    r4828 = result;
    r4829 = result >> 8;
    r482a = result >> 16;
    r482b = result >> 24;
  } else {
    //unsigned 16-bit x 16-bit multiplication
    uint16 r0 = (uint16)(r4824 | r4825 << 8);
    uint16 r1 = (uint16)(r4820 | r4821 << 8);

    uint result = r0 * r1;
    r4828 = result;
    r4829 = result >> 8;
    r482a = result >> 16;
    r482b = result >> 24;
  }

  r482f &= 0x7f;
}

auto SPC7110::aluDivide() -> void {
  addClocks(40);

  if(r482e & 1) {
    //signed 32-bit x 16-bit division
    int32 dividend = (int32)(r4820 | r4821 << 8 | r4822 << 16 | r4823 << 24);
    int16 divisor  = (int16)(r4826 | r4827 << 8);

    int32 quotient;
    int16 remainder;

    if(divisor) {
      quotient  = (int32)(dividend / divisor);
      remainder = (int32)(dividend % divisor);
    } else {
      //illegal division by zero
      quotient  = 0;
      remainder = dividend;
    }

    r4828 = quotient;
    r4829 = quotient >> 8;
    r482a = quotient >> 16;
    r482b = quotient >> 24;

    r482c = remainder;
    r482d = remainder >> 8;
  } else {
    //unsigned 32-bit x 16-bit division
    uint32 dividend = (uint32)(r4820 | r4821 << 8 | r4822 << 16 | r4823 << 24);
    uint16 divisor  = (uint16)(r4826 | r4827 << 8);

    uint32 quotient;
    uint16 remainder;

    if(divisor) {
      quotient  = (uint32)(dividend / divisor);
      remainder = (uint16)(dividend % divisor);
    } else {
      //illegal division by zero
      quotient  = 0;
      remainder = dividend;
    }

    r4828 = quotient;
    r4829 = quotient >> 8;
    r482a = quotient >> 16;
    r482b = quotient >> 24;

    r482c = remainder;
    r482d = remainder >> 8;
  }

  r482f &= 0x7f;
}

auto SPC7110::serialize(serializer& s) -> void {
  Thread::serialize(s);
  s.array(ram.data(), ram.size());

  s.integer(r4801);
  s.integer(r4802);
  s.integer(r4803);
  s.integer(r4804);
  s.integer(r4805);
  s.integer(r4806);
  s.integer(r4807);
  s.integer(r4809);
  s.integer(r480a);
  s.integer(r480b);
  s.integer(r480c);

  s.integer(dcuPending);
  s.integer(dcuMode);
  s.integer(dcuAddress);
  s.integer(dcuOffset);
  s.array(dcuTile);
  decompressor->serialize(s);

  s.integer(r4810);
  s.integer(r4811);
  s.integer(r4812);
  s.integer(r4813);
  s.integer(r4814);
  s.integer(r4815);
  s.integer(r4816);
  s.integer(r4817);
  s.integer(r4818);
  s.integer(r481a);

  s.integer(r4820);
  s.integer(r4821);
  s.integer(r4822);
  s.integer(r4823);
  s.integer(r4824);
  s.integer(r4825);
  s.integer(r4826);
  s.integer(r4827);
  s.integer(r4828);
  s.integer(r4829);
  s.integer(r482a);
  s.integer(r482b);
  s.integer(r482c);
  s.integer(r482d);
  s.integer(r482e);
  s.integer(r482f);

  s.integer(mulPending);
  s.integer(divPending);

  s.integer(r4830);
  s.integer(r4831);
  s.integer(r4832);
  s.integer(r4833);
  s.integer(r4834);
}

SPC7110 spc7110;

SPC7110::SPC7110() {
  decompressor = new Decompressor(*this);
}

SPC7110::~SPC7110() {
  delete decompressor;
}

auto SPC7110::synchronizeCPU() -> void {
  if(clock >= 0) scheduler.resume(cpu.thread);
}

auto SPC7110::Enter() -> void {
  while(true) {
    scheduler.synchronize();
    spc7110.main();
  }
}

auto SPC7110::main() -> void {
  if(dcuPending) { dcuPending = 0; dcuBeginTransfer(); }
  if(mulPending) { mulPending = 0; aluMultiply(); }
  if(divPending) { divPending = 0; aluDivide(); }
  addClocks(1);
}

auto SPC7110::step(uint clocks) -> void {
  clock += clocks * (uint64_t)cpu.frequency;
}

auto SPC7110::addClocks(uint clocks) -> void {
  step(clocks);
  synchronizeCPU();
}

auto SPC7110::unload() -> void {
  prom.reset();
  drom.reset();
  ram.reset();
}

auto SPC7110::power() -> void {
  create(SPC7110::Enter, 21'477'272);

  r4801 = 0x00;
  r4802 = 0x00;
  r4803 = 0x00;
  r4804 = 0x00;
  r4805 = 0x00;
  r4806 = 0x00;
  r4807 = 0x00;
  r4809 = 0x00;
  r480a = 0x00;
  r480b = 0x00;
  r480c = 0x00;

  dcuPending = 0;
  dcuMode = 0;
  dcuAddress = 0;

  r4810 = 0x00;
  r4811 = 0x00;
  r4812 = 0x00;
  r4813 = 0x00;
  r4814 = 0x00;
  r4815 = 0x00;
  r4816 = 0x00;
  r4817 = 0x00;
  r4818 = 0x00;
  r481a = 0x00;

  r4820 = 0x00;
  r4821 = 0x00;
  r4822 = 0x00;
  r4823 = 0x00;
  r4824 = 0x00;
  r4825 = 0x00;
  r4826 = 0x00;
  r4827 = 0x00;
  r4828 = 0x00;
  r4829 = 0x00;
  r482a = 0x00;
  r482b = 0x00;
  r482c = 0x00;
  r482d = 0x00;
  r482e = 0x00;
  r482f = 0x00;

  mulPending = 0;
  divPending = 0;

  r4830 = 0x00;
  r4831 = 0x00;
  r4832 = 0x01;
  r4833 = 0x02;
  r4834 = 0x00;
}

auto SPC7110::read(uint addr, uint8 data) -> uint8 {
  cpu.synchronizeCoprocessors();
  if((addr & 0xff0000) == 0x500000) addr = 0x4800;  //$50:0000-ffff == $4800
  if((addr & 0xff0000) == 0x580000) addr = 0x4808;  //$58:0000-ffff == $4808
  addr = 0x4800 | (addr & 0x3f);  //$00-3f,80-bf:4800-483f

  switch(addr) {
  //==================
  //decompression unit
  //==================
  case 0x4800: {
    uint16 counter = r4809 | r480a << 8;
    counter--;
    r4809 = counter >> 0;
    r480a = counter >> 8;
    return dcuRead();
  }
  case 0x4801: return r4801;
  case 0x4802: return r4802;
  case 0x4803: return r4803;
  case 0x4804: return r4804;
  case 0x4805: return r4805;
  case 0x4806: return r4806;
  case 0x4807: return r4807;
  case 0x4808: return 0x00;
  case 0x4809: return r4809;
  case 0x480a: return r480a;
  case 0x480b: return r480b;
  case 0x480c: return r480c;

  //==============
  //data port unit
  //==============
  case 0x4810: {
    data = r4810;
    dataPortIncrement4810();
    return data;
  }
  case 0x4811: return r4811;
  case 0x4812: return r4812;
  case 0x4813: return r4813;
  case 0x4814: return r4814;
  case 0x4815: return r4815;
  case 0x4816: return r4816;
  case 0x4817: return r4817;
  case 0x4818: return r4818;
  case 0x481a: {
    dataPortIncrement481a();
    return 0x00;
  }

  //=====================
  //arithmetic logic unit
  //=====================
  case 0x4820: return r4820;
  case 0x4821: return r4821;
  case 0x4822: return r4822;
  case 0x4823: return r4823;
  case 0x4824: return r4824;
  case 0x4825: return r4825;
  case 0x4826: return r4826;
  case 0x4827: return r4827;
  case 0x4828: return r4828;
  case 0x4829: return r4829;
  case 0x482a: return r482a;
  case 0x482b: return r482b;
  case 0x482c: return r482c;
  case 0x482d: return r482d;
  case 0x482e: return r482e;
  case 0x482f: return r482f;

  //===================
  //memory control unit
  //===================
  case 0x4830: return r4830;
  case 0x4831: return r4831;
  case 0x4832: return r4832;
  case 0x4833: return r4833;
  case 0x4834: return r4834;
  }

  return data;
}

auto SPC7110::write(uint addr, uint8 data) -> void {
  cpu.synchronizeCoprocessors();
  if((addr & 0xff0000) == 0x500000) addr = 0x4800;  //$50:0000-ffff == $4800
  if((addr & 0xff0000) == 0x580000) addr = 0x4808;  //$58:0000-ffff == $4808
  addr = 0x4800 | (addr & 0x3f);  //$00-3f,80-bf:4800-483f

  switch(addr) {
  //==================
  //decompression unit
  //==================
  case 0x4801: r4801 = data; break;
  case 0x4802: r4802 = data; break;
  case 0x4803: r4803 = data; break;
  case 0x4804: r4804 = data; dcuLoadAddress(); break;
  case 0x4805: r4805 = data; break;
  case 0x4806: r4806 = data; r480c &= 0x7f; dcuPending = 1; break;
  case 0x4807: r4807 = data; break;
  case 0x4808: break;
  case 0x4809: r4809 = data; break;
  case 0x480a: r480a = data; break;
  case 0x480b: r480b = data & 0x03; break;

  //==============
  //data port unit
  //==============
  case 0x4811: r4811 = data; break;
  case 0x4812: r4812 = data; break;
  case 0x4813: r4813 = data; dataPortRead(); break;
  case 0x4814: r4814 = data; dataPortIncrement4814(); break;
  case 0x4815: r4815 = data; if(r4818 & 2) dataPortRead(); dataPortIncrement4815(); break;
  case 0x4816: r4816 = data; break;
  case 0x4817: r4817 = data; break;
  case 0x4818: r4818 = data & 0x7f; dataPortRead(); break;

  //=====================
  //arithmetic logic unit
  //=====================
  case 0x4820: r4820 = data; break;
  case 0x4821: r4821 = data; break;
  case 0x4822: r4822 = data; break;
  case 0x4823: r4823 = data; break;
  case 0x4824: r4824 = data; break;
  case 0x4825: r4825 = data; r482f |= 0x81; mulPending = 1; break;
  case 0x4826: r4826 = data; break;
  case 0x4827: r4827 = data; r482f |= 0x80; divPending = 1; break;
  case 0x482e: r482e = data & 0x01; break;

  //===================
  //memory control unit
  //===================
  case 0x4830: r4830 = data & 0x87; break;
  case 0x4831: r4831 = data & 0x07; break;
  case 0x4832: r4832 = data & 0x07; break;
  case 0x4833: r4833 = data & 0x07; break;
  case 0x4834: r4834 = data & 0x07; break;
  }
}

//===============
//SPC7110::MCUROM
//===============

//map address=00-3f,80-bf:8000-ffff mask=0x800000 => 00-3f:8000-ffff
//map address=c0-ff:0000-ffff mask=0xc00000 => c0-ff:0000-ffff
auto SPC7110::mcuromRead(uint addr, uint8 data) -> uint8 {
  uint mask = (1 << (r4834 & 3)) - 1;  //8mbit, 16mbit, 32mbit, 64mbit DROM

  if(addr < 0x100000) {  //$00-0f,80-8f:8000-ffff; $c0-cf:0000-ffff
    addr &= 0x0fffff;
    if(prom.size()) {  //8mbit PROM
      return prom.read(bus.mirror(0x000000 + addr, prom.size()));
    }
    addr |= 0x100000 * (r4830 & 7);
    return dataromRead(addr);
  }

  if(addr < 0x200000) {  //$10-1f,90-9f:8000-ffff; $d0-df:0000-ffff
    addr &= 0x0fffff;
    if(r4834 & 4) {  //16mbit PROM
      return prom.read(bus.mirror(0x100000 + addr, prom.size()));
    }
    addr |= 0x100000 * (r4831 & 7);
    return dataromRead(addr);
  }

  if(addr < 0x300000) {  //$20-2f,a0-af:8000-ffff; $e0-ef:0000-ffff
    addr &= 0x0fffff;
    addr |= 0x100000 * (r4832 & 7);
    return dataromRead(addr);
  }

  if(addr < 0x400000) {  //$30-3f,b0-bf:8000-ffff; $f0-ff:0000-ffff
    addr &= 0x0fffff;
    addr |= 0x100000 * (r4833 & 7);
    return dataromRead(addr);
  }

  return data;
}

auto SPC7110::mcuromWrite(uint addr, uint8 data) -> void {
}

//===============
//SPC7110::MCURAM
//===============

//map address=00-3f,80-bf:6000-7fff mask=0x80e000 => 00-07:0000-ffff
auto SPC7110::mcuramRead(uint addr, uint8) -> uint8 {
  if(r4830 & 0x80) {
    addr = bus.mirror(addr, ram.size());
    return ram.read(addr);
  }
  return 0x00;
}

auto SPC7110::mcuramWrite(uint addr, uint8 data) -> void {
  if(r4830 & 0x80) {
    addr = bus.mirror(addr, ram.size());
    ram.write(addr, data);
  }
}

}
