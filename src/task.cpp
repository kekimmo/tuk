
#include <list>
#include "path.hpp"
#include "task.hpp"


GoTask::GoTask (const Level& level, Actor& actor, const Vec<int>& target)
: _level(level), _actor(actor), _target(target) {
}


Action* GoTask::work (DebugInfo& dbg) {
  if (_path.empty()) {
    if (find_path(_path, _level, _actor.p, _target) && _path.size() > 1) {
      _path.pop_front();
      dbg.paths.push_back(_path);
    }
    return new WaitAction(_actor);
  }
  else {
    Point next = _path.front();
    _path.pop_front();
    dbg.paths.push_back(_path);
    return new MoveAction(_actor, next);
  }
}


bool GoTask::ready () const {
  return (_actor.p == _target);
}

