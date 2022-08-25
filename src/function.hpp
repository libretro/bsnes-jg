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

#pragma once

#include <utility>

template <typename T, unsigned B>
inline T signextend(const T x) {
  struct { T x:B; } s;
  return s.x = x;
}

template<typename T> struct bfunction;

template<typename R, typename... P> struct bfunction<R (P...)> {
  template<typename L> struct is_compatible {
    template<typename T> static const typename std::is_same<R, decltype(std::declval<T>().operator()(std::declval<P>()...))>::type exists(T*);
    static constexpr bool value = decltype(exists<L>(0))::value;
  };

  bfunction() {}
  template<typename C> bfunction(R (C::*_bfunction)(P...), C* object) { callback = new member<C>(_bfunction, object); }
  template<typename L, typename = typename std::enable_if<is_compatible<L>::value>::type> bfunction(const L& object) { callback = new lambda<L>(object); }
  ~bfunction() { if(callback) delete callback; }

  explicit operator bool() const { return callback; }
  R operator()(P... p) const { return (*callback)(std::forward<P>(p)...); }
  void reset() { if(callback) { delete callback; callback = nullptr; } }

  bfunction& operator=(const bfunction& source) {
    if(this != &source) {
      if(callback) { delete callback; callback = nullptr; }
      if(source.callback) callback = source.callback->copy();
    }
    return *this;
  }

private:
  struct container {
    virtual R operator()(P... p) const = 0;
    virtual container* copy() const = 0;
    virtual ~container() = default;
  };

  container* callback = nullptr;

  template<typename C> struct member : container {
    R (C::*bfunction)(P...);
    C* object;
    R operator()(P... p) const { return (object->*bfunction)(std::forward<P>(p)...); }
    container* copy() const { return new member(bfunction, object); }
    member(R (C::*_bfunction)(P...), C* obj) : bfunction(_bfunction), object(obj) {}
  };

  template<typename L> struct lambda : container {
    mutable L object;
    R operator()(P... p) const { return object(std::forward<P>(p)...); }
    container* copy() const { return new lambda(object); }
    lambda(const L& obj) : object(obj) {}
  };
};
