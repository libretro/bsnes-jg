struct DIP {
  //dip.cpp
  auto power() -> void;

  auto read(uint addr, uint8_t data) -> uint8_t;
  auto write(uint addr, uint8_t data) -> void;

  //serialization.cpp
  auto serialize(serializer&) -> void;

  uint8_t value = 0x00;
};

extern DIP dip;
