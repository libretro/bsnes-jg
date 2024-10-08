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

#include "audio.hpp"
#include "cheat.hpp"
#include "cartridge.hpp"
#include "cpu.hpp"
#include "settings.hpp"
#include "sha256.hpp"

#include "icd.hpp"

extern "C" {
  #include <gb/gb.h>
}

//warning: the size of this object will be too large due to C++ size rules differing from C rules.
//in practice, this won't pose a problem so long as the struct is never accessed from C++ code,
//as the offsets of all member variables will be wrong compared to what the C SameBoy code expects.
static GB_gameboy_t sameboy;

namespace SuperFamicom {

static Stream *stream;

ICD icd;

uint8_t& ICD::Packet::operator[](uint8_t address) {
  return data[address & 0x0f];
}

unsigned ICD::pathID() const {
  return information.pathID;
}

void ICD::ppuHreset() {
  hcounter = 0;
  vcounter++;
  if((vcounter & 0x07) == 0) writeBank = (writeBank + 1) & 0x03;
}

void ICD::ppuVreset() {
  hcounter = 0;
  vcounter = 0;
}

void ICD::ppuWrite(uint8_t color) {
  uint8_t x = hcounter++;
  uint8_t y = vcounter & 0x07;
  if(x >= 160) return;  //unverified behavior

  uint16_t address = (writeBank * 512 + y * 2 + x / 8 * 16) & 0x7ff;
  output[address + 0] = (output[address + 0] << 1) | !!(color & 1);
  output[address + 1] = (output[address + 1] << 1) | !!(color & 2);
}

void ICD::apuWrite(int16_t left, int16_t right) {
  int16_t samples[] = {left, right};
  if(!system.runAhead) stream->write(samples);
}

void ICD::joypWrite(bool p14, bool p15) {
  //joypad handling
  if(p14 == 1 && p15 == 1) {
    if(joypLock == 0) {
      joypLock = 1;
      joypID = (joypID + 1) & 0x03;
      if(mltReq == 0) joypID &= 0;  //1-player mode
      if(mltReq == 1) joypID &= 1;  //2-player mode
      if(mltReq == 2) joypID &= 3;  //4-player mode (unverified; but the most likely behavior)
      if(mltReq == 3) joypID &= 3;  //4-player mode
    }
  }

  uint8_t joypad = 0;
  if(joypID == 0) joypad = r6004;
  if(joypID == 1) joypad = r6005;
  if(joypID == 2) joypad = r6006;
  if(joypID == 3) joypad = r6007;

  uint8_t input = 0xf;
  if(p14 == 1 && p15 == 1) input = 0xf - joypID;
  if(p14 == 0) input &= (joypad >> 0 & 15);  //d-pad
  if(p15 == 0) input &= (joypad >> 4 & 15);  //buttons

  GB_icd_set_joyp(&sameboy, input);

  //if(p14 == 0 && p15 == 1); // Does nothing?

  if(p14 == 1 && p15 == 0) joypLock ^= 1;

  //packet handling
  if(p14 == 0 && p15 == 0) {  //pulse
    pulseLock = 0;
    packetOffset = 0;
    bitOffset = 0;
    strobeLock = 1;
    packetLock = 0;
    return;
  }

  if(pulseLock == 1) return;

  if(p14 == 1 && p15 == 1) {
    strobeLock = 0;
    return;
  }

  if(strobeLock == 1) {
    if(p14 == 1 || p15 == 1) {  //malformed packet
      packetLock = 0;
      pulseLock = 1;
      bitOffset = 0;
      packetOffset = 0;
    }
    else {
      return;
    }
  }

  //p14:0, p15:1 = 0
  //p14:1, p15:0 = 1
  bool bit = p15 == 0;
  strobeLock = 1;

  if(packetLock == 1) {
    if(p14 == 0 && p15 == 1) {
      if(packetSize < 64) {
        packet[packetSize] = joypPacket;
        packetSize = (packetSize + 1) & 0x7f;
      }
      packetLock = 0;
      pulseLock = 1;
    }
    return;
  }

  bitData = bit << 7 | bitData >> 1;
  bitOffset = (bitOffset + 1) & 0x07;
  if(bitOffset) return;

  joypPacket[packetOffset] = bitData;
  packetOffset = (packetOffset + 1) & 0x0f;
  if(packetOffset) return;

  packetLock = 1;
}

uint8_t ICD::readIO(unsigned addr, uint8_t data) {
  addr &= 0x40ffff;

  //LY counter
  if(addr == 0x6000) {
    return (vcounter & ~7) | writeBank;
  }

  //command ready port
  if(addr == 0x6002) {
    data = packetSize > 0;
    if(data) {
      for(unsigned n = 0; n < 16; ++n) r7000[n] = packet[0][n];
      packetSize--;
      for(unsigned n = 0; n < packetSize; ++n) packet[n] = packet[n + 1];
    }
    return data;
  }

  //ICD2 revision
  if(addr == 0x600f) {
    return 0x21;
  }

  //command port
  if((addr & 0x40fff0) == 0x7000) {
    return r7000[addr & 15];
  }

  //VRAM port
  if(addr == 0x7800) {
    data = output[readBank * 512 + readAddress];
    readAddress = (readAddress + 1) & 0x1ff;
    return data;
  }

  return 0x00;
}

void ICD::writeIO(unsigned addr, uint8_t data) {
  addr &= 0xffff;

  //VRAM port
  if(addr == 0x6001) {
    readBank = data & 3;
    readAddress = 0;
    return;
  }

  //control port
  //d7: 0 = halt, 1 = reset
  //d5,d4: 0 = 1-player, 1 = 2-player, 2 = 4-player, 3 = ???
  //d1,d0: 0 = frequency divider (clock rate adjust)
  if(addr == 0x6003) {
    if((r6003 & 0x80) == 0x00 && (data & 0x80) == 0x80) {
      power(true);  //soft reset
    }

    mltReq = data >> 4 & 3;
    if(mltReq == 0) joypID &= 3;  //1-player mode
    if(mltReq == 1) joypID &= 1;  //2-player mode
    if(mltReq == 2) joypID &= 0;  //4-player mode (unverified; but the most likely behavior)
    if(mltReq == 3) joypID &= 0;  //4-player mode

    unsigned freq = clockFrequency();
    switch(data & 3) {
    case 0: this->frequency = freq / 4; break;  //fast (glitchy, even on real hardware)
    case 1: this->frequency = freq / 5; break;  //normal
    case 2: this->frequency = freq / 7; break;  //slow
    case 3: this->frequency = freq / 9; break;  //very slow
    }

    stream->setFrequency(this->frequency / 128, 0);
    r6003 = data;
    return;
  }

  if(addr == 0x6004) { r6004 = data; return; }  //joypad 1
  if(addr == 0x6005) { r6005 = data; return; }  //joypad 2
  if(addr == 0x6006) { r6006 = data; return; }  //joypad 3
  if(addr == 0x6007) { r6007 = data; return; }  //joypad 4
}

static const uint8_t SGB1BootROM[256] = {
  49,254,255,62,48,224,0,175,33,255,159,50,203,124,32,251,33,38,255,14,17,62,128,50,226,12,62,243,226,50,62,119,
  119,62,252,224,71,33,95,192,14,8,175,50,13,32,252,17,79,1,62,251,14,6,245,6,0,26,27,50,128,71,13,32,
  248,50,241,50,14,14,214,2,254,239,32,234,17,4,1,33,16,128,26,205,211,0,205,212,0,19,123,254,52,32,243,17,
  230,0,6,8,26,19,34,35,5,32,249,62,25,234,16,153,33,47,153,14,12,61,40,8,50,13,32,249,46,15,24,243,
  62,145,224,64,33,0,192,14,0,62,0,226,62,48,226,6,16,30,8,42,87,203,66,62,16,32,2,62,32,226,62,48,
  226,203,26,29,32,239,5,32,232,62,32,226,62,48,226,205,194,0,125,254,96,32,210,14,19,62,193,226,12,62,7,226,
  24,58,22,4,240,68,254,144,32,250,30,0,29,32,253,21,32,242,201,79,6,4,197,203,17,23,193,203,17,23,5,32,
  245,34,35,34,35,201,60,66,185,165,185,165,66,60,0,0,0,0,0,0,0,0,0,0,0,0,0,0,62,1,224,80,
};

static const uint8_t SGB2BootROM[256] = {
  49,254,255,62,48,224,0,175,33,255,159,50,203,124,32,251,33,38,255,14,17,62,128,50,226,12,62,243,226,50,62,119,
  119,62,252,224,71,33,95,192,14,8,175,50,13,32,252,17,79,1,62,251,14,6,245,6,0,26,27,50,128,71,13,32,
  248,50,241,50,14,14,214,2,254,239,32,234,17,4,1,33,16,128,26,205,211,0,205,212,0,19,123,254,52,32,243,17,
  230,0,6,8,26,19,34,35,5,32,249,62,25,234,16,153,33,47,153,14,12,61,40,8,50,13,32,249,46,15,24,243,
  62,145,224,64,33,0,192,14,0,62,0,226,62,48,226,6,16,30,8,42,87,203,66,62,16,32,2,62,32,226,62,48,
  226,203,26,29,32,239,5,32,232,62,32,226,62,48,226,205,194,0,125,254,96,32,210,14,19,62,193,226,12,62,7,226,
  24,58,22,4,240,68,254,144,32,250,30,0,29,32,253,21,32,242,201,79,6,4,197,203,17,23,193,203,17,23,5,32,
  245,34,35,34,35,201,60,66,185,165,185,165,66,60,0,0,0,0,0,0,0,0,0,0,0,0,0,0,62,255,224,80,
};

void ICD::serialize(serializer& s) {
  Thread::serialize(s);

  size_t size = GB_get_save_state_size(&sameboy);
  uint8_t *data = new uint8_t[size];

  if(s.mode() == serializer::Save) {
    GB_save_state_to_buffer(&sameboy, data);
  }

  s.array(data, size);
  if(s.mode() == serializer::Load) {
    GB_load_state_from_buffer(&sameboy, data, size);
  }

  delete[] data;

  for(unsigned n = 0; n < 64; ++n) s.array(packet[n].data);
  s.integer(packetSize);

  s.integer(joypID);
  s.integer(joypLock);
  s.integer(pulseLock);
  s.integer(strobeLock);
  s.integer(packetLock);
  s.array(joypPacket.data);
  s.integer(packetOffset);
  s.integer(bitData);
  s.integer(bitOffset);

  s.array(output);
  s.integer(readBank);
  s.integer(readAddress);
  s.integer(writeBank);

  s.integer(r6003);
  s.integer(r6004);
  s.integer(r6005);
  s.integer(r6006);
  s.integer(r6007);
  s.array(r7000);
  s.integer(mltReq);

  s.integer(hcounter);
  s.integer(vcounter);
}

namespace SameBoy {
  static void hreset(GB_gameboy_t*) {
    icd.ppuHreset();
  }

  static void vreset(GB_gameboy_t*) {
    icd.ppuVreset();
  }

  static void icd_pixel(GB_gameboy_t*, uint8_t pixel) {
    icd.ppuWrite(pixel & 0x03);
  }

  static void joyp_write(GB_gameboy_t*, uint8_t value) {
    bool p14 = value & 0x10;
    bool p15 = value & 0x20;
    icd.joypWrite(p14, p15);
  }

  static uint8_t read_memory(GB_gameboy_t*, uint16_t addr, uint8_t data) {
    uint8_t replace;
    if(cheats.find(&replace, addr, data)) return replace;
    return data;
  }

  static uint32_t rgb_encode(GB_gameboy_t*, uint8_t r, uint8_t g, uint8_t b) {
    return r << 16 | g << 8 | b << 0;
  }

  static void sample(GB_gameboy_t*, GB_sample_t* sample) {
    int16_t left  = sample->left;
    int16_t right = sample->right;
    icd.apuWrite(left, right);
  }

  static void vblank(GB_gameboy_t*, GB_vblank_type_t) {
  }

  static void log(GB_gameboy_t *gb, const char *string, GB_log_attributes attributes) {
      if (gb || string || attributes) {}
  }
}

void ICD::synchronizeCPU() {
  if(clock >= 0) scheduler.resume(cpu.thread);
}

[[noreturn]] static void Enter() {
  while(true) {
    scheduler.synchronize();
    icd.main();
  }
}

void ICD::main() {
  if(r6003 & 0x80) {
    unsigned clocks = GB_run(&sameboy);
    step(clocks >> 1);
  }
  else {  //DMG halted
    apuWrite(0, 0);
    step(128);
  }
  synchronizeCPU();
}

void ICD::step(unsigned clocks) {
  clock += clocks * (uint64_t)cpu.frequency;
}

//SGB1 uses the CPU oscillator (~2.4% faster than a real Game Boy)
//SGB2 uses a dedicated oscillator (same speed as a real Game Boy)
unsigned ICD::clockFrequency() const {
  return Frequency ? Frequency : system.cpuFrequency();
}

void* ICD::sramData() {
  size_t sram_size = sramSize();
  if (sram_size) {
    uint16_t bank;
    return GB_get_direct_access(&sameboy, GB_DIRECT_ACCESS_CART_RAM, &sram_size, &bank);
  }
  return nullptr;
}

unsigned ICD::sramSize() {
  return GB_save_battery_size(&sameboy);
}

bool ICD::load() {
  information = {};

  GB_random_set_enabled((bool)configuration.entropy);
  if(Frequency == 0) {
    GB_init(&sameboy, GB_MODEL_SGB_NO_SFC);
    GB_load_boot_rom_from_buffer(&sameboy, (const unsigned char*)&SGB1BootROM[0], 256);
  }
  else {
    GB_init(&sameboy, GB_MODEL_SGB2_NO_SFC);
    GB_load_boot_rom_from_buffer(&sameboy, (const unsigned char*)&SGB2BootROM[0], 256);
  }
  GB_set_sample_rate_by_clocks(&sameboy, 256);
  GB_set_highpass_filter_mode(&sameboy, GB_HIGHPASS_ACCURATE);
  GB_set_icd_hreset_callback(&sameboy, &SameBoy::hreset);
  GB_set_icd_vreset_callback(&sameboy, &SameBoy::vreset);
  GB_set_icd_pixel_callback(&sameboy, &SameBoy::icd_pixel);
  GB_set_joyp_write_callback(&sameboy, &SameBoy::joyp_write);
  GB_set_read_memory_callback(&sameboy, &SameBoy::read_memory);
  GB_set_rgb_encode_callback(&sameboy, &SameBoy::rgb_encode);
  GB_apu_set_sample_callback(&sameboy, &SameBoy::sample);
  GB_set_vblank_callback(&sameboy, &SameBoy::vblank);
  GB_set_log_callback(&sameboy, &SameBoy::log);
  GB_set_pixels_output(&sameboy, &bitmap[0]);

  information.pathID = 2; // TODO: Magic numbers are bad

  cartridge.information.sha256 = sha256_digest(romdata, romsize).c_str();
  GB_load_rom_from_buffer(&sameboy, romdata, romsize);

  std::vector<uint8_t> sram;
  if (openFileCallback(udata_v, "save.ram", sram)) {
    GB_load_battery_from_buffer(&sameboy, (const uint8_t*)sram.data(), sram.size());
  }
  return true;
}

void ICD::save() {
  if(int size = GB_save_battery_size(&sameboy)) {
    uint8_t *data = (uint8_t*)malloc(size);
    GB_save_battery_to_buffer(&sameboy, data, size);
    writeCallback(udata_wr, "save.ram", data, size);
    free(data);
  }
}

void ICD::unload() {
  //save(); // redundant
  GB_free(&sameboy);
  destroy();
}

void ICD::power(bool reset) {
  unsigned freq = clockFrequency() / 5;
  create(Enter, freq);
  if(!reset) stream = audio.createStream(freq / 128);

  for(Packet& p : this->packet) p = {};
  packetSize = 0;

  joypID = 0;
  joypLock = 1;
  pulseLock = 1;
  strobeLock = 0;
  packetLock = 0;
  joypPacket = {};
  packetOffset = 0;
  bitData = 0;
  bitOffset = 0;

  for(uint8_t& n : output) n = 0xff;
  readBank = 0;
  readAddress = 0;
  writeBank = 0;

  r6003 = 0x00;
  r6004 = 0xff;
  r6005 = 0xff;
  r6006 = 0xff;
  r6007 = 0xff;
  for(uint8_t& r : r7000) r = 0x00;
  mltReq = 0;

  hcounter = 0;
  vcounter = 0;

  GB_reset(&sameboy);
}

void ICD::setRom(const uint8_t *data, size_t size) {
  romdata = data;
  romsize = size;
}

void ICD::setOpenFileCallback(void *ptr, bool (*cb)(void*, std::string, std::vector<uint8_t>&)) {
  openFileCallback = cb;
  udata_v = ptr;
}

void ICD::setWriteCallback(void *ptr, void (*cb)(void*, std::string, const uint8_t*, unsigned)) {
  writeCallback = cb;
  udata_wr = ptr;
}

}
