#ifndef ACTOR_HPP
#define ACTOR_HPP

#include <vector>
#include <string>
#include "behavior.hpp"
#include "item.hpp"

using std::string;
using std::vector;

class Actor {
  public:
    vector<Consumable> consumable_inventory;
    vector<Gear> gear_inventory; 
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
    int get_inv_size_consumable();
    int get_inv_size_gear();
    int get_total_inv_size();
    int add_consumable(Consumable item);
    int remove_consumable(Consumable* item);
    int add_gear(Gear item); // TODO
    int remove_gear(Gear* item); // TODO
    int get_gold();
    int change_gold(int amt);
  private:
    string name;
    bool is_player;
    char symbol;
    int gold;
    int y;
    int x;
    int health;
    int max_health;
    Behavior behavior;
    int movement_points;
};

Actor Player(int y, int x);
Actor Slime(int y, int x);
Actor Bugbear(int y, int x);
Actor Troll(int y, int x);

#endif

