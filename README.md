# Dungeon Under Grindstone

**Version:** 0.0.5

**Description:** A Roguelike. The player is to explore a dungeon under the town of Grindstone, and retrieve the Orb of Victory from the bottom. Very simple for now -- just the engine, mostly, and a barebones game that just implements basic mechanics. Will grow much more complex as I go.

**Installation (Linux):** 
  * Ensure you have `gcc` (if using the build script).
  * Ensure you have `libncurses-dev` to compile against.
  * (on Ubuntu and similar distributions using `apt` you can find the packages `build-essential` and `libncurses-dev` which will cover the requirements)
  * run `build.sh`

**To run the program:**
  * run the executable `dug`.

**Command-line options:**
  * `--debug` : Enables debug commands during gameplay.
  * `--name <string>` : Names the player `<string>`. 

**Controls:**
  * *h/j/k/l/y/u/b/n/.* -- movement keys
  * *H/J/K/L/Y/U/B/N* -- auto-movement keys
  * *Q* -- quit game
  * *>* -- go downstairs or pick up Orb of Victory
  * *i* -- open inventory
  * *D* -- (debug cmd) overlay a distance map centered on the player
  * *R* -- (debug cmd) reveal the map's terrain
  * *F* -- (debug cmd) heal player to full health

**Consumable Types:**
* Minor Healing Potion: Heals 10+d8 hp.
* Extra Damage Potion: Increases dmg for 3d6 turns.

**Monster Types:**
* Slime: Low health. No pathfinding (random movement). Can hit player.
* Fungoid: Low health. Hunts player if it can see the player.
* Troll: High health. High damage. Hunts player if it can see the player.
* Bugbear: High health. High damage. Hunts player by smell, unerringly.

**Player Abilities:**
* Extra dmg every third level.
(more to come)

