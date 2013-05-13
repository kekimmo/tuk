
#include <cstdio>
#include <cmath>
#include <cstdarg>
#include <cerrno>
#include <cstring>
#include <cassert>
#include <vector>

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
#include "load_level.hpp"
#include "draw.hpp"
#include "actor.hpp"
#include "collide.hpp"
#include "save.hpp"


const int TEXTURE_SIZE = 32;


void save_state (const Level& level) {
  char name[8];

  int num = 0;
  int fd = -1;

  do {
    if (num > 999) {
      raise("More than %d save files!", 999);
    }
    snprintf(name, 8, "%03d.sav", num);
    num += 1;
    fd = open(name, O_CREAT | O_EXCL | O_WRONLY, 0644);
    if (fd == -1 && errno != EEXIST) {
      raise("Failed to open save file %s: %s", name, std::strerror(errno));
    }
  }
  while (fd == -1);

  FILE* file = fdopen(fd, "wb");
  if (file == NULL) {
    raise("Failed to open save file %s: %s", name, std::strerror(errno));
  }

  save(file, level);

  fclose(file); // this also closes fd

  printf("Saved: %s\n", name);
}


void inner_main () {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Event event;
  bool running = true;

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

  GLuint tile_textures[] = {
    load_texture("tex/floor.png", TEXTURE_SIZE),
    load_texture("tex/wall.png", TEXTURE_SIZE / 2),
    load_texture("tex/wall-L.png", TEXTURE_SIZE / 2),
    load_texture("tex/wall-C.png", TEXTURE_SIZE / 2),
    load_texture("tex/wall-LC.png", TEXTURE_SIZE / 2),
    load_texture("tex/wall-T.png", TEXTURE_SIZE / 2),
    load_texture("tex/wall-LT.png", TEXTURE_SIZE / 2),
    load_texture("tex/wall-CT.png", TEXTURE_SIZE / 2),
    load_texture("tex/wall-LCT.png", TEXTURE_SIZE / 2),
  };

  GLuint tex_sel = load_texture("tex/selection.png", TEXTURE_SIZE);
  GLuint tex_actor = load_texture("tex/actor.png", TEXTURE_SIZE);

  //Level* level = load_level("lev/test.lev");
  LoadState state = load("003.sav");
  Level* level = state.level;

  /* int frame = 0; */

  Vec<int> mouse(0, 0);
  std::vector<Actor*> actors;

  Selection sel;

  while (running) {
    /* const auto tick = SDL_GetTicks(); */

    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          running = false;
          break;

        case SDL_KEYUP:
          switch (event.key.keysym.sym) {
            case SDLK_s:
              save_state(*level);
              break;

            default:
              break;
          }

        case SDL_MOUSEMOTION:
          mouse.x = event.motion.x;
          mouse.y = event.motion.y;
          if (sel.started) {
            sel.update((mouse / TEXTURE_SIZE).clamped(level->w - 1, level->h - 1));
          }
          break;

        case SDL_MOUSEBUTTONDOWN:
          if (event.button.button == SDL_BUTTON_LEFT && !sel.started) {
            sel.start((mouse / TEXTURE_SIZE).clamped(level->w - 1, level->h - 1));
          }
          break;

        case SDL_MOUSEBUTTONUP:
          if (event.button.button == SDL_BUTTON_LEFT) {
            sel.finish();
            sel.foreach([level](int x, int y) {
              level->tile(x, y).type = Tile::FLOOR;
              level->tile(x, y).passable = true;
            });
          }
          else if (event.button.button == SDL_BUTTON_RIGHT) {
            actors.push_back(new Actor(mouse.x, mouse.y, 16));
          }

          break;
      }
    }

    for (int i = 0; i < 10; ++i) {
      bool moved = false;
      for (Actor* actor : actors) {
        if (collide_level_actor(*level, *actor, TEXTURE_SIZE)) {
          moved = true;
        }
      }
      for (Actor* a : actors) {
        for (Actor* b : actors) {
          if (collide_actor_actor(*a, *b)) {
            moved = true;
          }
        }
      }
      if (!moved) {
        break;
      }
    }

    glClear(GL_COLOR_BUFFER_BIT);

    draw_level(*level, tile_textures, TEXTURE_SIZE);
    if (sel.started) {
      draw_selection(sel, TEXTURE_SIZE, tex_sel);
    }
    draw_actors(actors, TEXTURE_SIZE, tex_actor);

    SDL_GL_SwapBuffers();

    /* const float fps = 1.0 / ((SDL_GetTicks() - tick) / 1000.0); */
    //fprintf(stderr, "FPS: %f\n", fps);
  }

  for (Actor* actor : actors) {
    delete actor;
  }

  actors.clear();
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

