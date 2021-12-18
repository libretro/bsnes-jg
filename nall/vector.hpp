#pragma once

namespace nall {

template<typename T>
struct vector_base {
  //core.hpp
  vector_base() = default;
  vector_base(const std::initializer_list<T>& values);
  vector_base(const vector_base& source);
  vector_base(vector_base&& source);
  ~vector_base();

  explicit operator bool() const;
  operator array_span<T>();
  template<typename Cast = T> auto size() const -> uint64_t;
  template<typename Cast = T> auto data() -> Cast*;
  template<typename Cast = T> auto data() const -> const Cast*;

  //assign.hpp
  auto operator=(const vector_base& source) -> vector_base&;
  auto operator=(vector_base&& source) -> vector_base&;

  //memory.hpp
  auto reset() -> void;

  auto reserveLeft(uint64_t capacity) -> bool;
  auto reserveRight(uint64_t capacity) -> bool;
  auto reserve(uint64_t capacity) -> bool { return reserveRight(capacity); }

  auto reallocateRight(uint64_t size) -> bool;
  auto reallocate(uint64_t size) -> bool { return reallocateRight(size); }

  auto resizeLeft(uint64_t size, const T& value = T()) -> bool;
  auto resizeRight(uint64_t size, const T& value = T()) -> bool;
  auto resize(uint64_t size, const T& value = T()) -> bool { return resizeRight(size, value); }

  //access.hpp
  inline auto operator[](uint64_t offset) -> T&;
  inline auto operator[](uint64_t offset) const -> const T&;

  inline auto operator()(uint64_t offset) -> T&;
  inline auto operator()(uint64_t offset, const T& value) const -> const T&;

  inline auto right() -> T&;

  //modify.hpp
  auto prepend(const T& value) -> void;

  auto append(const T& value) -> void;
  auto append(T&& value) -> void;
  auto append(vector_base&& values) -> void;

  auto insert(uint64_t offset, const T& value) -> void;

  auto removeLeft(uint64_t length = 1) -> void;
  auto remove(uint64_t offset, uint64_t length = 1) -> void;

  auto takeLeft() -> T;
  auto takeRight() -> T;
  auto take(uint64_t offset) -> T;

  //iterator.hpp
  auto begin() -> iterator<T> { return {data(), 0}; }
  auto end() -> iterator<T> { return {data(), size()}; }

  auto begin() const -> iterator_const<T> { return {data(), 0}; }
  auto end() const -> iterator_const<T> { return {data(), size()}; }

protected:
  T* _pool = nullptr;   //pointer to first initialized element in pool
  uint64_t _size = 0;   //number of initialized elements in pool
  uint64_t _left = 0;   //number of allocated elements free on the left of pool
  uint64_t _right = 0;  //number of allocated elements free on the right of pool
};

}

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

template<typename T> auto vector_base<T>::operator=(const vector_base<T>& source) -> vector_base<T>& {
  if(this == &source) return *this;
  _pool = memory::allocate<T>(source._size);
  _size = source._size;
  _left = 0;
  _right = 0;
  for(uint64_t n : range(_size)) new(_pool + n) T(source._pool[n]);
  return *this;
}

template<typename T> auto vector_base<T>::operator=(vector_base<T>&& source) -> vector_base<T>& {
  if(this == &source) return *this;
  _pool = source._pool;
  _size = source._size;
  _left = source._left;
  _right = source._right;
  source._pool = nullptr;
  source._size = 0;
  source._left = 0;
  source._right = 0;
  return *this;
}

//nall::vector_base acts internally as a deque (double-ended queue)
//it does this because it's essentially free to do so, only costing an extra integer in sizeof(vector_base)

template<typename T> auto vector_base<T>::reset() -> void {
  if(!_pool) return;

  for(uint64_t n : range(_size)) _pool[n].~T();
  memory::free(_pool - _left);

  _pool = nullptr;
  _size = 0;
  _left = 0;
  _right = 0;
}

//reserve allocates memory for objects, but does not initialize them
//when the vector_base desired size is known, this can be used to avoid growing the capacity dynamically
//reserve will not actually shrink the capacity, only expand it
//shrinking the capacity would destroy objects, and break amortized growth with reallocate and resize

template<typename T> auto vector_base<T>::reserveLeft(uint64_t capacity) -> bool {
  if(_size + _left >= capacity) return false;

  uint64_t left = nall::round(capacity);
  auto pool = memory::allocate<T>(left + _right) + (left - _size);
  for(uint64_t n : range(_size)) new(pool + n) T(std::move(_pool[n]));
  memory::free(_pool - _left);

  _pool = pool;
  _left = left - _size;

  return true;
}

template<typename T> auto vector_base<T>::reserveRight(uint64_t capacity) -> bool {
  if(_size + _right >= capacity) return false;

  uint64_t right = nall::round(capacity);
  auto pool = memory::allocate<T>(_left + right) + _left;
  for(uint64_t n : range(_size)) new(pool + n) T(std::move(_pool[n]));
  memory::free(_pool - _left);

  _pool = pool;
  _right = right - _size;

  return true;
}

//resize is meant for non-POD types, and will properly construct objects

template<typename T> auto vector_base<T>::resizeLeft(uint64_t size, const T& value) -> bool {
  if(size < _size) {  //shrink
    for(uint64_t n : range(_size - size)) _pool[n].~T();
    _pool += _size - size;
    _left += _size - size;
    _size = size;
    return true;
  }
  if(size > _size) {  //grow
    reserveLeft(size);
    _pool -= size - _size;
    for(uint64_t n : nall::reverse(range(size - _size))) new(_pool + n) T(value);
    _left -= size - _size;
    _size = size;
    return true;
  }
  return false;
}

template<typename T> auto vector_base<T>::resizeRight(uint64_t size, const T& value) -> bool {
  if(size < _size) {  //shrink
    for(uint64_t n : range(size, _size)) _pool[n].~T();
    _right += _size - size;
    _size = size;
    return true;
  }
  if(size > _size) {  //grow
    reserveRight(size);
    for(uint64_t n : range(_size, size)) new(_pool + n) T(value);
    _right -= size - _size;
    _size = size;
    return true;
  }
  return false;
}

template<typename T> auto vector_base<T>::operator[](uint64_t offset) -> T& {
  return _pool[offset];
}

template<typename T> auto vector_base<T>::operator[](uint64_t offset) const -> const T& {
  return _pool[offset];
}

template<typename T> auto vector_base<T>::operator()(uint64_t offset) -> T& {
  while(offset >= size()) append(T());
  return _pool[offset];
}

template<typename T> auto vector_base<T>::operator()(uint64_t offset, const T& value) const -> const T& {
  if(offset >= size()) return value;
  return _pool[offset];
}

template<typename T> auto vector_base<T>::right() -> T& {
  return _pool[_size - 1];
}

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

template<typename T> struct vector : vector_base<T> {
  using vector_base<T>::vector_base;
};

}
