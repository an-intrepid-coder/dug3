#include "game.hpp"

void Game::run_behavior() {
  for (auto i = 0; i < (int) this->actors.size(); i++) {
    Actor* actor = &this->actors[i];
    if (actor->get_behavior() == NO_BEHAVIOR || !actor->is_alive()) {
      continue;
    }
    actor->change_movement_points(1);
    if (actor->get_movement_points() == 1) {
      Actor* player = this->get_player();
      Behavior behavior = actor->get_behavior();
      if (behavior == OBLIVIOUS_WANDERER) {
        // Wander in a random direction, with no regard for the
        // player. Attack if they are in the targeted square.
        int dy = this->rng() % 2;
        if (this->rng() % 2 == 0) {
          dy = dy * -1;
        }
        int dx = this->rng() % 2;
        if (this->rng() % 2 == 0) {
          dx = dx * -1;
        }
        this->move_actor(actor, dy, dx);
      } else if (behavior == SEEKING_HUNTER) {
        /* Rolls "down-hill" towards the player, regardless of 
           FOV.  */
        Coord coord = Coord{actor->get_y(), actor->get_x()};
        Coord dest = this->downhill_from(coord);
        int dy = dest.y - actor->get_y();
        int dx = dest.x - actor->get_x();
        this->move_actor(actor, dy, dx);
      } else if (behavior == WAITING_HUNTER) {
        Coord player_loc = Coord{player->get_y(), player->get_x()};
        if (can_see(actor, player_loc)) {
          Coord coord = Coord{actor->get_y(), actor->get_x()};
          Coord dest = this->downhill_from(coord);
          int dy = dest.y - actor->get_y();
          int dx = dest.x - actor->get_x();
          this->move_actor(actor, dy, dx);
        }
      }
      // TODO: Other behavior types
    }
  }
}

