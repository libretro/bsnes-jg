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

#include <string.h>

struct serializer;

template<typename T>
struct has_serialize {
  template<typename C> static char test(decltype(std::declval<C>().serialize(std::declval<serializer&>()))*);
  template<typename C> static long test(...);
  static const bool value = sizeof(test<T>(0)) == sizeof(char);
};

struct serializer {
  enum Mode : unsigned { Load, Save, Size };

  explicit operator bool() const {
    return _size;
  }

  void setMode(Mode mode) {
    _mode = mode;
    _size = 0;
  }

  Mode mode() const {
    return _mode;
  }

  const uint8_t* data() const {
    return _data;
  }

  unsigned size() const {
    return _size;
  }

  unsigned capacity() const {
    return _capacity;
  }

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
    enum : unsigned { size = std::is_same<bool, T>::value ? 1 : sizeof(T) };
    if(_mode == Save) {
      T copy = value;
      for(unsigned n = 0; n < size; ++n) _data[_size++] = copy, copy >>= 8;
    } else if(_mode == Load) {
      value = 0;
      for(unsigned n = 0; n < size; ++n) value |= (T)_data[_size++] << (n << 3);
    } else if(_mode == Size) {
      _size += size;
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

  template<typename T> serializer& operator()(T& value, typename std::enable_if<has_serialize<T>::value>::type* = 0) { value.serialize(*this); return *this; }
  template<typename T> serializer& operator()(T& value, typename std::enable_if<std::is_integral<T>::value>::type* = 0) { return integer(value); }
  template<typename T> serializer& operator()(T& value, typename std::enable_if<std::is_floating_point<T>::value>::type* = 0) { return real(value); }
  template<typename T> serializer& operator()(T& value, typename std::enable_if<std::is_array<T>::value>::type* = 0) { return array(value); }
  template<typename T> serializer& operator()(T& value, unsigned size, typename std::enable_if<std::is_pointer<T>::value>::type* = 0) { return array(value, size); }

  serializer& operator=(const serializer& s) {
    if(_data) delete[] _data;

    _mode = s._mode;
    _data = new uint8_t[s._capacity];
    _size = s._size;
    _capacity = s._capacity;

    memcpy(_data, s._data, s._capacity);
    return *this;
  }

  serializer& operator=(serializer&& s) {
    if(_data) delete[] _data;

    _mode = s._mode;
    _data = s._data;
    _size = s._size;
    _capacity = s._capacity;

    s._data = nullptr;
    return *this;
  }

  serializer() = default;
  serializer(const serializer& s) { operator=(s); }
  serializer(serializer&& s) { operator=(std::move(s)); }

  serializer(unsigned capacity) {
    _mode = Save;
    _data = new uint8_t[capacity]();
    _size = 0;
    _capacity = capacity;
  }

  serializer(const uint8_t* data, unsigned capacity) {
    _mode = Load;
    _data = new uint8_t[capacity];
    _size = 0;
    _capacity = capacity;
    memcpy(_data, data, capacity);
  }

  ~serializer() {
    if(_data) delete[] _data;
  }

private:
  Mode _mode = Size;
  uint8_t* _data = nullptr;
  unsigned _size = 0;
  unsigned _capacity = 0;
};
