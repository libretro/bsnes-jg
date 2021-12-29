struct SuperFX : Processor::GSU, Thread {
  ReadableMemory rom;
  WritableMemory ram;

  inline bool synchronizing() const { return scheduler.synchronizing(); }

  //superfx.cpp
  void synchronizeCPU();
  static void Enter();
  void main();
  void unload();
  void power();

  //bus.cpp
  struct CPUROM : Memory {
    uint8_t* data() override;
    unsigned size() const override;
    uint8_t read(unsigned, uint8_t) override;
    void write(unsigned, uint8_t) override;
  };

  struct CPURAM : Memory {
    uint8_t* data() override;
    unsigned size() const override;
    uint8_t read(unsigned, uint8_t) override;
    void write(unsigned, uint8_t) override;
  };

  //core.cpp
  void stop() override;
  uint8_t color(uint8_t source) override;
  void plot(uint8_t x, uint8_t y) override;
  uint8_t rpix(uint8_t x, uint8_t y) override;

  void flushPixelCache(PixelCache& cache);

  //memory.cpp
  uint8_t read(unsigned addr, uint8_t data = 0x00) override;
  void write(unsigned addr, uint8_t data) override;

  uint8_t readOpcode(uint16_t addr);
  inline uint8_t peekpipe();
  inline uint8_t pipe() override;

  void flushCache() override;
  uint8_t readCache(uint16_t addr);
  void writeCache(uint16_t addr, uint8_t data);

  //io.cpp
  uint8_t readIO(unsigned addr, uint8_t data);
  void writeIO(unsigned addr, uint8_t data);

  //timing.cpp
  void step(unsigned clocks) override;

  void syncROMBuffer() override;
  uint8_t readROMBuffer() override;
  void updateROMBuffer();

  void syncRAMBuffer() override;
  uint8_t readRAMBuffer(uint16_t addr) override;
  void writeRAMBuffer(uint16_t addr, uint8_t data) override;

  //serialization.cpp
  void serialize(serializer&);

  unsigned Frequency;

  CPUROM cpurom;
  CPURAM cpuram;

private:
  unsigned romMask;
  unsigned ramMask;
};

extern SuperFX superfx;
