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

struct ICD : Emulator::Platform, Thread {
  Emulator::Stream *stream;
  Emulator::Cheat cheats;

  inline unsigned pathID() const { return information.pathID; }

  void synchronizeCPU();
  static void Enter();
  void main();
  void step(unsigned clocks);
  unsigned clockFrequency() const;

  bool load();
  void save();
  void unload();
  void power(bool reset = false);

  //interface.cpp
  void ppuHreset();
  void ppuVreset();
  void ppuWrite(nall::Natural< 2> color);
  void apuWrite(int16_t left, int16_t right);
  void joypWrite(bool p14, bool p15);

  //io.cpp
  uint8_t readIO(unsigned addr, uint8_t data);
  void writeIO(unsigned addr, uint8_t data);

  //boot-roms.cpp
  static const uint8_t SGB1BootROM[256];
  static const uint8_t SGB2BootROM[256];

  //serialization.cpp
  void serialize(serializer&);

  unsigned Revision = 0;
  unsigned Frequency = 0;

private:
  struct Packet {
    auto operator[](nall::Natural< 4> address) -> uint8_t& { return data[address]; }
    uint8_t data[16];
  };
  Packet packet[64];
  nall::Natural< 7> packetSize;

  nall::Natural< 2> joypID;
  nall::Natural< 1> joypLock;
  nall::Natural< 1> pulseLock;
  nall::Natural< 1> strobeLock;
  nall::Natural< 1> packetLock;
  Packet joypPacket;
  nall::Natural< 4> packetOffset;
  uint8_t bitData;
  nall::Natural< 3> bitOffset;

  uint8_t output[4 * 512];
  nall::Natural< 2> readBank;
  nall::Natural< 9> readAddress;
  nall::Natural< 2> writeBank;

  uint8_t r6003;      //control port
  uint8_t r6004;      //joypad 1
  uint8_t r6005;      //joypad 2
  uint8_t r6006;      //joypad 3
  uint8_t r6007;      //joypad 4
  uint8_t r7000[16];  //JOYP packet data
  uint8_t mltReq;     //number of active joypads

  uint8_t hcounter;
  uint8_t vcounter;

  struct Information {
    unsigned pathID = 0;
  } information;

public:
  //warning: the size of this object will be too large due to C++ size rules differing from C rules.
  //in practice, this won't pose a problem so long as the struct is never accessed from C++ code,
  //as the offsets of all member variables will be wrong compared to what the C SameBoy code expects.
  GB_gameboy_t sameboy;
  uint32_t bitmap[160 * 144];
};

extern ICD icd;
