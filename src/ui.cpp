
#include "exception.hpp"
#include "ui.hpp"


UI::UI (const Level& level) : level(level) {
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


Point UI::tc (const Point& p) {
  return p / TILE_SIZE;
}

