
#include "dig.hpp"
#include "path.hpp"
#include <list>
#include <set>


Dig::Dig (const std::set<Point>& tiles) : undug_tiles(tiles) {
}


bool Dig::assign (Actor* actor) {
  if (useless.empty() && idle.size() + working.size() < MAX_WORKERS) {
    idle.push_back(actor);
    return true;
  }
  else {
    return false;
  }
}


void Dig::dismiss (Pool& out) {
  // Dismiss all useless workers
  out.splice(out.end(), useless);
}


void Dig::dismiss_all (Pool& out) {
  out.splice(out.end(), useless);
  out.splice(out.end(), idle);
  out.splice(out.end(), working);
}


size_t min (size_t a, size_t b) {
  return a < b ? a : b;
}


std::list<Action*> Dig::work (DebugInfo& dbg, Level& level) {
  std::list<Point> workable;

  // Can't use foreach since the set needs to be modified
  for (auto it = undug_tiles.begin(); it != undug_tiles.end(); ) {
    const Point& p = *it;
    const int x = p.x;
    const int y = p.y;
    if (level.tile(x, y).type == Tile::WALL) {
      if (!level.surrounded(x, y)) {
        workable.push_back(p);
        dbg.workable_tiles.push_back(p);
      }
      dbg.undug_tiles.push_back(p);
      ++it;
    }
    else {
      undug_tiles.erase(it++);
    }
  }

  std::list<Action*> actions;

  // Prioritize those who worked last iteration
  idle.splice(idle.begin(), working);

  // If there are more idle workers than undug tiles, they are clearly useless
  const size_t undug = undug_tiles.size();
  while (idle.size() > undug) {
    useless.push_back(idle.back());
    idle.pop_back();
  }

  // Assign idle workers to tiles until either run out
  while (!idle.empty() && !workable.empty()) {
    auto worker = idle.front();
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
      working.push_back(worker);
      idle.pop_front();
    }
    workable.pop_front(); 
  }
  
  return actions;
}


bool Dig::finished () const {
  return undug_tiles.empty();
}

