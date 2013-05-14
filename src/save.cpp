
#include <cerrno>
#include <cstring>
#include "exception.hpp"
#include "save.hpp"


const int STATE_VERSION = 1;


void save (FILE* file, const SaveState& state) {
  fprintf(file, "Version: %d\n", STATE_VERSION);
  save_level(file, state.level);
  save_actors(file, state.actors);
}


void save_actors (FILE* file, const std::vector<Actor*>& actors) {
  fprintf(file, "Actors: %ld\n", actors.size());
  for (const Actor* actor : actors) {
    fprintf(file, "(%d, %d)\n",
        actor->p.x, actor->p.y);
  }
}


void save_level (FILE* file, const Level& level) {
  fprintf(file, "Level: %d x %d\n", level.w, level.h);
  for (int y = 0; y < level.h; ++y) {
    for (int x = 0; x < level.w; ++x) {
      save_tile(file, level.tile(x, y));
    }
    fprintf(file, "\n");
  }
}


void save_tile (FILE* file, const Tile& tile) {
  fprintf(file, "[%c %03d]", tile.type, tile.hp);
}


void load (const char* filename, LoadState& state) {
  FILE* file = fopen(filename, "rb");
  if (file == NULL) {
    raise("Failed to open save file %s: %s\n",
        filename, std::strerror(errno));
  }
  
  int version = 0;
  if (fscanf(file, "Version: %d\n", &version) != 1) {
    raise("Invalid save version in %s", filename);
  }

  const int STATE_VERSION = 1;
  if (version != STATE_VERSION) {
    raise("Save version mismatch: expected %d, got %d.\n",
        STATE_VERSION, version);
  }

  state.level = load_level(file);
  load_actors(file, state.actors);

  fclose(file);
}


void load_actors (FILE* file, std::vector<Actor*>& actors) {
  long int size = 0;
  const char* fmt = "Failed to read actors: %s";

  int ret = fscanf(file, "Actors: %ld\n", &size);
  if (ret == EOF) {
    if (ferror(file)) {
      raise(fmt, strerror(errno));
    }
    else {
      raise(fmt, "Premature EOF");
    }
  }
  else if (ret == 0) {
    raise(fmt, "Invalid header!");
  }

  actors.reserve(size);

  for (int i = 0; i < size; ++i) {
    int x, y;
    if (fscanf(file, "(%d, %d)\n", &x, &y) != 2) {
      raise(fmt, "Invalid actor!");
    }
    actors.push_back(new Actor(x, y));
  }
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
      char type;
      int hp;

      int ret = fscanf(file, "[%c %03d]", &type, &hp);
      if (ret == EOF) {
        raise("Failed to read save file: %s",
            ferror(file) ? strerror(errno): "premature EOF");
      }
      else if (ret != 2) {
        raise(fmt, "could not read data");
      }
      else if (!strchr(Tile::TYPECHARS, type)) {
        raise("Invalid tile type: [%c]", type);
      }
      Tile& tile = level->tile(x, y);
      tile.type = (Tile::Type)type;
      tile.hp = hp;
    }
    if (fgetc(file) != '\n') {
      raise(fmt, "row");
    }
  }

  return level;
}

