#ifndef TILE_HPP_1BK7VGXJ
#define TILE_HPP_1BK7VGXJ


struct Tile {
  Tile ();

  enum Type {
    FLOOR = ' ',
    WALL = '#',
    GOLD = '$',
  };
  static constexpr const char* TYPECHARS = " #$";

  Type type = WALL;
  int hp = 0;
};


void tile_freshen (Tile& tile, Tile::Type type);


#endif /* end of include guard: TILE_HPP_1BK7VGXJ */

