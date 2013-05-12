#ifndef TEXTURE_HPP_PTTPVNAG
#define TEXTURE_HPP_PTTPVNAG


extern "C" {
  #include <GL/gl.h>
}


const int NULL_TEXTURE = 0;


GLuint load_texture (const char* filename, const int texture_size);


#endif /* end of include guard: TEXTURE_HPP_PTTPVNAG */

