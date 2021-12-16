struct SharpRTC : Thread {
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
  auto rtcRead(uint4 addr) -> uint4;
  auto rtcWrite(uint4 addr, uint4 data) -> void;

  auto load(const uint8_t* data) -> void;
  auto save(uint8_t* data) -> void;

  //time.cpp
  static const unsigned daysInMonth[12];
  auto tickSecond() -> void;
  auto tickMinute() -> void;
  auto tickHour() -> void;
  auto tickDay() -> void;
  auto tickMonth() -> void;
  auto tickYear() -> void;

  auto calculateWeekday(unsigned year, unsigned month, unsigned day) -> unsigned;
};

extern SharpRTC sharprtc;
