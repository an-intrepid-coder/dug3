#include "game.hpp"

// Returns true if the player used their turn; false otherwise.
bool Game::handle_consumable_inventory_input() {
  flushinp();
  auto input = getch();
  Actor* player = this->get_player();
  if (input >= 97 && input <= 120) { // ASCII a - x
    int i = input - 97;
    if (i < (int) player->consumable_inventory.size() && i >= 0) {
      this->interface_mode = MAIN_GAME;
      Consumable* item = &player->consumable_inventory[i];
      return this->use_consumable(item, player);
    } 
  } else {
    this->interface_mode = MAIN_GAME;
  }
  return false;
}

bool Game::handle_drop_consumable_input() {
  flushinp();
  auto input = getch();
  Actor* player = this->get_player();
  if (input >= 97 && input <= 120) { // ASCII a - x
    int i = input - 97;
    if (i < (int) player->consumable_inventory.size() && i >= 0) {
      this->interface_mode = MAIN_GAME;
      Consumable* item = &player->consumable_inventory[i];
      player->remove_consumable(item);
      return true;
    }
  } else {
    this->interface_mode = MAIN_GAME;
  }
  return false;
}

// Returns true if the player used their turn; false otherwise.
bool Game::handle_input() {
  flushinp();
  auto input = getch();
  Actor* player = this->get_player();
  switch(input) {
    case 'Q': // Quit
      uninit_curses();
      exit(0);
      break;
    // Movement keys:
    case 'h':
      return this->move_actor(player, 0, -1);
      break;
    case 'j':
      return this->move_actor(player, 1, 0);
      break;
    case 'k':
      return this->move_actor(player, -1, 0);
      break;
    case 'l':
      return this->move_actor(player, 0, 1);
      break;
    case 'y':
      return this->move_actor(player, -1, -1);
      break;
    case 'u':
      return this->move_actor(player, -1, 1);
      break;
    case 'b':
      return this->move_actor(player, 1, -1);
      break;
    case 'n':
      return this->move_actor(player, 1, 1);
      break;
    case '.':
      return this->move_actor(player, 0, 0);
      break;
    case '>':
      if (player->get_y() == this->level_exit.y &&
          player->get_x() == this->level_exit.x) {
        if (this->level == FINAL_DLVL) {
          string victory = "You grab the Orb! Victory!";
          this->log.push_back(victory);
          this->display_game_over(true);
        } else {
          this->level++;
          this->generate_level(this->level);
        }
      }
      break;
    case 'D':
      // Overlay Djikstra distance: (Debug cmd!)
      if (this->flags.debug) {
        this->dijkstra_map_distance(Coord{player->get_y(), player->get_x()});
        this->toggle_displaying_distance_map();
      }
      break;
    case 'd':
      // Drop consumables menu: 
      // TODO: Drop anything menu
      // TODO: Persistent items (currently destroyed on drop)
      this->interface_mode = DROP_CONSUMABLE;
      break;
    case 'R':
      // Reveal the map. (Debug cmd!)
      if (this->flags.debug) {
        for (auto y = 0; y < MAP_HEIGHT; y++) {
          for (auto x = 0; x < MAP_WIDTH; x++) {
            this->fov_map[y][x] = VISIBLE;
          }
        }
      }
      break;
    case 'F':
      // Heal player to full health (debug cmd!):
      if (this->flags.debug) {
        player->set_health(player->get_max_health());
      }
      break;
    case 'i': // Inventory menu
      this->interface_mode = CONSUMABLE_INVENTORY;
      break;
    default:
      break;
  }
  return false;
}


