#ifndef LEVEL_HPP_BRKLIAXL
#define LEVEL_HPP_BRKLIAXL

#include "tile.hpp"
#include "vec.hpp"


class Level
{
public:
  Level (int width, int height);
  virtual ~Level ();

  bool valid (int x, int y) const;
  Tile& tile (int x, int y) const;
  bool passable (int x, int y) const;
  bool diggable (const Point& p) const;
  bool surrounded (int x, int y) const;

  const int w;
  const int h;

private:
  Tile* _tiles;
};


#endif /* end of include guard: LEVEL_HPP_BRKLIAXL */

