
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
      else {
        // This tile is clearly unworkable (surrounded, there can be no
        // route any worker could take)
        dbg.unworkable_tiles.push_back(p);
      }
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

  // This could be (!idle.empty() && !workable.empty()), but then I couldn't
  // get a list of all unworkable tiles
  while (!workable.empty()) {
    auto& target = workable.front();
    Actor* closest;
    std::list<Point> shortest;

    if (find_closest_worker(idle, level, target, &closest, shortest)) {
      if (shortest.size() == 2) {
        actions.push_back(new DigAction(*closest, target, level));
      }
      else {
        shortest.pop_back(); // drop the tile the worker is standing on
        actions.push_back(new MoveAction(*closest, shortest.back()));
      }
      idle.remove(closest);
      working.push_back(closest);
    }
    else {
      // This tile is currently unworkable (no worker has path to it)
      dbg.unworkable_tiles.push_back(target);
    }
    
    workable.pop_front(); 
  }

  // Decide that all workers not currently used are useless too
  useless.splice(useless.begin(), idle);
  
  return actions;
}


// Find the worker with the shortest path to a tile adjacent to p.
// Set closest to this worker and populate shortest with the path.
// Return true on success false if no worker has a route.
bool Dig::find_closest_worker (Pool& workers, const Level& level, const Point& p,
    Actor** closest, std::list<Point>& shortest) {
  if (workers.empty()) {
    return false;
  }

  std::list<Point> path;

  bool found_one = false;
  for (Actor* worker : workers) {
    if (find_path(path, level, p, worker->p, false) &&
        (!found_one || path.size() < shortest.size())) {
      *closest = worker;
      swap(shortest, path);
      path.clear();
      found_one = true;
      if (path.size() <= 2) {
        // There can be no shorter path
        break;
      }
    }
  }

  return found_one;
}


bool Dig::finished () const {
  return undug_tiles.empty();
}

