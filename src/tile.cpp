
#include "tile.hpp"


Tile::Tile () {
}


void tile_freshen (Tile& tile, Tile::Type type) {
  switch (type) {
    case Tile::FLOOR:
      tile.hp = 0;
      break;

    case Tile::WALL:
      tile.hp = 5;
      break;

    case Tile::GOLD:
      tile.hp = 100;
      break;
  }

  tile.type = type;
}

