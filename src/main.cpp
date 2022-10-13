#include <iostream>
#include "ncurses_display.hpp"
#include "game.hpp"

struct Flags get_flags(int argc, char* argsv[]) {
    struct Flags flags{false, "Player"};

    string debug = "--debug";
    string name = "--name";
    for (int arg = 1; arg < argc; arg++) {
        if (argsv[arg] == debug) {
          flags.debug = true;
        } else if (argsv[arg] == name && argc > arg + 1) {
          flags.pname = argsv[arg + 1];
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

