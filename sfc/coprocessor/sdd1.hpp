struct SDD1 {
  auto unload() -> void;
  auto power() -> void;

  auto ioRead(uint addr, uint8_t data) -> uint8_t;
  auto ioWrite(uint addr, uint8_t data) -> void;

  auto dmaRead(uint addr, uint8_t data) -> uint8_t;
  auto dmaWrite(uint addr, uint8_t data) -> void;

  auto mmcRead(uint addr) -> uint8_t;

  auto mcuRead(uint addr, uint8_t data) -> uint8_t;
  auto mcuWrite(uint addr, uint8_t data) -> void;

  auto serialize(serializer&) -> void;

  ReadableMemory rom;

private:
  uint8_t r4800;  //hard enable
  uint8_t r4801;  //soft enable
  uint8_t r4804;  //MMC bank 0
  uint8_t r4805;  //MMC bank 1
  uint8_t r4806;  //MMC bank 2
  uint8_t r4807;  //MMC bank 3

  struct DMA {
    uint24 addr;  //$43x2-$43x4 -- DMA transfer address
    uint16_t size;  //$43x5-$43x6 -- DMA transfer size
  } dma[8];
  bool dmaReady;  //used to initialize decompression module

public:
  struct Decompressor {
    struct IM {  //input manager
      IM(SDD1::Decompressor& self) : self(self) {}
      auto init(uint offset) -> void;
      auto getCodeWord(uint8_t codeLength) -> uint8_t;
      auto serialize(serializer&) -> void;

    private:
      Decompressor& self;
      uint offset;
      uint bitCount;
    };

    struct GCD {  //golomb-code decoder
      GCD(SDD1::Decompressor& self) : self(self) {}
      auto getRunCount(uint8_t codeNumber, uint8_t &mpsCount, bool& lpsIndex) -> void;
      auto serialize(serializer&) -> void;

    private:
      Decompressor& self;
      static const uint8_t runCount[256];
    };

    struct BG {  //bits generator
      BG(SDD1::Decompressor& self, uint8_t codeNumber) : self(self), codeNumber(codeNumber) {}
      auto init() -> void;
      auto getBit(bool& endOfRun) -> uint8_t;
      auto serialize(serializer&) -> void;

    private:
      Decompressor& self;
      const uint8_t codeNumber;
      uint8_t mpsCount;
      bool lpsIndex;
    };

    struct PEM {  //probability estimation module
      PEM(SDD1::Decompressor& self) : self(self) {}
      auto init() -> void;
      auto getBit(uint8_t context) -> uint8_t;
      auto serialize(serializer&) -> void;

    private:
      Decompressor& self;
      struct State {
        uint8_t codeNumber;
        uint8_t nextIfMps;
        uint8_t nextIfLps;
      };
      static const State evolutionTable[33];
      struct ContextInfo {
        uint8_t status;
        uint8_t mps;
      } contextInfo[32];
    };

    struct CM {  //context model
      CM(SDD1::Decompressor& self) : self(self) {}
      auto init(uint offset) -> void;
      auto getBit() -> uint8_t;
      auto serialize(serializer&) -> void;

    private:
      Decompressor& self;
      uint8_t bitplanesInfo;
      uint8_t contextBitsInfo;
      uint8_t bitNumber;
      uint8_t currentBitplane;
      uint16_t previousBitplaneBits[8];
    };

    struct OL {  //output logic
      OL(SDD1::Decompressor& self) : self(self) {}
      auto init(uint offset) -> void;
      auto decompress() -> uint8_t;
      auto serialize(serializer&) -> void;

    private:
      Decompressor& self;
      uint8_t bitplanesInfo;
      uint8_t r0, r1, r2;
    };

    Decompressor();
    auto init(uint offset) -> void;
    auto read() -> uint8_t;
    auto serialize(serializer&) -> void;

    IM  im;
    GCD gcd;
    BG  bg0, bg1, bg2, bg3, bg4, bg5, bg6, bg7;
    PEM pem;
    CM  cm;
    OL  ol;
  };

  Decompressor decompressor;
};

extern SDD1 sdd1;
