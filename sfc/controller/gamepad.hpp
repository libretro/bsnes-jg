struct Gamepad : Controller {
  enum : uint {
    Up, Down, Left, Right, B, A, Y, X, L, R, Select, Start,
  };

  Gamepad(uint port);

  auto data() -> uint2;
  auto latch(bool data) -> void;

private:
  bool latched;
  uint counter;

  Boolean b, y, select, start;
  Boolean up, down, left, right;
  Boolean a, x, l, r;
};
