#include "game.hpp"

using std::string;
using std::to_string;
using std::cout;
using std::endl;

void Game::display_game_over(bool victory) {
  uninit_curses();
  Actor* player = this->get_player();
  if (victory) {
    cout << "\nYou have won! The Orb is yours!" << endl;
  } else {
    cout << "\nYou have fallen in the dungeon! Game Over." << endl;
  }
  cout << "You were on dungeon level " << this->level << endl;
  cout << "It was turn " << this->turn << endl;
  cout << "Your name was " << player->get_name() << endl;
  cout << "You had " << player->get_gold() << " gold!" << endl;
  cout << "You were level " << player->get_level() << endl;
  cout << "\nLast ten console messages:" << endl;
  for (auto i = 0; i < 10; i++) {
    int j = (int) this->log.size() - 1 - i;
    if (j >= 0 && j < (int) this->log.size()) {
      string msg = this->log[j];  
      cout << msg << endl;
    }
  }
  exit(0);
}


bool Game::toggle_displaying_distance_map() {
  this->displaying_distance_map = !this->displaying_distance_map;
  return this->displaying_distance_map;
}

void Game::display_title_screen() {
  int max_y;
  int max_x;
  getmaxyx(stdscr, max_y, max_x);
  erase();
  string title = "DUNGEON UNDER GRINDSTONE";
  string version = "< version 0.0.5 >";
  string prompt = "...any key to continue...";
  mvaddstr(max_y / 2 - 2, max_x / 2 - (int) title.size() / 2, title.c_str());
  mvaddstr(max_y / 2, max_x / 2 - (int) version.size() / 2, version.c_str());
  mvaddstr(max_y - 2, max_x / 2 - (int) prompt.size() / 2, prompt.c_str());
  refresh();
}

void Game::display_consumable_inventory() { // Consumables only
  erase();
  Actor* player = this->get_player();
  int n = 0;
  for (auto item : player->consumable_inventory) {
    char symbol = (char) ('a' + n);
    mvaddch(n, 0, symbol);
    string str = "). " + item.get_name();
    mvaddstr(n, 1, str.c_str());
    n++;
    if (n > INVENTORY_ROWS) {
      break;
    }
  }
  refresh();
}

void Game::display_scene() {
  erase();

  // Display tiles: 
  for (auto y = 0; y < MAP_HEIGHT; y++) {
    for (auto x = 0; x < MAP_WIDTH; x++) {
      Visibility vis = this->get_fov(y, x);
      if (vis == UNEXPLORED) {
        mvaddch(y, x, ' ');
      } else {
        if (vis == EXPLORED) {
          attron(COLOR_PAIR(BLUE_ON_BLACK));
        } else if (vis == VISIBLE) {
          attron(COLOR_PAIR(WHITE_ON_BLACK));
        }
        Terrain terrain = this->get_terrain(y, x);
        switch (terrain) {
          case NO_TERRAIN:
            mvaddch(y, x, ' ');
            break;
          case WALL:
            mvaddch(y, x, '#');
            break;
          case FLOOR:
            if (this->displaying_distance_map) { 
              char d = (char) ('a' + this->distance_map_player[y][x]);
              mvaddch(y, x, d);
            } else if (y == this->level_exit.y && x == this->level_exit.x) {
              if (this->level == FINAL_DLVL) {
                attron(COLOR_PAIR(GREEN_ON_BLACK));
                mvaddch(y, x, '&');
                attroff(COLOR_PAIR(GREEN_ON_BLACK));
              } else {
                mvaddch(y, x, '>');
              }
            } else {
              mvaddch(y, x, '.');
            }
            break;
        }
        if (vis == EXPLORED) {
          attroff(COLOR_PAIR(BLUE_ON_BLACK));
        } else if (vis == VISIBLE && terrain == FLOOR) {
          if (this->gold_map[y][x]) {
            attron(COLOR_PAIR(YELLOW_ON_BLACK));
            mvaddch(y, x, '*');
            attroff(COLOR_PAIR(YELLOW_ON_BLACK));
          } else if (this->loot_map[y][x]) {
            attron(COLOR_PAIR(YELLOW_ON_BLACK));
            mvaddch(y, x, '?');
            attroff(COLOR_PAIR(YELLOW_ON_BLACK));
          }
        }
      }
    }
  }

  // Display Actors:
  Actor* player = this->get_player();
  for (auto actor : this->actors) {
    Coord coord = Coord{actor.get_y(), actor.get_x()};
    if (can_see(player, coord)) {
      mvaddch(actor.get_y(), actor.get_x(), actor.get_symbol());
    }
  }

  // HUD stuff:
  string mp_str = "Move: " + to_string(player->get_movement_points());
  mvaddstr(0, MAP_WIDTH, mp_str.c_str());

  string loc_str = "Loc: (" + to_string(player->get_y()) + ", " + to_string(player->get_x()) + ")";
  mvaddstr(1, MAP_WIDTH, loc_str.c_str());

  string dlvl_str = "DLVL: " + to_string(this->level);
  mvaddstr(2, MAP_WIDTH, dlvl_str.c_str());
  
  string turn_str = "Turn: " + to_string(this->turn);
  mvaddstr(3, MAP_WIDTH, turn_str.c_str());

  string hp_str = "HP: " + to_string(player->get_health()) + "/" + to_string(player->get_max_health());
  mvaddstr(4, MAP_WIDTH, hp_str.c_str());

  string gold_str = "Gold: " + to_string(player->get_gold());
  mvaddstr(5, MAP_WIDTH, gold_str.c_str());

  string lvl_str = "LVL: " + to_string(player->get_level());
  mvaddstr(6, MAP_WIDTH, lvl_str.c_str());

  string xp_str = "XP: " + to_string(player->get_xp());
  mvaddstr(7, MAP_WIDTH, xp_str.c_str());

  int bonus_dmg = 0;
  for (auto i = 0; i < (int) player->bonuses.size(); i++) {
    Bonus bonus = player->bonuses[i];
    if (bonus.get_type() == EXTRA_DMG_BONUS) {
      bonus_dmg = bonus_dmg + bonus.get_amt();
    }
  }
  string dmg_str = "EXTRA DMG: " + to_string(bonus_dmg);
  mvaddstr(8, MAP_WIDTH, dmg_str.c_str());

  string auto_str = "AUTO: " + this->get_auto_str();
  mvaddstr(9, MAP_WIDTH, auto_str.c_str());

  // Console stuff:
  for (auto i = 0; i < CONSOLE_ROWS; i++) {
    int j = (int) this->log.size() - 1 - i;
    if (j >= 0 && j < (int) this->log.size()) {
      string msg = this->log[j];  
      mvaddstr(MAP_HEIGHT + i, 0, msg.c_str());
    }
  }

  refresh();
}

