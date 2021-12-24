//Sony CXP1100Q-1

struct SMP : Processor::SPC700, Thread {
  inline bool synchronizing() const override { return scheduler.synchronizing(); }

  //io.cpp
  uint8_t portRead(nall::Natural< 2> port) const;
  void portWrite(nall::Natural< 2> port, uint8_t data);

  //smp.cpp
  void synchronizeCPU();
  void synchronizeDSP();
  static void Enter();
  void main();
  bool load();
  void power(bool reset);

  //serialization.cpp
  void serialize(serializer&);

  static const uint8_t iplrom[64];

private:
  struct IO {
    //timing
    unsigned clockCounter = 0;
    unsigned dspCounter = 0;

    //external
    uint8_t apu0 = 0;
    uint8_t apu1 = 0;
    uint8_t apu2 = 0;
    uint8_t apu3 = 0;

    //$00f0
    nall::Natural< 1> timersDisable = 0;
    nall::Natural< 1> ramWritable = 1;
    nall::Natural< 1> ramDisable = 0;
    nall::Natural< 1> timersEnable = 1;
    nall::Natural< 2> externalWaitStates = 0;
    nall::Natural< 2> internalWaitStates = 0;

    //$00f1
    nall::Natural< 1> iplromEnable = 1;

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
  inline uint8_t readRAM(uint16_t address);
  inline void writeRAM(uint16_t address, uint8_t data);

  void idle() override;
  uint8_t read(uint16_t address) override;
  void write(uint16_t address, uint8_t data) override;

  uint8_t readDisassembler(uint16_t address) override;

  //io.cpp
  inline uint8_t readIO(uint16_t address);
  inline void writeIO(uint16_t address, uint8_t data);

  //timing.cpp
  template<unsigned Frequency>
  struct Timer {
    uint8_t stage0 = 0;
    uint8_t stage1 = 0;
    uint8_t stage2 = 0;
    nall::Natural< 4>   stage3 = 0;
    nall::Boolean line = 0;
    nall::Boolean enable = 0;
    uint8_t target = 0;

    void step(unsigned clocks);
    void synchronizeStage1();
  };

  Timer<128> timer0;
  Timer<128> timer1;
  Timer< 16> timer2;

  inline void wait(nall::maybe<uint16_t> address = nall::nothing, bool half = false);
  inline void waitIdle(nall::maybe<uint16_t> address = nall::nothing, bool half = false);
  inline void step(unsigned clocks);
  inline void stepIdle(unsigned clocks);
  inline void stepTimers(unsigned clocks);
};

extern SMP smp;
