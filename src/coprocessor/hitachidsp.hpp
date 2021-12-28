struct HitachiDSP : Processor::HG51B, Thread {
  ReadableMemory rom;
  WritableMemory ram;

  //hitachidsp.cpp
  void synchronizeCPU();
  static void Enter();
  void step(unsigned clocks) override;
  void halt() override;

  void unload();
  void power();

  bool isROM(unsigned address) override;
  bool isRAM(unsigned address) override;

  //HG51B read/write
  uint8_t read(unsigned address) override;
  void write(unsigned address, uint8_t data) override;

  //CPU ROM read/write
  nall::maybe<unsigned> addressROM(unsigned address) const;
  uint8_t readROM(unsigned address, uint8_t data = 0);
  void writeROM(unsigned address, uint8_t data);

  //CPU RAM read/write
  nall::maybe<unsigned> addressRAM(unsigned address) const;
  uint8_t readRAM(unsigned address, uint8_t data = 0);
  void writeRAM(unsigned address, uint8_t data);

  //HG51B data RAM read/write
  nall::maybe<unsigned> addressDRAM(unsigned address) const;
  uint8_t readDRAM(unsigned address, uint8_t data = 0);
  void writeDRAM(unsigned address, uint8_t data);

  //CPU IO read/write
  nall::maybe<unsigned> addressIO(unsigned address) const;
  uint8_t readIO(unsigned address, uint8_t data = 0);
  void writeIO(unsigned address, uint8_t data);

  std::vector<uint8_t> firmware() const;
  void serialize(serializer&);

  unsigned Frequency;
  unsigned Roms;
  bool Mapping;

  //data-rom.cpp
  static const uint8_t staticDataROM[3072];
};

extern HitachiDSP hitachidsp;
