struct Expansion : Thread {
  Expansion();
  virtual ~Expansion();
};

struct ExpansionPort {
  void connect(unsigned deviceID);

  void power();
  void unload();
  void serialize(serializer&);

  Expansion* device = nullptr;
};

extern ExpansionPort expansionPort;

#include "satellaview.hpp"
//#include "21fx.hpp"
