namespace Heuristics {

struct Memory {
  auto& type(string type) { _type = type; return *this; }
  auto& size(Natural<> size) { _size = size; return *this; }
  auto& content(string content) { _content = content; return *this; }
  auto& manufacturer(string manufacturer) { _manufacturer = manufacturer; return *this; }
  auto& architecture(string architecture) { _architecture = architecture; return *this; }
  auto& identifier(string identifier) { _identifier = identifier; return *this; }
  auto& isVolatile() { _volatile = true; return *this; }
  string text() const;

  string _type;
  Boolean _battery;
  Natural<> _size;
  string _content;
  string _manufacturer;
  string _architecture;
  string _identifier;
  Boolean _volatile;
};

struct Oscillator {
  auto& frequency(Natural<> frequency) { _frequency = frequency; return *this; }
  string text() const;

  Natural<> _frequency;
};

struct Slot {
  auto& type(string type) { _type = type; return *this; }
  string text() const;

  string _type;
};

struct BSMemory {
  BSMemory(vector<uint8_t>& data, string location);
  explicit operator bool() const;
  string manifest() const;

private:
  vector<uint8_t>& data;
  string location;
};

struct GameBoy {
  GameBoy(vector<uint8_t>& data, string location);
  explicit operator bool() const;
  string manifest() const;

private:
  vector<uint8_t>& data;
  string location;
  unsigned headerAddress = 0;
  
  uint8_t read(unsigned offset) const { return data[headerAddress + offset]; }
};

struct SufamiTurbo {
  SufamiTurbo(vector<uint8_t>& data, string location);
  explicit operator bool() const;

  std::string manifest() const;

private:
  vector<uint8_t>& data;
  string location;
};

struct SuperFamicom {
  SuperFamicom(vector<uint8_t>& data, string location);
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
  string firmwareARM() const;
  string firmwareEXNEC() const;
  string firmwareGB() const;
  string firmwareHITACHI() const;
  string firmwareNEC() const;

  vector<uint8_t>& data;
  string location;
  unsigned headerAddress = 0;
};
}
