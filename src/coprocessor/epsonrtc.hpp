//Epson RTC-4513 Real-Time Clock

struct EpsonRTC : Thread {
  void synchronizeCPU();
  static void Enter();
  void main();
  void step(unsigned clocks);

  void initialize();
  void power();
  void synchronize(uint64_t timestamp);

  uint8_t read(unsigned addr, uint8_t data);
  void write(unsigned addr, uint8_t data);

  void serialize(serializer&);

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
  void rtcReset();
  nall::Natural< 4> rtcRead(nall::Natural< 4> addr);
  void rtcWrite(nall::Natural< 4> addr, nall::Natural< 4> data);

  void load(const uint8_t* data);
  void save(uint8_t* data);

  //time.cpp
  void irq(nall::Natural< 2> period);
  void duty();
  void roundSeconds();
  void tick();

  void tickSecond();
  void tickMinute();
  void tickHour();
  void tickDay();
  void tickMonth();
  void tickYear();
};

extern EpsonRTC epsonrtc;
