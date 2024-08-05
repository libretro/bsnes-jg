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

#include <algorithm>
#include <cstring>
#include <iterator>

#include "coprocessor/armdsp.hpp"
#include "coprocessor/cx4.hpp"
#include "coprocessor/dip.hpp"
#include "coprocessor/dsp1.hpp"
#include "coprocessor/dsp2.hpp"
#include "coprocessor/dsp4.hpp"
#include "coprocessor/epsonrtc.hpp"
#include "coprocessor/hitachidsp.hpp"
#include "coprocessor/icd.hpp"
#include "coprocessor/mcc.hpp"
#include "coprocessor/msu1.hpp"
#include "coprocessor/necdsp.hpp"
#include "coprocessor/obc1.hpp"
#include "coprocessor/sa1.hpp"
#include "coprocessor/sdd1.hpp"
#include "coprocessor/sharprtc.hpp"
#include "coprocessor/spc7110.hpp"
#include "coprocessor/st0010.hpp"
#include "coprocessor/superfx.hpp"
#include "bsmemory.hpp"
#include "emulator.hpp"
#include "heuristics.hpp"
#include "markup.hpp"
#include "settings.hpp"
#include "sha256.hpp"
#include "sufamiturbo.hpp"
#include "system.hpp"

#include "cartridge.hpp"

namespace SuperFamicom {

/* note: this is not copyrightable, as it is purely math tables such as sin and cos */

static const uint8_t hitachidspStaticDataROM[3072] = {
  255,255,255,0,0,128,0,0,64,170,170,42,0,0,32,153,153,25,85,85,21,36,73,18,0,0,16,227,56,14,204,204,
  12,232,162,11,170,170,10,157,216,9,146,36,9,136,136,8,0,0,8,135,135,7,113,28,7,161,188,6,102,102,6,97,
  24,6,116,209,5,178,144,5,85,85,5,184,30,5,78,236,4,161,189,4,73,146,4,238,105,4,68,68,4,8,33,4,
  0,0,4,248,224,3,195,195,3,58,168,3,56,142,3,159,117,3,80,94,3,52,72,3,51,51,3,56,31,3,48,12,
  3,11,250,2,186,232,2,45,216,2,89,200,2,49,185,2,170,170,2,188,156,2,92,143,2,130,130,2,39,118,2,67,
  106,2,208,94,2,200,83,2,36,73,2,224,62,2,247,52,2,99,43,2,34,34,2,46,25,2,132,16,2,32,8,2,
  0,0,2,31,248,1,124,240,1,19,233,1,225,225,1,230,218,1,29,212,1,133,205,1,28,199,1,224,192,1,207,186,
  1,232,180,1,40,175,1,142,169,1,26,164,1,200,158,1,153,153,1,139,148,1,156,143,1,203,138,1,24,134,1,129,
  129,1,5,125,1,164,120,1,93,116,1,46,112,1,22,108,1,22,104,1,44,100,1,88,96,1,152,92,1,237,88,1,
  85,85,1,208,81,1,94,78,1,253,74,1,174,71,1,111,68,1,65,65,1,34,62,1,19,59,1,19,56,1,33,53,
  1,62,50,1,104,47,1,159,44,1,228,41,1,53,39,1,146,36,1,251,33,1,112,31,1,240,28,1,123,26,1,17,
  24,1,177,21,1,92,19,1,17,17,1,207,14,1,151,12,1,104,10,1,66,8,1,36,6,1,16,4,1,4,2,1,
  0,0,1,3,254,0,15,252,0,35,250,0,62,248,0,96,246,0,137,244,0,185,242,0,240,240,0,46,239,0,115,237,
  0,189,235,0,14,234,0,101,232,0,194,230,0,37,229,0,142,227,0,252,225,0,112,224,0,233,222,0,103,221,0,235,
  219,0,116,218,0,1,217,0,148,215,0,43,214,0,199,212,0,104,211,0,13,210,0,182,208,0,100,207,0,22,206,0,
  204,204,0,135,203,0,69,202,0,7,201,0,206,199,0,152,198,0,101,197,0,55,196,0,12,195,0,228,193,0,192,192,
  0,160,191,0,130,190,0,105,189,0,82,188,0,62,187,0,46,186,0,33,185,0,23,184,0,15,183,0,11,182,0,9,
  181,0,11,180,0,15,179,0,22,178,0,31,177,0,44,176,0,58,175,0,76,174,0,96,173,0,118,172,0,143,171,0,
  170,170,0,200,169,0,232,168,0,10,168,0,47,167,0,85,166,0,126,165,0,169,164,0,215,163,0,6,163,0,55,162,
  0,107,161,0,160,160,0,216,159,0,17,159,0,76,158,0,137,157,0,200,156,0,9,156,0,76,155,0,144,154,0,215,
  153,0,31,153,0,104,152,0,180,151,0,1,151,0,79,150,0,160,149,0,242,148,0,69,148,0,154,147,0,241,146,0,
  73,146,0,162,145,0,253,144,0,90,144,0,184,143,0,23,143,0,120,142,0,218,141,0,61,141,0,162,140,0,8,140,
  0,112,139,0,216,138,0,66,138,0,174,137,0,26,137,0,136,136,0,247,135,0,103,135,0,217,134,0,75,134,0,191,
  133,0,52,133,0,169,132,0,33,132,0,153,131,0,18,131,0,140,130,0,8,130,0,132,129,0,2,129,0,128,128,0,
  0,0,0,0,0,16,158,160,22,122,182,27,0,0,32,239,198,35,28,49,39,255,84,42,60,65,45,0,0,48,176,152,
  50,229,16,53,245,108,55,86,176,57,212,221,59,189,247,61,0,0,64,61,248,65,219,225,67,12,190,69,222,141,71,58,
  82,73,241,11,75,185,187,76,56,98,78,0,0,80,149,149,81,112,35,83,254,169,84,162,41,86,183,162,87,144,21,89,
  121,130,90,186,233,91,148,75,93,67,168,94,0,0,96,254,82,97,112,161,98,131,235,99,96,49,101,50,115,102,29,177,
  103,68,235,104,202,33,106,205,84,107,108,132,108,194,176,109,235,217,110,0,0,112,24,35,113,74,67,114,173,96,115,84,
  123,116,84,147,117,191,168,118,168,187,119,31,204,120,52,218,121,249,229,122,122,239,123,200,246,124,239,251,125,253,254,126,
  0,0,128,1,255,128,15,252,129,52,247,130,123,240,131,238,231,132,152,221,133,130,209,134,182,195,135,61,180,136,31,163,
  137,102,144,138,25,124,139,65,102,140,228,78,141,11,54,142,188,27,143,0,0,144,219,226,144,86,196,145,117,164,146,65,
  131,147,189,96,148,241,60,149,226,23,150,150,241,150,17,202,151,89,161,152,115,119,153,100,76,154,49,32,155,222,242,155,
  112,196,156,235,148,157,84,100,158,175,50,159,0,0,160,74,204,160,146,151,161,220,97,162,42,43,163,130,243,163,230,186,
  164,90,129,165,225,70,166,126,11,167,53,207,167,9,146,168,253,83,169,19,21,170,80,213,170,180,148,171,69,83,172,3,
  17,173,242,205,173,21,138,174,110,69,175,0,0,176,204,185,176,214,114,177,32,43,178,172,226,178,124,153,179,147,79,180,
  243,4,181,157,185,181,149,109,182,220,32,183,117,211,183,96,133,184,160,54,185,56,231,185,40,151,186,115,70,187,26,245,
  187,32,163,188,134,80,189,78,253,189,121,169,190,9,85,191,0,0,192,95,170,192,40,84,193,92,253,193,253,165,194,13,
  78,195,140,245,195,125,156,196,225,66,197,184,232,197,5,142,198,201,50,199,6,215,199,187,122,200,235,29,201,152,192,201,
  193,98,202,105,4,203,145,165,203,58,70,204,100,230,204,18,134,205,68,37,206,252,195,206,58,98,207,0,0,208,78,157,
  208,38,58,209,137,214,209,119,114,210,243,13,211,252,168,211,148,67,212,188,221,212,116,119,213,190,16,214,155,169,214,11,
  66,215,15,218,215,169,113,216,216,8,217,159,159,217,254,53,218,245,203,218,133,97,219,176,246,219,118,139,220,216,31,221,
  215,179,221,115,71,222,173,218,222,134,109,223,0,0,224,25,146,224,212,35,225,48,181,225,48,70,226,210,214,226,25,103,
  227,4,247,227,148,134,228,203,21,229,168,164,229,45,51,230,90,193,230,47,79,231,173,220,231,214,105,232,169,246,232,38,
  131,233,80,15,234,38,155,234,168,38,235,217,177,235,183,60,236,67,199,236,127,81,237,106,219,237,6,101,238,82,238,238,
  80,119,239,0,0,240,97,136,240,118,16,241,62,152,241,186,31,242,234,166,242,207,45,243,105,180,243,185,58,244,192,192,
  244,125,70,245,242,203,245,30,81,246,2,214,246,159,90,247,245,222,247,5,99,248,206,230,248,82,106,249,144,237,249,138,
  112,250,63,243,250,177,117,251,223,247,251,202,121,252,114,251,252,216,124,253,251,253,253,222,126,254,127,255,254,223,127,255,
  0,0,0,58,36,3,85,72,6,50,108,9,178,143,12,183,178,15,32,213,18,208,246,21,166,23,25,133,55,28,78,86,
  31,225,115,34,32,144,37,237,170,40,40,196,43,179,219,46,112,241,49,64,5,53,4,23,56,159,38,59,242,51,62,224,
  62,65,73,71,68,16,77,71,24,80,74,67,80,77,114,77,80,137,71,83,105,62,86,247,49,89,20,34,92,164,14,95,
  138,247,97,169,220,100,229,189,103,32,155,106,64,116,109,39,73,112,186,25,115,221,229,117,116,173,120,101,112,123,147,46,
  126,228,231,128,60,156,131,130,75,134,154,245,136,107,154,139,217,57,142,204,211,144,42,104,147,217,246,149,191,127,152,197,
  2,155,209,127,157,202,246,159,153,103,162,36,210,164,85,54,167,20,148,169,73,235,171,221,59,174,186,133,176,201,200,178,
  243,4,181,34,58,183,65,104,185,58,143,187,249,174,189,103,199,191,112,216,193,0,226,195,3,228,197,101,222,199,18,209,
  201,247,187,203,2,159,205,31,122,207,61,77,209,72,24,211,49,219,212,228,149,214,82,72,216,105,242,217,26,148,219,83,
  45,221,5,190,222,33,70,224,151,197,225,89,60,227,89,170,228,135,15,230,215,107,231,59,191,232,166,9,234,11,75,235,
  94,131,236,147,178,237,157,216,238,115,245,239,8,9,241,82,19,242,71,20,243,221,11,244,10,250,244,198,222,245,7,186,
  246,197,139,247,247,83,248,151,18,249,157,199,249,1,115,250,190,20,251,205,172,251,39,59,252,201,191,252,171,58,253,203,
  171,253,35,19,254,175,112,254,109,196,254,87,14,255,109,78,255,171,132,255,15,177,255,151,211,255,67,236,255,16,251,255,
  0,0,0,249,162,0,246,69,1,248,232,1,1,140,2,20,47,3,52,210,3,100,117,4,165,24,5,251,187,5,104,95,
  6,239,2,7,146,166,7,84,74,8,56,238,8,64,146,9,110,54,10,199,218,10,76,127,11,1,36,12,231,200,12,2,
  110,13,85,19,14,227,184,14,174,94,15,185,4,16,8,171,16,158,81,17,125,248,17,169,159,18,37,71,19,244,238,19,
  25,151,20,153,63,21,117,232,21,178,145,22,83,59,23,92,229,23,209,143,24,180,58,25,10,230,25,216,145,26,32,62,
  27,231,234,27,49,152,28,2,70,29,95,244,29,76,163,30,206,82,31,234,2,32,163,179,32,0,101,33,5,23,34,184,
  201,34,30,125,35,60,49,36,24,230,36,185,155,37,36,82,38,95,9,39,113,193,39,97,122,40,54,52,41,246,238,41,
  170,170,42,89,103,43,10,37,44,199,227,44,152,163,45,133,100,46,153,38,47,220,233,47,89,174,48,27,116,49,44,59,
  50,152,3,51,107,205,51,177,152,52,120,101,53,206,51,54,193,3,55,96,213,55,187,168,56,228,125,57,236,84,58,230,
  45,59,230,8,60,1,230,60,77,197,61,227,166,62,220,138,63,82,113,64,98,90,65,44,70,66,208,52,67,113,38,68,
  55,27,69,74,19,70,214,14,71,12,14,72,32,17,73,76,24,74,205,35,75,234,51,76,236,72,77,39,99,78,249,130,
  79,201,168,80,10,213,81,63,8,83,252,66,84,234,133,85,204,209,86,130,39,88,21,136,89,188,244,90,237,110,92,108,
  248,93,105,147,95,163,66,97,165,9,99,30,237,100,129,243,102,23,38,105,34,147,107,165,82,110,124,147,113,180,206,117,
  0,0,0,36,3,0,72,6,0,109,9,0,147,12,0,186,15,0,226,18,0,11,22,0,54,25,0,99,28,0,147,31,
  0,196,34,0,249,37,0,48,41,0,107,44,0,169,47,0,235,50,0,50,54,0,124,57,0,203,60,0,31,64,0,121,
  67,0,216,70,0,61,74,0,168,77,0,25,81,0,146,84,0,17,88,0,153,91,0,40,95,0,192,98,0,96,102,0,
  9,106,0,188,109,0,122,113,0,65,117,0,20,121,0,242,124,0,220,128,0,210,132,0,213,136,0,230,140,0,5,145,
  0,51,149,0,112,153,0,190,157,0,28,162,0,139,166,0,13,171,0,162,175,0,75,180,0,9,185,0,220,189,0,198,
  194,0,200,199,0,227,204,0,24,210,0,103,215,0,211,220,0,93,226,0,6,232,0,207,237,0,187,243,0,202,249,0,
  0,0,1,92,6,1,226,12,1,148,19,1,115,26,1,131,33,1,198,40,1,62,48,1,239,55,1,220,63,1,8,72,
  1,119,80,1,45,89,1,45,98,1,125,107,1,34,117,1,33,127,1,128,137,1,68,148,1,118,159,1,28,171,1,62,
  183,1,231,195,1,31,209,1,241,222,1,105,237,1,149,252,1,131,12,2,68,29,2,233,46,2,134,65,2,51,85,2,
  9,106,2,37,128,2,167,151,2,181,176,2,120,203,2,35,232,2,236,6,3,21,40,3,235,75,3,198,114,3,16,157,
  3,71,203,3,2,254,3,247,53,4,5,116,4,63,185,4,255,6,5,249,94,5,93,195,5,9,55,6,207,189,6,230,
  92,7,151,27,8,109,4,9,54,39,10,198,156,11,129,142,13,233,70,16,255,90,20,113,38,27,72,188,40,181,123,81,
  255,255,255,16,251,255,67,236,255,151,211,255,15,177,255,171,132,255,109,78,255,87,14,255,109,196,254,175,112,254,35,19,
  254,203,171,253,171,58,253,201,191,252,39,59,252,205,172,251,190,20,251,1,115,250,157,199,249,151,18,249,247,83,248,197,
  139,247,7,186,246,198,222,245,10,250,244,221,11,244,71,20,243,82,19,242,8,9,241,115,245,239,157,216,238,147,178,237,
  94,131,236,11,75,235,166,9,234,59,191,232,215,107,231,135,15,230,89,170,228,89,60,227,151,197,225,33,70,224,5,190,
  222,83,45,221,26,148,219,105,242,217,82,72,216,228,149,214,49,219,212,72,24,211,61,77,209,31,122,207,2,159,205,247,
  187,203,18,209,201,101,222,199,3,228,197,0,226,195,112,216,193,103,199,191,249,174,189,58,143,187,65,104,185,34,58,183,
  243,4,181,201,200,178,186,133,176,221,59,174,73,235,171,20,148,169,85,54,167,36,210,164,153,103,162,202,246,159,209,127,
  157,197,2,155,191,127,152,217,246,149,42,104,147,204,211,144,217,57,142,107,154,139,154,245,136,130,75,134,60,156,131,228,
  231,128,147,46,126,101,112,123,116,173,120,221,229,117,186,25,115,39,73,112,64,116,109,32,155,106,229,189,103,169,220,100,
  138,247,97,164,14,95,20,34,92,247,49,89,105,62,86,137,71,83,114,77,80,67,80,77,24,80,74,16,77,71,73,71,
  68,224,62,65,242,51,62,159,38,59,4,23,56,64,5,53,112,241,49,179,219,46,40,196,43,237,170,40,32,144,37,225,
  115,34,78,86,31,133,55,28,166,23,25,208,246,21,32,213,18,183,178,15,178,143,12,50,108,9,85,72,6,58,36,3
};

static Game game;
static Game slotGameBoy;
static Game slotBSMemory;
static Game slotSufamiTurboA;
static Game slotSufamiTurboB;

unsigned Cartridge::pathID() const { return information.pathID; }
std::string Cartridge::region() const { return information.region; }
std::string Cartridge::headerTitle() const { return game.title; }

std::string Cartridge::loadBoard(std::string node) {
  if (node.find("SNSP-") == 0) node.replace(0, 5, "SHVC-");
  if (node.find("MAXI-") == 0) node.replace(0, 5, "SHVC-");
  if (node.find("MJSC-") == 0) node.replace(0, 5, "SHVC-");
  if (node.find("EA-") == 0) node.replace(0, 3, "SHVC-");
  if (node.find("WEI-") == 0) node.replace(0, 4, "SHVC-");

  std::ifstream boardsfile = openCallback(ID::System, "boards.bml");
  if (boardsfile.is_open()) {
    std::string boards((std::istreambuf_iterator<char>(boardsfile)),
      (std::istreambuf_iterator<char>()));

    return BML::searchBoard(boards, node);
  }

  return {};
}

template<typename T>  //T = ReadableMemory, WritableMemory
unsigned Cartridge::loadMap(std::string map, T& memory) {
  std::string addr = BML::search(map, {"map", "address"});
  std::string strsize = BML::search(map, {"map", "size"});
  std::string strbase = BML::search(map, {"map", "base"});
  std::string strmask = BML::search(map, {"map", "mask"});
  unsigned size = strsize.empty() ? 0 : std::stoi(strsize, nullptr, 16);
  unsigned base = strbase.empty() ? 0 : std::stoi(strbase, nullptr, 16);
  unsigned mask = strmask.empty() ? 0 : std::stoi(strmask, nullptr, 16);
  if(size == 0) size = memory.size();
  if(size == 0) return 0; //does this ever actually occur? - Yes! Sufami Turbo.
  return bus.map({&T::read, &memory}, {&T::write, &memory}, addr, size, base, mask);
}

unsigned Cartridge::loadMap(
  std::string map,
  const bfunction<uint8_t (unsigned, uint8_t)>& reader,
  const bfunction<void  (unsigned, uint8_t)>& writer
) {
  std::string addr = BML::search(map, {"map", "address"});
  std::string strsize = BML::search(map, {"map", "size"});
  std::string strbase = BML::search(map, {"map", "base"});
  std::string strmask = BML::search(map, {"map", "mask"});
  unsigned size = strsize.empty() ? 0 : std::stoi(strsize, nullptr, 16);
  unsigned base = strbase.empty() ? 0 : std::stoi(strbase, nullptr, 16);
  unsigned mask = strmask.empty() ? 0 : std::stoi(strmask, nullptr, 16);
  return bus.map(reader, writer, addr, size, base, mask);
}

void Cartridge::loadMemory(Memory& mem, std::string node) {
  Game::Memory memory;
  if(game.memory(memory, node)) {
    mem.allocate(memory.size);
    if ((memory.type == "RAM" && !memory.nonVolatile)
        || (memory.type == "RTC" && !memory.nonVolatile))
      return;

    const std::string memory_name = memory.name();

    if (memory_name == "program.rom") {
      for (unsigned i = 0; i < memory.size; ++i) {
        mem.data()[i] = game.prgrom[i];
      }
    }
    else if (memory_name == "data.rom") {
      for (unsigned i = 0; i < memory.size; ++i) {
        mem.data()[i] = game.datarom[i];
      }
    }
    else if (memory_name == "expansion.rom") {
      for (unsigned i = 0; i < memory.size; ++i) {
        mem.data()[i] = game.exprom[i];
      }
    }
    else {
      std::ifstream memfile = openCallback(pathID(), memory_name);
      if (memfile.is_open()) {
        memfile.seekg(0, memfile.end);
        unsigned fsize = memfile.tellg();
        memfile.seekg(0, memfile.beg);
        memfile.read((char*)mem.data(), std::min(fsize, mem.size()));
        memfile.close();
      }
    }
  }
}

//slot(type=BSMemory)
void Cartridge::loadBSMemory(std::string node) {
  has.BSMemorySlot = true;

  if(romCallback(ID::BSMemory)) {
    bsmemory.pathID = ID::BSMemory;

    if (!slotBSMemory.document.empty()) {
      slotBSMemory.load(slotBSMemory.document);
      if (Game::Memory memory = Game::Memory(BML::searchNode(slotBSMemory.document, {"game", "board", "memory"}))) {
        bsmemory.ROM = memory.type == "ROM";
        bsmemory.memory.allocate(memory.size);
        for (unsigned i = 0; i < memory.size; ++i) {
          bsmemory.memory.data()[i] = slotBSMemory.prgrom.data()[i];
        }
      }
    }

    for (std::string map : BML::searchList(node, "map")) {
      loadMap(map, bsmemory);
    }
  }
}

//processor(architecture=uPD78214)
/*void Cartridge::loadEvent(Markup::Node node) {
  has.Event = true;
  event.board = Event::Board::Unknown;
  if(node["identifier"].text() == "Campus Challenge '92") event.board = Event::Board::CampusChallenge92;
  if(node["identifier"].text() == "PowerFest '94") event.board = Event::Board::PowerFest94;

  for(auto map : node.find("map")) {
    loadMap(map, {&Event::read, &event}, {&Event::write, &event});
  }

  if(auto mcu = node["mcu"]) {
    for(auto map : mcu.find("map")) {
      loadMap(map, {&Event::mcuRead, &event}, {&Event::mcuWrite, &event});
    }
    if(auto memory = mcu["memory(type=ROM,content=Program)"]) {
      loadMemory(event.rom[0], memory);
    }
    if(auto memory = mcu["memory(type=ROM,content=Level-1)"]) {
      loadMemory(event.rom[1], memory);
    }
    if(auto memory = mcu["memory(type=ROM,content=Level-2)"]) {
      loadMemory(event.rom[2], memory);
    }
    if(auto memory = mcu["memory(type=ROM,content=Level-3)"]) {
      loadMemory(event.rom[3], memory);
    }
  }
}*/

void Cartridge::saveMemory(Memory& sram, std::string node) {
  Game::Memory memory;
  if (game.memory(memory, node)
      && !(memory.type == "RAM" && !memory.nonVolatile)
      && !(memory.type == "RTC" && !memory.nonVolatile)) {
    writeCallback(pathID(), memory.name(), sram.data(), sram.size());
  }
}

void Cartridge::serialize(serializer& s) {
  s.array(ram.data(), ram.size());
}

Cartridge cartridge;

bool Cartridge::load() {
  information = {};
  has = {};
  game = {};
  slotGameBoy = {};
  slotBSMemory = {};
  slotSufamiTurboA = {};
  slotSufamiTurboB = {};

  if(!romCallback(ID::SuperFamicom) || game.document.empty())
    return false;

  information.pathID = ID::SuperFamicom;

  if (forceRegion == "")
    information.region = "Auto";
  else
    information.region = forceRegion;

  game.load(game.document);

  board = BML::searchNode(game.document, {"board"});

  if (board.empty())
    board = loadBoard(game.board);

  // If it is still empty, the board is not supported
  if (board.empty())
    return false;

  if(region() == "Auto") {
    std::string regstr = game.region.substr(game.region.length() - 3, game.region.length());
    if (regstr == "BRA" || regstr == "CAN" || regstr == "HKG" ||
        regstr == "JPN" || regstr == "KOR" || regstr == "LTN" ||
        regstr == "ROC" || regstr == "USA" || (!game.region.rfind("SHVC-", 0))
        || game.region == "NTSC") {
      information.region = "NTSC";
    }
    else {
      information.region = "PAL";
    }
  }

  /* Some boards define memory deeper in the BML document, and in these cases
     it is loaded elsewhere. Loading it twice will break things, so a shallow
     search is necessary here.
  */
  std::vector<std::string> boardmem = BML::searchListShallow(board, "board", "memory");

  for (std::string& m : boardmem) {
    std::string type = BML::search(m, {"memory", "type"});
    std::string content = BML::search(m, {"memory", "content"});
    //memory(type=ROM,content=Program)
    if (type == "ROM" && (content == "Program" || content == "Expansion")) {
      loadMemory(rom, m);
      for (std::string map : BML::searchList(m, "map")) {
        loadMap(map, rom);
      }
    }
    //ram(type=RAM,content=Save)
    else if (type == "RAM" && content == "Save") {
      loadMemory(ram, m);
      for (std::string map : BML::searchList(m, "map")) {
        loadMap(map, ram);
      }
    }
  }

  std::vector<std::string> processors = BML::searchList(board, "processor");
  for (std::string& p : processors) {
    std::string arch = BML::search(p, {"processor", "architecture"});
    std::string id = BML::search(p, {"processor", "identifier"});

    //processor(identifier=ICD)
    if (id == "ICD") {
      has.GameBoySlot = true;
      has.ICD = true;

      if(game.oscillatorList.size()) {
        icd.Frequency = game.oscillatorList[0].frequency;
      } else {
        icd.Frequency = 0;
      }

      //Game Boy core loads data through ICD interface
      for (std::string map : BML::searchList(p, "map")) {
        loadMap(map, {&ICD::readIO, &icd}, {&ICD::writeIO, &icd});
      }
    }
    //processor(identifier=MCC)
    else if (id == "MCC") {
      has.MCC = true;

      std::string pmap = BML::searchNode(p, {"processor", "map"});

      if (!pmap.empty()) {
        loadMap(pmap, {&MCC::read, &mcc}, {&MCC::write, &mcc});
      }

      std::string mcu = BML::searchNode(p, {"processor", "mcu"});

      if (!mcu.empty()) {
        for (std::string map : BML::searchList(mcu, "map")) {
          loadMap(map, {&MCC::mcuRead, &mcc}, {&MCC::mcuWrite, &mcc});
        }

        for (std::string& m : BML::searchList(mcu, "memory")) {
          std::string type = BML::search(m, {"memory", "type"});
          std::string content = BML::search(m, {"memory", "content"});
          if (type == "ROM" && content == "Program") {
            loadMemory(mcc.rom, m);
          }
          else if (type == "RAM" && content == "Download") {
            loadMemory(mcc.psram, m);
          }
        }

        for (std::string& s : BML::searchList(mcu, "slot")) {
          loadBSMemory(s);
        }
      }
    }

    //processor(architecture=W65C816S)
    if (arch == "W65C816S") {
      has.SA1 = true;

      std::string pmap = BML::searchNode(p, {"processor", "map"});

      if (!pmap.empty()) {
        loadMap(pmap, {&SA1::readIOCPU, &sa1}, {&SA1::writeIOCPU, &sa1});
      }

      std::string mcu = BML::searchNode(p, {"processor", "mcu"});

      if (!mcu.empty()) {
        for (std::string map : BML::searchList(mcu, "map")) {
          loadMap(map, {&SA1::ROM::readCPU, &sa1.rom}, {&SA1::ROM::writeCPU, &sa1.rom});
        }

        for (std::string& m : BML::searchList(mcu, "memory")) {
          if (BML::search(m, {"memory", "type"}) == "ROM" &&
              BML::search(m, {"memory", "content"}) == "Program") {
            loadMemory(sa1.rom, m);
          }
        }

        std::string slot = BML::searchNode(mcu, {"slot"});

        if (!slot.empty() && BML::search(slot, {"type"}) == "BSMemory") {
          loadBSMemory(slot);
        }
      }

      for (std::string& m : BML::searchList(p, "memory")) {
        if (BML::search(m, {"memory", "type"}) == "RAM") {
          std::string content = BML::search(m, {"memory", "content"});
          if (content == "Save") {
            loadMemory(sa1.bwram, m);
            for (std::string map : BML::searchList(m, "map")) {
              loadMap(map, {&SA1::BWRAM::readCPU, &sa1.bwram}, {&SA1::BWRAM::writeCPU, &sa1.bwram});
            }
          }
          else if (content == "Internal") {
            loadMemory(sa1.iram, m);
            for (std::string map : BML::searchList(m, "map")) {
              loadMap(map, {&SA1::IRAM::readCPU, &sa1.iram}, {&SA1::IRAM::writeCPU, &sa1.iram});
            }
          }
        }
      }
    }
    //processor(architecture=GSU)
    else if (arch == "GSU") {
      has.SuperFX = true;

      if(game.oscillatorList.size()) {
        superfx.Frequency = game.oscillatorList[0].frequency; //GSU-1, GSU-2
      } else {
        superfx.Frequency = system.cpuFrequency();  //MARIO CHIP 1
      }

      std::string pmap = BML::searchNode(p, {"processor", "map"});

      if (!pmap.empty()) {
        loadMap(pmap, {&SuperFX::readIO, &superfx}, {&SuperFX::writeIO, &superfx});
      }

      for (std::string& m : BML::searchList(p, "memory")) {
        std::string type = BML::search(m, {"memory", "type"});
        std::string content = BML::search(m, {"memory", "content"});
        if (type == "ROM" && content == "Program") {
          loadMemory(superfx.rom, m);
          for (std::string& c : BML::searchList(m, "map")) {
            loadMap(c, superfx.cpurom);
          }
        }
        else if (type == "RAM" && content == "Save") {
          loadMemory(superfx.ram, m);
          for (std::string& c : BML::searchList(m, "map")) {
            loadMap(c, superfx.cpuram);
          }
        }
      }
    }
    //processor(architecture=ARM6)
    else if (arch == "ARM6") {
      has.ARMDSP = true;

      for(uint8_t& word : armdsp.programROM) word = 0x00;
      for(uint8_t& word : armdsp.dataROM) word = 0x00;
      for(uint8_t& word : armdsp.programRAM) word = 0x00;

      if(game.oscillatorList.size()) {
        armdsp.Frequency = game.oscillatorList[0].frequency;
      } else {
        armdsp.Frequency = 21440000;
      }

      std::string pmap = BML::searchNode(p, {"processor", "map"});

      if (!pmap.empty()) {
        loadMap(pmap, {&ArmDSP::read, &armdsp}, {&ArmDSP::write, &armdsp});
      }

      for (std::string& m : BML::searchList(p, "memory")) {
        std::string type = BML::search(m, {"memory", "type"});
        std::string content = BML::search(m, {"memory", "content"});

        if (type == "ROM") {
          if (content == "Program" ) {
            Game::Memory file;
            if(game.memory(file, m)) {
              std::ifstream firmfile = openCallback(ID::SuperFamicom, file.name());
              if (firmfile.is_open()) {
                firmfile.read((char*)armdsp.programROM, (128 * 1024));
                firmfile.close();
              }
            }
          }
          else if (content == "Data") {
            Game::Memory file;
            if(game.memory(file, m)) {
              std::ifstream firmfile = openCallback(ID::SuperFamicom, file.name());
              if (firmfile.is_open()) {
                firmfile.read((char*)armdsp.dataROM, (32 * 1024));
                firmfile.close();
              }
            }
          }
        }
        else if (type == "RAM" && content == "Data") {
          Game::Memory file;
          if(game.memory(file, m)) {
            std::ifstream sramfile = openCallback(ID::SuperFamicom, "save.ram");
            if (sramfile.is_open()) {
              sramfile.read((char*)armdsp.programRAM, (16 * 1024));
              sramfile.close();
            }
          }
        }
      }
    }
    //processor(architecture=HG51BS169)
    else if (arch == "HG51BS169") {
      for(unsigned& word : hitachidsp.dataROM) word = 0x000000;
      for(uint8_t& word : hitachidsp.dataRAM) word = 0x00;

      if(game.oscillatorList.size()) {
        hitachidsp.Frequency = game.oscillatorList[0].frequency;
      } else {
        hitachidsp.Frequency = 20000000;
      }
      hitachidsp.Roms = game.board.find("2DC") != std::string::npos ? 2 : 1; //1 or 2
      hitachidsp.Mapping = 0; //0 or 1

      std::vector<std::string> memlist = BML::searchList(p, "memory");

      for (std::string& m : memlist) {
        std::string type = BML::search(m, {"memory", "type"});
        std::string content = BML::search(m, {"memory", "content"});
        if (type == "ROM" && content == "Program") {
          loadMemory(hitachidsp.rom, m);
          for (std::string map : BML::searchList(m, "map")) {
            loadMap(map, {&HitachiDSP::readROM, &hitachidsp}, {&HitachiDSP::writeROM, &hitachidsp});
          }
        }
        else if (type == "RAM" && content == "Save") {
          loadMemory(hitachidsp.ram, m);
          for (std::string map : BML::searchList(m, "map")) {
            loadMap(map, {&HitachiDSP::readRAM, &hitachidsp}, {&HitachiDSP::writeRAM, &hitachidsp});
          }
        }
      }

      if(configuration.coprocessor.preferHLE) {
        has.Cx4 = true;
        std::string pmap = BML::searchNode(p, {"processor", "map"});

        if (!pmap.empty()) {
          loadMap(pmap, {&Cx4::read, &cx4}, {&Cx4::write, &cx4});
        }

        for (std::string& m : memlist) {
          if (BML::search(m, {"memory", "type"}) == "RAM" &&
              BML::search(m, {"memory", "content"}) == "Data") {
            for (std::string map : BML::searchList(m, "map")) {
              loadMap(map, {&Cx4::read, &cx4}, {&Cx4::write, &cx4});
            }
          }
        }
      }
      else {
        for (std::string& m : memlist) {
          if (BML::search(m, {"memory", "content"}) == "Data") {
            std::string type = BML::search(m, {"memory", "type"});

            if (type == "ROM") {
              Game::Memory file;
              if(game.memory(file, m)) {
                for (int n = 0; n < 1024; ++n) {
                  hitachidsp.dataROM[n]  = hitachidspStaticDataROM[n * 3 + 0] <<  0;
                  hitachidsp.dataROM[n] |= hitachidspStaticDataROM[n * 3 + 1] <<  8;
                  hitachidsp.dataROM[n] |= hitachidspStaticDataROM[n * 3 + 2] << 16;
                }
              }
            }
            else if (type == "RAM") {
              for (std::string map : BML::searchList(m, "map")) {
                loadMap(map, {&HitachiDSP::readDRAM, &hitachidsp}, {&HitachiDSP::writeDRAM, &hitachidsp});
              }
            }
          }
        }

        has.HitachiDSP = true;

        std::string pmap = BML::searchNode(p, {"processor", "map"});

        if (!pmap.empty()) {
          loadMap(pmap, {&HitachiDSP::readIO, &hitachidsp}, {&HitachiDSP::writeIO, &hitachidsp});
        }
      }
    }
    //processor(architecture=uPD7725)
    else if (arch == "uPD7725") {
      for(unsigned& word : necdsp.programROM) word = 0x000000;
      for(uint16_t& word : necdsp.dataROM) word = 0x0000;
      for(uint16_t& word : necdsp.dataRAM) word = 0x0000;

      if(game.oscillatorList.size()) {
        necdsp.Frequency = game.oscillatorList[0].frequency;
      } else {
        necdsp.Frequency = 7600000;
      }

      std::vector<std::string> identifiers = BML::searchList(game.document, "identifier");
      std::string ident = identifiers[0].erase(0, identifiers[0].find_first_of(' ') + 1);
      ident.pop_back();
      std::transform(ident.begin(), ident.end(), ident.begin(), ::tolower);

      bool failed = false;

      std::ifstream prgfile = openCallback(ID::SuperFamicom,
        ident + ".program.rom");

      if (prgfile.is_open()) {
        for (unsigned i = 0; i < 2048; ++i) {
          necdsp.programROM[i] = prgfile.get() | (prgfile.get() << 8) |
            (prgfile.get() << 16);
        }
        prgfile.close();
      }
      else {
        failed = true;
      }

      std::ifstream datafile = openCallback(ID::SuperFamicom, ident + ".data.rom");

      if (datafile.is_open()) {
        for (unsigned i = 0; i < 1024; ++i) {
          necdsp.dataROM[i] = datafile.get() | (datafile.get() << 8);
        }
        datafile.close();
      }
      else {
        failed = true;
      }

      int foundDSP = 0;
      std::string pmap = BML::searchNode(p, {"processor", "map"});

      if (failed || configuration.coprocessor.preferHLE) {
        if (ident == "dsp1" || ident == "dsp1b") {
          has.DSP1 = true;
          if (!pmap.empty()) {
            loadMap(pmap, {&DSP1::read, &dsp1}, {&DSP1::write, &dsp1});
          }
	  foundDSP = 1;
        }
        else if (ident == "dsp2") {
          has.DSP2 = true;
          if (!pmap.empty()) {
            loadMap(pmap, {&DSP2::read, &dsp2}, {&DSP2::write, &dsp2});
          }
	  foundDSP = 1;
        }
        else if (ident == "dsp4") {
          has.DSP4 = true;
          if (!pmap.empty()) {
            loadMap(pmap, {&DSP4::read, &dsp4}, {&DSP4::write, &dsp4});
          }
	  foundDSP = 1;
        }
      }

      if (!foundDSP) {
        for (std::string& m : BML::searchList(p, "memory")) {
          if (BML::search(m, {"memory", "type"}) == "RAM" &&
              BML::search(m, {"memory", "content"}) == "Data") {
            std::ifstream sramfile = openCallback(ID::SuperFamicom, "save.ram");
            if (sramfile.is_open()) {
              sramfile.read((char*)necdsp.dataRAM, 2 * 256);
              sramfile.close();
            }
          }
        }

        has.NECDSP = true;
        necdsp.revision = NECDSP::Revision::uPD7725;

        if (!pmap.empty()) {
          loadMap(pmap, {&NECDSP::read, &necdsp}, {&NECDSP::write, &necdsp});
        }
      }
    }
   //processor(architecture=uPD96050)
    else if (arch == "uPD96050") {
      for(unsigned& word : necdsp.programROM) word = 0x000000;
      for(uint16_t& word : necdsp.dataROM) word = 0x0000;
      for(uint16_t& word : necdsp.dataRAM) word = 0x0000;

      if(game.oscillatorList.size()) {
        necdsp.Frequency = game.oscillatorList[0].frequency;
      } else {
        necdsp.Frequency = 11000000;
      }

      std::vector<std::string> identifiers = BML::searchList(game.document, "identifier");
      std::string ident = identifiers[0].erase(0, identifiers[0].find_first_of(' ') + 1);
      ident.pop_back();
      std::transform(ident.begin(), ident.end(), ident.begin(), ::tolower);

      bool failed = false;

      std::ifstream prgfile = openCallback(ID::SuperFamicom,
        ident + ".program.rom");

      if (prgfile.is_open()) {
        for (unsigned i = 0; i < 16384; ++i) {
          necdsp.programROM[i] = prgfile.get() | (prgfile.get() << 8) |
            (prgfile.get() << 16);
        }
        prgfile.close();
      }
      else {
        failed = true;
      }

      std::ifstream datafile = openCallback(ID::SuperFamicom, ident + ".data.rom");
      if (datafile.is_open()) {
        for (unsigned i = 0; i < 2048; ++i) {
          necdsp.dataROM[i] = datafile.get() | (datafile.get() << 8);
        }
        datafile.close();
      }
      else {
        failed = true;
      }

      if(failed || configuration.coprocessor.preferHLE) {
        if (ident == "st010") {
          has.ST0010 = true;
          for (std::string& m : BML::searchList(p, "memory")) {
            if (BML::search(m, {"memory", "type"}) == "RAM" &&
                BML::search(m, {"memory", "content"}) == "Data") {
              std::string mem = BML::searchNode(m, {"memory", "map"});
              loadMap(mem, {&ST0010::read, &st0010}, {&ST0010::write, &st0010});
            }
          }
        }
      }
      else {
        for (std::string& m : BML::searchList(p, "memory")) {
          if (BML::search(m, {"memory", "type"}) == "RAM" &&
              BML::search(m, {"memory", "content"}) == "Data") {
            std::ifstream sramfile = openCallback(ID::SuperFamicom, "save.ram");
            if (sramfile.is_open()) {
              for (unsigned i = 0; i < 2048; ++i) {
                necdsp.dataRAM[i] = sramfile.get() | (sramfile.get() << 8);
              }
              sramfile.close();
            }
            for (std::string map : BML::searchList(m, "map")) {
              loadMap(map, {&NECDSP::readRAM, &necdsp}, {&NECDSP::writeRAM, &necdsp});
            }
          }
        }

        has.NECDSP = true;
        necdsp.revision = NECDSP::Revision::uPD96050;
        std::string pmap = BML::searchNode(p, {"processor", "map"});

        if (!pmap.empty()) {
          loadMap(pmap, {&NECDSP::read, &necdsp}, {&NECDSP::write, &necdsp});
        }
      }
    }

    //processor(identifier=SPC7110)
    if (id == "SPC7110") {
      has.SPC7110 = true;

      for (std::string& m : BML::searchListShallow(p, "processor", "map")) {
        loadMap(m, {&SPC7110::read, &spc7110}, {&SPC7110::write, &spc7110});
      }

      std::string mcu = BML::searchNode(p, {"processor", "mcu"});

      if (!mcu.empty()) {
        for (std::string& m : BML::searchList(mcu, "map")) {
          loadMap(m, {&SPC7110::mcuromRead, &spc7110}, {&SPC7110::mcuromWrite, &spc7110});
        }

        for (std::string& m : BML::searchList(mcu, "memory")) {
          if (BML::search(m, {"memory", "type"}) == "ROM") {
            std::string content = BML::search(m, {"memory", "content"});
            if (content == "Program") {
              loadMemory(spc7110.prom, m);
            }
            else if (content == "Data") {
              loadMemory(spc7110.drom, m);
            }
          }
        }
      }

      std::string sram = BML::searchNode(p, {"processor", "memory"});

      if (!sram.empty()) {
        if (BML::search(sram, {"memory", "type"}) == "RAM" &&
            BML::search(sram, {"memory", "content"}) == "Save") {
          loadMemory(spc7110.ram, sram);
          for (std::string& m : BML::searchList(sram, "map")) {
            loadMap(m, {&SPC7110::mcuramRead, &spc7110}, {&SPC7110::mcuramWrite, &spc7110});
          }
        }
      }
    }
    //processor(identifier=SDD1)
    else if (id == "SDD1") {
      has.SDD1 = true;

      std::string pmap = BML::searchNode(p, {"processor", "map"});

      if (!pmap.empty()) {
        loadMap(pmap, {&SDD1::ioRead, &sdd1}, {&SDD1::ioWrite, &sdd1});
      }

      std::string mcu = BML::searchNode(p, {"processor", "mcu"});

      if (!mcu.empty()) {
        for (std::string map : BML::searchList(mcu, "map")) {
          loadMap(map, {&SDD1::mcuRead, &sdd1}, {&SDD1::mcuWrite, &sdd1});
        }

        for (std::string& m : BML::searchList(mcu, "memory")) {
          if (BML::search(m, {"memory", "type"}) == "ROM" &&
              BML::search(m, {"memory", "content"}) == "Program") {
            loadMemory(sdd1.rom, m);
          }
        }
      }
    }
    //processor(identifier=OBC1)
    else if (id == "OBC1") {
      has.OBC1 = true;

      for (std::string map : BML::searchList(p, "map")) {
        loadMap(map, {&OBC1::read, &obc1}, {&OBC1::write, &obc1});
      }

      for (std::string m : BML::searchList(p, "memory")) {
        if (BML::search(m, {"memory", "type"}) == "RAM" &&
            BML::search(m, {"memory", "content"}) == "Save") {
          loadMemory(obc1.ram, m);
        }
      }
    }
  }

  std::vector<std::string> slots = BML::searchList(board, "slot");
  for (size_t i = 0; i < slots.size(); ++i) {
    std::string id = BML::search(slots[i], {"slot", "type"});
    if (id == "BSMemory") {
      loadBSMemory(slots[i]);
    }
    else if (id == "SufamiTurbo") {
      //slot(type=SufamiTurbo)[0]
      if (i == 0 && romCallback(ID::SufamiTurboA)) {
        sufamiturboA.pathID = ID::SufamiTurboA;
        if (!slotSufamiTurboA.document.empty()) {
          slotSufamiTurboA.load(slotSufamiTurboA.document);
          for (std::string& m : BML::searchList(slotSufamiTurboA.document, "memory")) {
            std::string type = BML::search(m, {"memory", "type"});
            std::string content = BML::search(m, {"memory", "content"});
            if (type == "ROM" && content == "Program") {
              if(Game::Memory memory = Game::Memory(m)) {
                sufamiturboA.rom.allocate(memory.size);
                for (unsigned n = 0; n < memory.size; ++n) {
                  sufamiturboA.rom.data()[n] = slotSufamiTurboA.prgrom.data()[n];
                }
              }
            }
            else if (type == "RAM" && content == "Save") {
              if(Game::Memory memory = Game::Memory(m)) {
                sufamiturboA.ram.allocate(memory.size);
                std::ifstream sramfile =
                  openCallback(sufamiturboA.pathID, "save.ram");
                if (sramfile.is_open()) {
                  sramfile.read((char*)sufamiturboA.ram.data(), memory.size);
                  sramfile.close();
                }
              }
            }
          }
        }
        loadMap(BML::searchNode(slots[i], {"slot", "rom", "map"}), sufamiturboA.rom);
        loadMap(BML::searchNode(slots[i], {"slot", "ram", "map"}), sufamiturboA.ram);
      }
      //slot(type=SufamiTurbo)[1]
      else if (i == 1 && romCallback(ID::SufamiTurboB)) {
        sufamiturboB.pathID = ID::SufamiTurboB;
        if (!slotSufamiTurboB.document.empty()) {
          slotSufamiTurboB.load(slotSufamiTurboB.document);
          for (std::string& m : BML::searchList(slotSufamiTurboB.document, "memory")) {
            std::string type = BML::search(m, {"memory", "type"});
            std::string content = BML::search(m, {"memory", "content"});
            if (type == "ROM" && content == "Program") {
              if(Game::Memory memory = Game::Memory(m)) {
                sufamiturboB.rom.allocate(memory.size);
                for (unsigned n = 0; n < memory.size; ++n) {
                  sufamiturboB.rom.data()[n] = slotSufamiTurboB.prgrom.data()[n];
                }
              }
            }
            else if (type == "RAM" && content == "Save") {
              if(Game::Memory memory = Game::Memory(m)) {
                sufamiturboB.ram.allocate(memory.size);
                std::ifstream sramfile =
                  openCallback(sufamiturboB.pathID, "save.ram");
                if (sramfile.is_open()) {
                  sramfile.read((char*)sufamiturboB.ram.data(), memory.size);
                  sramfile.close();
                }
              }
            }
          }
        }
        loadMap(BML::searchNode(slots[i], {"slot", "rom", "map"}), sufamiturboB.rom);
        loadMap(BML::searchNode(slots[i], {"slot", "ram", "map"}), sufamiturboB.ram);
      }
    }
  }

  //dip
  if (BML::exists(board, {"board", "processor", "dip"})) {
    has.DIP = true;
    // Multi-game carts (Campus Challenge '92, PowerFest '94) were no longer
    // supported after higan v106, and in bsnes standalone this always returns 0.
    //dip.value = platform->dipSettings(board);
    dip.value = 0;

    for (std::string map : BML::searchList(board, "map")) {
      loadMap(map, {&DIP::read, &dip}, {&DIP::write, &dip});
    }
  }

  std::vector<std::string> rtclist = BML::searchList(board, "rtc");
  for (std::string& rtc : rtclist) {
    std::string manufacturer = BML::search(rtc, {"rtc", "manufacturer"});
    //rtc(manufacturer=Epson)
    if (manufacturer == "Epson") {
      has.EpsonRTC = true;

      epsonrtc.initialize();

      std::string map = BML::searchNode(rtc, {"rtc", "map"});
      loadMap(map, {&EpsonRTC::read, &epsonrtc}, {&EpsonRTC::write, &epsonrtc});

      Game::Memory file;

      if(game.memory(file, BML::searchNode(rtc, {"rtc", "memory"}))) {
        std::ifstream rtcfile = openCallback(ID::SuperFamicom, "time.rtc");
        if (rtcfile.is_open()) {
          uint8_t data[16] = {0};
          for(uint8_t& byte : data) byte = rtcfile.get();
          epsonrtc.load(data);
          rtcfile.close();
        }
      }
    }
    //rtc(manufacturer=Sharp)
    else if (manufacturer == "Sharp") {
      has.SharpRTC = true;

      sharprtc.initialize();

      std::string map = BML::searchNode(rtc, {"rtc", "map"});
      loadMap(map, {&SharpRTC::read, &sharprtc}, {&SharpRTC::write, &sharprtc});

      Game::Memory file;

      if(game.memory(file, BML::searchNode(rtc, {"rtc", "memory"}))) {
        std::ifstream rtcfile = openCallback(ID::SuperFamicom, "time.rtc");
        if (rtcfile.is_open()) {
          uint8_t data[16] = {0};
          for(uint8_t& byte : data) byte = rtcfile.get();
          sharprtc.load(data);
          rtcfile.close();
        }
      }
    }
  }

  std::vector<uint8_t> msu1file;
  if (openFileCallback(ID::SuperFamicom, "msu1/data.rom", msu1file)) {
    msu1file.clear();
    has.MSU1 = true;
    bus.map({&MSU1::readIO, &msu1}, {&MSU1::writeIO, &msu1}, "00-3f,80-bf:2000-2007");
  }

  //Game Boy
  if(cartridge.has.ICD) {
    information.sha256 = "";  //Game Boy cartridge not loaded yet: set later via loadGameBoy()
  }

  //BS Memory
  else if(cartridge.has.MCC && cartridge.has.BSMemorySlot) {
    information.sha256 = sha256_digest(bsmemory.data(), bsmemory.size());
  }

  //Sufami Turbo
  else if(cartridge.has.SufamiTurboSlotA || cartridge.has.SufamiTurboSlotB) {
    std::vector<uint8_t> carts;

    // Ugly, but works during the nall removal, needs to change when nall is gone
    if(cartridge.has.SufamiTurboSlotA) {
      for (size_t i = 0; i < sufamiturboA.rom.size(); ++i)
        carts.push_back(sufamiturboA.rom[i]);
    }
    if(cartridge.has.SufamiTurboSlotB) {
      for (size_t i = 0; i < sufamiturboB.rom.size(); ++i)
        carts.push_back(sufamiturboB.rom[i]);
    }

    information.sha256 = sha256_digest(carts.data(), carts.size());
  }

  //Super Famicom
  else {
    // Again ugly, but can/should be rewritten cleanly when nall is gone
    std::vector<uint8_t> buf;

    //hash each ROM image that exists; any with size() == 0 is ignored
    for (size_t i = 0; i < rom.size(); ++i) buf.push_back(rom[i]);
    for (size_t i = 0; i < mcc.rom.size(); ++i) buf.push_back(mcc.rom[i]);
    for (size_t i = 0; i < sa1.rom.size(); ++i) buf.push_back(sa1.rom[i]);
    for (size_t i = 0; i < superfx.rom.size(); ++i) buf.push_back(superfx.rom[i]);
    for (size_t i = 0; i < hitachidsp.rom.size(); ++i) buf.push_back(hitachidsp.rom[i]);
    for (size_t i = 0; i < spc7110.prom.size(); ++i) buf.push_back(spc7110.prom[i]);
    for (size_t i = 0; i < spc7110.drom.size(); ++i) buf.push_back(spc7110.drom[i]);
    for (size_t i = 0; i < sdd1.rom.size(); ++i) buf.push_back(sdd1.rom[i]);

    //hash all firmware that exists
    std::vector<uint8_t> firm;
    if(cartridge.has.ARMDSP) {
      firm = armdsp.firmware();
      for (size_t i = 0; i < firm.size(); ++i) buf.push_back(firm[i]);
    }

    if(cartridge.has.HitachiDSP) {
      firm = hitachidsp.firmware();
      for (size_t i = 0; i < firm.size(); ++i) buf.push_back(firm[i]);
    }

    if(cartridge.has.NECDSP) {
      firm = necdsp.firmware();
      for (size_t i = 0; i < firm.size(); ++i) buf.push_back(firm[i]);
    }

    //finalize hash
    information.sha256 = sha256_digest(buf.data(), buf.size());
  }

  return true;
}

void Cartridge::save() {
  std::vector<std::string> boardmem = BML::searchListShallow(board, "board", "memory");
  for (std::string& m : boardmem) {
    if (BML::search(m, {"memory", "type"}) == "RAM" &&
        BML::search(m, {"memory", "content"}) == "Save") {
      saveMemory(ram, m);
    }
  }

  std::vector<std::string> processors = BML::searchList(board, "processor");
  for (std::string& p : processors) {
    std::string arch = BML::search(p, {"processor", "architecture"});
    std::string id = BML::search(p, {"processor", "identifier"});

    //processor(identifier=MCC)
    if (id == "MCC") {
      std::string mcu = BML::searchNode(p, {"processor", "mcu"});
      if (!mcu.empty()) {
        std::vector<std::string> mcumem = BML::searchList(mcu, "memory");
        for (std::string& m : mcumem) {
          if (BML::search(m, {"memory", "type"}) == "RAM" &&
              BML::search(m, {"memory", "content"}) == "Download") {
            saveMemory(mcc.psram, m);
          }
        }
      }
    }
    //processor(identifier=SPC7110)
    else if (id == "SPC7110") {
      std::string sram = BML::searchNode(p, {"processor", "memory"});
      if (!sram.empty()) {
        if (BML::search(sram, {"memory", "type"}) == "RAM" &&
            BML::search(sram, {"memory", "content"}) == "Save") {
          saveMemory(spc7110.ram, sram);
        }
      }
    }
    //processor(identifier=OBC1)
    else if (id == "OBC1") {
      for (std::string& m : BML::searchList(p, "memory")) {
        if (BML::search(m, {"memory", "type"}) == "RAM" &&
            BML::search(m, {"memory", "content"}) == "Save") {
          saveMemory(obc1.ram, m);
        }
      }
    }

    //processor(architecture=W65C816S)
    if (arch == "W65C816S") {
      for (std::string& m : BML::searchList(p, "memory")) {
        if (BML::search(m, {"memory", "type"}) == "RAM") {
          std::string content = BML::search(m, {"memory", "content"});
          if (content == "Save")
            saveMemory(sa1.bwram, m);
          else if (content == "Internal")
            saveMemory(sa1.iram, m);
        }
      }
    }
    //processor(architecture=GSU)
    else if (arch == "GSU") {
      for (std::string& m : BML::searchList(p, "memory")) {
        if (BML::search(m, {"memory", "type"}) == "RAM" &&
            BML::search(m, {"memory", "content"}) == "Save") {
          saveMemory(superfx.ram, m);
        }
      }
    }
    //processor(architecture=ARM6)
    else if (arch == "ARM6") {
      for (std::string& m : BML::searchList(p, "memory")) {
        if (BML::search(m, {"memory", "type"}) == "RAM" &&
            BML::search(m, {"memory", "content"}) == "Data" &&
            BML::search(m, {"memory", "architecture"}) == "ARM6") {
          Game::Memory file;
          if (game.memory(file, m) && file.nonVolatile) {
            writeCallback(ID::SuperFamicom, "save.ram", armdsp.programRAM, (16 * 1024));
          }
        }
      }
    }
    //processor(architecture=HG51BS169)
    else if (arch == "HG51BS169") {
      for (std::string& m : BML::searchList(p, "memory")) {
        if (BML::search(m, {"memory", "type"}) == "RAM") {
          std::string content = BML::search(m, {"memory", "content"});
          if (content == "Data" && BML::search(m, {"memory", "architecture"}) == "HG51BS169") {
            Game::Memory file;
            if (game.memory(file, m) && file.nonVolatile) {
              writeCallback(ID::SuperFamicom, "save.ram", hitachidsp.dataRAM, (3 * 1024));
            }
          }
          else if (content == "Save") {
            writeCallback(ID::SuperFamicom, "save.ram", hitachidsp.ram.data(), hitachidsp.ram.size());
          }
        }
      }
    }
    //processor(architecture=uPD7725)
    else if (arch == "uPD7725") {
      for (std::string& m : BML::searchList(p, "memory")) {
        if (BML::search(m, {"memory", "type"}) == "RAM" &&
            BML::search(m, {"memory", "content"}) == "Data" &&
            BML::search(m, {"memory", "architecture"}) == "uPD7725") {
          Game::Memory file;
          if (game.memory(file, m) && file.nonVolatile) {
            writeCallback(ID::SuperFamicom, "save.ram", (uint8_t*)necdsp.dataRAM, 2 * 256);
          }
        }
      }
    }
    //processor(architecture=uPD96050)
    else if (arch == "uPD96050") {
      for (std::string& m : BML::searchList(p, "memory")) {
        if (BML::search(m, {"memory", "type"}) == "RAM" &&
            BML::search(m, {"memory", "content"}) == "Data" &&
            BML::search(m, {"memory", "architecture"}) == "uPD96050") {
          Game::Memory file;
          if (game.memory(file, m) && file.nonVolatile) {
            writeCallback(ID::SuperFamicom, "save.ram", (uint8_t*)necdsp.dataRAM, (4 * 1024));
          }
        }
      }
    }
  }

  std::vector<std::string> rtclist = BML::searchList(board, "rtc");
  for (std::string& rtc : rtclist) {
    std::string manufacturer = BML::search(rtc, {"rtc", "manufacturer"});
    //rtc(manufacturer=Epson)
    if (manufacturer == "Epson") {
      std::string memory = BML::searchNode(rtc, {"rtc", "memory"});
      Game::Memory file;
      if (game.memory(file, memory) && file.nonVolatile) {
        uint8_t data[16] = {0};
        epsonrtc.save(data);
        writeCallback(ID::SuperFamicom, "time.rtc", data, 16);
      }
    }
    //rtc(manufacturer=Sharp)
    else if (manufacturer == "Sharp") {
      std::string memory = BML::searchNode(rtc, {"rtc", "memory"});
      Game::Memory file;
      if (game.memory(file, memory) && file.nonVolatile) {
        uint8_t data[16] = {0};
        sharprtc.save(data);
        writeCallback(ID::SuperFamicom, "time.rtc", data, 16);
      }
    }
  }

  if(has.GameBoySlot) {
    icd.save();
  }
  if(has.BSMemorySlot) {
    for (std::string& m : BML::searchList(slotBSMemory.document, "memory")) {
      if (BML::search(m, {"memory", "type"}) == "Flash" &&
          BML::search(m, {"memory", "content"}) == "Program") {
        if(Game::Memory memory = Game::Memory(m)) {
          if (bsmemory.memory.data() != nullptr) {
            writeCallback(bsmemory.pathID, memory.name(), bsmemory.memory.data(), memory.size);
          }
        }
      }
    }
  }
  if(has.SufamiTurboSlotA) {
    for (std::string& m : BML::searchList(slotSufamiTurboA.document, "memory")) {
      if (BML::search(m, {"memory", "type"}) == "RAM" &&
          BML::search(m, {"memory", "content"}) == "Save") {
        if(Game::Memory memory = Game::Memory(m)) {
          if (memory.nonVolatile) {
            writeCallback(sufamiturboA.pathID, memory.name(), sufamiturboA.ram.data(), memory.size);
          }
        }
      }
    }
  }
  if(has.SufamiTurboSlotB) {
    for (std::string& m : BML::searchList(slotSufamiTurboB.document, "memory")) {
      if (BML::search(m, {"memory", "type"}) == "RAM" &&
          BML::search(m, {"memory", "content"}) == "Save") {
        if(Game::Memory memory = Game::Memory(m)) {
          if (memory.nonVolatile) {
            writeCallback(sufamiturboB.pathID, memory.name(), sufamiturboB.ram.data(), memory.size);
          }
        }
      }
    }
  }
}

void Cartridge::setRegion(std::string region) {
    forceRegion = region;
}

void Cartridge::setRomBSMemory(std::vector<uint8_t>& data, std::string& loc) {
  slotBSMemory.prgrom = data;
  Heuristics::BSMemory heuristics = Heuristics::BSMemory(data, loc);

  std::ifstream dbfile = openCallback(ID::System, "BSMemory.bml");

  std::string sha256 = sha256_digest(data.data(), data.size());
  std::string manifest = BML::gendoc(dbfile, "game", "sha256", sha256);
  dbfile.close();

  slotBSMemory.document = manifest.empty() ? heuristics.manifest() : manifest;
}

void Cartridge::setRomSufamiTurboA(std::vector<uint8_t>& data, std::string& loc) {
  slotSufamiTurboA.prgrom = data;
  Heuristics::SufamiTurbo heuristics = Heuristics::SufamiTurbo(data, loc);

  std::ifstream dbfile =
    openCallback(ID::System, "SufamiTurbo.bml");

  std::string sha256 = sha256_digest(data.data(), data.size());
  std::string manifest = BML::gendoc(dbfile, "game", "sha256", sha256);
  dbfile.close();

  slotSufamiTurboA.document =
    manifest.empty() ? heuristics.manifest() : manifest;

  has.SufamiTurboSlotA = true;
}

void Cartridge::setRomSufamiTurboB(std::vector<uint8_t>& data, std::string& loc) {
  slotSufamiTurboB.prgrom = data;
  Heuristics::SufamiTurbo heuristics = Heuristics::SufamiTurbo(data, loc);

  std::ifstream dbfile =
    openCallback(ID::System, "SufamiTurbo.bml");

  std::string sha256 = sha256_digest(data.data(), data.size());
  std::string manifest = BML::gendoc(dbfile, "game", "sha256", sha256);
  dbfile.close();

  slotSufamiTurboB.document =
    manifest.empty() ? heuristics.manifest() : manifest;

  has.SufamiTurboSlotB = true;
}

void Cartridge::setRomSuperFamicom(std::vector<uint8_t>& data, std::string& loc) {
  Heuristics::SuperFamicom heuristics = Heuristics::SuperFamicom(data, loc);

  std::ifstream dbfile =
    openCallback(ID::System, "SuperFamicom.bml");
  std::string sha256 = sha256_digest(data.data(), data.size());
  std::string manifest = BML::gendoc(dbfile, "game", "sha256", sha256);
  dbfile.close();

  if (manifest.empty()) {
    game.document = heuristics.manifest();
  }
  else {
    //the internal ROM header title is not present in the database, but
    //is needed for internal core overrides
    manifest += "  title: " + heuristics.title() + "\n";
    game.document = manifest;
  }

  if (heuristics.title() == "Satellaview BS-X" && data.size() >= 0x100000) {
    //BS-X: Sore wa Namae o Nusumareta Machi no Monogatari (JPN) (1.1)
    //disable limited play check for BS Memory flash cartridges
    //benefit: allow locked out BS Memory flash games to play without manual
    //header patching.
    //detriment: BS Memory ROM cartridges will cause the game to hang in the
    //load menu
    if (data[0x4a9b] == 0x10) data[0x4a9b] = 0x80;
    if (data[0x4d6d] == 0x10) data[0x4d6d] = 0x80;
    if (data[0x4ded] == 0x10) data[0x4ded] = 0x80;
    if (data[0x4e9a] == 0x10) data[0x4e9a] = 0x80;
  }

  unsigned offset = 0;
  if (unsigned size = heuristics.programRomSize()) {
    game.prgrom.resize(size);
    std::memcpy(&game.prgrom[0], &data[offset], size);
    offset += size;
  }
  if (unsigned size = heuristics.dataRomSize()) {
    game.datarom.resize(size);
    std::memcpy(&game.datarom[0], &data[offset], size);
    offset += size;
  }
  if (unsigned size = heuristics.expansionRomSize()) {
    game.exprom.resize(size);
    std::memcpy(&game.exprom[0], &data[offset], size);
    offset += size;
  }
}

void Cartridge::unload() {
  rom.reset();
  ram.reset();
}

void Cartridge::setOpenFileCallback(bool (*cb)(unsigned, std::string, std::vector<uint8_t>&)) {
  openFileCallback = cb;
}

void Cartridge::setOpenCallback(std::ifstream (*cb)(unsigned, std::string)) {
  openCallback = cb;
}

void Cartridge::setRomCallback(bool (*cb)(unsigned)) {
  romCallback = cb;
}

void Cartridge::setWriteCallback(void (*cb)(unsigned, std::string, const uint8_t*, unsigned)) {
  writeCallback = cb;
}

}
