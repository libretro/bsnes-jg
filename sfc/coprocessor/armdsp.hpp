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

    auto status() const -> uint8_t {
      return (
        armtocpu.ready << 0
      | signal         << 2
      | cputoarm.ready << 3
      | ready          << 7
      );
    }
  } bridge;

  auto synchronizeCPU() -> void;
  static auto Enter() -> void;
  auto boot() -> void;
  auto main() -> void;

  auto step(uint clocks) -> void override;
  auto sleep() -> void override;
  auto get(uint mode, uint32_t addr) -> uint32_t override;
  auto set(uint mode, uint32_t addr, uint32_t word) -> void override;

  auto read(uint addr, uint8_t data) -> uint8_t;
  auto write(uint addr, uint8_t data) -> void;

  auto power() -> void;
  auto reset() -> void;  //soft reset

  auto firmware() const -> nall::vector<uint8_t>;
  auto serialize(serializer&) -> void;

  uint Frequency;

  uint8_t programROM[128 * 1024];
  uint8_t dataROM[32 * 1024];
  uint8_t programRAM[16 * 1024];
};

extern ArmDSP armdsp;
