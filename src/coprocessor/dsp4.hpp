struct DSP4 {
  void power();

  uint8_t read(unsigned addr, uint8_t data);
  void write(unsigned addr, uint8_t data);

  void serialize(serializer&);
};

extern DSP4 dsp4;
