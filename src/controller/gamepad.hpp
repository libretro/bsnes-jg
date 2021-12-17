struct Gamepad : Controller {
  enum : unsigned {
    Up, Down, Left, Right, B, A, Y, X, L, R, Select, Start,
  };

  Gamepad(unsigned port);

  auto data() -> nall::Natural< 2>;
  auto latch(bool data) -> void;

private:
  bool latched;
  unsigned counter;

  Boolean b, y, select, start;
  Boolean up, down, left, right;
  Boolean a, x, l, r;
};
