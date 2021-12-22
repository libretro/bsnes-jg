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
  function<void (
    function<bool ()>,      //quit
    function<void (unsigned)>,  //usleep
    function<bool ()>,      //readable
    function<bool ()>,      //writable
    function<uint8_t ()>,     //read
    function<void (uint8_t)>  //write
  )> linkInit;
  function<void (vector<nall::string>)> linkMain;

  vector<uint8_t> snesBuffer;  //SNES -> Link
  vector<uint8_t> linkBuffer;  //Link -> SNES
};
