#ifndef GAME_HPP
#define GAME_HPP

#include <random>
#include <time.h>
#include <vector>
#include <string>
#include "actor.hpp"
#include "coord.hpp"
#include "terrain.hpp"
#include "display.hpp"

#define MAP_HEIGHT 20
#define MAP_WIDTH 64
#define CONSOLE_ROWS 4
#define HUD_STRING_SIZE 16
#define LOG_STRING_SIZE 80

using std::vector;
using std::string;

// Integrated early game object for now. No separate scene stuff yet.

class Game {
  public:
    Game();
    void generate_test_map();
    void display_scene(); // TODO: args
    void game_loop(); 
    bool move_actor(Actor* actor, int dy, int dx);
    bool handle_input();
    
  private:
    std::mt19937_64 rng;
    unsigned long int turn = 0;
    vector<Actor> actors;
    vector<string> log;
    // Map stuff
    bool map_generated;
    vector<vector<Terrain>> terrain_map;

    Terrain get_terrain(int y, int x);
    Coord get_spawn_loc();
    void run_behavior();
    Actor* get_player();
    void clear_dead();
};

#endif

