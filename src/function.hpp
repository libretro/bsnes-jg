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

template<typename T> struct bfunction;

template<typename R, typename... P> struct bfunction<auto (P...) -> R> {
  //value = true if auto L::operator()(P...) -> R exists
  template<typename L> struct is_compatible {
    template<typename T> static auto exists(T*) -> const typename std::is_same<R, decltype(std::declval<T>().operator()(std::declval<P>()...))>::type;
    static constexpr bool value = decltype(exists<L>(0))::value;
  };

  bfunction() {}
  template<typename C> bfunction(auto (C::*bfunction)(P...) -> R, C* object) { callback = new member<C>(bfunction, object); }
  template<typename L, typename = std::enable_if_t<is_compatible<L>::value>> bfunction(const L& object) { callback = new lambda<L>(object); }
  ~bfunction() { if(callback) delete callback; }

  explicit operator bool() const { return callback; }
  auto operator()(P... p) const -> R { return (*callback)(std::forward<P>(p)...); }
  auto reset() -> void { if(callback) { delete callback; callback = nullptr; } }

  auto operator=(const bfunction& source) -> bfunction& {
    if(this != &source) {
      if(callback) { delete callback; callback = nullptr; }
      if(source.callback) callback = source.callback->copy();
    }
    return *this;
  }

private:
  struct container {
    virtual auto operator()(P... p) const -> R = 0;
    virtual auto copy() const -> container* = 0;
    virtual ~container() = default;
  };

  container* callback = nullptr;

  template<typename C> struct member : container {
    auto (C::*bfunction)(P...) -> R;
    C* object;
    auto operator()(P... p) const -> R { return (object->*bfunction)(std::forward<P>(p)...); }
    auto copy() const -> container* { return new member(bfunction, object); }
    member(auto (C::*bfunction)(P...) -> R, C* object) : bfunction(bfunction), object(object) {}
  };

  template<typename L> struct lambda : container {
    mutable L object;
    auto operator()(P... p) const -> R { return object(std::forward<P>(p)...); }
    auto copy() const -> container* { return new lambda(object); }
    lambda(const L& object) : object(object) {}
  };
};
