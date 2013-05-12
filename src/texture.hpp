#ifndef TUK_TEXTURE_HPP
#define TUK_TEXTURE_HPP

extern "C" {
  #include <GL/gl.h>
}


const int NULL_TEXTURE = 0;


GLuint load_texture (const char* filename, const int texture_size);


#endif // TUK_TEXTURE_HPP

