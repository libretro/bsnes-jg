struct SuperScope : Controller {
  enum : unsigned {
    X, Y, Trigger, Cursor, Turbo, Pause,
  };

  SuperScope(unsigned port);

  auto data() -> nall::Natural< 2>;
  auto latch(bool data) -> void;
  auto latch() -> void override;
  auto draw(uint16_t* data, unsigned pitch, unsigned width, unsigned height) -> void override;

private:
  bool latched;
  unsigned counter;

  int x;
  int y;

  bool trigger;
  bool cursor;
  bool turbo;
  bool pause;
  bool offscreen;

  bool oldturbo;
  bool triggerlock;
  bool pauselock;

  unsigned prev;
};
