
#include <cerrno>
#include <cstring>
#include "exception.hpp"
#include "save.hpp"


void save (const char* filename, const SaveState& state) {
  FILE* file = fopen(filename, "wb");
  if (file == NULL) {
    raise("Failed to open save file %s: %s\n", filename, std::strerror(errno));
  }

  save(file, state.level);
}


void save (FILE* file, const Level& level) {
  fprintf(file, "Level: %d x %d\n", level.w, level.h);
  for (int y = 0; y < level.h; ++y) {
    for (int x = 0; x < level.w; ++x) {
      save(file, level.tile(x, y));
    }
    fprintf(file, "\n");
  }
}


void save (FILE* file, const Tile& tile) {
  fprintf(file, "[%d %c]", tile.type, tile.passable ? '.' : '#');
}


LoadState load (const char* filename) {
  FILE* file = fopen(filename, "rb");
  if (file == NULL) {
    raise("Failed to open save file %s: %s\n", filename, std::strerror(errno));
  }

  const LoadState state = {
    load_level(file),
  };

  fclose(file);

  return state;
}


Level* load_level (FILE* file) {
  int w;
  int h;
  const char* fmt = "Invalid level data: %s";

  if (fscanf(file, "Level: %d x %d\n", &w, &h) != 2) {
    raise(fmt, "width / height");
  }

  Level* level = new Level(w, h);

  for (int y = 0; y < level->h; ++y) {
    for (int x = 0; x < level->w; ++x) {
      int type;
      char passable;
      int matches = fscanf(file, "[%d %c]", &type, &passable);
      if (matches != 2) {
        raise("Invalid tile format (%d)", matches);
      }
      else if (passable != '#' && passable != '.') {
        raise("Invalid tile data: [%d %c]", type, passable);
      }
      Tile& tile = level->tile(x, y);
      tile.type = (Tile::Type)type;
      tile.passable = (passable == '.');
    }
    if (fgetc(file) != '\n') {
      raise(fmt, "row");
    }
  }

  return level;
}

