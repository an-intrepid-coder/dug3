#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <ncurses.h>

typedef enum {
  WHITE_ON_BLACK,
  BLACK_ON_WHITE,
  YELLOW_ON_BLACK
  // TODO: More colors as the game gets more complex
} Color_Pairs;

void init_curses();
void uninit_curses();

#endif

