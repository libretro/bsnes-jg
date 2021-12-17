struct Mouse : Controller {
  enum : unsigned {
    X, Y, Left, Right,
  };

  Mouse(unsigned port);

  auto data() -> nall::Natural< 2>;
  auto latch(bool data) -> void;

private:
  bool latched;
  unsigned counter;

  unsigned speed;  //0 = slow, 1 = normal, 2 = fast
  int  x;      //x-coordinate
  int  y;      //y-coordinate
  bool dx;     //x-direction
  bool dy;     //y-direction
  bool l;      //left button
  bool r;      //right button
};
