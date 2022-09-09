// TODO: Rect intersections and so on

#include "rect.hpp"

bool rects_intersect(Rect a, Rect b) {
  for (auto y = a.top_left.y; y < a.top_left.y + a.height; y++) {
    for (auto x = a.top_left.x; x < a.top_left.x + a.width; x++) {
      if (y >= b.top_left.y &&
          y < b.top_left.y + b.height &&
          x >= b.top_left.x &&
          x < b.top_left.x + b.width) {
        return true;
      }
    }
  }
  return false;
}

bool rect_contains(Rect rect, Coord coord) {
  return coord.y >= rect.top_left.y &&
         coord.y < rect.top_left.y + rect.height &&
         coord.x >= rect.top_left.x &&
         coord.x < rect.top_left.x + rect.width;
}

Coord rect_center(Rect rect) {
  return Coord{
    rect.top_left.y + rect.height / 2,
    rect.top_left.x + rect.width / 2
  };
}

