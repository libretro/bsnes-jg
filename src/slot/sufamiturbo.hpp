struct SufamiTurboCartridge {
  void unload();
  void power();
  void serialize(serializer&);

  unsigned pathID = 0;
  ReadableMemory rom;
  WritableMemory ram;
};

extern SufamiTurboCartridge sufamiturboA;
extern SufamiTurboCartridge sufamiturboB;
