#ifndef TILE_HPP_1BK7VGXJ
#define TILE_HPP_1BK7VGXJ

#include "texture.hpp"


struct Tile {
  Tile ();

  enum Type {
    UNKNOWN,
    FLOOR,
    WALL,
  };

  bool passable = false;
  Type type = UNKNOWN;
};


#endif /* end of include guard: TILE_HPP_1BK7VGXJ */

