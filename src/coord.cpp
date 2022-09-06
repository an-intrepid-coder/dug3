#include <cmath>
#include "coord.hpp"

using std::abs;

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


