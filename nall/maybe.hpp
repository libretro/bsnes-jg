#pragma once

#include <assert.h>

namespace nall {

struct nothing_t {};
static nothing_t nothing;

template<typename T>
struct maybe {
  inline maybe() {}
  inline maybe(nothing_t) {}
  inline maybe(const T& source) { operator=(source); }

  inline auto operator=(const T& source) -> maybe& { reset(); _valid = true; new(&_value.t) T(source); return *this; }

  inline auto operator=(const maybe& source) -> maybe& {
    if(this == &source) return *this;
    reset();
    if(_valid = source._valid) new(&_value.t) T(source.get());
    return *this;
  }

  inline explicit operator bool() const { return _valid; }
  inline auto reset() -> void { if(_valid) { _value.t.~T(); _valid = false; } }
  inline auto data() -> T* { return _valid ? &_value.t : nullptr; }
  inline auto get() -> T& { assert(_valid); return _value.t; }
  inline auto get() const -> const T& { return ((maybe*)this)->get(); }
  inline auto operator->() -> T* { return data(); }
  inline auto operator*() -> T& { return get(); }
  inline auto operator()() const -> const T& { return get(); }

private:
  union U {
    T t;
    U() {}
    ~U() {}
  } _value;
  bool _valid = false;
};

template<typename T>
struct maybe<T&> {
  inline maybe() : _value(nullptr) {}
  inline maybe(const T& source) : _value((T*)&source) {}

  inline explicit operator bool() const { return _value; }
  inline auto data() -> T* { return _value; }

  inline auto operator->() -> T* { return data(); }

private:
  T* _value;
};

}
