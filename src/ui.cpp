
#include <algorithm>
#include "exception.hpp"
#include "ui.hpp"


UI::UI (int w, int h, Level& level, Pool& actors, Tasklist& tasks)
  : level(level), actors(actors), tasks(tasks),
    port(0, 0, w, h),
    view(0, 0, w - 200, h),
    view_tiles(0, 0, view.w / TILE_SIZE + 1, view.h / TILE_SIZE + 1),
    mouse(port.w / 2, port.h / 2) {
}


void UI::dig () {
  if (state == IDLE) {
    start_selecting(DIGGING);
  }
  else {
    whine("Tried to enter dig mode while busy.");
  }
}


void UI::build () {
  if (state == IDLE) {
    start_selecting(BUILDING);
  }
  else {
    whine("Tried to enter build mode while busy.");
  }
}


void UI::edit () {
  if (state == IDLE) {
    start_selecting(EDITING);
  }
  else {
    whine("Tried to enter edit mode while busy.");
  }
}


void UI::accept () {
  switch (select_for) {
    case NOTHING:
      whine("Tried to accept nothing.");
      break;

    case EDITING:
      for (const Point& p : finish_selecting()) {
        tile_freshen(level.tile(p), editor.brush_tiles[editor.brush_tile_selected]);
      }
      // Immediately enter editing mode again
      start_selecting(EDITING);
      break;

    case DIGGING:
      start_digging(finish_selecting());
      break;

    case BUILDING:
      finish_selecting();
      break;
  }
}


void UI::cancel () {
  if (select_for == NOTHING) {
    whine("Tried to cancel nothing.");
  }
  else {
    select_for = NOTHING;
    cancel_selecting();
  }
}


void UI::start_digging (const std::set<Point>& tiles) {
  // Get all tiles already ordered to be dug
  std::set<Point> all_undug_tiles;
  for (Dig* task : tasks) {
    all_undug_tiles.insert(task->undug_tiles.begin(), task->undug_tiles.end());
  }

  // Get all tiles that can be dug and aren't already ordered to be
  std::set<Point> tiles_to_be_dug;
  for (const Point& p : tiles) {
    if (level.diggable(p) && !all_undug_tiles.count(p)) {
      tiles_to_be_dug.insert(p);
    }
  }
  // Order them to be dug
  tasks.push_back(new Dig(tiles_to_be_dug));
}


void UI::start_selecting (Mode mode) {
  if (state == IDLE) {
    select_for = mode;
    selected_tiles.clear();
    state = SELECTING;
  }
  else {
    raise("Tried to start selecting when already selecting");
  }
}


void UI::cancel_selecting () {
  switch (state) {
    case SELECTING:
    case SELECTING_DRAGGING:
    case SELECTED:
      state = IDLE;
      break;

    default:
      raise("Tried to cancel selecting when not selecting");
      break;
  }
}


const std::set<Point>& UI::finish_selecting () {
  switch (state) {
    case IDLE:
      raise("Tried to finish selecting when not selecting");

    case SELECTING_DRAGGING: // just ignore the last selection
      //raise("Tried to finish selecting while dragging");
    case SELECTING: // just return nothing
     // raise("Tried to finish selecting even though nothing has been selected");
    case SELECTED:
      state = IDLE;
      select_for = NOTHING;
      return selected_tiles;

    default:
      raise("Unexpected state");
  }
}


void UI::mouse_move (const Point& p) {
  mouse_move(p.x, p.y);
}


void UI::mouse_move (int x, int y) {
  mouse.x = x;
  mouse.y = y;

  switch (state) {
    case MINIMAP_DRAGGING:
      center_view_minimap(x, y);
      break;

    case SELECTING_DRAGGING:
      sel.update(tc(mouse).clamped(0, 0, level.w - 1, level.h - 1));
      break;

    default:
      break;
  }
}


void UI::mouse_down () {
  switch (state) {
    case IDLE:
      if (minimap.contains(mouse)) {
        state = MINIMAP_DRAGGING;
        center_view_minimap(mouse.x, mouse.y);
      }
      break;

    case SELECTING:
    case SELECTED:
      state = SELECTING_DRAGGING;
      sel.start(tc(mouse).clamped(0, 0, level.w - 1, level.h - 1));
      break;

    case MINIMAP_DRAGGING:
    case SELECTING_DRAGGING:
      //raise("Logic error: mouse_down() when it should already be down");
      // Apparently this order sometimes happens, don't crash
      break;
  }
}


void UI::mouse_up () {
  switch (state) {
    case IDLE:
      // just a random click that means nothing
      break;

    case MINIMAP_DRAGGING:
      state = IDLE;
      break;

    case SELECTING:
    case SELECTED:
      //raise("Logic error: mouse_up() when it should already be up");
      // Apparently this order sometimes happens, don't crash
      break;

    case SELECTING_DRAGGING:
      sel.finish();
      sel.foreach([this](const Point& p) {
        this->selected_tiles.insert(p);
      });
      state = SELECTED;
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
  if (mouse.x > port.w - SCROLL_AREA) {
    dx += d;
  }
  if (mouse.y < SCROLL_AREA) {
    dy -= d;
  }
  if (mouse.y > port.h - SCROLL_AREA) {
    dy += d;
  }
  
  if (dx != 0 || dy != 0) {
    scroll_view(dx, dy);
  }
}


void UI::scroll_view (int dx, int dy) {
  set_view(view.x += dx, view.y += dy);
}


void UI::center_view (int x, int y) {
  set_view(x - view.w / 2, y - view.h / 2);
}


void UI::center_view_minimap (int x, int y) {
  const int vx = pc((x - minimap.x) / minimap_scale.x);
  const int vy = pc((y - minimap.y) / minimap_scale.y);
  center_view(vx, vy);
}


void UI::set_view (int x, int y) {
  const int right = level.w * TILE_SIZE;
  const int bottom = level.h * TILE_SIZE;

  if (x + view.w > right) {
    x = right - view.w;
  }
  if (y + view.h > bottom) {
    y = bottom - view.h;
  }

  if (x < 0) {
    x = 0;
  }
  if (y < 0) {
    y = 0;
  }

  view.x = x;
  view.y = y;
  view_tiles.x = x / TILE_SIZE;
  view_tiles.y = y / TILE_SIZE;
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


void UI::draw (const Textures& tex, const DebugInfo& dbg) const {
  std::map<Point, std::list<const Actor*>> occupied_tiles;

  for (const Actor* actor : actors) {
    occupied_tiles[actor->p].push_back(actor);
  }

  glClear(GL_COLOR_BUFFER_BIT);
  
  if (layers.tiles) {
    draw_level(level, tex, TILE_SIZE);
  }

  if (layers.actors) {
    draw_actors(occupied_tiles, TILE_SIZE, tex.actor);
  }

  for (const Dig* task : tasks) {
    for (const Point& p : task->undug_tiles) {
      draw_texture(p * TILE_SIZE, tex.undug, TILE_SIZE);
    }
  }

  for (const Point& p : dbg.workable_tiles) {
    draw_texture(p * TILE_SIZE, tex.remove, TILE_SIZE);
  }

  //for (const Point& p : dbg.unworkable_tiles) {
  //  draw_texture(p * TILE_SIZE, tex.remove_inaccessible, TILE_SIZE);
  //}

  if (layers.paths) {
    for (const auto& path : dbg.paths) {
      for (const auto& p : path) {
        draw_texture(p.x * TILE_SIZE, p.y * TILE_SIZE, tex.path, TILE_SIZE);
      }
    }
  }

  const GLuint brush = cursor_texture(tex,
      editor.brush_tiles[editor.brush_tile_selected]);
  const bool editing = select_for == EDITING;

  const Point& mt = mouse_tile();

  if (select_for != NOTHING) {
    if (editing) {
      draw_texture(mt * TILE_SIZE, brush, TILE_SIZE);
    }
    draw_texture(mt * TILE_SIZE, tex.selection, TILE_SIZE);
    for (const Point& p : selected_tiles) {
      draw_texture(p * TILE_SIZE, tex.selection, TILE_SIZE);
    }
  }

  if (state == SELECTING_DRAGGING) {
    draw_selection(sel, TILE_SIZE, tex.selection);
  }

  draw_sidebar();
}


void UI::draw_sidebar () const {

  glDisable(GL_TEXTURE_2D);
  glLoadIdentity();

  glTranslatef(sidebar.x, sidebar.y, 0);

  glColor3ub(100, 100, 100);
  glBegin(GL_QUADS);
  glVertex2i(sidebar.w, sidebar.h);
  glVertex2i(sidebar.w, 0);
  glVertex2i(0, 0);
  glVertex2i(0, sidebar.h);
  glEnd();

  draw_minimap();
}


void UI::draw_minimap () const {
  glLoadIdentity();

  glTranslated(minimap.x, minimap.y, 0.0);
  glScaled(minimap_scale.x, minimap_scale.y, 0.0);

  // Background
  /* glBegin(GL_QUADS); */
  /* glColor3ub(0, 0, 0); */
  /* glVertex2i(level.w, level.h); */
  /* glVertex2i(level.w, 0); */
  /* glVertex2i(0, 0); */
  /* glVertex2i(0, level.h); */
  /* glEnd(); */

  // Actual map
  glColor3ub(255, 178, 127);
  glPointSize(1.5);
  glBegin(GL_POINTS);
  for (int y = 0; y < level.w; ++y) {
    for (int x = 0; x < level.h; ++x) {
      Tile::Type type = level.tile(x, y).type;
      if (type != Tile::WALL) {
        glColor3ub(255, 178, 127);
      }
      else {
        glColor3ub(127, 51, 0);
      }
      glVertex2d(x, y);
    }
  }
  glEnd();

  // Viewport
  glTranslated(view_tiles.x, view_tiles.y, 0);
  glColor3ub(255, 255, 255);
  glBegin(GL_LINE_LOOP);
  glVertex2i(view_tiles.w, view_tiles.h);
  glVertex2i(view_tiles.w, 0);
  glVertex2i(0, 0);
  glVertex2i(0, view_tiles.h);
  glEnd();
}


void UI::draw_selection (const Selection& sel, int texture_size, GLuint sel_tex) const {
  sel.foreach([this, texture_size, sel_tex](int x, int y) {
      draw_texture(x * texture_size, y * texture_size, sel_tex, texture_size);
  });
}


void UI::draw_actors (const std::map<Point, std::list<const Actor*>>& occupied_tiles,
    int texture_size, GLuint tex_actor) const {
  for (const auto& kv : occupied_tiles) {
    const Point& p = kv.first;
    const auto& actors = kv.second;

    int headcount = actors.size();
    if (headcount > 4) {
      //raisef("Too many actors on single tile: %ld", headcount); 
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
    int texture_size) const
{
  const int x1 = view.x / TILE_SIZE;
  const int y1 = view.y / TILE_SIZE;
  const int x2 = std::min(x1 + view_tiles.w + 1, level.w);
  const int y2 = std::min(y1 + view_tiles.h + 1, level.h);

  for (int y = y1; y < y2; ++y) {
    for (int x = x1; x < x2; ++x) {
      draw_tile(level, x, y, tex, texture_size);
    }
  }
}


void UI::draw_tile (const Level& level, int x, int y,
    const Textures& tex, int texture_size) const {
  const Tile& tile = level.tile(x, y);

  const int px = pc(x);
  const int py = pc(y);

  // Draw the floor underneath anyway
  draw_texture(px, py, tex.floor, texture_size);

  if (tile.type == Tile::WALL || tile.type == Tile::GOLD) {
    auto w = [&level](int x, int y) {
      return !level.valid(x, y) ||
        level.tile(x, y).type == Tile::WALL ||
        level.tile(x, y).type == Tile::GOLD;
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

    if (tile.type == Tile::GOLD) {
      draw_texture(px, py, tex.gold, texture_size);
    }
  }
}


void UI::draw_texture (const Point& p, GLuint texture, int texture_size) const {
  draw_texture(p.x, p.y, texture, texture_size);
}


void UI::draw_texture (int x, int y, GLuint texture, int texture_size, bool center, int angle) const {
  glEnable(GL_TEXTURE_2D);
  glLoadIdentity();

  glBindTexture(GL_TEXTURE_2D, texture);

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


void UI::whine (const char* msg) const {
  fprintf(stderr, "%s\n", msg);
}


template<typename T> void roll_inc (const T limit, T& value) {
  value = (value < limit) ? (value + 1) : 0;
}

template<typename T> void roll_dec (const T limit, T& value) {
  value = (value == 0) ? limit : (value - 1);
}


void UI::tile_prev () {
  roll_dec(editor.brush_tiles.size() - 1, editor.brush_tile_selected);
}


void UI::tile_next () {
  roll_inc(editor.brush_tiles.size() - 1, editor.brush_tile_selected);
}



