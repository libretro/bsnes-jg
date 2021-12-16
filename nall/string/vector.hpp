#pragma once

namespace nall {

template<typename... P> auto vector<string>::append(const string& data, P&&... p) -> type& {
  vector_base::append(data);
  append(std::forward<P>(p)...);
  return *this;
}

auto vector<string>::append() -> type& {
  return *this;
}

auto vector<string>::find(string_view source) const -> maybe<unsigned> {
  for(unsigned n = 0; n < size(); n++) {
    if(operator[](n).equals(source)) return n;
  }
  return {};
}

auto vector<string>::ifind(string_view source) const -> maybe<unsigned> {
  for(unsigned n = 0; n < size(); n++) {
    if(operator[](n).iequals(source)) return n;
  }
  return {};
}

auto vector<string>::match(string_view pattern) const -> vector<string> {
  vector<string> result;
  for(unsigned n = 0; n < size(); n++) {
    if(operator[](n).match(pattern)) result.append(operator[](n));
  }
  return result;
}

auto vector<string>::merge(string_view separator) const -> string {
  string output;
  for(unsigned n = 0; n < size(); n++) {
    output.append(operator[](n));
    if(n < size() - 1) output.append(separator.data());
  }
  return output;
}

auto vector<string>::strip() -> type& {
  for(unsigned n = 0; n < size(); n++) {
    operator[](n).strip();
  }
  return *this;
}

}
