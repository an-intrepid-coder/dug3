#include "display.hpp"

// Inits curses with desired options:
void init_curses() {
  initscr();
  start_color();
  noecho();
  cbreak();
  curs_set(0);
  keypad(stdscr, true);
  init_color(COLOR_BLACK, 0, 0, 0);
  init_color(COLOR_WHITE, 1000, 1000, 1000);
  init_color(COLOR_YELLOW, 1000, 1000, 0);
  init_pair(BLACK_ON_WHITE, COLOR_BLACK, COLOR_WHITE);
  init_pair(WHITE_ON_BLACK, COLOR_WHITE, COLOR_BLACK);
  init_pair(YELLOW_ON_BLACK, COLOR_YELLOW, COLOR_BLACK);
}

// Resets the terminal to pre-curses settings:
void uninit_curses() {
  echo();
  nocbreak();
  curs_set(1);
  keypad(stdscr, false);
  endwin();
}

