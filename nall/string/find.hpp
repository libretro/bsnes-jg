#pragma once

namespace nall {

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

}
