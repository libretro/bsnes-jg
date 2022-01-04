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

#include <fstream>
#include <sstream>

#include "../audio.hpp"
#include "../interface.hpp"
#include "../cpu.hpp"
#include "../dsp.hpp"

#include "msu1.hpp"

namespace SuperFamicom {

MSU1 msu1;

void MSU1::serialize(serializer& s) {
  Thread::serialize(s);

  s.integer(io.dataSeekOffset);
  s.integer(io.dataReadOffset);

  s.integer(io.audioPlayOffset);
  s.integer(io.audioLoopOffset);

  s.integer(io.audioTrack);
  s.integer(io.audioVolume);

  s.integer(io.audioResumeTrack);
  s.integer(io.audioResumeOffset);

  s.boolean(io.audioError);
  s.boolean(io.audioPlay);
  s.boolean(io.audioRepeat);
  s.boolean(io.audioBusy);
  s.boolean(io.dataBusy);

  dataOpen();
  audioOpen();
}

void MSU1::synchronizeCPU() {
  if(clock >= 0) scheduler.resume(cpu.thread);
}


void MSU1::Enter() {
  while(true) {
    scheduler.synchronize();
    msu1.main();
  }
}

void MSU1::main() {
  int16_t left  = 0;
  int16_t right = 0;

  if(io.audioPlay) {
    if(audioFile.is_open()) {
      if(audioFile.eof()) {
        if(!io.audioRepeat) {
          io.audioPlay = false;
          audioFile.clear();
          audioFile.seekg(io.audioPlayOffset = 8, std::ios::beg);
        }
        else {
          audioFile.clear();
          audioFile.seekg(io.audioPlayOffset = io.audioLoopOffset, std::ios::beg);
        }
      }
      else {
        io.audioPlayOffset += 4;
        left  = audioFile.get() | (audioFile.get() << 8);
        right = audioFile.get() | (audioFile.get() << 8);
        left *= (io.audioVolume / 255.0);
        right *= (io.audioVolume / 255.0);
        if(dsp.mute()) left = 0, right = 0;
      }
    }
    else {
      io.audioPlay = false;
    }
  }

  if(!system.runAhead) stream->sample(left, right);
  step(1);
  synchronizeCPU();
}

void MSU1::step(unsigned clocks) {
  clock += clocks * (uint64_t)cpu.frequency;
}

void MSU1::unload() {
  dataFile.close();
  audioFile.close();
}

void MSU1::power() {
  create(MSU1::Enter, 44100);
  stream = Emulator::audio.createStream(frequency);

  io.dataSeekOffset = 0;
  io.dataReadOffset = 0;

  io.audioPlayOffset = 0;
  io.audioLoopOffset = 0;

  io.audioTrack = 0;
  io.audioVolume = 0;

  io.audioResumeTrack = ~0;  //no resume
  io.audioResumeOffset = 0;

  io.audioError = false;
  io.audioPlay = false;
  io.audioRepeat = false;
  io.audioBusy = false;
  io.dataBusy = false;

  dataOpen();
  audioOpen();
}

void MSU1::dataOpen() {
  dataFile.close();
  dataFile = Emulator::platform->fopen(ID::SuperFamicom, "msu1/data.rom");
}

void MSU1::audioOpen() {
  audioFile.close();
  std::stringstream name;
  name << "msu1/track-" << io.audioTrack << ".pcm";
  audioFile = Emulator::platform->fopen(ID::SuperFamicom, name.str());
  if(audioFile.is_open()) {
    audioFile.seekg(0, audioFile.end);
    unsigned size = audioFile.tellg();
    audioFile.seekg(0, audioFile.beg);
    if(size >= 8) {
      uint32_t header = (audioFile.get() << 24) | (audioFile.get() << 16) |
            (audioFile.get() << 8) | audioFile.get();
      if(header == 0x4d535531) {  //"MSU1"
        uint32_t offset =  audioFile.get() | (audioFile.get() << 8) |
            (audioFile.get() << 16) | (audioFile.get() << 24);
        io.audioLoopOffset = 8 + offset * 4;
        if(io.audioLoopOffset > size) io.audioLoopOffset = 8;
        io.audioError = false;
        audioFile.seekg(io.audioPlayOffset, std::ios::beg);
        return;
      }
    }
    audioFile.close();
  }
  io.audioError = true;
}

uint8_t MSU1::readIO(unsigned addr, uint8_t) {
  cpu.synchronizeCoprocessors();

  switch(0x2000 | (addr & 7)) {
  case 0x2000:
    return (
      Revision       << 0
    | io.audioError  << 3
    | io.audioPlay   << 4
    | io.audioRepeat << 5
    | io.audioBusy   << 6
    | io.dataBusy    << 7
    );
  case 0x2001:
    if(io.dataBusy) return 0x00;
    if(!dataFile) return 0x00;
    if(dataFile.eof()) return 0x00;
    io.dataReadOffset++;
    return dataFile.get();
  case 0x2002: return 'S';
  case 0x2003: return '-';
  case 0x2004: return 'M';
  case 0x2005: return 'S';
  case 0x2006: return 'U';
  case 0x2007: return '1';
  }

  return 0; // unreachable
}

void MSU1::writeIO(unsigned addr, uint8_t data) {
  cpu.synchronizeCoprocessors();

  switch(0x2000 | (addr & 7)) {
  case 0x2000: io.dataSeekOffset = (io.dataSeekOffset & 0xffffff00) | data <<  0; break;
  case 0x2001: io.dataSeekOffset = (io.dataSeekOffset & 0xffff00ff) | data <<  8; break;
  case 0x2002: io.dataSeekOffset = (io.dataSeekOffset & 0xff00ffff) | data << 16; break;
  case 0x2003: io.dataSeekOffset = (io.dataSeekOffset & 0x00ffffff) | data << 24;
    io.dataReadOffset = io.dataSeekOffset;
    if(dataFile.is_open()) dataFile.seekg(io.dataReadOffset, std::ios::beg);
    break;
  case 0x2004: io.audioTrack = (io.audioTrack & 0xff00) | data << 0; break;
  case 0x2005: io.audioTrack = (io.audioTrack & 0x00ff) | data << 8;
    io.audioPlay = false;
    io.audioRepeat = false;
    io.audioPlayOffset = 8;
    if(io.audioTrack == io.audioResumeTrack) {
      io.audioPlayOffset = io.audioResumeOffset;
      io.audioResumeTrack = ~0;  //erase resume track
      io.audioResumeOffset = 0;
    }
    audioOpen();
    break;
  case 0x2006:
    io.audioVolume = data;
    break;
  case 0x2007:
    if(io.audioBusy) break;
    if(io.audioError) break;
    io.audioPlay = bool(data & 1);
    io.audioRepeat = bool(data & 2);
    bool audioResume = bool(data & 4);
    if(!io.audioPlay && audioResume) {
      io.audioResumeTrack = io.audioTrack;
      io.audioResumeOffset = io.audioPlayOffset;
    }
    break;
  }
}

}
