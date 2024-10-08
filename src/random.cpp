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

#include <cstring>
#include <ctime>

#include "memory.hpp"
#include "serializer.hpp"

#include "random.hpp"

namespace SuperFamicom {

Random random;

static Random::Entropy _entropy = Random::Entropy::High;
static uint64_t _state;
static uint64_t _increment;

static uint32_t step() {
  uint64_t state = _state;
  _state = state * 6364136223846793005ull + _increment;
  uint32_t xorshift = (state >> 18 ^ state) >> 27;
  uint32_t rotate = state >> 59;
  return xorshift >> rotate | xorshift << (-rotate & 31);
}

static void seed() {
  uint32_t seed = (uint32_t)clock();
  uint32_t sequence = 0;

  _state = 0;
  _increment = sequence << 1 | 1;
  step();
  _state += seed;
  step();
}

uint64_t Random::operator()() {
  return random();
}

void Random::entropy(Random::Entropy entropy) {
  _entropy = entropy;
  seed();
}

uint64_t Random::random() {
  if(_entropy == Random::Entropy::None) return 0;
  return (uint64_t)step() << 32 | (uint64_t)step() << 0;
}

uint64_t Random::bias(uint64_t bias) {
  if(_entropy == Random::Entropy::None) return bias;
  return random();
}

uint64_t Random::bound(uint64_t bound) {
  uint64_t threshold = -bound % bound;
  while(true) {
    uint64_t result = random();
    if(result >= threshold) return result % bound;
  }
}

void Random::array(uint8_t* data, uint32_t size) {
  if(_entropy == Random::Entropy::None) {
    std::memset(data, 0, size * sizeof(uint8_t));
    return;
  }

  if(_entropy == Random::Entropy::High) {
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

void Random::serialize(serializer& s) {
  unsigned ent = (unsigned)_entropy;
  s.integer((unsigned&)ent);
  s.integer(_state);
  s.integer(_increment);
}

}
