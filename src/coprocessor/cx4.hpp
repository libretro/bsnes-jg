struct Cx4 {
  void power();

  uint8_t read(unsigned addr, uint8_t data = 0);
  void write(unsigned addr, uint8_t data);

  void serialize(serializer&);

  uint8_t  ram[0x0c00];
  uint8_t  reg[0x0100];
  uint32_t r0, r1, r2,  r3,  r4,  r5,  r6,  r7,
           r8, r9, r10, r11, r12, r13, r14, r15;

  static const uint8_t  immediate_data[48];
  static const uint16_t wave_data[40];
  static const uint32_t sin_table[256];

  static const int16_t SinTable[512];
  static const int16_t CosTable[512];

  int16_t C4WFXVal, C4WFYVal, C4WFZVal, C4WFX2Val, C4WFY2Val, C4WFDist, C4WFScale;
  int16_t C41FXVal, C41FYVal, C41FAngleRes, C41FDist, C41FDistVal;

  void C4TransfWireFrame();
  void C4TransfWireFrame2();
  void C4CalcWireFrame();
  void C4DrawLine(int32_t X1, int32_t Y1, int16_t Z1, int32_t X2, int32_t Y2, int16_t Z2, uint8_t Color);
  void C4DrawWireFrame();
  void C4DoScaleRotate(int row_padding);

public:
  uint32_t ldr(uint8_t r);
  void     str(uint8_t r, uint32_t data);
  void     mul(uint32_t x, uint32_t y, uint32_t &rl, uint32_t &rh);
  uint32_t sin(uint32_t rx);
  uint32_t cos(uint32_t rx);

  void     transfer_data();
  void     immediate_reg(uint32_t num);

  void     op00_00();
  void     op00_03();
  void     op00_05();
  void     op00_07();
  void     op00_08();
  void     op00_0b();
  void     op00_0c();

  void     op00();
  void     op01();
  void     op05();
  void     op0d();
  void     op10();
  void     op13();
  void     op15();
  void     op1f();
  void     op22();
  void     op25();
  void     op2d();
  void     op40();
  void     op54();
  void     op5c();
  void     op5e();
  void     op60();
  void     op62();
  void     op64();
  void     op66();
  void     op68();
  void     op6a();
  void     op6c();
  void     op6e();
  void     op70();
  void     op72();
  void     op74();
  void     op76();
  void     op78();
  void     op7a();
  void     op7c();
  void     op89();

  uint8_t  readb(uint16_t addr);
  uint16_t readw(uint16_t addr);
  uint32_t readl(uint16_t addr);

  void     writeb(uint16_t addr, uint8_t  data);
  void     writew(uint16_t addr, uint16_t data);
  void     writel(uint16_t addr, uint32_t data);
};

extern Cx4 cx4;
