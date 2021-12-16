struct PPU : Thread, PPUcounter {
  alwaysinline auto interlace() const -> bool { return display.interlace; }
  alwaysinline auto overscan() const -> bool { return display.overscan; }
  alwaysinline auto vdisp() const -> uint { return display.vdisp; }

  PPU();
  ~PPU();

  auto synchronizeCPU() -> void;
  static auto Enter() -> void;
  auto load() -> bool;
  auto power(bool reset) -> void;

  auto main() -> void;
  noinline auto cycleObjectEvaluate() -> void;
  template<uint Cycle> noinline auto cycleBackgroundFetch() -> void;
  noinline auto cycleBackgroundBegin() -> void;
  noinline auto cycleBackgroundBelow() -> void;
  noinline auto cycleBackgroundAbove() -> void;
  noinline auto cycleRenderPixel() -> void;
  template<uint> auto cycle() -> void;

  auto latchCounters(uint hcounter, uint vcounter) -> void;
  auto latchCounters() -> void;

  auto serialize(serializer&) -> void;

private:
  alwaysinline auto step() -> void;
  alwaysinline auto step(uint clocks) -> void;

  alwaysinline auto addressVRAM() const -> uint16_t;
  alwaysinline auto readVRAM() -> uint16_t;
  alwaysinline auto writeVRAM(bool byte, uint8_t data) -> void;
  alwaysinline auto readOAM(uint10 address) -> uint8_t;
  alwaysinline auto writeOAM(uint10 address, uint8_t data) -> void;
  alwaysinline auto readCGRAM(bool byte, uint8_t address) -> uint8_t;
  alwaysinline auto writeCGRAM(uint8_t address, uint15 data) -> void;
  auto readIO(uint address, uint8_t data) -> uint8_t;
  auto writeIO(uint address, uint8_t data) -> void;
  auto updateVideoMode() -> void;

  struct VRAM {
    auto& operator[](uint address) { return data[address & mask]; }
    uint16_t data[64 * 1024];
    uint16_t mask = 0x7fff;
  } vram;

  uint16_t output[512 * 480];
  uint16_t lightTable[16][32768];

  struct {
    bool interlace;
    bool overscan;
    uint vdisp;
  } display;

  auto refresh() -> void;

  struct {
    uint4 version;
    uint8_t mdr;
  } ppu1, ppu2;

  struct Latch {
    uint16_t vram;
    uint8_t oam;
    uint8_t cgram;
    uint8_t bgofsPPU1;
    uint3 bgofsPPU2;
    uint8_t mode7;
    uint1 counters;
    uint1 hcounter;
    uint1 vcounter;

    uint10 oamAddress;
    uint8_t cgramAddress;
  } latch;

  struct IO {
    //$2100  INIDISP
    uint1 displayDisable;
    uint4 displayBrightness;

    //$2102  OAMADDL
    //$2103  OAMADDH
    uint10 oamBaseAddress;
    uint10 oamAddress;
    uint1 oamPriority;

    //$2105  BGMODE
    uint1 bgPriority;
    uint8_t bgMode;

    //$210d  BG1HOFS
    uint16_t hoffsetMode7;

    //$210e  BG1VOFS
    uint16_t voffsetMode7;

    //$2115  VMAIN
    uint1 vramIncrementMode;
    uint2 vramMapping;
    uint8_t vramIncrementSize;

    //$2116  VMADDL
    //$2117  VMADDH
    uint16_t vramAddress;

    //$211a  M7SEL
    uint2 repeatMode7;
    uint1 vflipMode7;
    uint1 hflipMode7;

    //$211b  M7A
    uint16_t m7a;

    //$211c  M7B
    uint16_t m7b;

    //$211d  M7C
    uint16_t m7c;

    //$211e  M7D
    uint16_t m7d;

    //$211f  M7X
    uint16_t m7x;

    //$2120  M7Y
    uint16_t m7y;

    //$2121  CGADD
    uint8_t cgramAddress;
    uint1 cgramAddressLatch;

    //$2133  SETINI
    uint1 extbg;
    uint1 pseudoHires;
    uint1 overscan;
    uint1 interlace;

    //$213c  OPHCT
    uint16_t hcounter;

    //$213d  OPVCT
    uint16_t vcounter;
  } io;

struct Mosaic {
  //mosaic.cpp
  alwaysinline auto enable() const -> bool;
  alwaysinline auto voffset() const -> uint;
  auto scanline() -> void;
  auto power() -> void;

  //serialization.cpp
  auto serialize(serializer&) -> void;

  uint5 size;
  uint5 vcounter;
};

struct Background {
  Background(uint id) : id(id) {}

  alwaysinline auto hires() const -> bool;

  //background.cpp
  auto frame() -> void;
  auto scanline() -> void;
  auto begin() -> void;
  auto fetchNameTable() -> void;
  auto fetchOffset(uint y) -> void;
  auto fetchCharacter(uint index, bool half = 0) -> void;
  auto run(bool screen) -> void;
  auto power() -> void;

  //mode7.cpp
  alwaysinline auto clip(int n) -> int;
  auto runMode7() -> void;

  auto serialize(serializer&) -> void;

  struct ID { enum : uint { BG1, BG2, BG3, BG4 }; };
  const uint id;

  struct Mode { enum : uint { BPP2, BPP4, BPP8, Mode7, Inactive }; };
  struct ScreenSize { enum : uint { Size32x32, Size32x64, Size64x32, Size64x64 }; };
  struct TileSize { enum : uint { Size8x8, Size16x16 }; };
  struct Screen { enum : uint { Above, Below }; };

  struct IO {
    uint16_t tiledataAddress;
    uint16_t screenAddress;
    uint2 screenSize;
    uint1 tileSize;

    uint8_t mode;
    uint8_t priority[2];

    uint1 aboveEnable;
    uint1 belowEnable;

    uint16_t hoffset;
    uint16_t voffset;
  } io;

  struct Pixel {
    uint8_t priority;  //0 = none (transparent)
    uint8_t palette;
    uint3 paletteGroup;
  } above, below;

  struct Output {
    Pixel above;
    Pixel below;
  } output;

  struct Mosaic {
     uint1 enable;
    uint16_t hcounter;
    uint16_t hoffset;
    Pixel  pixel;
  } mosaic;

  struct OffsetPerTile {
    //set in BG3 only; used by BG1 and BG2
    uint16_t hoffset;
    uint16_t voffset;
  } opt;

  struct Tile {
    uint16_t address;
    uint10 character;
    uint8_t palette;
    uint3 paletteGroup;
    uint8_t priority;
    uint1 hmirror;
    uint1 vmirror;
    uint16_t data[4];
  } tiles[66];

  uint7 renderingIndex;
  uint3 pixelCounter;

  friend class PPU;
};

struct OAM {
  auto read(uint10 address) -> uint8_t;
  auto write(uint10 address, uint8_t data) -> void;

  struct Object {
    alwaysinline auto width() const -> uint;
    alwaysinline auto height() const -> uint;

    uint9 x;
    uint8_t y;
    uint8_t character;
    uint1 nameselect;
    uint1 vflip;
    uint1 hflip;
    uint2 priority;
    uint3 palette;
    uint1 size;
  } object[128];
};

struct Object {
  alwaysinline auto addressReset() -> void;
  alwaysinline auto setFirstSprite() -> void;
  auto frame() -> void;
  auto scanline() -> void;
  auto evaluate(uint7 index) -> void;
  auto run() -> void;
  auto fetch() -> void;
  auto power() -> void;

  auto onScanline(PPU::OAM::Object&) -> bool;

  auto serialize(serializer&) -> void;

  OAM oam;

  struct IO {
    uint1 aboveEnable;
    uint1 belowEnable;
    uint1 interlace;

    uint3 baseSize;
    uint2 nameselect;
    uint16_t tiledataAddress;
    uint7 firstSprite;

    uint8_t priority[4];

    uint1 timeOver;
    uint1 rangeOver;
  } io;

  struct Latch {
     uint7 firstSprite;
  } latch;

  struct Item {
     uint1 valid;
     uint7 index;
  };

  struct Tile {
     uint1 valid;
     uint9 x;
     uint2 priority;
     uint8_t palette;
     uint1 hflip;
     uint32_t data;
  };

  struct State {
    uint x;
    uint y;

    uint itemCount;
    uint tileCount;

    bool active;
    Item item[2][32];
    Tile tile[2][34];
  } t;

  struct Output {
    struct Pixel {
      uint8_t priority;  //0 = none (transparent)
      uint8_t palette;
    } above, below;
  } output;

  friend class PPU;
};

struct Window {
  auto scanline() -> void;
  auto run() -> void;
  auto test(bool oneEnable, bool one, bool twoEnable, bool two, uint mask) -> bool;
  auto power() -> void;

  auto serialize(serializer&) -> void;

  struct IO {
    struct Layer {
      bool oneEnable;
      bool oneInvert;
      bool twoEnable;
      bool twoInvert;
      uint2 mask;
      bool aboveEnable;
      bool belowEnable;
    } bg1, bg2, bg3, bg4, obj;

    struct Color {
      bool oneEnable;
      bool oneInvert;
      bool twoEnable;
      bool twoInvert;
      uint2 mask;
      uint2 aboveMask;
      uint2 belowMask;
    } col;

    uint8_t oneLeft;
    uint8_t oneRight;
    uint8_t twoLeft;
    uint8_t twoRight;
  } io;

  struct Output {
    struct Pixel {
      bool colorEnable;
    } above, below;
  } output;

  struct {
    uint x;
  };

  friend class PPU;
};

struct Screen {
  auto scanline() -> void;
  auto run() -> void;
  auto power() -> void;

  auto below(bool hires) -> uint16_t;
  auto above() -> uint16_t;

  auto blend(uint x, uint y) const -> uint15;
  alwaysinline auto paletteColor(uint8_t palette) const -> uint15;
  alwaysinline auto directColor(uint8_t palette, uint3 paletteGroup) const -> uint15;
  alwaysinline auto fixedColor() const -> uint15;

  auto serialize(serializer&) -> void;

  uint16_t *lineA;
  uint16_t *lineB;

  uint15 cgram[256];

  struct IO {
    uint1 blendMode;
    uint1 directColor;

    uint1 colorMode;
    uint1 colorHalve;
    struct Layer {
      uint1 colorEnable;
    } bg1, bg2, bg3, bg4, obj, back;

    uint5 colorBlue;
    uint5 colorGreen;
    uint5 colorRed;
  } io;

  struct Math {
    struct Screen {
      uint15 color;
       uint1 colorEnable;
    } above, below;
    uint1 transparent;
    uint1 blendMode;
    uint1 colorHalve;
  } math;

  friend class PPU;
};

  Mosaic mosaic;
  Background bg1;
  Background bg2;
  Background bg3;
  Background bg4;
  Object obj;
  Window window;
  Screen screen;

  friend class PPU::Background;
  friend class PPU::Object;
  friend class PPU::Window;
  friend class PPU::Screen;
  friend class System;
  friend class PPUfast;
};

extern PPU ppu;
