
#include "tile.hpp"


Tile::Tile () {
}


bool Tile::depositable () const {
  return type == HOARD && hp < 1000;
}


void tile_freshen (Tile& tile, Tile::Type type) {
  tile.hp = 0;

  switch (type) {
    case Tile::FLOOR:
      break;

    case Tile::WALL:
      tile.hp = 5;
      break;

    case Tile::GOLD:
      tile.hp = 100;
      break;

    case Tile::HOARD:
      break;
  }

  tile.type = type;
}

