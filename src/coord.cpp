#include <algorithm>
#include <cmath>
#include <limits.h>
#include "coord.hpp"
#include "display.hpp"

using std::abs;
using std::max;

/* Returns a vector of coordinates representing a Bresenham line
   from the start Coord to the end Coord.  */
vector<Coord> bresenham_line(Coord start, Coord goal) {
  /* Algorithm pseudocode courtesy of Wikipedia:
     https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm#All_cases */
  //Vector_Coord line = make_empty_vector_coord(100);
  vector<Coord> result = vector<Coord>();
  int plot_x = start.x;
  int plot_y = start.y;
  int dx = abs(goal.x - start.x);
  int dy = -1 * abs(goal.y - start.y);
  int sx = start.x < goal.x ? 1 : -1, sy = start.y < goal.y ? 1 : -1;
  int err = dx + dy;
  while (plot_x != goal.x || plot_y != goal.y) {
    //vector_coord_add(&line, make_coord(plot_y, plot_x));
    result.push_back(Coord{plot_y, plot_x});
    int err2 = err * 2;
    if (err2 >= dy) {
      err += dy;
      plot_x = plot_x + sx;
    }
    if (err2 <= dx) {
      err += dx;
      plot_y = plot_y + sy;
    }
  }
  result.push_back(Coord{plot_y, plot_x});
  return result;
}

vector<Coord> get_neighbors(Coord coord) {
  vector<Coord> vec = vector<Coord>();
  for (int y = coord.y - 1; y <= coord.y + 1; y++) {
    for (int x = coord.x - 1; x <= coord.x + 1; x++) {
      if (y >= 0 && y < MAP_HEIGHT && x >= 0 && x < MAP_WIDTH) {
        Coord next = Coord{y, x};
        if (!(coord.y == next.y && coord.x == next.x)) {
          vec.push_back(next);
        }
      }
    }
  }
  return vec;
}

// Manhattan Distance:
int get_distance(Coord start, Coord other) {
  return abs(start.x - other.x) + abs(start.y - other.y);
}

// Chebyshev distance:
int get_chebyshev_distance(Coord start, Coord other) {
  return max(abs(other.x - start.x), abs(other.y - start.y));
}

PriorityQueueCoord::PriorityQueueCoord(int cap) {
  this->size = 0;
  this->cap = cap;
  this->priorities = vector<int>(cap);
  this->contents = vector<Coord>(cap);
}

int PriorityQueueCoord::get_size() {
  return this->size;
}

int PriorityQueueCoord::parent(int i) {
  return i / 2;
}

int PriorityQueueCoord::left(int i) {
  return 2 * i;
}

int PriorityQueueCoord::right(int i) {
  return 2 * i + 1;
}

void PriorityQueueCoord::swap(int i, int j) {
  int temp = this->priorities[i];
  this->priorities[i] = this->priorities[j];
  this->priorities[j] = temp;
  Coord temp2 = this->contents[i];
  this->contents[i] = this->contents[j];
  this->contents[j] = temp2;
}

void PriorityQueueCoord::sift_up(int i) {
  while (i > 1 && this->priorities[parent(i)] > this->priorities[i]) {
    this->swap(this->parent(i), i);
    i = this->parent(i);
  }
}

void PriorityQueueCoord::sift_down(int i) {
  int max_index = i;
  int l = this->left(i);
  if (l <= this->get_size() && this->priorities[l] < this->priorities[max_index]) {
    max_index = l;
  }
  int r = this->right(i);
  if (r <= this->get_size() && this->priorities[r] < this->priorities[max_index]) {
    max_index = r;
  }
  if (i != max_index) {
    this->swap(i, max_index);
    this->sift_down(max_index);
  }
}

void PriorityQueueCoord::insert(Coord coord, int priority) {
  if (this->size == this->cap) {
    uninit_curses();
    exit(1); // TODO: Exception handling
  }
  this->size++;
  this->priorities[size] = priority;
  this->contents[size] = coord;
  this->sift_up(size);
}

Coord PriorityQueueCoord::extract_min() {
  Coord result = this->contents[1];
  this->priorities[1] = this->priorities[size];
  this->contents[1] = this->contents[size];
  this->size--;
  this->sift_down(1);
  return result;
}

void PriorityQueueCoord::remove(int i) {
  this->priorities[i] = INT_MAX;
  this->sift_up(i);
  this->extract_min();
}

void PriorityQueueCoord::change_priority(int i, int p) {
  int oldp = this->priorities[i];
  this->priorities[i] = p;
  if (p < oldp) {
    this->sift_up(i);
  } else {
    this->sift_down(i);
  }
}

void PriorityQueueCoord::change_priority_by_coord(Coord coord, int p) {
  for (auto i = 0; i < this->get_size(); i++) {
    Coord next = this->contents[i];
    if (coord.y == next.y && coord.x == next.x) {
      this->change_priority(i, p);
    }
  }
}

bool PriorityQueueCoord::contains(Coord coord) {
  for (auto coord2 : this->contents) {
    if (coord.x == coord2.x && coord.y == coord2.y) {
      return true;
    }
  }
  return false;
}

