
#include "dig.hpp"
#include "path.hpp"
#include <list>
#include <set>


bool Dig::assign (Actor* actor) {
  if (fresh.size() < MAX_WORKERS) {
    fresh.push_back(actor);
    return true;
  }
  else {
    return false;
  }
}


size_t min (size_t a, size_t b) {
  return a < b ? a : b;
}


std::list<Action*> Dig::work (DebugInfo& dbg, Level& level) {
  std::list<Point> workable;
  for (int y = area.y; y < area.y + area.h; ++y) {
    for (int x = area.x; x < area.x + area.w; ++x) {
      if (level.tile(x, y).type == Tile::WALL && !level.surrounded(x, y)) {
        workable.push_back(Point(x, y));
        dbg.workable_tiles.push_back(Point(x, y));
      }
    }
  }

  std::list<Action*> actions;
  Pool workers = fresh;

  while (!workers.empty() && !workable.empty()) {
    auto worker = workers.front();
    auto& point = workable.front();

    std::list<Point> path;
    if (find_path(path, level, point, worker->p, false)) {
      if (path.size() > 2) {
        path.pop_back();
        actions.push_back(new MoveAction(*worker, path.back()));
      }
      else {
        // In position
        actions.push_back(new DigAction(*worker, point, level));
      }
    }

    workers.pop_front();
    workable.pop_front(); 
  }

  return actions;
}

