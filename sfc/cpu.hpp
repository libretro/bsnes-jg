struct CPU : Processor::WDC65816, Thread, PPUcounter {
  inline auto interruptPending() const -> bool override { return status.interruptPending; }
  inline auto pio() const -> uint8_t { return io.pio; }
  inline auto refresh() const -> bool { return status.dramRefresh == 1; }
  inline auto synchronizing() const -> bool override { return scheduler.synchronizing(); }

  //cpu.cpp
  auto synchronizeSMP() -> void;
  auto synchronizePPU() -> void;
  auto synchronizeCoprocessors() -> void;
  static auto Enter() -> void;
  auto main() -> void;
  auto load() -> bool;
  auto power(bool reset) -> void;

  //dma.cpp
  inline auto dmaEnable() -> bool;
  inline auto hdmaEnable() -> bool;
  inline auto hdmaActive() -> bool;

  auto dmaRun() -> void;
  auto hdmaReset() -> void;
  auto hdmaSetup() -> void;
  auto hdmaRun() -> void;

  //memory.cpp
  auto idle() -> void override;
  auto read(unsigned addr) -> uint8_t override;
  auto write(unsigned addr, uint8_t data) -> void override;
  auto readDisassembler(unsigned addr) -> uint8_t override;

  //io.cpp
  auto readRAM(unsigned address, uint8_t data) -> uint8_t;
  auto readAPU(unsigned address, uint8_t data) -> uint8_t;
  auto readCPU(unsigned address, uint8_t data) -> uint8_t;
  auto readDMA(unsigned address, uint8_t data) -> uint8_t;
  auto writeRAM(unsigned address, uint8_t data) -> void;
  auto writeAPU(unsigned address, uint8_t data) -> void;
  auto writeCPU(unsigned address, uint8_t data) -> void;
  auto writeDMA(unsigned address, uint8_t data) -> void;

  //timing.cpp
  inline auto dmaCounter() const -> unsigned;
  inline auto joypadCounter() const -> unsigned;

  alwaysinline auto stepOnce() -> void;
  alwaysinline auto step(unsigned clocks) -> void;
  template<unsigned Clocks, bool Synchronize> auto step() -> void;
  auto scanline() -> void;

  alwaysinline auto aluEdge() -> void;
  alwaysinline auto dmaEdge() -> void;

  //irq.cpp
  alwaysinline auto nmiPoll() -> void;
  alwaysinline auto irqPoll() -> void;
  auto nmitimenUpdate(uint8_t data) -> void;
  auto rdnmi() -> bool;
  auto timeup() -> bool;

  alwaysinline auto nmiTest() -> bool;
  alwaysinline auto irqTest() -> bool;
  alwaysinline auto lastCycle() -> void;

  //joypad.cpp
  auto joypadEdge() -> void;

  //serialization.cpp
  auto serialize(serializer&) -> void;

  uint8_t wram[128 * 1024];
  vector<Thread*> coprocessors;

  struct Overclocking {
    unsigned counter = 0;
    unsigned target = 0;
  } overclocking;

private:
  unsigned version = 2;  //allowed: 1, 2

  struct Counter {
    unsigned cpu = 0;
    unsigned dma = 0;
  } counter;

  struct Status {
    unsigned clockCount = 0;

    bool irqLock = 0;

    unsigned dramRefreshPosition = 0;
    unsigned dramRefresh = 0;  //0 = not refreshed; 1 = refresh active; 2 = refresh inactive

    unsigned hdmaSetupPosition = 0;
    bool hdmaSetupTriggered = 0;

    unsigned hdmaPosition = 0;
    bool hdmaTriggered = 0;

    Boolean nmiValid = 0;
    Boolean nmiLine = 0;
    Boolean nmiTransition = 0;
    Boolean nmiPending = 0;
    Boolean nmiHold = 0;

    Boolean irqValid = 0;
    Boolean irqLine = 0;
    Boolean irqTransition = 0;
    Boolean irqPending = 0;
    Boolean irqHold = 0;

    bool resetPending = 0;
    bool interruptPending = 0;

    bool dmaActive = 0;
    bool dmaPending = 0;
    bool hdmaPending = 0;
    bool hdmaMode = 0;  //0 = init, 1 = run

    unsigned autoJoypadCounter = 33;  //state machine; 4224 / 128 = 33 (inactive)
  } status;

  struct IO {
    //$2181-$2183
    nall::Natural<17> wramAddress = 0;

    //$4200
    Boolean hirqEnable = 0;
    Boolean virqEnable = 0;
    Boolean irqEnable = 0;
    Boolean nmiEnable = 0;
    Boolean autoJoypadPoll = 0;

    //$4201
    uint8_t pio = 0xff;

    //$4202-$4203
    uint8_t wrmpya = 0xff;
    uint8_t wrmpyb = 0xff;

    //$4204-$4206
    uint16_t wrdiva = 0xffff;
    uint8_t wrdivb = 0xff;

    //$4207-$420a
    nall::Natural<12> htime = 0x1ff + 1 << 2;
    nall::Natural< 9>  vtime = 0x1ff;

    //$420d
    nall::Natural< 1> fastROM = 0;

    //$4214-$4217
    uint16_t rddiv = 0;
    uint16_t rdmpy = 0;

    //$4218-$421f
    uint16_t joy1 = 0;
    uint16_t joy2 = 0;
    uint16_t joy3 = 0;
    uint16_t joy4 = 0;
  } io;

  struct ALU {
    unsigned mpyctr = 0;
    unsigned divctr = 0;
    unsigned shift = 0;
  } alu;

  struct Channel {
    //dma.cpp
    template<unsigned Clocks, bool Synchronize> inline auto step() -> void;
    inline auto edge() -> void;

    inline auto validA(nall::Natural<24> address) -> bool;
    inline auto readA(nall::Natural<24> address) -> uint8_t;
    inline auto readB(uint8_t address, bool valid) -> uint8_t;
    inline auto writeA(nall::Natural<24> address, uint8_t data) -> void;
    inline auto writeB(uint8_t address, uint8_t data, bool valid) -> void;
    inline auto transfer(nall::Natural<24> address, nall::Natural< 2> index) -> void;

    inline auto dmaRun() -> void;
    inline auto hdmaActive() -> bool;
    inline auto hdmaFinished() -> bool;
    inline auto hdmaReset() -> void;
    inline auto hdmaSetup() -> void;
    inline auto hdmaReload() -> void;
    inline auto hdmaTransfer() -> void;
    inline auto hdmaAdvance() -> void;

    //$420b
    nall::Natural< 1> dmaEnable = 0;

    //$420c
    nall::Natural< 1> hdmaEnable = 0;

    //$43x0
    nall::Natural< 3> transferMode = 7;
    nall::Natural< 1> fixedTransfer = 1;
    nall::Natural< 1> reverseTransfer = 1;
    nall::Natural< 1> unused = 1;
    nall::Natural< 1> indirect = 1;
    nall::Natural< 1> direction = 1;

    //$43x1
    uint8_t targetAddress = 0xff;

    //$43x2-$43x3
    uint16_t sourceAddress = 0xffff;

    //$43x4
    uint8_t sourceBank = 0xff;

    //$43x5-$43x6
    union {
      uint16_t transferSize;
      uint16_t indirectAddress;
    };

    //$43x7
    uint8_t indirectBank = 0xff;

    //$43x8-$43x9
    uint16_t hdmaAddress = 0xffff;

    //$43xa
    uint8_t lineCounter = 0xff;

    //$43xb/$43xf
    uint8_t unknown = 0xff;

    //internal state
    nall::Natural< 1> hdmaCompleted = 0;
    nall::Natural< 1> hdmaDoTransfer = 0;

    maybe<Channel&> next;

    Channel() : transferSize(0xffff) {}
  } channels[8];
};

extern CPU cpu;
