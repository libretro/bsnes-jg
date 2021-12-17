struct Memory {
  static bool GlobalWriteEnable;

  virtual ~Memory() { reset(); }
  inline explicit operator bool() const { return size() > 0; }

  virtual auto reset() -> void {}
  virtual auto allocate(unsigned, uint8_t = 0xff) -> void {}

  virtual auto data() -> uint8_t* = 0;
  virtual auto size() const -> unsigned = 0;

  virtual auto read(unsigned address, uint8_t data = 0) -> uint8_t = 0;
  virtual auto write(unsigned address, uint8_t data) -> void = 0;

  unsigned id = 0;
};

struct ReadableMemory : Memory {
  inline auto reset() -> void override {
    delete[] self.data;
    self.data = nullptr;
    self.size = 0;
  }

  inline auto allocate(unsigned size, uint8_t fill = 0xff) -> void override {
    if(self.size != size) {
      delete[] self.data;
      self.data = new uint8_t[self.size = size];
    }
    for(unsigned address : range(size)) {
      self.data[address] = fill;
    }
  }

  inline auto data() -> uint8_t* override {
    return self.data;
  }

  inline auto size() const -> unsigned override {
    return self.size;
  }

  inline auto read(unsigned address, uint8_t data = 0) -> uint8_t override {
    return self.data[address];
  }

  inline auto write(unsigned address, uint8_t data) -> void override {
    if(Memory::GlobalWriteEnable) {
      self.data[address] = data;
    }
  }

  inline auto operator[](unsigned address) const -> uint8_t {
    return self.data[address];
  }

private:
  struct {
    uint8_t* data = nullptr;
    unsigned size = 0;
  } self;
};

struct WritableMemory : Memory {
  inline auto reset() -> void override {
    delete[] self.data;
    self.data = nullptr;
    self.size = 0;
  }

  inline auto allocate(unsigned size, uint8_t fill = 0xff) -> void override {
    if(self.size != size) {
      delete[] self.data;
      self.data = new uint8_t[self.size = size];
    }
    for(unsigned address : range(size)) {
      self.data[address] = fill;
    }
  }

  inline auto data() -> uint8_t* override {
    return self.data;
  }

  inline auto size() const -> unsigned override {
    return self.size;
  }

  inline auto read(unsigned address, uint8_t data = 0) -> uint8_t override {
    return self.data[address];
  }

  inline auto write(unsigned address, uint8_t data) -> void override {
    self.data[address] = data;
  }

  inline auto operator[](unsigned address) -> uint8_t& {
    return self.data[address];
  }

private:
  struct {
    uint8_t* data = nullptr;
    unsigned size = 0;
  } self;
};

struct ProtectableMemory : Memory {
  inline auto reset() -> void override {
    delete[] self.data;
    self.data = nullptr;
    self.size = 0;
  }

  inline auto allocate(unsigned size, uint8_t fill = 0xff) -> void override {
    if(self.size != size) {
      delete[] self.data;
      self.data = new uint8_t[self.size = size];
    }
    for(unsigned address : range(size)) {
      self.data[address] = fill;
    }
  }

  inline auto data() -> uint8_t* override {
    return self.data;
  }

  inline auto size() const -> unsigned override {
    return self.size;
  }

  inline auto writable() const -> bool {
    return self.writable;
  }

  inline auto writable(bool writable) -> void {
    self.writable = writable;
  }

  inline auto read(unsigned address, uint8_t data = 0) -> uint8_t override {
    return self.data[address];
  }

  inline auto write(unsigned address, uint8_t data) -> void override {
    if(self.writable || Memory::GlobalWriteEnable) {
      self.data[address] = data;
    }
  }

  inline auto operator[](unsigned address) const -> uint8_t {
    return self.data[address];
  }

private:
  struct {
    uint8_t* data = nullptr;
    unsigned size = 0;
    bool writable = false;
  } self;
};

struct Bus {
  alwaysinline static auto mirror(unsigned address, unsigned size) -> unsigned;
  alwaysinline static auto reduce(unsigned address, unsigned mask) -> unsigned;

  ~Bus();

  alwaysinline auto read(unsigned address, uint8_t data = 0) -> uint8_t;
  alwaysinline auto write(unsigned address, uint8_t data) -> void;

  auto reset() -> void;
  auto map(
    const function<uint8_t (unsigned, uint8_t)>& read,
    const function<void (unsigned, uint8_t)>& write,
    const string& address, unsigned size = 0, unsigned base = 0, unsigned mask = 0
  ) -> unsigned;
  auto unmap(const string& address) -> void;

private:
  uint8_t *lookup = nullptr;
  uint32_t *target = nullptr;

  function<uint8_t (unsigned, uint8_t)> reader[256];
  function<void  (unsigned, uint8_t)> writer[256];
  unsigned counter[256];
};

extern Bus bus;
