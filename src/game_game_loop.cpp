#include "game.hpp"

using namespace std::chrono_literals;

void Game::game_loop() {
  this->display_title_screen();
  getch();
  for (;;) {
    Actor* player = this->get_player();
    InterfaceMode mode = this->interface_mode;

    // Display according to game's current mode:
    if (mode == MAIN_GAME) {
      this->display_scene();
    } else if (mode == CONSUMABLE_INVENTORY) {
      this->display_consumable_inventory();
    } else if (mode == DROP_CONSUMABLE) {
      this->display_consumable_inventory();
    }

    // Take input based on the game's current mode:
    bool turn_taken = false;
    if (mode == MAIN_GAME) {
      if (player->get_movement_points() == 1) {
        turn_taken = this->handle_input();
      } else if (player->get_movement_points() < 1) {
        turn_taken = true;
      }
    } else if (mode == CONSUMABLE_INVENTORY) {
      turn_taken = this->handle_consumable_inventory_input();
    } else if (mode == DROP_CONSUMABLE) {
      turn_taken = this->handle_drop_consumable_input();
    }

    // Process a game turn if input resulted in a turn taken:
    if (turn_taken) { 
      this->gold_check();
      this->loot_check();
      player->change_movement_points(1);
      this->run_behavior();
      if (!player->is_alive()) {
        break;
      }
      this->clear_dead_check();
      this->bonus_check();
      this->turn++;
      Coord coord = Coord{player->get_y(), player->get_x()};
      this->dijkstra_map_distance(coord);
      this->calculate_fov();
    }
    
    // About 30 FPS
    std::this_thread::sleep_for(33ms);
  } 
  this->display_game_over(false);
}


