
#include <list>
#include "path.hpp"
#include "task.hpp"


MoveTask::MoveTask (const Level& level, Actor& actor, const Vec<int>& target)
: _level(level), _actor(actor), _target(target) {

}


void MoveTask::work () {
  std::list<Point> path;
  if (find_path(path, _level, _actor.p, _target) && path.size() > 1) {
    path.pop_front();
    _actor.p = path.front();
  }
}


bool MoveTask::ready () const {
  return (_actor.p == _target);
}

