
#include <list>
#include "path.hpp"
#include "exception.hpp"
#include "task.hpp"


GoTask::GoTask (const Level& level, Actor& actor, const Point& target)
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


DigTask::DigTask (Level& level, Actor& actor, const Point& target)
: _level(level), _actor(actor), _target(target), _subtask(0), _state(STARTING) {
}


bool DigTask::find_digging_place (Point& point) const {
  std::list<Point> path;
  if (find_path(path, _level, _target, _actor.p)) {
    if (path.size() < 2) {
      raise("Path should have at least 2 elements: has %d", path.size());
    }
    path.pop_front();
    point = path.front();
    return true;
  }
  else {
    return false;
  }
}


Action* DigTask::work (DebugInfo& dbg) {
  Action* action = 0;
  switch (_state) {
    case STARTING:
      if (find_digging_place(_digging_place)) {
        _subtask = new GoTask(_level, _actor, _digging_place); 
        _state = FINDING;
      }

    case FINDING:
      action = _subtask->work(dbg);
      if (_subtask->ready()) {
        delete _subtask;
        _subtask = 0;
        _state = DIGGING;
      }
      break;

    case DIGGING:
      if (_level.passable(_target.x, _target.y)) {
        _state = DONE;
      }
      else if (_actor.p == _digging_place) {
        action = new DigAction(_actor, _target, _level);
      }
      else {
        _state = STARTING;
      }
      break;

    case DONE:
      break;
  }

  if (!action) {
    action = new WaitAction(_actor);
  }

  return action;
}


bool DigTask::ready () const {
  return _state == DONE;
}

