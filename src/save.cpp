
#include <cerrno>
#include <cstring>
#include "exception.hpp"
#include "save.hpp"


const int STATE_VERSION = 1;


void save (FILE* file, const SaveState& state) {
  fprintf(file, "Version: %d\n", STATE_VERSION);
  save_level(file, state.level);
  save_actors(file, state.actors);
  save_tasks(file, state.tasks);
}


void save_tasks (FILE* file, const Tasklist& tasks) {
  fprintf(file, "Tasks: %ld\n", tasks.size());
  for (const Dig* task : tasks) {
    save_task(file, *task);
  }
}


void save_task (FILE* file, const Dig& task) {
  fprintf(file, "Dig:");
  for (const Point& p : task.undug_tiles) {
    fprintf(file, " (%d, %d)", p.x, p.y);
  }
  fprintf(file, "\n");
}


void save_actors (FILE* file, const Pool& actors) {
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
    raisef("Failed to open save file %s: %s",
        filename, std::strerror(errno));
  }
  
  int version = 0;
  if (fscanf(file, "Version: %d\n", &version) != 1) {
    raisef("Invalid save version in %s", filename);
  }

  const int STATE_VERSION = 1;
  if (version != STATE_VERSION) {
    raisef("Save version mismatch: expected %d, got %d.",
        STATE_VERSION, version);
  }

  state.level = load_level(file);
  load_actors(file, state.actors);
  load_tasks(file, state.tasks);

  fclose(file);
}


void load_tasks (FILE* file, Tasklist& tasks) {
  long int size = 0;
  
  const int ret = fscanf(file, "Tasks: %ld\n", &size);
  if (ret == EOF) {
    raisef("Failed to read tasks: %s",
        ferror(file) ? strerror(errno) : "Premature EOF");
  }
  else if (ret != 1) {
    raise("Invalid task header");
  }

  for (int i = 0; i < size; ++i) {
    char type[21]; // 20 chars and \0
    if (fscanf(file, "%20[^:]:", type) != 1) {
      raisef("Invalid task (%d)", i);
    }

    if (strcmp(type, "Dig") != 0) {
      raisef("Unknown task type: %s", type);
    }

    std::set<Point> undug;
    char c;
    int x;
    int y;
    while ((c = fgetc(file)) == ' ') {
      if (fscanf(file, "(%d, %d)", &x, &y) != 2) {
        raisef("Invalid task (%d): invalid tile list", i);
      }
      undug.insert(Point(x, y));
    }

    if (c != '\n') {
      raisef("Invalid task line: %d", i);
    }

    tasks.push_back(new Dig(undug));
  }
}


void load_actors (FILE* file, Pool& actors) {
  long int size = 0;

  int ret = fscanf(file, "Actors: %ld\n", &size);
  if (ret == EOF) {
    raisef("Failed to read actors: %s",
        ferror(file) ? strerror(errno) : "Premature EOF");
  }
  else if (ret == 0) {
    raise("Invalid actor header");
  }

  for (int i = 0; i < size; ++i) {
    int x, y;
    if (fscanf(file, "(%d, %d)\n", &x, &y) != 2) {
      raise("Invalid actor");
    }
    actors.push_back(new Actor(x, y));
  }
}


Level* load_level (FILE* file) {
  int w;
  int h;
  const char* fmt = "Invalid level data: %s";

  if (fscanf(file, "Level: %d x %d\n", &w, &h) != 2) {
    raisef(fmt, "width / height");
  }

  Level* level = new Level(w, h);

  for (int y = 0; y < level->h; ++y) {
    for (int x = 0; x < level->w; ++x) {
      char type;
      int hp;

      int ret = fscanf(file, "[%c %03d]", &type, &hp);
      if (ret == EOF) {
        raisef("Failed to read save file: %s",
            ferror(file) ? strerror(errno): "premature EOF");
      }
      else if (ret != 2) {
        raisef(fmt, "could not read data");
      }
      else if (!strchr(Tile::TYPECHARS, type)) {
        raisef("Invalid tile type: [%c]", type);
      }
      Tile& tile = level->tile(x, y);
      tile.type = (Tile::Type)type;
      tile.hp = hp;
    }
    if (fgetc(file) != '\n') {
      raisef(fmt, "row");
    }
  }

  return level;
}

