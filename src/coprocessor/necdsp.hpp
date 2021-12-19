struct NECDSP : Processor::uPD96050, Thread {
  auto synchronizeCPU() -> void;
  static auto Enter() -> void;
  auto main() -> void;
  auto step(unsigned clocks) -> void;

  auto read(unsigned addr, uint8_t data) -> uint8_t;
  auto write(unsigned addr, uint8_t data) -> void;

  auto readRAM(unsigned addr, uint8_t data) -> uint8_t;
  auto writeRAM(unsigned addr, uint8_t data) -> void;

  auto power() -> void;

  auto firmware() const -> std::vector<uint8_t>;
  auto serialize(serializer&) -> void;

  unsigned Frequency = 0;
};

extern NECDSP necdsp;
