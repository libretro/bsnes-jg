#include <iostream>
#include <iterator>
#include <vector>

#include "sfc.hpp"
#include "sha256.hpp"

namespace SuperFamicom {

nall::Markup::Node Cartridge::loadBoard(nall::string board) {
  nall::string output;

  if(board.beginsWith("SNSP-")) board.replace("SNSP-", "SHVC-", 1L);
  if(board.beginsWith("MAXI-")) board.replace("MAXI-", "SHVC-", 1L);
  if(board.beginsWith("MJSC-")) board.replace("MJSC-", "SHVC-", 1L);
  if(board.beginsWith("EA-"  )) board.replace("EA-",   "SHVC-", 1L);
  if(board.beginsWith("WEI-" )) board.replace("WEI-",  "SHVC-", 1L);

  std::ifstream boardsfile = platform->fopen(ID::System, "boards.bml");
  if (boardsfile.is_open()) {
    std::string boards((std::istreambuf_iterator<char>(boardsfile)),
      (std::istreambuf_iterator<char>()));
    auto document = nall::BML::unserialize(boards.c_str());
    for(auto leaf : document.find("board")) {
      auto id = leaf.text();
      bool matched = id == board;
      if(!matched && id.match("*(*)*")) {
        auto part = id.transform("()", "||").split("|");
        for(auto& revision : part(1).split(",")) {
          if(nall::string{part(0), revision, part(2)} == board) matched = true;
        }
      }
      if(matched) return leaf;
    }
  }

  return {};
}

void Cartridge::loadCartridge(nall::Markup::Node node) {
  board = node["board"];
  if(!board) board = loadBoard(game.board);

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

  if(auto node = board["memory(type=ROM,content=Program)"]) loadROM(node);
  if(auto node = board["memory(type=ROM,content=Expansion)"]) loadROM(node);  //todo: handle this better
  if(auto node = board["memory(type=RAM,content=Save)"]) loadRAM(node);
  if(auto node = board["processor(identifier=ICD)"]) loadICD(node);
  if(auto node = board["processor(identifier=MCC)"]) loadMCC(node);
  if(auto node = board["slot(type=BSMemory)"]) loadBSMemory(node);
  if(auto node = board["slot(type=SufamiTurbo)[0]"]) loadSufamiTurboA(node);
  if(auto node = board["slot(type=SufamiTurbo)[1]"]) loadSufamiTurboB(node);
  if(auto node = board["dip"]) loadDIP(node);
  if(auto node = board["processor(architecture=uPD78214)"]) loadEvent(node);
  if(auto node = board["processor(architecture=W65C816S)"]) loadSA1(node);
  if(auto node = board["processor(architecture=GSU)"]) loadSuperFX(node);
  if(auto node = board["processor(architecture=ARM6)"]) loadARMDSP(node);
  if(auto node = board["processor(architecture=HG51BS169)"]) loadHitachiDSP(node, game.board.match("2DC*") ? 2 : 1);
  if(auto node = board["processor(architecture=uPD7725)"]) loaduPD7725(node);
  if(auto node = board["processor(architecture=uPD96050)"]) loaduPD96050(node);
  if(auto node = board["rtc(manufacturer=Epson)"]) loadEpsonRTC(node);
  if(auto node = board["rtc(manufacturer=Sharp)"]) loadSharpRTC(node);
  if(auto node = board["processor(identifier=SPC7110)"]) loadSPC7110(node);
  if(auto node = board["processor(identifier=SDD1)"]) loadSDD1(node);
  if(auto node = board["processor(identifier=OBC1)"]) loadOBC1(node);

  if(platform->fopen(ID::SuperFamicom, "msu1/data.rom")) loadMSU1();
}

void Cartridge::loadCartridgeBSMemory(nall::Markup::Node node) {
  if(auto memory = Emulator::Game::Memory{node["game/board/memory(content=Program)"]}) {
    bsmemory.ROM = memory.type == "ROM";
    bsmemory.memory.allocate(memory.size);
    std::vector<uint8_t> buf = platform->mopen(bsmemory.pathID, std::string(memory.name()));
    if (!buf.empty()) {
      for (int i = 0; i < memory.size; ++i)
        bsmemory.memory.data()[i] = buf[i];
    }
  }
}

void Cartridge::loadCartridgeSufamiTurboA(nall::Markup::Node node) {
  if(auto memory = Emulator::Game::Memory{node["game/board/memory(type=ROM,content=Program)"]}) {
    sufamiturboA.rom.allocate(memory.size);
    std::vector<uint8_t> buf = platform->mopen(sufamiturboA.pathID, std::string(memory.name()));
    if (!buf.empty()) {
      for (int i = 0; i < memory.size; ++i)
        sufamiturboA.rom.data()[i] = buf[i];
    }
  }

  if(auto memory = Emulator::Game::Memory{node["game/board/memory(type=RAM,content=Save)"]}) {
    sufamiturboA.ram.allocate(memory.size);
    std::ifstream sramfile = platform->fopen(sufamiturboA.pathID, "save.ram");
    if (sramfile.is_open()) {
      sramfile.read((char*)sufamiturboA.ram.data(), memory.size);
      sramfile.close();
    }
  }
}

void Cartridge::loadCartridgeSufamiTurboB(nall::Markup::Node node) {
  if(auto memory = Emulator::Game::Memory{node["game/board/memory(type=ROM,content=Program)"]}) {
    sufamiturboB.rom.allocate(memory.size);
    std::vector<uint8_t> buf = platform->mopen(sufamiturboB.pathID, std::string(memory.name()));
    if (!buf.empty()) {
      for (int i = 0; i < memory.size; ++i)
        sufamiturboB.rom.data()[i] = buf[i];
    }
  }

  if(auto memory = Emulator::Game::Memory{node["game/board/memory(type=RAM,content=Save)"]}) {
    sufamiturboB.ram.allocate(memory.size);
    std::ifstream sramfile = platform->fopen(sufamiturboB.pathID, "save.ram");
    if (sramfile.is_open()) {
      sramfile.read((char*)sufamiturboB.ram.data(), memory.size);
      sramfile.close();
    }
  }
}

void Cartridge::loadMemory(Memory& mem, nall::Markup::Node node, bool required) {
  if(auto memory = game.memory(node)) {
    mem.allocate(memory->size);
    if(memory->type == "RAM" && !memory->nonVolatile) return;
    if(memory->type == "RTC" && !memory->nonVolatile) return;

    if (memory->name() == "program.rom") {
      std::vector<uint8_t> buf = platform->mopen(pathID(), std::string(memory->name()));
      if (!buf.empty()) {
      for (int i = 0; i < memory->size; ++i)
        mem.data()[i] = buf[i];
      }
      return;
    }

    std::ifstream memfile = platform->fopen(pathID(), std::string(memory->name()));
    if (memfile.is_open()) {
      memfile.seekg(0, memfile.end);
      unsigned fsize = memfile.tellg();
      memfile.seekg(0, memfile.beg);
      memfile.read((char*)ram.data(), std::min(fsize, ram.size()));
      memfile.close();
    }
  }
}

template<typename T>  //T = ReadableMemory, WritableMemory, ProtectableMemory
unsigned Cartridge::loadMap(nall::Markup::Node map, T& memory) {
  auto addr = map["address"].text();
  auto size = map["size"].natural();
  auto base = map["base"].natural();
  auto mask = map["mask"].natural();
  if(size == 0) size = memory.size();
  if(size == 0) return 0; //does this ever actually occur? - Yes! Sufami Turbo.
  return bus.map({&T::read, &memory}, {&T::write, &memory}, addr, size, base, mask);
}

unsigned Cartridge::loadMap(
  nall::Markup::Node map,
  const nall::function<uint8_t (unsigned, uint8_t)>& reader,
  const nall::function<void  (unsigned, uint8_t)>& writer
) {
  auto addr = map["address"].text();
  auto size = map["size"].natural();
  auto base = map["base"].natural();
  auto mask = map["mask"].natural();
  return bus.map(reader, writer, addr, size, base, mask);
}

//memory(type=ROM,content=Program)
void Cartridge::loadROM(nall::Markup::Node node) {
  loadMemory(rom, node, File::Required);
  for(auto leaf : node.find("map")) loadMap(leaf, rom);
}

//memory(type=RAM,content=Save)
void Cartridge::loadRAM(nall::Markup::Node node) {
  loadMemory(ram, node, File::Optional);
  for(auto leaf : node.find("map")) loadMap(leaf, ram);
}

//processor(identifier=ICD)
void Cartridge::loadICD(nall::Markup::Node node) {
  has.GameBoySlot = true;
  has.ICD = true;

  icd.Revision = node["revision"].natural();
  if(auto oscillator = game.oscillator()) {
    icd.Frequency = oscillator->frequency;
  } else {
    icd.Frequency = 0;
  }

  //Game Boy core loads data through ICD interface
  for(auto map : node.find("map")) {
    loadMap(map, {&ICD::readIO, &icd}, {&ICD::writeIO, &icd});
  }
}

//processor(identifier=MCC)
void Cartridge::loadMCC(nall::Markup::Node node) {
  has.MCC = true;

  for(auto map : node.find("map")) {
    loadMap(map, {&MCC::read, &mcc}, {&MCC::write, &mcc});
  }

  if(auto mcu = node["mcu"]) {
    for(auto map : mcu.find("map")) {
      loadMap(map, {&MCC::mcuRead, &mcc}, {&MCC::mcuWrite, &mcc});
    }
    if(auto memory = mcu["memory(type=ROM,content=Program)"]) {
      loadMemory(mcc.rom, memory, File::Required);
    }
    if(auto memory = mcu["memory(type=RAM,content=Download)"]) {
      loadMemory(mcc.psram, memory, File::Optional);
    }
    if(auto slot = mcu["slot(type=BSMemory)"]) {
      loadBSMemory(slot);
    }
  }
}

//slot(type=BSMemory)
void Cartridge::loadBSMemory(nall::Markup::Node node) {
  has.BSMemorySlot = true;

  if(auto loaded = platform->load(ID::BSMemory, "BS Memory", "bs")) {
    bsmemory.pathID = loaded.pathID;
    loadBSMemory();

    for(auto map : node.find("map")) {
      loadMap(map, bsmemory);
    }
  }
}

//slot(type=SufamiTurbo)[0]
void Cartridge::loadSufamiTurboA(nall::Markup::Node node) {
  has.SufamiTurboSlotA = true;

  if(auto loaded = platform->load(ID::SufamiTurboA, "Sufami Turbo", "st")) {
    sufamiturboA.pathID = loaded.pathID;
    loadSufamiTurboA();

    for(auto map : node.find("rom/map")) {
      loadMap(map, sufamiturboA.rom);
    }

    for(auto map : node.find("ram/map")) {
      loadMap(map, sufamiturboA.ram);
    }
  }
}

//slot(type=SufamiTurbo)[1]
void Cartridge::loadSufamiTurboB(nall::Markup::Node node) {
  has.SufamiTurboSlotB = true;

  if(auto loaded = platform->load(ID::SufamiTurboB, "Sufami Turbo", "st")) {
    sufamiturboB.pathID = loaded.pathID;
    loadSufamiTurboB();

    for(auto map : node.find("rom/map")) {
      loadMap(map, sufamiturboB.rom);
    }

    for(auto map : node.find("ram/map")) {
      loadMap(map, sufamiturboB.ram);
    }
  }
}

//dip
void Cartridge::loadDIP(nall::Markup::Node node) {
  has.DIP = true;
  // Multi-game carts (Campus Challenge '92, PowerFest '94) were no longer
  // supported after higan v106, and in bsnes standalone this always returns 0.
  //dip.value = platform->dipSettings(node);
  dip.value = 0;

  for(auto map : node.find("map")) {
    loadMap(map, {&DIP::read, &dip}, {&DIP::write, &dip});
  }
}

//processor(architecture=uPD78214)
void Cartridge::loadEvent(nall::Markup::Node node) {
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
      loadMemory(event.rom[0], memory, File::Required);
    }
    if(auto memory = mcu["memory(type=ROM,content=Level-1)"]) {
      loadMemory(event.rom[1], memory, File::Required);
    }
    if(auto memory = mcu["memory(type=ROM,content=Level-2)"]) {
      loadMemory(event.rom[2], memory, File::Required);
    }
    if(auto memory = mcu["memory(type=ROM,content=Level-3)"]) {
      loadMemory(event.rom[3], memory, File::Required);
    }
  }
}

//processor(architecture=W65C816S)
void Cartridge::loadSA1(nall::Markup::Node node) {
  has.SA1 = true;

  for(auto map : node.find("map")) {
    loadMap(map, {&SA1::readIOCPU, &sa1}, {&SA1::writeIOCPU, &sa1});
  }

  if(auto mcu = node["mcu"]) {
    for(auto map : mcu.find("map")) {
      loadMap(map, {&SA1::ROM::readCPU, &sa1.rom}, {&SA1::ROM::writeCPU, &sa1.rom});
    }
    if(auto memory = mcu["memory(type=ROM,content=Program)"]) {
      loadMemory(sa1.rom, memory, File::Required);
    }
    if(auto slot = mcu["slot(type=BSMemory)"]) {
      loadBSMemory(slot);
    }
  }

  if(auto memory = node["memory(type=RAM,content=Save)"]) {
    loadMemory(sa1.bwram, memory, File::Optional);
    for(auto map : memory.find("map")) {
      loadMap(map, {&SA1::BWRAM::readCPU, &sa1.bwram}, {&SA1::BWRAM::writeCPU, &sa1.bwram});
    }
  }

  if(auto memory = node["memory(type=RAM,content=Internal)"]) {
    loadMemory(sa1.iram, memory, File::Optional);
    for(auto map : memory.find("map")) {
      loadMap(map, {&SA1::IRAM::readCPU, &sa1.iram}, {&SA1::IRAM::writeCPU, &sa1.iram});
    }
  }
}

//processor(architecture=GSU)
void Cartridge::loadSuperFX(nall::Markup::Node node) {
  has.SuperFX = true;

  if(auto oscillator = game.oscillator()) {
    superfx.Frequency = oscillator->frequency;  //GSU-1, GSU-2
  } else {
    superfx.Frequency = system.cpuFrequency();  //MARIO CHIP 1
  }

  for(auto map : node.find("map")) {
    loadMap(map, {&SuperFX::readIO, &superfx}, {&SuperFX::writeIO, &superfx});
  }

  if(auto memory = node["memory(type=ROM,content=Program)"]) {
    loadMemory(superfx.rom, memory, File::Required);
    for(auto map : memory.find("map")) {
      loadMap(map, superfx.cpurom);
    }
  }

  if(auto memory = node["memory(type=RAM,content=Save)"]) {
    loadMemory(superfx.ram, memory, File::Optional);
    for(auto map : memory.find("map")) {
      loadMap(map, superfx.cpuram);
    }
  }
}

//processor(architecture=ARM6)
void Cartridge::loadARMDSP(nall::Markup::Node node) {
  has.ARMDSP = true;

  for(auto& word : armdsp.programROM) word = 0x00;
  for(auto& word : armdsp.dataROM) word = 0x00;
  for(auto& word : armdsp.programRAM) word = 0x00;

  if(auto oscillator = game.oscillator()) {
    armdsp.Frequency = oscillator->frequency;
  } else {
    armdsp.Frequency = 21'440'000;
  }

  for(auto map : node.find("map")) {
    loadMap(map, {&ArmDSP::read, &armdsp}, {&ArmDSP::write, &armdsp});
  }

  if(auto memory = node["memory(type=ROM,content=Program,architecture=ARM6)"]) {
    if(auto file = game.memory(memory)) {
      std::ifstream firmfile = platform->fopen(ID::SuperFamicom, std::string(file->name()));
      if (firmfile.is_open()) {
        firmfile.read((char*)armdsp.programROM, (128 * 1024));
        firmfile.close();
      }
    }
  }

  if(auto memory = node["memory(type=ROM,content=Data,architecture=ARM6)"]) {
    if(auto file = game.memory(memory)) {
      std::ifstream firmfile = platform->fopen(ID::SuperFamicom, std::string(file->name()));
      if (firmfile.is_open()) {
        firmfile.read((char*)armdsp.dataROM, (32 * 1024));
        firmfile.close();
      }
    }
  }

  if(auto memory = node["memory(type=RAM,content=Data,architecture=ARM6)"]) {
    if(auto file = game.memory(memory)) {
      std::ifstream sramfile = platform->fopen(ID::SuperFamicom, "save.ram");
      if (sramfile.is_open()) {
        sramfile.read((char*)armdsp.programRAM, (16 * 1024));
        sramfile.close();
      }
    }
  }
}

//processor(architecture=HG51BS169)
void Cartridge::loadHitachiDSP(nall::Markup::Node node, unsigned roms) {
  for(auto& word : hitachidsp.dataROM) word = 0x000000;
  for(auto& word : hitachidsp.dataRAM) word = 0x00;

  if(auto oscillator = game.oscillator()) {
    hitachidsp.Frequency = oscillator->frequency;
  } else {
    hitachidsp.Frequency = 20'000'000;
  }
  hitachidsp.Roms = roms;  //1 or 2
  hitachidsp.Mapping = 0;  //0 or 1

  if(auto memory = node["memory(type=ROM,content=Program)"]) {
    loadMemory(hitachidsp.rom, memory, File::Required);
    for(auto map : memory.find("map")) {
      loadMap(map, {&HitachiDSP::readROM, &hitachidsp}, {&HitachiDSP::writeROM, &hitachidsp});
    }
  }

  if(auto memory = node["memory(type=RAM,content=Save)"]) {
    loadMemory(hitachidsp.ram, memory, File::Optional);
    for(auto map : memory.find("map")) {
      loadMap(map, {&HitachiDSP::readRAM, &hitachidsp}, {&HitachiDSP::writeRAM, &hitachidsp});
    }
  }

  if(configuration.hacks.coprocessor.preferHLE) {
    has.Cx4 = true;
    for(auto map : node.find("map")) {
      loadMap(map, {&Cx4::read, &cx4}, {&Cx4::write, &cx4});
    }
    if(auto memory = node["memory(type=RAM,content=Data,architecture=HG51BS169)"]) {
      for(auto map : memory.find("map")) {
        loadMap(map, {&Cx4::read, &cx4}, {&Cx4::write, &cx4});
      }
    }
    return;
  }

  if(auto memory = node["memory(type=ROM,content=Data,architecture=HG51BS169)"]) {
    if(auto file = game.memory(memory)) {
      for(auto n : nall::range(1 * 1024)) {
        hitachidsp.dataROM[n]  = hitachidsp.staticDataROM[n * 3 + 0] <<  0;
        hitachidsp.dataROM[n] |= hitachidsp.staticDataROM[n * 3 + 1] <<  8;
        hitachidsp.dataROM[n] |= hitachidsp.staticDataROM[n * 3 + 2] << 16;
      }
    }
  }

  if(auto memory = node["memory(type=RAM,content=Data,architecture=HG51BS169)"]) {
    if(auto file = game.memory(memory)) {
      std::ifstream sramfile = platform->fopen(ID::SuperFamicom, "save.ram");
      if (sramfile.is_open()) {
        sramfile.read((char*)hitachidsp.dataRAM, (3 * 1024));
        sramfile.close();
      }
    }
    for(auto map : memory.find("map")) {
      loadMap(map, {&HitachiDSP::readDRAM, &hitachidsp}, {&HitachiDSP::writeDRAM, &hitachidsp});
    }
  }

  has.HitachiDSP = true;

  for(auto map : node.find("map")) {
    loadMap(map, {&HitachiDSP::readIO, &hitachidsp}, {&HitachiDSP::writeIO, &hitachidsp});
  }
}

//processor(architecture=uPD7725)
void Cartridge::loaduPD7725(nall::Markup::Node node) {
  for(auto& word : necdsp.programROM) word = 0x000000;
  for(auto& word : necdsp.dataROM) word = 0x0000;
  for(auto& word : necdsp.dataRAM) word = 0x0000;

  if(auto oscillator = game.oscillator()) {
    necdsp.Frequency = oscillator->frequency;
  } else {
    necdsp.Frequency = 7'600'000;
  }

  bool failed = false;

  if(auto memory = node["memory(type=ROM,content=Program,architecture=uPD7725)"]) {
    if(auto file = game.memory(memory)) {
      /*if(auto fp = platform->open(ID::SuperFamicom, std::string(file->name()), File::Read)) {
        for(auto n : nall::range(2048)) necdsp.programROM[n] = fp->readl(3);
      } else*/ failed = true;
    }
  }

  if(auto memory = node["memory(type=ROM,content=Data,architecture=uPD7725)"]) {
    if(auto file = game.memory(memory)) {
      /*if(auto fp = platform->open(ID::SuperFamicom, std::string(file->name()), File::Read)) {
        for(auto n : nall::range(1024)) necdsp.dataROM[n] = fp->readl(2);
      } else*/ failed = true;
    }
  }

  if(failed || configuration.hacks.coprocessor.preferHLE) {
    auto manifest = nall::BML::serialize(game.document);
    if(manifest.find("identifier: DSP1")) {  //also matches DSP1B
      has.DSP1 = true;
      for(auto map : node.find("map")) {
        loadMap(map, {&DSP1::read, &dsp1}, {&DSP1::write, &dsp1});
      }
      return;
    }
    if(manifest.find("identifier: DSP2")) {
      has.DSP2 = true;
      for(auto map : node.find("map")) {
        loadMap(map, {&DSP2::read, &dsp2}, {&DSP2::write, &dsp2});
      }
      return;
    }
    if(manifest.find("identifier: DSP4")) {
      has.DSP4 = true;
      for(auto map : node.find("map")) {
        loadMap(map, {&DSP4::read, &dsp4}, {&DSP4::write, &dsp4});
      }
      return;
    }
  }

  if(failed) {
    //throw an error to the user
    //platform->open(ID::SuperFamicom, "DSP3", File::Read, File::Required);
    return;
  }

  if(auto memory = node["memory(type=RAM,content=Data,architecture=uPD7725)"]) {
    if(auto file = game.memory(memory)) {
      std::ifstream sramfile = platform->fopen(ID::SuperFamicom, "save.ram");
      if (sramfile.is_open()) {
        sramfile.read((char*)necdsp.dataRAM, 2 * 256);
        sramfile.close();
      }
    }
    for(auto map : memory.find("map")) {
      loadMap(map, {&NECDSP::readRAM, &necdsp}, {&NECDSP::writeRAM, &necdsp});
    }
  }

  has.NECDSP = true;
  necdsp.revision = NECDSP::Revision::uPD7725;

  for(auto map : node.find("map")) {
    loadMap(map, {&NECDSP::read, &necdsp}, {&NECDSP::write, &necdsp});
  }
}

//processor(architecture=uPD96050)
void Cartridge::loaduPD96050(nall::Markup::Node node) {
  for(auto& word : necdsp.programROM) word = 0x000000;
  for(auto& word : necdsp.dataROM) word = 0x0000;
  for(auto& word : necdsp.dataRAM) word = 0x0000;

  if(auto oscillator = game.oscillator()) {
    necdsp.Frequency = oscillator->frequency;
  } else {
    necdsp.Frequency = 11'000'000;
  }

  bool failed = false;

  if(auto memory = node["memory(type=ROM,content=Program,architecture=uPD96050)"]) {
    if(auto file = game.memory(memory)) {
      /*if(auto fp = platform->open(ID::SuperFamicom, std::string(file->name()), File::Read)) {
        for(auto n : nall::range(16384)) necdsp.programROM[n] = fp->readl(3);
      } else*/ failed = true;
    }
  }

  if(auto memory = node["memory(type=ROM,content=Data,architecture=uPD96050)"]) {
    if(auto file = game.memory(memory)) {
      /*if(auto fp = platform->open(ID::SuperFamicom, std::string(file->name()), File::Read)) {
        for(auto n : nall::range(2048)) necdsp.dataROM[n] = fp->readl(2);
      } else*/ failed = true;
    }
  }

  if(failed || configuration.hacks.coprocessor.preferHLE) {
    auto manifest = nall::BML::serialize(game.document);
    if(manifest.find("identifier: ST010")) {
      has.ST0010 = true;
      if(auto memory = node["memory(type=RAM,content=Data,architecture=uPD96050)"]) {
        for(auto map : memory.find("map")) {
          loadMap(map, {&ST0010::read, &st0010}, {&ST0010::write, &st0010});
        }
      }
      return;
    }
  }

  if(failed) {
    //throw an error to the user
    //platform->open(ID::SuperFamicom, "ST011", File::Read, File::Required);
    return;
  }

  if(auto memory = node["memory(type=RAM,content=Data,architecture=uPD96050)"]) {
    if(auto file = game.memory(memory)) {
      std::ifstream sramfile = platform->fopen(ID::SuperFamicom, "save.ram");
      if (sramfile.is_open()) {
        sramfile.read((char*)necdsp.dataRAM, (4 * 1024));
        sramfile.close();
      }
    }
    for(auto map : memory.find("map")) {
      loadMap(map, {&NECDSP::readRAM, &necdsp}, {&NECDSP::writeRAM, &necdsp});
    }
  }

  has.NECDSP = true;
  necdsp.revision = NECDSP::Revision::uPD96050;

  for(auto map : node.find("map")) {
    loadMap(map, {&NECDSP::read, &necdsp}, {&NECDSP::write, &necdsp});
  }
}

//rtc(manufacturer=Epson)
void Cartridge::loadEpsonRTC(nall::Markup::Node node) {
  has.EpsonRTC = true;

  epsonrtc.initialize();

  for(auto map : node.find("map")) {
    loadMap(map, {&EpsonRTC::read, &epsonrtc}, {&EpsonRTC::write, &epsonrtc});
  }

  if(auto memory = node["memory(type=RTC,content=Time,manufacturer=Epson)"]) {
    if(auto file = game.memory(memory)) {
      std::ifstream rtcfile = platform->fopen(ID::SuperFamicom, "time.rtc");
      if (rtcfile.is_open()) {
        uint8_t data[16] = {0};
        for(auto& byte : data) byte = rtcfile.get();
        epsonrtc.load(data);
        rtcfile.close();
      }
    }
  }
}

//rtc(manufacturer=Sharp)
void Cartridge::loadSharpRTC(nall::Markup::Node node) {
  has.SharpRTC = true;

  sharprtc.initialize();

  for(auto map : node.find("map")) {
    loadMap(map, {&SharpRTC::read, &sharprtc}, {&SharpRTC::write, &sharprtc});
  }

  if(auto memory = node["memory(type=RTC,content=Time,manufacturer=Sharp)"]) {
    if(auto file = game.memory(memory)) {
      std::ifstream rtcfile = platform->fopen(ID::SuperFamicom, "time.rtc");
      if (rtcfile.is_open()) {
        uint8_t data[16] = {0};
        for(auto& byte : data) byte = rtcfile.get();
        sharprtc.load(data);
        rtcfile.close();
      }
    }
  }
}

//processor(identifier=SPC7110)
void Cartridge::loadSPC7110(nall::Markup::Node node) {
  has.SPC7110 = true;

  for(auto map : node.find("map")) {
    loadMap(map, {&SPC7110::read, &spc7110}, {&SPC7110::write, &spc7110});
  }

  if(auto mcu = node["mcu"]) {
    for(auto map : mcu.find("map")) {
      loadMap(map, {&SPC7110::mcuromRead, &spc7110}, {&SPC7110::mcuromWrite, &spc7110});
    }
    if(auto memory = mcu["memory(type=ROM,content=Program)"]) {
      loadMemory(spc7110.prom, memory, File::Required);
    }
    if(auto memory = mcu["memory(type=ROM,content=Data)"]) {
      loadMemory(spc7110.drom, memory, File::Required);
    }
  }

  if(auto memory = node["memory(type=RAM,content=Save)"]) {
    loadMemory(spc7110.ram, memory, File::Optional);
    for(auto map : memory.find("map")) {
      loadMap(map, {&SPC7110::mcuramRead, &spc7110}, {&SPC7110::mcuramWrite, &spc7110});
    }
  }
}

//processor(identifier=SDD1)
void Cartridge::loadSDD1(nall::Markup::Node node) {
  has.SDD1 = true;

  for(auto map : node.find("map")) {
    loadMap(map, {&SDD1::ioRead, &sdd1}, {&SDD1::ioWrite, &sdd1});
  }

  if(auto mcu = node["mcu"]) {
    for(auto map : mcu.find("map")) {
      loadMap(map, {&SDD1::mcuRead, &sdd1}, {&SDD1::mcuWrite, &sdd1});
    }
    if(auto memory = mcu["memory(type=ROM,content=Program)"]) {
      loadMemory(sdd1.rom, memory, File::Required);
    }
  }
}

//processor(identifier=OBC1)
void Cartridge::loadOBC1(nall::Markup::Node node) {
  has.OBC1 = true;

  for(auto map : node.find("map")) {
    loadMap(map, {&OBC1::read, &obc1}, {&OBC1::write, &obc1});
  }

  if(auto memory = node["memory(type=RAM,content=Save)"]) {
    loadMemory(obc1.ram, memory, File::Optional);
  }
}

//file::exists("msu1/data.rom")
void Cartridge::loadMSU1() {
  has.MSU1 = true;

  bus.map({&MSU1::readIO, &msu1}, {&MSU1::writeIO, &msu1}, "00-3f,80-bf:2000-2007");
}

void Cartridge::saveCartridge(nall::Markup::Node node) {
  if(auto node = board["memory(type=RAM,content=Save)"]) saveRAM(node);
  if(auto node = board["processor(identifier=MCC)"]) saveMCC(node);
  if(auto node = board["processor(architecture=W65C816S)"]) saveSA1(node);
  if(auto node = board["processor(architecture=GSU)"]) saveSuperFX(node);
  if(auto node = board["processor(architecture=ARM6)"]) saveARMDSP(node);
  if(auto node = board["processor(architecture=HG51BS169)"]) saveHitachiDSP(node);
  if(auto node = board["processor(architecture=uPD7725)"]) saveuPD7725(node);
  if(auto node = board["processor(architecture=uPD96050)"]) saveuPD96050(node);
  if(auto node = board["rtc(manufacturer=Epson)"]) saveEpsonRTC(node);
  if(auto node = board["rtc(manufacturer=Sharp)"]) saveSharpRTC(node);
  if(auto node = board["processor(identifier=SPC7110)"]) saveSPC7110(node);
  if(auto node = board["processor(identifier=OBC1)"]) saveOBC1(node);
}

void Cartridge::saveCartridgeBSMemory(nall::Markup::Node node) {
  if(auto memory = Emulator::Game::Memory{node["game/board/memory(type=Flash,content=Program)"]}) {
    if (bsmemory.memory.data() != nullptr) {
      platform->write(bsmemory.pathID, std::string(memory.name()), bsmemory.memory.data(), memory.size);
    }
  }
}

void Cartridge::saveCartridgeSufamiTurboA(nall::Markup::Node node) {
  if(auto memory = Emulator::Game::Memory{node["game/board/memory(type=RAM,content=Save)"]}) {
    if(memory.nonVolatile) {
      platform->write(sufamiturboA.pathID, std::string(memory.name()), sufamiturboA.ram.data(), memory.size);
    }
  }
}

void Cartridge::saveCartridgeSufamiTurboB(nall::Markup::Node node) {
  if(auto memory = Emulator::Game::Memory{node["game/board/memory(type=RAM,content=Save)"]}) {
    if(memory.nonVolatile) {
      platform->write(sufamiturboB.pathID, std::string(memory.name()), sufamiturboB.ram.data(), memory.size);
    }
  }
}

void Cartridge::saveMemory(Memory& ram, nall::Markup::Node node) {
  if(auto memory = game.memory(node)) {
    if(memory->type == "RAM" && !memory->nonVolatile) return;
    if(memory->type == "RTC" && !memory->nonVolatile) return;
    platform->write(pathID(), std::string(memory->name()), ram.data(), ram.size());
  }
}

//memory(type=RAM,content=Save)
void Cartridge::saveRAM(nall::Markup::Node node) {
  saveMemory(ram, node);
}

//processor(identifier=MCC)
void Cartridge::saveMCC(nall::Markup::Node node) {
  if(auto mcu = node["mcu"]) {
    if(auto memory = mcu["memory(type=RAM,content=Download)"]) {
      saveMemory(mcc.psram, memory);
    }
  }
}

//processor(architecture=W65C816S)
void Cartridge::saveSA1(nall::Markup::Node node) {
  if(auto memory = node["memory(type=RAM,content=Save)"]) {
    saveMemory(sa1.bwram, memory);
  }

  if(auto memory = node["memory(type=RAM,content=Internal)"]) {
    saveMemory(sa1.iram, memory);
  }
}

//processor(architecture=GSU)
void Cartridge::saveSuperFX(nall::Markup::Node node) {
  if(auto memory = node["memory(type=RAM,content=Save)"]) {
    saveMemory(superfx.ram, memory);
  }
}

//processor(architecture=ARM6)
void Cartridge::saveARMDSP(nall::Markup::Node node) {
  if(auto memory = node["memory(type=RAM,content=Data,architecture=ARM6)"]) {
    if(auto file = game.memory(memory)) {
      if(file->nonVolatile) {
        platform->write(ID::SuperFamicom, "save.ram", armdsp.programRAM, (16 * 1024));
      }
    }
  }
}

//processor(architecture=HG51BS169)
void Cartridge::saveHitachiDSP(nall::Markup::Node node) {
  saveMemory(hitachidsp.ram, node["ram"]);

  if(auto memory = node["memory(type=RAM,content=Save)"]) {
    saveMemory(hitachidsp.ram, memory);
  }

  if(auto memory = node["memory(type=RAM,content=Data,architecture=HG51BS169)"]) {
    if(auto file = game.memory(memory)) {
      if(file->nonVolatile) {
        platform->write(ID::SuperFamicom, "save.ram", hitachidsp.dataRAM, (3 * 1024));
      }
    }
  }
}

//processor(architecture=uPD7725)
void Cartridge::saveuPD7725(nall::Markup::Node node) {
  if(auto memory = node["memory(type=RAM,content=Data,architecture=uPD7725)"]) {
    if(auto file = game.memory(memory)) {
      if(file->nonVolatile) {
        platform->write(ID::SuperFamicom, "save.ram", (uint8_t*)necdsp.dataRAM, 2 * 256);
      }
    }
  }
}

//processor(architecture=uPD96050)
void Cartridge::saveuPD96050(nall::Markup::Node node) {
  if(auto memory = node["memory(type=RAM,content=Data,architecture=uPD96050)"]) {
    if(auto file = game.memory(memory)) {
      if(file->nonVolatile) {
        platform->write(ID::SuperFamicom, "save.ram", (uint8_t*)necdsp.dataRAM, (4 * 1024));
      }
    }
  }
}

//rtc(manufacturer=Epson)
void Cartridge::saveEpsonRTC(nall::Markup::Node node) {
  if(auto memory = node["memory(type=RTC,content=Time,manufacturer=Epson)"]) {
    if(auto file = game.memory(memory)) {
      if(file->nonVolatile) {
        uint8_t data[16] = {0};
        epsonrtc.save(data);
        platform->write(ID::SuperFamicom, "save.ram", data, 16);
      }
    }
  }
}

//rtc(manufacturer=Sharp)
void Cartridge::saveSharpRTC(nall::Markup::Node node) {
  if(auto memory = node["memory(type=RTC,content=Time,manufacturer=Sharp)"]) {
    if(auto file = game.memory(memory)) {
      if(file->nonVolatile) {
        uint8_t data[16] = {0};
        sharprtc.save(data);
        platform->write(ID::SuperFamicom, "save.ram", data, 16);
      }
    }
  }
}

//processor(identifier=SPC7110)
void Cartridge::saveSPC7110(nall::Markup::Node node) {
  if(auto memory = node["memory(type=RAM,content=Save)"]) {
    saveMemory(spc7110.ram, memory);
  }
}

//processor(identifier=OBC1)
void Cartridge::saveOBC1(nall::Markup::Node node) {
  if(auto memory = node["memory(type=RAM,content=Save)"]) {
    saveMemory(obc1.ram, memory);
  }
}

void Cartridge::serialize(serializer& s) {
  s.array(ram.data(), ram.size());
}

Cartridge cartridge;

std::vector<std::string> Cartridge::hashes() const {
  std::vector<std::string> hashes;
  hashes.push_back(game.sha256);
  if(!slotGameBoy.sha256.empty()) hashes.push_back(slotGameBoy.sha256);
  if(!slotBSMemory.sha256.empty()) hashes.push_back(slotBSMemory.sha256);
  if(!slotSufamiTurboA.sha256.empty()) hashes.push_back(slotSufamiTurboA.sha256);
  if(!slotSufamiTurboB.sha256.empty()) hashes.push_back(slotSufamiTurboB.sha256);
  return hashes;
}

std::vector<std::string> Cartridge::manifests() const {
  std::vector<std::string> manifests;

  manifests.push_back(std::string(
    nall::string{nall::BML::serialize(game.document), "\n", nall::BML::serialize(board)}
  ));

  if(slotGameBoy.document) manifests.push_back(std::string(nall::BML::serialize(slotGameBoy.document)));
  if(slotBSMemory.document) manifests.push_back(std::string(nall::BML::serialize(slotBSMemory.document)));
  if(slotSufamiTurboA.document) manifests.push_back(std::string(nall::BML::serialize(slotSufamiTurboA.document)));
  if(slotSufamiTurboB.document) manifests.push_back(std::string(nall::BML::serialize(slotSufamiTurboB.document)));
  return manifests;
}

std::vector<std::string> Cartridge::titles() const {
  std::vector<std::string> titles;
  titles.push_back(game.label);
  if(!slotGameBoy.label.empty()) titles.push_back(slotGameBoy.label);
  if(!slotBSMemory.label.empty()) titles.push_back(slotBSMemory.label);
  if(!slotSufamiTurboA.label.empty()) titles.push_back(slotSufamiTurboA.label);
  if(!slotSufamiTurboB.label.empty()) titles.push_back(slotSufamiTurboB.label);
  return titles;
}

std::string Cartridge::title() const {
  if(!slotGameBoy.label.empty()) return slotGameBoy.label;
  if(has.MCC && !slotBSMemory.label.empty()) return slotBSMemory.label;
  if(!slotBSMemory.label.empty())
    return game.label + " + " + slotBSMemory.label;
  if(!slotSufamiTurboA.label.empty() && !slotSufamiTurboB.label.empty())
    return slotSufamiTurboA.label + " + " + slotSufamiTurboB.label;
  if(!slotSufamiTurboA.label.empty()) return slotSufamiTurboA.label;
  if(!slotSufamiTurboB.label.empty()) return slotSufamiTurboB.label;
  if(has.Cx4 || has.DSP1 || has.DSP2 || has.DSP4 || has.ST0010)
    return "[HLE] " + game.label;
  return game.label;
}

bool Cartridge::load() {
  information = {};
  has = {};
  game = {};
  slotGameBoy = {};
  slotBSMemory = {};
  slotSufamiTurboA = {};
  slotSufamiTurboB = {};

  if(auto loaded = platform->load(ID::SuperFamicom, "Super Famicom", "sfc", {"Auto", "NTSC", "PAL"})) {
    information.pathID = loaded.pathID;
    information.region = loaded.option;
  } else return false;

  std::string manifest = platform->stropen(ID::SuperFamicom, "manifest.bml");

  if (!manifest.empty()) {
    game.load(manifest);
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
        for (int i = 0; i < sufamiturboA.rom.size(); ++i)
            carts.push_back(sufamiturboA.rom[i]);
    }
    if(cartridge.has.SufamiTurboSlotB) {
        for (int i = 0; i < sufamiturboB.rom.size(); ++i)
            carts.push_back(sufamiturboB.rom[i]);
    }

    information.sha256 = sha256_digest(carts.data(), carts.size());
  }

  //Super Famicom
  else {
    // Again ugly, but can/should be rewritten cleanly when nall is gone
    std::vector<uint8_t> buf;

    //hash each ROM image that exists; any with size() == 0 is ignored
    for (int i = 0; i < rom.size(); ++i) buf.push_back(rom[i]);
    for (int i = 0; i < mcc.rom.size(); ++i) buf.push_back(mcc.rom[i]);
    for (int i = 0; i < sa1.rom.size(); ++i) buf.push_back(sa1.rom[i]);
    for (int i = 0; i < superfx.rom.size(); ++i) buf.push_back(superfx.rom[i]);
    for (int i = 0; i < hitachidsp.rom.size(); ++i) buf.push_back(hitachidsp.rom[i]);
    for (int i = 0; i < spc7110.prom.size(); ++i) buf.push_back(spc7110.prom[i]);
    for (int i = 0; i < spc7110.drom.size(); ++i) buf.push_back(spc7110.drom[i]);
    for (int i = 0; i < sdd1.rom.size(); ++i) buf.push_back(sdd1.rom[i]);

    //hash all firmware that exists
    std::vector<uint8_t> firm;
    firm = armdsp.firmware();
    for (int i = 0; i < firm.size(); ++i) buf.push_back(firm[i]);

    firm = hitachidsp.firmware();
    for (int i = 0; i < firm.size(); ++i) buf.push_back(firm[i]);

    firm = necdsp.firmware();
    for (int i = 0; i < firm.size(); ++i) buf.push_back(firm[i]);

    //finalize hash
    information.sha256 = sha256_digest(buf.data(), buf.size());
  }

  return true;
}

bool Cartridge::loadBSMemory() {
  std::string manifest = platform->stropen(bsmemory.pathID, "manifest.bml");

  if (!manifest.empty()) {
    slotBSMemory.load(manifest);
  }
  else {
      return false;
  }

  loadCartridgeBSMemory(slotBSMemory.document);
  return true;
}

bool Cartridge::loadSufamiTurboA() {
  std::string manifest = platform->stropen(sufamiturboA.pathID, "manifest.bml");

  if (!manifest.empty()) {
    slotSufamiTurboA.load(manifest);
  }
  else {
      return false;
  }

  loadCartridgeSufamiTurboA(slotSufamiTurboA.document);
  return true;
}

bool Cartridge::loadSufamiTurboB() {
  std::string manifest = platform->stropen(sufamiturboB.pathID, "manifest.bml");

  if (!manifest.empty()) {
    slotSufamiTurboB.load(manifest);
  }
  else {
      return false;
  }

  loadCartridgeSufamiTurboB(slotSufamiTurboB.document);
  return true;
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

void Cartridge::unload() {
  rom.reset();
  ram.reset();
}

}
