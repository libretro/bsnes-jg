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

struct System {
  enum class Region : unsigned { NTSC, PAL };

  inline bool loaded() const { return information.loaded; }
  inline Region region() const { return information.region; }
  inline double cpuFrequency() const { return information.cpuFrequency; }
  inline double apuFrequency() const { return information.apuFrequency; }

  inline bool fastPPU() const { return hacks.fastPPU; }

  void run();
  void runToSave();
  void runToSaveFast();
  void runToSaveStrict();
  void frameEvent();

  bool load(Emulator::Interface*);
  void save();
  void unload();
  void power(bool reset);

  //serialization.cpp
  serializer serialize(bool synchronize);
  bool unserialize(serializer&);

  unsigned frameSkip = 0;
  unsigned frameCounter = 0;
  bool runAhead = 0;

private:
  Emulator::Interface* interface = nullptr;

  struct Information {
    bool loaded = false;
    Region region = Region::NTSC;
    double cpuFrequency = Emulator::Constants::Colorburst::NTSC * 6.0;
    double apuFrequency = 32040.0 * 768.0;
    unsigned serializeSize[2] = {0, 0};
  } information;

  struct Hacks {
    bool fastPPU = false;
  } hacks;

  void serializeAll(serializer&, bool synchronize);
  unsigned serializeInit(bool synchronize);

  friend struct Cartridge;
};

extern System system;

bool Region::NTSC() { return system.region() == System::Region::NTSC; }
bool Region::PAL() { return system.region() == System::Region::PAL; }
