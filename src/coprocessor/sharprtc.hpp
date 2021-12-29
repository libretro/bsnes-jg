struct SharpRTC : Thread {
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

  enum class State : unsigned { Ready, Command, Read, Write } state;
  int index;

  unsigned second;
  unsigned minute;
  unsigned hour;
  unsigned day;
  unsigned month;
  unsigned year;
  unsigned weekday;

  //memory.cpp
  nall::Natural< 4> rtcRead(nall::Natural< 4> addr);
  void rtcWrite(nall::Natural< 4> addr, nall::Natural< 4> data);

  void load(const uint8_t* data);
  void save(uint8_t* data);

  //time.cpp
  static const unsigned daysInMonth[12];
  void tickSecond();
  void tickMinute();
  void tickHour();
  void tickDay();
  void tickMonth();
  void tickYear();

  unsigned calculateWeekday(int year, int month, int day);
};

extern SharpRTC sharprtc;
