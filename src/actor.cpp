#include <random>
#include "actor.hpp"
#include "constants.hpp"

Actor::Actor(bool is_player, 
             char symbol, 
             int y, int x, 
             int max_health,
             int xp_worth,
             Behavior behavior,
             string name) {
  this->is_player = is_player;
  this->gold = 0;
  this->level = 1;
  this->xp = 0;
  this->xp_worth = xp_worth;
  this->symbol = symbol;
  this->y = y;
  this->x = x;
  this->health = max_health;
  this->max_health = max_health;
  this->behavior = behavior;
  this->name = name;
  this->movement_points = 1;
  this->consumable_inventory = vector<Consumable>();
  this->gear_inventory = vector<Gear>();
  this->bonuses = vector<Bonus>();
}

// Returns true if the actor leveled up:
bool Actor::award_xp(int amt) {
  this->xp = this->xp + amt;
  if (this->xp >= XP_TO_LEVEL) {
    this->level++;
    this->xp = this->xp % XP_TO_LEVEL;
    return true;
  }
  return false;
}

int Actor::get_dmg(std::mt19937_64 rng) { 
  int base_dmg = (int) (rng() % BASE_MAX_DMG + 1);
  int bonus_dmg = this->level - 1;
  for (auto bonus : this->bonuses) {
    if (bonus.type == EXTRA_DMG_BONUS) {
      return (base_dmg + bonus_dmg) * 2;
    }
  }
  return base_dmg + bonus_dmg;
}

int Actor::get_level() {
  return this->level;
}

int Actor::get_xp() {
  return this->xp;
}

int Actor::get_xp_worth() {
  return this->xp_worth;
}

int Actor::get_gold() {
  return this->gold;
}

int Actor::change_gold(int amt) {
  this->gold = this->gold + amt;
  if (this->gold < 0) {
    this->gold = 0;
  }
  return this->gold;
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

int Actor::get_inv_size_consumable() {
  return (int) this->consumable_inventory.size();
}

int Actor::get_inv_size_gear() {
  return (int) this->gear_inventory.size();
}

int Actor::get_total_inv_size() {
  return (int) (this->consumable_inventory.size() + this->gear_inventory.size());
}

// Returns total inventory size:
int Actor::add_consumable(Consumable item) {
  if (this->consumable_inventory.size() < INVENTORY_ROWS) {
    this->consumable_inventory.push_back(item);
  }
  return this->get_total_inv_size();
}

// Returns total inventory size:
int Actor::remove_consumable(Consumable* item) { // TODO
  int i = 0;
  for (auto it = this->consumable_inventory.begin(); it != this->consumable_inventory.end(); i++) {
    if (&this->consumable_inventory[i] == item) {
      this->consumable_inventory.erase(it);
    } else {
      ++it;
    }
  }
  return (int) this->consumable_inventory.size();
}

Actor Player(int y, int x) {
  return Actor(true, '@', y, x, 10, 0, NO_BEHAVIOR, "Player");
}

// TODO: Leveled versions.

Actor Slime(int y, int x) {
  return Actor(false, 's', y, x, 3, 1, OBLIVIOUS_WANDERER, "Slime");
}

Actor Fungoid(int y, int x) {
  return Actor(false, 'f', y, x, 3, 10, WAITING_HUNTER, "Fungoid");
}

Actor Bugbear(int y, int x) {
  return Actor(false, 'B', y, x, 10, 25, SEEKING_HUNTER, "Bugbear");
}

Actor Troll(int y, int x) {
  return Actor(false, 'T', y, x, 6, 10, WAITING_HUNTER, "Troll");
}

