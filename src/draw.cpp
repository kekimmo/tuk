
#include "draw.hpp"


void draw (const Level& level, const GLuint tile_textures[], int texture_size) {
  for (int y = 0; y < level.h; ++y) {
    for (int x = 0; x < level.w; ++x) {
      bool center = false;

      const Tile& tile = level.tile(x, y);

      GLuint tex = tile_textures[tile.type - 1];
      if (tile.type == Tile::WALL) {
        auto w = [&level](int x, int y) {
          return level.valid(x, y) && level.tile(x, y).type == Tile::WALL;
        };

        struct { int x; int y; } c[4][3] = {
          {
            { -1,  0 },
            { -1, -1 },
            {  0, -1 },
          },
          {
            {  0, -1 },
            {  1, -1 },
            {  1,  0 },
          },
          {
            {  1,  0 },
            {  1,  1 },
            {  0,  1 },
          },
          {
            {  0,  1 },
            { -1,  1 },
            { -1,  0 },
          },
        };

        struct { int dx; int dy; } where[4] = {
          { 0, 0 },
          { 1, 0 },
          { 1, 1 },
          { 0, 1 },
        };

        for (int i = 0; i < 4; ++i) {
          int p = 0;
          int bit = 1;
          for (int j = 0; j < 3; ++j) {
            if (w(x + c[i][j].x, y + c[i][j].y)) {
              p |= bit;
            }
            bit <<= 1;
          }

          const int tx = x * texture_size + where[i].dx * texture_size / 2;
          const int ty = y * texture_size + where[i].dy * texture_size / 2;

          draw_texture(tx, ty, tex + p, texture_size / 2, false, i * 90);
        }

      }
      else {
        draw_texture(texture_size * x, texture_size * y, tex, texture_size);
      }
    }
  }
}


void draw_texture (int x, int y, GLuint texture, int texture_size, bool center, int angle) {
  glBindTexture(GL_TEXTURE_2D, texture);

  glLoadIdentity();
  glTranslated(x, y, 0.0);

  glScaled(texture_size, texture_size, 1.0);

  if (!center) {
    glTranslated(0.5, 0.5, 0);
  }

  glRotated(angle, 0.0, 0.0, 1.0);

  glBegin(GL_QUADS);

  glTexCoord2d(1.0, 1.0);
  glVertex2d(.5, .5);

  glTexCoord2d(1.0, 0.0);
  glVertex2d(.5, -.5);

  glTexCoord2d(0.0, 0.0);
  glVertex2d(-.5, -.5);

  glTexCoord2d(0.0, 1.0);
  glVertex2d(-.5, .5);

  glEnd();
}

