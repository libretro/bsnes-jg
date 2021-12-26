#include <algorithm>
#include <iomanip>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "emulator.hpp"
#include "cheat.hpp"

namespace Emulator {

void Cheat::reset() {
  codes.clear();
}

void Cheat::append(unsigned address, unsigned data, std::optional<unsigned> compare) {
  codes.push_back({address, data, compare});
}

void Cheat::assign(const std::vector<std::string>& list) {
  reset();

  for (std::string entry : list) {
      std::replace(entry.begin(), entry.end(), '?', '=');
      std::stringstream ss(entry);
      std::vector<unsigned> part;
      while(std::getline(ss, entry, '=')) {
          part.push_back(std::stoul(entry, nullptr, 16));
      }
      if (part.size() == 2) append(part[0], part[1]);
      if (part.size() == 3) append(part[0], part[2], part[1]);
  }
}

bool Cheat::find(uint8_t *val, unsigned address, unsigned compare) {
  for(auto& code : codes) {
    if(code.address == address && (!code.compare || code.compare == compare)) {
      *val = code.data;
      return true;
    }
  }
  return false;
}

}

namespace CheatDecoder {

bool gb(std::string& code) {
    std::transform(code.begin(), code.end(), code.begin(), ::tolower);

    auto nib = [&](const std::string& s, unsigned index) -> unsigned {
        if (index >= s.size()) return 0;
        if (s[index] >= '0' && s[index] <= '9') return s[index] - '0';
        return s[index] - 'a' + 10;
    };

    //Game Genie 6
    std::regex rgx_gg6("[a-f0-9]{3}[-][a-f0-9]{3}");
    if (std::regex_match(code, rgx_gg6)) {
        code.erase(std::remove(code.begin(), code.end(), '-'), code.end());
        unsigned data = nib(code, 0) << 4 | nib(code, 1) << 0;
        unsigned address = (nib(code, 5) ^ 15) << 12 | nib(code, 2) << 8 |
            nib(code, 3) << 4 | nib(code, 4) << 0;
        std::stringstream ss;
        ss << std::hex << std::setfill('0') << std::setw(4) << address << "="
            << std::setw(2) << data;
        code = ss.str();
        return true;
    }

    // Game Genie 8
    std::regex rgx_gg8("[a-f0-9]{3}[-][a-f0-9]{3}[-][a-f0-9]{3}");
    if (std::regex_match(code, rgx_gg8)) {
        code.erase(std::remove(code.begin(), code.end(), '-'), code.end());
        unsigned data = nib(code, 0) << 4 | nib(code, 1) << 0;
        unsigned address = (nib(code, 5) ^ 15) << 12 | nib(code, 2) << 8 |
            nib(code, 3) << 4 | nib(code, 4) << 0;
        uint8_t t = nib(code, 6) << 4 | nib(code, 8) << 0;
        t = t >> 2 | t << 6;
        unsigned compare = t ^ 0xba;
        std::stringstream ss;
        ss << std::hex << std::setfill('0') << std::setw(4) << address << "="
            << std::setw(2) << compare << "?" << std::setw(2) << data;
        code = ss.str();
        return true;
    }

    // GameShark
    std::regex rgx_gs("[0]{1}[1]{1}[a-f0-9]{6}");
    if (std::regex_match(code, rgx_gs)) {
        uint32_t r;
        std::stringstream ss; ss << std::hex << code; ss >> r;
        unsigned data = (r >> 16) & 0xff;
        uint16_t address = r & 0xffff;
        address = address >> 8 | address << 8;
        ss.clear(); ss.str(std::string());
        ss << std::hex << std::setfill('0') << std::setw(4) << address << "="
            << std::setw(2) << data;
        code = ss.str();
        return true;
    }

    std::regex rgx_raw6("[a-f0-9]{4}[=][a-f0-9]{2}");
    if (std::regex_match(code, rgx_raw6)) {
        return true;
    }

    std::regex rgx_raw8("[a-f0-9]{4}[=][a-f0-9]{2}[?][a-f0-9]{2}");
    if (std::regex_match(code, rgx_raw8)) {
        return true;
    }

    return false;
}

bool snes(std::string& code) {
    std::transform(code.begin(), code.end(), code.begin(), ::tolower);

    // Game Genie
    std::regex rgx_gg("[a-f0-9]{4}[-][a-f0-9]{4}");
    if (std::regex_match(code, rgx_gg)) {
        code.erase(std::remove(code.begin(), code.end(), '-'), code.end());

        std::string ggcipher[2] = { "df4709156bc8a23e", "0123456789abcdef" };
        for (int c = 0; c < code.size(); ++c) {
            for (int i = 0; i < 16; ++i) {
                if (code[c] == ggcipher[0][i]) {
                    code[c] = ggcipher[1][i];
                    break;
                }
            }
        }

        uint32_t r;
        std::stringstream ss; ss << std::hex << code; ss >> r;

        unsigned address =
          (!!(r & 0x002000) << 23) | (!!(r & 0x001000) << 22)
        | (!!(r & 0x000800) << 21) | (!!(r & 0x000400) << 20)
        | (!!(r & 0x000020) << 19) | (!!(r & 0x000010) << 18)
        | (!!(r & 0x000008) << 17) | (!!(r & 0x000004) << 16)
        | (!!(r & 0x800000) << 15) | (!!(r & 0x400000) << 14)
        | (!!(r & 0x200000) << 13) | (!!(r & 0x100000) << 12)
        | (!!(r & 0x000002) << 11) | (!!(r & 0x000001) << 10)
        | (!!(r & 0x008000) <<  9) | (!!(r & 0x004000) <<  8)
        | (!!(r & 0x080000) <<  7) | (!!(r & 0x040000) <<  6)
        | (!!(r & 0x020000) <<  5) | (!!(r & 0x010000) <<  4)
        | (!!(r & 0x000200) <<  3) | (!!(r & 0x000100) <<  2)
        | (!!(r & 0x000080) <<  1) | (!!(r & 0x000040) <<  0);
        unsigned data = r >> 24;

        ss.clear(); ss.str(std::string());
        ss << std::hex << std::setfill('0') << std::setw(6) << address << "="
            << std::setw(2) << data;
        code = ss.str();
        return true;
    }

    // Pro Action Replay
    std::regex rgx_par("[a-f0-9]{8}");
    if (std::regex_match(code, rgx_par)) {
        uint32_t r;
        std::stringstream ss; ss << std::hex << code; ss >> r;
        unsigned address = r >> 8;
        unsigned data = r & 0xff;
        ss.clear(); ss.str(std::string());
        ss << std::hex << std::setfill('0') << std::setw(6) << address << "="
            << std::setw(2) << data;
        code = ss.str();
        return true;
    }

    std::regex rgx_raw8("[a-f0-9]{6}[=][a-f0-9]{2}");
    if (std::regex_match(code, rgx_raw8)) {
        return true;
    }

    std::regex rgx_raw10("[a-f0-9]{6}[=][a-f0-9]{2}[?][a-f0-9]{2}");
    if (std::regex_match(code, rgx_raw10)) {
        return true;
    }

    return false;
}

}
