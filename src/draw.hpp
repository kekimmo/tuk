#ifndef DRAW_HPP_NVNJMC7E
#define DRAW_HPP_NVNJMC7E

#include <vector>
#include <map>
#include <list>

extern "C" {
  #include "GL/gl.h"
}

#include "level.hpp"
#include "vec.hpp"
#include "selection.hpp"
#include "actor.hpp"


void draw_level (const Level& level, const GLuint tile_textures[], int texture_size);

void draw_selection (const Selection& sel, int texture_size, GLuint sel_tex);

void draw_actors (const std::map<Point, std::list<const Actor*>>& occupied_tiles,
    int texture_size, GLuint tex_actor);

void draw_texture (int x, int y, GLuint texture, int texture_size,
    bool center = false, int angle = 0);

void draw_tile (const Level& level, int x, int y, const GLuint tile_textures[],
    int texture_size); 

#endif /* end of include guard: DRAW_HPP_NVNJMC7E */

