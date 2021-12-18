#pragma once

namespace nall {

template<typename T> struct array_view {
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

  inline auto operator++() -> array_view& { _data++; _size--; return *this; }
  inline auto operator--() -> array_view& { _data--; _size++; return *this; }

  inline auto operator++(int) -> array_view { auto copy = *this; ++(*this); return copy; }
  inline auto operator--(int) -> array_view { auto copy = *this; --(*this); return copy; }

  inline auto operator-=(int distance) -> array_view& { _data -= distance; _size += distance; return *this; }
  inline auto operator+=(int distance) -> array_view& { _data += distance; _size -= distance; return *this; }

  inline auto operator[](unsigned index) const -> const T& {
    #ifdef DEBUG
    struct out_of_bounds {};
    if(index >= _size) throw out_of_bounds{};
    #endif
    return _data[index];
  }

  inline auto operator()(unsigned index, const T& fallback = {}) const -> T {
    if(index >= _size) return fallback;
    return _data[index];
  }

  template<typename U = T> inline auto data() const -> const U* { return (const U*)_data; }
  template<typename U = T> inline auto size() const -> uint64_t { return _size * sizeof(T) / sizeof(U); }

  inline auto begin() const -> iterator_const<T> { return {_data, (unsigned)0}; }
  inline auto end() const -> iterator_const<T> { return {_data, (unsigned)_size}; }

  inline auto rbegin() const -> reverse_iterator_const<T> { return {_data, (unsigned)_size - 1}; }
  inline auto rend() const -> reverse_iterator_const<T> { return {_data, (unsigned)-1}; }

  auto read() -> T {
    auto value = operator[](0);
    _data++;
    _size--;
    return value;
  }

  auto view(unsigned offset, unsigned length) const -> array_view {
    #ifdef DEBUG
    struct out_of_bounds {};
    if(offset + length >= _size) throw out_of_bounds{};
    #endif
    return {_data + offset, length};
  }

protected:
  const T* _data;
  int _size;
};

template<typename T> struct array_span : array_view<T> {
  inline array_span() {
    array_view<T>::_data = nullptr;
    array_view<T>::_size = 0;
  }

  inline array_span(std::nullptr_t) {
    array_view<T>::_data = nullptr;
    array_view<T>::_size = 0;
  }

  inline array_span(void* data, uint64_t size) {
    array_view<T>::_data = (T*)data;
    array_view<T>::_size = (int)size;
  }

  inline operator T*() { return (T*)array_view<T>::operator const T*(); }

  inline auto operator[](unsigned index) -> T& { return (T&)array_view<T>::operator[](index); }

  template<typename U = T> inline auto data() -> U* { return (U*)array_view<T>::_data; }

  inline auto begin() -> iterator<T> { return {(T*)array_view<T>::_data, (unsigned)0}; }
  inline auto end() -> iterator<T> { return {(T*)array_view<T>::_data, (unsigned)array_view<T>::_size}; }

  inline auto rbegin() -> reverse_iterator<T> { return {(T*)array_view<T>::_data, (unsigned)array_view<T>::_size - 1}; }
  inline auto rend() -> reverse_iterator<T> { return {(T*)array_view<T>::_data, (unsigned)-1}; }

  auto write(T value) -> void {
    operator[](0) = value;
    array_view<T>::_data++;
    array_view<T>::_size--;
  }

  auto span(unsigned offset, unsigned length) const -> array_span {
    #ifdef DEBUG
    struct out_of_bounds {};
    if(offset + length >= array_view<T>::_size) throw out_of_bounds{};
    #endif
    return {array_view<T>::_data + offset, length};
  }

  //array_span<uint8_t> specializations
  template<typename U> auto writel(U value, unsigned size) -> void;
};

template<> inline auto array_span<uint8_t>::write(uint8_t value) -> void {
  operator[](0) = value;
  _data++;
  _size--;
}

template<> template<typename U> inline auto array_span<uint8_t>::writel(U value, unsigned size) -> void {
  for(unsigned byte : range(size)) write(value >> byte * 8);
}

template<typename T> struct array;

//usage: int x[256] => array<int[256]> x
template<typename T, unsigned Size> struct array<T[Size]> {
  array() = default;

  array(const std::initializer_list<T>& source) {
    unsigned index = 0;
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

  inline auto operator[](unsigned index) -> T& {
    #ifdef DEBUG
    struct out_of_bounds {};
    if(index >= Size) throw out_of_bounds{};
    #endif
    return values[index];
  }

  inline auto operator[](unsigned index) const -> const T& {
    #ifdef DEBUG
    struct out_of_bounds {};
    if(index >= Size) throw out_of_bounds{};
    #endif
    return values[index];
  }

  inline auto operator()(unsigned index, const T& fallback = {}) const -> const T& {
    if(index >= Size) return fallback;
    return values[index];
  }

  auto fill(const T& fill = {}) -> array& {
    for(auto& value : values) value = fill;
    return *this;
  }

  auto data() -> T* { return values; }
  auto data() const -> const T* { return values; }
  auto size() const -> unsigned { return Size; }

  auto begin() -> T* { return &values[0]; }
  auto end() -> T* { return &values[Size]; }

  auto begin() const -> const T* { return &values[0]; }
  auto end() const -> const T* { return &values[Size]; }

private:
  T values[Size];
};

template<typename T, T... p> auto from_array(unsigned index) -> T {
  static const array<T[sizeof...(p)]> table{p...};
  struct out_of_bounds {};
  #if defined(DEBUG)
  if(index >= sizeof...(p)) throw out_of_bounds{};
  #endif
  return table[index];
}

template<int64_t... p> auto from_array(unsigned index) -> int64_t {
  static const array<int64_t[sizeof...(p)]> table{p...};
  struct out_of_bounds {};
  #if defined(DEBUG)
  if(index >= sizeof...(p)) throw out_of_bounds{};
  #endif
  return table[index];
}

}
