
#include "exception.hpp"
#include "draw.hpp"


const int TILE_SIZE = 32;


void draw_level (const Level& level, const Textures& tex, int texture_size)
{
  for (int y = 0; y < level.h; ++y) {
    for (int x = 0; x < level.w; ++x) {
      draw_tile(level, x, y, tex, texture_size);
    }
  }
}


void draw_selection (const Selection& sel, int texture_size, GLuint sel_tex) {
  sel.foreach([texture_size, sel_tex](int x, int y) {
      draw_texture(x * texture_size, y * texture_size, sel_tex, texture_size);
  });
}


void draw_actors (const std::map<Point, std::list<const Actor*>>& occupied_tiles,
    int texture_size, GLuint tex_actor) {
  for (const auto& kv : occupied_tiles) {
    const Point& p = kv.first;
    const auto& actors = kv.second;

    int headcount = actors.size();
    if (headcount > 4) {
      //raise("Too many actors on single tile: %ld", headcount); 
      headcount = 4;
    }

    const int tx = texture_size * p.x;
    const int ty = texture_size * p.y;

    const int s = texture_size / 2;

    struct D { int x; int y; };

    D d1[] = { { s/2, s/2 } };
    D d2[] = {
      {   0, s/2 },
      {   s, s/2 },
    };
    D d3[] = {
      { s/2, 0 },
      {   0, s },
      {   s, s },
    };
    D d4[] = {
      { 0, 0 },
      { s, s },
      { 0, s },
      { s, 0 },
    };

    const D* ds[4] = { d1, d2, d3, d4, };
    const D* d = ds[headcount - 1];

    for (int i = 0; i < headcount; ++i) {
      draw_texture(tx + d[i].x, ty + d[i].y, tex_actor, texture_size / 2);
    }
  }
}


void draw_tile (const Level& level, int x, int y, const Textures& tex, int texture_size) {
  const Tile& tile = level.tile(x, y);

  // Draw floor underneath anyway
  draw_texture(texture_size * x, texture_size * y, tex.floor, texture_size);

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

      draw_texture(tx, ty, tex.wall_parts[p], texture_size / 2, false, i * 90);
    }
  }
}


void draw_texture (const Point& p, GLuint texture, int texture_size) {
  draw_texture(p.x, p.y, texture, texture_size);
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

