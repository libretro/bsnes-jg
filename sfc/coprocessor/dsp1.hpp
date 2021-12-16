struct DSP1 {
  auto power() -> void;

  auto read(unsigned addr, uint8_t data) -> uint8_t;
  auto write(unsigned addr, uint8_t data) -> void;

  auto serialize(serializer&) -> void;
};

extern DSP1 dsp1;
