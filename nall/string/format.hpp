#pragma once

namespace nall {

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

}
