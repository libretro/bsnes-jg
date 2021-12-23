struct SuperMultitap : Controller {
  enum : unsigned {
    Up, Down, Left, Right, B, A, Y, X, L, R, Select, Start,
  };

  SuperMultitap(unsigned port);

  nall::Natural< 2> data();
  void latch(bool data);

private:
  bool latched;
  unsigned counter1;
  unsigned counter2;

  struct Gamepad {
    bool b, y, select, start;
    bool up, down, left, right;
    bool a, x, l, r;
  } gamepads[4];
};
