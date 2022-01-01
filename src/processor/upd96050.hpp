//NEC uPD7725
//NEC uPD96050

#pragma once

namespace Processor {

struct uPD96050 {
  void power();
  void exec();
  void serialize(serializer&);

  void execOP(nall::Natural<24> opcode);
  void execRT(nall::Natural<24> opcode);
  void execJP(nall::Natural<24> opcode);
  void execLD(nall::Natural<24> opcode);

  uint8_t readSR();
  void writeSR(uint8_t data);

  uint8_t readDR();
  void writeDR(uint8_t data);

  uint8_t readDP(nall::Natural<12> addr);
  void writeDP(nall::Natural<12> addr, uint8_t data);

  //nall::string disassemble(nall::Natural<14> ip);

  enum class Revision : unsigned { uPD7725, uPD96050 } revision;
  nall::Natural<24> programROM[16384];
  uint16_t dataROM[2048];
  uint16_t dataRAM[2048];

  struct Flag {
    inline operator unsigned() const {
      return ov0 << 0 | ov1 << 1 | z << 2 | c << 3 | s0 << 4 | s1 << 5;
    }

    inline Flag& operator=(uint16_t data) {
      ov0 = data >> 0 & 1;
      ov1 = data >> 1 & 1;
      z   = data >> 2 & 1;
      c   = data >> 3 & 1;
      s0  = data >> 4 & 1;
      s1  = data >> 5 & 1;
      return *this;
    }

    void serialize(serializer&);

    bool ov0;  //overflow 0
    bool ov1;  //overflow 1
    bool z;    //zero
    bool c;    //carry
    bool s0;   //sign 0
    bool s1;   //sign 1
  };

  struct Status {
    inline operator unsigned() const {
      bool _drs = drs & !drc;  //when DRC=1, DRS=0
      return p0 << 0 | p1 << 1 | ei << 7 | sic << 8 | soc << 9 | drc << 10
           | dma << 11 | _drs << 12 | usf0 << 13 | usf1 << 14 | rqm << 15;
    }

    inline Status& operator=(uint16_t data) {
      p0   = data >>  0 & 1;
      p1   = data >>  1 & 1;
      ei   = data >>  7 & 1;
      sic  = data >>  8 & 1;
      soc  = data >>  9 & 1;
      drc  = data >> 10 & 1;
      dma  = data >> 11 & 1;
      drs  = data >> 12 & 1;
      usf0 = data >> 13 & 1;
      usf1 = data >> 14 & 1;
      rqm  = data >> 15 & 1;
      return *this;
    }

    void serialize(serializer&);

    bool p0;    //output port 0
    bool p1;    //output port 1
    bool ei;    //enable interrupts
    bool sic;   //serial input control  (0 = 16-bit; 1 = 8-bit)
    bool soc;   //serial output control (0 = 16-bit; 1 = 8-bit)
    bool drc;   //data register size    (0 = 16-bit; 1 = 8-bit)
    bool dma;   //data register DMA mode
    bool drs;   //data register status  (1 = active; 0 = stopped)
    bool usf0;  //user flag 0
    bool usf1;  //user flag 1
    bool rqm;   //request for master (=1 on internal access; =0 on external access)

    //internal
    bool siack;  //serial input acknowledge
    bool soack;  //serial output acknowledge
  };

  struct Registers {
    void serialize(serializer&);

    uint16_t stack[16];        //LIFO
    nall::VariadicNatural pc;  //program counter
    nall::VariadicNatural rp;  //ROM pointer
    nall::VariadicNatural dp;  //data pointer
    nall::Natural< 4> sp;      //stack pointer
    uint16_t si;               //serial input
    uint16_t so;               //serial output
    int16_t k;
    int16_t l;
    int16_t m;
    int16_t n;
    int16_t a;                 //accumulator
    int16_t b;                 //accumulator
    uint16_t tr;               //temporary register
    uint16_t trb;              //temporary register
    uint16_t dr;               //data register
    Status sr;                 //status register
  } regs;

  struct Flags {
    Flag a;
    Flag b;
  } flags;
};

}
