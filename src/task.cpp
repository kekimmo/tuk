
#include "task.hpp"
#include "path.hpp"
#include <list>
#include <set>


Task::~Task () {}


bool Task::find_closest_worker (Pool& workers, const Level& level, const Point& p,
    Actor*& closest, std::list<Point>& shortest, bool (*pred) (const Actor&)) {
  if (workers.empty()) {
    return false;
  }

  std::list<Point> path;

  bool found_one = false;
  for (Actor* worker : workers) {
    if ((!pred || pred(*worker)) &&
        find_path(path, level, p, worker->p, false) &&
        (!found_one || path.size() < shortest.size())) {
      closest = worker;
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


template<typename T>
Action* Task::approach_or_act (DebugInfo& dbg, Pool& idle, Pool& working,
    const Point& target, Level& level, bool (*pred) (const Actor&)) {
  Actor* closest;
  std::list<Point> shortest;
  Action* action = 0;

  if (find_closest_worker(idle, level, target, closest, shortest, pred)) {
    if (shortest.size() == 2) {
      action = new T(*closest, target, level);
    }
    else {
      shortest.pop_back(); // drop the tile the worker is standing on
      action = new MoveAction(*closest, shortest.back());
    }
    dbg.workable_tiles.push_back(target);
    idle.remove(closest);
    working.push_back(closest);
  }
  else {
    // This tile is currently unworkable (no worker has path to it)
    dbg.unworkable_tiles.push_back(target);
  }
  return action;
}


Dig::Dig (const std::set<Point>& tiles) : undug_tiles(tiles) {
}


size_t min (size_t a, size_t b) {
  return a < b ? a : b;
}


std::list<Action*> Dig::work (DebugInfo& dbg, Level& level,
    Pool& idle, Pool& working) {
  std::list<Point> workable;

  // Can't use foreach since the set needs to be modified
  for (auto it = undug_tiles.begin(); it != undug_tiles.end(); ) {
    const Point& p = *it;
    if (level.diggable(p)) {
      if (!level.surrounded(p)) {
        workable.push_back(p);
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

  // This could be (!workers.empty() && !workable.empty()), but then I couldn't
  // get a list of all unworkable tiles
  while (!workable.empty()) {
    const auto& target = workable.front();
    Action* action = approach_or_act<DigAction>(dbg, idle, working, target, level);
    if (action) {
      actions.push_back(action);
    }
    
    workable.pop_front(); 
  }

  return actions;
}


bool Dig::finished (const Level& level) const {
  return undug_tiles.empty();
}


std::list<Action*> Mine::work (DebugInfo& dbg, Level& level,
    Pool& idle, Pool& working) {
  std::list<Action*> actions;

  const auto& has_gold_ore = [](const Actor& worker) {
    return worker.item == Item::GOLD_ORE;
  };

  for (const Point& hoard : level.tiles_of_type(Tile::HOARD)) {
    if (idle.empty()) {
      break;
    }

    while (level.tile(hoard).hp < 999) {
      Action* action = approach_or_act<DepositAction>(dbg, idle, working,
            hoard, level, has_gold_ore);
      if (action) {
        actions.push_back(action);
      }
      else {
        break;
      }
    }
  }

  const auto& has_space = [](const Actor& worker) {
    return worker.item == NOTHING;
  };

  for (const Point& target : level.tiles_of_type(Tile::GOLD)) {
    if (idle.empty()) {
      break;
    }
    
    if (!level.surrounded(target)) {
      Action* action = approach_or_act<MineAction>(dbg, idle, working,
          target, level, has_space);
      if (action) {
        actions.push_back(action);
      }
    }
  }

  return actions;
}


bool Mine::finished (const Level& level) const {
  return false;
}


