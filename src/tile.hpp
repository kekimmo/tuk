#ifndef TILE_HPP_1BK7VGXJ
#define TILE_HPP_1BK7VGXJ


struct Tile {
  Tile ();

  enum Type {
    FLOOR = ' ',
    WALL = '#',
    GOLD = '$',
    HOARD = 'H',
  };
  static constexpr const char* TYPECHARS = " #$H";

  Type type = WALL;
  int hp = 0;

  bool depositable () const;
};


void tile_freshen (Tile& tile, Tile::Type type);


#endif /* end of include guard: TILE_HPP_1BK7VGXJ */

