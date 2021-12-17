#pragma once

namespace nall {

template<typename T> auto vector_base<T>::append(const T& value) -> void {
  reserveRight(size() + 1);
  new(_pool + _size) T(value);
  _right--;
  _size++;
}

template<typename T> auto vector_base<T>::append(T&& value) -> void {
  reserveRight(size() + 1);
  new(_pool + _size) T(std::move(value));
  _right--;
  _size++;
}

template<typename T> auto vector_base<T>::append(vector_base<T>&& values) -> void {
  reserveRight(size() + values.size());
  for(uint64_t n : range(values.size())) new(_pool + _size + n) T(std::move(values[n]));
  _right -= values.size();
  _size += values.size();
}

template<typename T> auto vector_base<T>::removeLeft(uint64_t length) -> void {
  if(length > size()) length = size();
  resizeLeft(size() - length);
}

template<typename T> auto vector_base<T>::takeLeft() -> T {
  T value = std::move(_pool[0]);
  removeLeft();
  return value;
}

template<typename T> auto vector_base<T>::take(uint64_t offset) -> T {
  if(offset == 0) return takeLeft();
  if(offset == size() - 1) return takeRight();

  T value = std::move(_pool[offset]);
  remove(offset);
  return value;
}

}
