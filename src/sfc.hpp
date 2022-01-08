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

#include <libco/libco.h>

#include "serializer.hpp"

inline constexpr auto operator"" _KiB(unsigned long long value) {
  return value * 1024;
}

namespace SuperFamicom {

struct Scheduler {
  enum class Mode : unsigned { Run, Synchronize } mode;
  enum class Event : unsigned { Frame, Synchronized, Desynchronized } event;

  cothread_t host = nullptr;
  cothread_t active = nullptr;
  bool desynchronized = false;

  void enter();
  void leave(Event);
  void resume(cothread_t);

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

  void create(void (*entrypoint)(), unsigned);
  bool active() const;
  void serialize(serializer&);
  void serializeStack(serializer&);

  cothread_t thread = nullptr;
  uint32_t frequency = 0;
  int64_t clock = 0;
};

}
