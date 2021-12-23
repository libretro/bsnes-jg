struct System {
  enum class Region : unsigned { NTSC, PAL };

  inline bool loaded() const { return information.loaded; }
  inline Region region() const { return information.region; }
  inline double cpuFrequency() const { return information.cpuFrequency; }
  inline double apuFrequency() const { return information.apuFrequency; }

  inline bool fastPPU() const { return hacks.fastPPU; }

  void run();
  void runToSave();
  void runToSaveFast();
  void runToSaveStrict();
  void frameEvent();

  bool load(Emulator::Interface*);
  void save();
  void unload();
  void power(bool reset);

  //serialization.cpp
  serializer serialize(bool synchronize);
  bool unserialize(serializer&);

  unsigned frameSkip = 0;
  unsigned frameCounter = 0;
  bool runAhead = 0;

private:
  Emulator::Interface* interface = nullptr;

  struct Information {
    bool loaded = false;
    Region region = Region::NTSC;
    double cpuFrequency = Emulator::Constants::Colorburst::NTSC * 6.0;
    double apuFrequency = 32040.0 * 768.0;
    unsigned serializeSize[2] = {0, 0};
  } information;

  struct Hacks {
    bool fastPPU = false;
  } hacks;

  void serializeAll(serializer&, bool synchronize);
  unsigned serializeInit(bool synchronize);

  friend class Cartridge;
};

extern System system;

bool Region::NTSC() { return system.region() == System::Region::NTSC; }
bool Region::PAL() { return system.region() == System::Region::PAL; }
