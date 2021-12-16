struct Satellaview : Expansion {
  Satellaview();
  ~Satellaview();

  auto read(unsigned addr, uint8_t data) -> uint8_t;
  auto write(unsigned addr, uint8_t data) -> void;

private:
  struct {
    uint8_t r2188, r2189, r218a, r218b;
    uint8_t r218c, r218d, r218e, r218f;
    uint8_t r2190, r2191, r2192, r2193;
    uint8_t r2194, r2195, r2196, r2197;
    uint8_t r2198, r2199, r219a, r219b;
    uint8_t r219c, r219d, r219e, r219f;

    uint8_t rtcCounter;
    uint8_t rtcHour;
    uint8_t rtcMinute;
    uint8_t rtcSecond;
  } regs;
};
