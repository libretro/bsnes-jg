struct SuperScope : Controller {
  enum : unsigned {
    X, Y, Trigger, Cursor, Turbo, Pause,
  };

  SuperScope(unsigned port);

  nall::Natural< 2> data() override;
  void latch(bool data) override;
  void latch() override;
  void draw(uint16_t* data, unsigned pitch, unsigned width, unsigned height) override;

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
