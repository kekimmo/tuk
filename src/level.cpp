
#include "exception.hpp"
#include "level.hpp"


Level::Level (int width, int height)
  : w(width), h(height)
{
  if (w <= 0 || h <= 0) {
    raise("Invalid dimensions: %d x %d", w, h);
  }

  _tiles = new Tile[w * h];
}


Level::~Level () {
  delete _tiles;
}


bool Level::valid (int x, int y) const {
  return x >= 0 && y >= 0 && x < w && y < h;
}


Tile& Level::tile (int x, int y) const {
  if (!this->valid(x, y)) {
    raise("Coordinate outside level: (%d, %d)", x, y);
  }

  return _tiles[y * w + x];
}


bool Level::passable (int x, int y) const {
  return this->valid(x, y) && this->tile(x, y).type == Tile::FLOOR;
}


bool Level::diggable (const Point& p) const {
  return this->valid(p.x, p.y) && this->tile(p.x, p.y).hp > 0;
}


bool Level::surrounded (int x, int y) const {
  return
   !this->valid(x, y) || !(passable(x - 1, y) ||
                           passable(x + 1, y) ||
                           passable(x, y - 1) ||
                           passable(x, y + 1));
}

