#include "game.hpp"

using std::string;
using std::to_string;

// Returns true if the Actor was able to move:
bool Game::move_actor(Actor* actor, int dy, int dx) {
  Coord actor_loc = Coord{actor->get_y(), actor->get_x()};
  int ty = actor->get_y() + dy;
  int tx = actor->get_x() + dx;
  Terrain terrain = this->get_terrain(ty, tx);
  if (terrain == WALL) {
    return false;
  }
  bool blocked = false;
  // Check for actors and initiate combat if needed:
  for (auto i = 0; i < (int) this->actors.size(); i++) {
    Actor* actor2 = &this->actors[i];
    Coord actor2_loc = Coord{actor2->get_y(), actor2->get_x()};
    if (actor2_loc.y == ty && 
        actor2_loc.x == tx && 
        (actor->get_is_player() || actor2->get_is_player()) &&
        // NOTE: ^ For now, enemies don't attack each other. Will add a
        //       toggle for that soon (confused enemies, berserkers, etc.).
        actor != actor2) {
      int dmg = actor->get_dmg(this->rng);
      actor2->change_health(-dmg);
      Actor* player = this->get_player();
      if (can_see(player, actor_loc) && can_see(player, actor2_loc)) {
        string dmg_log = actor->get_name() + 
                         " hits " + actor2->get_name() + 
                         " for " + to_string(dmg) + 
                         " dmg! (" + to_string(actor2->get_health()) + 
                         "/" + to_string(actor2->get_max_health()) + ")";
        this->log.push_back(dmg_log);
        if (!actor2->is_alive()) {
          this->award_xp_to(actor2->get_xp_worth(), actor);
        }
      }
      return true;
    } else if (actor2_loc.y == ty &&
               actor2_loc.x == tx &&
               actor != actor2) {
      blocked = true;
    }
  }
  if (!blocked) {
    // Move the actor:
    if (terrain == FLOOR) {
      actor->set_y(ty);
      actor->set_x(tx);
      // Assess the cost (diagonal is effectively 2 moves):
      if (dy != 0 && dx != 0) {
        actor->change_movement_points(-2);
      } else {
        actor->change_movement_points(-1);
      }
      return true;
    }
  }
  return false;
}


