#include <iostream>
#include "display.hpp"
#include "game.hpp"

int main() { // TODO: Args
  init_curses();

  Game dug = Game();
  dug.game_loop();

  uninit_curses();

  return 0;
}

