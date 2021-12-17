#pragma once

//convert any (supported) type to a const char* without constructing a new nall::string
//this is used inside string{...} to build nall::string values

namespace nall {

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

template<> struct stringify<unsigned short> {
  stringify(unsigned short source) { fromNatural(_data, source); }
  auto data() const -> const char* { return _data; }
  auto size() const -> unsigned { return strlen(_data); }
  char _data[1 + sizeof(unsigned short) * 3];
};

template<> struct stringify<unsigned int> {
  stringify(unsigned int source) { fromNatural(_data, source); }
  auto data() const -> const char* { return _data; }
  auto size() const -> unsigned { return strlen(_data); }
  char _data[1 + sizeof(unsigned int) * 3];
};

#if defined(__SIZEOF_INT128__)
template<> struct stringify<uint128_t> {
  stringify(uint128_t source) { fromNatural(_data, source); }
  auto data() const -> const char* { return _data; }
  auto size() const -> unsigned { return strlen(_data); }
  char _data[1 + sizeof(uint128_t) * 3];
};
#endif

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

}
