Justifier::Justifier(uint port, bool chained):
Controller(port),
chained(chained),
device(!chained ? ID::Device::Justifier : ID::Device::Justifiers)
{
  latched = 0;
  counter = 0;
  active = 0;
  prev = 0;

  player1.x = 256 / 2;
  player1.y = 240 / 2;
  player1.trigger = false;
  player2.start = false;

  player2.x = 256 / 2;
  player2.y = 240 / 2;
  player2.trigger = false;
  player2.start = false;

  if(chained == false) {
    player2.x = -1;
    player2.y = -1;
  } else {
    player1.x -= 16;
    player2.x += 16;
  }
}

auto Justifier::data() -> uint2 {
  if(counter >= 32) return 1;

  if(counter == 0) {
    player1.trigger = platform->inputPoll(port, device, 0 + Trigger);
    player1.start   = platform->inputPoll(port, device, 0 + Start);
  }

  if(counter == 0 && chained) {
    player2.trigger = platform->inputPoll(port, device, 4 + Trigger);
    player2.start   = platform->inputPoll(port, device, 4 + Start);
  }

  switch(counter++) {
  case  0: return 0;
  case  1: return 0;
  case  2: return 0;
  case  3: return 0;
  case  4: return 0;
  case  5: return 0;
  case  6: return 0;
  case  7: return 0;
  case  8: return 0;
  case  9: return 0;
  case 10: return 0;
  case 11: return 0;

  case 12: return 1;  //signature
  case 13: return 1;  // ||
  case 14: return 1;  // ||
  case 15: return 0;  // ||

  case 16: return 0;
  case 17: return 1;
  case 18: return 0;
  case 19: return 1;
  case 20: return 0;
  case 21: return 1;
  case 22: return 0;
  case 23: return 1;

  case 24: return player1.trigger;
  case 25: return player2.trigger;
  case 26: return player1.start;
  case 27: return player2.start;
  case 28: return active;

  case 29: return 0;
  case 30: return 0;
  case 31: return 0;
  }

  unreachable;
}

auto Justifier::latch(bool data) -> void {
  if(latched == data) return;
  latched = data;
  counter = 0;
  if(latched == 0) active = !active;  //toggle between both controllers, even when unchained
}

auto Justifier::latch() -> void {
  if(!active) {
    player1.x = platform->inputPoll(port, device, 0 + X);
    player1.y = platform->inputPoll(port, device, 0 + Y);
    bool offscreen = (player1.x < 0 || player1.y < 0 || player1.x >= 256 || player1.y >= (int)ppu.vdisp());
    if(!offscreen) ppu.latchCounters(player1.x, player1.y);
  }
  else {
    player2.x = platform->inputPoll(port, device, 4 + X);
    player2.y = platform->inputPoll(port, device, 4 + Y);
    bool offscreen = (player2.x < 0 || player2.y < 0 || player2.x >= 256 || player2.y >= (int)ppu.vdisp());
    if(!offscreen) ppu.latchCounters(player2.x, player2.y);
  }
}
