#pragma once

namespace nall {

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
