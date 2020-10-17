namespace Heuristics {

struct Memory {
  auto& type(string type) { _type = type; return *this; }
  auto& size(natural size) { _size = size; return *this; }
  auto& content(string content) { _content = content; return *this; }
  auto& manufacturer(string manufacturer) { _manufacturer = manufacturer; return *this; }
  auto& architecture(string architecture) { _architecture = architecture; return *this; }
  auto& identifier(string identifier) { _identifier = identifier; return *this; }
  auto& isVolatile() { _volatile = true; return *this; }
  string text() const;

  string _type;
  boolean _battery;
  natural _size;
  string _content;
  string _manufacturer;
  string _architecture;
  string _identifier;
  boolean _volatile;
};

struct Oscillator {
  auto& frequency(natural frequency) { _frequency = frequency; return *this; }
  string text() const;

  natural _frequency;
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
  uint headerAddress = 0;
  
  uint8_t read(uint offset) const { return data[headerAddress + offset]; }
};

struct SufamiTurbo {
  SufamiTurbo(vector<uint8_t>& data, string location);
  explicit operator bool() const;

  string manifest() const;

private:
  vector<uint8_t>& data;
  string location;
};

struct SuperFamicom {
  SuperFamicom(vector<uint8_t>& data, string location);
  explicit operator bool() const;

  string manifest() const;
  string region() const;
  string videoRegion() const;
  string revision() const;
  string board() const;
  string title() const;
  string serial() const;
  uint romSize() const;
  uint programRomSize() const;
  uint dataRomSize() const;
  uint expansionRomSize() const;
  uint firmwareRomSize() const;
  uint ramSize() const;
  uint expansionRamSize() const;
  bool nonVolatile() const;

private:
  uint size() const { return data.size(); }
  uint scoreHeader(uint address);
  string firmwareARM() const;
  string firmwareEXNEC() const;
  string firmwareGB() const;
  string firmwareHITACHI() const;
  string firmwareNEC() const;

  vector<uint8_t>& data;
  string location;
  uint headerAddress = 0;
};
}
