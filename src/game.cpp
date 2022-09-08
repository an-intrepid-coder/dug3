#include <thread>
#include <chrono>
#include <queue>
#include <numeric>
#include <string>
#include "game.hpp"

using std::to_string;
using std::vector;
using std::priority_queue;

using namespace std::chrono_literals;

Game::Game() {
  this->interface_mode = MAIN_GAME;
  this->turn = 0;
  this->rng = std::mt19937_64();
  this->rng.seed(time(NULL));

  // Begin map setup:
  this->terrain_map_generated = false;
  this->distance_map = vector<vector<int>>();
  this->displaying_distance_map = false;
  this->generate_test_map();
  this->terrain_map_generated = true;
  this->fov_map = vector<vector<Visibility>>();
  for (auto y = 0; y < MAP_HEIGHT; y++) {
    this->fov_map.push_back(vector<Visibility>());
    for (auto x = 0; x < MAP_WIDTH; x++) {
      this->fov_map[y].push_back(UNEXPLORED);
    }
  }

  // Spawn player:
  this->actors = vector<Actor>();
  Coord spawn = this->get_spawn_loc();
  this->actors.push_back(Player(spawn.y, spawn.x));

  // Spawn some test enemies:
  Coord spawn2 = this->get_spawn_loc();
  this->actors.push_back(Slime(spawn2.y, spawn2.x));
  Coord spawn3 = this->get_spawn_loc();
  this->actors.push_back(Bugbear(spawn3.y, spawn3.x));
  Coord spawn4 = this->get_spawn_loc();
  this->actors.push_back(Troll(spawn4.y, spawn4.x));

  // Spawn some items:
  Actor* player = this->get_player();
  player->add_consumable(MinorHealingPotion());
  player->add_consumable(MinorHealingPotion());
  player->add_consumable(MinorHealingPotion());

  // Finalize map setup:
  this->dijkstra_map_distance(spawn);
  this->calculate_fov();
}

int Game::roll_d8() {
  return this->rng() % 8 + 1;
}

// Returns true if the item was successfully used:
bool Game::use_consumable(Consumable* item, Actor* user) {
  // TODO: Targetable items, such as wands & scrolls
  ItemEffect effect = item->get_effect();
  if (item->get_charges() > 0) {
    if (effect == MINOR_HEALING) {
      // Tentative:
      int heal_amt = 10 + this->roll_d8();
      user->change_health(heal_amt);
      string log_str = user->get_name() + " healed for " + to_string(heal_amt);
      this->log.push_back(log_str);
      if (item->change_charges(-1) == 0) {
        user->remove_consumable(item);
      }
      return true;
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
    if (!this->actors[i].is_alive()) {
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
      // TODO: Advanced combat -- all dmg is 1 for now
      int dmg = 1;
      actor2->change_health(-dmg);
      Actor* player = this->get_player();
      if (can_see(player, actor_loc) && can_see(player, actor2_loc)) {
        string dmg_log = actor->get_name() + 
                         " hits " + actor2->get_name() + 
                         " for " + to_string(dmg) + 
                         " dmg! (" + to_string(actor2->get_health()) + 
                         "/" + to_string(actor2->get_max_health()) + ")";
        this->log.push_back(dmg_log);
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
bool Game::handle_inventory_input() {
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
    case 'D':
      // Overlay Djikstra distance: (Debug cmd!)
      this->dijkstra_map_distance(Coord{player->get_y(), player->get_x()});
      this->toggle_displaying_distance_map();
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
      this->interface_mode = INVENTORY;
      break;
    default:
      break;
  }
  return false;
}

void Game::game_loop() {
  for (;;) {
    Actor* player = this->get_player();
    InterfaceMode mode = this->interface_mode;

    if (mode == MAIN_GAME) {
      this->display_scene();
    } else if (mode == INVENTORY) {
      this->display_inventory(); // <-- TODO
    }

    bool turn_taken = false;
    if (mode == MAIN_GAME) {
      if (player->get_movement_points() == 1) {
        turn_taken = this->handle_input();
      } else if (player->get_movement_points() < 1) {
        turn_taken = true;
      }
    } else if (mode == INVENTORY) {
      turn_taken = this->handle_inventory_input(); // TODO
    }
    if (turn_taken) { 
      player->change_movement_points(1);
      this->run_behavior();
      if (!player->is_alive()) {
        break;
      }
      this->clear_dead();
      this->turn++;
      Coord coord = Coord{player->get_y(), player->get_x()};
      this->dijkstra_map_distance(coord);
      this->calculate_fov();
    }
    
    // About 30 FPS
    std::this_thread::sleep_for(33ms);
  } 
  // TODO: End game and hi-score
}

Terrain Game::get_terrain(int y, int x) {
  if (y < 0 || x < 0 || y >= MAP_HEIGHT || x >= MAP_WIDTH) {
    return NO_TERRAIN;
  } else {
    return this->terrain_map[y][x];
  }
}

void Game::display_inventory() { // Consumables only
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
            } else {
              mvaddch(y, x, '.');
            }
            break;
        }
        if (vis == EXPLORED) {
          attroff(COLOR_PAIR(BLUE_ON_BLACK));
        }
      }
    }
  }

  // Display Actors: (FOV TODO)
  Actor* player = this->get_player();
  //Coord player_loc = Coord{player->get_y(), player->get_x()};
  for (auto actor : this->actors) {
    Coord coord = Coord{actor.get_y(), actor.get_x()};
    if (can_see(player, coord)) {
      mvaddch(actor.get_y(), actor.get_x(), actor.get_symbol());
    }
  }

  // HUD stuff:
  std::string loc_str = "Loc: (" + to_string(player->get_y()) + ", " + to_string(player->get_x()) + ")";
  mvaddstr(0, MAP_WIDTH, loc_str.c_str());
  
  std::string turn_str = "Turn: " + to_string(this->turn);
  mvaddstr(1, MAP_WIDTH, turn_str.c_str());

  std::string mp_str = "Move: " + to_string(player->get_movement_points());
  mvaddstr(2, MAP_WIDTH, mp_str.c_str());

  std::string hp_str = "HP: " + to_string(player->get_health()) + "/" + to_string(player->get_max_health());
  mvaddstr(3, MAP_WIDTH, hp_str.c_str());

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
}

void Game::run_behavior() {
  for (auto i = 0; i < (int) this->actors.size(); i++) {
    Actor* actor = &this->actors[i];
    if (actor->get_behavior() == NO_BEHAVIOR) {
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


