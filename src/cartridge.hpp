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
  } has;

private:
  std::string board;

  //cartridge.cpp
  bool loadBSMemory();
  bool loadSufamiTurboA();
  bool loadSufamiTurboB();

  //load.cpp
  std::string loadBoard(std::string board);
  void loadCartridge(std::string);
  void loadCartridgeBSMemory(std::string);
  void loadCartridgeSufamiTurboA(std::string);
  void loadCartridgeSufamiTurboB(std::string);

  void loadMemory(Memory&, std::string);
  template<typename T> unsigned loadMap(std::string, T&);
  unsigned loadMap(std::string, const bfunction<uint8_t (unsigned, uint8_t)>&, const bfunction<void (unsigned, uint8_t)>&);

  void loadROM(std::string);
  void loadRAM(std::string);
  void loadICD(std::string);
  void loadMCC(std::string);
  void loadBSMemory(std::string);
  void loadSufamiTurboA(std::string);
  void loadSufamiTurboB(std::string);
  void loadDIP(std::string);
  void loadSA1(std::string);
  void loadSuperFX(std::string);
  void loadARMDSP(std::string);
  void loadHitachiDSP(std::string, unsigned roms);
  void loaduPD7725(std::string);
  void loaduPD96050(std::string);
  void loadSPC7110(std::string);
  void loadSDD1(std::string);
  void loadOBC1(std::string);
  void loadEpsonRTC(std::string);
  void loadSharpRTC(std::string);
  void loadMSU1();

  //save.cpp
  void saveCartridge(std::string);
  void saveCartridgeBSMemory(std::string);
  void saveCartridgeSufamiTurboA(std::string);
  void saveCartridgeSufamiTurboB(std::string);

  void saveMemory(Memory&, std::string);

  void saveRAM(std::string);
  void saveMCC(std::string);
  void saveSA1(std::string);
  void saveSuperFX(std::string);
  void saveARMDSP(std::string);
  void saveHitachiDSP(std::string);
  void saveuPD7725(std::string);
  void saveuPD96050(std::string);
  void saveSPC7110(std::string);
  void saveOBC1(std::string);
  void saveEpsonRTC(std::string);
  void saveSharpRTC(std::string);

  friend struct Interface;
  friend struct ICD;
};

extern Cartridge cartridge;

}
