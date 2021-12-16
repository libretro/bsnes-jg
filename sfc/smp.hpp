//Sony CXP1100Q-1

struct SMP : Processor::SPC700, Thread {
  inline auto synchronizing() const -> bool override { return scheduler.synchronizing(); }

  //io.cpp
  auto portRead(uint2 port) const -> uint8_t;
  auto portWrite(uint2 port, uint8_t data) -> void;

  //smp.cpp
  auto synchronizeCPU() -> void;
  auto synchronizeDSP() -> void;
  static auto Enter() -> void;
  auto main() -> void;
  auto load() -> bool;
  auto power(bool reset) -> void;

  //serialization.cpp
  auto serialize(serializer&) -> void;

  uint8_t iplrom[64];

private:
  struct IO {
    //timing
    uint clockCounter = 0;
    uint dspCounter = 0;

    //external
    uint8_t apu0 = 0;
    uint8_t apu1 = 0;
    uint8_t apu2 = 0;
    uint8_t apu3 = 0;

    //$00f0
    uint1 timersDisable = 0;
    uint1 ramWritable = 1;
    uint1 ramDisable = 0;
    uint1 timersEnable = 1;
    uint2 externalWaitStates = 0;
    uint2 internalWaitStates = 0;

    //$00f1
    uint1 iplromEnable = 1;

    //$00f2
    uint8_t dspAddr = 0;

    //$00f4-00f7
    uint8_t cpu0 = 0;
    uint8_t cpu1 = 0;
    uint8_t cpu2 = 0;
    uint8_t cpu3 = 0;

    //$00f8-00f9
    uint8_t aux4 = 0;
    uint8_t aux5 = 0;
  } io;

  //memory.cpp
  inline auto readRAM(uint16_t address) -> uint8_t;
  inline auto writeRAM(uint16_t address, uint8_t data) -> void;

  auto idle() -> void override;
  auto read(uint16_t address) -> uint8_t override;
  auto write(uint16_t address, uint8_t data) -> void override;

  auto readDisassembler(uint16_t address) -> uint8_t override;

  //io.cpp
  inline auto readIO(uint16_t address) -> uint8_t;
  inline auto writeIO(uint16_t address, uint8_t data) -> void;

  //timing.cpp
  template<uint Frequency>
  struct Timer {
    uint8_t stage0 = 0;
    uint8_t stage1 = 0;
    uint8_t stage2 = 0;
    uint4   stage3 = 0;
    Boolean line = 0;
    Boolean enable = 0;
    uint8_t target = 0;

    auto step(uint clocks) -> void;
    auto synchronizeStage1() -> void;
  };

  Timer<128> timer0;
  Timer<128> timer1;
  Timer< 16> timer2;

  inline auto wait(maybe<uint16_t> address = nothing, bool half = false) -> void;
  inline auto waitIdle(maybe<uint16_t> address = nothing, bool half = false) -> void;
  inline auto step(uint clocks) -> void;
  inline auto stepIdle(uint clocks) -> void;
  inline auto stepTimers(uint clocks) -> void;
};

extern SMP smp;
