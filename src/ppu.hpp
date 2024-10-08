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

#pragma once

#include "function.hpp"
#include "sfc.hpp"
#include "system.hpp"

#if defined(__clang__) || defined(__GNUC__)
  #define alwaysinline inline __attribute__((always_inline))
#else
  #define alwaysinline inline
#endif

namespace SuperFamicom {

//PPUcounter emulates the H/V latch counters of the S-PPU2.
//
//real hardware has the S-CPU maintain its own copy of these counters that are
//updated based on the state of the S-PPU Vblank and Hblank pins. emulating this
//would require full lock-step synchronization for every clock tick.
//to bypass this and allow the two to run out-of-order, both the CPU and PPU
//classes inherit PPUcounter and keep their own counters.
//the timers are kept in sync, as the only differences occur on V=240 and V=261,
//based on interlace. thus, we need only synchronize and fetch interlace at any
//point before this in the frame, which is handled internally by this class at
//V=128.

struct PPUcounter {
  alwaysinline void tick();
  alwaysinline void tick(unsigned);

  inline bool interlace() const;
  inline bool field() const;
  inline unsigned vcounter() const;
  inline unsigned hcounter() const;
  inline unsigned hdot() const;
  inline unsigned hperiod() const;

  inline unsigned vcounter(unsigned) const;
  inline unsigned hcounter(unsigned) const;

  inline void reset();
  void serialize(serializer&);

  bfunction<void ()> scanline;

private:
  inline void tickScanline();
  inline unsigned vperiod() const;

  struct {
    bool interlace = 0;
    bool field = 0;
    unsigned vperiod = 0;
    unsigned hperiod = 0;
    unsigned vcounter = 0;
    unsigned hcounter = 0;
  } time;

  struct {
    unsigned vperiod = 0;
    unsigned hperiod = 0;
  } last;
};

struct PPU : Thread, PPUcounter {
  inline bool interlace() const;
  inline bool overscan() const;
  inline unsigned vdisp() const;

  PPU();
  ~PPU();

  void setCallback(void*, void (*)(const void*, unsigned, unsigned, unsigned));
  void (*videoFrame)(const void*, unsigned, unsigned, unsigned);

  alwaysinline void synchronizeCPU();
  bool load();
  void power(bool);

  alwaysinline void main();
  void cycleObjectEvaluate();
  template<unsigned Cycle> void cycleBackgroundFetch();
  void cycleBackgroundBegin();
  void cycleBackgroundBelow();
  void cycleBackgroundAbove();
  void cycleRenderPixel();
  template<unsigned> void cycle();

  void latchCounters(unsigned, unsigned);
  void latchCounters();

  void serialize(serializer&);

  void setBuffer(uint32_t*);
  void genPalette(double = 1.0, double = 1.0, double = 1.2);

  struct VRAM {
    uint16_t& operator[](unsigned);
    uint16_t data[64 * 1024];
    uint16_t mask = 0x7fff;
  } vram;

  void *udata;

private:
  inline void step();
  inline void step(unsigned);

  inline uint16_t addressVRAM() const;
  inline uint16_t readVRAM();
  inline void writeVRAM(bool, uint8_t);
  inline uint8_t readOAM(uint16_t);
  inline void writeOAM(uint16_t, uint8_t);
  inline uint8_t readCGRAM(bool, uint8_t);
  inline void writeCGRAM(uint8_t, uint16_t);
  uint8_t readIO(unsigned, uint8_t);
  void writeIO(unsigned, uint8_t);
  void updateVideoMode();

  uint32_t *output;
  uint32_t lightTable[16][32768];

  struct {
    bool interlace;
    bool overscan;
    unsigned vdisp;
  } display;

  void refresh();

  struct {
    uint8_t version;
    uint8_t mdr;
  } ppu1, ppu2;

  struct Latch {
    uint16_t vram;
    uint8_t oam;
    uint8_t cgram;
    uint8_t bgofsPPU1;
    uint8_t bgofsPPU2;
    uint8_t mode7;
    uint8_t counters;
    uint8_t hcounter;
    uint8_t vcounter;

    uint16_t oamAddress;
    uint8_t cgramAddress;
  } latch;

  struct IO {
    //$2100  INIDISP
    uint8_t displayDisable;
    uint8_t displayBrightness;

    //$2102  OAMADDL
    //$2103  OAMADDH
    uint16_t oamBaseAddress;
    uint16_t oamAddress;
    uint8_t oamPriority;

    //$2105  BGMODE
    uint8_t bgPriority;
    uint8_t bgMode;

    //$210d  BG1HOFS
    uint16_t hoffsetMode7;

    //$210e  BG1VOFS
    uint16_t voffsetMode7;

    //$2115  VMAIN
    uint8_t vramIncrementMode;
    uint8_t vramMapping;
    uint8_t vramIncrementSize;

    //$2116  VMADDL
    //$2117  VMADDH
    uint16_t vramAddress;

    //$211a  M7SEL
    uint8_t repeatMode7;
    uint8_t vflipMode7;
    uint8_t hflipMode7;

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
    uint8_t cgramAddressLatch;

    //$2133  SETINI
    uint8_t extbg;
    uint8_t pseudoHires;
    uint8_t overscan;
    uint8_t interlace;

    //$213c  OPHCT
    uint16_t hcounter;

    //$213d  OPVCT
    uint16_t vcounter;
  } io;

  struct Mosaic {
    inline bool enable() const;
    inline unsigned voffset() const;
    void scanline();
    void power();

    void serialize(serializer&);

    uint8_t size;
    uint8_t vcounter;
  };

  struct Background {
    Background(unsigned _id) : id(_id) {}

    inline bool hires() const;

    inline void scanline();
    void begin();
    void fetchNameTable();
    void fetchOffset(unsigned y);
    void fetchCharacter(unsigned, bool = false);
    alwaysinline void run(bool);
    void power();

    inline int clip(int);
    void runMode7();

    void serialize(serializer&);

    struct ID { enum : unsigned { BG1, BG2, BG3, BG4 }; };
    const unsigned id;

    struct Mode { enum : unsigned { BPP2, BPP4, BPP8, Mode7, Inactive }; };
    struct ScreenSize { enum : unsigned { Size32x32, Size32x64, Size64x32, Size64x64 }; };
    struct TileSize { enum : unsigned { Size8x8, Size16x16 }; };
    struct Screen { enum : unsigned { Above, Below }; };

    struct IO {
      uint16_t tiledataAddress;
      uint16_t screenAddress;
      uint8_t screenSize;
      uint8_t tileSize;

      uint8_t mode;
      uint8_t priority[2];

      uint8_t aboveEnable;
      uint8_t belowEnable;

      uint16_t hoffset;
      uint16_t voffset;
    } io;

    struct Pixel {
      uint8_t priority;  //0 = none (transparent)
      uint8_t palette;
      uint8_t paletteGroup;
    } above, below;

    struct Output {
      Pixel above;
      Pixel below;
    } output;

    struct Mosaic {
      uint8_t enable;
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
      uint16_t character;
      uint8_t palette;
      uint8_t paletteGroup;
      uint8_t priority;
      uint8_t hmirror;
      uint8_t vmirror;
      uint16_t data[4];
    } tiles[66];

    uint8_t renderingIndex;
    uint8_t pixelCounter;
  };

  struct OAM {
    uint8_t read(uint16_t);
    void write(uint16_t, uint8_t);

    struct Object {
      inline unsigned width() const;
      inline unsigned height() const;

      uint16_t x;
      uint8_t y;
      uint8_t character;
      uint8_t nameselect;
      uint8_t vflip;
      uint8_t hflip;
      uint8_t priority;
      uint8_t palette;
      uint8_t size;
    } object[128];
  };

  struct Object {
    inline void addressReset();
    inline void setFirstSprite();
    void frame();
    void scanline();
    void evaluate(uint8_t);
    void run();
    void fetch();
    void power();

    bool onScanline(PPU::OAM::Object&);

    void serialize(serializer&);

    OAM oam;

    struct IO {
      uint8_t aboveEnable;
      uint8_t belowEnable;
      uint8_t interlace;

      uint8_t baseSize;
      uint8_t nameselect;
      uint16_t tiledataAddress;
      uint8_t firstSprite;

      uint8_t priority[4];

      uint8_t timeOver;
      uint8_t rangeOver;
    } io;

    struct Latch {
      uint8_t firstSprite;
    } latch;

    struct Item {
      uint8_t valid;
      uint8_t index;
    };

    struct Tile {
      uint8_t valid;
      uint16_t x;
      uint8_t priority;
      uint8_t palette;
      uint8_t hflip;
      uint32_t data;
    };

    struct State {
      unsigned x;
      unsigned y;

      unsigned itemCount;
      unsigned tileCount;

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
  };

  struct Window {
    void scanline();
    void run();
    inline bool test(bool, bool, bool, bool, unsigned);
    void power();

    void serialize(serializer&);

    struct IO {
      struct Layer {
        bool oneEnable;
        bool oneInvert;
        bool twoEnable;
        bool twoInvert;
        uint8_t mask;
        bool aboveEnable;
        bool belowEnable;
      } bg1, bg2, bg3, bg4, obj;

      struct Color {
        bool oneEnable;
        bool oneInvert;
        bool twoEnable;
        bool twoInvert;
        uint8_t mask;
        uint8_t aboveMask;
        uint8_t belowMask;
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

    unsigned x;
  };

  struct Screen {
    void scanline();
    void run();
    void power();

    unsigned below(bool hires);
    unsigned above();

    unsigned blend(unsigned, unsigned) const;
    inline unsigned paletteColor(uint8_t) const;
    inline unsigned directColor(uint8_t, uint8_t) const;
    inline unsigned fixedColor() const;

    void serialize(serializer&);

    uint32_t *lineA;
    uint32_t *lineB;

    uint16_t cgram[256];

    struct IO {
      uint8_t blendMode;
      uint8_t directColor;

      uint8_t colorMode;
      uint8_t colorHalve;
      struct Layer {
        uint8_t colorEnable;
      } bg1, bg2, bg3, bg4, obj, back;

      uint8_t colorBlue;
      uint8_t colorGreen;
      uint8_t colorRed;
    } io;

    struct Math {
      struct Screen {
        uint16_t color;
        uint8_t colorEnable;
      } above, below;
      uint8_t transparent;
      uint8_t blendMode;
      uint8_t colorHalve;
    } math;
  };

  Mosaic mosaic;
  Background bg1;
  Background bg2;
  Background bg3;
  Background bg4;
  Object obj;
  Window window;
  Screen screen;

  friend struct System;
};

extern PPU ppu;

bool PPU::interlace() const {
  return display.interlace;
}

unsigned PPU::vdisp() const {
  return display.vdisp;
}

void PPUcounter::tick() {
  time.hcounter += 2;  //increment by smallest unit of time.
  if(time.hcounter == hperiod()) {
    last.hperiod = hperiod();
    time.hcounter = 0;
    tickScanline();
  }
}

void PPUcounter::tickScanline() {
  if(++time.vcounter == 128) {
    //it's not important when this is captured: it is only needed at V=240 or V=311.
    time.interlace = ppu.interlace();
    time.vperiod += interlace() && !field();
  }

  if(vcounter() == vperiod()) {
    last.vperiod = vperiod();
    //this may be off by one until V=128, hence why vperiod() is a private function.
    time.vperiod = Region::NTSC() ? 262 : 312;
    time.vcounter = 0;
    time.field ^= 1;
  }

  time.hperiod = 1364;
  //NTSC and PAL scanline rates would not match up with color clocks if every scanline were 1364 clocks.
  //to offset for this error, NTSC has one short scanline, and PAL has one long scanline.
  if(Region::NTSC() && interlace() == 0 && field() == 1 && vcounter() == 240) time.hperiod -= 4;
  if(Region::PAL()  && interlace() == 1 && field() == 1 && vcounter() == 311) time.hperiod += 4;
  if(scanline) scanline();
}

bool PPUcounter::interlace() const {
  return time.interlace;
}

bool PPUcounter::field() const {
  return time.field;
}

unsigned PPUcounter::vcounter() const {
  return time.vcounter;
}

unsigned PPUcounter::hcounter() const {
  return time.hcounter;
}

unsigned PPUcounter::vperiod() const {
  return time.vperiod;
}

unsigned PPUcounter::hperiod() const {
  return time.hperiod;
}

unsigned PPUcounter::vcounter(unsigned offset) const {
  if(offset <= hcounter()) return vcounter();
  if(vcounter() > 0) return vcounter() - 1;
  return last.vperiod - 1;
}

unsigned PPUcounter::hcounter(unsigned offset) const {
  if(offset <= hcounter()) return hcounter() - offset;
  return hcounter() + last.hperiod - offset;
}

void PPUcounter::reset() {
  time = {};
  last = {};

  time.vperiod = last.vperiod = Region::NTSC() ? 262 : 312;
  time.hperiod = last.hperiod = 1364;
}

}

#undef alwaysinline
