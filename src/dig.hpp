#ifndef DIG_HPP_QC4BRTMB
#define DIG_HPP_QC4BRTMB


#include <vector>
#include <cstddef>
#include <set>
#include "actor.hpp"
#include "level.hpp"
#include "debuginfo.hpp"
#include "action.hpp"
#include "area.hpp"


typedef std::list<Actor*> Pool;


struct Dig {
  Dig (const std::set<Point>& tiles);

  std::list<Action*> work (DebugInfo& dbg, Level& level,
      Pool& idle, Pool& working);

  bool finished () const;

  std::set<Point> undug_tiles;

  static const int MAX_WORKERS = 100;

  private:
    static bool find_closest_worker (Pool& workers, const Level& level,
        const Point& p, Actor** closest, std::list<Point>& shortest);
};


#endif /* end of include guard: DIG_HPP_QC4BRTMB */

