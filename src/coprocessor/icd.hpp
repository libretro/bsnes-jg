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

#include "sfc.hpp"

namespace SuperFamicom {

struct ICD : Thread {
  inline unsigned pathID() const;

  void setOpenCallback(std::ifstream (*)(unsigned, std::string));
  void setWriteCallback(void (*)(unsigned, std::string, const uint8_t*, unsigned));

  void synchronizeCPU();
  [[noreturn]] static void Enter();
  void main();
  void step(unsigned);
  unsigned clockFrequency() const;

  bool load();
  void save();
  void unload();
  void power(bool = false);

  //interface.cpp
  void ppuHreset();
  void ppuVreset();
  void ppuWrite(uint8_t);
  void apuWrite(int16_t, int16_t);
  void joypWrite(bool, bool);

  //io.cpp
  uint8_t readIO(unsigned, uint8_t);
  void writeIO(unsigned, uint8_t);

  //boot-roms.cpp
  static const uint8_t SGB1BootROM[256];
  static const uint8_t SGB2BootROM[256];

  //serialization.cpp
  void serialize(serializer&);

  void setRom(const uint8_t*, size_t);

  unsigned Revision = 0;
  unsigned Frequency = 0;

private:
  struct Packet {
    uint8_t& operator[](uint8_t address) { return data[address & 0x0f]; }
    uint8_t data[16];
  };

  void (*writeCallback)(unsigned, std::string, const uint8_t*, unsigned);

  Packet packet[64];
  uint8_t packetSize;

  uint8_t joypID;
  uint8_t joypLock;
  uint8_t pulseLock;
  uint8_t strobeLock;
  uint8_t packetLock;
  Packet joypPacket;
  uint8_t packetOffset;
  uint8_t bitData;
  uint8_t bitOffset;

  uint8_t output[4 * 512];
  uint8_t readBank;
  uint16_t readAddress;
  uint8_t writeBank;

  uint8_t r6003;      //control port
  uint8_t r6004;      //joypad 1
  uint8_t r6005;      //joypad 2
  uint8_t r6006;      //joypad 3
  uint8_t r6007;      //joypad 4
  uint8_t r7000[16];  //JOYP packet data
  uint8_t mltReq;     //number of active joypads

  uint8_t hcounter;
  uint8_t vcounter;

  const uint8_t *romdata;
  size_t romsize;

  struct Information {
    unsigned pathID = 0;
  } information;

  std::ifstream (*openCallback)(unsigned, std::string);

public:
  uint32_t bitmap[160 * 144];
};

extern ICD icd;

}
