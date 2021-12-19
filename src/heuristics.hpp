namespace Heuristics {

struct Memory {
  auto& type(std::string type) { _type = type; return *this; }
  auto& size(size_t size) { _size = size; return *this; }
  auto& content(std::string content) { _content = content; return *this; }
  auto& manufacturer(std::string manufacturer) { _manufacturer = manufacturer; return *this; }
  auto& architecture(std::string architecture) { _architecture = architecture; return *this; }
  auto& identifier(std::string identifier) { _identifier = identifier; return *this; }
  auto& isVolatile() { _volatile = true; return *this; }
  std::string text() const;

  std::string _type;
  bool _battery;
  size_t _size;
  std::string _content;
  std::string _manufacturer;
  std::string _architecture;
  std::string _identifier;
  bool _volatile;
};

struct Oscillator {
  auto& frequency(unsigned frequency) { _frequency = frequency; return *this; }
  std::string text() const;

  unsigned _frequency;
};

struct Slot {
  auto& type(std::string type) { _type = type; return *this; }
  std::string text() const;

  std::string _type;
};

struct BSMemory {
  BSMemory(std::vector<uint8_t>& data, std::string location);
  explicit operator bool() const;
  std::string manifest() const;

private:
  std::vector<uint8_t>& data;
  std::string location;
};

struct GameBoy {
  GameBoy(std::vector<uint8_t>& data, std::string location);
  explicit operator bool() const;
  std::string manifest() const;

private:
  std::vector<uint8_t>& data;
  std::string location;
  unsigned headerAddress = 0;
  
  uint8_t read(unsigned offset) const { return data[headerAddress + offset]; }
};

struct SufamiTurbo {
  SufamiTurbo(std::vector<uint8_t>& data, std::string location);
  explicit operator bool() const;

  std::string manifest() const;

private:
  std::vector<uint8_t>& data;
  std::string location;
};

struct SuperFamicom {
  SuperFamicom(std::vector<uint8_t>& data, std::string location);
  explicit operator bool() const;

  std::string manifest() const;
  std::string region() const;
  std::string videoRegion() const;
  std::string revision() const;
  std::string board() const;
  std::string title() const;
  std::string serial() const;
  unsigned romSize() const;
  unsigned programRomSize() const;
  unsigned dataRomSize() const;
  unsigned expansionRomSize() const;
  unsigned firmwareRomSize() const;
  unsigned ramSize() const;
  unsigned expansionRamSize() const;
  bool nonVolatile() const;

private:
  unsigned size() const { return data.size(); }
  unsigned scoreHeader(unsigned address);
  std::string firmwareARM() const;
  std::string firmwareEXNEC() const;
  std::string firmwareGB() const;
  std::string firmwareHITACHI() const;
  std::string firmwareNEC() const;

  std::vector<uint8_t>& data;
  std::string location;
  unsigned headerAddress = 0;
};
}
