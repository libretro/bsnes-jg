#pragma once

namespace nall {

template<typename T> vector_base<T>::vector_base(const std::initializer_list<T>& values) {
  reserveRight(values.size());
  for(auto& value : values) append(value);
}

template<typename T> vector_base<T>::vector_base(const vector_base<T>& source) {
  operator=(source);
}

template<typename T> vector_base<T>::vector_base(vector_base<T>&& source) {
  operator=(std::move(source));
}

template<typename T> vector_base<T>::~vector_base() {
  reset();
}

template<typename T> vector_base<T>::operator bool() const {
  return _size;
}

template<typename T> vector_base<T>::operator array_span<T>() {
  return {data(), size()};
}

template<typename T> template<typename Cast> auto vector_base<T>::size() const -> uint64_t {
  return _size * sizeof(T) / sizeof(Cast);
}

template<typename T> template<typename Cast> auto vector_base<T>::data() -> Cast* {
  return (Cast*)_pool;
}

template<typename T> template<typename Cast> auto vector_base<T>::data() const -> const Cast* {
  return (const Cast*)_pool;
}

}
