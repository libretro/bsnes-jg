struct Cartridge {
  unsigned pathID() const { return information.pathID; }
  std::string region() const { return information.region; }
  nall::string headerTitle() const { return game.title; }

  std::vector<nall::string> hashes() const;
  std::vector<nall::string> manifests() const;
  std::vector<nall::string> titles() const;
  nall::string title() const;

  bool load();
  void save();
  void unload();

  void serialize(serializer&);

  ReadableMemory rom;
  WritableMemory ram;

  struct Information {
    unsigned pathID = 0;
    std::string region;
    nall::string sha256;
  } information;

  struct Has {
    nall::Boolean ICD;
    nall::Boolean MCC;
    nall::Boolean DIP;
    nall::Boolean Event;
    nall::Boolean SA1;
    nall::Boolean SuperFX;
    nall::Boolean ARMDSP;
    nall::Boolean HitachiDSP;
    nall::Boolean NECDSP;
    nall::Boolean EpsonRTC;
    nall::Boolean SharpRTC;
    nall::Boolean SPC7110;
    nall::Boolean SDD1;
    nall::Boolean OBC1;
    nall::Boolean MSU1;

    nall::Boolean Cx4;
    nall::Boolean DSP1;
    nall::Boolean DSP2;
    nall::Boolean DSP4;
    nall::Boolean ST0010;

    nall::Boolean GameBoySlot;
    nall::Boolean BSMemorySlot;
    nall::Boolean SufamiTurboSlotA;
    nall::Boolean SufamiTurboSlotB;
  } has;

private:
  Emulator::Game game;
  Emulator::Game slotGameBoy;
  Emulator::Game slotBSMemory;
  Emulator::Game slotSufamiTurboA;
  Emulator::Game slotSufamiTurboB;
  nall::Markup::Node board;

  //cartridge.cpp
  bool loadBSMemory();
  bool loadSufamiTurboA();
  bool loadSufamiTurboB();

  //load.cpp
  nall::Markup::Node loadBoard(nall::string);
  void loadCartridge(nall::Markup::Node);
  void loadCartridgeBSMemory(nall::Markup::Node);
  void loadCartridgeSufamiTurboA(nall::Markup::Node);
  void loadCartridgeSufamiTurboB(nall::Markup::Node);

  void loadMemory(Memory&, nall::Markup::Node, bool required);
  template<typename T> unsigned loadMap(nall::Markup::Node, T&);
  unsigned loadMap(nall::Markup::Node, const nall::function<uint8_t (unsigned, uint8_t)>&, const nall::function<void (unsigned, uint8_t)>&);

  void loadROM(nall::Markup::Node);
  void loadRAM(nall::Markup::Node);
  void loadICD(nall::Markup::Node);
  void loadMCC(nall::Markup::Node);
  void loadBSMemory(nall::Markup::Node);
  void loadSufamiTurboA(nall::Markup::Node);
  void loadSufamiTurboB(nall::Markup::Node);
  void loadDIP(nall::Markup::Node);
  void loadEvent(nall::Markup::Node);
  void loadSA1(nall::Markup::Node);
  void loadSuperFX(nall::Markup::Node);
  void loadARMDSP(nall::Markup::Node);
  void loadHitachiDSP(nall::Markup::Node, unsigned roms);
  void loaduPD7725(nall::Markup::Node);
  void loaduPD96050(nall::Markup::Node);
  void loadEpsonRTC(nall::Markup::Node);
  void loadSharpRTC(nall::Markup::Node);
  void loadSPC7110(nall::Markup::Node);
  void loadSDD1(nall::Markup::Node);
  void loadOBC1(nall::Markup::Node);
  void loadMSU1();

  //save.cpp
  void saveCartridge(nall::Markup::Node);
  void saveCartridgeBSMemory(nall::Markup::Node);
  void saveCartridgeSufamiTurboA(nall::Markup::Node);
  void saveCartridgeSufamiTurboB(nall::Markup::Node);

  void saveMemory(Memory&, nall::Markup::Node);

  void saveRAM(nall::Markup::Node);
  void saveMCC(nall::Markup::Node);
  void saveSA1(nall::Markup::Node);
  void saveSuperFX(nall::Markup::Node);
  void saveARMDSP(nall::Markup::Node);
  void saveHitachiDSP(nall::Markup::Node);
  void saveuPD7725(nall::Markup::Node);
  void saveuPD96050(nall::Markup::Node);
  void saveEpsonRTC(nall::Markup::Node);
  void saveSharpRTC(nall::Markup::Node);
  void saveSPC7110(nall::Markup::Node);
  void saveOBC1(nall::Markup::Node);

  friend class Interface;
  friend class ICD;
};

extern Cartridge cartridge;
