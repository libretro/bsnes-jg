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

  nall::Natural< 2> chipselect;
  enum class State : unsigned { Mode, Seek, Read, Write } state;
  nall::Natural< 4> mdr;
  nall::Natural< 4> offset;
  unsigned wait;
  uint1 ready;
  uint1 holdtick;

  nall::Natural< 4> secondlo;
  nall::Natural< 3> secondhi;
  uint1 batteryfailure;

  nall::Natural< 4> minutelo;
  nall::Natural< 3> minutehi;
  uint1 resync;

  nall::Natural< 4> hourlo;
  nall::Natural< 2> hourhi;
  uint1 meridian;

  nall::Natural< 4> daylo;
  nall::Natural< 2> dayhi;
  uint1 dayram;

  nall::Natural< 4> monthlo;
  uint1 monthhi;
  nall::Natural< 2> monthram;

  nall::Natural< 4> yearlo;
  nall::Natural< 4> yearhi;

  nall::Natural< 3> weekday;

  uint1 hold;
  uint1 calendar;
  uint1 irqflag;
  uint1 roundseconds;

  uint1 irqmask;
  uint1 irqduty;
  nall::Natural< 2> irqperiod;

  uint1 pause;
  uint1 stop;
  uint1 atime;  //astronomical time (24-hour mode)
  uint1 test;

  //memory.cpp
  auto rtcReset() -> void;
  auto rtcRead(nall::Natural< 4> addr) -> nall::Natural< 4>;
  auto rtcWrite(nall::Natural< 4> addr, nall::Natural< 4> data) -> void;

  auto load(const uint8_t* data) -> void;
  auto save(uint8_t* data) -> void;

  //time.cpp
  auto irq(nall::Natural< 2> period) -> void;
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
