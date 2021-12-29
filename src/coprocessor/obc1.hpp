struct OBC1 {
  void unload();
  void power();

  uint8_t read(unsigned addr, uint8_t data);
  void write(unsigned addr, uint8_t data);

  void serialize(serializer&);

  WritableMemory ram;

private:
  uint8_t ramRead(unsigned addr);
  void ramWrite(unsigned addr, uint8_t data);

  struct {
    uint16_t address;
    uint16_t baseptr;
    uint16_t shift;
  } status;
};

extern OBC1 obc1;
