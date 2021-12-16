struct OBC1 {
  auto unload() -> void;
  auto power() -> void;

  auto read(uint addr, uint8_t data) -> uint8_t;
  auto write(uint addr, uint8_t data) -> void;

  auto serialize(serializer&) -> void;

  WritableMemory ram;

private:
  auto ramRead(uint addr) -> uint8_t;
  auto ramWrite(uint addr, uint8_t data) -> void;

  struct {
    uint16_t address;
    uint16_t baseptr;
    uint16_t shift;
  } status;
};

extern OBC1 obc1;
