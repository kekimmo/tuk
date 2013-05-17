#ifndef UI_HPP_2GLE8SO5
#define UI_HPP_2GLE8SO5


#include "vec.hpp"
#include "level.hpp"
#include "selection.hpp"
#include "actor.hpp"
#include "texture.hpp"
#include "task.hpp"

#include <map>
#include <list>

extern "C" {
  #include "GL/gl.h"
}


struct UI {
  static const int TILE_SIZE = 32;
  static const int SCROLL_AREA = 40;
  static const int MINIMAP_BORDER = 40;

  Level& level;
  Pool& actors;
  Tasklist& tasks;

  bool selecting = false;
  bool dragging = false;
  std::set<Point> selected_tiles;

  const Area port;
  Area view;
  Area view_tiles;

  const Area sidebar = { view.w, 0, port.w - view.w, port.h };
  const Area minimap = {
      sidebar.x + MINIMAP_BORDER, sidebar.y + MINIMAP_BORDER,
      sidebar.w - 2 * MINIMAP_BORDER, sidebar.h / 4 - 2 * MINIMAP_BORDER };
  const Vec<double> minimap_scale = { (double)minimap.w / level.w,
                                      (double)minimap.h / level.h };

  Point mouse;
  Selection sel;

  enum {
    IDLE,
    MINIMAP_DRAGGING,
    SELECTING,
    SELECTING_DRAGGING,
    SELECTED,
  } state = IDLE;

  enum Mode {
    NOTHING,
    EDITING,
    DIGGING,
    BUILDING,
  };
  Mode select_for = NOTHING;

  struct {
    bool tiles = true;
    bool actors = true;
    bool paths = true;
  } layers;

  struct {
    enum {
      TILE,
      ACTOR,
    } brush_type = TILE;
    std::vector<Tile::Type> brush_tiles = {
      Tile::FLOOR,
      Tile::WALL,
      Tile::GOLD,
      Tile::HOARD,
    };
    long unsigned int brush_tile_selected = 0;
  } editor;

  UI (int w, int h, Level& level, Pool& actors, Tasklist& tasks);

  void dig ();
  void build ();
  void edit ();
  void tile_prev ();
  void tile_next ();

  void accept ();
  void cancel ();

  void start_selecting (Mode mode);
  void cancel_selecting ();
  const std::set<Point>& finish_selecting ();

  void mouse_move (const Point& p);
  void mouse_move (int x, int y);
  void mouse_down ();
  void mouse_up ();

  void update ();
  void scroll_view (int dx, int dy);
  void center_view (int x, int y);
  void center_view_minimap (int x, int y);
  void set_view (int x, int y);

  // Get the tile the mouse is on
  Point mouse_tile () const;

  void draw (const Textures& tex, const DebugInfo& dbg) const;

  void draw_sidebar () const;
  void draw_minimap () const;
  void draw_selection (const Selection& sel, int texture_size, GLuint sel_tex) const;
  void draw_actors (const std::map<Point, std::list<const Actor*>>& occupied_tiles, int texture_size, const Textures& tex) const;
  void draw_level (const Level& level, const Textures& tex, int texture_size) const;
  void draw_tile (const Level& level, int x, int y,
      const Textures& tex, int texture_size) const;
  void draw_texture (const Point& p, GLuint texture, int texture_size) const;
  void draw_texture (int x, int y, GLuint texture, int texture_size, bool center = false, int angle = 0) const;

  private:
    // Pixel -> Tile coordinates
    Point tc (const Point& p) const;
    // Tile -> Pixel coordinates
    static int pc (int coord);
    void start_digging (const std::set<Point>& tiles);
    void whine (const char* msg) const;
};


#endif /* end of include guard: UI_HPP_2GLE8SO5 */

