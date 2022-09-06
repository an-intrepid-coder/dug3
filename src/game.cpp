#include <thread>
#include <chrono>
#include <numeric>
#include "game.hpp"

using std::to_string;
using std::vector;

using namespace std::chrono_literals;

Game::Game() {
  this->map_generated = false;

  this->rng = std::mt19937_64();
  this->rng.seed(time(NULL));

  this->turn = 0;

  this->generate_test_map();
  this->map_generated = true;

  this->actors = vector<Actor>();
  Coord spawn = this->get_spawn_loc();
  this->actors.push_back(Player(spawn.y, spawn.x));
  Coord spawn2 = this->get_spawn_loc();
  this->actors.push_back(WanderBot(spawn2.y, spawn2.x));
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
  if (!this->map_generated) {
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
  int ty = actor->get_y() + dy;
  int tx = actor->get_x() + dx;
  Terrain terrain = this->get_terrain(ty, tx);
  //for (auto actor2 : this->actors) {
  for (auto i = 0; i < (int) this->actors.size(); i++) {
    Actor* actor2 = &this->actors[i];
    if (actor2->get_y() == ty && actor2->get_x() == tx && actor != actor2) {
      // TODO: Advanced combat -- all dmg is 1 for now
      int dmg = 1;
      actor2->change_health(-dmg);
      string dmg_log = actor->get_name() + 
                       " hits " + actor2->get_name() + 
                       " for " + to_string(dmg) + 
                       " dmg! (" + to_string(actor2->get_health()) + 
                       "/" + to_string(actor2->get_max_health()) + ")";
      this->log.push_back(dmg_log);
      return true;
    }
  }
  if (terrain == FLOOR) {
    actor->set_y(ty);
    actor->set_x(tx);
    return true;
  }
  return false;
}

// Returns true if the player used their turn; false otherwise.
bool Game::handle_input() {
  auto input = getch();
  switch(input) {
    case 'Q':
      uninit_curses();
      exit(0);
      break;
    case 'h':
      return this->move_actor(this->get_player(), 0, -1);
      break;
    case 'j':
      return this->move_actor(this->get_player(), 1, 0);
      break;
    case 'k':
      return this->move_actor(this->get_player(), -1, 0);
      break;
    case 'l':
      return this->move_actor(this->get_player(), 0, 1);
      break;
    case 'y':
      return this->move_actor(this->get_player(), -1, -1);
      break;
    case 'u':
      return this->move_actor(this->get_player(), -1, 1);
      break;
    case 'b':
      return this->move_actor(this->get_player(), 1, -1);
      break;
    case 'n':
      return this->move_actor(this->get_player(), 1, 1);
      break;
    case '.':
      return this->move_actor(this->get_player(), 0, 0);
      break;
    default:
      break;
  }
  return false;
}

void Game::game_loop() {
  while (this->get_player()->is_alive()) {
    this->display_scene();

    bool turn_taken = this->handle_input();
    if (turn_taken) { // TODO: Diagonal movement cost
      this->run_behavior();
      this->clear_dead();
      this->turn++;
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

// TODO: args
void Game::display_scene() {
  erase();

  // Display tiles: (FOV TODO)
  for (auto y = 0; y < MAP_HEIGHT; y++) {
    for (auto x = 0; x < MAP_WIDTH; x++) {
      Terrain terrain = this->get_terrain(y, x);
      switch (terrain) {
        case NO_TERRAIN:
          mvaddch(y, x, ' ');
          break;
        case WALL:
          mvaddch(y, x, '#');
          break;
        case FLOOR:
          mvaddch(y, x, '.');
          break;
      } 
    }
  }

  // Display Actors: (FOV TODO)
  for (auto actor : this->actors) {
    mvaddch(actor.get_y(), actor.get_x(), actor.get_symbol());
  }

  // HUD stuff:
  Actor* player = this->get_player();
  std::string loc_str = "Loc: (" + to_string(player->get_y()) + ", " + to_string(player->get_x()) + ")";
  mvaddstr(0, MAP_WIDTH, loc_str.c_str());
  
  std::string turn_str = "Turn: " + to_string(this->turn);
  mvaddstr(1, MAP_WIDTH, turn_str.c_str());

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
  // TODO: Actors and Items
}

void Game::run_behavior() {
  for (auto i = 0; i < (int) this->actors.size(); i++) {
    Actor* actor = &this->actors[i];
    Behavior behavior = actor->get_behavior();
    if (behavior == OBLIVIOUS_WANDERER) {
      // TODO: Wander in a random direction, with no regard for the
      //       player. Attack if they are in the targeted square.
      int dy = this->rng() % 2;
      if (this->rng() % 2 == 0) {
        dy = dy * -1;
      }
      int dx = this->rng() % 2;
      if (this->rng() % 2 == 0) {
        dx = dx * -1;
      }
      this->move_actor(actor, dy, dx);
    } // TODO: Other behavior types
  }
}

