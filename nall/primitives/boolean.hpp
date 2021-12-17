#pragma once

namespace nall {

struct Boolean {
  inline Boolean() : data(false) {}
  template<typename T> inline Boolean(const T& value) : data(value) {}

  inline operator bool() const { return data; }

  inline auto lower() { return data == 1 ? data = 0, true : false; }

  inline auto flip(bool value) { return data != value ? (data = value, true) : false; }
  inline auto raise(bool value) { return !data && value ? (data = value, true) : (data = value, false); }
  inline auto lower(bool value) { return data && !value ? (data = value, true) : (data = value, false); }

private:
  bool data;
};

}
