#ifndef GAME_HPP
#define GAME_HPP

#include <algorithm>
#include <chrono>
#include <iostream>
#include <numeric>
#include <queue>
#include <random>
#include <string>
#include <thread>
#include <time.h>
#include <vector>
#include "actor.hpp"
#include "coord.hpp"
#include "terrain.hpp"
#include "ncurses_display.hpp"
#include "rect.hpp"
#include "bonus.hpp"
#include "visibility.hpp"
#include "flags.hpp"
#include "interface_mode.hpp"
#include "direction_type.hpp"
#include "auto_type.hpp"

// Integrated early game object for now. No separate scene stuff yet.
class Game {
  public:
    Game(Flags flags);
    void game_loop(); 
    
  private:
    Flags flags;
    InterfaceMode interface_mode;

    std::mt19937_64 rng;
    unsigned long int turn; 
    int level;

    AutoType auto_type;

    vector<Actor> actors;
    vector<string> log;

    // Map stuff:
    vector<vector<bool>> loot_map;
    vector<vector<bool>> gold_map;
    // TODO: Gear map
    bool terrain_map_generated;
    Coord level_exit;
    vector<vector<Terrain>> terrain_map;
    vector<vector<Visibility>> fov_map;
    bool distance_map_generated;
    bool displaying_distance_map;
    vector<vector<int>> distance_map_player;
    
    void display_title_screen();
    void display_game_over(bool victory);
    void generate_level(int level);
    void generate_test_map();
    void generate_map_room_accretion();
    void display_scene(); 
    void display_consumable_inventory();
    bool move_actor(Actor* actor, int dy, int dx);
    bool handle_input();
    bool toggle_displaying_distance_map();
    int roll_dx(int x);
    int roll_xdy(int x, int y);
    bool award_xp_to(int amt, Actor* actor);
    int gold_amt(int level);
    void gold_check();
    void loot_check();
    void bonus_check();
    void clear_actors();
    void auto_check(bool move_result);
    void clear_dead_check();
    Consumable generate_random_consumable();
    Terrain get_terrain(int y, int x);
    vector<Coord> get_all_terrain(Terrain type);
    Visibility get_fov(int y, int x);
    Coord get_spawn_loc();
    void run_behavior();
    Actor* get_player();
    void dijkstra_map_player(Coord start);
    Coord downhill_to_player(Coord coord);
    bool can_see(Actor* viewer, Coord goal);
    void calculate_fov();
    bool use_consumable(Consumable* item, Actor* user);
    bool handle_consumable_inventory_input();
    bool handle_drop_consumable_input();
    string get_auto_str();
};

#endif

