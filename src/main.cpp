
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
#include "task.hpp"


const int TILE_SIZE = 32;


void toggle (bool& var) {
  var = !var;
}


void save_state (const Level& level, const Pool& actors, const Tasklist& tasks) {
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
    tasks,
  };

  save(file, state);

  fclose(file); // this also closes fd

  printf("Saved: %s\n", name);
}


void game_main (UI& ui, const Textures& tex, Level& level, Pool& actors, Tasklist& tasks) {
  Vec<int> mouse(0, 0);

  bool running = true;
  bool freerun = false;
  std::map<Point, std::list<const Actor*>> occupied_tiles;

  DebugInfo dbg;

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
              save_state(level, actors, tasks);
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
              toggle(ui.layers.workable);
              break;
            case SDLK_4:
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

            case SDLK_i:
              tmp = ui.mouse_tile();
              fprintf(stderr, "(%d, %d): %d\n", tmp.x, tmp.y, level.tile(tmp).hp);
              break;

            case SDLK_e:
              ui.edit();
              break;

            case SDLK_SPACE:
              advance = true;
              fprintf(stderr, "-!- Step.\n");
              break;

            case SDLK_d:
              ui.dig();
              break;

            case SDLK_b:
              ui.build();
              break;

            case SDLK_q:
              ui.tile_prev();
              break;

            case SDLK_w:
              ui.tile_next();
              break;

            case SDLK_a:
              ui.editor.brush_type = (ui.editor.brush_type == ui.editor.TILE)
                ? ui.editor.ACTOR
                : ui.editor.TILE;
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
          switch (event.button.button) {
            case SDL_BUTTON_LEFT:
              ui.mouse_down();
              break;

            case SDL_BUTTON_RIGHT:
              ui.accept();
              break;

            default:
              break;
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
      tasks.remove_if([&level](Task* task) {
        //fprintf(stderr, "Task %p has %ld undug tiles.\n",
        //  task, task->undug_tiles.size());
        if (task->finished(level)) {
          delete task;
          return true;
        }
        else {
          return false;
        }
      });

      dbg.workable_tiles.clear();
      dbg.unworkable_tiles.clear();

      Pool idle = actors;
      Pool working;

      for (Task* task : tasks) {
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

    ui.draw(tex, dbg);
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
    load_texture("tex/undug-blue.png", TILE_SIZE),
    load_texture("tex/actor.png", TILE_SIZE / 2),
    load_texture("tex/actor_gold_ore.png", TILE_SIZE / 2),
    load_texture("tex/path.png", TILE_SIZE),
    load_texture("tex/floor.png", TILE_SIZE),
    load_texture("tex/gold.png", TILE_SIZE),
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
    {
      load_texture("tex/hoard-0.png", TILE_SIZE),
      load_texture("tex/hoard-50.png", TILE_SIZE),
      load_texture("tex/hoard-100.png", TILE_SIZE),
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
  load("sav/026.sav", state);

  //SDL_WM_GrabInput(SDL_GRAB_ON);

  Level* level = state.level;
  auto& actors = state.actors;
  auto& tasks = state.tasks;

  UI ui(win_w, win_h, *level, actors, tasks);

  game_main(ui, tex, *level, actors, tasks);

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

