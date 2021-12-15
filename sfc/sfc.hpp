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
  #include <gb/Core/gb.h>
  #include <gb/Core/random.h>
}

namespace SuperFamicom {
  #define platform Emulator::platform
  namespace File = Emulator::File;
  using Random = Emulator::Random;
  using Cheat = Emulator::Cheat;
  extern Random random;
  extern Cheat cheat;

  struct Scheduler {
    enum class Mode : uint { Run, Synchronize } mode;
    enum class Event : uint { Frame, Synchronized, Desynchronized } event;

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
    enum : uint { Size = 4_KiB * sizeof(void*) };

    auto create(auto (*entrypoint)() -> void, uint frequency_) -> void {
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
    alwaysinline auto tick(uint clocks) -> void; private:
    alwaysinline auto tickScanline() -> void; public:

    alwaysinline auto interlace() const -> bool;
    alwaysinline auto field() const -> bool;
    alwaysinline auto vcounter() const -> uint;
    alwaysinline auto hcounter() const -> uint;
    alwaysinline auto hdot() const -> uint; private:
    alwaysinline auto vperiod() const -> uint; public:
    alwaysinline auto hperiod() const -> uint;

    alwaysinline auto vcounter(uint offset) const -> uint;
    alwaysinline auto hcounter(uint offset) const -> uint;

    inline auto reset() -> void;
    auto serialize(serializer&) -> void;

    function<void ()> scanline;

  private:
    struct {
      bool interlace = 0;
      bool field = 0;
      uint vperiod = 0;
      uint hperiod = 0;
      uint vcounter = 0;
      uint hcounter = 0;
    } time;

    struct {
      uint vperiod = 0;
      uint hperiod = 0;
    } last;
  };

  #include <sfc/system.hpp>
  #include <sfc/memory.hpp>

  #include <sfc/cpu.hpp>
  #include <sfc/dsp.hpp>
  #include <sfc/ppu.hpp>
  #include <sfc/smp.hpp>

  #include <sfc/cartridge.hpp>
  #include <sfc/controller/controller.hpp>
  #include <sfc/coprocessor/armdsp.hpp>
  #include <sfc/coprocessor/cx4.hpp>
  #include <sfc/coprocessor/dip.hpp>
  #include <sfc/coprocessor/dsp1.hpp>
  #include <sfc/coprocessor/dsp2.hpp>
  #include <sfc/coprocessor/dsp4.hpp>
  #include <sfc/coprocessor/epsonrtc.hpp>
  #include <sfc/coprocessor/event.hpp>
  #include <sfc/coprocessor/hitachidsp.hpp>
  #include <sfc/coprocessor/icd.hpp>
  #include <sfc/coprocessor/mcc.hpp>
  #include <sfc/coprocessor/msu1.hpp>
  #include <sfc/coprocessor/necdsp.hpp>
  #include <sfc/coprocessor/obc1.hpp>
  #include <sfc/coprocessor/sa1.hpp>
  #include <sfc/coprocessor/sdd1.hpp>
  #include <sfc/coprocessor/sharprtc.hpp>
  #include <sfc/coprocessor/spc7110.hpp>
  #include <sfc/coprocessor/st0010.hpp>
  #include <sfc/coprocessor/superfx.hpp>
  #include <sfc/expansion/expansion.hpp>
  #include <sfc/slot/bsmemory.hpp>
  #include <sfc/slot/sufamiturbo.hpp>

  auto Bus::mirror(uint addr, uint size) -> uint {
    if(size == 0) return 0;
    uint base = 0;
    uint mask = 1 << 23;
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

  auto Bus::reduce(uint addr, uint mask) -> uint {
    while(mask) {
      uint bits = (mask & -mask) - 1;
      addr = ((addr >> 1) & ~bits) | (addr & bits);
      mask = (mask & (mask - 1)) >> 1;
    }
    return addr;
  }

  auto Bus::read(uint addr, uint8 data) -> uint8 {
    return reader[lookup[addr]](target[addr], data);
  }

  auto Bus::write(uint addr, uint8 data) -> void {
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

  auto PPUcounter::tick(uint clocks) -> void {
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
  auto PPUcounter::vcounter() const -> uint { return time.vcounter; }
  auto PPUcounter::hcounter() const -> uint { return time.hcounter; }
  auto PPUcounter::vperiod() const -> uint { return time.vperiod; }
  auto PPUcounter::hperiod() const -> uint { return time.hperiod; }

  auto PPUcounter::vcounter(uint offset) const -> uint {
    if(offset <= hcounter()) return vcounter();
    if(vcounter() > 0) return vcounter() - 1;
    return last.vperiod - 1;
  }

  auto PPUcounter::hcounter(uint offset) const -> uint {
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

  auto PPUcounter::hdot() const -> uint {
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

#include <sfc/interface.hpp>
