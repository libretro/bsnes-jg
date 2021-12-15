struct Expansion : Thread {
  Expansion();
  virtual ~Expansion();
};

struct ExpansionPort {
  auto connect(uint deviceID) -> void;

  auto power() -> void;
  auto unload() -> void;
  auto serialize(serializer&) -> void;

  Expansion* device = nullptr;
};

extern ExpansionPort expansionPort;

#include "satellaview.hpp"
//#include "21fx.hpp"
