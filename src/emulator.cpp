#include "emulator.hpp"

namespace Emulator {

Platform* platform = nullptr;

void Game::load(std::string text) {
  document = BML::unserialize(text.c_str());

  sha256 = BML::search(text, {"game", "sha256"});
  label = BML::search(text, {"game", "label"});
  name = BML::search(text, {"game", "name"});
  title = BML::search(text, {"game", "title"});
  region = BML::search(text, {"game", "region"});
  revision = BML::search(text, {"game", "revision"});
  board = BML::search(text, {"game", "board"});

  for(auto node : document.find("game/board/memory")) {
    memoryList.push_back(Memory{node});
  }

  for(auto node : document.find("game/board/oscillator")) {
    oscillatorList.push_back(Oscillator{node});
  }
}

nall::maybe<Game::Memory> Game::memory(Markup::Node node) {
  if(!node) return nall::nothing;
  for(auto& memory : memoryList) {
    auto type = std::string(node["type"].text());
    auto size = node["size"].natural();
    auto content = std::string(node["content"].text());
    auto manufacturer = std::string(node["manufacturer"].text());
    auto architecture = std::string(node["architecture"].text());
    auto identifier = std::string(node["identifier"].text());
    if(!type.empty() && type != memory.type) continue;
    if(size && size != memory.size) continue;
    if(!content.empty() && content != memory.content) continue;
    if(!manufacturer.empty() && manufacturer != memory.manufacturer) continue;
    if(!architecture.empty() && architecture != memory.architecture) continue;
    if(!identifier.empty() && identifier != memory.identifier) continue;
    return memory;
  }
  return nall::nothing;
}

nall::maybe<Game::Oscillator> Game::oscillator(unsigned index) {
  if(index < oscillatorList.size()) return oscillatorList[index];
  return nall::nothing;
}

Game::Oscillator::Oscillator(Markup::Node node) {
  frequency = node["frequency"].natural();
}

Game::Memory::Memory(Markup::Node node) {
  type = node["type"].text();
  size = node["size"].natural();
  content = node["content"].text();
  manufacturer = node["manufacturer"].text();
  architecture = node["architecture"].text();
  identifier = node["identifier"].text();
  nonVolatile = !(bool)node["volatile"];
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
