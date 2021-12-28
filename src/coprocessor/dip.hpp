struct DIP {
  //dip.cpp
  void power();

  uint8_t read(unsigned addr, uint8_t data);
  void write(unsigned addr, uint8_t data);

  //serialization.cpp
  void serialize(serializer&);

  uint8_t value = 0x00;
};

extern DIP dip;
