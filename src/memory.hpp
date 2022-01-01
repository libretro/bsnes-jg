struct Memory {
  static bool GlobalWriteEnable;

  virtual ~Memory() { reset(); }
  inline explicit operator bool() const { return size() > 0; }

  virtual void reset() {}
  virtual void allocate(unsigned, uint8_t = 0xff) {}

  virtual uint8_t* data() = 0;
  virtual unsigned size() const = 0;

  virtual uint8_t read(unsigned address, uint8_t data = 0) = 0;
  virtual void write(unsigned address, uint8_t data) = 0;

  unsigned id = 0;
};

struct ReadableMemory : Memory {
  inline void reset() override {
    delete[] self.data;
    self.data = nullptr;
    self.size = 0;
  }

  inline void allocate(unsigned size, uint8_t fill = 0xff) override {
    if(self.size != size) {
      delete[] self.data;
      self.data = new uint8_t[self.size = size];
    }
    for(unsigned address = 0; address < size; ++address) {
      self.data[address] = fill;
    }
  }

  inline uint8_t* data() override {
    return self.data;
  }

  inline unsigned size() const override {
    return self.size;
  }

  inline uint8_t read(unsigned address, uint8_t data = 0) override {
    return self.data[address];
  }

  inline void write(unsigned address, uint8_t data) override {
    if(Memory::GlobalWriteEnable) {
      self.data[address] = data;
    }
  }

  inline uint8_t operator[](unsigned address) const {
    return self.data[address];
  }

private:
  struct {
    uint8_t* data = nullptr;
    unsigned size = 0;
  } self;
};

struct WritableMemory : Memory {
  inline void reset() override {
    delete[] self.data;
    self.data = nullptr;
    self.size = 0;
  }

  inline void allocate(unsigned size, uint8_t fill = 0xff) override {
    if(self.size != size) {
      delete[] self.data;
      self.data = new uint8_t[self.size = size];
    }
    for(unsigned address = 0; address < size; ++address) {
      self.data[address] = fill;
    }
  }

  inline uint8_t* data() override {
    return self.data;
  }

  inline unsigned size() const override {
    return self.size;
  }

  inline uint8_t read(unsigned address, uint8_t data = 0) override {
    return self.data[address];
  }

  inline void write(unsigned address, uint8_t data) override {
    self.data[address] = data;
  }

  inline uint8_t& operator[](unsigned address) {
    return self.data[address];
  }

private:
  struct {
    uint8_t* data = nullptr;
    unsigned size = 0;
  } self;
};

struct Bus {
  inline static unsigned mirror(unsigned address, unsigned size);
  inline static unsigned reduce(unsigned address, unsigned mask);

  ~Bus();

  inline uint8_t read(unsigned address, uint8_t data = 0);
  inline void write(unsigned address, uint8_t data);

  void reset();
  unsigned map(
    const nall::function<uint8_t (unsigned, uint8_t)>& read,
    const nall::function<void (unsigned, uint8_t)>& write,
    const std::string& address, unsigned size = 0, unsigned base = 0, unsigned mask = 0
  );
  void unmap(const std::string& address);

private:
  uint8_t *lookup = nullptr;
  uint32_t *target = nullptr;

  nall::function<uint8_t (unsigned, uint8_t)> reader[256];
  nall::function<void  (unsigned, uint8_t)> writer[256];
  unsigned counter[256];
};

extern Bus bus;
