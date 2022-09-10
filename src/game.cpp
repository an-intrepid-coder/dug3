#include <thread>
#include <chrono>
#include <queue>
#include <numeric>
#include <string>
#include <iostream>
#include "game.hpp"

using std::cout;
using std::endl;
using std::string;
using std::to_string;
using std::vector;
using std::priority_queue;

using namespace std::chrono_literals;

Game::Game() {
  this->interface_mode = MAIN_GAME;
  this->turn = 0;
  this->level = 1;
  this->rng = std::mt19937_64();
  this->rng.seed(time(NULL)); 
  this->actors = vector<Actor>();
  this->actors.push_back(Player(0, 0));
  this->generate_level(1);
}

void Game::gold_check() {
  Actor* player = this->get_player();
  if (this->gold_map[player->get_y()][player->get_x()]) {
    int gold = gold_amt(this->level);
    player->change_gold(gold);
    string log_str = player->get_name() + " picked up " + to_string(gold) + " gold.";
    this->log.push_back(log_str);
    this->gold_map[player->get_y()][player->get_x()] = false;
  }
}

void Game::loot_check() {
  Actor* player = this->get_player();
  if (this->loot_map[player->get_y()][player->get_x()]) {
    Consumable loot = generate_random_consumable(this->level);

    string log_str = player->get_name() + " picked up " + loot.get_name();
    this->log.push_back(log_str);
    
    player->add_consumable(loot);


    this->loot_map[player->get_y()][player->get_x()] = false;
  }
}

void Game::generate_level(int level) { // TODO: More args
  // Dungeon map setup:
  this->terrain_map_generated = false;
  this->distance_map = vector<vector<int>>();
  this->displaying_distance_map = false;

  this->generate_map_room_accretion();
  this->terrain_map_generated = true;

  this->fov_map = vector<vector<Visibility>>();
  this->gold_map = vector<vector<bool>>();
  this->loot_map = vector<vector<bool>>();

  for (auto y = 0; y < MAP_HEIGHT; y++) {
    this->fov_map.push_back(vector<Visibility>());
    this->gold_map.push_back(vector<bool>());
    this->loot_map.push_back(vector<bool>());

    for (auto x = 0; x < MAP_WIDTH; x++) {
      this->fov_map[y].push_back(UNEXPLORED);

      if (this->roll_dx(LOOT_DIE) <= GOLD_CHANCE) {
        this->gold_map[y].push_back(true);
      } else {
        this->gold_map[y].push_back(false);
      }

      if (this->roll_dx(LOOT_DIE) <= LOOT_CHANCE) {
        this->loot_map[y].push_back(true);
      } else {
        this->loot_map[y].push_back(false);
      }
    }
  }

  // Spawn player and potentially some items:
  Actor* player = this->get_player();
  Coord spawn = this->get_spawn_loc();
  player->set_y(spawn.y);
  player->set_x(spawn.x);
  if (level == 1) {
    player->add_consumable(MinorHealingPotion());
  }

  // Spawn some enemies:
  switch (level) {
    case 1: // 10 fungoids and 10 slimes
      for (auto i = 0; i < 10; i++) {
        Coord spawn2 = this->get_spawn_loc();
        this->actors.push_back(Slime(spawn2.y, spawn2.x));
        Coord spawn3 = this->get_spawn_loc();
        this->actors.push_back(Fungoid(spawn3.y, spawn3.x));
      }
      break;
    case 2: // 12 Fungoids, 8 Slimes 
      for (auto i = 0; i < 12; i++) {
        Coord spawn2 = this->get_spawn_loc();
        this->actors.push_back(Fungoid(spawn2.y, spawn2.x));
        if (i < 8) {
          Coord spawn3 = this->get_spawn_loc();
          this->actors.push_back(Slime(spawn3.y, spawn3.x));
        }
      }
      break;
    case 3: // 16 Fungoids, 2 slimes
      for (auto i = 0; i < 16; i++) {
        Coord spawn2 = this->get_spawn_loc();
        this->actors.push_back(Fungoid(spawn2.y, spawn2.x));
        if (i < 2) {
          Coord spawn3 = this->get_spawn_loc();
          this->actors.push_back(Slime(spawn3.y, spawn3.x));
        }
      }
      break;
    case 4: // 8 Fungoids, 2 Trolls, Bugbear
      for (auto i = 0; i < 8; i++) {
        Coord spawn2 = this->get_spawn_loc();
        this->actors.push_back(Fungoid(spawn2.y, spawn2.x));
        if (i < 2) {
          Coord spawn3 = this->get_spawn_loc();
          this->actors.push_back(Troll(spawn3.y, spawn3.x));
        }
        if (i < 1) {
          Coord spawn4 = this->get_spawn_loc();
          this->actors.push_back(Bugbear(spawn4.y, spawn4.x));
        }
      }
      break;
    case 5: // 6 Trolls, 6 Fungoids, Bugbear
      for (auto i = 0; i < 8; i++) {
        Coord spawn2 = this->get_spawn_loc();
        this->actors.push_back(Fungoid(spawn2.y, spawn2.x));
        Coord spawn3 = this->get_spawn_loc();
        this->actors.push_back(Troll(spawn3.y, spawn3.x));
        if (i < 1) {
          Coord spawn4 = this->get_spawn_loc();
          this->actors.push_back(Bugbear(spawn4.y, spawn4.x));
        }
      }
      break;
    case 6: // 8 Trolls, 10 Fungoids, Bugbear
      for (auto i = 0; i < 10; i++) {
        Coord spawn2 = this->get_spawn_loc();
        this->actors.push_back(Fungoid(spawn2.y, spawn2.x));
        if (i < 8) {
          Coord spawn3 = this->get_spawn_loc();
          this->actors.push_back(Troll(spawn3.y, spawn3.x));
        }
        if (i < 1) {
          Coord spawn4 = this->get_spawn_loc();
          this->actors.push_back(Bugbear(spawn4.y, spawn4.x));
        }
      }
      break;
  }

  // Spawn the level exit:
  vector<Coord> exit_spawn = this->get_all_terrain(FLOOR);
  int j = (int) (this->rng() % exit_spawn.size());
  this->level_exit = exit_spawn[j];

  // Finalize map setup:
  this->dijkstra_map_distance(spawn);
  this->calculate_fov();
}

Consumable Game::generate_random_consumable(int level) {
  auto vec = vector<Consumable>();

  vec.push_back(MinorHealingPotion());
  vec.push_back(ExtraDamagePotion());

  int i = (int) (this->rng() % vec.size()); 

  return vec[i];
  // TODO: More items & level variation in this function
}

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

// Returns true if the item was successfully used:
bool Game::use_consumable(Consumable* item, Actor* user) {
  // TODO: Targetable items, such as wands & scrolls
  ItemEffect effect = item->get_effect();
  if (item->get_charges() > 0) {
    if (effect == MINOR_HEALING) {
      int heal_amt = 10 + this->roll_dx(8);
      user->change_health(heal_amt);
      string log_str = user->get_name() + " healed for " + to_string(heal_amt);
      this->log.push_back(log_str);
      if (item->change_charges(-1) == 0) {
        user->remove_consumable(item);
      }
      return true;
    } else if (effect == EXTRA_DMG_EFFECT) {
      int duration = this->roll_xdy(3, 6);
      auto bonus = Bonus{EXTRA_DMG_BONUS, true, duration};
      user->bonuses.push_back(bonus);
      string log_str = user->get_name() + " drinks a " + item->get_name() + ".";
      this->log.push_back(log_str);
      if (item->change_charges(-1) == 0) {
        user->remove_consumable(item);
      }
    }
  }
  return false;
}

Visibility Game::get_fov(int y, int x) {
  return this->fov_map[y][x];
}

void Game::calculate_fov() { // TODO: Args
  for (auto y = 0; y < MAP_HEIGHT; y++) {
    for (auto x = 0; x < MAP_WIDTH; x++) {
      Visibility vis = this->fov_map[y][x];
      if (vis == VISIBLE) {
        this->fov_map[y][x] = EXPLORED;
      }
    }
  }

  Actor* player = this->get_player();
  Coord player_loc = Coord{player->get_y(), player->get_x()};
  for (auto y = player_loc.y - VISION_RADIUS; y <= player_loc.y + VISION_RADIUS; y++) {
    for (auto x = player_loc.x - VISION_RADIUS; x <= player_loc.x + VISION_RADIUS; x++) {
      if (y >= 0 && y < MAP_HEIGHT && x >= 0 && x < MAP_WIDTH) {
        Coord coord = Coord{y, x};
        if (can_see(player, coord)) {
          this->fov_map[y][x] = VISIBLE;
        }
      }
    }
  }
}

bool Game::toggle_displaying_distance_map() {
  this->displaying_distance_map = !this->displaying_distance_map;
  return this->displaying_distance_map;
}

Actor* Game::get_player() {
  for (long unsigned int i = 0; i < this->actors.size(); i++) {
    Actor* actor = &this->actors[i];
    if (actor->get_is_player()) {
      return actor;
    }
  }
  return nullptr;
}

Coord Game::get_spawn_loc() {
  if (!this->terrain_map_generated) {
    // TODO: Exception handling
    uninit_curses();
    exit(1);
  }
  vector<Coord> results = vector<Coord>();
  for (auto y = 0; y < MAP_HEIGHT; y++) {
    for (auto x = 0; x < MAP_WIDTH; x++) {
      Terrain terrain = this->get_terrain(y, x);
      if (terrain == FLOOR) {
        bool valid = true;
        for (auto actor : this->actors) {
          if (actor.get_y() == y && actor.get_x() == x) {
            valid = false;
            break;
          }
        }
        if (valid) {
          results.push_back(Coord{y, x});
        }
      }
    }
  }
  auto i = this->rng() % results.size();
  return results[i];
}

void Game::clear_dead() {
  int i = 0;
  for (auto it = this->actors.begin(); it != this->actors.end(); i++) {
    if (!this->actors[i].is_alive() && !this->actors[i].get_is_player()) {
      this->actors.erase(it);
    } else {
      ++it;
    }
  }
}

// Returns true if the Actor was able to move:
bool Game::move_actor(Actor* actor, int dy, int dx) {
  Coord actor_loc = Coord{actor->get_y(), actor->get_x()};
  int ty = actor->get_y() + dy;
  int tx = actor->get_x() + dx;
  Terrain terrain = this->get_terrain(ty, tx);
  bool blocked = false;
  // Check for actors and initiate combat if needed:
  for (auto i = 0; i < (int) this->actors.size(); i++) {
    Actor* actor2 = &this->actors[i];
    Coord actor2_loc = Coord{actor2->get_y(), actor2->get_x()};
    if (actor2_loc.y == ty && 
        actor2_loc.x == tx && 
        (actor->get_is_player() || actor2->get_is_player()) &&
        // NOTE: ^ For now, enemies don't attack each other. Will add a
        //       toggle for that soon (confused enemies, berserkers, etc.).
        actor != actor2) {
      int dmg = actor->get_dmg(this->rng);
      actor2->change_health(-dmg);
      Actor* player = this->get_player();
      if (can_see(player, actor_loc) && can_see(player, actor2_loc)) {
        string dmg_log = actor->get_name() + 
                         " hits " + actor2->get_name() + 
                         " for " + to_string(dmg) + 
                         " dmg! (" + to_string(actor2->get_health()) + 
                         "/" + to_string(actor2->get_max_health()) + ")";
        this->log.push_back(dmg_log);
        if (!actor2->is_alive()) {
          actor->award_xp(actor2->get_xp_worth());
        }
      }
      return true;
    } else if (actor2_loc.y == ty &&
               actor2_loc.x == tx &&
               actor != actor2) {
      blocked = true;
    }
  }
  if (!blocked) {
    // Move the actor:
    if (terrain == FLOOR) {
      actor->set_y(ty);
      actor->set_x(tx);
      // Assess the cost (diagonal is effectively 2 moves):
      if (dy != 0 && dx != 0) {
        actor->change_movement_points(-2);
      } else {
        actor->change_movement_points(-1);
      }
      return true;
    }
  }
  return false;
}

// Returns true if the player used their turn; false otherwise.
bool Game::handle_consumable_inventory_input() {
  flushinp();
  auto input = getch();
  Actor* player = this->get_player();
  if (input >= 97 || input <= 120) { // ASCII a - x
    int i = input - 97;
    if (i < (int) player->consumable_inventory.size()) {
      this->interface_mode = MAIN_GAME;
      Consumable* item = &player->consumable_inventory[i];
      return this->use_consumable(item, player);
    }
  }
  return false;
}

bool Game::handle_drop_consumable_input() {
  flushinp();
  auto input = getch();
  Actor* player = this->get_player();
  if (input >= 97 || input <= 120) { // ASCII a - x
    int i = input - 97;
    if (i < (int) player->consumable_inventory.size()) {
      this->interface_mode = MAIN_GAME;
      Consumable* item = &player->consumable_inventory[i];
      //return this->use_consumable(item, player);
      player->remove_consumable(item);
      return true;
    }
  }
  return false;
}


// Returns true if the player used their turn; false otherwise.
bool Game::handle_input() {
  flushinp();
  auto input = getch();
  Actor* player = this->get_player();
  switch(input) {
    case 'Q': // Quit
      uninit_curses();
      exit(0);
      break;
    // Movement keys:
    case 'h':
      return this->move_actor(player, 0, -1);
      break;
    case 'j':
      return this->move_actor(player, 1, 0);
      break;
    case 'k':
      return this->move_actor(player, -1, 0);
      break;
    case 'l':
      return this->move_actor(player, 0, 1);
      break;
    case 'y':
      return this->move_actor(player, -1, -1);
      break;
    case 'u':
      return this->move_actor(player, -1, 1);
      break;
    case 'b':
      return this->move_actor(player, 1, -1);
      break;
    case 'n':
      return this->move_actor(player, 1, 1);
      break;
    case '.':
      return this->move_actor(player, 0, 0);
      break;
    case '>':
      if (player->get_y() == this->level_exit.y &&
          player->get_x() == this->level_exit.x) {
        if (this->level == FINAL_DLVL) {
          this->game_over(true);
        } else {
          this->level++;
          this->generate_level(this->level);
        }
      }
      break;
    case 'D':
      // Overlay Djikstra distance: (Debug cmd!)
      this->dijkstra_map_distance(Coord{player->get_y(), player->get_x()});
      this->toggle_displaying_distance_map();
      break;
    case 'd':
      // Drop consumables menu: 
      // TODO: Drop anything menu
      // TODO: Persistent items (currently destroyed on drop)
      this->interface_mode = DROP_CONSUMABLE;
      break;
    case 'R':
      // Reveal the map. (Debug cmd!)
      for (auto y = 0; y < MAP_HEIGHT; y++) {
        for (auto x = 0; x < MAP_WIDTH; x++) {
          this->fov_map[y][x] = VISIBLE;
        }
      }
      break;
    case 'i': // Inventory menu
      this->interface_mode = CONSUMABLE_INVENTORY;
      break;
    default:
      break;
  }
  return false;
}

void Game::game_over(bool victory) {
  uninit_curses();
  Actor* player = this->get_player();
  if (victory) {
    cout << "You have won! The Orb is yours!" << endl;
  } else {
    cout << "You have fallen in the dungeon! Game Over." << endl;
  }
  cout << "You were on dungeon level " << this->level << endl;
  cout << "It was turn " << this->turn << endl;
  cout << "Your name was " << player->get_name() << endl;
  cout << "You had " << player->get_gold() << " gold!" << endl;
  cout << "You were level " << player->get_level() << endl;
  cout << "Last ten console messages:" << endl;
  for (auto i = 0; i < 10; i++) {
    int j = (int) this->log.size() - 1 - i;
    if (j >= 0 && j < (int) this->log.size()) {
      string msg = this->log[j];  
      cout << msg << endl;
    }
  }
  exit(0);
}

void Game::title_screen() {
  int max_y;
  int max_x;
  getmaxyx(stdscr, max_y, max_x);
  erase();
  string title = "DUNGEON UNDER GRINDSTONE";
  string version = "< version 0.0.1 >";
  string prompt = "...any key to continue...";
  mvaddstr(max_y / 2 - 2, max_x / 2 - (int) title.size() / 2, title.c_str());
  mvaddstr(max_y / 2, max_x / 2 - (int) version.size() / 2, version.c_str());
  mvaddstr(max_y - 2, max_x / 2 - (int) prompt.size() / 2, prompt.c_str());
  refresh();
}

void Game::game_loop() {
  this->title_screen();
  getch();
  for (;;) {
    Actor* player = this->get_player();
    InterfaceMode mode = this->interface_mode;

    // Display according to game's current mode:
    if (mode == MAIN_GAME) {
      this->display_scene();
    } else if (mode == CONSUMABLE_INVENTORY) {
      this->display_consumable_inventory();
    } else if (mode == DROP_CONSUMABLE) {
      this->display_consumable_inventory();
    }

    // Take input based on the game's current mode:
    bool turn_taken = false;
    if (mode == MAIN_GAME) {
      if (player->get_movement_points() == 1) {
        turn_taken = this->handle_input();
      } else if (player->get_movement_points() < 1) {
        turn_taken = true;
      }
    } else if (mode == CONSUMABLE_INVENTORY) {
      turn_taken = this->handle_consumable_inventory_input();
    } else if (mode == DROP_CONSUMABLE) {
      turn_taken = this->handle_drop_consumable_input();
    }

    // Process a game turn if input resulted in a turn taken:
    if (turn_taken) { 
      this->gold_check();
      this->loot_check();
      player->change_movement_points(1);
      this->run_behavior();
      if (!player->is_alive()) {
        break;
      }
      this->clear_dead();
      this->bonus_check();
      this->turn++;
      Coord coord = Coord{player->get_y(), player->get_x()};
      this->dijkstra_map_distance(coord);
      this->calculate_fov();
    }
    
    // About 30 FPS
    std::this_thread::sleep_for(33ms);
  } 
  this->game_over(false);
}

void Game::bonus_check() {
  for (auto actor : actors) { 
    int i = 0;
    for (auto it = actor.bonuses.begin(); it != actor.bonuses.end(); i++) {
      if (actor.bonuses[i].temp) {
        actor.bonuses[i].duration--;
      }
      if (actor.bonuses[i].temp && actor.bonuses[i].duration <= 0) {
        actor.bonuses.erase(it);
      } else {
        ++it;
      }
    }
  }
}

Terrain Game::get_terrain(int y, int x) {
  if (y < 0 || x < 0 || y >= MAP_HEIGHT || x >= MAP_WIDTH) {
    return NO_TERRAIN;
  } else {
    return this->terrain_map[y][x];
  }
}

vector<Coord> Game::get_all_terrain(Terrain type) {
  auto vec = vector<Coord>();
  for (auto y = 0; y < MAP_HEIGHT; y++) {
    for (auto x = 0; x < MAP_WIDTH; x++) {
      if (this->terrain_map[y][x] == type) {
        vec.push_back(Coord{y, x});
      }
    }
  }
  return vec;
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

int Game::gold_amt(int level) {
  int gold_max = 100;
  int gold_min = 20;
  // ^ Tentative
  int amt;
  for (auto l = 0; l < level; l++) {
    amt = amt + (int) (rng() % (gold_max - gold_min) + gold_min);
  }
  return amt;
}

void Game::display_scene() {
  erase();

  // Display tiles: (FOV TODO)
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
              char d = (char) ('a' + this->distance_map[y][x]);
              mvaddch(y, x, d);
            } else if (y == this->level_exit.y && x == this->level_exit.x) {
              if (this->level == FINAL_DLVL) {
                mvaddch(y, x, '!');
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

// Generates an open square map surrounded by four walls, with pillars.
void Game::generate_test_map() {
  this->terrain_map = vector<vector<Terrain>>();
  for (auto y = 0; y < MAP_HEIGHT; y++) {
    this->terrain_map.push_back(vector<Terrain>());
    for (auto x = 0; x < MAP_WIDTH; x++) {
      if (y == 0 || x == 0 || y == MAP_HEIGHT - 1 || x == MAP_WIDTH - 1) {
        this->terrain_map[y].push_back(WALL);
      } else {
        this->terrain_map[y].push_back(FLOOR);
      }
    }
  }
  for (auto i = 0; i < 20; i++) {
    int y = (int) (rng() % (MAP_HEIGHT - 1) + 1);
    int x = (int) (rng() % (MAP_WIDTH - 1) + 1);
    this->terrain_map[y][x] = WALL;
  }
}

// Generates rooms with binary space partition:
void Game::generate_map_room_accretion() {
  // Fill the terrain map initially with walls:
  this->terrain_map = vector<vector<Terrain>>();
  for (auto y = 0; y < MAP_HEIGHT; y++) {
    this->terrain_map.push_back(vector<Terrain>());
    for (auto x = 0; x < MAP_WIDTH; x++) {
      this->terrain_map[y].push_back(WALL);
    }
  }

  // Stamp out rooms one-by-one:
  auto rooms = vector<Rect>();

  int min_room_height = 2;
  int max_room_height = 4;
  int min_room_width = 3;
  int max_room_width = 6;

  int total_map_space = MAP_HEIGHT * MAP_WIDTH;
  int used_map_space = 0;

  //for (auto r = 0; r < 3; r++) { 
  for (;;) {
    // Generate room dimensions:
    int y = (int) (rng() % (MAP_HEIGHT - max_room_height - 2) + 1);
    int x = (int) (rng() % (MAP_WIDTH - max_room_width - 2) + 1);
    int h = (int) (rng() % (max_room_height - min_room_height) + min_room_height);
    int w = (int) (rng() % (max_room_width - min_room_width) + min_room_width);
    Rect room = Rect{Coord{y, x}, h, w};
  
    // Check for conflicting rooms:
    bool bad_placement = false;
    for (auto rect : rooms) {
      if (rects_intersect(room, rect)) {
        bad_placement = true;
        break;
      }
    }
    if (bad_placement) {
      continue;
    }

    rooms.push_back(room);

    // Stamp room:
    for (auto y = room.top_left.y; y < room.top_left.y + room.height; y++) {
      for (auto x = room.top_left.x; x < room.top_left.x + room.width; x++) {
        this->terrain_map[y][x] = FLOOR;
        used_map_space++;
      }
    }

    // Connect room to previous room:
    if (rooms.size() > 1) {
      Coord start = rect_center(room);
      Coord goal = rect_center(rooms[rooms.size() - 2]);
      vector<Coord> hall = bresenham_line(start, goal);
      for (auto coord : hall) {
        Terrain terrain = this->terrain_map[coord.y][coord.x];
        if (terrain == WALL) {
          this->terrain_map[coord.y][coord.x] = FLOOR;
          used_map_space++;
        } else if (terrain == FLOOR && !rect_contains(room, coord)) {
          break;
        }
      }
    }

    // Break condition:
    if (used_map_space > total_map_space / 3) {
      break;
    }
  }
}

void Game::run_behavior() {
  for (auto i = 0; i < (int) this->actors.size(); i++) {
    Actor* actor = &this->actors[i];
    if (actor->get_behavior() == NO_BEHAVIOR || !actor->is_alive()) {
      continue;
    }
    actor->change_movement_points(1);
    if (actor->get_movement_points() == 1) {
      Actor* player = this->get_player();
      Behavior behavior = actor->get_behavior();
      if (behavior == OBLIVIOUS_WANDERER) {
        // Wander in a random direction, with no regard for the
        // player. Attack if they are in the targeted square.
        int dy = this->rng() % 2;
        if (this->rng() % 2 == 0) {
          dy = dy * -1;
        }
        int dx = this->rng() % 2;
        if (this->rng() % 2 == 0) {
          dx = dx * -1;
        }
        this->move_actor(actor, dy, dx);
      } else if (behavior == SEEKING_HUNTER) {
        /* Rolls "down-hill" towards the player, regardless of 
           FOV.  */
        Coord coord = Coord{actor->get_y(), actor->get_x()};
        Coord dest = this->downhill_from(coord);
        int dy = dest.y - actor->get_y();
        int dx = dest.x - actor->get_x();
        this->move_actor(actor, dy, dx);
      } else if (behavior == WAITING_HUNTER) {
        Coord player_loc = Coord{player->get_y(), player->get_x()};
        if (can_see(actor, player_loc)) {
          Coord coord = Coord{actor->get_y(), actor->get_x()};
          Coord dest = this->downhill_from(coord);
          int dy = dest.y - actor->get_y();
          int dx = dest.x - actor->get_x();
          this->move_actor(actor, dy, dx);
        }
      }
      // TODO: Other behavior types
    }
  }
}

Coord Game::downhill_from(Coord coord) {
  // TODO: Account for actors in the way
  vector<Coord> vec = get_neighbors(coord);
  vector<int> vec2 = vector<int>();
  for (auto coord : vec) {
    vec2.push_back(this->distance_map[coord.y][coord.x]);
  }
  int low = INT_MAX;
  for (auto i = 0; i < (int) vec2.size(); i++) {
    if (vec2[i] < low) {
      low = vec2[i];
    }
  }
  vector<Coord> vec3 = vector<Coord>();
  for (auto i = 0; i < (int) vec2.size(); i++) {
    if (vec2[i] == low) {
      vec3.push_back(vec[i]);
    }
  }
  int j = (int) (this->rng() % vec3.size());
  Coord dest = vec3[j];
  if (vec3.empty()) {
    return coord;
  }
  return dest;
}

bool Game::can_see(Actor* viewer, Coord goal) {
  Coord start = Coord{viewer->get_y(), viewer->get_x()};
  if (get_chebyshev_distance(start, goal) > VISION_RADIUS) {
  // TODO: ^ actor modifier
    return false;
  }
  vector<Coord> sight_line = bresenham_line(start, goal);
  for (auto i = 0; i < (int) sight_line.size() - 1; i++) {
    Coord coord = sight_line[i];
    Terrain terrain = this->terrain_map[coord.y][coord.x];
    if (terrain == WALL) {
      return false;
    }
  }
  return true;
}

void Game::dijkstra_map_distance(Coord start) {
  this->distance_map = vector<vector<int>>();
  vector<vector<int>> distance = vector<vector<int>>();
  vector<vector<vector<Coord>>> previous = vector<vector<vector<Coord>>>(); 
  PriorityQueueCoord pq = PriorityQueueCoord(MAP_HEIGHT * MAP_WIDTH);

  for (auto y = 0; y < MAP_HEIGHT; y++) {
    distance.push_back(vector<int>());
    previous.push_back(vector<vector<Coord>>());
    for (auto x = 0; x < MAP_WIDTH; x++) {
      if (!(start.y == y && start.x == x)) {
        distance[y].push_back(INT_MAX);
      } else {
        distance[y].push_back(0);
      }
      previous[y].push_back(vector<Coord>());
      pq.insert(Coord{y, x}, distance[y][x]);
    }
  }

  while (pq.get_size() != 0) {
    Coord next = pq.extract_min();
    vector<Coord> neighbors = get_neighbors(next);
    for (auto coord : neighbors) {
      if (pq.contains(coord)) {
        int alt = distance[next.y][next.x] + get_distance(next, coord);
        if (alt < distance[coord.y][coord.x]) {
          distance[coord.y][coord.x] = alt;
          previous[coord.y][coord.x].push_back(next);
          pq.change_priority_by_coord(coord, alt);
        }
      }
    }
  }

  for (auto y = 0; y < MAP_HEIGHT; y++) {
    this->distance_map.push_back(vector<int>());
    for (auto x = 0; x < MAP_WIDTH; x++) {
      this->distance_map[y].push_back(distance[y][x]);
    }
  }
  this->distance_map_generated = true;
  /* TODO: Backtrack to the origin for the shortest path(s). Currently
           not set up to do that yet.  */
}


