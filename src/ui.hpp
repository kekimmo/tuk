#ifndef UI_HPP_2GLE8SO5
#define UI_HPP_2GLE8SO5


#include "vec.hpp"
#include "level.hpp"
#include "selection.hpp"


struct UI {
  const int TILE_SIZE = 32;

  const Level& level;

  enum {
    IDLE,
    SELECTING,
    SELECTED,
  } state = IDLE;
  Point mouse;
  Selection sel;

  struct {
    bool tiles = true;
    bool actors = true;
    bool paths = false;
  } layers;

  UI (const Level& level);

  void mouse_move (const Point& p);
  void mouse_move (int x, int y);
  void mouse_down ();
  void mouse_up ();

  private:
    // Convert to tile coordinates
    Point tc (const Point& p);
};


#endif /* end of include guard: UI_HPP_2GLE8SO5 */

