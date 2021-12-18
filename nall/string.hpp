#pragma once

#include <string>
#include <limits.h>

namespace nall {

constexpr inline auto toBinary_(const char* s, uintmax sum = 0) -> uintmax {
  return (
    *s == '0' || *s == '1' ? toBinary_(s + 1, (sum << 1) | *s - '0') :
    *s == '\'' ? toBinary_(s + 1, sum) :
    sum
  );
}

constexpr inline auto toOctal_(const char* s, uintmax sum = 0) -> uintmax {
  return (
    *s >= '0' && *s <= '7' ? toOctal_(s + 1, (sum << 3) | *s - '0') :
    *s == '\'' ? toOctal_(s + 1, sum) :
    sum
  );
}

constexpr inline auto toDecimal_(const char* s, uintmax sum = 0) -> uintmax {
  return (
    *s >= '0' && *s <= '9' ? toDecimal_(s + 1, (sum * 10) + *s - '0') :
    *s == '\'' ? toDecimal_(s + 1, sum) :
    sum
  );
}

constexpr inline auto toHex_(const char* s, uintmax sum = 0) -> uintmax {
  return (
    *s >= 'A' && *s <= 'F' ? toHex_(s + 1, (sum << 4) | *s - 'A' + 10) :
    *s >= 'a' && *s <= 'f' ? toHex_(s + 1, (sum << 4) | *s - 'a' + 10) :
    *s >= '0' && *s <= '9' ? toHex_(s + 1, (sum << 4) | *s - '0') :
    *s == '\'' ? toHex_(s + 1, sum) :
    sum
  );
}

constexpr inline auto toHex(const char* s) -> uintmax {
  return (
    *s == '0' && (*(s + 1) == 'X' || *(s + 1) == 'x') ? toHex_(s + 2) :
    *s == '$' ? toHex_(s + 1) : toHex_(s)
  );
}

constexpr inline auto toNatural(const char* s) -> uintmax {
  return (
    *s == '0' && (*(s + 1) == 'B' || *(s + 1) == 'b') ? toBinary_(s + 2) :
    *s == '0' && (*(s + 1) == 'O' || *(s + 1) == 'o') ? toOctal_(s + 2) :
    *s == '0' && (*(s + 1) == 'X' || *(s + 1) == 'x') ? toHex_(s + 2) :
    *s == '%' ? toBinary_(s + 1) : *s == '$' ? toHex_(s + 1) : toDecimal_(s)
  );
}

constexpr inline auto toInteger(const char* s) -> intmax {
  return (
    *s == '+' ? +toNatural(s + 1) : *s == '-' ? -toNatural(s + 1) : toNatural(s)
  );
}

inline auto toReal(const char* s) -> double {
  return atof(s);
}

struct string;
struct string_format;

struct string_view {
  //view.hpp
  inline string_view();
  inline string_view(const string_view& source);
  inline string_view(string_view&& source);
  inline string_view(const char* data);
  inline string_view(const char* data, unsigned size);
  inline string_view(const string& source);
  template<typename... P> inline string_view(P&&... p);
  inline ~string_view();

  inline auto operator=(const string_view& source) -> string_view&;
  inline auto operator=(string_view&& source) -> string_view&;

  inline explicit operator bool() const;
  inline operator const char*() const;
  inline auto data() const -> const char*;
  inline auto size() const -> unsigned;

  inline auto begin() const { return &_data[0]; }
  inline auto end() const { return &_data[size()]; }

protected:
  string* _string;
  const char* _data;
  mutable int _size;
};

//cast.hpp
template<typename T> struct stringify;

//format.hpp
inline auto hex(uintmax value, long precision = 0, char padchar = '0') -> string;

//utility.hpp
inline auto slice(string_view self, int offset = 0, int length = -1) -> string;
template<typename T> inline auto fromInteger(char* result, T value) -> char*;
template<typename T> inline auto fromNatural(char* result, T value) -> char*;

struct string {

protected:
  //adaptive (SSO + COW) is by far the best choice, the others exist solely to:
  //1) demonstrate the performance benefit of combining SSO + COW
  //2) rule out allocator bugs by trying different allocators when needed

  enum : unsigned { SSO = 24 };
  union {
    struct {  //copy-on-write
      char* _data;
      unsigned* _refs;
    };
    struct {  //small-string-optimization
      char _text[SSO];
    };
  };
  inline auto _allocate() -> void;
  inline auto _copy() -> void;
  inline auto _resize() -> void;

  unsigned _capacity;
  unsigned _size;

public:
  inline string();
  inline string(string& source) : string() { operator=(source); }
  inline string(const string& source) : string() { operator=(source); }
  inline string(string&& source) : string() { operator=(std::move(source)); }
  template<typename T = char> inline auto get() -> T*;
  template<typename T = char> inline auto data() const -> const T*;
  template<typename T = char> auto size() const -> unsigned { return _size / sizeof(T); }
  template<typename T = char> auto capacity() const -> unsigned { return _capacity / sizeof(T); }
  inline auto reset() -> string&;
  inline auto reserve(unsigned) -> string&;
  inline auto resize(unsigned) -> string&;
  inline auto operator=(const string&) -> string&;
  inline auto operator=(string&&) -> string&;

  template<typename T, typename... P> string(T&& s, P&&... p) : string() {
    append(std::forward<T>(s), std::forward<P>(p)...);
  }
  ~string() { reset(); }

  explicit operator bool() const { return _size; }
  operator const char*() const { return (const char*)data(); }
  operator array_span<char>() { return {(char*)get(), size()}; }
  operator array_view<char>() const { return {(const char*)data(), size()}; }
  operator array_span<uint8_t>() { return {(uint8_t*)get(), size()}; }
  operator array_view<uint8_t>() const { return {(const uint8_t*)data(), size()}; }

  auto operator==(const string& source) const -> bool {
    return size() == source.size() && memory::compare(data(), source.data(), size()) == 0;
  }
  auto operator!=(const string& source) const -> bool {
    return size() != source.size() || memory::compare(data(), source.data(), size()) != 0;
  }

  auto operator==(const char* source) const -> bool { return strcmp(data(), source) == 0; }
  auto operator!=(const char* source) const -> bool { return strcmp(data(), source) != 0; }

  auto operator==(string_view source) const -> bool { return compare(source) == 0; }
  auto operator!=(string_view source) const -> bool { return compare(source) != 0; }
  auto operator< (string_view source) const -> bool { return compare(source) <  0; }
  auto operator<=(string_view source) const -> bool { return compare(source) <= 0; }
  auto operator> (string_view source) const -> bool { return compare(source) >  0; }
  auto operator>=(string_view source) const -> bool { return compare(source) >= 0; }

  auto begin() -> char* { return &get()[0]; }
  auto end() -> char* { return &get()[size()]; }
  auto begin() const -> const char* { return &data()[0]; }
  auto end() const -> const char* { return &data()[size()]; }

  //atoi.hpp
  inline auto integer() const -> intmax;
  inline auto natural() const -> uintmax;
  inline auto hex() const -> uintmax;
  inline auto real() const -> double;

  //core.hpp
  inline auto operator[](unsigned) const -> const char&;
  template<typename T, typename... P> inline auto append(const T&, P&&...) -> string&;
  template<typename T> inline auto _append(const stringify<T>&) -> string&;
  inline auto length() const -> unsigned;

  //find.hpp
  template<bool, bool> inline auto _find(int, string_view) const -> maybe<unsigned>;

  inline auto find(string_view source) const -> maybe<unsigned>;

  //compare.hpp
  template<bool> inline static auto _compare(const char*, unsigned, const char*, unsigned) -> int;

  inline auto compare(string_view source) const -> int;
  inline auto beginsWith(string_view source) const -> bool;
  inline auto endsWith(string_view source) const -> bool;

  //convert.hpp
  inline auto downcase() -> string&;
  inline auto transform(string_view from, string_view to) -> string&;

  //match.hpp
  inline auto match(string_view source) const -> bool;

  //replace.hpp
  template<bool, bool> inline auto _replace(string_view, string_view, long) -> string&;
  inline auto replace(string_view from, string_view to, long limit = LONG_MAX) -> string&;

  //split.hpp
  inline auto split(string_view key, long limit = LONG_MAX) const -> vector<string>;

  //trim.hpp
  inline auto trim(string_view lhs, string_view rhs, long limit = LONG_MAX) -> string&;
  inline auto trimLeft(string_view lhs, long limit = LONG_MAX) -> string&;
  inline auto trimRight(string_view rhs, long limit = LONG_MAX) -> string&;

  inline auto itrim(string_view lhs, string_view rhs, long limit = LONG_MAX) -> string&;

  inline auto strip() -> string&;
  inline auto stripLeft() -> string&;
  inline auto stripRight() -> string&;

  //utf8.hpp
  inline auto characters(int offset = 0, int length = -1) const -> unsigned;

  //utility.hpp
  inline static auto read(string_view filename) -> string;
  inline static auto repeat(string_view pattern, unsigned times) -> string;
  inline auto fill(char fill = ' ') -> string&;
  inline auto hash() const -> unsigned;
  inline auto remove(unsigned offset, unsigned length) -> string&;
  inline auto reverse() -> string&;
  inline auto size(int length, char fill = ' ') -> string&;
  inline auto slice(int offset = 0, int length = -1) const -> string;
};

template<> struct vector<string> : vector_base<string> {
  vector(const vector& source) { vector_base::operator=(source); }
  vector(vector& source) { vector_base::operator=(source); }
  vector(vector&& source) { vector_base::operator=(std::move(source)); }
  template<typename... P> vector(P&&... p) { append(std::forward<P>(p)...); }

  inline auto operator=(const vector& source) -> vector<string>& { return vector_base::operator=(source), *this; }
  inline auto operator=(vector& source) -> vector<string>& { return vector_base::operator=(source), *this; }
  inline auto operator=(vector&& source) -> vector<string>& { return vector_base::operator=(std::move(source)), *this; }

  //vector.hpp
  template<typename... P> inline auto append(const string&, P&&...) -> vector<string>&;
  inline auto append() -> vector<string>&;
  inline auto merge(string_view separator) const -> string;

  //split.hpp
  template<bool, bool> inline auto _split(string_view, string_view, long) -> vector<string>&;
};

struct string_format : vector<string> {
  template<typename... P> string_format(P&&... p) { reserve(sizeof...(p)); append(std::forward<P>(p)...); }
  template<typename T, typename... P> inline auto append(const T&, P&&... p) -> string_format&;
  inline auto append() -> string_format&;
};

inline auto operator"" _s(const char* value, std::size_t) -> string { return {value}; }

struct string_pascal {
  string_pascal(const char* text = nullptr) {
    if(text && *text) {
      unsigned size = strlen(text);
      _data = memory::allocate<char>(sizeof(unsigned) + size + 1);
      ((unsigned*)_data)[0] = size;
      memory::copy(_data + sizeof(unsigned), text, size);
      _data[sizeof(unsigned) + size] = 0;
    }
  }

  string_pascal(const string& text) {
    if(text.size()) {
      _data = memory::allocate<char>(sizeof(unsigned) + text.size() + 1);
      ((unsigned*)_data)[0] = text.size();
      memory::copy(_data + sizeof(unsigned), text.data(), text.size());
      _data[sizeof(unsigned) + text.size()] = 0;
    }
  }

  string_pascal(const string_pascal& source) { operator=(source); }
  string_pascal(string_pascal&& source) { operator=(std::move(source)); }

  ~string_pascal() {
    if(_data) memory::free(_data);
  }

  explicit operator bool() const { return _data; }
  operator const char*() const { return _data ? _data + sizeof(unsigned) : nullptr; }
  operator string() const { return _data ? string{_data + sizeof(unsigned)} : ""; }

  auto operator=(const string_pascal& source) -> string_pascal& {
    if(this == &source) return *this;
    if(_data) { memory::free(_data); _data = nullptr; }
    if(source._data) {
      unsigned size = source.size();
      _data = memory::allocate<char>(sizeof(unsigned) + size);
      memory::copy(_data, source._data, sizeof(unsigned) + size);
    }
    return *this;
  }

  auto operator=(string_pascal&& source) -> string_pascal& {
    if(this == &source) return *this;
    if(_data) memory::free(_data);
    _data = source._data;
    source._data = nullptr;
    return *this;
  }

  auto operator==(string_view source) const -> bool {
    return size() == source.size() && memory::compare(data(), source.data(), size()) == 0;
  }

  auto operator!=(string_view source) const -> bool {
    return size() != source.size() || memory::compare(data(), source.data(), size()) != 0;
  }

  auto data() const -> char* {
    if(!_data) return nullptr;
    return _data + sizeof(unsigned);
  }

  auto size() const -> unsigned {
    if(!_data) return 0;
    return ((unsigned*)_data)[0];
  }

protected:
  char* _data = nullptr;
};

}

namespace nall {

/*****
  adaptive allocator
  sizeof(string) == SSO + 8

  aggressively tries to avoid heap allocations
  small strings are stored on the stack
  large strings are shared via copy-on-write

  SSO alone is very slow on large strings due to copying
  SSO alone is very slightly faster than this allocator on small strings

  COW alone is very slow on small strings due to heap allocations
  COW alone is very slightly faster than this allocator on large strings

  adaptive is thus very fast for all string sizes
*****/

string::string() : _data(nullptr), _capacity(SSO - 1), _size(0) {
}

template<typename T>
auto string::get() -> T* {
  if(_capacity < SSO) return (T*)_text;
  if(*_refs > 1) _copy();
  return (T*)_data;
}

template<typename T>
auto string::data() const -> const T* {
  if(_capacity < SSO) return (const T*)_text;
  return (const T*)_data;
}

auto string::reset() -> string& {
  if(_capacity >= SSO && !--*_refs) memory::free(_data);
  _data = nullptr;
  _capacity = SSO - 1;
  _size = 0;
  return *this;
}

auto string::reserve(unsigned capacity) -> string& {
  if(capacity <= _capacity) return *this;
  capacity = nall::round(capacity + 1) - 1;
  if(_capacity < SSO) {
    _capacity = capacity;
    _allocate();
  } else if(*_refs > 1) {
    _capacity = capacity;
    _copy();
  } else {
    _capacity = capacity;
    _resize();
  }
  return *this;
}

auto string::resize(unsigned size) -> string& {
  reserve(size);
  get()[_size = size] = 0;
  return *this;
}

auto string::operator=(const string& source) -> string& {
  if(&source == this) return *this;
  reset();
  if(source._capacity >= SSO) {
    _data = source._data;
    _refs = source._refs;
    _capacity = source._capacity;
    _size = source._size;
    ++*_refs;
  } else {
    memory::copy(_text, source._text, SSO);
    _capacity = source._capacity;
    _size = source._size;
  }
  return *this;
}

auto string::operator=(string&& source) -> string& {
  if(&source == this) return *this;
  reset();
  memory::copy(this, &source, sizeof(string));
  source._data = nullptr;
  source._capacity = SSO - 1;
  source._size = 0;
  return *this;
}

//SSO -> COW
auto string::_allocate() -> void {
  char _temp[SSO];
  memory::copy(_temp, _text, SSO);
  _data = memory::allocate<char>(_capacity + 1 + sizeof(unsigned));
  memory::copy(_data, _temp, SSO);
  _refs = (unsigned*)(_data + _capacity + 1);  //always aligned by 32 via reserve()
  *_refs = 1;
}

//COW -> Unique
auto string::_copy() -> void {
  auto _temp = memory::allocate<char>(_capacity + 1 + sizeof(unsigned));
  memory::copy(_temp, _data, _size = std::min(_capacity, _size));
  _temp[_size] = 0;
  --*_refs;
  _data = _temp;
  _refs = (unsigned*)(_data + _capacity + 1);
  *_refs = 1;
}

//COW -> Resize
auto string::_resize() -> void {
  _data = memory::resize<char>(_data, _capacity + 1 + sizeof(unsigned));
  _refs = (unsigned*)(_data + _capacity + 1);
  *_refs = 1;
}

auto string::integer() const -> intmax {
  return toInteger(data());
}

auto string::natural() const -> uintmax {
  return toNatural(data());
}

auto string::hex() const -> uintmax {
  return toHex(data());
}

auto string::real() const -> double {
  return toReal(data());
}

//convert any (supported) type to a const char* without constructing a new nall::string
//this is used inside string{...} to build nall::string values

//booleans

template<> struct stringify<bool> {
  stringify(bool value) : _value(value) {}
  auto data() const -> const char* { return _value ? "true" : "false"; }
  auto size() const -> unsigned { return _value ? 4 : 5; }
  bool _value;
};

//characters

template<> struct stringify<char> {
  stringify(char source) { _data[0] = source; _data[1] = 0; }
  auto data() const -> const char* { return _data; }
  auto size() const -> unsigned { return 1; }
  char _data[2];
};

//signed integers

template<> struct stringify<signed int> {
  stringify(signed int source) { fromInteger(_data, source); }
  auto data() const -> const char* { return _data; }
  auto size() const -> unsigned { return strlen(_data); }
  char _data[2 + sizeof(signed int) * 3];
};

//unsigned integers

template<> struct stringify<unsigned int> {
  stringify(unsigned int source) { fromNatural(_data, source); }
  auto data() const -> const char* { return _data; }
  auto size() const -> unsigned { return strlen(_data); }
  char _data[1 + sizeof(unsigned int) * 3];
};

template<unsigned Bits> struct stringify<Natural<Bits>> {
  stringify(Natural<Bits> source) { fromNatural(_data, source); }
  auto data() const -> const char* { return _data; }
  auto size() const -> unsigned { return strlen(_data); }
  char _data[1 + sizeof(uint64_t) * 3];
};

//char arrays

template<> struct stringify<char*> {
  stringify(char* source) : _data(source ? source : "") {}
  auto data() const -> const char* { return _data; }
  auto size() const -> unsigned { return strlen(_data); }
  const char* _data;
};

template<> struct stringify<const char*> {
  stringify(const char* source) : _data(source ? source : "") {}
  auto data() const -> const char* { return _data; }
  auto size() const -> unsigned { return strlen(_data); }
  const char* _data;
};

//strings

template<> struct stringify<string> {
  stringify(const string& source) : _text(source) {}
  auto data() const -> const char* { return _text.data(); }
  auto size() const -> unsigned { return _text.size(); }
  const string& _text;
};

template<> struct stringify<string_view> {
  stringify(const string_view& source) : _view(source) {}
  auto data() const -> const char* { return _view.data(); }
  auto size() const -> unsigned { return _view.size(); }
  const string_view& _view;
};

//pointers

template<typename T> auto make_string(T value) -> stringify<T> {
  return stringify<T>(std::forward<T>(value));
}

template<bool Insensitive>
auto string::_compare(const char* target, unsigned capacity, const char* source, unsigned size) -> int {
  if(Insensitive) return memory::icompare(target, capacity, source, size);
  return memory::compare(target, capacity, source, size);
}

auto string::compare(string_view source) const -> int {
  return memory::compare(data(), size() + 1, source.data(), source.size() + 1);
}

auto string::beginsWith(string_view source) const -> bool {
  if(source.size() > size()) return false;
  return memory::compare(data(), source.data(), source.size()) == 0;
}

auto string::endsWith(string_view source) const -> bool {
  if(source.size() > size()) return false;
  return memory::compare(data() + size() - source.size(), source.data(), source.size()) == 0;
}

auto string::downcase() -> string& {
  char* p = get();
  for(unsigned n = 0; n < size(); n++) {
    if(p[n] >= 'A' && p[n] <= 'Z') p[n] += 0x20;
  }
  return *this;
}

auto string::transform(string_view from, string_view to) -> string& {
  if(from.size() != to.size() || from.size() == 0) return *this;  //patterns must be the same length
  char* p = get();
  for(unsigned n = 0; n < size(); n++) {
    for(unsigned s = 0; s < from.size(); s++) {
      if(p[n] == from[s]) {
        p[n] = to[s];
        break;
      }
    }
  }
  return *this;
}

//only allocators may access _data or modify _size and _capacity
//all other functions must use data(), size(), capacity()

auto string::operator[](unsigned position) const -> const char& {
  return data()[position];
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

template<bool Insensitive, bool Quoted> auto string::_find(int offset, string_view source) const -> maybe<unsigned> {
  if(source.size() == 0) return nothing;
  auto p = data();
  for(unsigned n = offset, quoted = 0; n < size();) {
    if(Quoted) { if(p[n] == '\"') { quoted ^= 1; n++; continue; } if(quoted) { n++; continue; } }
    if(_compare<Insensitive>(p + n, size() - n, source.data(), source.size())) { n++; continue; }
    return n - offset;
  }
  return nothing;
}

auto string::find(string_view source) const -> maybe<unsigned> { return _find<0, 0>(0, source); }

auto hex(uintmax value, long precision, char padchar) -> string {
  string buffer;
  buffer.resize(sizeof(uintmax) * 2);
  char* p = buffer.get();

  unsigned size = 0;
  do {
    unsigned n = value & 15;
    p[size++] = n < 10 ? '0' + n : 'a' + n - 10;
    value >>= 4;
  } while(value);
  buffer.resize(size);
  buffer.reverse();
  if(precision) buffer.size(precision, padchar);
  return buffer;
}

auto string::match(string_view source) const -> bool {
  const char* s = data();
  const char* p = source.data();

  const char* cp = nullptr;
  const char* mp = nullptr;
  while(*s && *p != '*') {
    if(*p != '?' && *s != *p) return false;
    p++, s++;
  }
  while(*s) {
    if(*p == '*') {
      if(!*++p) return true;
      mp = p, cp = s + 1;
    } else if(*p == '?' || *p == *s) {
      p++, s++;
    } else {
      p = mp, s = cp++;
    }
  }
  while(*p == '*') p++;
  return !*p;
}

template<bool Insensitive, bool Quoted>
auto string::_replace(string_view from, string_view to, long limit) -> string& {
  if(limit <= 0 || from.size() == 0) return *this;

  int size = this->size();
  int matches = 0;
  int quoted = 0;

  //count matches first, so that we only need to reallocate memory once
  //(recording matches would also require memory allocation, so this is not done)
  { const char* p = data();
    for(int n = 0; n <= size - (int)from.size();) {
      if(Quoted) { if(p[n] == '\"') { quoted ^= 1; n++; continue; } if(quoted) { n++; continue; } }
      if(_compare<Insensitive>(p + n, size - n, from.data(), from.size())) { n++; continue; }

      if(++matches >= limit) break;
      n += from.size();
    }
  }
  if(matches == 0) return *this;

  //in-place overwrite
  if(to.size() == from.size()) {
    char* p = get();

    for(int n = 0, remaining = matches, quoted = 0; n <= size - (int)from.size();) {
      if(Quoted) { if(p[n] == '\"') { quoted ^= 1; n++; continue; } if(quoted) { n++; continue; } }
      if(_compare<Insensitive>(p + n, size - n, from.data(), from.size())) { n++; continue; }

      memory::copy(p + n, to.data(), to.size());

      if(!--remaining) break;
      n += from.size();
    }
  }

  //left-to-right shrink
  else if(to.size() < from.size()) {
    char* p = get();
    int offset = 0;
    int base = 0;

    for(int n = 0, remaining = matches, quoted = 0; n <= size - (int)from.size();) {
      if(Quoted) { if(p[n] == '\"') { quoted ^= 1; n++; continue; } if(quoted) { n++; continue; } }
      if(_compare<Insensitive>(p + n, size - n, from.data(), from.size())) { n++; continue; }

      if(offset) memory::move(p + offset, p + base, n - base);
      memory::copy(p + offset + (n - base), to.data(), to.size());
      offset += (n - base) + to.size();

      n += from.size();
      base = n;
      if(!--remaining) break;
    }

    memory::move(p + offset, p + base, size - base);
    resize(size - matches * (from.size() - to.size()));
  }

  //right-to-left expand
  else if(to.size() > from.size()) {
    resize(size + matches * (to.size() - from.size()));
    char* p = get();

    int offset = this->size();
    int base = size;

    for(int n = size, remaining = matches; n >= (int)from.size();) {  //quoted reused from parent scope since we are iterating backward
      if(Quoted) { if(p[n] == '\"') { quoted ^= 1; n--; continue; } if(quoted) { n--; continue; } }
      if(_compare<Insensitive>(p + n - from.size(), size - n + from.size(), from.data(), from.size())) { n--; continue; }

      memory::move(p + offset - (base - n), p + base - (base - n), base - n);
      memory::copy(p + offset - (base - n) - to.size(), to.data(), to.size());
      offset -= (base - n) + to.size();

      if(!--remaining) break;
      n -= from.size();
      base = n;
    }
  }

  return *this;
}

auto string::replace(string_view from, string_view to, long limit) -> string& { return _replace<0, 0>(from, to, limit); }

template<bool Insensitive, bool Quoted>
auto vector<string>::_split(string_view source, string_view find, long limit) -> vector<string>& {
  reset();
  if(limit <= 0 || find.size() == 0) return *this;

  const char* p = source.data();
  int size = source.size();
  int base = 0;
  int matches = 0;

  for(int n = 0, quoted = 0; n <= size - (int)find.size();) {
    if(Quoted) { if(p[n] == '\"') { quoted ^= 1; n++; continue; } if(quoted) { n++; continue; } }
    if(string::_compare<Insensitive>(p + n, size - n, find.data(), find.size())) { n++; continue; }
    if(matches >= limit) break;

    string& s = operator()(matches);
    s.resize(n - base);
    memory::copy(s.get(), p + base, n - base);

    n += find.size();
    base = n;
    matches++;
  }

  string& s = operator()(matches);
  s.resize(size - base);
  memory::copy(s.get(), p + base, size - base);

  return *this;
}

auto string::split(string_view on, long limit) const -> vector<string> { return vector<string>()._split<0, 0>(*this, on, limit); }

auto string::trimLeft(string_view lhs, long limit) -> string& {
  if(lhs.size() == 0) return *this;
  long matches = 0;
  while(matches < limit) {
    int offset = lhs.size() * matches;
    int length = (int)size() - offset;
    if(length < (int)lhs.size()) break;
    if(memory::compare(data() + offset, lhs.data(), lhs.size()) != 0) break;
    matches++;
  }
  if(matches) remove(0, lhs.size() * matches);
  return *this;
}

auto string::trimRight(string_view rhs, long limit) -> string& {
  if(rhs.size() == 0) return *this;
  long matches = 0;
  while(matches < limit) {
    int offset = (int)size() - rhs.size() * (matches + 1);
    int length = (int)size() - offset;
    if(offset < 0 || length < (int)rhs.size()) break;
    if(memory::compare(data() + offset, rhs.data(), rhs.size()) != 0) break;
    matches++;
  }
  if(matches) resize(size() - rhs.size() * matches);
  return *this;
}

auto string::strip() -> string& {
  stripRight();
  stripLeft();
  return *this;
}

auto string::stripLeft() -> string& {
  unsigned length = 0;
  while(length < size()) {
    char input = operator[](length);
    if(input != ' ' && input != '\t' && input != '\r' && input != '\n') break;
    length++;
  }
  if(length) remove(0, length);
  return *this;
}

auto string::stripRight() -> string& {
  unsigned length = 0;
  while(length < size()) {
    bool matched = false;
    char input = operator[](size() - length - 1);
    if(input != ' ' && input != '\t' && input != '\r' && input != '\n') break;
    length++;
  }
  if(length) resize(size() - length);
  return *this;
}

auto string::read(string_view filename) -> string {
  FILE* fp = fopen(filename, "rb");

  string result;
  if(!fp) return result;

  fseek(fp, 0, SEEK_END);
  int filesize = ftell(fp);
  if(filesize < 0) return fclose(fp), result;

  rewind(fp);
  result.resize(filesize);
  (void)fread(result.get(), 1, filesize, fp);
  return fclose(fp), result;
}

auto string::fill(char fill) -> string& {
  memory::fill(get(), size(), fill);
  return *this;
}

auto string::remove(unsigned offset, unsigned length) -> string& {
  char* p = get();
  length = std::min(length, size());
  memory::move(p + offset, p + offset + length, size() - length);
  return resize(size() - length);
}

auto string::reverse() -> string& {
  char* p = get();
  unsigned length = size();
  unsigned pivot = length >> 1;
  for(int x = 0, y = length - 1; x < pivot && y >= 0; x++, y--) std::swap(p[x], p[y]);
  return *this;
}

//+length => insert/delete from start (right justify)
//-length => insert/delete from end (left justify)
auto string::size(int length, char fill) -> string& {
  unsigned size = this->size();
  if(size == length) return *this;

  bool right = length >= 0;
  length = abs(length);

  if(size < length) {  //expand
    resize(length);
    char* p = get();
    unsigned displacement = length - size;
    if(right) memory::move(p + displacement, p, size);
    else p += size;
    while(displacement--) *p++ = fill;
  } else {  //shrink
    char* p = get();
    unsigned displacement = size - length;
    if(right) memory::move(p, p + displacement, length);
    resize(length);
  }

  return *this;
}

auto slice(string_view self, int offset, int length) -> string {
  string result;
  if(offset < 0) offset = self.size() - abs(offset);
  if(offset >= 0 && offset < self.size()) {
    if(length < 0) length = self.size() - offset;
    if(length >= 0) {
      result.resize(length);
      memory::copy(result.get(), self.data() + offset, length);
    }
  }
  return result;
}

auto string::slice(int offset, int length) const -> string {
  return nall::slice(*this, offset, length);
}

template<typename T> auto fromInteger(char* result, T value) -> char* {
  bool negative = value < 0;
  if(!negative) value = -value;  //negate positive integers to support eg INT_MIN

  char buffer[1 + sizeof(T) * 3];
  unsigned size = 0;

  do {
    int n = value % 10;  //-0 to -9
    buffer[size++] = '0' - n;  //'0' to '9'
    value /= 10;
  } while(value);
  if(negative) buffer[size++] = '-';

  for(int x = size - 1, y = 0; x >= 0 && y < size; x--, y++) result[x] = buffer[y];
  result[size] = 0;
  return result;
}

template<typename T> auto fromNatural(char* result, T value) -> char* {
  char buffer[1 + sizeof(T) * 3];
  unsigned size = 0;

  do {
    unsigned n = value % 10;
    buffer[size++] = '0' + n;
    value /= 10;
  } while(value);

  for(int x = size - 1, y = 0; x >= 0 && y < size; x--, y++) result[x] = buffer[y];
  result[size] = 0;
  return result;
}

template<typename... P> auto vector<string>::append(const string& data, P&&... p) -> vector<string>& {
  vector_base::append(data);
  append(std::forward<P>(p)...);
  return *this;
}

auto vector<string>::append() -> vector<string>& {
  return *this;
}

auto vector<string>::merge(string_view separator) const -> string {
  string output;
  for(unsigned n = 0; n < size(); n++) {
    output.append(operator[](n));
    if(n < size() - 1) output.append(separator.data());
  }
  return output;
}

string_view::string_view() {
  _string = nullptr;
  _data = "";
  _size = 0;
}

string_view::string_view(const string_view& source) {
  if(this == &source) return;
  _string = nullptr;
  _data = source._data;
  _size = source._size;
}

string_view::string_view(const char* data) {
  _string = nullptr;
  _data = data;
  _size = -1;  //defer length calculation, as it is often unnecessary
}

string_view::string_view(const string& source) {
  _string = nullptr;
  _data = source.data();
  _size = source.size();
}

template<typename... P>
string_view::string_view(P&&... p) {
  _string = new string{std::forward<P>(p)...};
  _data = _string->data();
  _size = _string->size();
}

string_view::~string_view() {
  if(_string) delete _string;
}

string_view::operator const char*() const {
  return _data;
}

auto string_view::data() const -> const char* {
  return _data;
}

auto string_view::size() const -> unsigned {
  if(_size < 0) _size = strlen(_data);
  return _size;
}

}
