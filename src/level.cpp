
#include "exception.hpp"
#include "level.hpp"


Level::Level (int width, int height)
  : w(width), h(height)
{
  if (w <= 0 || h <= 0) {
    raisef("Invalid dimensions: %d x %d", w, h);
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
    raisef("Coordinate outside level: (%d, %d)", x, y);
  }

  return _tiles[y * w + x];
}


Tile& Level::tile (const Point& p) const {
  return tile(p.x, p.y);
}


bool Level::passable (int x, int y) const {
  return this->valid(x, y) && this->tile(x, y).type == Tile::FLOOR;
}


bool Level::diggable (const Point& p) const {
  return this->valid(p.x, p.y) && this->tile(p).type == Tile::WALL;
}


bool Level::surrounded (int x, int y) const {
  return
   !this->valid(x, y) || !(passable(x - 1, y) ||
                           passable(x + 1, y) ||
                           passable(x, y - 1) ||
                           passable(x, y + 1));
}


bool Level::surrounded (const Point& p) const {
  return surrounded(p.x, p.y);
}


bool Level::has_tiles_of_type (Tile::Type type) const {
  std::set<Point> tiles;
  for (int i = 0; i < w * h; ++i) {
    if (_tiles[i].type == type) {
      return true;
    }
  }
  return false;
}


std::set<Point> Level::tiles_of_type (Tile::Type type) const {
  std::set<Point> tiles;
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      if (tile(x, y).type == type) {
        tiles.insert(Point(x, y));
      }
    }
  }
  return tiles;
}

