#pragma once

namespace nall::vfs {

struct file {
  enum class mode : unsigned { read, write, modify, create };
  enum class index : unsigned { absolute, relative };

  virtual ~file() = default;

  virtual auto size() const -> uintmax = 0;
  virtual auto offset() const -> uintmax = 0;

  virtual auto seek(intmax offset, index = index::absolute) -> void = 0;
  virtual auto read() -> uint8_t = 0;
  virtual auto write(uint8_t data) -> void = 0;

  auto end() const -> bool {
    return offset() >= size();
  }

  auto read(void* vdata, uintmax bytes) -> void {
    auto data = (uint8_t*)vdata;
    while(bytes--) *data++ = read();
  }

  auto readl(unsigned bytes) -> uintmax {
    uintmax data = 0;
    for(auto n : range(bytes)) data |= (uintmax)read() << n * 8;
    return data;
  }

  auto readm(unsigned bytes) -> uintmax {
    uintmax data = 0;
    for(auto n : range(bytes)) data = data << 8 | read();
    return data;
  }

  auto reads() -> string {
    string s;
    s.resize(size());
    read(s.get<uint8_t>(), s.size());
    return s;
  }

  auto write(const void* vdata, uintmax bytes) -> void {
    auto data = (const uint8_t*)vdata;
    while(bytes--) write(*data++);
  }

  auto writel(uintmax data, unsigned bytes) -> void {
    for(auto n : range(bytes)) write(data), data >>= 8;
  }

  auto writes(const string& s) -> void {
    write(s.data<uint8_t>(), s.size());
  }
};

}

namespace nall::vfs::fs {

struct file : vfs::file {
  static auto open(string location_, mode mode_) -> shared_pointer<vfs::file> {
    auto instance = shared_pointer<file>{new file};
    if(!instance->_open(location_, mode_)) return {};
    return instance;
  }

  auto size() const -> uintmax override {
    return _fp.size();
  }

  auto offset() const -> uintmax override {
    return _fp.offset();
  }

  auto seek(intmax offset_, index index_) -> void override {
    _fp.seek(offset_, (unsigned)index_);
  }

  auto read() -> uint8_t override {
    return _fp.read();
  }

  auto write(uint8_t data_) -> void override {
    _fp.write(data_);
  }

private:
  file() = default;
  file(const file&) = delete;
  auto operator=(const file&) -> file& = delete;

  auto _open(string location_, mode mode_) -> bool {
    if(!_fp.open(location_, (unsigned)mode_)) return false;
    return true;
  }

  file_buffer _fp;
};

}

namespace nall::vfs::memory {

struct file : vfs::file {
  ~file() { delete[] _data; }

  static auto open(const void* data, uintmax size) -> shared_pointer<vfs::file> {
    auto instance = shared_pointer<file>{new file};
    instance->_open((const uint8_t*)data, size);
    return instance;
  }

  auto data() const -> const uint8_t* { return _data; }
  auto size() const -> uintmax override { return _size; }
  auto offset() const -> uintmax override { return _offset; }

  auto seek(intmax offset, index mode) -> void override {
    if(mode == index::absolute) _offset = (uintmax)offset;
    if(mode == index::relative) _offset += (intmax)offset;
  }

  auto read() -> uint8_t override {
    if(_offset >= _size) return 0x00;
    return _data[_offset++];
  }

  auto write(uint8_t data) -> void override {
    if(_offset >= _size) return;
    _data[_offset++] = data;
  }

private:
  file() = default;
  file(const file&) = delete;
  auto operator=(const file&) -> file& = delete;

  auto _open(const uint8_t* data, uintmax size) -> void {
    _size = size;
    _data = new uint8_t[size];
    nall::memory::copy(_data, data, size);
  }

  uint8_t* _data = nullptr;
  uintmax _size = 0;
  uintmax _offset = 0;
};

}
