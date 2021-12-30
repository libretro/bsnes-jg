struct Justifier : Controller {
  enum : unsigned {
    X, Y, Trigger, Start,
  };

  Justifier(unsigned port, bool chained);

  nall::Natural< 2> data() override;
  void latch(bool data) override;
  void latch() override;

//private:
  const bool chained;  //true if the second justifier is attached to the first
  const unsigned device;
  bool latched;
  unsigned counter;
  unsigned prev;

  bool active;
  struct Player {
    int x;
    int y;
    bool trigger;
    bool start;
  } player1, player2;
};
