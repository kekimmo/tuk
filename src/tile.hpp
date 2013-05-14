#ifndef TILE_HPP_1BK7VGXJ
#define TILE_HPP_1BK7VGXJ


struct Tile {
  Tile ();

  enum Type {
    UNKNOWN = '?',
    FLOOR = ' ',
    WALL = '#',
  };
  static constexpr const char* TYPECHARS = "? #";

  Type type = UNKNOWN;
};


#endif /* end of include guard: TILE_HPP_1BK7VGXJ */

