#ifndef RECT_HPP
#define RECT_HPP

#include "coord.hpp"

using std::vector;

// TODO: Rect class

typedef struct {
  Coord top_left;
  int height;
  int width;
} Rect;

bool rects_intersect(Rect a, Rect b);
Coord rect_center(Rect rect);
bool rect_contains(Rect rect, Coord coord);

#endif

