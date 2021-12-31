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
  void loadCartridgeBSMemory(Markup::Node);
  void loadCartridgeSufamiTurboA(Markup::Node);
  void loadCartridgeSufamiTurboB(Markup::Node);

  void loadMemory(Memory&, std::string);
  template<typename T> unsigned loadMap(std::string, T&);
  unsigned loadMap(std::string, const nall::function<uint8_t (unsigned, uint8_t)>&, const nall::function<void (unsigned, uint8_t)>&);

  void loadMemory(Memory&, Markup::Node);
  template<typename T> unsigned loadMap(Markup::Node, T&);
  unsigned loadMap(Markup::Node, const nall::function<uint8_t (unsigned, uint8_t)>&, const nall::function<void (unsigned, uint8_t)>&);

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

  void loadSDD1(std::string);
  void loadOBC1(std::string);

  void loadROM(Markup::Node);
  void loadRAM(Markup::Node);
  void loadICD(Markup::Node);
  void loadMCC(Markup::Node);
  void loadBSMemory(Markup::Node);
  void loadSufamiTurboA(Markup::Node);
  void loadSufamiTurboB(Markup::Node);
  void loadDIP(Markup::Node);
  void loadSA1(Markup::Node);
  void loadSuperFX(Markup::Node);
  void loadARMDSP(Markup::Node);
  void loadHitachiDSP(Markup::Node, unsigned roms);
  void loaduPD7725(Markup::Node);
  void loaduPD96050(Markup::Node);
  void loadEpsonRTC(Markup::Node);
  void loadSharpRTC(Markup::Node);
  void loadSPC7110(Markup::Node);
  void loadSDD1(Markup::Node);
  void loadOBC1(Markup::Node);
  void loadMSU1();

  //save.cpp
  void saveCartridge(Markup::Node);
  void saveCartridgeBSMemory(Markup::Node);
  void saveCartridgeSufamiTurboA(Markup::Node);
  void saveCartridgeSufamiTurboB(Markup::Node);

  void saveMemory(Memory&, Markup::Node);

  void saveRAM(Markup::Node);
  void saveMCC(Markup::Node);
  void saveSA1(Markup::Node);
  void saveSuperFX(Markup::Node);
  void saveARMDSP(Markup::Node);
  void saveHitachiDSP(Markup::Node);
  void saveuPD7725(Markup::Node);
  void saveuPD96050(Markup::Node);
  void saveEpsonRTC(Markup::Node);
  void saveSharpRTC(Markup::Node);
  void saveSPC7110(Markup::Node);
  void saveOBC1(Markup::Node);

  friend class Interface;
  friend class ICD;
};

extern Cartridge cartridge;
