#! /usr/bin/bash

echo "Compiling Dungeon Under Grindstone..."

g++ src/*.cpp -std=c++20 -g -Wall -Wextra -pedantic -Wconversion -l ncurses -o bin/dug

