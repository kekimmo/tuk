#ifndef SAVE_HPP_2YUGWOEK
#define SAVE_HPP_2YUGWOEK

#include <cstdio>
#include <vector>

#include "level.hpp"
#include "tile.hpp"
#include "actor.hpp"
#include "dig.hpp"


struct SaveState {
  const Level& level;
  const Pool& actors;
  const Tasklist& tasks;
};


struct LoadState {
  Level* level;
  Pool actors;
  Tasklist tasks;
};



void save (FILE* file, const SaveState& state);
void save_tasks (FILE* file, const Tasklist& tasks);
void save_task (FILE* file, const Dig& task);
void save_actors (FILE* file, const Pool& actors);
void save_level (FILE* file, const Level& level);
void save_tile (FILE* file, const Tile& tile);

void load (const char* filename, LoadState& state);
void load_tasks (FILE* file, Tasklist& tasks);
void load_actors (FILE* file, Pool& actors);
Level* load_level (FILE* file);

#endif /* end of include guard: SAVE_HPP_2YUGWOEK */

