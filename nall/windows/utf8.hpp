#pragma once

using uint = unsigned;

namespace nall {
  //UTF-8 to UTF-16
  struct utf16_t {
    utf16_t(const char* s = "") { operator=(s); }
    ~utf16_t() { reset(); }

    utf16_t(const utf16_t&) = delete;
    auto operator=(const utf16_t&) -> utf16_t& = delete;

    auto operator=(const char* s) -> utf16_t& {
      reset();
      if(!s) s = "";
      length = MultiByteToWideChar(CP_UTF8, 0, s, -1, nullptr, 0);
      buffer = new wchar_t[length + 1];
      MultiByteToWideChar(CP_UTF8, 0, s, -1, buffer, length);
      buffer[length] = 0;
      return *this;
    }

    operator wchar_t*() { return buffer; }
    operator const wchar_t*() const { return buffer; }

    auto reset() -> void {
      delete[] buffer;
      length = 0;
    }

    auto data() -> wchar_t* { return buffer; }
    auto data() const -> const wchar_t* { return buffer; }

    auto size() const -> uint { return length; }

  private:
    wchar_t* buffer = nullptr;
    uint length = 0;
  };
}
