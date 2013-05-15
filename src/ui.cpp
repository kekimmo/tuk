
#include <algorithm>
#include "exception.hpp"
#include "ui.hpp"


UI::UI (int w, int h, const Level& level)
  : level(level), port(w, h), view(Area { 0, 0, w, h }),
    view_tiles(Area { 0, 0, view.w / TILE_SIZE + 2, view.h / TILE_SIZE + 2 }),
    mouse(port.x / 2, port.y / 2) {
}


void UI::mouse_move (const Point& p) {
  mouse_move(p.x, p.y);
}


void UI::mouse_move (int x, int y) {
  mouse.x = x;
  mouse.y = y;

  switch (state) {
    case SELECTING:
      sel.update(tc(mouse).clamped(0, 0, level.w - 1, level.h - 1));
      break;

    default:
      break;
  }
}


void UI::mouse_down () {
  switch (state) {
    case IDLE:
    case SELECTED:
      sel.start(tc(mouse));
      state = SELECTING;
      break;

    default:
      break;
  }
}


void UI::mouse_up () {
  switch (state) {
    case SELECTING:
      sel.finish();
      state = SELECTED;
      break;

    default:
      break;
  }
}


void UI::update () {
  const int d = 10; 
  int dx = 0;
  int dy = 0;

  if (mouse.x < SCROLL_AREA) {
    dx -= d;
  }
  if (mouse.x > view.w - SCROLL_AREA) {
    dx += d;
  }
  if (mouse.y < SCROLL_AREA) {
    dy -= d;
  }
  if (mouse.y > view.h - SCROLL_AREA) {
    dy += d;
  }
  
  if (dx != 0 || dy != 0) {
    scroll_view(dx, dy);
  }
}


void UI::scroll_view (int dx, int dy) {
  view.x += dx;
  view.y += dy;

  const int right = level.w * TILE_SIZE;
  const int bottom = level.h * TILE_SIZE;

  if (view.x + view.w > right) {
    view.x = right - view.w;
  }
  if (view.y + view.h > bottom) {
    view.y = bottom - view.h;
  }

  if (view.x < 0) {
    view.x = 0;
  }
  if (view.y < 0) {
    view.y = 0;
  }
}


Point UI::mouse_tile () const {
  return tc(mouse).clamped(0, 0, level.w - 1, level.h - 1);
}


Point UI::tc (const Point& p) const {
  return Point((p.x + view.x) / TILE_SIZE, (p.y + view.y) / TILE_SIZE);
}


int UI::pc (int coord) {
  return coord * TILE_SIZE;
}


void UI::draw_selection (const Selection& sel, int texture_size, GLuint sel_tex) {
  sel.foreach([this, texture_size, sel_tex](int x, int y) {
      draw_texture(x * texture_size, y * texture_size, sel_tex, texture_size);
  });
}


void UI::draw_actors (const std::map<Point, std::list<const Actor*>>& occupied_tiles,
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

void UI::draw_level (const Level& level, const Textures& tex,
    int texture_size)
{
  const int x1 = view.x / TILE_SIZE;
  const int y1 = view.y / TILE_SIZE;
  const int x2 = std::min(x1 + view_tiles.w, level.w);
  const int y2 = std::min(y1 + view_tiles.h, level.h);

  for (int y = y1; y < y2; ++y) {
    for (int x = x1; x < x2; ++x) {
      draw_tile(level, x, y, tex, texture_size);
    }
  }
}


void UI::draw_tile (const Level& level, int x, int y,
    const Textures& tex, int texture_size) {
  const Tile& tile = level.tile(x, y);

  const int px = pc(x);
  const int py = pc(y);

  if (tile.type == Tile::FLOOR) {
    draw_texture(px, py, tex.floor, texture_size);
  }
  else if (tile.type == Tile::WALL) {
    auto w = [&level](int x, int y) {
      return level.valid(x, y) &&
        level.tile(x, y).type == Tile::WALL;
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

      const int tx = px + where[i].dx * texture_size / 2;
      const int ty = py + where[i].dy * texture_size / 2;

      draw_texture(tx, ty, tex.wall_parts[p], texture_size / 2,
          false, i * 90);
    }
  }
}


void UI::draw_texture (const Point& p, GLuint texture, int texture_size) {
  draw_texture(p.x, p.y, texture, texture_size);
}


void UI::draw_texture (int x, int y, GLuint texture, int texture_size, bool center, int angle) {
  glBindTexture(GL_TEXTURE_2D, texture);

  glLoadIdentity();
  glTranslated(x - view.x, y - view.y, 0.0);

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

