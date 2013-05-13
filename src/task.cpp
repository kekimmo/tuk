
#include "task.hpp"


MoveTask::MoveTask (const Level& level, Actor& actor, const Vec<int>& target)
: _level(level), _actor(actor), _target(target) {

}


void MoveTask::work () {
}


bool MoveTask::ready () const {
  return (_actor.p == _target);
}

