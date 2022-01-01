#pragma once

namespace nall {

template<typename T> struct iterator {
  iterator(T* self, uint64_t offset) : _self(self), _offset(offset) {}
  auto operator*() -> T& { return _self[_offset]; }
  auto operator!=(const iterator& source) const -> bool { return _offset != source._offset; }
  auto operator++() -> iterator& { return _offset++, *this; }
  auto offset() const -> uint64_t { return _offset; }

private:
  T* _self;
  uint64_t _offset;
};

template<typename T> struct iterator_const {
  iterator_const(const T* self, uint64_t offset) : _self(self), _offset(offset) {}
  auto operator*() -> const T& { return _self[_offset]; }
  auto operator!=(const iterator_const& source) const -> bool { return _offset != source._offset; }
  auto operator++() -> iterator_const& { return _offset++, *this; }
  auto offset() const -> uint64_t { return _offset; }

private:
  const T* _self;
  uint64_t _offset;
};

template<typename T> struct reverse_iterator {
  reverse_iterator(T* self, uint64_t offset) : _self(self), _offset(offset) {}
  auto operator*() -> T& { return _self[_offset]; }
  auto operator!=(const reverse_iterator& source) const -> bool { return _offset != source._offset; }
  auto operator++() -> reverse_iterator& { return _offset--, *this; }
  auto offset() const -> uint64_t { return _offset; }

private:
  T* _self;
  uint64_t _offset;
};

template<typename T> struct reverse_iterator_const {
  reverse_iterator_const(const T* self, uint64_t offset) : _self(self), _offset(offset) {}
  auto operator*() -> const T& { return _self[_offset]; }
  auto operator!=(const reverse_iterator_const& source) const -> bool { return _offset != source._offset; }
  auto operator++() -> reverse_iterator_const& { return _offset--, *this; }
  auto offset() const -> uint64_t { return _offset; }

private:
  const T* _self;
  uint64_t _offset;
};

template<typename T> auto rbegin(T& self) { return self.rbegin(); }
template<typename T> auto rend(T& self) { return self.rend(); }

template<typename T> struct reverse_wrapper {
  auto begin() { return rbegin(_self); }
  auto end() { return rend(_self); }

  auto begin() const { return rbegin(_self); }
  auto end() const { return rend(_self); }

  T _self;
};

template<typename T> auto reverse(T&& object) -> reverse_wrapper<T> {
  return {object};
}

struct range_t {
  struct iterator {
    iterator(int64_t position, int64_t step = 0) : position(position), step(step) {}
    auto operator*() const -> int64_t { return position; }
    auto operator!=(const iterator& source) const -> bool { return step > 0 ? position < source.position : position > source.position; }

// Workaround a hang when starting Tengai Makyo Zero
#if defined(__clang__)
#pragma clang optimize off
#endif

    auto operator++() -> iterator& { position += step; return *this; }

#if defined(__clang__)
#pragma clang optimize on
#endif

  private:
    int64_t position;
    const int64_t step;
  };

  struct reverse_iterator {
    reverse_iterator(int64_t position, int64_t step = 0) : position(position), step(step) {}
    auto operator*() const -> int64_t { return position; }
    auto operator!=(const reverse_iterator& source) const -> bool { return step > 0 ? position > source.position : position < source.position; }
    auto operator++() -> reverse_iterator& { position -= step; return *this; }

  private:
    int64_t position;
    const int64_t step;
  };

  auto begin() const -> iterator { return {origin, stride}; }
  auto end() const -> iterator { return {target}; }

  auto rbegin() const -> reverse_iterator { return {target - stride, stride}; }
  auto rend() const -> reverse_iterator { return {origin - stride}; }

  int64_t origin;
  int64_t target;
  int64_t stride;
};

inline auto range(int64_t size) {
  return range_t{0, size, 1};
}

inline auto range(int64_t offset, int64_t size) {
  return range_t{offset, size, 1};
}

}
