struct SuperFX : Processor::GSU, Thread {
  ReadableMemory rom;
  WritableMemory ram;

  inline auto synchronizing() const -> bool { return scheduler.synchronizing(); }

  //superfx.cpp
  auto synchronizeCPU() -> void;
  static auto Enter() -> void;
  auto main() -> void;
  auto unload() -> void;
  auto power() -> void;

  //bus.cpp
  struct CPUROM : Memory {
    auto data() -> uint8_t *override;
    auto size() const -> unsigned override;
    auto read(unsigned, uint8_t) -> uint8_t override;
    auto write(unsigned, uint8_t) -> void override;
  };

  struct CPURAM : Memory {
    auto data() -> uint8_t *override;
    auto size() const -> unsigned override;
    auto read(unsigned, uint8_t) -> uint8_t override;
    auto write(unsigned, uint8_t) -> void override;
  };

  //core.cpp
  auto stop() -> void override;
  auto color(uint8_t source) -> uint8_t override;
  auto plot(uint8_t x, uint8_t y) -> void override;
  auto rpix(uint8_t x, uint8_t y) -> uint8_t override;

  auto flushPixelCache(PixelCache& cache) -> void;

  //memory.cpp
  auto read(unsigned addr, uint8_t data = 0x00) -> uint8_t override;
  auto write(unsigned addr, uint8_t data) -> void override;

  auto readOpcode(uint16_t addr) -> uint8_t;
  inline auto peekpipe() -> uint8_t;
  inline auto pipe() -> uint8_t override;

  auto flushCache() -> void override;
  auto readCache(uint16_t addr) -> uint8_t;
  auto writeCache(uint16_t addr, uint8_t data) -> void;

  //io.cpp
  auto readIO(unsigned addr, uint8_t data) -> uint8_t;
  auto writeIO(unsigned addr, uint8_t data) -> void;

  //timing.cpp
  auto step(unsigned clocks) -> void override;

  auto syncROMBuffer() -> void override;
  auto readROMBuffer() -> uint8_t override;
  auto updateROMBuffer() -> void;

  auto syncRAMBuffer() -> void override;
  auto readRAMBuffer(uint16_t addr) -> uint8_t override;
  auto writeRAMBuffer(uint16_t addr, uint8_t data) -> void override;

  //serialization.cpp
  auto serialize(serializer&) -> void;

  unsigned Frequency;

  CPUROM cpurom;
  CPURAM cpuram;

private:
  unsigned romMask;
  unsigned ramMask;
};

extern SuperFX superfx;
