#ifndef COORD_HPP
#define COORD_HPP

#include <vector>
#include "constants.hpp"

using std::vector;

typedef struct {
  int y;
  int x;
} Coord;

vector<Coord> bresenham_line(Coord start, Coord goal);
vector<Coord> get_neighbors(Coord coord);
int get_distance(Coord start, Coord other);

// Custom Priority Queue that does some things the STL version doesn't:
class PriorityQueueCoord {
  public:
    PriorityQueueCoord(int cap);
    int get_size();
    void insert(Coord coord, int priority);
    Coord extract_min();
    void remove(int i);
    void change_priority_by_coord(Coord coord, int p);
    void change_priority(int i, int p);
    bool contains(Coord coord);
  private:
    int size;
    int cap;
    vector<int> priorities;
    vector<Coord> contents;
    
    int parent(int i);
    int left(int i);
    int right(int i);
    void swap(int i, int j);
    void sift_up(int i);
    void sift_down(int i);
};

#endif

