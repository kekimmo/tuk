
#include <cstdio>
#include <cmath>
#include <cstdarg>
#include <cerrno>
#include <cstring>
#include <cassert>
#include <vector>
#include <algorithm>
#include <map>
#include <list>

#define GL_GLEXT_PROTOTYPES
extern "C" {
  #include <SDL/SDL.h>
  #include <GL/gl.h>
  #include <GL/glu.h>
  #include <sys/stat.h>
  #include <fcntl.h>
}

#include "exception.hpp"
#include "vec.hpp"
#include "selection.hpp"
#include "texture.hpp"
#include "level.hpp"
#include "actor.hpp"
#include "save.hpp"
#include "ui.hpp"
#include "path.hpp"
#include "dig.hpp"


const int TILE_SIZE = 32;


void toggle (bool& var) {
  var = !var;
}


template<typename T> void roll_inc (const T limit, T& value) {
  value = (value < limit) ? (value + 1) : 0;
}

template<typename T> void roll_dec (const T limit, T& value) {
  value = (value == 0) ? limit : (value - 1);
}


void save_state (const Level& level, const std::vector<Actor*>& actors) {
  const int MAXLEN = 128;
  char name[MAXLEN];

  int num = 0;
  int fd = -1;

  do {
    if (num > 999) {
      raisef("More than %d save files!", 999);
    }
    snprintf(name, MAXLEN, "sav/%03d.sav", num);
    num += 1;
    fd = open(name, O_CREAT | O_EXCL | O_WRONLY, 0644);
    if (fd == -1 && errno != EEXIST) {
      raisef("Failed to open save file %s: %s", name, std::strerror(errno));
    }
  }
  while (fd == -1);

  FILE* file = fdopen(fd, "wb");
  if (file == NULL) {
    raisef("Failed to open save file %s: %s", name, std::strerror(errno));
  }

  SaveState state = {
    level,
    actors,
  };

  save(file, state);

  fclose(file); // this also closes fd

  printf("Saved: %s\n", name);
}


void game_main (UI& ui, const Textures& tex, Level& level, std::list<Dig*>& tasks, std::vector<Actor*>& actors) {
  Vec<int> mouse(0, 0);

  bool running = true;
  bool freerun = false;
  std::map<Point, std::list<const Actor*>> occupied_tiles;

  DebugInfo dbg;

  struct {
    bool active = false;
    enum {
      TILE,
      ACTOR,
    } brush_type = TILE;
    std::vector<Tile::Type> brush_tiles = {
      Tile::FLOOR,
      Tile::WALL,
    };
    long unsigned int brush_tile_selected = 0;
  } editor;

  std::list<Actor*> worker_pool;
  for (Actor* actor : actors) {
    worker_pool.push_back(actor);
  }

  SDL_WarpMouse(ui.view.w / 2, ui.view.h / 2);
  ui.mouse.x = ui.view.w / 2;
  ui.mouse.y = ui.view.h / 2;
  
  Uint32 ticks = SDL_GetTicks();

  while (running) {
    SDL_Event event;
    bool advance = false;

    while (SDL_PollEvent(&event)) {
      Point tmp;
      switch (event.type) {
        case SDL_QUIT:
          running = false;
          break;

        case SDL_KEYUP:
          switch (event.key.keysym.sym) {
            // Toggle free running
            case SDLK_f:
              freerun = !freerun;
              fprintf(stderr, "-!- %s.\n", freerun ? "Running" : "Paused");
              break;

            // Save
            case SDLK_s:
              save_state(level, actors);
              break;

            // Reset level
            case SDLK_r:
              for (Actor* actor : actors) {
                delete actor;
              }
              actors.clear();
              for (int y = 0; y < level.h; ++y) {
                for (int x = 0; x < level.w; ++x) {
                  level.tile(x, y).type = Tile::WALL;
                }
              }
              break;

            // To draw or not to draw
            case SDLK_1:
              toggle(ui.layers.tiles);
              break;
            case SDLK_2:
              toggle(ui.layers.actors);
              break;
            case SDLK_3:
              toggle(ui.layers.paths);
              break;

            case SDLK_LEFT:
              ui.scroll_view(-128, 0);
              break;

            case SDLK_RIGHT:
              ui.scroll_view(128, 0);
              break;

            case SDLK_UP:
              ui.scroll_view(0, -128);
              break;

            case SDLK_DOWN:
              ui.scroll_view(0, 128);
              break;

            case SDLK_q:
              tmp = ui.mouse_tile();
              fprintf(stderr, "(%d, %d)\n", tmp.x, tmp.y);
              break;

            case SDLK_F1:
              toggle(editor.active);
              break;

            case SDLK_w:
              if (editor.active) {
                roll_dec(editor.brush_tiles.size() - 1, editor.brush_tile_selected);
              }
              break;

            case SDLK_e:
              if (editor.active) {
                roll_inc(editor.brush_tiles.size() - 1, editor.brush_tile_selected);
              }
              break;

            case SDLK_SPACE:
              advance = true;
              fprintf(stderr, "-!- Step.\n");
              break;

            case SDLK_d:
              ui.dig();
              break;

            case SDLK_RETURN:
              ui.accept();
              break;

            case SDLK_ESCAPE:
              ui.cancel();
              break;

            case SDLK_c:
              if (!tasks.empty()) {
                delete tasks.front();
                tasks.pop_front();
              }
              break;

            default:
              break;
          }
          break;

        case SDL_MOUSEMOTION:
          ui.mouse_move(event.motion.x, event.motion.y);
          break;

        case SDL_MOUSEBUTTONDOWN:
          ui.mouse_move(event.button.x, event.button.y);
          if (event.button.button == SDL_BUTTON_LEFT) {
            ui.mouse_down();
          }
          break;

        case SDL_MOUSEBUTTONUP:
          ui.mouse_move(event.button.x, event.button.y);
          if (event.button.button == SDL_BUTTON_LEFT) {
            ui.mouse_up();
          }
          break;
      }
    }

    ui.update();

    Uint32 ticks_now = SDL_GetTicks();
    if (advance || (freerun && ticks_now - ticks > 100)) {
      //fprintf(stderr, "- Tasks: %ld\n", tasks.size());
      ticks = ticks_now;

      // Finished tasks are deleted
      tasks.remove_if([](Dig* task) {
        //fprintf(stderr, "Task %p has %ld undug tiles.\n",
        //  task, task->undug_tiles.size());
        if (task->finished()) {
          delete task;
          return true;
        }
        else {
          return false;
        }
      });

      dbg.workable_tiles.clear();
      dbg.unworkable_tiles.clear();

      Pool idle = worker_pool;
      Pool working;

      for (Dig* task : tasks) {
        // Actual work is done
        std::list<Action*> actions = task->work(dbg, level, idle, working);
        //fprintf(stderr, "- Actions -\n");
        for (Action* action : actions) {
          //fprintf(stderr, "%s\n", action->str().c_str());
          action->perform();
          delete action;
        }

        //fprintf(stderr, "Idle: %ld, Working: %ld, Useless: %ld\n",
        //    task->idle.size(), task->working.size(), task->useless.size());
      }
    }

    occupied_tiles.clear();
    for (const Actor* actor : actors) {
      occupied_tiles[actor->p].push_back(actor);
    }

    glClear(GL_COLOR_BUFFER_BIT);
    
    if (ui.layers.tiles) {
      ui.draw_level(level, tex, TILE_SIZE);
    }

    if (ui.layers.actors) {
      ui.draw_actors(occupied_tiles, TILE_SIZE, tex.actor);
    }

    for (const Dig* task : tasks) {
      for (const Point& p : task->undug_tiles) {
        ui.draw_texture(p * TILE_SIZE, tex.undug, TILE_SIZE);
      }
    }

    for (const Point& p : dbg.workable_tiles) {
      ui.draw_texture(p * TILE_SIZE, tex.remove, TILE_SIZE);
    }

    for (const Point& p : dbg.unworkable_tiles) {
      ui.draw_texture(p * TILE_SIZE, tex.remove_inaccessible, TILE_SIZE);
    }

    if (ui.layers.paths) {
      for (const auto& path : dbg.paths) {
        for (const auto& p : path) {
          ui.draw_texture(p.x * TILE_SIZE, p.y * TILE_SIZE, tex.path, TILE_SIZE);
        }
      }
    }

    GLuint brush_texture = cursor_texture(tex,
        editor.brush_tiles[editor.brush_tile_selected]);
    const bool draw_brush = editor.active && editor.brush_type == editor.TILE;

    const Point& mouse_tile = ui.mouse_tile();
    if (draw_brush) {
      ui.draw_texture(mouse_tile * TILE_SIZE, brush_texture, TILE_SIZE);
    }

    if (ui.select_for == UI::DIGGING) {
      ui.draw_texture(mouse_tile * TILE_SIZE, tex.selection, TILE_SIZE);
      for (const Point& p : ui.selected_tiles) {
        ui.draw_texture(p * TILE_SIZE, tex.selection, TILE_SIZE);
      }
    }

    if (ui.state == UI::SELECTING_DRAGGING) {
      if (draw_brush) {
        ui.draw_selection(ui.sel, TILE_SIZE, brush_texture);
      }
      ui.draw_selection(ui.sel, TILE_SIZE, tex.selection);
    }

    ui.draw_sidebar();

    SDL_GL_SwapBuffers();

    /*if (freerun) {
      SDL_Delay(50);
    }*/
  }
}


Textures load_textures () {
  Textures tex = {
    load_texture("tex/selection.png", TILE_SIZE),
    load_texture("tex/remove.png", TILE_SIZE),
    load_texture("tex/remove-inaccessible.png", TILE_SIZE),
    load_texture("tex/undug.png", TILE_SIZE),
    load_texture("tex/actor.png", TILE_SIZE / 2),
    load_texture("tex/path.png", TILE_SIZE),
    load_texture("tex/floor.png", TILE_SIZE),
    load_texture("tex/wall-full.png", TILE_SIZE),
    {
      load_texture("tex/wall.png", TILE_SIZE / 2),
      load_texture("tex/wall-L.png", TILE_SIZE / 2),
      load_texture("tex/wall-C.png", TILE_SIZE / 2),
      load_texture("tex/wall-LC.png", TILE_SIZE / 2),
      load_texture("tex/wall-T.png", TILE_SIZE / 2),
      load_texture("tex/wall-LT.png", TILE_SIZE / 2),
      load_texture("tex/wall-CT.png", TILE_SIZE / 2),
      load_texture("tex/wall-LCT.png", TILE_SIZE / 2),
    },
  };

  return tex;
}


void inner_main () {
  SDL_Init(SDL_INIT_VIDEO);

  const int win_w = 800;
  const int win_h = 600;

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_SetVideoMode(win_w, win_h, 32, SDL_OPENGL);

  glClearColor(255, 255, 255, 0);
  glViewport(0, 0, win_w, win_h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, win_w, win_h, 0.0);
  glMatrixMode(GL_MODELVIEW);

  glEnable(GL_BLEND);
  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  Textures tex = load_textures();
  
  LoadState state;
  load("sav/011.sav", state);

  //SDL_WM_GrabInput(SDL_GRAB_ON);

  Level* level = state.level;
  auto& actors = state.actors;

  std::list<Dig*> tasks;

  UI ui(win_w, win_h, *level, tasks);

  game_main(ui, tex, *level, tasks, actors);

  for (Actor* actor : actors) {
    delete actor;
  }
  actors.clear();

  delete level;
}


int main (int argc, char *argv[]) {
  try {
    inner_main();
  }
  catch (const Exception& e) {
    fprintf(stderr, "\n%s\n", e.message());
  }

  SDL_Quit();
}

