#ifndef DRAW_HPP_NVNJMC7E
#define DRAW_HPP_NVNJMC7E


extern "C" {
  #include "GL/gl.h"
}

#include "level.hpp"


void draw (const Level& level, const GLuint tile_textures[], int texture_size);

void draw_texture (int x, int y, GLuint texture, int texture_size, bool center = false, int angle = 0);

#endif /* end of include guard: DRAW_HPP_NVNJMC7E */

