#ifndef ACTOR_HPP
#define ACTOR_HPP

#include <string>
#include "behavior.hpp"

using std::string;

class Actor {
  public:
    Actor(bool is_player, 
          char symbol,  
          int y, int x, 
          int max_health, 
          Behavior behavior,
          string name);
    bool get_is_player();
    char get_symbol();
    int get_y();
    int get_x();
    void set_y(int y);
    void set_x(int x);
    bool is_alive();
    Behavior get_behavior();
    int change_health(int a);
    int get_health();
    int get_max_health();
    string get_name();
    int get_movement_points();
    int change_movement_points(int amt);
  private:
    string name;
    bool is_player;
    char symbol;
    int y;
    int x;
    int health;
    int max_health;
    Behavior behavior;
    int movement_points;
    // TODO: Much more
};

Actor Player(int y, int x);
Actor Slime(int y, int x);
Actor Bugbear(int y, int x);
Actor Troll(int y, int x);

#endif

