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

#include <sstream>
#include <utility>
#include <vector>

#include "memory.hpp"
#include "serializer.hpp"

namespace SuperFamicom {

struct Cartridge {
  unsigned pathID() const;
  std::string region() const;
  std::string headerTitle() const;

  bool load();
  void save();
  void unload();

  void serialize(serializer&);

  std::pair<void*, unsigned> getMemoryRaw(unsigned);

  void setOpenFileCallback(void*, bool (*)(void*, std::string, std::vector<uint8_t>&));
  void setOpenStreamCallback(void*, bool (*)(void*, std::string, std::stringstream&));
  void setRomCallback(void*, bool (*)(void*, unsigned));
  void setWriteCallback(void*, void (*)(void*, std::string, const uint8_t*, unsigned));

  void setRegion(std::string);

  void setRomBSMemory(std::vector<uint8_t>&, std::string&);
  void setRomSufamiTurboA(std::vector<uint8_t>&, std::string&);
  void setRomSufamiTurboB(std::vector<uint8_t>&, std::string&);
  void setRomSuperFamicom(std::vector<uint8_t>&, std::string&);

  ReadableMemory rom;
  WritableMemory ram;

  struct Information {
    unsigned pathID = 0;
    std::string region;
    std::string sha256;
  } information;

  struct Has {
    bool ICD;
    bool MCC;
    bool DIP;
    bool Event;
    bool SA1;
    bool SuperFX;
    bool ARMDSP;
    bool HitachiDSP;
    bool NECDSP;
    bool EpsonRTC;
    bool SharpRTC;
    bool SPC7110;
    bool SDD1;
    bool OBC1;
    bool MSU1;

    bool Cx4;
    bool DSP1;
    bool DSP2;
    bool DSP4;
    bool ST0010;

    bool GameBoySlot;
    bool BSMemorySlot;
    bool SufamiTurboSlotA;
    bool SufamiTurboSlotB;

    bool SRAM;
  } has;

private:
  std::string loadBoard(std::string);
  void loadBSMemory(std::string);

  void loadMemory(Memory&, std::string);
  template<typename T> unsigned loadMap(std::string, T&);
  unsigned loadMap(std::string, const bfunction<uint8_t (unsigned, uint8_t)>&, const bfunction<void (unsigned, uint8_t)>&);

  void saveMemory(Memory&, std::string);

  std::string board;
  std::string forceRegion;

  bool (*openFileCallback)(void*, std::string, std::vector<uint8_t>&);
  bool (*openStreamCallback)(void*, std::string, std::stringstream&);
  bool (*romCallback)(void*, unsigned);
  void (*writeCallback)(void*, std::string, const uint8_t*, unsigned);

  void *udata_v;
  void *udata_s;
  void *udata_rom;
  void *udata_wr;
};

extern Cartridge cartridge;

}
