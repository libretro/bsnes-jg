#pragma once

//only allocators may access _data or modify _size and _capacity
//all other functions must use data(), size(), capacity()

namespace nall {

auto string::operator[](unsigned position) const -> const char& {
  #ifdef DEBUG
  struct out_of_bounds {};
  if(position >= size() + 1) throw out_of_bounds{};
  #endif
  return data()[position];
}

template<typename T, typename... P> auto string::prepend(const T& value, P&&... p) -> string& {
  if constexpr(sizeof...(p)) prepend(std::forward<P>(p)...);
  return _prepend(make_string(value));
}

template<typename T> auto string::_prepend(const stringify<T>& source) -> string& {
  resize(source.size() + size());
  memory::move(get() + source.size(), get(), size() - source.size());
  memory::copy(get(), source.data(), source.size());
  return *this;
}

template<typename T, typename... P> auto string::append(const T& value, P&&... p) -> string& {
  _append(make_string(value));
  if constexpr(sizeof...(p) > 0) append(std::forward<P>(p)...);
  return *this;
}

template<typename T> auto string::_append(const stringify<T>& source) -> string& {
  resize(size() + source.size());
  memory::copy(get() + size() - source.size(), source.data(), source.size());
  return *this;
}

auto string::length() const -> unsigned {
  return strlen(data());
}

}
