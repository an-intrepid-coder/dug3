#include "game.hpp"

Visibility Game::get_fov(int y, int x) {
  return this->fov_map[y][x];
}

void Game::calculate_fov() {
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
  int vis = player->get_vision_radius();
  for (auto y = player_loc.y - vis; y <= player_loc.y + vis; y++) {
    for (auto x = player_loc.x - vis; x <= player_loc.x + vis; x++) {
      if (y >= 0 && y < MAP_HEIGHT && x >= 0 && x < MAP_WIDTH) {
        Coord coord = Coord{y, x};
        if (can_see(player, coord)) {
          this->fov_map[y][x] = VISIBLE;
        }
      }
    }
  }
}

bool Game::can_see(Actor* viewer, Coord goal) {
  Coord start = Coord{viewer->get_y(), viewer->get_x()};
  if (get_chebyshev_distance(start, goal) > viewer->get_vision_radius()) {
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

