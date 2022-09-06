#ifndef GAME_HPP
#define GAME_HPP

#include <algorithm>
#include <random>
#include <time.h>
#include <vector>
#include <string>
#include "actor.hpp"
#include "coord.hpp"
#include "terrain.hpp"
#include "display.hpp"

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
    bool toggle_displaying_distance_map();
    
  private:
    std::mt19937_64 rng;
    unsigned long int turn = 0; 
    vector<Actor> actors;
    vector<string> log;
    // Map stuff
    bool terrain_map_generated;
    vector<vector<Terrain>> terrain_map;
    bool distance_map_generated;
    // TODO: Distance map display is debug only
    bool displaying_distance_map;
    vector<vector<int>> distance_map;

    Terrain get_terrain(int y, int x);
    Coord get_spawn_loc();
    void run_behavior();
    Actor* get_player();
    void clear_dead();
    void dijkstra_map_distance(Coord start);
    Coord downhill_from(Coord coord);
};

#endif

