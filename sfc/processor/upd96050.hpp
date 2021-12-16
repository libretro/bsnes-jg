//NEC uPD7725
//NEC uPD96050

#pragma once

namespace Processor {

struct uPD96050 {
  auto power() -> void;
  auto exec() -> void;
  auto serialize(serializer&) -> void;

  auto execOP(nall::Natural<24> opcode) -> void;
  auto execRT(nall::Natural<24> opcode) -> void;
  auto execJP(nall::Natural<24> opcode) -> void;
  auto execLD(nall::Natural<24> opcode) -> void;

  auto readSR() -> uint8_t;
  auto writeSR(uint8_t data) -> void;

  auto readDR() -> uint8_t;
  auto writeDR(uint8_t data) -> void;

  auto readDP(uint12 addr) -> uint8_t;
  auto writeDP(uint12 addr, uint8_t data) -> void;

  auto disassemble(uint14 ip) -> string;

  enum class Revision : unsigned { uPD7725, uPD96050 } revision;
  nall::Natural<24> programROM[16384];
  uint16_t dataROM[2048];
  uint16_t dataRAM[2048];

  struct Flag {
    inline operator unsigned() const {
      return ov0 << 0 | ov1 << 1 | z << 2 | c << 3 | s0 << 4 | s1 << 5;
    }

    inline auto operator=(uint16_t data) -> Flag& {
      ov0 = data >> 0 & 1;
      ov1 = data >> 1 & 1;
      z   = data >> 2 & 1;
      c   = data >> 3 & 1;
      s0  = data >> 4 & 1;
      s1  = data >> 5 & 1;
      return *this;
    }

    auto serialize(serializer&) -> void;

    Boolean ov0;  //overflow 0
    Boolean ov1;  //overflow 1
    Boolean z;    //zero
    Boolean c;    //carry
    Boolean s0;   //sign 0
    Boolean s1;   //sign 1
  };

  struct Status {
    inline operator unsigned() const {
      bool _drs = drs & !drc;  //when DRC=1, DRS=0
      return p0 << 0 | p1 << 1 | ei << 7 | sic << 8 | soc << 9 | drc << 10
           | dma << 11 | _drs << 12 | usf0 << 13 | usf1 << 14 | rqm << 15;
    }

    inline auto operator=(uint16_t data) -> Status& {
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

    auto serialize(serializer&) -> void;

    Boolean p0;    //output port 0
    Boolean p1;    //output port 1
    Boolean ei;    //enable interrupts
    Boolean sic;   //serial input control  (0 = 16-bit; 1 = 8-bit)
    Boolean soc;   //serial output control (0 = 16-bit; 1 = 8-bit)
    Boolean drc;   //data register size    (0 = 16-bit; 1 = 8-bit)
    Boolean dma;   //data register DMA mode
    Boolean drs;   //data register status  (1 = active; 0 = stopped)
    Boolean usf0;  //user flag 0
    Boolean usf1;  //user flag 1
    Boolean rqm;   //request for master (=1 on internal access; =0 on external access)

    //internal
    Boolean siack;  //serial input acknowledge
    Boolean soack;  //serial output acknowledge
  };

  struct Registers {
    auto serialize(serializer&) -> void;

    uint16_t stack[16];    //LIFO
    VariadicNatural pc;    //program counter
    VariadicNatural rp;    //ROM pointer
    VariadicNatural dp;    //data pointer
    uint4 sp;              //stack pointer
    uint16_t si;           //serial input
    uint16_t so;           //serial output
    int16_t k;
    int16_t l;
    int16_t m;
    int16_t n;
    int16_t a;             //accumulator
    int16_t b;             //accumulator
    uint16_t tr;           //temporary register
    uint16_t trb;          //temporary register
    uint16_t dr;           //data register
    Status sr;             //status register
  } regs;

  struct Flags {
    Flag a;
    Flag b;
  } flags;
};

}
