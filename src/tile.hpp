#ifndef TILE_HPP_1BK7VGXJ
#define TILE_HPP_1BK7VGXJ


struct Tile {
  Tile ();

  enum Type {
    FLOOR = ' ',
    WALL = '#',
  };
  static constexpr const char* TYPECHARS = " #";

  Type type = FLOOR;
  int hp = 0;
};


#endif /* end of include guard: TILE_HPP_1BK7VGXJ */

