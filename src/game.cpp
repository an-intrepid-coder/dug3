#include "game.hpp"

using std::cout;
using std::endl;
using std::string;
using std::to_string;
using std::vector;
using std::priority_queue;

Game::Game(Flags flags) {
  this->interface_mode = MAIN_GAME;
  this->flags = flags;
  this->turn = 0;
  this->level = 1;
  this->rng = std::mt19937_64();
  this->rng.seed(time(NULL)); 
  this->actors = vector<Actor>();
  this->actors.push_back(Player(0, 0, flags.pname));
  this->generate_level(1);
}

// Returns true if the actor leveled up:
bool Game::award_xp_to(int amt, Actor* actor) {
  actor->set_xp(actor->get_xp() + amt);
  if (actor->get_xp() >= XP_TO_LEVEL) {
    string log_str = actor->get_name() + " leveled up!";

    actor->set_level(actor->get_level() + 1);
    actor->set_xp(actor->get_xp() % XP_TO_LEVEL);

    int bonus_hp = this->roll_dx(PLAYER_HIT_DIE);

    log_str = log_str + "(+" + to_string(bonus_hp) + " HP)";

    actor->set_max_health(actor->get_max_health() + bonus_hp);
    actor->set_health(actor->get_health() + bonus_hp);

    // Award bonus damage at certain levels:
    if (actor->get_level() % BONUS_DMG_LVL_FREQ == 0) {
      actor->bonuses.push_back(Bonus(EXTRA_DMG_BONUS, false, -1, 1));
      log_str = log_str + "(+1 DMG)";
    }

    if (actor->get_is_player()) {
      this->log.push_back(log_str);
    }

    return true;
  }
  return false;
}

// Returns true if the item was successfully used:
bool Game::use_consumable(Consumable* item, Actor* user) {
  // TODO: Targetable items, such as wands & scrolls
  ItemEffect effect = item->get_effect();
  if (item->get_charges() > 0) {
    if (effect == MINOR_HEALING) {
      int heal_amt = 10 + this->roll_dx(8);
      user->change_health(heal_amt);
      string log_str = user->get_name() + " healed for " + to_string(heal_amt);
      this->log.push_back(log_str);
      if (item->change_charges(-1) == 0) {
        user->remove_consumable(item);
      }
      return true;
    } else if (effect == EXTRA_DMG_EFFECT) {
      int duration = this->roll_xdy(3, 6);
      auto bonus = Bonus(EXTRA_DMG_BONUS, true, duration, EXTRA_DMG_POTION_AMT);
      user->bonuses.push_back(bonus);
      string log_str = user->get_name() + " drinks a " + item->get_name() + ".";
      this->log.push_back(log_str);
      if (item->change_charges(-1) == 0) {
        user->remove_consumable(item);
      }
      return true;
    }
  }
  return false;
}

Actor* Game::get_player() {
  for (long unsigned int i = 0; i < this->actors.size(); i++) {
    Actor* actor = &this->actors[i];
    if (actor->get_is_player()) {
      return actor;
    }
  }
  return nullptr;
}

Terrain Game::get_terrain(int y, int x) {
  if (y < 0 || x < 0 || y >= MAP_HEIGHT || x >= MAP_WIDTH) {
    return NO_TERRAIN;
  } else {
    return this->terrain_map[y][x];
  }
}

vector<Coord> Game::get_all_terrain(Terrain type) {
  auto vec = vector<Coord>();
  for (auto y = 0; y < MAP_HEIGHT; y++) {
    for (auto x = 0; x < MAP_WIDTH; x++) {
      if (this->terrain_map[y][x] == type) {
        vec.push_back(Coord{y, x});
      }
    }
  }
  return vec;
}

int Game::gold_amt(int level) {
  int gold_max = 100;
  int gold_min = 20;
  // ^ Tentative
  int amt;
  for (auto l = 0; l < level; l++) {
    amt = amt + (int) (rng() % (gold_max - gold_min) + gold_min);
  }
  return amt;
}

