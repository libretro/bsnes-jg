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

#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include "sha256.hpp"

#include "heuristics.hpp"

namespace Heuristics {

std::string Memory::text() const {
  std::stringstream output;
  output << "    memory\n";
  output << "      type: " << _type << "\n";
  output << "      size: 0x" << std::hex << _size << "\n";
  output << "      content: " << _content << "\n";
  if(!_manufacturer.empty())
    output << "      manufacturer: " << _manufacturer << "\n";
  if(!_architecture.empty())
    output << "      architecture: " << _architecture << "\n";
  if(!_identifier.empty())
    output << "      identifier: " << _identifier << "\n";
  if(_volatile)
    output << "      volatile\n";
  return output.str();
}

std::string Oscillator::text() const {
  std::stringstream output;
  output << "    oscillator\n";
  output << "      frequency: " << _frequency << "\n";
  return output.str();
}

std::string Slot::text() const {
  std::string output;
  output += "    slot\n";
  output += "      type: " + _type + "\n";
  return output;
}

BSMemory::BSMemory(std::vector<uint8_t>& data, std::string location) : data(data), location(location) {
}

BSMemory::operator bool() const {
  return data.size() >= 0x8000;
}

std::string BSMemory::manifest() const {
  if(!operator bool()) return {};

  std::string gamename(location);
  gamename = gamename.substr(0, gamename.find_last_of("."));
  gamename = gamename.substr(gamename.find_last_of("/\\") + 1);

  std::string output;
  output += "game\n";
  output += "  sha256: " + sha256_digest(data.data(), data.size()) + "\n";
  output += "  label:  " + gamename + "\n";
  output += "  name:   " + gamename + "\n";
  output += "  board\n";
  output += Memory{}.type("Flash").size(data.size()).content("Program").text();
  return output;
}

GameBoy::GameBoy(std::vector<uint8_t>& data, std::string location) : data(data), location(location) {
  headerAddress = data.size() < 0x8000 ? data.size() : data.size() - 0x8000;
  if(read(0x0104) == 0xce && read(0x0105) == 0xed && read(0x0106) == 0x66 && read(0x0107) == 0x66
  && read(0x0108) == 0xcc && read(0x0109) == 0x0d && read(0x0147) >= 0x0b && read(0x0147) <= 0x0d
  ) {  //MMM01 stores header at bottom of data[]
  } else { //all other mappers store header at top of data[]
    headerAddress = 0;
  }
}

GameBoy::operator bool() const {
  return data.size() >= 0x4000;
}

std::string GameBoy::manifest() const {
  if(!operator bool()) return {};

  bool black = (read(0x0143) & 0xc0) == 0x80;  //cartridge works in DMG+CGB mode
  bool clear = (read(0x0143) & 0xc0) == 0xc0;  //cartridge works in CGB mode only

  bool ram = false;
  bool battery = false;
  bool eeprom = false;
  bool flash = false;
  bool rtc = false;
  bool accelerometer = false;
  bool rumble = false;

  //unsigned romSize = 0; // TODO - why is this set and unused below?
  unsigned ramSize = 0;
  unsigned eepromSize = 0;
  unsigned flashSize = 0;
  unsigned rtcSize = 0;

  std::string mapper = "MBC0";

  switch(read(0x0147)) {

  case 0x00:
    mapper = "MBC0";
    break;

  case 0x01:
    mapper = "MBC1";
    break;

  case 0x02:
    mapper = "MBC1";
    ram = true;
    break;

  case 0x03:
    mapper = "MBC1";
    battery = true;
    ram = true;
    break;

  case 0x05:
    mapper = "MBC2";
    ram = true;
    break;

  case 0x06:
    mapper = "MBC2";
    battery = true;
    ram = true;
    break;

  case 0x08:
    mapper = "MBC0";
    ram = true;
    break;

  case 0x09:
    mapper = "MBC0";
    battery = true;
    ram = true;
    break;

  case 0x0b:
    mapper = "MMM01";
    break;

  case 0x0c:
    mapper = "MMM01";
    ram = true;
    break;

  case 0x0d:
    mapper = "MMM01";
    battery = true;
    ram = true;
    break;

  case 0x0f:
    mapper = "MBC3";
    battery = true;
    rtc = true;
    break;

  case 0x10:
    mapper = "MBC3";
    battery = true;
    ram = true;
    rtc = true;
    break;

  case 0x11:
    mapper = "MBC3";
    break;

  case 0x12:
    mapper = "MBC3";
    ram = true;
    break;

  case 0x13:
    mapper = "MBC3";
    battery = true;
    ram = true;
    break;

  case 0x19:
    mapper = "MBC5";
    break;

  case 0x1a:
    mapper = "MBC5";
    ram = true;
    break;

  case 0x1b:
    mapper = "MBC5";
    battery = true;
    ram = true;
    break;

  case 0x1c:
    mapper = "MBC5";
    rumble = true;
    break;

  case 0x1d:
    mapper = "MBC5";
    ram = true;
    rumble = true;
    break;

  case 0x1e:
    mapper = "MBC5";
    battery = true;
    ram = true;
    rumble = true;
    break;

  case 0x20:
    mapper = "MBC6";
    flash = true;
    battery = true;
    ram = true;
    break;

  case 0x22:
    mapper = "MBC7";
    battery = true;
    eeprom = true;
    accelerometer = true;
    rumble = true;
    break;

  case 0xfc:
    mapper = "CAMERA";
    break;

  case 0xfd:
    mapper = "TAMA";
    battery = true;
    ram = true;
    rtc = true;
    break;

  case 0xfe:
    mapper = "HuC3";
    break;

  case 0xff:
    mapper = "HuC1";
    battery = true;
    ram = true;
    break;

  }

  //Game Boy: title = $0134-0143
  //Game Boy Color (early games): title = $0134-0142; model = $0143
  //Game Boy Color (later games): title = $0134-013e; serial = $013f-0142; model = $0143
  std::string title;
  for(unsigned n = 0; n < (black || clear ? 15 : 16); ++n) {
    char byte = read(0x0134 + n);
    if(byte < 0x20 || byte > 0x7e) byte = ' ';
    title += byte;
  }

  std::string serial = title.substr(0, title.size() - 4);

  if(!black && !clear) serial = "";
  for(auto& byte : serial) {
    if(byte >= 'A' && byte <= 'Z') continue;
    //invalid serial
    serial = "";
    break;
  }
  //remove the serial from the title, if it exists
  title = title.substr(0, title.find_last_of(serial) - 4);

  //remove any excess whitespace from the title
  title.erase(title.find_last_not_of(' ') + 1);

  /*switch(read(0x0148)) { default:
  case 0x00: romSize =   2 * 16 * 1024; break;
  case 0x01: romSize =   4 * 16 * 1024; break;
  case 0x02: romSize =   8 * 16 * 1024; break;
  case 0x03: romSize =  16 * 16 * 1024; break;
  case 0x04: romSize =  32 * 16 * 1024; break;
  case 0x05: romSize =  64 * 16 * 1024; break;
  case 0x06: romSize = 128 * 16 * 1024; break;
  case 0x07: romSize = 256 * 16 * 1024; break;
  case 0x52: romSize =  72 * 16 * 1024; break;
  case 0x53: romSize =  80 * 16 * 1024; break;
  case 0x54: romSize =  96 * 16 * 1024; break;
  }*/

  switch(read(0x0149)) { default:
  case 0x00: ramSize =  0 * 1024; break;
  case 0x01: ramSize =  2 * 1024; break;
  case 0x02: ramSize =  8 * 1024; break;
  case 0x03: ramSize = 32 * 1024; break;
  }

  if(mapper == "MBC2" && ram) ramSize = 256;
  if(mapper == "MBC6" && ram) ramSize =  32 * 1024;
  if(mapper == "TAMA" && ram) ramSize =  32;

  if(mapper == "MBC6" && flash) flashSize = 1024 * 1024;

  //Game Boy header does not specify EEPROM size: detect via game title instead
  //Command Master:        EEPROM = 512 bytes
  //Kirby Tilt 'n' Tumble: EEPROM = 256 bytes
  //Korokoro Kirby:        EEPROM = 256 bytes
  if(mapper == "MBC7" && eeprom) {
    eepromSize = 256;  //fallback guess; supported values are 128, 256, 512
    if(title == "CMASTER"     && serial == "KCEJ") eepromSize = 512;
    if(title == "KIRBY TNT"   && serial == "KTNE") eepromSize = 256;
    if(title == "KORO2 KIRBY" && serial == "KKKJ") eepromSize = 256;
  }

  if(mapper == "MBC3" && rtc) rtcSize = 13;
  if(mapper == "TAMA" && rtc) rtcSize = 21;

  std::string gamename(location);
  gamename = gamename.substr(0, gamename.find_last_of("."));
  gamename = gamename.substr(gamename.find_last_of("/\\") + 1);

  std::string output;
  output += "game\n";
  output += "  sha256: " + sha256_digest(data.data(), data.size()) + "\n";
  output += "  label:  " + gamename + "\n";
  output += "  name:   " + gamename + "\n";
  output += "  title:  " + title + "\n";
  if(!serial.empty())
    output += "  serial: " + serial + "\n";
  output += "  board:  " + mapper + "\n";
  output += Memory{}.type("ROM").size(data.size()).content("Program").text();
  if(ram && ramSize && battery)
    output += Memory{}.type("RAM").size(ramSize).content("Save").text();
  if(ram && ramSize && !battery)
    output += Memory{}.type("RAM").size(ramSize).content("Save").isVolatile().text();
  if(eeprom && eepromSize)
    output += Memory{}.type("EEPROM").size(eepromSize).content("Save").text();
  if(flash && flashSize)
    output += Memory{}.type("Flash").size(flashSize).content("Download").text();
  if(rtc && rtcSize)
    output += Memory{}.type("RTC").size(rtcSize).content("Time").text();
  if(accelerometer)
    output += "    accelerometer\n";
  if(rumble)
    output += "    rumble\n";
  return output;
}

SufamiTurbo::SufamiTurbo(std::vector<uint8_t>& data, std::string location) : data(data), location(location) {
}

SufamiTurbo::operator bool() const {
  return data.size() >= 0x20000;
}

std::string SufamiTurbo::manifest() const {
  if(!operator bool()) return {};

  unsigned ramSize = data[0x37] *   0x800;  //  2KB

  std::string gamename(location);
  gamename = gamename.substr(0, gamename.find_last_of("."));
  gamename = gamename.substr(gamename.find_last_of("/\\") + 1);

  std::string output;
  output += "game\n";
  output += "  sha256: " + sha256_digest(data.data(), data.size()) + "\n";
  output += "  label:  " + gamename + "\n";
  output += "  name:   " + gamename + "\n";
  output += "  board\n";
  output += Memory{}.type("ROM").size(data.size()).content("Program").text();

  if(ramSize)
    output += Memory{}.type("RAM").size(ramSize).content("Save").text();

  return output;
}

SuperFamicom::SuperFamicom(std::vector<uint8_t>& data, std::string location) : data(data), location(location) {
  if((size() & 0x7fff) == 512) {
    //remove header if present
    //nall::memory::move(&data[0], &data[512], size() - 512);
    //data.resize(size() - 512);
    data.erase(data.begin(), data.begin() + 512);
  }

  if(size() < 0x8000) return;  //ignore images too small to be valid

  unsigned LoROM   = scoreHeader(  0x7fb0);
  unsigned HiROM   = scoreHeader(  0xffb0);
  unsigned ExLoROM = scoreHeader(0x407fb0);
  unsigned ExHiROM = scoreHeader(0x40ffb0);
  if(ExLoROM) ExLoROM += 4;
  if(ExHiROM) ExHiROM += 4;

  if(LoROM >= HiROM && LoROM >= ExLoROM && LoROM >= ExHiROM) headerAddress = 0x7fb0;
  else if(HiROM >= ExLoROM && HiROM >= ExHiROM) headerAddress = 0xffb0;
  else if(ExLoROM >= ExHiROM) headerAddress = 0x407fb0;
  else headerAddress = 0x40ffb0;
}

SuperFamicom::operator bool() const {
  return headerAddress;
}

std::string SuperFamicom::manifest() const {
  if(!operator bool()) return {};

  std::string gamename(location);
  gamename = gamename.substr(0, gamename.find_last_of("."));
  gamename = gamename.substr(gamename.find_last_of("/\\") + 1);

  std::string output;
  output += "game\n";
  output += "  sha256:   " + sha256_digest(data.data(), data.size()) + "\n";
  output += "  label:    " + gamename + "\n";
  output += "  name:     " + gamename + "\n";
  output += "  title:    " + title() + "\n";
  output += "  region:   " + region() + "\n";
  output += "  revision: " + revision() + "\n";
  output += "  board:    " + board() + "\n";

  std::string board = this->board();
  board.erase(board.find_last_not_of("#A") + 1);

  std::istringstream boardstream(board);
  std::vector<std::string> btokens;
  std::string token;

  while (std::getline(boardstream, token, '-')) {
    btokens.push_back(token);
  }
  if (btokens.size() < 2) {
    btokens.push_back("");
  }

  if(auto size = romSize()) {
    if(btokens[0] == "SPC7110" && size > 0x100000) {
      output += Memory{}.type("ROM").size(0x100000).content("Program").text();
      output += Memory{}.type("ROM").size(size - 0x100000).content("Data").text();
    } else if(btokens[0] == "EXSPC7110" && size == 0x700000) {
      //Tengai Maykou Zero (fan translation)
      output += Memory{}.type("ROM").size(0x100000).content("Program").text();
      output += Memory{}.type("ROM").size(0x500000).content("Data").text();
      output += Memory{}.type("ROM").size(0x100000).content("Expansion").text();
    } else {
      output += Memory{}.type("ROM").size(size).content("Program").text();
    }
  }

  if(auto size = ramSize()) {
    output += Memory{}.type("RAM").size(size).content("Save").text();
  }

  if(auto size = expansionRamSize()) {
    output += Memory{}.type("RAM").size(size).content("Save").text();
  }

  if(0) {
  } else if(btokens[0] == "ARM") {
    output += Memory{}.type("ROM").size(0x20000).content("Program").manufacturer("SETA").architecture("ARM6").identifier(firmwareARM()).text();
    output += Memory{}.type("ROM").size( 0x8000).content("Data"   ).manufacturer("SETA").architecture("ARM6").identifier(firmwareARM()).text();
    output += Memory{}.type("RAM").size( 0x4000).content("Data"   ).manufacturer("SETA").architecture("ARM6").identifier(firmwareARM()).isVolatile().text();
    output += Oscillator{}.frequency(21'440'000).text();
  } else if(btokens[0] == "BS" && btokens[1] == "MCC") {
    output += Memory{}.type("RAM").size(0x80000).content("Download").text();
  } else if(btokens[0] == "EXNEC") {
    output += Memory{}.type("ROM").size(0xc000).content("Program").manufacturer("NEC").architecture("uPD96050").identifier(firmwareEXNEC()).text();
    output += Memory{}.type("ROM").size(0x1000).content("Data"   ).manufacturer("NEC").architecture("uPD96050").identifier(firmwareEXNEC()).text();
    output += Memory{}.type("RAM").size(0x1000).content("Data"   ).manufacturer("NEC").architecture("uPD96050").identifier(firmwareEXNEC()).text();
    output += Oscillator{}.frequency(firmwareEXNEC() == "ST010" ? 11'000'000 : 15'000'000).text();
  } else if(btokens[0] == "GB") {
    output += Memory{}.type("ROM").size(0x100).content("Boot").manufacturer("Nintendo").architecture("LR35902").identifier(firmwareGB()).text();
  if(firmwareGB() == "SGB2")
    output += Oscillator{}.frequency(20'971'520).text();
  } else if(btokens[0] == "GSU") {
  //todo: MARIO CHIP 1 uses CPU oscillator
    output += Oscillator{}.frequency(21'440'000).text();
  } else if(btokens[0] == "HITACHI") {
    output += Memory{}.type("ROM").size(0xc00).content("Data").manufacturer("Hitachi").architecture("HG51BS169").identifier(firmwareHITACHI()).text();
    output += Memory{}.type("RAM").size(0xc00).content("Data").manufacturer("Hitachi").architecture("HG51BS169").identifier(firmwareHITACHI()).isVolatile().text();
    output += Oscillator{}.frequency(20'000'000).text();
  } else if(btokens[0] == "NEC") {
    output += Memory{}.type("ROM").size(0x1800).content("Program").manufacturer("NEC").architecture("uPD7725").identifier(firmwareNEC()).text();
    output += Memory{}.type("ROM").size( 0x800).content("Data"   ).manufacturer("NEC").architecture("uPD7725").identifier(firmwareNEC()).text();
    output += Memory{}.type("RAM").size( 0x200).content("Data"   ).manufacturer("NEC").architecture("uPD7725").identifier(firmwareNEC()).isVolatile().text();
    output += Oscillator{}.frequency(7'600'000).text();
  } else if(btokens[0] == "SA1" || btokens[1] == "SA1") {  //SA1-* or BS-SA1-*
    output += Memory{}.type("RAM").size(0x800).content("Internal").isVolatile().text();
  }

  if(btokens.back() == "EPSONRTC") {
    output += Memory{}.type("RTC").size(0x10).content("Time").manufacturer("Epson").text();
  } else if(btokens.back() == "SHARPRTC") {
    output += Memory{}.type("RTC").size(0x10).content("Time").manufacturer("Sharp").text();
  }

  return output;
}

std::string SuperFamicom::region() const {
  //Unlicensed software (homebrew, ROM hacks, etc) often change the standard region code,
  //and then neglect to change the extended header region code. Thanks to that, we can't
  //decode and display the full game serial + region code.
  return videoRegion();

  // This is commented because it is never actually called
  /*string region;

  char A = data[headerAddress + 0x02];  //game type
  char B = data[headerAddress + 0x03];  //game code
  char C = data[headerAddress + 0x04];  //game code
  char D = data[headerAddress + 0x05];  //region code (new; sometimes ambiguous)
  auto E = data[headerAddress + 0x29];  //region code (old)

  auto valid = [](char n) { return (n >= '0' && n <= '9') || (n >= 'A' && n <= 'Z'); };
  if(data[headerAddress + 0x2a] == 0x33 && valid(A) && valid(B) & valid(C) & valid(D)) {
    nall::string code{A, B, C, D};
    if(D == 'B') region = {"SNS-",  code, "-BRA"};
    if(D == 'C') region = {"SNSN-", code, "-ROC"};
    if(D == 'D') region = {"SNSP-", code, "-NOE"};
    if(D == 'E') region = {"SNS-",  code, "-USA"};
    if(D == 'F') region = {"SNSP-", code, "-FRA"};
    if(D == 'H') region = {"SNSP-", code, "-HOL"};
    if(D == 'I') region = {"SNSP-", code, "-ITA"};
    if(D == 'J') region = {"SHVC-", code, "-JPN"};
    if(D == 'K') region = {"SNSN-", code, "-KOR"};
    if(D == 'N') region = {"SNS-",  code, "-CAN"};
    if(D == 'P') region = {"SNSP-", code, "-EUR"};
    if(D == 'S') region = {"SNSP-", code, "-ESP"};
    if(D == 'U') region = {"SNSP-", code, "-AUS"};
    if(D == 'X') region = {"SNSP-", code, "-SCN"};
  }

  if(!region) {
    if(E == 0x00) region = {"JPN"};
    if(E == 0x01) region = {"USA"};
    if(E == 0x02) region = {"EUR"};
    if(E == 0x03) region = {"SCN"};
    if(E == 0x06) region = {"FRA"};
    if(E == 0x07) region = {"HOL"};
    if(E == 0x08) region = {"ESP"};
    if(E == 0x09) region = {"NOE"};
    if(E == 0x0a) region = {"ITA"};
    if(E == 0x0b) region = {"ROC"};
    if(E == 0x0d) region = {"KOR"};
    if(E == 0x0f) region = {"CAN"};
    if(E == 0x10) region = {"BRA"};
    if(E == 0x11) region = {"AUS"};
    if(E == 0x12) region = {"SCN"};
  }

  return region ? region : "NTSC";*/
}

std::string SuperFamicom::videoRegion() const {
  auto region = data[headerAddress + 0x29];
  if(region == 0x00) return "NTSC";  //JPN
  if(region == 0x01) return "NTSC";  //USA
  if(region == 0x0b) return "NTSC";  //ROC
  if(region == 0x0d) return "NTSC";  //KOR
  if(region == 0x0f) return "NTSC";  //CAN
  if(region == 0x10) return "NTSC";  //BRA
  return "PAL";
}

std::string SuperFamicom::revision() const {
  std::string revision;

  char A = data[headerAddress + 0x02];  //game type
  char B = data[headerAddress + 0x03];  //game code
  char C = data[headerAddress + 0x04];  //game code
  char D = data[headerAddress + 0x05];  //region code (new; sometimes ambiguous)
  //auto E = data[headerAddress + 0x29];  //region code (old) // TODO - why unused?
  char F = data[headerAddress + 0x2b] + '0';  //revision code

  auto valid = [](char n) { return (n >= '0' && n <= '9') || (n >= 'A' && n <= 'Z'); };
  if(data[headerAddress + 0x2a] == 0x33 && valid(A) && valid(B) & valid(C) & valid(D)) {
    std::string code; code = A; code += B; code += C; code += D;
    if(D == 'B') revision = "SNS-"  + code + "-" + F;
    if(D == 'C') revision = "SNSN-" + code + "-" + F;
    if(D == 'D') revision = "SNSP-" + code + "-" + F;
    if(D == 'E') revision = "SNS-"  + code + "-" + F;
    if(D == 'F') revision = "SNSP-" + code + "-" + F;
    if(D == 'H') revision = "SNSP-" + code + "-" + F;
    if(D == 'I') revision = "SNSP-" + code + "-" + F;
    if(D == 'J') revision = "SHVC-" + code + "-" + F;
    if(D == 'K') revision = "SNSN-" + code + "-" + F;
    if(D == 'N') revision = "SNS-"  + code + "-" + F;
    if(D == 'P') revision = "SNSP-" + code + "-" + F;
    if(D == 'S') revision = "SNSP-" + code + "-" + F;
    if(D == 'U') revision = "SNSP-" + code + "-" + F;
    if(D == 'X') revision = "SNSP-" + code + "-" + F;
  }

  if (revision.empty()) {
    std::string code; code = F;
    revision = "1." + code;
  }
  return revision;
}

//format: [slot]-[coprocessor]-[mapper]-[ram]-[rtc]
std::string SuperFamicom::board() const {
  std::string board;

  auto mapMode          = data[headerAddress + 0x25];
  auto cartridgeTypeLo  = data[headerAddress + 0x26] & 15;
  auto cartridgeTypeHi  = data[headerAddress + 0x26] >> 4;
  auto cartridgeSubType = data[headerAddress + 0x0f];

  std::string mode;
  if(mapMode == 0x20 || mapMode == 0x30) mode = "LOROM-";
  if(mapMode == 0x21 || mapMode == 0x31) mode = "HIROM-";
  if(mapMode == 0x22 || mapMode == 0x32) mode = "SDD1-";
  if(mapMode == 0x23 || mapMode == 0x33) mode = "SA1-";
  if(mapMode == 0x25 || mapMode == 0x35) mode = "EXHIROM-";
  if(mapMode == 0x2a || mapMode == 0x3a) mode = "SPC7110-";

  //many games will store an extra title character, overwriting the map mode
  //further, ExLoROM mode is unofficial, and lacks a mapping mode value
  if(mode.empty()) {
    if(headerAddress ==   0x7fb0) mode = "LOROM-";
    if(headerAddress ==   0xffb0) mode = "HIROM-";
    if(headerAddress == 0x407fb0) mode = "EXLOROM-";
    if(headerAddress == 0x40ffb0) mode = "EXHIROM-";
  }

  //this game's title ovewrites the map mode with '!' (0x21), but is a LOROM game
  if(title() == "YUYU NO QUIZ DE GO!GO") mode = "LOROM-";

  if(mode == "LOROM-" && headerAddress == 0x407fb0) mode = "EXLOROM-";

  bool epsonRTC = false;
  bool sharpRTC = false;

  std::string cser = serial();
  const char *cserial = cser.c_str();

  if(serial() == "A9PJ") {
  //Sufami Turbo (JPN)
    board += "ST-" + mode;
  } else if(serial() == "ZBSJ") {
  //BS-X: Sore wa Namae o Nusumareta Machi no Monogatari (JPN)
    board += "BS-MCC-";
  } else if(serial() == "042J") {
  //Super Game Boy 2
    board += "GB-" + mode;
  } else if(cserial[0] == 'Z' && cserial[3] == 'J') {
    board += "BS-" + mode;
  } else if(cartridgeTypeLo >= 0x3) {
    if(cartridgeTypeHi == 0x0) board += "NEC-" + mode;
    if(cartridgeTypeHi == 0x1) board += "GSU-";
    if(cartridgeTypeHi == 0x2) board += "OBC1-" + mode;
    if(cartridgeTypeHi == 0x3) board += "SA1-";
    if(cartridgeTypeHi == 0x4) board += "SDD1-";
    if(cartridgeTypeHi == 0x5) board += mode, sharpRTC = true;
    if(cartridgeTypeHi == 0xe && cartridgeTypeLo == 0x3) board += "GB-" + mode;
    if(cartridgeTypeHi == 0xf && cartridgeTypeLo == 0x5 && cartridgeSubType == 0x00) board += "SPC7110-";
    if(cartridgeTypeHi == 0xf && cartridgeTypeLo == 0x9 && cartridgeSubType == 0x00) board += "SPC7110-", epsonRTC = true;
    if(cartridgeTypeHi == 0xf                           && cartridgeSubType == 0x01) board += "EXNEC-" + mode;
    if(cartridgeTypeHi == 0xf                           && cartridgeSubType == 0x02) board += "ARM-" + mode;
    if(cartridgeTypeHi == 0xf                           && cartridgeSubType == 0x10) board += "HITACHI-" + mode;
  }
  if(board.empty()) board += mode;

  if(ramSize() || expansionRamSize()) board += "RAM-";
  if(epsonRTC) board += "EPSONRTC-";
  if(sharpRTC) board += "SHARPRTC-";

  board.erase(board.find_last_not_of('-') + 1);

  if((board.rfind(    "LOROM-RAM") == 0) && romSize() <= 0x200000) board += "#A";
  if((board.rfind("NEC-LOROM-RAM") == 0) && romSize() <= 0x100000) board += "#A";

  //Tengai Makyou Zero (fan translation)
  if((board.rfind("SPC7110-") == 0) && data.size() == 0x700000) board = "EX" + board;

  return board;
}

std::string SuperFamicom::title() const {
  std::string label;

  for(unsigned n = 0; n < 0x15; n++) {
    auto x = data[headerAddress + 0x10 + n];
    auto y = n == 0x14 ? 0 : data[headerAddress + 0x11 + n];

    //null terminator (padding)
    if(x == 0x00 || x == 0xff);

    //ASCII
    else if(x >= 0x20 && x <= 0x7e) label += (char)x;

    //Shift-JIS (half-width katakana)
    else if(x == 0xa1) label += "。";
    else if(x == 0xa2) label += "「";
    else if(x == 0xa3) label += "」";
    else if(x == 0xa4) label += "、";
    else if(x == 0xa5) label += "・";
    else if(x == 0xa6) label += "ヲ";
    else if(x == 0xa7) label += "ァ";
    else if(x == 0xa8) label += "ィ";
    else if(x == 0xa9) label += "ゥ";
    else if(x == 0xaa) label += "ェ";
    else if(x == 0xab) label += "ォ";
    else if(x == 0xac) label += "ャ";
    else if(x == 0xad) label += "ュ";
    else if(x == 0xae) label += "ョ";
    else if(x == 0xaf) label += "ッ";
    else if(x == 0xb0) label += "ー";

    else if(x == 0xb1) label +=                  "ア";
    else if(x == 0xb2) label +=                  "イ";
    else if(x == 0xb3) label += y == 0xde ? "ヴ" : "ウ";
    else if(x == 0xb4) label +=                  "エ";
    else if(x == 0xb5) label +=                  "オ";

    else if(x == 0xb6) label += y == 0xde ? "ガ" : "カ";
    else if(x == 0xb7) label += y == 0xde ? "ギ" : "キ";
    else if(x == 0xb8) label += y == 0xde ? "グ" : "ク";
    else if(x == 0xb9) label += y == 0xde ? "ゲ" : "ケ";
    else if(x == 0xba) label += y == 0xde ? "ゴ" : "コ";

    else if(x == 0xbb) label += y == 0xde ? "ザ" : "サ";
    else if(x == 0xbc) label += y == 0xde ? "ジ" : "シ";
    else if(x == 0xbd) label += y == 0xde ? "ズ" : "ス";
    else if(x == 0xbe) label += y == 0xde ? "ゼ" : "セ";
    else if(x == 0xbf) label += y == 0xde ? "ゾ" : "ソ";

    else if(x == 0xc0) label += y == 0xde ? "ダ" : "タ";
    else if(x == 0xc1) label += y == 0xde ? "ヂ" : "チ";
    else if(x == 0xc2) label += y == 0xde ? "ヅ" : "ツ";
    else if(x == 0xc3) label += y == 0xde ? "デ" : "テ";
    else if(x == 0xc4) label += y == 0xde ? "ド" : "ト";

    else if(x == 0xc5) label += "ナ";
    else if(x == 0xc6) label += "ニ";
    else if(x == 0xc7) label += "ヌ";
    else if(x == 0xc8) label += "ネ";
    else if(x == 0xc9) label += "ノ";

    else if(x == 0xca) label += y == 0xdf ? "パ" : y == 0xde ? "バ" : "ハ";
    else if(x == 0xcb) label += y == 0xdf ? "ピ" : y == 0xde ? "ビ" : "ヒ";
    else if(x == 0xcc) label += y == 0xdf ? "プ" : y == 0xde ? "ブ" : "フ";
    else if(x == 0xcd) label += y == 0xdf ? "ペ" : y == 0xde ? "ベ" : "ヘ";
    else if(x == 0xce) label += y == 0xdf ? "ポ" : y == 0xde ? "ボ" : "ホ";

    else if(x == 0xcf) label += "マ";
    else if(x == 0xd0) label += "ミ";
    else if(x == 0xd1) label += "ム";
    else if(x == 0xd2) label += "メ";
    else if(x == 0xd3) label += "モ";

    else if(x == 0xd4) label += "ヤ";
    else if(x == 0xd5) label += "ユ";
    else if(x == 0xd6) label += "ヨ";

    else if(x == 0xd7) label += "ラ";
    else if(x == 0xd8) label += "リ";
    else if(x == 0xd9) label += "ル";
    else if(x == 0xda) label += "レ";
    else if(x == 0xdb) label += "ロ";

    else if(x == 0xdc) label += "ワ";
    else if(x == 0xdd) label += "ン";

    else if(x == 0xde) label += "\xef\xbe\x9e";  //dakuten
    else if(x == 0xdf) label += "\xef\xbe\x9f";  //handakuten

    //unknown
    else label += "?";

    //(han)dakuten skip
    if(y == 0xde && x == 0xb3) n++;
    if(y == 0xde && x >= 0xb6 && x <= 0xc4) n++;
    if(y == 0xde && x >= 0xca && x <= 0xce) n++;
    if(y == 0xdf && x >= 0xca && y <= 0xce) n++;
  }

  const char *badchars = " \t\n\r";
  label.erase(0, label.find_first_not_of(badchars));
  label.erase(label.find_last_not_of(badchars) + 1);
  return label;
}

std::string SuperFamicom::serial() const {
  char A = data[headerAddress + 0x02];  //game type
  char B = data[headerAddress + 0x03];  //game code
  char C = data[headerAddress + 0x04];  //game code
  char D = data[headerAddress + 0x05];  //region code (new; sometimes ambiguous)

  auto valid = [](char n) { return (n >= '0' && n <= '9') || (n >= 'A' && n <= 'Z'); };
  if(data[headerAddress + 0x2a] == 0x33 && valid(A) && valid(B) & valid(C) & valid(D)) {
    std::string ret; ret = A; ret += B; ret += C; ret += D;
    return ret;
  }

  return "";
}

unsigned SuperFamicom::romSize() const {
  return size() - firmwareRomSize();
}

unsigned SuperFamicom::programRomSize() const {
  if(board().rfind("SPC7110-") == 0) return 0x100000;
  if(board().rfind("EXSPC7110-") == 0) return 0x100000;
  return romSize();
}

unsigned SuperFamicom::dataRomSize() const {
  if(board().rfind("SPC7110-") == 0) return romSize() - 0x100000;
  if(board().rfind("EXSPC7110-") == 0) return 0x500000;
  return 0;
}

unsigned SuperFamicom::expansionRomSize() const {
  if(board().rfind("EXSPC7110-") == 0) return 0x100000;
  return 0;
}

//detect if any firmware is appended to the ROM image, and return its size if so
unsigned SuperFamicom::firmwareRomSize() const {
  auto cartridgeTypeLo  = data[headerAddress + 0x26] & 15;
  auto cartridgeTypeHi  = data[headerAddress + 0x26] >> 4;
  auto cartridgeSubType = data[headerAddress + 0x0f];

  if(serial() == "042J" || (cartridgeTypeLo == 0x3 && cartridgeTypeHi == 0xe)) {
    //Game Boy
    if((size() & 0x7fff) == 0x100) return 0x100;
  }

  if(cartridgeTypeLo >= 0x3 && cartridgeTypeHi == 0xf && cartridgeSubType == 0x10) {
    //Hitachi HG51BS169
    if((size() & 0x7fff) == 0xc00) return 0xc00;
  }

  if(cartridgeTypeLo >= 0x3 && cartridgeTypeHi == 0x0) {
    //NEC uPD7725
    if((size() & 0x7fff) == 0x2000) return 0x2000;
  }

  if(cartridgeTypeLo >= 0x3 && cartridgeTypeHi == 0xf && cartridgeSubType == 0x01) {
    //NEC uPD96050
    if((size() & 0xffff) == 0xd000) return 0xd000;
  }

  if(cartridgeTypeLo >= 0x3 && cartridgeTypeHi == 0xf && cartridgeSubType == 0x02) {
    //ARM6
    if((size() & 0x3ffff) == 0x28000) return 0x28000;
  }

  return 0;
}

unsigned SuperFamicom::ramSize() const {
  auto ramSize = data[headerAddress + 0x28] & 15;
  if(ramSize > 8) ramSize = 8;
  if(ramSize > 0) return 1024 << ramSize;
  return 0;
}

unsigned SuperFamicom::expansionRamSize() const {
  if(data[headerAddress + 0x2a] == 0x33) {
    auto ramSize = data[headerAddress + 0x0d] & 15;
    if(ramSize > 8) ramSize = 8;
    if(ramSize > 0) return 1024 << ramSize;
  }
  if((data[headerAddress + 0x26] >> 4) == 1) {
    //GSU: Starfox / Starwing lacks an extended header; but still has expansion RAM
    return 0x8000;
  }
  return 0;
}

bool SuperFamicom::nonVolatile() const {
  auto cartridgeTypeLo = data[headerAddress + 0x26] & 15;
  return cartridgeTypeLo == 0x2 || cartridgeTypeLo == 0x5 || cartridgeTypeLo == 0x6;
}

unsigned SuperFamicom::scoreHeader(unsigned address) {
  int score = 0;
  if(size() < address + 0x50) return score;

  uint8_t  mapMode     = data[address + 0x25] & ~0x10;  //ignore FastROM bit
  uint16_t complement  = data[address + 0x2c] << 0 | data[address + 0x2d] << 8;
  uint16_t checksum    = data[address + 0x2e] << 0 | data[address + 0x2f] << 8;
  uint16_t resetVector = data[address + 0x4c] << 0 | data[address + 0x4d] << 8;
  if(resetVector < 0x8000) return score;  //$00:0000-7fff is never ROM data

  uint8_t opcode = data[(address & ~0x7fff) | (resetVector & 0x7fff)];  //first instruction executed

  //most likely opcodes
  if(opcode == 0x78  //sei
  || opcode == 0x18  //clc (clc; xce)
  || opcode == 0x38  //sec (sec; xce)
  || opcode == 0x9c  //stz $nnnn (stz $4200)
  || opcode == 0x4c  //jmp $nnnn
  || opcode == 0x5c  //jml $nnnnnn
  ) score += 8;

  //plausible opcodes
  if(opcode == 0xc2  //rep #$nn
  || opcode == 0xe2  //sep #$nn
  || opcode == 0xad  //lda $nnnn
  || opcode == 0xae  //ldx $nnnn
  || opcode == 0xac  //ldy $nnnn
  || opcode == 0xaf  //lda $nnnnnn
  || opcode == 0xa9  //lda #$nn
  || opcode == 0xa2  //ldx #$nn
  || opcode == 0xa0  //ldy #$nn
  || opcode == 0x20  //jsr $nnnn
  || opcode == 0x22  //jsl $nnnnnn
  ) score += 4;

  //implausible opcodes
  if(opcode == 0x40  //rti
  || opcode == 0x60  //rts
  || opcode == 0x6b  //rtl
  || opcode == 0xcd  //cmp $nnnn
  || opcode == 0xec  //cpx $nnnn
  || opcode == 0xcc  //cpy $nnnn
  ) score -= 4;

  //least likely opcodes
  if(opcode == 0x00  //brk #$nn
  || opcode == 0x02  //cop #$nn
  || opcode == 0xdb  //stp
  || opcode == 0x42  //wdm
  || opcode == 0xff  //sbc $nnnnnn,x
  ) score -= 8;

  if(checksum + complement == 0xffff) score += 4;

  if(address == 0x7fb0 && mapMode == 0x20) score += 2;
  if(address == 0xffb0 && mapMode == 0x21) score += 2;

  return std::max(0, score);
}

std::string SuperFamicom::firmwareARM() const {
  return "ST018";
}

std::string SuperFamicom::firmwareEXNEC() const {
  if(title() == "EXHAUST HEAT2") return "ST010";
  if(title() == "F1 ROC II") return "ST010";
  if(title() == "2DAN MORITA SHOUGI") return "ST011";
  return "ST010";
}

std::string SuperFamicom::firmwareGB() const {
  if(title() == "Super GAMEBOY") return "SGB1";
  if(title() == "Super GAMEBOY2") return "SGB2";
  return "SGB1";
}

std::string SuperFamicom::firmwareHITACHI() const {
  return "Cx4";
}

std::string SuperFamicom::firmwareNEC() const {
  if(title() == "PILOTWINGS") return "DSP1";
  if(title() == "DUNGEON MASTER") return "DSP2";
  if(title() == "SDガンダムGX") return "DSP3";
  if(title() == "PLANETS CHAMP TG3000") return "DSP4";
  if(title() == "TOP GEAR 3000") return "DSP4";
  return "DSP1B";
}

}
