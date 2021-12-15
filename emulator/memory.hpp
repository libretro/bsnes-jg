#pragma once

namespace Emulator::Memory {

inline auto mirror(uint address, uint size) -> uint {
  if(size == 0) return 0;
  uint base = 0;
  uint mask = 1 << 31;
  while(address >= size) {
    while(!(address & mask)) mask >>= 1;
    address -= mask;
    if(size > mask) {
      size -= mask;
      base += mask;
    }
    mask >>= 1;
  }
  return base + address;
}

inline auto reduce(uint address, uint mask) -> uint {
  while(mask) {
    uint bits = (mask & -mask) - 1;
    address = address >> 1 & ~bits | address & bits;
    mask = (mask & mask - 1) >> 1;
  }
  return address;
}

template<typename T>
struct Readable {
  ~Readable() { reset(); }

  inline auto reset() -> void {
    delete[] self.data;
    self.data = nullptr;
    self.size = 0;
    self.mask = 0;
  }

  inline auto allocate(uint size, T fill = ~0ull) -> void {
    if(!size) return reset();
    delete[] self.data;
    self.size = size;
    self.mask = bit::round(self.size) - 1;
    self.data = new T[self.mask + 1];
    memory::fill<T>(self.data, self.mask + 1, fill);
  }

  inline auto load(shared_pointer<vfs::file> fp) -> void {
    fp->read(self.data, min(fp->size(), self.size * sizeof(T)));
    for(uint address = self.size; address <= self.mask; address++) {
      self.data[address] = self.data[mirror(address, self.size)];
    }
  }

  inline auto save(shared_pointer<vfs::file> fp) -> void {
    fp->write(self.data, self.size * sizeof(T));
  }

  explicit operator bool() const { return (bool)self.data; }
  inline auto data() const -> const T* { return self.data; }
  inline auto size() const -> uint { return self.size; }
  inline auto mask() const -> uint { return self.mask; }

  inline auto operator[](uint address) const -> T { return self.data[address & self.mask]; }
  inline auto read(uint address) const -> T { return self.data[address & self.mask]; }
  inline auto write(uint address, T data) const -> void {}

  auto serialize(serializer& s) -> void {
    const uint size = self.size;
    s.integer(self.size);
    s.integer(self.mask);
    if(self.size != size) allocate(self.size);
    s.array(self.data, self.size);
  }

private:
  struct {
    T* data = nullptr;
    uint size = 0;
    uint mask = 0;
  } self;
};

template<typename T>
struct Writable {
  ~Writable() { reset(); }

  inline auto reset() -> void {
    delete[] self.data;
    self.data = nullptr;
    self.size = 0;
    self.mask = 0;
  }

  inline auto allocate(uint size, T fill = ~0ull) -> void {
    if(!size) return reset();
    delete[] self.data;
    self.size = size;
    self.mask = bit::round(self.size) - 1;
    self.data = new T[self.mask + 1];
    memory::fill<T>(self.data, self.mask + 1, fill);
  }

  inline auto load(shared_pointer<vfs::file> fp) -> void {
    fp->read(self.data, min(fp->size(), self.size * sizeof(T)));
    for(uint address = self.size; address <= self.mask; address++) {
      self.data[address] = self.data[mirror(address, self.size)];
    }
  }

  inline auto save(shared_pointer<vfs::file> fp) -> void {
    fp->write(self.data, self.size * sizeof(T));
  }

  explicit operator bool() const { return (bool)self.data; }
  inline auto data() -> T* { return self.data; }
  inline auto data() const -> const T* { return self.data; }
  inline auto size() const -> uint { return self.size; }
  inline auto mask() const -> uint { return self.mask; }

  inline auto operator[](uint address) -> T& { return self.data[address & self.mask]; }
  inline auto operator[](uint address) const -> T { return self.data[address & self.mask]; }
  inline auto read(uint address) const -> T { return self.data[address & self.mask]; }
  inline auto write(uint address, T data) -> void { self.data[address & self.mask] = data; }

  auto serialize(serializer& s) -> void {
    const uint size = self.size;
    s.integer(self.size);
    s.integer(self.mask);
    if(self.size != size) allocate(self.size);
    s.array(self.data, self.size);
  }

private:
  struct {
    T* data = nullptr;
    uint size = 0;
    uint mask = 0;
  } self;
};

}
