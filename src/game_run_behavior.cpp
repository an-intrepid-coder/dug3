#include "game.hpp"

void Game::run_behavior() {
  for (auto i = 0; i < (int) this->actors.size(); i++) {
    Actor* actor = &this->actors[i];
    Coord coord = Coord{actor->get_y(), actor->get_x()};
    if (actor->get_behavior() == NO_BEHAVIOR || !actor->is_alive()) {
      continue;
    }
    actor->change_movement_points(1);
    if (actor->get_movement_points() == 1) {
      Actor* player = this->get_player();
      Coord pcoord = Coord{player->get_y(), player->get_x()};
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
        if (is_neighbor(pcoord, coord)) {
          int dy = pcoord.y - coord.y;
          int dx = pcoord.x - coord.x;
          this->move_actor(actor, dy, dx);
        } else {
          Coord dest = this->downhill_from(coord);
          int dy = dest.y - coord.y;
          int dx = dest.x - coord.x;
          this->move_actor(actor, dy, dx);
        }
      } else if (behavior == WAITING_HUNTER) {
        if (can_see(actor, pcoord)) {
          if (is_neighbor(pcoord, coord)) {
            int dy = pcoord.y - coord.y;
            int dx = pcoord.x - coord.x;
            this->move_actor(actor, dy, dx);
          } else {
            Coord coord = Coord{actor->get_y(), actor->get_x()};
            Coord dest = this->downhill_from(coord);
            int dy = dest.y - coord.y;
            int dx = dest.x - coord.x;
            this->move_actor(actor, dy, dx);
          }
        }
      }
      // TODO: Other behavior types
    }
  }
}

