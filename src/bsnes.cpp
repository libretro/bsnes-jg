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

#include <cstddef>

#include "audio.hpp"
#include "cartridge.hpp"
#include "cheat.hpp"
#include "controller.hpp"
#include "coprocessor/icd.hpp"
#include "coprocessor/msu1.hpp"
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
  save();
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

bool Bsnes::getRtcPresent() {
  return (SuperFamicom::cartridge.has.EpsonRTC || SuperFamicom::cartridge.has.SharpRTC);
}

void Bsnes::serialize(std::vector<uint8_t>& state, bool synchronize) {
  serializer s = SuperFamicom::system.serialize(synchronize);
  state = std::vector<uint8_t>(s.data(), s.data() + s.size());
}

bool Bsnes::unserialize(std::vector<uint8_t>& state) {
  serializer s(state.data(), state.size());
  return SuperFamicom::system.unserialize(s);
}

void Bsnes::cheatsApply(const std::vector<std::string>& list) {
  if(SuperFamicom::cartridge.has.ICD) {
    SuperFamicom::cheats.assign(list);
    return;
  }

  SuperFamicom::Memory::GlobalWriteEnable = true;

  for (SuperFamicom::Cheat::Code& code : SuperFamicom::cheats.codes) {
    SuperFamicom::bus.write(code.address, code.restore);
  }

  SuperFamicom::cheats.assign(list);

  for (SuperFamicom::Cheat::Code& code : SuperFamicom::cheats.codes) {
    code.restore = SuperFamicom::bus.read(code.address);
    if (!code.compare || code.compare == code.restore) {
      code.enable = true;
      SuperFamicom::bus.write(code.address, code.data);
    } else {
      code.enable = false;
    }
  }

  SuperFamicom::Memory::GlobalWriteEnable = false;
}

void Bsnes::cheatsClear() {
  std::vector<std::string> list{};
  cheatsApply(list);
}

std::string Bsnes::cheatsDecode(int sys, std::string code) {
  bool decoded = false;
  if (sys == 1) // Game Boy
    decoded = CheatDecoder::gb(code);
  else // Other
    decoded = CheatDecoder::snes(code);

  return decoded ? code : std::string{};
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

void Bsnes::setOpenFileCallback(void *ptr, bool (*cb)(void*, unsigned, std::string, std::vector<uint8_t>&)) {
  SuperFamicom::cartridge.setOpenFileCallback(ptr, cb);
  SuperFamicom::icd.setOpenFileCallback(ptr, cb);
  SuperFamicom::msu1.setOpenFileCallback(ptr, cb);
}

void Bsnes::setRegion(std::string region) {
  SuperFamicom::cartridge.setRegion(region);
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

void Bsnes::setWriteCallback(void *ptr, void (*cb)(void*, unsigned, std::string, const uint8_t*, unsigned)) {
  SuperFamicom::cartridge.setWriteCallback(ptr, cb);
  SuperFamicom::icd.setWriteCallback(ptr, cb);
}

void Bsnes::setVideoSpec(Video::Spec spec) {
  SuperFamicom::ppu.setPixelFormat(spec.pixfmt);
  SuperFamicom::ppu.setBuffer(spec.buf);
  SuperFamicom::ppu.setCallback(spec.cb);
}
