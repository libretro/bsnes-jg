/*
 * bsnes-jg - Super Nintendo emulator
 *
 * Copyright (C) 2004-2020 byuu
 * Copyright (C) 2020-2022 Rupert Carmichael
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, specifically version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "../memory.hpp"
#include "../sfc.hpp"

#include "../nall/primitives.hpp"

namespace SuperFamicom {

//MaskROMs supported:
//  Sharp LH5S4TNI (MaskROM 512K x 8-bit) [BSMC-CR-01: BSMC-ZS5J-JPN, BSMC-YS5J-JPN]
//  Sharp LH534VNF (MaskROM 512K x 8-bit) [BSMC-BR-01: BSMC-ZX3J-JPN]

//Flash chips supported: (16-bit modes unsupported)
//  Sharp LH28F800SUT-ZI (Flash 16 x 65536 x 8-bit) [BSMC-AF-01: BSMC-HM-JPN]
//  Sharp LH28F016SU ??? (Flash 32 x 65536 x 8-bit) [unreleased: experimental]
//  Sharp LH28F032SU ??? (Flash 64 x 65536 x 8-bit) [unreleased: experimental]

//unsupported:
//  Sharp LH28F400SU ??? (Flash 32 x 16384 x 8-bit) [unreleased] {vendor ID: 0x00'b0; device ID: 0x66'21}

//notes:
//timing emulation is only present for block erase commands
//other commands generally complete so quickly that it's unnecessary (eg 70-120ns for writes)
//suspend, resume, abort, ready/busy modes are not supported

struct BSMemory : Thread, Memory {
  unsigned pathID = 0;
  unsigned ROM = 1;

  bool writable() const { return pin.writable; }
  void writable(bool writable) { pin.writable = !ROM && writable; }

  //bsmemory.cpp
  BSMemory();
  void synchronizeCPU();
  static void Enter();
  void main();
  void step(unsigned clocks);

  bool load();
  void unload();
  void power();

  uint8_t* data() override;
  unsigned size() const override;
  uint8_t read(unsigned address, uint8_t data) override;
  void write(unsigned address, uint8_t data) override;

  //serialization.cpp
  void serialize(serializer&);

  WritableMemory memory;

private:
  struct Pin {
    bool writable;  // => /WP
  } pin;

  struct Chip {
    uint16_t vendor;
    uint16_t device;
    nall::Natural<48> serial;
  } chip;

  struct Page {
    BSMemory* self = nullptr;

    void swap();
    uint8_t read(uint8_t address);
    void write(uint8_t address, uint8_t data);

    uint8_t buffer[2][256];
  } page;

  struct BlockInformation {
    BSMemory* self = nullptr;

    inline unsigned bitCount() const;
    inline unsigned byteCount() const;
    inline unsigned count() const;
  };

  struct Block : BlockInformation {
    uint8_t read(unsigned address);
    void write(unsigned address, uint8_t data);
    void erase();
    void lock();
    void update();

    nall::Natural< 4>  id;
    uint32_t erased;
    nall::Natural< 1>  locked;
    nall::Natural< 1>  erasing;

    struct Status {
      uint8_t operator()();

      nall::Natural< 1> vppLow;
      nall::Natural< 1> queueFull;
      nall::Natural< 1> aborted;
      nall::Natural< 1> failed;
      nall::Natural< 1> locked = 1;
      nall::Natural< 1> ready = 1;
    } status;
  } blocks[64];  //8mbit = 16; 16mbit = 32; 32mbit = 64

  struct Blocks : BlockInformation {
    auto operator()(nall::Natural< 6> id) -> Block&;
  } block;

  struct Compatible {
    struct Status {
      uint8_t operator()();

      nall::Natural< 1> vppLow;
      nall::Natural< 1> writeFailed;
      nall::Natural< 1> eraseFailed;
      nall::Natural< 1> eraseSuspended;
      nall::Natural< 1> ready = 1;
    } status;
  } compatible;

  struct Global {
    struct Status {
      uint8_t operator()();

      nall::Natural< 1> page;
      nall::Natural< 1> pageReady = 1;
      nall::Natural< 1> pageAvailable = 1;
      nall::Natural< 1> queueFull;
      nall::Natural< 1> sleeping;
      nall::Natural< 1> failed;
      nall::Natural< 1> suspended;
      nall::Natural< 1> ready = 1;
    } status;
  } global;

  struct Mode { enum : unsigned {
    Flash,
    Chip,
    Page,
    CompatibleStatus,
    ExtendedStatus,
  };};
  nall::Natural< 3> mode;

  struct ReadyBusyMode { enum : unsigned {
    EnableToLevelMode,
    PulseOnWrite,
    PulseOnErase,
    Disable,
  };};
  nall::Natural< 2> readyBusyMode;

  struct Queue {
    void flush();
    void pop();
    void push(nall::Natural<24> address, uint8_t data);
    unsigned size();
    nall::Natural<24> address(unsigned index);
    uint8_t data(unsigned index);

    //serialization.cpp
    void serialize(serializer&);

    struct History {
      nall::Natural< 1>  valid;
      nall::Natural<24> address;
      uint8_t  data;
    } history[4];
  } queue;

  void failed();
};

extern BSMemory bsmemory;

}
