/*
 * nall - C++ template library
 *
 * Copyright © 2006-2020 byuu et al
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#pragma once

//serializer: a class designed to save and restore the state of classes.
//
//benefits:
//- data() will be portable in size (it is not necessary to specify type sizes.)
//- data() will be portable in endianness (always stored internally as little-endian.)
//- one serialize function can both save and restore class states.
//
//caveats:
//- only plain-old-data can be stored. complex classes must provide serialize(serializer&);
//- floating-point usage is not portable across different implementations

#include <cstdint>
#include <utility>

struct serializer;

template<typename T>
struct has_serialize {
  template<typename C> static char test(decltype(std::declval<C>().serialize(std::declval<serializer&>()))*);
  template<typename C> static long test(...);
  static const bool value = sizeof(test<T>(0)) == sizeof(char);
};

struct serializer {
  enum Mode : unsigned { Load, Save, Size };

  Mode mode() const;
  const uint8_t* data() const;
  unsigned size() const;

  void setMode(Mode);

  template<typename T> serializer& boolean(T& value) {
    if(_mode == Save) {
      _data[_size++] = (bool)value;
    } else if(_mode == Load) {
      value = (bool)_data[_size++];
    } else if(_mode == Size) {
      _size += 1;
    }
    return *this;
  }

  template<typename T> serializer& integer(T& value) {
    enum : unsigned { intsize = std::is_same<bool, T>::value ? 1 : sizeof(T) };
    if(_mode == Save) {
      T copy = value;
      for(unsigned n = 0; n < intsize; ++n) _data[_size++] = copy, copy >>= 8;
    } else if(_mode == Load) {
      value = 0;
      for(unsigned n = 0; n < intsize; ++n) value |= (T)_data[_size++] << (n << 3);
    } else if(_mode == Size) {
      _size += intsize;
    }
    return *this;
  }

  template<typename T, int N> serializer& array(T (&array)[N]) {
    for(unsigned n = 0; n < N; ++n) operator()(array[n]);
    return *this;
  }

  template<typename T> serializer& array(T array, unsigned size) {
    for(unsigned n = 0; n < size; ++n) operator()(array[n]);
    return *this;
  }

  //optimized specializations

  template<int N> serializer& array(uint8_t (&data)[N]) {
    return array(data, N);
  }

  template<typename T> serializer& operator()(T& value, typename std::enable_if<has_serialize<T>::value>::type* = 0) {
    value.serialize(*this);
    return *this;
  }

  template<typename T> serializer& operator()(T& value, typename std::enable_if<std::is_integral<T>::value>::type* = 0) {
    return integer(value);
  }

  serializer& operator=(const serializer&);
  serializer();
  serializer(const serializer&);
  serializer(unsigned);
  serializer(const uint8_t*, unsigned);
  ~serializer();

private:
  Mode _mode = Size;
  uint8_t* _data = nullptr;
  unsigned _size = 0;
  unsigned _capacity = 0;
};
