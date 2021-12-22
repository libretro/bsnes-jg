struct SuperMultitap : Controller {
  enum : unsigned {
    Up, Down, Left, Right, B, A, Y, X, L, R, Select, Start,
  };

  SuperMultitap(unsigned port);

  auto data() -> nall::Natural< 2>;
  auto latch(bool data) -> void;

private:
  bool latched;
  unsigned counter1;
  unsigned counter2;

  struct Gamepad {
    nall::Boolean b, y, select, start;
    nall::Boolean up, down, left, right;
    nall::Boolean a, x, l, r;
  } gamepads[4];
};
