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
  nall::Natural< 1> ready;
  nall::Natural< 1> holdtick;

  nall::Natural< 4> secondlo;
  nall::Natural< 3> secondhi;
  nall::Natural< 1> batteryfailure;

  nall::Natural< 4> minutelo;
  nall::Natural< 3> minutehi;
  nall::Natural< 1> resync;

  nall::Natural< 4> hourlo;
  nall::Natural< 2> hourhi;
  nall::Natural< 1> meridian;

  nall::Natural< 4> daylo;
  nall::Natural< 2> dayhi;
  nall::Natural< 1> dayram;

  nall::Natural< 4> monthlo;
  nall::Natural< 1> monthhi;
  nall::Natural< 2> monthram;

  nall::Natural< 4> yearlo;
  nall::Natural< 4> yearhi;

  nall::Natural< 3> weekday;

  nall::Natural< 1> hold;
  nall::Natural< 1> calendar;
  nall::Natural< 1> irqflag;
  nall::Natural< 1> roundseconds;

  nall::Natural< 1> irqmask;
  nall::Natural< 1> irqduty;
  nall::Natural< 2> irqperiod;

  nall::Natural< 1> pause;
  nall::Natural< 1> stop;
  nall::Natural< 1> atime;  //astronomical time (24-hour mode)
  nall::Natural< 1> test;

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
