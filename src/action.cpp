
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

