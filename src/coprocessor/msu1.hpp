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

#include <istream>

namespace SuperFamicom {

struct MSU1 : Thread {
  void setOpenMsuCallback(void*, bool (*)(void*, std::string, std::istream**));

  void synchronizeCPU();
  void main();
  void step(unsigned);
  void unload();
  void power();

  void dataOpen();
  void audioOpen();

  uint8_t readIO(unsigned, uint8_t);
  void writeIO(unsigned, uint8_t);

  void serialize(serializer&);

private:
  std::istream *dataFile = nullptr;
  std::istream *audioFile = nullptr;
  void *udata;

  enum Flag : unsigned {
    Revision       = 0x02,  //max: 0x07
    AudioError     = 0x08,
    AudioPlaying   = 0x10,
    AudioRepeating = 0x20,
    AudioBusy      = 0x40,
    DataBusy       = 0x80,
  };

  struct IO {
    uint32_t dataSeekOffset;
    uint32_t dataReadOffset;

    uint32_t audioPlayOffset;
    uint32_t audioLoopOffset;

    uint16_t audioTrack;
    uint8_t audioVolume;

    uint32_t audioResumeTrack;
    uint32_t audioResumeOffset;

    bool audioError;
    bool audioPlay;
    bool audioRepeat;
    bool audioBusy;
    bool dataBusy;
  } io;

  bool (*openMsuCallback)(void*, std::string, std::istream**);
};

extern MSU1 msu1;

}
