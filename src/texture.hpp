#ifndef TUK_TEXTURE_HPP
#define TUK_TEXTURE_HPP

extern "C" {
  #include <GL/gl.h>
}

#include "tile.hpp"


const int NULL_TEXTURE = 0;


struct Textures {
  GLuint selection;
  GLuint remove;
  GLuint remove_inaccessible;
  GLuint undug;
  GLuint actor;
  GLuint actor_gold_ore;
  GLuint path;
  GLuint floor;
  GLuint gold;
  GLuint wall_full;
  GLuint wall_parts[8];
  GLuint hoard[3];
};


GLuint cursor_texture (const Textures& tex, const Tile::Type type);
GLuint load_texture (const char* filename, const int texture_size);


#endif // TUK_TEXTURE_HPP

