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

#include <cstring>

#include "serializer.hpp"
#include "sfc.hpp"

namespace SuperFamicom {

void Scheduler::enter() {
  host = co_active();
  co_switch(active);
}

void Scheduler::leave(Event event_) {
  event = event_;
  active = co_active();
  co_switch(host);
}

void Scheduler::resume(cothread_t thread) {
  if(mode == Mode::Synchronize) desynchronized = true;
  co_switch(thread);
}

void Thread::create(void (*entrypoint)(), unsigned frequency_) {
  if(!thread) {
    thread = co_create(Thread::Size, entrypoint);
  } else {
    thread = co_derive(thread, Thread::Size, entrypoint);
  }
  frequency = frequency_;
  clock = 0;
}

void Thread::destroy() {
  if(thread) co_delete(thread);
  thread = nullptr;
}

bool Thread::active() const {
  return thread == co_active();
}

void Thread::serialize(serializer& s) {
  s.integer(frequency);
  s.integer(clock);
}

void Thread::serializeStack(serializer& s) {
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

}
