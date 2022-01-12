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

struct Interface {
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

  serializer serialize(bool = true);
  bool unserialize(serializer&);

  void cheats(const std::vector<std::string>&);

  bool runAhead();
  void setRunAhead(bool);

  std::string getRegion();

  void setAudioBuffer(float*);
  void setAudioCallback(void (*)(size_t));
  void setAudioSpf(unsigned);
  void setAudioQuality(unsigned);
  void setInputCallback(int16_t (*)(unsigned, unsigned, unsigned));
  void setRomCallback(bool (*)(unsigned));
  void setRomBSMemory(std::vector<uint8_t>&, std::string&);
  void setRomGB(const uint8_t*, size_t);
  void setRomSufamiTurboA(std::vector<uint8_t>&, std::string&);
  void setRomSufamiTurboB(std::vector<uint8_t>&, std::string&);
  void setRomSuperFamicom(std::vector<uint8_t>&, std::string&);
  void setVideoCallback(void (*)(const uint16_t*, unsigned, unsigned, unsigned));
};

}
