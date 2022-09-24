#include "game.hpp"

void Game::generate_level(int level) { // TODO: More args
  // Remove any actors (minus the player) from the previous level:
  this->clear_actors();

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

Consumable Game::generate_random_consumable() {
  auto vec = vector<Consumable>();

  vec.push_back(MinorHealingPotion());
  vec.push_back(ExtraDamagePotion());

  int i = (int) (this->rng() % vec.size()); 

  return vec[i];
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


