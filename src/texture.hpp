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
  GLuint undug;
  GLuint actor;
  GLuint path;
  GLuint floor;
  GLuint wall_full;
  GLuint wall_parts[8];
};


GLuint cursor_texture (const Textures& tex, const Tile::Type type);
GLuint load_texture (const char* filename, const int texture_size);


#endif // TUK_TEXTURE_HPP

