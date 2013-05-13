#ifndef SAVE_HPP_2YUGWOEK
#define SAVE_HPP_2YUGWOEK

#include <cstdio>
#include "level.hpp"
#include "tile.hpp"


struct SaveState {
  const Level& level;
};


struct LoadState {
  Level* level;
};



void save (const char* filename, const SaveState& state);
void save (FILE* file, const Level& level);
void save (FILE* file, const Tile& tile);

LoadState load (const char* filename);
Level* load_level (FILE* file);

#endif /* end of include guard: SAVE_HPP_2YUGWOEK */

