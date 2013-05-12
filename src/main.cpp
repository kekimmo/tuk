
#include <cstdio>
#include <cmath>
#include <cstdarg>
#include <cerrno>
#include <cstring>
#include <cassert>

#define GL_GLEXT_PROTOTYPES
extern "C" {
  #include <SDL/SDL.h>
  #include <GL/gl.h>
  #include <GL/glu.h>
}

#include "exception.hpp"
#include "vec.hpp"
#include "selection.hpp"
#include "texture.hpp"
#include "level.hpp"
#include "load_level.hpp"
#include "draw.hpp"


const int TEXTURE_SIZE = 32;


void inner_main () {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_Event event;
  bool running = true;

  const int win_w = 800;
  const int win_h = 600;

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  SDL_Surface* screen = SDL_SetVideoMode(win_w, win_h, 32, SDL_OPENGL);

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

  Level* level = load_level("lev/test.lev");

  int frame = 0;

  Vec<int> mouse(0, 0);

  Selection sel;

  while (running) {
    const auto tick = SDL_GetTicks();

    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          running = false;
          break;

        case SDL_MOUSEMOTION:
          mouse.x = event.motion.x;
          mouse.y = event.motion.y;
          if (sel.started) {
            sel.update(mouse / TEXTURE_SIZE);
          }
          break;

        case SDL_MOUSEBUTTONDOWN:
          if (!sel.started) {
            sel.start(mouse / TEXTURE_SIZE);
            fprintf(stderr, "Started selection at (%d, %d)\n", sel.from.x, sel.from.y);
          }
          break;

        case SDL_MOUSEBUTTONUP:
          sel.finish();
          break;
      }
    }

    if (sel.finished) {
      sel.foreach([level](int x, int y) {
          level->tile(x, y).type = Tile::FLOOR;
          level->tile(x, y).passable = true;
      });
    }

    glClear(GL_COLOR_BUFFER_BIT);
    draw(*level, tile_textures, TEXTURE_SIZE, sel, tex_sel);

    glEnd();

    SDL_GL_SwapBuffers();

    const float fps = 1.0 / ((SDL_GetTicks() - tick) / 1000.0);
    //fprintf(stderr, "FPS: %f\n", fps);
  }
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

