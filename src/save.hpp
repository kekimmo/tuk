#ifndef SAVE_HPP_2YUGWOEK
#define SAVE_HPP_2YUGWOEK

#include <cstdio>
#include <vector>

#include "level.hpp"
#include "tile.hpp"
#include "actor.hpp"


struct SaveState {
  const Level& level;
  const std::vector<Actor*>& actors;
};


struct LoadState {
  Level* level;
  std::vector<Actor*> actors;
};



void save (FILE* file, const SaveState& state);
void save_actors (FILE* file, const std::vector<Actor*>& actors);
void save_level (FILE* file, const Level& level);
void save_tile (FILE* file, const Tile& tile);

void load (const char* filename, LoadState& state);
void load_actors (FILE* file, std::vector<Actor*>& actors);
Level* load_level (FILE* file);

#endif /* end of include guard: SAVE_HPP_2YUGWOEK */

