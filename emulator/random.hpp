#pragma once

namespace Emulator {

struct Random {
  enum class Entropy : unsigned { None, Low, High };

  auto operator()() -> uint64_t {
    return random();
  }

  auto entropy(Entropy entropy) -> void {
    _entropy = entropy;
    seed();
  }

  auto seed(nall::maybe<uint32_t> seed = nothing, nall::maybe<uint32_t> sequence = nothing) -> void {
    if(!seed) seed = (uint32_t)clock();
    if(!sequence) sequence = 0;

    _state = 0;
    _increment = sequence() << 1 | 1;
    step();
    _state += seed();
    step();
  }

  auto random() -> uint64_t {
    if(_entropy == Entropy::None) return 0;
    return (uint64_t)step() << 32 | (uint64_t)step() << 0;
  }

  auto bias(uint64_t bias) -> uint64_t {
    if(_entropy == Entropy::None) return bias;
    return random();
  }

  auto bound(uint64_t bound) -> uint64_t {
    uint64_t threshold = -bound % bound;
    while(true) {
      uint64_t result = random();
      if(result >= threshold) return result % bound;
    }
  }

  auto array(uint8_t* data, uint32_t size) -> void {
    if(_entropy == Entropy::None) {
      memory::fill(data, size);
      return;
    }

    if(_entropy == Entropy::High) {
      for(uint32_t address : nall::range(size)) {
        data[address] = random();
      }
      return;
    }

    //Entropy::Low
    unsigned lobit = random() & 3;
    unsigned hibit = (lobit + 8 + (random() & 3)) & 15;
    unsigned lovalue = random() & 255;
    unsigned hivalue = random() & 255;
    if((random() & 3) == 0) lovalue = 0;
    if((random() & 1) == 0) hivalue = ~lovalue;

    for(uint32_t address : nall::range(size)) {
      uint8_t value = (address & 1ull << lobit) ? lovalue : hivalue;
      if((address & 1ull << hibit)) value = ~value;
      if((random() &  511) == 0) value ^= 1 << (random() & 7);
      if((random() & 2047) == 0) value ^= 1 << (random() & 7);
      data[address] = value;
    }
  }

  auto serialize(serializer& s) -> void {
    s.integer((unsigned&)_entropy);
    s.integer(_state);
    s.integer(_increment);
  }

private:
  auto step() -> uint32_t {
    uint64_t state = _state;
    _state = state * 6364136223846793005ull + _increment;
    uint32_t xorshift = (state >> 18 ^ state) >> 27;
    uint32_t rotate = state >> 59;
    return xorshift >> rotate | xorshift << (-rotate & 31);
  }

  Entropy _entropy = Entropy::High;
  uint64_t _state;
  uint64_t _increment;
};

}
