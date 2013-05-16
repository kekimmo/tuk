#ifndef UI_HPP_2GLE8SO5
#define UI_HPP_2GLE8SO5


#include "vec.hpp"
#include "level.hpp"
#include "selection.hpp"
#include "actor.hpp"
#include "texture.hpp"

#include <map>
#include <list>

extern "C" {
  #include "GL/gl.h"
}


struct UI {
  static const int TILE_SIZE = 32;
  static const int SCROLL_AREA = 100;

  const Level& level;

  enum {
    IDLE,
    SELECTING,
    SELECTED,
  } state = IDLE;

  Area port;
  Area view;
  Area view_tiles;

  Point mouse;
  Selection sel;

  struct {
    bool tiles = true;
    bool actors = true;
    bool paths = true;
  } layers;

  UI (int w, int h, const Level& level);

  void mouse_move (const Point& p);
  void mouse_move (int x, int y);
  void mouse_down ();
  void mouse_up ();

  void update ();
  void scroll_view (int dx, int dy);

  // Get the tile the mouse is on
  Point mouse_tile () const;

  void draw_sidebar () const;
  void draw_minimap () const;
  void draw_selection (const Selection& sel, int texture_size, GLuint sel_tex);
  void draw_actors (const std::map<Point, std::list<const Actor*>>& occupied_tiles, int texture_size, GLuint tex_actor);
  void draw_level (const Level& level, const Textures& tex, int texture_size);
  void draw_tile (const Level& level, int x, int y,
      const Textures& tex, int texture_size);
  void draw_texture (const Point& p, GLuint texture, int texture_size);
  void draw_texture (int x, int y, GLuint texture, int texture_size, bool center = false, int angle = 0);

  private:
    // Pixel -> Tile coordinates
    Point tc (const Point& p) const;
    // Tile -> Pixel coordinates
    static int pc (int coord);
};


#endif /* end of include guard: UI_HPP_2GLE8SO5 */

