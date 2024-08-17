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

namespace SuperFamicom {

static constexpr double FREQ_NTSC = 315.0 / 88.0 * 1000000.0;
static constexpr double FREQ_PAL = 283.75 * 15625.0 + 25.0;

struct Region {
  static inline bool NTSC();
  static inline bool PAL();
};

struct System {
  enum class Region : unsigned { NTSC, PAL };

  inline bool loaded() const;
  inline Region region() const;
  inline double cpuFrequency() const;
  inline double apuFrequency() const;

  void run();
  void runToSave();
  void frameEvent();

  bool load();
  void save();
  void unload();
  void power(bool);

  serializer serialize(bool);
  bool unserialize(serializer&);

private:
  struct Information {
    bool loaded = false;
    Region region = Region::NTSC;
    double cpuFrequency = FREQ_NTSC * 6.0;
    double apuFrequency = 32040.0 * 768.0;
    unsigned serializeSize[2] = {0, 0};
  } information;

  struct Hacks {
    bool fastPPU = false;
  } hacks;

  void serializeAll(serializer&, bool);
  unsigned serializeInit(bool);
};

extern System system;

bool System::loaded() const {
  return information.loaded;
}

System::Region System::region() const {
  return information.region;
}

double System::cpuFrequency() const {
  return information.cpuFrequency;
}

double System::apuFrequency() const {
  return information.apuFrequency;
}

bool Region::NTSC() {
  return system.region() == System::Region::NTSC;
}

bool Region::PAL() {
  return system.region() == System::Region::PAL;
}

}
