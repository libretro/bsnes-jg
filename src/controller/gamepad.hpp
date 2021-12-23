struct Gamepad : Controller {
  enum : unsigned {
    Up, Down, Left, Right, B, A, Y, X, L, R, Select, Start,
  };

  Gamepad(unsigned port);

  nall::Natural< 2> data();
  void latch(bool data);

private:
  bool latched;
  unsigned counter;

  nall::Boolean b, y, select, start;
  nall::Boolean up, down, left, right;
  nall::Boolean a, x, l, r;
};
