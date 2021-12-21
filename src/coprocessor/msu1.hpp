struct MSU1 : Thread {
  Emulator::Stream *stream;

  auto synchronizeCPU() -> void;
  static auto Enter() -> void;
  auto main() -> void;
  auto step(unsigned clocks) -> void;
  auto unload() -> void;
  auto power() -> void;

  auto dataOpen() -> void;
  auto audioOpen() -> void;

  auto readIO(unsigned addr, uint8_t data) -> uint8_t;
  auto writeIO(unsigned addr, uint8_t data) -> void;

  auto serialize(serializer&) -> void;

private:
  shared_pointer<vfs::file> dataFile;
  shared_pointer<vfs::file> audioFile;

  enum Flag : unsigned {
    Revision       = 0x02,  //max: 0x07
    AudioError     = 0x08,
    AudioPlaying   = 0x10,
    AudioRepeating = 0x20,
    AudioBusy      = 0x40,
    DataBusy       = 0x80,
  };

  struct IO {
    uint32_t dataSeekOffset;
    uint32_t dataReadOffset;

    uint32_t audioPlayOffset;
    uint32_t audioLoopOffset;

    uint16_t audioTrack;
    uint8_t audioVolume;

    uint32_t audioResumeTrack;
    uint32_t audioResumeOffset;

    Boolean audioError;
    Boolean audioPlay;
    Boolean audioRepeat;
    Boolean audioBusy;
    Boolean dataBusy;
  } io;
};

extern MSU1 msu1;
