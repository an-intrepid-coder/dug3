#include "game.hpp"

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
      // TODO: Connect in other ways, such as L-bends.
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

