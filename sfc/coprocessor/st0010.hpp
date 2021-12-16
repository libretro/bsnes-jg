struct ST0010 {
  auto power() -> void;

  auto read(unsigned addr, uint8_t data) -> uint8_t;
  auto write(unsigned addr, uint8_t data) -> void;

  auto serialize(serializer&) -> void;

  uint8_t ram[0x1000];
  static const int16_t sin_table[256];
  static const int16_t mode7_scale[176];
  static const uint8_t arctan[32][32];

  //interfaces to sin table
  int16_t sin(int16_t theta);
  int16_t cos(int16_t theta);

  //interfaces to ram buffer
  uint8_t readb (uint16_t addr);
  uint16_t readw (uint16_t addr);
  uint32_t readd (uint16_t addr);
  void writeb(uint16_t addr, uint8_t  data);
  void writew(uint16_t addr, uint16_t data);
  void writed(uint16_t addr, uint32_t data);

  //opcodes
  void op_01();
  void op_02();
  void op_03();
  void op_04();
  void op_05();
  void op_06();
  void op_07();
  void op_08();

  void op_01(int16_t x0, int16_t y0, int16_t &x1, int16_t &y1, int16_t &quadrant, int16_t &theta);
};

extern ST0010 st0010;
