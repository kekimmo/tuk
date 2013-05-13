
#include <list>
#include "path.hpp"
#include "task.hpp"


GoTask::GoTask (const Level& level, Actor& actor, const Vec<int>& target)
: _level(level), _actor(actor), _target(target) {

}


Action* GoTask::work () {
  std::list<Point> path;
  if (find_path(path, _level, _actor.p, _target) && path.size() > 1) {
    path.pop_front();
    return new MoveAction(_actor, path.front());
  }
  else {
    return new WaitAction(_actor);
  }
}


bool GoTask::ready () const {
  return (_actor.p == _target);
}

