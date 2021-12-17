#pragma once

//license: GPLv3
//started: 2004-10-14

#include <emulator/emulator.hpp>
#include <emulator/random.hpp>
#include <emulator/cheat.hpp>

#include "processor/arm7tdmi.hpp"
#include "processor/gsu.hpp"
#include "processor/hg51b.hpp"
#include "processor/spc700.hpp"
#include "processor/upd96050.hpp"
#include "processor/wdc65816.hpp"

inline constexpr auto operator"" _KiB(unsigned long long value) {
  return value * 1024;
}

extern "C" {
  #include <gb/gb.h>
  #include <gb/random.h>
}

namespace SuperFamicom {
  #define platform Emulator::platform
  namespace File = Emulator::File;
  using Random = Emulator::Random;
  using Cheat = Emulator::Cheat;
  extern Random random;
  extern Cheat cheat;

  struct Scheduler {
    enum class Mode : unsigned { Run, Synchronize } mode;
    enum class Event : unsigned { Frame, Synchronized, Desynchronized } event;

    cothread_t host = nullptr;
    cothread_t active = nullptr;
    bool desynchronized = false;

    auto enter() -> void {
      host = co_active();
      co_switch(active);
    }

    auto leave(Event event_) -> void {
      event = event_;
      active = co_active();
      co_switch(host);
    }

    auto resume(cothread_t thread) -> void {
      if(mode == Mode::Synchronize) desynchronized = true;
      co_switch(thread);
    }

    inline auto synchronizing() const -> bool {
      return mode == Mode::Synchronize;
    }

    inline auto synchronize() -> void {
      if(mode == Mode::Synchronize) {
        if(desynchronized) {
          desynchronized = false;
          leave(Event::Desynchronized);
        } else {
          leave(Event::Synchronized);
        }
      }
    }

    inline auto desynchronize() -> void {
      desynchronized = true;
    }
  };
  extern Scheduler scheduler;

  struct Thread {
    enum : unsigned { Size = 4_KiB * sizeof(void*) };

    auto create(auto (*entrypoint)() -> void, unsigned frequency_) -> void {
      if(!thread) {
        thread = co_create(Thread::Size, entrypoint);
      } else {
        thread = co_derive(thread, Thread::Size, entrypoint);
      }
      frequency = frequency_;
      clock = 0;
    }

    auto active() const -> bool {
      return thread == co_active();
    }

    auto serialize(serializer& s) -> void {
      s.integer(frequency);
      s.integer(clock);
    }

    auto serializeStack(serializer& s) -> void {
      static uint8_t stack[Thread::Size];
      bool active = co_active() == thread;

      if(s.mode() == serializer::Size) {
        s.array(stack, Thread::Size);
        s.boolean(active);
      }

      if(s.mode() == serializer::Load) {
        s.array(stack, Thread::Size);
        s.boolean(active);
        memory::copy(thread, stack, Thread::Size);
        if(active) scheduler.active = thread;
      }

      if(s.mode() == serializer::Save) {
        memory::copy(stack, thread, Thread::Size);
        s.array(stack, Thread::Size);
        s.boolean(active);
      }
    }

    cothread_t thread = nullptr;
      uint32_t frequency = 0;
       int64_t clock = 0;
  };

  struct Region {
    static inline auto NTSC() -> bool;
    static inline auto PAL() -> bool;
  };

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
    alwaysinline auto tick() -> void;
    alwaysinline auto tick(unsigned clocks) -> void; private:
    alwaysinline auto tickScanline() -> void; public:

    alwaysinline auto interlace() const -> bool;
    alwaysinline auto field() const -> bool;
    alwaysinline auto vcounter() const -> unsigned;
    alwaysinline auto hcounter() const -> unsigned;
    alwaysinline auto hdot() const -> unsigned; private:
    alwaysinline auto vperiod() const -> unsigned; public:
    alwaysinline auto hperiod() const -> unsigned;

    alwaysinline auto vcounter(unsigned offset) const -> unsigned;
    alwaysinline auto hcounter(unsigned offset) const -> unsigned;

    inline auto reset() -> void;
    auto serialize(serializer&) -> void;

    function<void ()> scanline;

  private:
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

  #include "system.hpp"
  #include "memory.hpp"

  #include "cpu.hpp"
  #include "dsp.hpp"
  #include "ppu.hpp"
  #include "smp.hpp"

  #include "cartridge.hpp"
  #include "controller/controller.hpp"
  #include "coprocessor/armdsp.hpp"
  #include "coprocessor/cx4.hpp"
  #include "coprocessor/dip.hpp"
  #include "coprocessor/dsp1.hpp"
  #include "coprocessor/dsp2.hpp"
  #include "coprocessor/dsp4.hpp"
  #include "coprocessor/epsonrtc.hpp"
  #include "coprocessor/event.hpp"
  #include "coprocessor/hitachidsp.hpp"
  #include "coprocessor/icd.hpp"
  #include "coprocessor/mcc.hpp"
  #include "coprocessor/msu1.hpp"
  #include "coprocessor/necdsp.hpp"
  #include "coprocessor/obc1.hpp"
  #include "coprocessor/sa1.hpp"
  #include "coprocessor/sdd1.hpp"
  #include "coprocessor/sharprtc.hpp"
  #include "coprocessor/spc7110.hpp"
  #include "coprocessor/st0010.hpp"
  #include "coprocessor/superfx.hpp"
  #include "expansion/expansion.hpp"
  #include "slot/bsmemory.hpp"
  #include "slot/sufamiturbo.hpp"

  auto Bus::mirror(unsigned addr, unsigned size) -> unsigned {
    if(size == 0) return 0;
    unsigned base = 0;
    unsigned mask = 1 << 23;
    while(addr >= size) {
      while(!(addr & mask)) mask >>= 1;
      addr -= mask;
      if(size > mask) {
        size -= mask;
        base += mask;
      }
      mask >>= 1;
    }
    return base + addr;
  }

  auto Bus::reduce(unsigned addr, unsigned mask) -> unsigned {
    while(mask) {
      unsigned bits = (mask & -mask) - 1;
      addr = ((addr >> 1) & ~bits) | (addr & bits);
      mask = (mask & (mask - 1)) >> 1;
    }
    return addr;
  }

  auto Bus::read(unsigned addr, uint8_t data) -> uint8_t {
    return reader[lookup[addr]](target[addr], data);
  }

  auto Bus::write(unsigned addr, uint8_t data) -> void {
    return writer[lookup[addr]](target[addr], data);
  }

  auto PPUcounter::tick() -> void {
    time.hcounter += 2;  //increment by smallest unit of time.
    if(time.hcounter == hperiod()) {
      last.hperiod = hperiod();
      time.hcounter = 0;
      tickScanline();
    }
  }

  auto PPUcounter::tick(unsigned clocks) -> void {
    time.hcounter += clocks;
    if(time.hcounter >= hperiod()) {
      last.hperiod = hperiod();
      time.hcounter -= hperiod();
      tickScanline();
    }
  }

  auto PPUcounter::tickScanline() -> void {
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

  auto PPUcounter::interlace() const -> bool { return time.interlace; }
  auto PPUcounter::field() const -> bool { return time.field; }
  auto PPUcounter::vcounter() const -> unsigned { return time.vcounter; }
  auto PPUcounter::hcounter() const -> unsigned { return time.hcounter; }
  auto PPUcounter::vperiod() const -> unsigned { return time.vperiod; }
  auto PPUcounter::hperiod() const -> unsigned { return time.hperiod; }

  auto PPUcounter::vcounter(unsigned offset) const -> unsigned {
    if(offset <= hcounter()) return vcounter();
    if(vcounter() > 0) return vcounter() - 1;
    return last.vperiod - 1;
  }

  auto PPUcounter::hcounter(unsigned offset) const -> unsigned {
    if(offset <= hcounter()) return hcounter() - offset;
    return hcounter() + last.hperiod - offset;
  }

  //one PPU dot = 4 CPU clocks.
  //
  //PPU dots 323 and 327 are 6 CPU clocks long.
  //this does not apply to NTSC non-interlace scanline 240 on odd fields. this is
  //because the PPU skips one dot to alter the color burst phase of the video signal.
  //it is not known what happens for PAL 1368 clock scanlines.
  //
  //dot 323 range = {1292, 1294, 1296}
  //dot 327 range = {1310, 1312, 1314}

  auto PPUcounter::hdot() const -> unsigned {
    if(hperiod() == 1360) {
      return hcounter() >> 2;
    } else {
      return hcounter() - ((hcounter() > 1292) << 1) - ((hcounter() > 1310) << 1) >> 2;
    }
  }

  auto PPUcounter::reset() -> void {
    time = {};
    last = {};

    time.vperiod = last.vperiod = Region::NTSC() ? 262 : 312;
    time.hperiod = last.hperiod = 1364;
  }
}

#include "interface.hpp"