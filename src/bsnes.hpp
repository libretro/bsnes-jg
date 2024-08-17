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

namespace Bsnes {
  namespace Audio {
    typedef struct _Spec {
      double freq;
      unsigned spf;
      unsigned rsqual;
      float *buf;
      void *ptr;
      void (*cb)(const void*, size_t);
    } Spec;
  }

  namespace Video {
    typedef struct _Spec {
      unsigned pixfmt;
      uint16_t *buf;
      void *ptr;
      void (*cb)(const uint16_t*, unsigned, unsigned, unsigned);
    } Spec;
  }

  namespace Input {
    namespace Device {
      enum : unsigned {
        Unconnected,
        Gamepad,
        Mouse,
        Multitap,
        SuperScope,
        Justifier,
      };
    }

    namespace Gamepad {
      constexpr unsigned Up =       (1 << 11);
      constexpr unsigned Down =     (1 << 10);
      constexpr unsigned Left =     (1 << 9);
      constexpr unsigned Right =    (1 << 8);
      constexpr unsigned Select =   (1 << 13);
      constexpr unsigned Start =    (1 << 12);
      constexpr unsigned A =        (1 << 7);
      constexpr unsigned B =        (1 << 15);
      constexpr unsigned X =        (1 << 6);
      constexpr unsigned Y =        (1 << 14);
      constexpr unsigned L =        (1 << 5);
      constexpr unsigned R =        (1 << 4);
    }

    namespace Mouse {
      constexpr unsigned ButtonL =  (1 << 22);
      constexpr unsigned ButtonR =  (1 << 23);
    }

    typedef struct _Spec {
      unsigned port;
      unsigned device;
      void *ptr;
      int (*cb)(const void*, unsigned, unsigned);
    } Spec;
  }

  namespace Region {
    constexpr unsigned NTSC = 0;
    constexpr unsigned PAL = 1;
  }

  bool loaded();
  bool load();
  void save();
  void unload();

  void power();
  void reset();
  void run();

  bool rtc();

  void serialize(std::vector<uint8_t>&, bool = true);
  bool unserialize(std::vector<uint8_t>&);

  void cheatsApply(const std::vector<std::string>&);
  void cheatsClear();
  bool cheatsDecode(int, std::string);

  unsigned getRegion();

  void setAudioSpec(Audio::Spec);
  void setCoprocDelayedSync(bool);
  void setCoprocPreferHLE(bool);
  void setInputSpec(Input::Spec);
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
  void setVideoSpec(Video::Spec);
}
