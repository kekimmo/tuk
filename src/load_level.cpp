
#include <cstdio>
#include <cerrno>
#include <cstring>

#include "exception.hpp"
#include "level.hpp"
#include "load_level.hpp"


Level* load_level (const char* filename) {
  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    raise("Opening %s failed: %s", filename, strerror(errno));
  }

  int width = 0;
  while (!feof(file) && fgetc(file) != '\n') {
    ++width;
  }

  int height = 1;
  while (!feof(file)) {
    if (fgetc(file) == '\n') {
      ++height;
    }
  }

  rewind(file);

  Level* level = new Level(width, height);

  int x = 0;
  int y = 0;

  while (!feof(file)) {
    char c = fgetc(file);
    if (c == '\n' || c == EOF) {
      x = 0;
      ++y;
      continue;
    }

    Tile& t = level->tile(x, y);

    switch (c) {
      case ' ':
        t.type = Tile::FLOOR;
        t.passable = true;
        break;

      case '#':
        t.type = Tile::WALL;
        break;

      default:
        raise("Invalid tile '%c' at (%d, %d)", c, x, y);
    }

    ++x;
  }

  return level;
}

