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
#include <string>
#include <vector>

#include "markup.hpp"

#include "emulator.hpp"

namespace Emulator {

void Game::load(std::string text) {
  sha256 = BML::search(text, {"game", "sha256"});
  label = BML::search(text, {"game", "label"});
  name = BML::search(text, {"game", "name"});
  title = BML::search(text, {"game", "title"});
  region = BML::search(text, {"game", "region"});
  revision = BML::search(text, {"game", "revision"});
  board = BML::search(text, {"game", "board"});
  std::vector<std::string> memlist = BML::searchList(text, "memory");
  std::vector<std::string> osclist = BML::searchList(text, "oscillator");

  for (std::string node : memlist) {
    memoryList.push_back(Memory{node});
  }

  for (std::string node : osclist) {
    oscillatorList.push_back(Oscillator{node});
  }
}

bool Game::memory(Game::Memory& mem, std::string node) {
  if (node.empty()) return false;
  for (Game::Memory& m : memoryList) {
    std::string type = BML::search(node, {"memory", "type"});
    std::string strsize = BML::search(node, {"memory", "size"});
    unsigned size = strsize.empty() ? 0 : std::stoi(strsize, nullptr, 16);
    std::string content = BML::search(node, {"memory", "content"});
    std::string manufacturer = BML::search(node, {"memory", "manufacturer"});
    std::string architecture = BML::search(node, {"memory", "architecture"});
    std::string identifier = BML::search(node, {"memory", "identifier"});
    if(!type.empty() && type != m.type) continue;
    if(size && size != m.size) continue;
    if(!content.empty() && content != m.content) continue;
    if(!manufacturer.empty() && manufacturer != m.manufacturer) continue;
    if(!architecture.empty() && architecture != m.architecture) continue;
    if(!identifier.empty() && identifier != m.identifier) continue;
    mem = m;
    return true;
  }
  return false;
}

Game::Oscillator::Oscillator(std::string node) {
  frequency = std::stoi(BML::search(node, {"oscillator", "frequency"}));
}

Game::Oscillator::operator bool() const {
  return frequency;
}

Game::Memory::Memory(std::string node) {
  type = BML::search(node, {"memory", "type"});
  size = std::stoi(BML::search(node, {"memory", "size"}), nullptr, 16);
  content = BML::search(node, {"memory", "content"});
  manufacturer = BML::search(node, {"memory", "manufacturer"});
  architecture = BML::search(node, {"memory", "architecture"});
  identifier = BML::search(node, {"memory", "identifier"});
  nonVolatile = !BML::exists(node, {"memory", "volatile"});
}

Game::Memory::operator bool() const {
  return (bool)!type.empty();
}

std::string Game::Memory::name() const {
  std::string ret;
  if(!architecture.empty()) {
    ret = architecture + "." + content + "." + type;
  }
  else {
    ret = content + "." + type;
  }

  std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
  return ret;
}

}
