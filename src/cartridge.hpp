struct Cartridge {
  unsigned pathID() const { return information.pathID; }
  std::string region() const { return information.region; }
  std::string headerTitle() const { return game.title; }

  std::vector<std::string> hashes() const;
  std::vector<std::string> manifests() const;
  std::vector<std::string> titles() const;
  std::string title() const;

  bool load();
  void save();
  void unload();

  void serialize(serializer&);

  ReadableMemory rom;
  WritableMemory ram;

  struct Information {
    unsigned pathID = 0;
    std::string region;
    std::string sha256;
  } information;

  struct Has {
    bool ICD;
    bool MCC;
    bool DIP;
    bool Event;
    bool SA1;
    bool SuperFX;
    bool ARMDSP;
    bool HitachiDSP;
    bool NECDSP;
    bool EpsonRTC;
    bool SharpRTC;
    bool SPC7110;
    bool SDD1;
    bool OBC1;
    bool MSU1;

    bool Cx4;
    bool DSP1;
    bool DSP2;
    bool DSP4;
    bool ST0010;

    bool GameBoySlot;
    bool BSMemorySlot;
    bool SufamiTurboSlotA;
    bool SufamiTurboSlotB;
  } has;

private:
  Emulator::Game game;
  Emulator::Game slotGameBoy;
  Emulator::Game slotBSMemory;
  Emulator::Game slotSufamiTurboA;
  Emulator::Game slotSufamiTurboB;
  Markup::Node board;
  std::string stdboard;

  //cartridge.cpp
  bool loadBSMemory();
  bool loadSufamiTurboA();
  bool loadSufamiTurboB();

  //load.cpp
  std::string loadBoard(std::string board);
  Markup::Node loadBoard(nall::string);
  void loadCartridge(Markup::Node);
  void loadCartridgeBSMemory(std::string);
  void loadCartridgeSufamiTurboA(std::string);
  void loadCartridgeSufamiTurboB(std::string);

  void loadMemory(Memory&, std::string);
  template<typename T> unsigned loadMap(std::string, T&);
  unsigned loadMap(std::string, const nall::function<uint8_t (unsigned, uint8_t)>&, const nall::function<void (unsigned, uint8_t)>&);

  void loadROM(std::string);
  void loadRAM(std::string);
  void loadICD(std::string);
  void loadMCC(std::string);
  void loadBSMemory(std::string);
  void loadSufamiTurboA(std::string);
  void loadSufamiTurboB(std::string);
  void loadDIP(std::string);
  void loadSA1(std::string);
  void loadSuperFX(std::string);
  void loadARMDSP(std::string);
  void loadHitachiDSP(std::string, unsigned roms);
  void loaduPD7725(std::string);
  void loaduPD96050(std::string);
  void loadSPC7110(std::string);
  void loadSDD1(std::string);
  void loadOBC1(std::string);
  void loadEpsonRTC(std::string);
  void loadSharpRTC(std::string);
  void loadMSU1();

  //save.cpp
  void saveCartridge(std::string);
  void saveCartridge(Markup::Node);
  void saveCartridgeBSMemory(std::string);
  void saveCartridgeSufamiTurboA(std::string);
  void saveCartridgeSufamiTurboB(std::string);

  void saveMemory(Memory&, std::string);
  void saveMemory(Memory&, Markup::Node);

  void saveRAM(std::string);
  void saveMCC(std::string);
  void saveSA1(std::string);
  void saveSuperFX(std::string);
  void saveARMDSP(std::string);
  void saveSPC7110(std::string);

  void saveEpsonRTC(std::string);
  void saveSharpRTC(std::string);

  void saveHitachiDSP(Markup::Node);
  void saveuPD7725(Markup::Node);
  void saveuPD96050(Markup::Node);
  void saveOBC1(Markup::Node);

  friend class Interface;
  friend class ICD;
};

extern Cartridge cartridge;
