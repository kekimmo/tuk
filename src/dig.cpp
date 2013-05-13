
#include "dig.hpp"
#include "path.hpp"
#include <list>
#include <set>


void Dig::assign (Actor* actor) {
  fresh.push_back(actor);
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

  while (!fresh.empty() && !workable.empty()) {
    auto worker = fresh.front();
    auto& point = workable.front();

    std::list<Point> path;
    if (find_path(path, level, point, worker->p)) {
      if (path.size() > 1) {
        path.pop_back();
        actions.push_back(new MoveAction(*worker, path.back()));
      }
    } 

    fresh.pop_front();
    workable.pop_front(); 
  }

  return actions;
}

