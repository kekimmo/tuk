
#include "action.hpp"
#include "string.hpp"
#include "exception.hpp"
#include <algorithm>


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
  if (tile.type == Tile::GOLD && actor.has_space()) {
    tile.hp -= 1;
    actor.get(Item::GOLD_ORE);
    if (tile.hp <= 0) {
      tile.type = Tile::FLOOR;
    }
  }
  else {
    // Failed to mine (perhaps someone else did it first?)
  }
}

std::string MineAction::str () const {
  return format("(%d, %d): Mine to (%d, %d)",
      actor.p.x, actor.p.y,
      target.x, target.y);
}


DepositAction::DepositAction (Actor& actor, const Point& target, Level& level)
  : Action(actor), target(target), level(level) {
}

void DepositAction::perform () const {
  Tile& tile = level.tile(target.x, target.y);
  const int amount = std::count(actor.items.cbegin(), actor.items.cend(),
      Item::GOLD_ORE);
  const int max_deposit = std::min(amount, 999 - tile.hp);
  if (tile.depositable() && max_deposit > 0) {
    tile.hp += max_deposit;
    for (int i = 0; i < max_deposit; ++i) {
      actor.items.remove(Item::GOLD_ORE);
    }
  }
}

std::string DepositAction::str () const {
  return format("(%d, %d): Deposit to (%d, %d)",
      actor.p.x, actor.p.y,
      target.x, target.y);
}


