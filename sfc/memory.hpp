struct Memory {
  static bool GlobalWriteEnable;

  virtual ~Memory() { reset(); }
  inline explicit operator bool() const { return size() > 0; }

  virtual auto reset() -> void {}
  virtual auto allocate(uint, uint8 = 0xff) -> void {}

  virtual auto data() -> uint8* = 0;
  virtual auto size() const -> uint = 0;

  virtual auto read(uint address, uint8 data = 0) -> uint8 = 0;
  virtual auto write(uint address, uint8 data) -> void = 0;

  uint id = 0;
};

struct ReadableMemory : Memory {
  inline auto reset() -> void override {
    delete[] self.data;
    self.data = nullptr;
    self.size = 0;
  }

  inline auto allocate(uint size, uint8 fill = 0xff) -> void override {
    if(self.size != size) {
      delete[] self.data;
      self.data = new uint8[self.size = size];
    }
    for(uint address : range(size)) {
      self.data[address] = fill;
    }
  }

  inline auto data() -> uint8* override {
    return self.data;
  }

  inline auto size() const -> uint override {
    return self.size;
  }

  inline auto read(uint address, uint8 data = 0) -> uint8 override {
    return self.data[address];
  }

  inline auto write(uint address, uint8 data) -> void override {
    if(Memory::GlobalWriteEnable) {
      self.data[address] = data;
    }
  }

  inline auto operator[](uint address) const -> uint8 {
    return self.data[address];
  }

private:
  struct {
    uint8* data = nullptr;
    uint size = 0;
  } self;
};

struct WritableMemory : Memory {
  inline auto reset() -> void override {
    delete[] self.data;
    self.data = nullptr;
    self.size = 0;
  }

  inline auto allocate(uint size, uint8 fill = 0xff) -> void override {
    if(self.size != size) {
      delete[] self.data;
      self.data = new uint8[self.size = size];
    }
    for(uint address : range(size)) {
      self.data[address] = fill;
    }
  }

  inline auto data() -> uint8* override {
    return self.data;
  }

  inline auto size() const -> uint override {
    return self.size;
  }

  inline auto read(uint address, uint8 data = 0) -> uint8 override {
    return self.data[address];
  }

  inline auto write(uint address, uint8 data) -> void override {
    self.data[address] = data;
  }

  inline auto operator[](uint address) -> uint8& {
    return self.data[address];
  }

private:
  struct {
    uint8* data = nullptr;
    uint size = 0;
  } self;
};

struct ProtectableMemory : Memory {
  inline auto reset() -> void override {
    delete[] self.data;
    self.data = nullptr;
    self.size = 0;
  }

  inline auto allocate(uint size, uint8 fill = 0xff) -> void override {
    if(self.size != size) {
      delete[] self.data;
      self.data = new uint8[self.size = size];
    }
    for(uint address : range(size)) {
      self.data[address] = fill;
    }
  }

  inline auto data() -> uint8* override {
    return self.data;
  }

  inline auto size() const -> uint override {
    return self.size;
  }

  inline auto writable() const -> bool {
    return self.writable;
  }

  inline auto writable(bool writable) -> void {
    self.writable = writable;
  }

  inline auto read(uint address, uint8 data = 0) -> uint8 override {
    return self.data[address];
  }

  inline auto write(uint address, uint8 data) -> void override {
    if(self.writable || Memory::GlobalWriteEnable) {
      self.data[address] = data;
    }
  }

  inline auto operator[](uint address) const -> uint8 {
    return self.data[address];
  }

private:
  struct {
    uint8* data = nullptr;
    uint size = 0;
    bool writable = false;
  } self;
};

struct Bus {
  alwaysinline static auto mirror(uint address, uint size) -> uint;
  alwaysinline static auto reduce(uint address, uint mask) -> uint;

  ~Bus();

  alwaysinline auto read(uint address, uint8 data = 0) -> uint8;
  alwaysinline auto write(uint address, uint8 data) -> void;

  auto reset() -> void;
  auto map(
    const function<uint8 (uint, uint8)>& read,
    const function<void (uint, uint8)>& write,
    const string& address, uint size = 0, uint base = 0, uint mask = 0
  ) -> uint;
  auto unmap(const string& address) -> void;

private:
  uint8* lookup = nullptr;
  uint32* target = nullptr;

  function<uint8 (uint, uint8)> reader[256];
  function<void  (uint, uint8)> writer[256];
  uint counter[256];
};

extern Bus bus;
