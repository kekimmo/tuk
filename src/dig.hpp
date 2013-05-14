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

  bool assign (Actor* actor);
  void dismiss (Pool& out);
  void dismiss_all (Pool& out);

  std::list<Action*> work (DebugInfo& dbg, Level& level);

  bool finished () const;

  Pool idle;
  Pool working;
  Pool useless;
  std::set<Point> undug_tiles;

  static const int MAX_WORKERS = 100;
};


#endif /* end of include guard: DIG_HPP_QC4BRTMB */

