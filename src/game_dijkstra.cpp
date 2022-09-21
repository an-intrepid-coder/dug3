#include "game.hpp"

/* Dijkstra map based on Wikipedia article:
     https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm  */
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


