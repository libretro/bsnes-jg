struct Decompressor;

struct SPC7110 : Thread {
  SPC7110();
  ~SPC7110();

  auto synchronizeCPU() -> void;
  static auto Enter() -> void;
  auto main() -> void;
  auto step(unsigned clocks) -> void;
  auto unload() -> void;
  auto power() -> void;

  auto addClocks(unsigned clocks) -> void;

  auto read(unsigned addr, uint8_t data) -> uint8_t;
  auto write(unsigned addr, uint8_t data) -> void;

  auto mcuromRead(unsigned addr, uint8_t data) -> uint8_t;
  auto mcuromWrite(unsigned addr, uint8_t data) -> void;

  auto mcuramRead(unsigned addr, uint8_t data) -> uint8_t;
  auto mcuramWrite(unsigned addr, uint8_t data) -> void;

  auto serialize(serializer&) -> void;

  //dcu.cpp
  auto dcuLoadAddress() -> void;
  auto dcuBeginTransfer() -> void;
  auto dcuRead() -> uint8_t;

  auto deinterleave1bpp(unsigned length) -> void;
  auto deinterleave2bpp(unsigned length) -> void;
  auto deinterleave4bpp(unsigned length) -> void;

  //data.cpp
  auto dataromRead(unsigned addr) -> uint8_t;

  auto dataOffset() -> unsigned;
  auto dataAdjust() -> unsigned;
  auto dataStride() -> unsigned;

  auto setDataOffset(unsigned addr) -> void;
  auto setDataAdjust(unsigned addr) -> void;

  auto dataPortRead() -> void;

  auto dataPortIncrement4810() -> void;
  auto dataPortIncrement4814() -> void;
  auto dataPortIncrement4815() -> void;
  auto dataPortIncrement481a() -> void;

  //alu.cpp
  auto aluMultiply() -> void;
  auto aluDivide() -> void;

  ReadableMemory prom;  //program ROM
  ReadableMemory drom;  //data ROM
  WritableMemory ram;

private:
  //decompression unit
  uint8_t r4801;  //compression table B0
  uint8_t r4802;  //compression table B1
  uint7 r4803;  //compression table B2
  uint8_t r4804;  //compression table index
  uint8_t r4805;  //adjust length B0
  uint8_t r4806;  //adjust length B1
  uint8_t r4807;  //stride length
  uint8_t r4809;  //compression counter B0
  uint8_t r480a;  //compression counter B1
  uint8_t r480b;  //decompression settings
  uint8_t r480c;  //decompression status

  bool dcuPending;
  uint2 dcuMode;
  uint23 dcuAddress;
  unsigned dcuOffset;
  uint8_t dcuTile[32];
  Decompressor* decompressor;

  //data port unit
  uint8_t r4810;  //data port read + seek
  uint8_t r4811;  //data offset B0
  uint8_t r4812;  //data offset B1
  uint7 r4813;  //data offset B2
  uint8_t r4814;  //data adjust B0
  uint8_t r4815;  //data adjust B1
  uint8_t r4816;  //data stride B0
  uint8_t r4817;  //data stride B1
  uint8_t r4818;  //data port settings
  uint8_t r481a;  //data port seek

  //arithmetic logic unit
  uint8_t r4820;  //16-bit multiplicand B0, 32-bit dividend B0
  uint8_t r4821;  //16-bit multiplicand B1, 32-bit dividend B1
  uint8_t r4822;  //32-bit dividend B2
  uint8_t r4823;  //32-bit dividend B3
  uint8_t r4824;  //16-bit multiplier B0
  uint8_t r4825;  //16-bit multiplier B1
  uint8_t r4826;  //16-bit divisor B0
  uint8_t r4827;  //16-bit divisor B1
  uint8_t r4828;  //32-bit product B0, 32-bit quotient B0
  uint8_t r4829;  //32-bit product B1, 32-bit quotient B1
  uint8_t r482a;  //32-bit product B2, 32-bit quotient B2
  uint8_t r482b;  //32-bit product B3, 32-bit quotient B3
  uint8_t r482c;  //16-bit remainder B0
  uint8_t r482d;  //16-bit remainder B1
  uint8_t r482e;  //math settings
  uint8_t r482f;  //math status

  bool mulPending;
  bool divPending;

  //memory control unit
  uint8_t r4830;  //bank 0 mapping + SRAM write enable
  uint8_t r4831;  //bank 1 mapping
  uint8_t r4832;  //bank 2 mapping
  uint8_t r4833;  //bank 3 mapping
  uint8_t r4834;  //bank mapping settings
};

extern SPC7110 spc7110;
