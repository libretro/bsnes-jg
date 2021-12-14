#pragma once

namespace nall {

template<typename T> struct array_view {
  using type = array_view;

  inline array_view() {
    _data = nullptr;
    _size = 0;
  }

  inline array_view(std::nullptr_t) {
    _data = nullptr;
    _size = 0;
  }

  inline array_view(const void* data, uint64_t size) {
    _data = (const T*)data;
    _size = (int)size;
  }

  inline explicit operator bool() const { return _data && _size > 0; }

  inline operator const T*() const {
    #ifdef DEBUG
    struct out_of_bounds {};
    if(_size < 0) throw out_of_bounds{};
    #endif
    return _data;
  }

  inline auto operator++() -> type& { _data++; _size--; return *this; }
  inline auto operator--() -> type& { _data--; _size++; return *this; }

  inline auto operator++(int) -> type { auto copy = *this; ++(*this); return copy; }
  inline auto operator--(int) -> type { auto copy = *this; --(*this); return copy; }

  inline auto operator-=(int distance) -> type& { _data -= distance; _size += distance; return *this; }
  inline auto operator+=(int distance) -> type& { _data += distance; _size -= distance; return *this; }

  inline auto operator[](uint index) const -> const T& {
    #ifdef DEBUG
    struct out_of_bounds {};
    if(index >= _size) throw out_of_bounds{};
    #endif
    return _data[index];
  }

  inline auto operator()(uint index, const T& fallback = {}) const -> T {
    if(index >= _size) return fallback;
    return _data[index];
  }

  template<typename U = T> inline auto data() const -> const U* { return (const U*)_data; }
  template<typename U = T> inline auto size() const -> uint64_t { return _size * sizeof(T) / sizeof(U); }

  inline auto begin() const -> iterator_const<T> { return {_data, (uint)0}; }
  inline auto end() const -> iterator_const<T> { return {_data, (uint)_size}; }

  inline auto rbegin() const -> reverse_iterator_const<T> { return {_data, (uint)_size - 1}; }
  inline auto rend() const -> reverse_iterator_const<T> { return {_data, (uint)-1}; }

  auto read() -> T {
    auto value = operator[](0);
    _data++;
    _size--;
    return value;
  }

  auto view(uint offset, uint length) const -> type {
    #ifdef DEBUG
    struct out_of_bounds {};
    if(offset + length >= _size) throw out_of_bounds{};
    #endif
    return {_data + offset, length};
  }

  //array_view<uint8_t> specializations
  template<typename U> auto readl(U& value, uint size) -> U;
  template<typename U> auto readm(U& value, uint size) -> U;
  template<typename U> auto readvn(U& value, uint size) -> U;
  template<typename U> auto readvi(U& value, uint size) -> U;

  template<typename U> auto readl(U& value, uint offset, uint size) -> U { return view(offset, size).readl(value, size); }

  template<typename U = uint64_t> auto readl(uint size) -> U { U value; return readl(value, size); }
  template<typename U = uint64_t> auto readm(uint size) -> U { U value; return readm(value, size); }
  template<typename U = uint64_t> auto readvn(uint size) -> U { U value; return readvn(value, size); }
  template<typename U =  int64_t> auto readvi(uint size) -> U { U value; return readvi(value, size); }

  template<typename U = uint64_t> auto readl(uint offset, uint size) -> U { U value; return readl(value, offset, size); }

protected:
  const T* _data;
  int _size;
};

template<> template<typename U> inline auto array_view<uint8_t>::readl(U& value, uint size) -> U {
  value = 0;
  for(uint byte : range(size)) value |= (U)read() << byte * 8;
  return value;
}

template<> template<typename U> inline auto array_view<uint8_t>::readm(U& value, uint size) -> U {
  value = 0;
  for(uint byte : reverse(range(size))) value |= (U)read() << byte * 8;
  return value;
}

template<> template<typename U> inline auto array_view<uint8_t>::readvn(U& value, uint size) -> U {
  value = 0;
  uint shift = 1;
  while(true) {
    auto byte = read();
    value += (byte & 0x7f) * shift;
    if(byte & 0x80) break;
    shift <<= 7;
    value += shift;
  }
  return value;
}

template<> template<typename U> inline auto array_view<uint8_t>::readvi(U& value, uint size) -> U {
  value = readvn<U>();
  bool negate = value & 1;
  value >>= 1;
  if(negate) value = ~value;
  return value;
}

template<typename T> struct array_span : array_view<T> {
  using type = array_span;
  using super = array_view<T>;

  inline array_span() {
    super::_data = nullptr;
    super::_size = 0;
  }

  inline array_span(std::nullptr_t) {
    super::_data = nullptr;
    super::_size = 0;
  }

  inline array_span(void* data, uint64_t size) {
    super::_data = (T*)data;
    super::_size = (int)size;
  }

  inline operator T*() { return (T*)super::operator const T*(); }

  inline auto operator[](uint index) -> T& { return (T&)super::operator[](index); }

  template<typename U = T> inline auto data() -> U* { return (U*)super::_data; }

  inline auto begin() -> iterator<T> { return {(T*)super::_data, (uint)0}; }
  inline auto end() -> iterator<T> { return {(T*)super::_data, (uint)super::_size}; }

  inline auto rbegin() -> reverse_iterator<T> { return {(T*)super::_data, (uint)super::_size - 1}; }
  inline auto rend() -> reverse_iterator<T> { return {(T*)super::_data, (uint)-1}; }

  auto write(T value) -> void {
    operator[](0) = value;
    super::_data++;
    super::_size--;
  }

  auto span(uint offset, uint length) const -> type {
    #ifdef DEBUG
    struct out_of_bounds {};
    if(offset + length >= super::_size) throw out_of_bounds{};
    #endif
    return {super::_data + offset, length};
  }

  //array_span<uint8_t> specializations
  template<typename U> auto writel(U value, uint size) -> void;
  template<typename U> auto writem(U value, uint size) -> void;
  template<typename U> auto writevn(U value, uint size) -> void;
  template<typename U> auto writevi(U value, uint size) -> void;
};

template<> inline auto array_span<uint8_t>::write(uint8_t value) -> void {
  operator[](0) = value;
  _data++;
  _size--;
}

template<> template<typename U> inline auto array_span<uint8_t>::writel(U value, uint size) -> void {
  for(uint byte : range(size)) write(value >> byte * 8);
}

template<> template<typename U> inline auto array_span<uint8_t>::writem(U value, uint size) -> void {
  for(uint byte : reverse(range(size))) write(value >> byte * 8);
}

template<> template<typename U> inline auto array_span<uint8_t>::writevn(U value, uint size) -> void {
  while(true) {
    auto byte = value & 0x7f;
    value >>= 7;
    if(value == 0) return write(0x80 | byte);
    write(byte);
    value--;
  }
}

template<> template<typename U> inline auto array_span<uint8_t>::writevi(U value, uint size) -> void {
  bool negate = value < 0;
  if(negate) value = ~value;
  value = value << 1 | negate;
  writevn(value);
}

template<typename T> struct array;

//usage: int x[256] => array<int[256]> x
template<typename T, uint Size> struct array<T[Size]> {
  array() = default;

  array(const std::initializer_list<T>& source) {
    uint index = 0;
    for(auto& value : source) {
      operator[](index++) = value;
    }
  }

  operator array_span<T>() {
    return {data(), size()};
  }

  operator array_view<T>() const {
    return {data(), size()};
  }

  alwaysinline auto operator[](uint index) -> T& {
    #ifdef DEBUG
    struct out_of_bounds {};
    if(index >= Size) throw out_of_bounds{};
    #endif
    return values[index];
  }

  alwaysinline auto operator[](uint index) const -> const T& {
    #ifdef DEBUG
    struct out_of_bounds {};
    if(index >= Size) throw out_of_bounds{};
    #endif
    return values[index];
  }

  alwaysinline auto operator()(uint index, const T& fallback = {}) const -> const T& {
    if(index >= Size) return fallback;
    return values[index];
  }

  auto fill(const T& fill = {}) -> array& {
    for(auto& value : values) value = fill;
    return *this;
  }

  auto data() -> T* { return values; }
  auto data() const -> const T* { return values; }
  auto size() const -> uint { return Size; }

  auto begin() -> T* { return &values[0]; }
  auto end() -> T* { return &values[Size]; }

  auto begin() const -> const T* { return &values[0]; }
  auto end() const -> const T* { return &values[Size]; }

private:
  T values[Size];
};

template<typename T, T... p> auto from_array(uint index) -> T {
  static const array<T[sizeof...(p)]> table{p...};
  struct out_of_bounds {};
  #if defined(DEBUG)
  if(index >= sizeof...(p)) throw out_of_bounds{};
  #endif
  return table[index];
}

template<int64_t... p> auto from_array(uint index) -> int64_t {
  static const array<int64_t[sizeof...(p)]> table{p...};
  struct out_of_bounds {};
  #if defined(DEBUG)
  if(index >= sizeof...(p)) throw out_of_bounds{};
  #endif
  return table[index];
}

}
