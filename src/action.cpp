
#include "action.hpp"
#include "string.hpp"


Action::Action (Actor& actor) : actor(actor) {}


WaitAction::WaitAction (Actor& actor) : Action(actor) {}

void WaitAction::perform () const {}

std::string WaitAction::str () const {
  return format("(%d, %d): Wait", actor.p.x, actor.p.y);
}


MoveAction::MoveAction (Actor& actor, const Point& target)
  : Action(actor), target(target) {
}

void MoveAction::perform () const {
  actor.p = target;
}

std::string MoveAction::str () const {
  return format("(%d, %d): Move to (%d, %d)",
      actor.p.x, actor.p.y,
      target.x, target.y);
}


DigAction::DigAction (Actor& actor, const Point& target, Level& level)
  : Action(actor), target(target), level(level) {
}

void DigAction::perform () const {
  Tile& tile = level.tile(target.x, target.y);
  if (tile.hp > 0) {
    tile.hp -= 1;
  }
  if (tile.hp == 0) {
    tile.type = Tile::FLOOR;
  }
}

std::string DigAction::str () const {
  return format("(%d, %d): Mine (%d, %d)",
      actor.p.x, actor.p.y,
      target.x, target.y);
}


MineAction::MineAction (Actor& actor, const Point& target, Level& level)
  : Action(actor), target(target), level(level) {
}

void MineAction::perform () const {
  Tile& tile = level.tile(target.x, target.y);
  if (tile.hp > 0) {
    tile.hp -= 1;
  }
  if (tile.hp == 0) {
    tile.type = Tile::FLOOR;
  }
}

std::string MineAction::str () const {
  return format("(%d, %d): Mine to (%d, %d)",
      actor.p.x, actor.p.y,
      target.x, target.y);
}

