struct NECDSP : Processor::uPD96050, Thread {
  void synchronizeCPU();
  static void Enter();
  void main();
  void step(unsigned clocks);

  uint8_t read(unsigned addr, uint8_t data);
  void write(unsigned addr, uint8_t data);

  uint8_t readRAM(unsigned addr, uint8_t data);
  void writeRAM(unsigned addr, uint8_t data);

  void power();

  std::vector<uint8_t> firmware() const;
  void serialize(serializer&);

  unsigned Frequency = 0;
};

extern NECDSP necdsp;
