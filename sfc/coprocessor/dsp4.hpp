struct DSP4 {
  auto power() -> void;

  auto read(uint addr, uint8_t data) -> uint8_t;
  auto write(uint addr, uint8_t data) -> void;

  auto serialize(serializer&) -> void;
};

extern DSP4 dsp4;
