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
#include <iterator>
#include <vector>

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

static Emulator::Game game;
static Emulator::Game slotGameBoy;
static Emulator::Game slotBSMemory;
static Emulator::Game slotSufamiTurboA;
static Emulator::Game slotSufamiTurboB;

unsigned Cartridge::pathID() const { return information.pathID; }
std::string Cartridge::region() const { return information.region; }
std::string Cartridge::headerTitle() const { return game.title; }

std::string Cartridge::loadBoard(std::string board) {
  if (board.find("SNSP-") == 0) board.replace(0, 5, "SHVC-");
  if (board.find("MAXI-") == 0) board.replace(0, 5, "SHVC-");
  if (board.find("MJSC-") == 0) board.replace(0, 5, "SHVC-");
  if (board.find("EA-") == 0) board.replace(0, 3, "SHVC-");
  if (board.find("WEI-") == 0) board.replace(0, 4, "SHVC-");

  std::ifstream boardsfile = Emulator::platform->fopen(ID::System, "boards.bml");
  if (boardsfile.is_open()) {
    std::string boards((std::istreambuf_iterator<char>(boardsfile)),
      (std::istreambuf_iterator<char>()));

    std::string bdoc = BML::searchBoard(boards, board);
    return bdoc;
  }

  return {};
}

void Cartridge::loadCartridge(std::string node) {
  board = BML::searchNode(node, {"board"});
  if (board.empty()) board = loadBoard(game.board);

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
    if (type == "ROM" && content == "Program") loadROM(m);
    if (type == "ROM" && content == "Expansion") loadROM(m); //todo: handle this better
    if (type == "RAM" && content == "Save") loadRAM(m);
  }

  std::vector<std::string> processors = BML::searchList(board, "processor");
  for (std::string& p : processors) {
    std::string arch = BML::search(p, {"processor", "architecture"});
    std::string id = BML::search(p, {"processor", "identifier"});
    if (id == "ICD") loadICD(p);
    if (id == "MCC") loadMCC(p);
    if (arch == "W65C816S") loadSA1(p);
    if (arch == "GSU") loadSuperFX(p);
    if (arch == "ARM6") loadARMDSP(p);
    if (arch == "HG51BS169") {
      loadHitachiDSP(p, game.board.find("2DC") != std::string::npos ? 2 : 1);
    }
    if (arch == "uPD7725") loaduPD7725(p);
    if (arch == "uPD96050") loaduPD96050(p);
    if (id == "SPC7110") loadSPC7110(p);
    if (id == "SDD1") loadSDD1(p);
    if (id == "OBC1") loadOBC1(p);
  }

  std::vector<std::string> slots = BML::searchList(board, "slot");
  for (size_t i = 0; i < slots.size(); ++i) {
    std::string type = BML::search(slots[i], {"slot", "type"});
    if (type == "BSMemory") loadBSMemory(slots[i]);
    if (type == "SufamiTurbo" && i == 0) loadSufamiTurboA(slots[i]);
    if (type == "SufamiTurbo" && i == 1) loadSufamiTurboB(slots[i]);
  }

  if (BML::exists(board, {"board", "processor", "dip"})) loadDIP(board);

  std::vector<std::string> rtclist = BML::searchList(board, "rtc");
  for (std::string& rtc : rtclist) {
    std::string manufacturer = BML::search(rtc, {"rtc", "manufacturer"});
    if (manufacturer == "Epson") loadEpsonRTC(rtc);
    if (manufacturer == "Sharp") loadSharpRTC(rtc);
  }

  std::ifstream msu1file =
    Emulator::platform->fopen(ID::SuperFamicom, "msu1/data.rom");
  if (msu1file.is_open()) {
    msu1file.close();
    loadMSU1();
  }
}

void Cartridge::loadCartridgeBSMemory(std::string node) {
  if (auto memory = Emulator::Game::Memory(BML::searchNode(node, {"game", "board", "memory"}))) {
    bsmemory.ROM = memory.type == "ROM";
    bsmemory.memory.allocate(memory.size);
    for (unsigned i = 0; i < memory.size; ++i) {
      bsmemory.memory.data()[i] = romdataBS.data()[i];
    }
  }
}

void Cartridge::loadCartridgeSufamiTurboA(std::string node) {
  std::vector<std::string> memlist = BML::searchList(node, "memory");
  for (std::string& m : memlist) {
    std::string type = BML::search(m, {"memory", "type"});
    std::string content = BML::search(m, {"memory", "content"});
    if (type == "ROM" && content == "Program") {
      if(auto memory = Emulator::Game::Memory(m)) {
        sufamiturboA.rom.allocate(memory.size);
        for (unsigned i = 0; i < memory.size; ++i) {
          sufamiturboA.rom.data()[i] = romdataSTA.data()[i];
        }
      }
    }
    else if (type == "RAM" && content == "Save") {
      if(auto memory = Emulator::Game::Memory(m)) {
        sufamiturboA.ram.allocate(memory.size);
        std::ifstream sramfile =
          Emulator::platform->fopen(sufamiturboA.pathID, "save.ram");
        if (sramfile.is_open()) {
          sramfile.read((char*)sufamiturboA.ram.data(), memory.size);
          sramfile.close();
        }
      }
    }
  }
}

void Cartridge::loadCartridgeSufamiTurboB(std::string node) {
  std::vector<std::string> memlist = BML::searchList(node, "memory");
  for (std::string& m : memlist) {
    std::string type = BML::search(m, {"memory", "type"});
    std::string content = BML::search(m, {"memory", "content"});
    if (type == "ROM" && content == "Program") {
      if(auto memory = Emulator::Game::Memory(m)) {
        sufamiturboB.rom.allocate(memory.size);
        for (unsigned i = 0; i < memory.size; ++i) {
          sufamiturboB.rom.data()[i] = romdataSTB.data()[i];
        }
      }
    }
    else if (type == "RAM" && content == "Save") {
      if(auto memory = Emulator::Game::Memory(m)) {
        sufamiturboB.ram.allocate(memory.size);
        std::ifstream sramfile =
          Emulator::platform->fopen(sufamiturboB.pathID, "save.ram");
        if (sramfile.is_open()) {
          sramfile.read((char*)sufamiturboB.ram.data(), memory.size);
          sramfile.close();
        }
      }
    }
  }
}

template<typename T>  //T = ReadableMemory, WritableMemory
unsigned Cartridge::loadMap(std::string map, T& memory) {
  auto addr = BML::search(map, {"map", "address"});
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
  auto addr = BML::search(map, {"map", "address"});
  std::string strsize = BML::search(map, {"map", "size"});
  std::string strbase = BML::search(map, {"map", "base"});
  std::string strmask = BML::search(map, {"map", "mask"});
  unsigned size = strsize.empty() ? 0 : std::stoi(strsize, nullptr, 16);
  unsigned base = strbase.empty() ? 0 : std::stoi(strbase, nullptr, 16);
  unsigned mask = strmask.empty() ? 0 : std::stoi(strmask, nullptr, 16);
  return bus.map(reader, writer, addr, size, base, mask);
}

void Cartridge::loadMemory(Memory& mem, std::string node) {
  if(auto memory = game.memory(node)) {
    mem.allocate(memory->size);
    if(memory->type == "RAM" && !memory->nonVolatile) return;
    if(memory->type == "RTC" && !memory->nonVolatile) return;

    if (memory->name() == "program.rom" || memory->name() == "data.rom" || memory->name() == "expansion.rom") {
      std::vector<uint8_t> buf = Emulator::platform->mopen(pathID(), memory->name());
      if (!buf.empty()) {
      for (unsigned i = 0; i < memory->size; ++i)
        mem.data()[i] = buf[i];
      }
      return;
    }

    std::ifstream memfile = Emulator::platform->fopen(pathID(), memory->name());
    if (memfile.is_open()) {
      memfile.seekg(0, memfile.end);
      unsigned fsize = memfile.tellg();
      memfile.seekg(0, memfile.beg);
      memfile.read((char*)mem.data(), std::min(fsize, mem.size()));
      memfile.close();
    }
  }
}

//memory(type=ROM,content=Program)
void Cartridge::loadROM(std::string node) {
  loadMemory(rom, node);
  std::vector<std::string> maps = BML::searchList(node, "map");
  for (auto map : maps) loadMap(map, rom);
}

void Cartridge::loadRAM(std::string node) {
  loadMemory(ram, node);
  std::vector<std::string> maps = BML::searchList(node, "map");
  for (auto map : maps) loadMap(map, ram);
}

//processor(identifier=ICD)
void Cartridge::loadICD(std::string node) {
  has.GameBoySlot = true;
  has.ICD = true;

  if(auto oscillator = game.oscillator()) {
    icd.Frequency = oscillator->frequency;
  } else {
    icd.Frequency = 0;
  }

  //Game Boy core loads data through ICD interface
  std::vector<std::string> maps = BML::searchList(node, "map");
  for (auto map : maps) {
    loadMap(map, {&ICD::readIO, &icd}, {&ICD::writeIO, &icd});
  }
}

//processor(identifier=MCC)
void Cartridge::loadMCC(std::string node) {
  has.MCC = true;

  std::string pmap = BML::searchNode(node, {"processor", "map"});
  if (!pmap.empty()) loadMap(pmap, {&MCC::read, &mcc}, {&MCC::write, &mcc});

  std::string mcu = BML::searchNode(node, {"processor", "mcu"});

  if (!mcu.empty()) {
    std::vector<std::string> maps = BML::searchList(mcu, "map");
    for (auto map : maps) {
      loadMap(map, {&MCC::mcuRead, &mcc}, {&MCC::mcuWrite, &mcc});
    }

    std::vector<std::string> mcumem = BML::searchList(mcu, "memory");
    for (std::string& m : mcumem) {
      std::string type = BML::search(m, {"memory", "type"});
      std::string content = BML::search(m, {"memory", "content"});
      if (type == "ROM" && content == "Program") loadMemory(mcc.rom, m);
      if (type == "RAM" && content == "Download") loadMemory(mcc.psram, m);
    }

    std::vector<std::string> mcuslot = BML::searchList(mcu, "slot");
    for (std::string& s : mcuslot) {
      loadBSMemory(s);
    }
  }
}

//slot(type=BSMemory)
void Cartridge::loadBSMemory(std::string node) {
  has.BSMemorySlot = true;

  if(auto loaded = Emulator::platform->load(ID::BSMemory, "BS Memory", "bs")) {
    bsmemory.pathID = loaded.pathID;
    loadBSMemory();

    std::vector<std::string> maps = BML::searchList(node, "map");
    for (auto map : maps) {
      loadMap(map, bsmemory);
    }
  }
}

//slot(type=SufamiTurbo)[0]
void Cartridge::loadSufamiTurboA(std::string node) {
  if(auto loaded = Emulator::platform->load(ID::SufamiTurboA, "Sufami Turbo", "st")) {
    sufamiturboA.pathID = loaded.pathID;
    loadSufamiTurboA();
    std::string rommap = BML::searchNode(node, {"slot", "rom", "map"});
    std::string rammap = BML::searchNode(node, {"slot", "ram", "map"});
    loadMap(rommap, sufamiturboA.rom);
    loadMap(rammap, sufamiturboA.ram);
  }
}

//slot(type=SufamiTurbo)[1]
void Cartridge::loadSufamiTurboB(std::string node) {
  if(auto loaded = Emulator::platform->load(ID::SufamiTurboB, "Sufami Turbo", "st")) {
    sufamiturboB.pathID = loaded.pathID;
    loadSufamiTurboB();
    std::string rommap = BML::searchNode(node, {"slot", "rom", "map"});
    std::string rammap = BML::searchNode(node, {"slot", "ram", "map"});
    loadMap(rommap, sufamiturboB.rom);
    loadMap(rammap, sufamiturboB.ram);
  }
}

//dip
void Cartridge::loadDIP(std::string node) {
  has.DIP = true;
  // Multi-game carts (Campus Challenge '92, PowerFest '94) were no longer
  // supported after higan v106, and in bsnes standalone this always returns 0.
  //dip.value = Emulator::platform->dipSettings(node);
  dip.value = 0;

  std::vector<std::string> maps = BML::searchList(node, "map");
  for (auto map : maps) {
    loadMap(map, {&DIP::read, &dip}, {&DIP::write, &dip});
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

//processor(architecture=W65C816S)
void Cartridge::loadSA1(std::string node) {
  has.SA1 = true;

  std::string pmap = BML::searchNode(node, {"processor", "map"});
  if (!pmap.empty()) {
      loadMap(pmap, {&SA1::readIOCPU, &sa1}, {&SA1::writeIOCPU, &sa1});
  }

  std::string mcu = BML::searchNode(node, {"processor", "mcu"});

  if (!mcu.empty()) {
    std::vector<std::string> maps = BML::searchList(mcu, "map");
    for (auto map : maps) {
      loadMap(map, {&SA1::ROM::readCPU, &sa1.rom}, {&SA1::ROM::writeCPU, &sa1.rom});
    }

    std::vector<std::string> mcumem = BML::searchList(mcu, "memory");
    for (std::string& m : mcumem) {
      std::string type = BML::search(m, {"memory", "type"});
      std::string content = BML::search(m, {"memory", "content"});
      if (type == "ROM" && content == "Program") loadMemory(sa1.rom, m);
    }

    std::string slot = BML::searchNode(mcu, {"slot"});
    if (!slot.empty()) {
      if (BML::search(slot, {"type"}) == "BSMemory") loadBSMemory(slot);
    }
  }

  std::vector<std::string> memlist = BML::searchList(node, "memory");
  for (std::string& m : memlist) {
    std::string type = BML::search(m, {"memory", "type"});
    std::string content = BML::search(m, {"memory", "content"});

    if (type == "RAM" && content == "Save") {
      loadMemory(sa1.bwram, m);
      std::vector<std::string> maps = BML::searchList(m, "map");
      for (auto map : maps) {
        loadMap(map, {&SA1::BWRAM::readCPU, &sa1.bwram}, {&SA1::BWRAM::writeCPU, &sa1.bwram});
      }
    }

    if (type == "RAM" && content == "Internal") {
      loadMemory(sa1.iram, m);
      std::vector<std::string> maps = BML::searchList(m, "map");
      for (auto map : maps) {
        loadMap(map, {&SA1::IRAM::readCPU, &sa1.iram}, {&SA1::IRAM::writeCPU, &sa1.iram});
      }
    }
  }
}

//processor(architecture=GSU)
void Cartridge::loadSuperFX(std::string node) {
  has.SuperFX = true;

  if(auto oscillator = game.oscillator()) {
    superfx.Frequency = oscillator->frequency;  //GSU-1, GSU-2
  } else {
    superfx.Frequency = system.cpuFrequency();  //MARIO CHIP 1
  }

  std::string pmap = BML::searchNode(node, {"processor", "map"});
  if (!pmap.empty()) {
      loadMap(pmap, {&SuperFX::readIO, &superfx}, {&SuperFX::writeIO, &superfx});
  }

  std::vector<std::string> sfxmem = BML::searchList(node, "memory");
  for (std::string& m : sfxmem) {
    std::string type = BML::search(m, {"memory", "type"});
    std::string content = BML::search(m, {"memory", "content"});
    if (type == "ROM" && content == "Program") {
      loadMemory(superfx.rom, m);
      std::vector<std::string> cpumaps = BML::searchList(m, "map");
      for (std::string& c : cpumaps) {
        loadMap(c, superfx.cpurom);
      }
    }
    if (type == "RAM" && content == "Save") {
      loadMemory(superfx.ram, m);
      std::vector<std::string> cpumaps = BML::searchList(m, "map");
      for (std::string& c : cpumaps) {
        loadMap(c, superfx.cpuram);
      }
    }
  }
}

//processor(architecture=ARM6)
void Cartridge::loadARMDSP(std::string node) {
  has.ARMDSP = true;

  for(auto& word : armdsp.programROM) word = 0x00;
  for(auto& word : armdsp.dataROM) word = 0x00;
  for(auto& word : armdsp.programRAM) word = 0x00;

  if(auto oscillator = game.oscillator()) {
    armdsp.Frequency = oscillator->frequency;
  } else {
    armdsp.Frequency = 21'440'000;
  }

  std::string pmap = BML::searchNode(node, {"processor", "map"});
  if (!pmap.empty()) loadMap(pmap, {&ArmDSP::read, &armdsp}, {&ArmDSP::write, &armdsp});

  std::vector<std::string> memlist = BML::searchList(node, "memory");
  for (std::string& m : memlist) {
    std::string type = BML::search(m, {"memory", "type"});
    std::string content = BML::search(m, {"memory", "content"});
    std::string arch = BML::search(m, {"memory", "architecture"});

    if (type == "ROM" && content == "Program" && arch == "ARM6") {
      if(auto file = game.memory(m)) {
        std::ifstream firmfile = Emulator::platform->fopen(ID::SuperFamicom, file->name());
        if (firmfile.is_open()) {
          firmfile.read((char*)armdsp.programROM, (128 * 1024));
          firmfile.close();
        }
      }
    }

    if (type == "ROM" && content == "Data" && arch == "ARM6") {
      if(auto file = game.memory(m)) {
        std::ifstream firmfile = Emulator::platform->fopen(ID::SuperFamicom, file->name());
        if (firmfile.is_open()) {
          firmfile.read((char*)armdsp.dataROM, (32 * 1024));
          firmfile.close();
        }
      }
    }

    if (type == "RAM" && content == "Data" && arch == "ARM6") {
      if(auto file = game.memory(m)) {
        std::ifstream sramfile = Emulator::platform->fopen(ID::SuperFamicom, "save.ram");
        if (sramfile.is_open()) {
          sramfile.read((char*)armdsp.programRAM, (16 * 1024));
          sramfile.close();
        }
      }
    }

  }
}

//processor(architecture=HG51BS169)
void Cartridge::loadHitachiDSP(std::string node, unsigned roms) {
  for(auto& word : hitachidsp.dataROM) word = 0x000000;
  for(auto& word : hitachidsp.dataRAM) word = 0x00;

  if(auto oscillator = game.oscillator()) {
    hitachidsp.Frequency = oscillator->frequency;
  } else {
    hitachidsp.Frequency = 20'000'000;
  }
  hitachidsp.Roms = roms;  //1 or 2
  hitachidsp.Mapping = 0;  //0 or 1

  std::vector<std::string> memlist = BML::searchList(node, "memory");

  for (std::string& m : memlist) {
    std::string type = BML::search(m, {"memory", "type"});
    std::string content = BML::search(m, {"memory", "content"});
    if (type == "ROM" && content == "Program") {
      loadMemory(hitachidsp.rom, m);
      std::vector<std::string> maps = BML::searchList(m, "map");
      for (std::string map : maps) {
        loadMap(map, {&HitachiDSP::readROM, &hitachidsp}, {&HitachiDSP::writeROM, &hitachidsp});
      }
    }
    if (type == "RAM" && content == "Save") {
      loadMemory(hitachidsp.ram, m);
      std::vector<std::string> maps = BML::searchList(m, "map");
      for (std::string map : maps) {
        loadMap(map, {&HitachiDSP::readRAM, &hitachidsp}, {&HitachiDSP::writeRAM, &hitachidsp});
      }
    }
  }

  if(configuration.coprocessor.preferHLE) {
    has.Cx4 = true;
    std::string pmap = BML::searchNode(node, {"processor", "map"});
    if (!pmap.empty()) {
      loadMap(pmap, {&Cx4::read, &cx4}, {&Cx4::write, &cx4});
    }

    for (std::string& m : memlist) {
      std::string type = BML::search(m, {"memory", "type"});
      std::string content = BML::search(m, {"memory", "content"});
      std::string arch = BML::search(m, {"memory", "architecture"});
      if (type == "RAM" && content == "Data" && arch == "HG51BS169") {
        std::vector<std::string> maps = BML::searchList(m, "map");
        for (std::string map : maps) {
          loadMap(map, {&Cx4::read, &cx4}, {&Cx4::write, &cx4});
        }
      }
    }
    return;
  }

  for (std::string& m : memlist) {
    std::string type = BML::search(m, {"memory", "type"});
    std::string content = BML::search(m, {"memory", "content"});
    std::string arch = BML::search(m, {"memory", "architecture"});
    if (type == "ROM" && content == "Data" && arch == "HG51BS169") {
      if(auto file = game.memory(m)) {
        for (int n = 0; n < 1024; ++n) {
          hitachidsp.dataROM[n]  = hitachidsp.staticDataROM[n * 3 + 0] <<  0;
          hitachidsp.dataROM[n] |= hitachidsp.staticDataROM[n * 3 + 1] <<  8;
          hitachidsp.dataROM[n] |= hitachidsp.staticDataROM[n * 3 + 2] << 16;
        }
      }
    }
    else if (type == "RAM" && content == "Data" && arch == "HG51BS169") {
      if(auto file = game.memory(m)) {
        std::ifstream sramfile = Emulator::platform->fopen(ID::SuperFamicom, "save.ram");
        if (sramfile.is_open()) {
          sramfile.read((char*)hitachidsp.dataRAM, (3 * 1024));
          sramfile.close();
        }
      }

      std::vector<std::string> maps = BML::searchList(m, "map");
      for (std::string map : maps) {
          loadMap(map, {&HitachiDSP::readDRAM, &hitachidsp}, {&HitachiDSP::writeDRAM, &hitachidsp});
      }
    }
  }

  has.HitachiDSP = true;

  std::string pmap = BML::searchNode(node, {"processor", "map"});
  if (!pmap.empty()) {
      loadMap(pmap, {&HitachiDSP::readIO, &hitachidsp}, {&HitachiDSP::writeIO, &hitachidsp});
  }
}

//processor(architecture=uPD7725)
void Cartridge::loaduPD7725(std::string node) {
  for(auto& word : necdsp.programROM) word = 0x000000;
  for(auto& word : necdsp.dataROM) word = 0x0000;
  for(auto& word : necdsp.dataRAM) word = 0x0000;

  if(auto oscillator = game.oscillator()) {
    necdsp.Frequency = oscillator->frequency;
  } else {
    necdsp.Frequency = 7'600'000;
  }

  std::vector<std::string> identifiers = BML::searchList(game.document, "identifier");
  std::string ident = identifiers[0].erase(0, identifiers[0].find_first_of(' ') + 1);
  ident.pop_back();
  std::transform(ident.begin(), ident.end(), ident.begin(), ::tolower);

  std::string pname = ident + ".program.rom";
  std::string dname = ident + ".data.rom";

  bool failed = false;

  std::ifstream prgfile = Emulator::platform->fopen(ID::SuperFamicom, pname);
  if (prgfile.is_open()) {
    for (unsigned i = 0; i < 2048; ++i) {
      uint8_t a = prgfile.get(); uint8_t b = prgfile.get(); uint8_t c = prgfile.get();
      necdsp.programROM[i] = a | (b << 8) | (c << 16);
    }
    prgfile.close();
  }
  else {
    failed = true;
  }

  std::ifstream datafile = Emulator::platform->fopen(ID::SuperFamicom, dname);
  if (datafile.is_open()) {
    for (unsigned i = 0; i < 1024; ++i) {
      uint8_t a = datafile.get(); uint8_t b = datafile.get();
      necdsp.dataROM[i] = a | (b << 8);
    }
    datafile.close();
  }
  else {
    failed = true;
  }

  std::string pmap = BML::searchNode(node, {"processor", "map"});

  if (failed || configuration.coprocessor.preferHLE) {
    if (ident == "dsp1" || ident == "dsp1b") {
      has.DSP1 = true;
      if (!pmap.empty()) {
        loadMap(pmap, {&DSP1::read, &dsp1}, {&DSP1::write, &dsp1});
      }
      return;
    }
    else if (ident == "dsp2") {
      has.DSP2 = true;
      if (!pmap.empty()) {
        loadMap(pmap, {&DSP2::read, &dsp2}, {&DSP2::write, &dsp2});
      }
      return;
    }
    else if (ident == "dsp4") {
      has.DSP4 = true;
      if (!pmap.empty()) {
        loadMap(pmap, {&DSP4::read, &dsp4}, {&DSP4::write, &dsp4});
      }
      return;
    }
  }

  std::vector<std::string> memlist = BML::searchList(node, "memory");
  for (std::string& m : memlist) {
    std::string type = BML::search(m, {"memory", "type"});
    std::string content = BML::search(m, {"memory", "content"});
    std::string arch = BML::search(m, {"memory", "architecture"});
    if (type == "RAM" && content == "Data" && arch == "uPD7725") {
      std::ifstream sramfile = Emulator::platform->fopen(ID::SuperFamicom, "save.ram");
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

//processor(architecture=uPD96050)
void Cartridge::loaduPD96050(std::string node) {
  for(auto& word : necdsp.programROM) word = 0x000000;
  for(auto& word : necdsp.dataROM) word = 0x0000;
  for(auto& word : necdsp.dataRAM) word = 0x0000;

  if(auto oscillator = game.oscillator()) {
    necdsp.Frequency = oscillator->frequency;
  } else {
    necdsp.Frequency = 11'000'000;
  }

  std::vector<std::string> identifiers = BML::searchList(game.document, "identifier");
  std::string ident = identifiers[0].erase(0, identifiers[0].find_first_of(' ') + 1);
  ident.pop_back();
  std::transform(ident.begin(), ident.end(), ident.begin(), ::tolower);

  std::string pname = ident + ".program.rom";
  std::string dname = ident + ".data.rom";

  bool failed = false;

  std::ifstream prgfile = Emulator::platform->fopen(ID::SuperFamicom, pname);
  if (prgfile.is_open()) {
    for (unsigned i = 0; i < 16384; ++i) {
      uint8_t a = prgfile.get(); uint8_t b = prgfile.get(); uint8_t c = prgfile.get();
      necdsp.programROM[i] = a | (b << 8) | (c << 16);
    }
    prgfile.close();
  }
  else {
    failed = true;
  }

  std::ifstream datafile = Emulator::platform->fopen(ID::SuperFamicom, dname);
  if (datafile.is_open()) {
    for (unsigned i = 0; i < 2048; ++i) {
      uint8_t a = datafile.get(); uint8_t b = datafile.get();
      necdsp.dataROM[i] = a | (b << 8);
    }
    datafile.close();
  }
  else {
    failed = true;
  }

  if(failed || configuration.coprocessor.preferHLE) {
    if (ident == "st010") {
      has.ST0010 = true;
      std::vector<std::string> memlist = BML::searchList(node, "memory");
      for (std::string& m : memlist) {
        std::string type = BML::search(m, {"memory", "type"});
        std::string content = BML::search(m, {"memory", "content"});
        if (type == "RAM" && content == "Data") {
          std::string mem = BML::searchNode(m, {"memory", "map"});
          loadMap(mem, {&ST0010::read, &st0010}, {&ST0010::write, &st0010});
        }
      }
    }
    return;
  }

  std::vector<std::string> memlist = BML::searchList(node, "memory");
  for (std::string& m : memlist) {
    std::string type = BML::search(m, {"memory", "type"});
    std::string content = BML::search(m, {"memory", "content"});
    std::string arch = BML::search(m, {"memory", "architecture"});
    if (type == "RAM" && content == "Data" && arch == "uPD96050") {
      std::ifstream sramfile = Emulator::platform->fopen(ID::SuperFamicom, "save.ram");
      if (sramfile.is_open()) {
        for (unsigned i = 0; i < 2048; ++i) {
          uint8_t a = sramfile.get(); uint8_t b = sramfile.get();
          necdsp.dataRAM[i] = a | (b << 8);
        }
        sramfile.close();
      }
      std::vector<std::string> maps = BML::searchList(m, "map");
      for (std::string map : maps) {
        loadMap(map, {&NECDSP::readRAM, &necdsp}, {&NECDSP::writeRAM, &necdsp});
      }
    }
  }

  has.NECDSP = true;
  necdsp.revision = NECDSP::Revision::uPD96050;

  std::string pmap = BML::searchNode(node, {"processor", "map"});
  if (!pmap.empty()) {
    loadMap(pmap, {&NECDSP::read, &necdsp}, {&NECDSP::write, &necdsp});
  }
}

//rtc(manufacturer=Epson)
void Cartridge::loadEpsonRTC(std::string node) {
  has.EpsonRTC = true;

  epsonrtc.initialize();

  std::string map = BML::searchNode(node, {"rtc", "map"});
  loadMap(map, {&EpsonRTC::read, &epsonrtc}, {&EpsonRTC::write, &epsonrtc});

  std::string memory = BML::searchNode(node, {"rtc", "memory"});
  if(auto file = game.memory(memory)) {
    std::ifstream rtcfile = Emulator::platform->fopen(ID::SuperFamicom, "time.rtc");
    if (rtcfile.is_open()) {
      uint8_t data[16] = {0};
      for(auto& byte : data) byte = rtcfile.get();
      epsonrtc.load(data);
      rtcfile.close();
    }
  }
}

//rtc(manufacturer=Sharp)
void Cartridge::loadSharpRTC(std::string node) {
  has.SharpRTC = true;

  sharprtc.initialize();

  std::string map = BML::searchNode(node, {"rtc", "map"});
  loadMap(map, {&SharpRTC::read, &sharprtc}, {&SharpRTC::write, &sharprtc});

  std::string memory = BML::searchNode(node, {"rtc", "memory"});
  if(auto file = game.memory(memory)) {
    std::ifstream rtcfile = Emulator::platform->fopen(ID::SuperFamicom, "time.rtc");
    if (rtcfile.is_open()) {
      uint8_t data[16] = {0};
      for(auto& byte : data) byte = rtcfile.get();
      sharprtc.load(data);
      rtcfile.close();
    }
  }
}

//processor(identifier=SPC7110)
void Cartridge::loadSPC7110(std::string node) {
  has.SPC7110 = true;

  std::vector<std::string> pmaps = BML::searchListShallow(node, "processor", "map");
  for (std::string& m : pmaps) {
    loadMap(m, {&SPC7110::read, &spc7110}, {&SPC7110::write, &spc7110});
  }

  std::string mcu = BML::searchNode(node, {"processor", "mcu"});

  if (!mcu.empty()) {
    std::vector<std::string> mcumaps = BML::searchList(mcu, "map");
    for (std::string& m : mcumaps) {
      loadMap(m, {&SPC7110::mcuromRead, &spc7110}, {&SPC7110::mcuromWrite, &spc7110});
    }

    std::vector<std::string> memlist = BML::searchList(mcu, "memory");
    for (std::string& m : memlist) {
      std::string type = BML::search(m, {"memory", "type"});
      std::string content = BML::search(m, {"memory", "content"});
      if (type == "ROM" && content == "Program") {
        loadMemory(spc7110.prom, m);
      }
      else if (type == "ROM" && content == "Data") {
        loadMemory(spc7110.drom, m);
      }
    }
  }

  std::string sram = BML::searchNode(node, {"processor", "memory"});
  if (!sram.empty()) {
    std::string type = BML::search(sram, {"memory", "type"});
    std::string content = BML::search(sram, {"memory", "content"});
    if (type == "RAM" && content == "Save") {
      loadMemory(spc7110.ram, sram);
      std::vector<std::string> maps = BML::searchList(sram, "map");
      for (std::string& m : maps) {
        loadMap(m, {&SPC7110::mcuramRead, &spc7110}, {&SPC7110::mcuramWrite, &spc7110});
      }
    }
  }
}

//processor(identifier=SDD1)
void Cartridge::loadSDD1(std::string node) {
  has.SDD1 = true;

  std::string pmap = BML::searchNode(node, {"processor", "map"});
  if (!pmap.empty()) loadMap(pmap, {&SDD1::ioRead, &sdd1}, {&SDD1::ioWrite, &sdd1});

  std::string mcu = BML::searchNode(node, {"processor", "mcu"});

  if (!mcu.empty()) {
    std::vector<std::string> maps = BML::searchList(mcu, "map");
    for (auto map : maps) {
      loadMap(map, {&SDD1::mcuRead, &sdd1}, {&SDD1::mcuWrite, &sdd1});
    }

    std::vector<std::string> mcumem = BML::searchList(mcu, "memory");
    for (std::string& m : mcumem) {
      std::string type = BML::search(m, {"memory", "type"});
      std::string content = BML::search(m, {"memory", "content"});
      if (type == "ROM" && content == "Program") loadMemory(sdd1.rom, m);
    }
  }
}

//processor(identifier=OBC1)
void Cartridge::loadOBC1(std::string node) {
  has.OBC1 = true;

  std::vector<std::string> maps = BML::searchList(node, "map");
  for (auto map : maps) {
    loadMap(map, {&OBC1::read, &obc1}, {&OBC1::write, &obc1});
  }

  std::vector<std::string> memory = BML::searchList(node, "memory");
  for (auto m : memory) {
    if (BML::search(m, {"memory", "type"}) == "RAM" &&
        BML::search(m, {"memory", "content"}) == "Save") {
      loadMemory(obc1.ram, m);
    }
  }
}

//file::exists("msu1/data.rom")
void Cartridge::loadMSU1() {
  has.MSU1 = true;

  bus.map({&MSU1::readIO, &msu1}, {&MSU1::writeIO, &msu1}, "00-3f,80-bf:2000-2007");
}

void Cartridge::saveCartridge(std::string node) {
  std::vector<std::string> boardmem = BML::searchListShallow(board, "board", "memory");
  for (std::string& m : boardmem) {
    std::string type = BML::search(m, {"memory", "type"});
    std::string content = BML::search(m, {"memory", "content"});
    if (type == "RAM" && content == "Save") saveRAM(m);
  }

  std::vector<std::string> processors = BML::searchList(board, "processor");
  for (std::string& p : processors) {
    std::string arch = BML::search(p, {"processor", "architecture"});
    std::string id = BML::search(p, {"processor", "identifier"});
    if (id == "MCC") saveMCC(p);
    if (id == "SPC7110") saveSPC7110(p);
    if (id == "OBC1") saveOBC1(p);
    if (arch == "W65C816S") saveSA1(p);
    if (arch == "GSU") saveSuperFX(p);
    if (arch == "ARM6") saveARMDSP(p);
    if (arch == "HG51BS169") saveHitachiDSP(p);
    if (arch == "uPD7725") saveuPD7725(p);
    if (arch == "uPD96050") saveuPD96050(p);
  }

  std::vector<std::string> rtclist = BML::searchList(board, "rtc");
  for (std::string& rtc : rtclist) {
    std::string manufacturer = BML::search(rtc, {"rtc", "manufacturer"});
    if (manufacturer == "Epson") saveEpsonRTC(rtc);
    if (manufacturer == "Sharp") saveSharpRTC(rtc);
  }
}

void Cartridge::saveCartridgeBSMemory(std::string node) {
  std::vector<std::string> memlist = BML::searchList(node, "memory");
  for (std::string& m : memlist) {
    std::string type = BML::search(m, {"memory", "type"});
    std::string content = BML::search(m, {"memory", "content"});
    if (type == "Flash" && content == "Program") {
      if(auto memory = Emulator::Game::Memory(m)) {
        if (bsmemory.memory.data() != nullptr) {
          Emulator::platform->write(bsmemory.pathID, memory.name(), bsmemory.memory.data(), memory.size);
        }
      }
    }
  }
}

void Cartridge::saveCartridgeSufamiTurboA(std::string node) {
  std::vector<std::string> memlist = BML::searchList(node, "memory");
  for (std::string& m : memlist) {
    std::string type = BML::search(m, {"memory", "type"});
    std::string content = BML::search(m, {"memory", "content"});
    if (type == "RAM" && content == "Save") {
      if(auto memory = Emulator::Game::Memory(m)) {
        if (memory.nonVolatile) {
          Emulator::platform->write(sufamiturboA.pathID, memory.name(), sufamiturboA.ram.data(), memory.size);
        }
      }
    }
  }
}

void Cartridge::saveCartridgeSufamiTurboB(std::string node) {
  std::vector<std::string> memlist = BML::searchList(node, "memory");
  for (std::string& m : memlist) {
    std::string type = BML::search(m, {"memory", "type"});
    std::string content = BML::search(m, {"memory", "content"});
    if (type == "RAM" && content == "Save") {
      if(auto memory = Emulator::Game::Memory(m)) {
        if (memory.nonVolatile) {
          Emulator::platform->write(sufamiturboB.pathID, memory.name(), sufamiturboB.ram.data(), memory.size);
        }
      }
    }
  }
}

void Cartridge::saveMemory(Memory& ram, std::string node) {
  if(auto memory = game.memory(node)) {
    if(memory->type == "RAM" && !memory->nonVolatile) return;
    if(memory->type == "RTC" && !memory->nonVolatile) return;
    Emulator::platform->write(pathID(), memory->name(), ram.data(), ram.size());
  }
}

//memory(type=RAM,content=Save)
void Cartridge::saveRAM(std::string node) {
  saveMemory(ram, node);
}

//processor(identifier=MCC)
void Cartridge::saveMCC(std::string node) {
  std::string mcu = BML::searchNode(node, {"processor", "mcu"});
  if (!mcu.empty()) {
    std::vector<std::string> mcumem = BML::searchList(mcu, "memory");
    for (std::string& m : mcumem) {
      std::string type = BML::search(m, {"memory", "type"});
      std::string content = BML::search(m, {"memory", "content"});
      if (type == "RAM" && content == "Download") saveMemory(mcc.psram, m);
    }
  }
}

//processor(architecture=W65C816S)
void Cartridge::saveSA1(std::string node) {
  std::vector<std::string> memlist = BML::searchList(node, "memory");
  for (std::string& m : memlist) {
    std::string type = BML::search(m, {"memory", "type"});
    std::string content = BML::search(m, {"memory", "content"});
    if (type == "RAM" && content == "Save") saveMemory(sa1.bwram, m);
    if (type == "RAM" && content == "Internal") saveMemory(sa1.iram, m);
  }
}

//processor(architecture=GSU)
void Cartridge::saveSuperFX(std::string node) {
  std::vector<std::string> memlist = BML::searchList(node, "memory");
  for (std::string& m : memlist) {
    std::string type = BML::search(m, {"memory", "type"});
    std::string content = BML::search(m, {"memory", "content"});
    if (type == "RAM" && content == "Save") saveMemory(superfx.ram, m);
  }
}

//processor(architecture=ARM6)
void Cartridge::saveARMDSP(std::string node) {
  std::vector<std::string> memlist = BML::searchList(node, "memory");
  for (std::string& m : memlist) {
    std::string type = BML::search(m, {"memory", "type"});
    std::string content = BML::search(m, {"memory", "content"});
    std::string arch = BML::search(m, {"memory", "architecture"});
    if (type == "RAM" && content == "Data" && arch == "ARM6") {
      if(auto file = game.memory(m)) {
      if(file->nonVolatile) {
        Emulator::platform->write(ID::SuperFamicom, "save.ram", armdsp.programRAM, (16 * 1024));
      }
    }
    }
  }
}

//processor(architecture=HG51BS169)
void Cartridge::saveHitachiDSP(std::string node) {
  std::vector<std::string> memlist = BML::searchList(node, "memory");
  for (std::string& m : memlist) {
    std::string type = BML::search(m, {"memory", "type"});
    std::string content = BML::search(m, {"memory", "content"});
    std::string arch = BML::search(m, {"memory", "architecture"});
    if (type == "RAM" && content == "Data" && arch == "HG51BS169") {
      if(auto file = game.memory(m)) {
        if(file->nonVolatile) {
          Emulator::platform->write(ID::SuperFamicom, "save.ram", hitachidsp.dataRAM, (3 * 1024));
        }
      }
    }
  }
}

//processor(architecture=uPD7725)
void Cartridge::saveuPD7725(std::string node) {
  std::vector<std::string> memlist = BML::searchList(node, "memory");
  for (std::string& m : memlist) {
    std::string type = BML::search(m, {"memory", "type"});
    std::string content = BML::search(m, {"memory", "content"});
    std::string arch = BML::search(m, {"memory", "architecture"});
    if (type == "RAM" && content == "Data" && arch == "uPD7725") {
      if(auto file = game.memory(m)) {
        if(file->nonVolatile) {
          Emulator::platform->write(ID::SuperFamicom, "save.ram", (uint8_t*)necdsp.dataRAM, 2 * 256);
        }
      }
    }
  }
}

//processor(architecture=uPD96050)
void Cartridge::saveuPD96050(std::string node) {
  std::vector<std::string> memlist = BML::searchList(node, "memory");
  for (std::string& m : memlist) {
    std::string type = BML::search(m, {"memory", "type"});
    std::string content = BML::search(m, {"memory", "content"});
    std::string arch = BML::search(m, {"memory", "architecture"});
    if (type == "RAM" && content == "Data" && arch == "uPD96050") {
      if(auto file = game.memory(m)) {
        if(file->nonVolatile) {
          Emulator::platform->write(ID::SuperFamicom, "save.ram", (uint8_t*)necdsp.dataRAM, (4 * 1024));
        }
      }
    }
  }
}

//rtc(manufacturer=Epson)
void Cartridge::saveEpsonRTC(std::string node) {
  std::string memory = BML::searchNode(node, {"rtc", "memory"});
  if(auto file = game.memory(memory)) {
    if(file->nonVolatile) {
      uint8_t data[16] = {0};
      epsonrtc.save(data);
      Emulator::platform->write(ID::SuperFamicom, "time.rtc", data, 16);
    }
  }
}

//rtc(manufacturer=Sharp)
void Cartridge::saveSharpRTC(std::string node) {
  std::string memory = BML::searchNode(node, {"rtc", "memory"});
  if(auto file = game.memory(memory)) {
    if(file->nonVolatile) {
      uint8_t data[16] = {0};
      sharprtc.save(data);
      Emulator::platform->write(ID::SuperFamicom, "time.rtc", data, 16);
    }
  }
}

//processor(identifier=SPC7110)
void Cartridge::saveSPC7110(std::string node) {
  std::string sram = BML::searchNode(node, {"processor", "memory"});
  if (!sram.empty()) {
    std::string type = BML::search(sram, {"memory", "type"});
    std::string content = BML::search(sram, {"memory", "content"});
    if (type == "RAM" && content == "Save") {
      saveMemory(spc7110.ram, sram);
    }
  }
}

//processor(identifier=OBC1)
void Cartridge::saveOBC1(std::string node) {
  std::vector<std::string> memlist = BML::searchList(node, "memory");
  for (std::string& m : memlist) {
    std::string type = BML::search(m, {"memory", "type"});
    std::string content = BML::search(m, {"memory", "content"});
    if (type == "RAM" && content == "Save") saveMemory(obc1.ram, m);
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

  if(auto loaded = Emulator::platform->load(ID::SuperFamicom, "Super Famicom", "sfc", {"Auto", "NTSC", "PAL"})) {
    information.pathID = loaded.pathID;
    information.region = loaded.option;
  } else return false;

  if (!game.document.empty()) {
    game.load(game.document);
  }
  else {
      return false;
  }

  loadCartridge(game.document);

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
    firm = armdsp.firmware();
    for (size_t i = 0; i < firm.size(); ++i) buf.push_back(firm[i]);

    firm = hitachidsp.firmware();
    for (size_t i = 0; i < firm.size(); ++i) buf.push_back(firm[i]);

    firm = necdsp.firmware();
    for (size_t i = 0; i < firm.size(); ++i) buf.push_back(firm[i]);

    //finalize hash
    information.sha256 = sha256_digest(buf.data(), buf.size());
  }

  return true;
}

bool Cartridge::loadBSMemory() {
  if (!slotBSMemory.document.empty()) {
    slotBSMemory.load(slotBSMemory.document);
    loadCartridgeBSMemory(slotBSMemory.document);
    return true;
  }

  return false;
}

bool Cartridge::loadSufamiTurboA() {
  if (!slotSufamiTurboA.document.empty()) {
    slotSufamiTurboA.load(slotSufamiTurboA.document);
    loadCartridgeSufamiTurboA(slotSufamiTurboA.document);
    return true;
  }

  return false;
}

bool Cartridge::loadSufamiTurboB() {
  if (!slotSufamiTurboB.document.empty()) {
    slotSufamiTurboB.load(slotSufamiTurboB.document);
    loadCartridgeSufamiTurboB(slotSufamiTurboB.document);
    return true;
  }

  return false;
}

void Cartridge::save() {
  saveCartridge(game.document);
  if(has.GameBoySlot) {
    icd.save();
  }
  if(has.BSMemorySlot) {
    saveCartridgeBSMemory(slotBSMemory.document);
  }
  if(has.SufamiTurboSlotA) {
    saveCartridgeSufamiTurboA(slotSufamiTurboA.document);
  }
  if(has.SufamiTurboSlotB) {
    saveCartridgeSufamiTurboB(slotSufamiTurboB.document);
  }
}

void Cartridge::setDocument(unsigned id, std::string doc) {
  if (id == 1) {
    game.document = doc;
  }
}

void Cartridge::setRomBSMemory(std::vector<uint8_t>& data, std::string& loc) {
  romdataBS = data;
  auto heuristics = Heuristics::BSMemory(data, loc);

  std::ifstream dbfile = Emulator::platform->fopen(ID::System, "BS Memory.bml");

  std::string sha256 = sha256_digest(data.data(), data.size());
  std::string manifest = BML::gendoc2(dbfile, "game", "sha256", sha256);

  slotBSMemory.document = manifest.empty() ? heuristics.manifest() : manifest;
}

void Cartridge::setRomSufamiTurboA(std::vector<uint8_t>& data, std::string& loc) {
  romdataSTA = data;
  auto heuristics = Heuristics::SufamiTurbo(data, loc);

  std::ifstream dbfile =
    Emulator::platform->fopen(ID::System, "Sufami Turbo.bml");

  std::string sha256 = sha256_digest(data.data(), data.size());
  std::string manifest = BML::gendoc2(dbfile, "game", "sha256", sha256);

  slotSufamiTurboA.document =
    manifest.empty() ? heuristics.manifest() : manifest;

  has.SufamiTurboSlotA = true;
}

void Cartridge::setRomSufamiTurboB(std::vector<uint8_t>& data, std::string& loc) {
  romdataSTB = data;
  auto heuristics = Heuristics::SufamiTurbo(data, loc);

  std::ifstream dbfile =
    Emulator::platform->fopen(ID::System, "Sufami Turbo.bml");

  std::string sha256 = sha256_digest(data.data(), data.size());
  std::string manifest = BML::gendoc2(dbfile, "game", "sha256", sha256);

  slotSufamiTurboB.document =
    manifest.empty() ? heuristics.manifest() : manifest;

  has.SufamiTurboSlotB = true;
}

void Cartridge::unload() {
  rom.reset();
  ram.reset();
}

}
