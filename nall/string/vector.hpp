#pragma once

namespace nall {

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

}
