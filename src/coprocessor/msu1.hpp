struct MSU1 : Thread {
  Emulator::Stream *stream;

  void synchronizeCPU();
  static void Enter();
  void main();
  void step(unsigned clocks);
  void unload();
  void power();

  void dataOpen();
  void audioOpen();

  uint8_t readIO(unsigned addr, uint8_t data);
  void writeIO(unsigned addr, uint8_t data);

  void serialize(serializer&);

private:
  std::ifstream dataFile;
  std::ifstream audioFile;

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

    bool audioError;
    bool audioPlay;
    bool audioRepeat;
    bool audioBusy;
    bool dataBusy;
  } io;
};

extern MSU1 msu1;
