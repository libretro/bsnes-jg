struct SufamiTurboCartridge {
  auto unload() -> void;
  auto power() -> void;
  auto serialize(serializer&) -> void;

  unsigned pathID = 0;
  ReadableMemory rom;
  WritableMemory ram;
};

extern SufamiTurboCartridge sufamiturboA;
extern SufamiTurboCartridge sufamiturboB;
