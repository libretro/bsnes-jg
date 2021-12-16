//Epson RTC-4513 Real-Time Clock

struct EpsonRTC : Thread {
  auto synchronizeCPU() -> void;
  static auto Enter() -> void;
  auto main() -> void;
  auto step(unsigned clocks) -> void;

  auto initialize() -> void;
  auto power() -> void;
  auto synchronize(uint64_t timestamp) -> void;

  auto read(unsigned addr, uint8_t data) -> uint8_t;
  auto write(unsigned addr, uint8_t data) -> void;

  auto serialize(serializer&) -> void;

  nall::Natural<21> clocks;
  unsigned seconds;

  uint2 chipselect;
  enum class State : unsigned { Mode, Seek, Read, Write } state;
  uint4 mdr;
  uint4 offset;
  unsigned wait;
  uint1 ready;
  uint1 holdtick;

  uint4 secondlo;
  uint3 secondhi;
  uint1 batteryfailure;

  uint4 minutelo;
  uint3 minutehi;
  uint1 resync;

  uint4 hourlo;
  uint2 hourhi;
  uint1 meridian;

  uint4 daylo;
  uint2 dayhi;
  uint1 dayram;

  uint4 monthlo;
  uint1 monthhi;
  uint2 monthram;

  uint4 yearlo;
  uint4 yearhi;

  uint3 weekday;

  uint1 hold;
  uint1 calendar;
  uint1 irqflag;
  uint1 roundseconds;

  uint1 irqmask;
  uint1 irqduty;
  uint2 irqperiod;

  uint1 pause;
  uint1 stop;
  uint1 atime;  //astronomical time (24-hour mode)
  uint1 test;

  //memory.cpp
  auto rtcReset() -> void;
  auto rtcRead(uint4 addr) -> uint4;
  auto rtcWrite(uint4 addr, uint4 data) -> void;

  auto load(const uint8_t* data) -> void;
  auto save(uint8_t* data) -> void;

  //time.cpp
  auto irq(uint2 period) -> void;
  auto duty() -> void;
  auto roundSeconds() -> void;
  auto tick() -> void;

  auto tickSecond() -> void;
  auto tickMinute() -> void;
  auto tickHour() -> void;
  auto tickDay() -> void;
  auto tickMonth() -> void;
  auto tickYear() -> void;
};

extern EpsonRTC epsonrtc;
