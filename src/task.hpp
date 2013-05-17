#ifndef DIG_HPP_QC4BRTMB
#define DIG_HPP_QC4BRTMB


#include <vector>
#include <cstddef>
#include <set>
#include <functional>
#include "actor.hpp"
#include "level.hpp"
#include "debuginfo.hpp"
#include "action.hpp"
#include "area.hpp"


typedef std::list<Actor*> Pool;


struct Task {
  virtual ~Task ();

  virtual std::list<Action*> work (DebugInfo& dbg, Level& level,
      Pool& idle, Pool& working) = 0;

  virtual bool finished (const Level& level) const = 0;

  // Approach a tile with a worker unless adjacent. If adjacent, act.
  template<typename T>
  Action* approach_or_act (DebugInfo& dbg, Pool& idle, Pool& working,
      const Point& target, Level& level, bool (*pred) (const Actor&) = 0);

  // Find the worker with the shortest path to a tile adjacent to p.
  // Set closest to this worker and populate shortest with the path.
  // Return true on success, false if no worker has a route.
  static bool find_closest_worker (Pool& workers, const Level& level,
      const Point& p, Actor*& closest, std::list<Point>& shortest,
      bool (*pred) (const Actor&) = 0);
};


struct Dig : public Task {
  Dig (const std::set<Point>& tiles);

  std::list<Action*> work (DebugInfo& dbg, Level& level,
      Pool& idle, Pool& working);

  bool finished (const Level& level) const;

  std::set<Point> undug_tiles;
};


struct Mine : public Task {
  std::list<Action*> work (DebugInfo& dbg, Level& level,
      Pool& idle, Pool& working);

  bool finished (const Level& level) const;
};


class Tasklist : public std::list<Task*> {
  public:
    template<typename T>
      void foreach (std::function<void(T&)> callback) const {
      for (Task* task : *this) {
        T* cast_task = dynamic_cast<T*>(task);
        if (cast_task != NULL) {
          callback(*cast_task);
        }
      }
    }
};


#endif /* end of include guard: DIG_HPP_QC4BRTMB */

