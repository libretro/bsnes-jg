//ARMv3 (ARM60)

//note: this coprocessor uses the ARMv4 (ARM7TDMI) core as its base
//instruction execution forces ARM mode to remove ARMv4 THUMB access
//there is a possibility the ARMv3 supports 26-bit mode; but cannot be verified

struct ArmDSP : Processor::ARM7TDMI, Thread {
  struct Bridge {
    struct Buffer {
      bool ready;
      uint8_t data;
    };
    Buffer cputoarm;
    Buffer armtocpu;
    uint32_t timer;
    uint32_t timerlatch;
    bool reset;
    bool ready;
    bool signal;

    uint8_t status() const {
      return (
        armtocpu.ready << 0
      | signal         << 2
      | cputoarm.ready << 3
      | ready          << 7
      );
    }
  } bridge;

  void synchronizeCPU();
  static void Enter();
  void boot();
  void main();

  void step(unsigned clocks) override;
  void sleep() override;
  uint32_t get(unsigned mode, uint32_t addr) override;
  void set(unsigned mode, uint32_t addr, uint32_t word) override;

  uint8_t read(unsigned addr, uint8_t data);
  void write(unsigned addr, uint8_t data);

  void power();
  void reset(); //soft reset

  std::vector<uint8_t> firmware() const;
  void serialize(serializer&);

  unsigned Frequency;

  uint8_t programROM[128 * 1024];
  uint8_t dataROM[32 * 1024];
  uint8_t programRAM[16 * 1024];
};

extern ArmDSP armdsp;
