//Super Accelerator (SA-1)

struct SA1 : Processor::WDC65816, Thread {
  inline auto synchronizing() const -> bool override { return scheduler.synchronizing(); }

  //sa1.cpp
  auto synchronizeCPU() -> void;
  static auto Enter() -> void;
  auto main() -> void;
  auto step() -> void;
  auto interrupt() -> void override;

  inline auto triggerIRQ() -> void;
  inline auto lastCycle() -> void override;
  inline auto interruptPending() const -> bool override;

  auto unload() -> void;
  auto power() -> void;

  //dma.cpp
  struct DMA {
    enum CDEN : unsigned { DmaNormal = 0, DmaCharConversion = 1 };
    enum SD : unsigned { SourceROM = 0, SourceBWRAM = 1, SourceIRAM = 2 };
    enum DD : unsigned { DestIRAM = 0, DestBWRAM = 1 };
    unsigned line;
  };

  auto dmaNormal() -> void;
  auto dmaCC1() -> void;
  auto dmaCC1Read(unsigned addr) -> uint8_t;
  auto dmaCC2() -> void;

  //memory.cpp
  inline auto conflictROM() const -> bool;
  inline auto conflictBWRAM() const -> bool;
  inline auto conflictIRAM() const -> bool;

  inline auto idle() -> void override;
  inline auto idleJump() -> void override;
  inline auto idleBranch() -> void override;
  inline auto read(unsigned address) -> uint8_t override;
  inline auto write(unsigned address, uint8_t data) -> void override;
  auto readVBR(unsigned address, uint8_t data = 0) -> uint8_t;
  auto readDisassembler(unsigned address) -> uint8_t override;

  //io.cpp
  auto readIOCPU(unsigned address, uint8_t data) -> uint8_t;
  auto readIOSA1(unsigned address, uint8_t data) -> uint8_t;
  auto writeIOCPU(unsigned address, uint8_t data) -> void;
  auto writeIOSA1(unsigned address, uint8_t data) -> void;
  auto writeIOShared(unsigned address, uint8_t data) -> void;

  //serialization.cpp
  auto serialize(serializer&) -> void;

  struct ROM : ReadableMemory {
    //rom.cpp
    inline auto conflict() const -> bool;

    inline auto read(unsigned address, uint8_t data = 0) -> uint8_t override;
    inline auto write(unsigned address, uint8_t data) -> void override;

    auto readCPU(unsigned address, uint8_t data = 0) -> uint8_t;
    auto writeCPU(unsigned address, uint8_t data) -> void;

    auto readSA1(unsigned address, uint8_t data = 0) -> uint8_t;
    auto writeSA1(unsigned address, uint8_t data) -> void;
  } rom;

  struct BWRAM : WritableMemory {
    //bwram.cpp
    inline auto conflict() const -> bool;

    inline auto read(unsigned address, uint8_t data = 0) -> uint8_t override;
    inline auto write(unsigned address, uint8_t data) -> void override;

    auto readCPU(unsigned address, uint8_t data = 0) -> uint8_t;
    auto writeCPU(unsigned address, uint8_t data) -> void;

    auto readSA1(unsigned address, uint8_t data = 0) -> uint8_t;
    auto writeSA1(unsigned address, uint8_t data) -> void;

    auto readLinear(unsigned address, uint8_t data = 0) -> uint8_t;
    auto writeLinear(unsigned address, uint8_t data) -> void;

    auto readBitmap(nall::Natural<20> address, uint8_t data = 0) -> uint8_t;
    auto writeBitmap(nall::Natural<20> address, uint8_t data) -> void;

    bool dma;
  } bwram;

  struct IRAM : WritableMemory {
    //iram.cpp
    inline auto conflict() const -> bool;

    inline auto read(unsigned address, uint8_t data = 0) -> uint8_t override;
    inline auto write(unsigned address, uint8_t data) -> void override;

    auto readCPU(unsigned address, uint8_t data) -> uint8_t;
    auto writeCPU(unsigned address, uint8_t data) -> void;

    auto readSA1(unsigned address, uint8_t data = 0) -> uint8_t;
    auto writeSA1(unsigned address, uint8_t data) -> void;
  } iram;

private:
  DMA dma;

  struct Status {
    uint8_t counter;

    bool interruptPending;

    uint16_t scanlines;
    uint16_t vcounter;
    uint16_t hcounter;
  } status;

  struct MMIO {
    //$2200 CCNT
    bool sa1_irq;
    bool sa1_rdyb;
    bool sa1_resb;
    bool sa1_nmi;
    uint8_t smeg;

    //$2201 SIE
    bool cpu_irqen;
    bool chdma_irqen;

    //$2202 SIC
    bool cpu_irqcl;
    bool chdma_irqcl;

    //$2203,$2204 CRV
    uint16_t crv;

    //$2205,$2206 CNV
    uint16_t cnv;

    //$2207,$2208 CIV
    uint16_t civ;

    //$2209 SCNT
    bool cpu_irq;
    bool cpu_ivsw;
    bool cpu_nvsw;
    uint8_t cmeg;

    //$220a CIE
    bool sa1_irqen;
    bool timer_irqen;
    bool dma_irqen;
    bool sa1_nmien;

    //$220b CIC
    bool sa1_irqcl;
    bool timer_irqcl;
    bool dma_irqcl;
    bool sa1_nmicl;

    //$220c,$220d SNV
    uint16_t snv;

    //$220e,$220f SIV
    uint16_t siv;

    //$2210 TMC
    bool hvselb;
    bool ven;
    bool hen;

    //$2212,$2213
    uint16_t hcnt;

    //$2214,$2215
    uint16_t vcnt;

    //$2220 CXB
    bool cbmode;
    unsigned cb;

    //$2221 DXB
    bool dbmode;
    unsigned db;

    //$2222 EXB
    bool ebmode;
    unsigned eb;

    //$2223 FXB
    bool fbmode;
    unsigned fb;

    //$2224 BMAPS
    uint8_t sbm;

    //$2225 BMAP
    bool sw46;
    uint8_t cbm;

    //$2226 SBWE
    bool swen;

    //$2227 CBWE
    bool cwen;

    //$2228 BWPA
    uint8_t bwp;

    //$2229 SIWP
    uint8_t siwp;

    //$222a CIWP
    uint8_t ciwp;

    //$2230 DCNT
    bool dmaen;
    bool dprio;
    bool cden;
    bool cdsel;
    bool dd;
    uint8_t sd;

    //$2231 CDMA
    bool chdend;
    uint8_t dmasize;
    uint8_t dmacb;

    //$2232-$2234 SDA
    uint32_t dsa;

    //$2235-$2237 DDA
    uint32_t dda;

    //$2238,$2239 DTC
    uint16_t dtc;

    //$223f BBF
    bool bbf;

    //$2240-224f BRF
    uint8_t brf[16];

    //$2250 MCNT
    bool acm;
    bool md;

    //$2251,$2252 MA
    uint16_t ma;

    //$2253,$2254 MB
    uint16_t mb;

    //$2258 VBD
    bool hl;
    uint8_t vb;

    //$2259-$225b VDA
    uint32_t va;
    uint8_t vbit;

    //$2300 SFR
    bool cpu_irqfl;
    bool chdma_irqfl;

    //$2301 CFR
    bool sa1_irqfl;
    bool timer_irqfl;
    bool dma_irqfl;
    bool sa1_nmifl;

    //$2302,$2303 HCR
    uint16_t hcr;

    //$2304,$2305 VCR
    uint16_t vcr;

    //$2306-230a MR
    uint64_t mr;

    //$230b OF
    bool overflow;
  } mmio;
};

extern SA1 sa1;
