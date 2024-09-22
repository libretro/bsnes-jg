/*
 * bsnes-jg - Super Nintendo emulator
 *
 * Copyright (C) 2004-2020 byuu
 * Copyright (C) 2020-2024 Rupert Carmichael
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
#include <cstddef>
#include <cstring>

#include "audio.hpp"
#include "cartridge.hpp"
#include "cheat.hpp"
#include "controller.hpp"
#include "cpu.hpp"
#include "coprocessor/dip.hpp"
#include "coprocessor/icd.hpp"
#include "coprocessor/msu1.hpp"
#include "dsp.hpp"
#include "expansion/expansion.hpp"
#include "logger.hpp"
#include "ppu.hpp"
#include "serializer.hpp"
#include "settings.hpp"
#include "system.hpp"

#include "bsnes.hpp"

namespace SuperFamicom {
Configuration configuration;
};

bool Bsnes::loaded() {
  return SuperFamicom::system.loaded();
}

bool Bsnes::load() {
  return SuperFamicom::system.load();
}

void Bsnes::save() {
  SuperFamicom::system.save();
}

void Bsnes::unload() {
  SuperFamicom::system.unload();
}

void Bsnes::power() {
  SuperFamicom::system.power(/* reset = */ false);
}

void Bsnes::reset() {
  SuperFamicom::system.power(/* reset = */ true);
}

void Bsnes::run() {
  SuperFamicom::system.run();
}

void Bsnes::runAhead(unsigned frames) {
  if (!frames)
    return;

  SuperFamicom::system.runAhead = true;
  SuperFamicom::system.run();
  serializer s = SuperFamicom::system.serialize(false); // deterministic

  for (unsigned i = 0; i < frames - 1; ++i)
    SuperFamicom::system.run();

  SuperFamicom::system.runAhead = false;
  SuperFamicom::system.run();
  s.setMode(serializer::Mode::Load);
  SuperFamicom::system.unserialize(s);
}

bool Bsnes::getRtcPresent() {
  return (SuperFamicom::cartridge.has.EpsonRTC || SuperFamicom::cartridge.has.SharpRTC);
}

std::pair<void*, unsigned> Bsnes::getMemoryRaw(unsigned type) {
  switch (type) {
    default: case Memory::CartRAM:
    case Memory::RealTimeClock:
    case Memory::SGBCartRAM:
    case Memory::BSXDownloadRAM:
    case Memory::SufamiARAM:
    case Memory::SufamiBRAM: {
      return SuperFamicom::cartridge.getMemoryRaw(type);
    }
    case Memory::MainRAM: {
      return std::make_pair((void*)SuperFamicom::cpu.wram, 128 * 1024);
    }
    case Memory::VideoRAM: { // 16-bit
      return std::make_pair((void*)SuperFamicom::ppu.vram.data, (64 * 1024) << 1);
    }
  }
  return std::make_pair(nullptr, 0);
}

unsigned Bsnes::serializeSize() {
  return SuperFamicom::system.serializeSize(true);
}

unsigned Bsnes::serialize(uint8_t *data) {
  serializer s = SuperFamicom::system.serialize(true);
  std::memcpy(data, s.data(), s.size());
  return s.size();
}

bool Bsnes::unserialize(const uint8_t *data, unsigned size) {
  serializer s(data, size);
  return SuperFamicom::system.unserialize(s);
}

void Bsnes::cheatsClear() {
  if (!SuperFamicom::cartridge.has.ICD) {
    SuperFamicom::Memory::GlobalWriteEnable = true;
    for (SuperFamicom::Cheat::Code& chtcode : SuperFamicom::cheats.codes) {
      SuperFamicom::bus.write(chtcode.address, chtcode.restore);
    }
    SuperFamicom::Memory::GlobalWriteEnable = false;
  }

  SuperFamicom::cheats.reset();
}

void Bsnes::cheatsSetCheat(std::string code) {
  if (!(SuperFamicom::cartridge.has.ICD ? CheatDecoder::gb(code) : CheatDecoder::snes(code))) {
    logger.log(Logger::WRN, std::string("Failed to decode cheat: ") + code + "\n");
    return;
  }

  SuperFamicom::cheats.set(code);

  if (SuperFamicom::cartridge.has.ICD) {
    return;
  }

  SuperFamicom::Cheat::Code& chtcode = SuperFamicom::cheats.codes.back();
  chtcode.restore = SuperFamicom::bus.read(chtcode.address);

  SuperFamicom::Memory::GlobalWriteEnable = true;
  if (!chtcode.compare || chtcode.compare == chtcode.restore) {
    SuperFamicom::bus.write(chtcode.address, chtcode.data);
  }
  SuperFamicom::Memory::GlobalWriteEnable = false;
}

unsigned Bsnes::getRegion() {
  return SuperFamicom::cartridge.region() == "PAL" ? Region::PAL : Region::NTSC;
}

void Bsnes::setAudioSpec(Audio::Spec spec) {
  SuperFamicom::audio.setFrequency(spec.freq);
  SuperFamicom::audio.setSpf(spec.spf);
  SuperFamicom::audio.setQuality(spec.rsqual);
  SuperFamicom::audio.setBuffer(spec.buf);
  SuperFamicom::audio.setCallback(spec.ptr, spec.cb);
}

void Bsnes::setCoprocDelayedSync(bool value) {
  SuperFamicom::configuration.coprocessor.delayedSync = value;
}

void Bsnes::setCoprocPreferHLE(bool value) {
  SuperFamicom::configuration.coprocessor.preferHLE = value;
}

void Bsnes::setHotfixes(bool value) {
  SuperFamicom::configuration.hotfixes = value;
}

void Bsnes::setDIPSwitches(uint8_t value) {
  SuperFamicom::dip.value = value;
}

void Bsnes::setVideoColourParams(unsigned luminance, unsigned saturation, unsigned gamma) {
  SuperFamicom::ppu.genPalette(
    std::max(0.0, std::min(1.0, luminance / 100.0)),
    std::max(0.0, std::min(2.0, saturation / 100.0)),
    std::max(1.0, std::min(2.0, gamma / 100.0))
  );
}

void Bsnes::setSpcInterpolation(unsigned algo) {
  SuperFamicom::dsp.setInterpolation(algo);
}

void Bsnes::setInputSpec(Input::Spec spec) {
  switch(spec.port) {
    case SuperFamicom::ID::Port::Controller1:
      SuperFamicom::controllerPort1.connect(
        SuperFamicom::configuration.controllerPort1 = spec.device,
        spec.ptr, spec.cb
      );
      break;
    case SuperFamicom::ID::Port::Controller2:
      SuperFamicom::controllerPort2.connect(
        SuperFamicom::configuration.controllerPort2 = spec.device,
        spec.ptr, spec.cb
      );
      break;
    case SuperFamicom::ID::Port::Expansion:
      SuperFamicom::expansionPort.connect(
        SuperFamicom::configuration.expansionPort = spec.device
      );
      break;
  }
}

void Bsnes::setLogCallback(void *ptr, void (*cb)(void*, int, std::string&)) {
  logger.setCallback(ptr, cb);
}

void Bsnes::setOpenStreamCallback(void *ptr, bool (*cb)(void*, std::string, std::stringstream&)) {
  SuperFamicom::cartridge.setOpenStreamCallback(ptr, cb);
}

void Bsnes::setOpenFileCallback(void *ptr, bool (*cb)(void*, std::string, std::vector<uint8_t>&)) {
  SuperFamicom::cartridge.setOpenFileCallback(ptr, cb);
  SuperFamicom::icd.setOpenFileCallback(ptr, cb);
}

void Bsnes::setOpenMsuCallback(void *ptr, bool (*cb)(void*, std::string, std::istream**)) {
  SuperFamicom::msu1.setOpenMsuCallback(ptr, cb);
}

void Bsnes::setRegion(unsigned region) {
  if (region == Region::NTSC) {
    SuperFamicom::cartridge.setRegion("NTSC");
  }
  else {
    SuperFamicom::cartridge.setRegion("PAL");
  }
}

void Bsnes::setRomLoadCallback(void *ptr, bool (*cb)(void*, unsigned)) {
  SuperFamicom::cartridge.setRomCallback(ptr, cb);
}

void Bsnes::setRomBSMemory(std::vector<uint8_t>& data, std::string& loc) {
  SuperFamicom::cartridge.setRomBSMemory(data, loc);
}

void Bsnes::setRomGB(const uint8_t *data, size_t size) {
  SuperFamicom::icd.setRom(data, size);
}

void Bsnes::setRomSufamiTurboA(std::vector<uint8_t>& data, std::string& loc) {
  SuperFamicom::cartridge.setRomSufamiTurboA(data, loc);
}

void Bsnes::setRomSufamiTurboB(std::vector<uint8_t>& data, std::string& loc) {
  SuperFamicom::cartridge.setRomSufamiTurboB(data, loc);
}

void Bsnes::setRomSuperFamicom(std::vector<uint8_t>& data, std::string& loc) {
  SuperFamicom::cartridge.setRomSuperFamicom(data, loc);
}

void Bsnes::setWriteCallback(void *ptr, void (*cb)(void*, std::string, const uint8_t*, unsigned)) {
  SuperFamicom::cartridge.setWriteCallback(ptr, cb);
  SuperFamicom::icd.setWriteCallback(ptr, cb);
}

void Bsnes::setVideoSpec(Video::Spec spec) {
  SuperFamicom::ppu.setBuffer(spec.buf);
  SuperFamicom::ppu.setCallback(spec.ptr, spec.cb);
}
