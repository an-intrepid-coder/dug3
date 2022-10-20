#include "game.hpp"

/* Dijkstra maps based on Wikipedia article:
     https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm  */

void Game::dijkstra_map_player(Coord start) {
  this->distance_map_player = vector<vector<int>>();
  vector<vector<int>> distance = vector<vector<int>>();
  PriorityQueueCoord pq = PriorityQueueCoord(MAP_HEIGHT * MAP_WIDTH);

  for (auto y = 0; y < MAP_HEIGHT; y++) {
    distance.push_back(vector<int>());
    for (auto x = 0; x < MAP_WIDTH; x++) {
      if (!(start.y == y && start.x == x)) {
        distance[y].push_back(INT_MAX);
      } else {
        distance[y].push_back(0);
      }
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
          pq.change_priority_by_coord(coord, alt);
        }
      }
    }
  }

  for (auto y = 0; y < MAP_HEIGHT; y++) {
    this->distance_map_player.push_back(vector<int>());
    for (auto x = 0; x < MAP_WIDTH; x++) {
      this->distance_map_player[y].push_back(distance[y][x]);
    }
  }
  this->distance_map_generated = true;
}

/* Grabs a random coordinate that is "down hill" from the
   given starting coordinate on the Dijkstra-generated
   distance map.  */
Coord Game::downhill_to_player(Coord coord) {
  // Get the distance to all neighbor coords:
  vector<Coord> neighbors = get_neighbors(coord);
  vector<int> distances = vector<int>();
  for (auto coord : neighbors) {
    // Set neighboring tiles that contain other actors to a high value:
    bool occupied = false;
    for (auto actor : this->actors) {
      if (is_neighbor(Coord{actor.get_y(), actor.get_x()}, coord) &&
          !actor.get_is_player()) {
        distances.push_back(INT_MAX);
        occupied = true;
      }
    }
    if (!occupied) {
      distances.push_back(this->distance_map_player[coord.y][coord.x]);
    }
  }

  // Find the lowest distance value among neighbor coords:
  int low = INT_MAX;
  for (auto i = 0; i < (int) distances.size(); i++) {
    if (distances[i] < low) {
      low = distances[i];
    }
  }

  /* Push all coords which are equal to the lowest-distance
     neighbor in to a new vector:  */
  vector<Coord> candidates = vector<Coord>();
  for (auto i = 0; i < (int) distances.size(); i++) {
    Coord coord = neighbors[i];
    Terrain terrain = this->get_terrain(coord.y, coord.x);
    if (distances[i] == low && terrain == FLOOR) {
      candidates.push_back(neighbors[i]);
    }
  }

  if (candidates.empty()) {
    // This block can occur if the actor is, for example, surrounded by walls.
    return coord;
  }

  // Return a random coord from the final vec:
  int j = (int) (this->rng() % candidates.size());
  Coord dest = candidates[j];
  return dest;
}

