
#include "action.hpp"


Action::Action (Actor& actor) : actor(actor) {}


WaitAction::WaitAction (Actor& actor) : Action(actor) {}

void WaitAction::perform () const {}


MoveAction::MoveAction (Actor& actor, const Point& target)
  : Action(actor), target(target) {
}

void MoveAction::perform () const {
  actor.p = target;
}


DigAction::DigAction (Actor& actor, const Point& target, Level& level)
  : Action(actor), target(target), level(level) {
}

void DigAction::perform () const {
  level.tile(target.x, target.y).passable = true;
  level.tile(target.x, target.y).type = Tile::FLOOR;
}

