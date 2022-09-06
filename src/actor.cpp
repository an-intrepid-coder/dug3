#include "actor.hpp"

Actor::Actor(bool is_player, 
             char symbol, 
             int y, int x, 
             int max_health,
             Behavior behavior,
             string name) {
  this->is_player = is_player;
  this->symbol = symbol;
  this->y = y;
  this->x = x;
  this->health = max_health;
  this->max_health = max_health;
  this->behavior = behavior;
  this->name = name;
  this->movement_points = 1;
}
    
int Actor::get_movement_points() {
  return this->movement_points;
}

// Returns movements points after change:
int Actor::change_movement_points(int amt) {
  this->movement_points = this->movement_points + amt;
  // For now, max of 1: (more granularity down the road TODO)
  if (this->movement_points > 1) {
    this->movement_points = 1;
  }
  return this->movement_points;
}

void Actor::set_y(int y) {
  this->y = y;
}

void Actor::set_x(int x) {
  this->x = x;
}

bool Actor::is_alive() {
  return this->health > 0;
}

bool Actor::get_is_player() {
  return this->is_player;
}

char Actor::get_symbol() {
  return this->symbol;
}

int Actor::get_y() {
  return this->y;
}

int Actor::get_x() {
  return this->x;
}

int Actor::get_health() {
  return this->health;
}

int Actor::get_max_health() {
  return this->max_health;
}

// Returns total health after the change:
int Actor::change_health(int a) {
  this->health = this->health + a;
  if (this->health < 0) {
    this->health = 0;
  }
  if (this->health > this->max_health) {
    this->health = this->max_health;
  }
  return this->health;
}

Behavior Actor::get_behavior() {
  return this->behavior;
}

string Actor::get_name() {
  return this->name;
}

Actor Player(int y, int x) {
  return Actor(true, '@', y, x, 10, NO_BEHAVIOR, "Player");
}

Actor Slime(int y, int x) {
  return Actor(false, 's', y, x, 3, OBLIVIOUS_WANDERER, "Slime");
}

Actor Bugbear(int y, int x) {
  return Actor(false, 'B', y, x, 8, SEEKING_HUNTER, "Bugbear");
}

Actor Troll(int y, int x) {
  return Actor(false, 'T', y, x, 12, WAITING_HUNTER, "Troll");
}

