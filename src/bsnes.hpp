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

#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

namespace BsnesInput {

namespace Gamepad {
  constexpr unsigned Up =     (1 << 11);
  constexpr unsigned Down =   (1 << 10);
  constexpr unsigned Left =   (1 << 9);
  constexpr unsigned Right =  (1 << 8);
  constexpr unsigned Select = (1 << 13);
  constexpr unsigned Start =  (1 << 12);
  constexpr unsigned A =      (1 << 7);
  constexpr unsigned B =      (1 << 15);
  constexpr unsigned X =      (1 << 6);
  constexpr unsigned Y =      (1 << 14);
  constexpr unsigned L =      (1 << 5);
  constexpr unsigned R =      (1 << 4);
}

namespace Mouse {
  constexpr unsigned ButtonL = (1 << 22);
  constexpr unsigned ButtonR = (1 << 23);
}

}

namespace Bsnes {

namespace Region {
  constexpr unsigned NTSC = 0;
  constexpr unsigned PAL = 1;
}

  bool loaded();
  bool load();
  void save();
  void unload();

  unsigned connected(unsigned);
  void connect(unsigned, unsigned);
  void power();
  void reset();
  void run();

  bool rtc();

  void serialize(std::vector<uint8_t>&, bool = true);
  bool unserialize(std::vector<uint8_t>&);

  void cheatsApply(const std::vector<std::string>&);
  void cheatsClear();
  bool cheatsDecode(int, std::string);

  bool runAhead();
  void setRunAhead(bool);

  unsigned getRegion();

  void setAudioBuffer(float*);
  void setAudioCallback(void (*)(size_t));
  void setAudioFrequency(double);
  void setAudioSpf(unsigned);
  void setAudioQuality(unsigned);
  void setCoprocDelayedSync(bool);
  void setCoprocPreferHLE(bool);
  void setInputGamepadCallback(int (*)(const void*, unsigned, unsigned));
  void setInputMouseCallback(int (*)(const void*, unsigned, unsigned));
  void setInputLightgunCallback(int (*)(const void*, unsigned, unsigned));
  void setOpenFileCallback(bool (*)(unsigned, std::string, std::vector<uint8_t>&));
  void setOpenStreamCallback(bool (*)(std::string, std::stringstream&));
  void setLogCallback(void (*)(int, const char *, ...));
  void setRegion(std::string);
  void setRomCallback(bool (*)(unsigned));
  void setRomBSMemory(std::vector<uint8_t>&, std::string&);
  void setRomGB(const uint8_t*, size_t);
  void setRomSufamiTurboA(std::vector<uint8_t>&, std::string&);
  void setRomSufamiTurboB(std::vector<uint8_t>&, std::string&);
  void setRomSuperFamicom(std::vector<uint8_t>&, std::string&);
  void setWriteCallback(void (*)(unsigned, std::string, const uint8_t*, unsigned));
  void setVideoBuffer(uint16_t*);
  void setVideoPixelFormat(unsigned);
  void setVideoCallback(void (*)(const uint16_t*, unsigned, unsigned, unsigned));
}
