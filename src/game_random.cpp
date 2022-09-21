#include "game.hpp"

int Game::roll_dx(int x) {
  return (int) (this->rng() % x + 1);
}

int Game::roll_xdy(int x, int y) {
  int k = 0;
  for (auto i = 0; i < x; i++) {
    k = k + this->roll_dx(y);
  }
  return k;
}

