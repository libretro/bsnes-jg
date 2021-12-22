struct S21FX : Expansion {
  S21FX();
  ~S21FX();

  static auto Enter() -> void;
  auto step(unsigned clocks) -> void;
  auto main() -> void;

  auto read(unsigned addr, uint8_t data) -> uint8_t;
  auto write(unsigned addr, uint8_t data) -> void;

private:
  auto quit() -> bool;
  auto usleep(unsigned) -> void;
  auto readable() -> bool;
  auto writable() -> bool;
  auto read() -> uint8_t;
  auto write(uint8_t) -> void;

  bool booted = false;
  uint16 resetVector;
  uint8_t ram[122];

  nall::library link;
  nall::function<void (
    nall::function<bool ()>,      //quit
    nall::function<void (unsigned)>,  //usleep
    nall::function<bool ()>,      //readable
    nall::function<bool ()>,      //writable
    nall::function<uint8_t ()>,     //read
    nall::function<void (uint8_t)>  //write
  )> linkInit;
  nall::function<void (vector<nall::string>)> linkMain;

  vector<uint8_t> snesBuffer;  //SNES -> Link
  vector<uint8_t> linkBuffer;  //Link -> SNES
};
