struct Cartridge {
  unsigned pathID() const { return information.pathID; }
  string region() const { return information.region; }
  string headerTitle() const { return game.title; }

  std::vector<string> hashes() const;
  std::vector<string> manifests() const;
  std::vector<string> titles() const;
  string title() const;

  bool load();
  void save();
  void unload();

  void serialize(serializer&);

  ReadableMemory rom;
  WritableMemory ram;

  struct Information {
    unsigned pathID = 0;
    string region;
    string sha256;
  } information;

  struct Has {
    Boolean ICD;
    Boolean MCC;
    Boolean DIP;
    Boolean Event;
    Boolean SA1;
    Boolean SuperFX;
    Boolean ARMDSP;
    Boolean HitachiDSP;
    Boolean NECDSP;
    Boolean EpsonRTC;
    Boolean SharpRTC;
    Boolean SPC7110;
    Boolean SDD1;
    Boolean OBC1;
    Boolean MSU1;

    Boolean Cx4;
    Boolean DSP1;
    Boolean DSP2;
    Boolean DSP4;
    Boolean ST0010;

    Boolean GameBoySlot;
    Boolean BSMemorySlot;
    Boolean SufamiTurboSlotA;
    Boolean SufamiTurboSlotB;
  } has;

private:
  Emulator::Game game;
  Emulator::Game slotGameBoy;
  Emulator::Game slotBSMemory;
  Emulator::Game slotSufamiTurboA;
  Emulator::Game slotSufamiTurboB;
  Markup::Node board;

  //cartridge.cpp
  bool loadBSMemory();
  bool loadSufamiTurboA();
  bool loadSufamiTurboB();

  //load.cpp
  Markup::Node loadBoard(string);
  void loadCartridge(Markup::Node);
  void loadCartridgeBSMemory(Markup::Node);
  void loadCartridgeSufamiTurboA(Markup::Node);
  void loadCartridgeSufamiTurboB(Markup::Node);

  void loadMemory(Memory&, Markup::Node, bool required);
  template<typename T> unsigned loadMap(Markup::Node, T&);
  unsigned loadMap(Markup::Node, const function<uint8_t (unsigned, uint8_t)>&, const function<void (unsigned, uint8_t)>&);

  void loadROM(Markup::Node);
  void loadRAM(Markup::Node);
  void loadICD(Markup::Node);
  void loadMCC(Markup::Node);
  void loadBSMemory(Markup::Node);
  void loadSufamiTurboA(Markup::Node);
  void loadSufamiTurboB(Markup::Node);
  void loadDIP(Markup::Node);
  void loadEvent(Markup::Node);
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
