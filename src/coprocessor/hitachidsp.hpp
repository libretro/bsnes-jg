struct HitachiDSP : Processor::HG51B, Thread {
  ReadableMemory rom;
  WritableMemory ram;

  //hitachidsp.cpp
  auto synchronizeCPU() -> void;
  static auto Enter() -> void;
  auto step(unsigned clocks) -> void override;
  auto halt() -> void override;

  auto unload() -> void;
  auto power() -> void;

  auto isROM(unsigned address) -> bool override;
  auto isRAM(unsigned address) -> bool override;

  //HG51B read/write
  auto read(unsigned address) -> uint8_t override;
  auto write(unsigned address, uint8_t data) -> void override;

  //CPU ROM read/write
  auto addressROM(unsigned address) const -> nall::maybe<unsigned>;
  auto readROM(unsigned address, uint8_t data = 0) -> uint8_t;
  auto writeROM(unsigned address, uint8_t data) -> void;

  //CPU RAM read/write
  auto addressRAM(unsigned address) const -> nall::maybe<unsigned>;
  auto readRAM(unsigned address, uint8_t data = 0) -> uint8_t;
  auto writeRAM(unsigned address, uint8_t data) -> void;

  //HG51B data RAM read/write
  auto addressDRAM(unsigned address) const -> nall::maybe<unsigned>;
  auto readDRAM(unsigned address, uint8_t data = 0) -> uint8_t;
  auto writeDRAM(unsigned address, uint8_t data) -> void;

  //CPU IO read/write
  auto addressIO(unsigned address) const -> nall::maybe<unsigned>;
  auto readIO(unsigned address, uint8_t data = 0) -> uint8_t;
  auto writeIO(unsigned address, uint8_t data) -> void;

  auto firmware() const -> std::vector<uint8_t>;
  auto serialize(serializer&) -> void;

  unsigned Frequency;
  unsigned Roms;
  bool Mapping;

  //data-rom.cpp
  static const uint8_t staticDataROM[3072];
};

extern HitachiDSP hitachidsp;
