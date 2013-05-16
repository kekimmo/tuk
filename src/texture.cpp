
#include <cerrno>
#include <cstring>
#include "texture.hpp"
#include "exception.hpp"

extern "C" {
  #include <png.h>
}


GLuint cursor_texture (const Textures& tex, const Tile::Type type) {
  switch (type) {
    case Tile::FLOOR:
      return tex.floor;

    case Tile::WALL:
      return tex.wall_full;

    default:
      return NULL_TEXTURE;
  }
}


GLuint load_texture (const char* filename, const int texture_size) {
  FILE* file = fopen(filename, "rb");
  if (file == NULL) {
    raisef("Failed to open %s: %s", filename, strerror(errno));
    return NULL_TEXTURE;
  }

  static const int HEADER_SIZE = 8;

  png_byte header[HEADER_SIZE];
  fread(header, sizeof(png_byte), HEADER_SIZE, file);

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
      NULL, NULL, NULL);

  png_infop info_ptr = png_create_info_struct(png_ptr);
  png_infop end_info_ptr = png_create_info_struct(png_ptr);

  png_init_io(png_ptr, file);
  png_set_sig_bytes(png_ptr, HEADER_SIZE);
  png_read_info(png_ptr, info_ptr);

  const size_t row_bytes = png_get_rowbytes(png_ptr, info_ptr);
  png_byte data[texture_size * row_bytes];
  for (int row = 0; row < texture_size; ++row) {
    png_read_row(png_ptr, data + row * row_bytes, NULL);
  }

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_size, texture_size, 0,
      GL_RGBA, GL_UNSIGNED_BYTE, data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);

  fclose(file);

  return texture;
}

