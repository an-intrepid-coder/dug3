#include <iostream>
#include "display.hpp"
#include "game.hpp"

struct Flags get_flags(int argc, char* argsv[]) {
    struct Flags flags{false};

    string debug = "--debug";
    for (int arg = 1; arg < argc; arg++) {
        if (argsv[arg] == debug) {
          flags.debug = true;
        } 
    }

    return flags;
};

int main(int argc, char* argsv[]) { 
  auto flags = get_flags(argc, argsv);

  init_curses();

  Game dug = Game(flags);
  dug.game_loop();

  uninit_curses();

  return 0;
}

