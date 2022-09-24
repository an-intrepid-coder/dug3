#include "game.hpp"

#include "ncurses_display.hpp"

using std::string;
using std::to_string;

using std::cout;
using std::endl;

void Game::clear_dead_check() {
  int i = 0;
  for (auto it = this->actors.begin(); it != this->actors.end(); i++) {
    if (!this->actors[i].is_alive() && !this->actors[i].get_is_player()) {
      this->actors.erase(it);
    } else {
      ++it;
    }
  }
}

// Clears all actors (minus the player):
void Game::clear_actors() {
  int i = 0;
  for (auto it = this->actors.begin(); it != this->actors.end(); i++) {
    if (!this->actors[i].get_is_player()) {
      this->actors.erase(it);
    } else {
      ++it;
    }
  }
}

void Game::gold_check() {
  Actor* player = this->get_player();
  if (this->gold_map[player->get_y()][player->get_x()]) {
    int gold = gold_amt(this->level);
    player->change_gold(gold);
    string log_str = player->get_name() + " picked up " + to_string(gold) + " gold.";
    this->log.push_back(log_str);
    this->gold_map[player->get_y()][player->get_x()] = false;
  }
}

void Game::loot_check() {
  Actor* player = this->get_player();
  if (this->loot_map[player->get_y()][player->get_x()]) {
    Consumable loot = generate_random_consumable();

    string log_str = player->get_name() + " picked up " + loot.get_name();
    this->log.push_back(log_str);
    
    player->add_consumable(loot);


    this->loot_map[player->get_y()][player->get_x()] = false;
  }
}

void Game::bonus_check() {
  for (auto j = 0; j < (int) this->actors.size(); j++) {
    Actor* actor = &this->actors[j];
    int i = 0;
    for (auto it = actor->bonuses.begin(); it != actor->bonuses.end(); i++) {
      if (actor->bonuses[i].get_duration() == 0) {
        actor->bonuses.erase(it);
      } else {
        actor->bonuses[i].decrement();
        ++it;
      }
    }
  }
}

