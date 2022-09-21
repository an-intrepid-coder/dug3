#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <ncurses.h>

typedef enum {
  WHITE_ON_BLACK,
  BLACK_ON_WHITE,
  YELLOW_ON_BLACK,
  BLUE_ON_BLACK,
  GREEN_ON_BLACK,
} Color_Pairs;

void init_curses();
void uninit_curses();

#endif

