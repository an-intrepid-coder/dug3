#ifndef COORD_HPP
#define COORD_HPP

#include <vector>

using std::vector;

typedef struct {
  int y;
  int x;
} Coord;

vector<Coord> bresenham_line(Coord start, Coord goal);

#endif

