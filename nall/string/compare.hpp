#pragma once

namespace nall {

template<bool Insensitive>
auto string::_compare(const char* target, unsigned capacity, const char* source, unsigned size) -> int {
  if(Insensitive) return memory::icompare(target, capacity, source, size);
  return memory::compare(target, capacity, source, size);
}

auto string::compare(string_view source) const -> int {
  return memory::compare(data(), size() + 1, source.data(), source.size() + 1);
}

auto string::equals(string_view source) const -> bool {
  if(size() != source.size()) return false;
  return memory::compare(data(), source.data(), source.size()) == 0;
}

auto string::beginsWith(string_view source) const -> bool {
  if(source.size() > size()) return false;
  return memory::compare(data(), source.data(), source.size()) == 0;
}

auto string::endsWith(string_view source) const -> bool {
  if(source.size() > size()) return false;
  return memory::compare(data() + size() - source.size(), source.data(), source.size()) == 0;
}

}
