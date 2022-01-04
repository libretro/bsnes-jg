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

#include <cstring>

#include "emulator.hpp"

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
  struct Scheduler {
    enum class Mode : unsigned { Run, Synchronize } mode;
    enum class Event : unsigned { Frame, Synchronized, Desynchronized } event;

    cothread_t host = nullptr;
    cothread_t active = nullptr;
    bool desynchronized = false;

    void enter() {
      host = co_active();
      co_switch(active);
    }

    void leave(Event event_) {
      event = event_;
      active = co_active();
      co_switch(host);
    }

    void resume(cothread_t thread) {
      if(mode == Mode::Synchronize) desynchronized = true;
      co_switch(thread);
    }

    inline bool synchronizing() const {
      return mode == Mode::Synchronize;
    }

    inline void synchronize() {
      if(mode == Mode::Synchronize) {
        if(desynchronized) {
          desynchronized = false;
          leave(Event::Desynchronized);
        } else {
          leave(Event::Synchronized);
        }
      }
    }

    inline void desynchronize() {
      desynchronized = true;
    }
  };
  extern Scheduler scheduler;

  struct Thread {
    enum : unsigned { Size = 4_KiB * sizeof(void*) };

    void create(void (*entrypoint)(), unsigned frequency_) {
      if(!thread) {
        thread = co_create(Thread::Size, entrypoint);
      } else {
        thread = co_derive(thread, Thread::Size, entrypoint);
      }
      frequency = frequency_;
      clock = 0;
    }

    bool active() const {
      return thread == co_active();
    }

    void serialize(serializer& s) {
      s.integer(frequency);
      s.integer(clock);
    }

    void serializeStack(serializer& s) {
      static uint8_t stack[Thread::Size];
      bool active = co_active() == thread;

      if(s.mode() == serializer::Size) {
        s.array(stack, Thread::Size);
        s.boolean(active);
      }

      if(s.mode() == serializer::Load) {
        s.array(stack, Thread::Size);
        s.boolean(active);
        std::memcpy(thread, stack, Thread::Size);
        if(active) scheduler.active = thread;
      }

      if(s.mode() == serializer::Save) {
        std::memcpy(stack, thread, Thread::Size);
        s.array(stack, Thread::Size);
        s.boolean(active);
      }
    }

    cothread_t thread = nullptr;
      uint32_t frequency = 0;
       int64_t clock = 0;
  };
}
