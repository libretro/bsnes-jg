#include "emulator.hpp"

namespace Emulator {

Platform* platform = nullptr;

void Game::load(std::string text) {
  stddocument = text;

  sha256 = BML::search(text, {"game", "sha256"});
  label = BML::search(text, {"game", "label"});
  name = BML::search(text, {"game", "name"});
  title = BML::search(text, {"game", "title"});
  region = BML::search(text, {"game", "region"});
  revision = BML::search(text, {"game", "revision"});
  board = BML::search(text, {"game", "board"});
  std::vector<std::string> memlist = BML::searchList(text, "memory");
  std::vector<std::string> osclist = BML::searchList(text, "oscillator");

  for (auto node : memlist) {
    memoryList.push_back(Memory{node});
  }

  for (auto node : osclist) {
    oscillatorList.push_back(Oscillator{node});
  }
}

std::optional<Game::Memory> Game::memory(std::string node) {
  if (node.empty()) return std::nullopt;
  for (auto& m : memoryList) {
    auto type = BML::search(node, {"memory", "type"});
    auto strsize = BML::search(node, {"memory", "size"});
    auto size = strsize.empty() ? 0 : std::stoi(strsize, nullptr, 16);
    auto content = BML::search(node, {"memory", "content"});
    auto manufacturer = BML::search(node, {"memory", "manufacturer"});
    auto architecture = BML::search(node, {"memory", "architecture"});
    auto identifier = BML::search(node, {"memory", "identifier"});
    auto nonVolatile = !BML::exists(node, {"memory", "volatile"});
    if(!type.empty() && type != m.type) continue;
    if(size && size != m.size) continue;
    if(!content.empty() && content != m.content) continue;
    if(!manufacturer.empty() && manufacturer != m.manufacturer) continue;
    if(!architecture.empty() && architecture != m.architecture) continue;
    if(!identifier.empty() && identifier != m.identifier) continue;
    return m;
  }
  return std::nullopt;
}

std::optional<Game::Oscillator> Game::oscillator(unsigned index) {
  if(index < oscillatorList.size()) return oscillatorList[index];
  return std::nullopt;
}

Game::Oscillator::Oscillator(std::string node) {
  frequency = std::stoi(BML::search(node, {"oscillator", "frequency"}));
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
