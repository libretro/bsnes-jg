#pragma once

namespace Emulator {

struct Random {
  enum class Entropy : unsigned { None, Low, High };

  uint64_t operator()() {
    return random();
  }

  void entropy(Entropy entropy) {
    _entropy = entropy;
    seed();
  }

  void seed(std::optional<uint32_t> seed = std::nullopt, std::optional<uint32_t> sequence = std::nullopt) {
    if(!seed) seed = (uint32_t)clock();
    if(!sequence) sequence = 0;

    _state = 0;
    _increment = sequence.value() << 1 | 1;
    step();
    _state += seed.value();
    step();
  }

  uint64_t random() {
    if(_entropy == Entropy::None) return 0;
    return (uint64_t)step() << 32 | (uint64_t)step() << 0;
  }

  uint64_t bias(uint64_t bias) {
    if(_entropy == Entropy::None) return bias;
    return random();
  }

  uint64_t bound(uint64_t bound) {
    uint64_t threshold = -bound % bound;
    while(true) {
      uint64_t result = random();
      if(result >= threshold) return result % bound;
    }
  }

  void array(uint8_t* data, uint32_t size) {
    if(_entropy == Entropy::None) {
      nall::memory::fill(data, size);
      return;
    }

    if(_entropy == Entropy::High) {
      for(uint32_t address = 0; address < size; ++address) {
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

    for(uint32_t address = 0; address < size; ++address) {
      uint8_t value = (address & 1ull << lobit) ? lovalue : hivalue;
      if((address & 1ull << hibit)) value = ~value;
      if((random() &  511) == 0) value ^= 1 << (random() & 7);
      if((random() & 2047) == 0) value ^= 1 << (random() & 7);
      data[address] = value;
    }
  }

  void serialize(serializer& s) {
    s.integer((unsigned&)_entropy);
    s.integer(_state);
    s.integer(_increment);
  }

private:
  uint32_t step() {
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
