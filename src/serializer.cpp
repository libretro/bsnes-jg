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

#include <cstring>

#include "serializer.hpp"

serializer::Mode serializer::mode() const {
  return _mode;
}

const uint8_t* serializer::data() const {
  return _data;
}

unsigned serializer::size() const {
  return _size;
}

serializer& serializer::operator=(const serializer& s) {
  if(_data) delete[] _data;

  _mode = s._mode;
  _data = new uint8_t[s._capacity];
  _size = s._size;
  _capacity = s._capacity;

  memcpy(_data, s._data, s._capacity);
  return *this;
}

serializer::serializer() = default;
serializer::serializer(const serializer& s) { operator=(s); }

serializer::serializer(unsigned capacity) {
  _mode = serializer::Save;
  _data = new uint8_t[capacity]();
  _size = 0;
  _capacity = capacity;
}

serializer::serializer(const uint8_t* data, unsigned capacity) {
  _mode = serializer::Load;
  _data = new uint8_t[capacity];
  _size = 0;
  _capacity = capacity;
  memcpy(_data, data, capacity);
}

serializer::~serializer() {
  if(_data) delete[] _data;
}

void serializer::setMode(Mode mode) {
  _mode = mode;
  _size = 0;
}
