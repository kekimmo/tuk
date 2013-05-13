#ifndef DIG_HPP_QC4BRTMB
#define DIG_HPP_QC4BRTMB


#include <vector>
#include <cstddef>
#include "actor.hpp"
#include "level.hpp"
#include "debuginfo.hpp"
#include "action.hpp"


struct Area {
  int x;
  int y;
  int w;
  int h;
};


typedef std::list<Actor*> Pool;


struct Dig {
  size_t MAX_WORKERS = 100;

  Pool fresh;

  bool assign (Actor* actor);
  std::list<Action*> work (DebugInfo& dbg, Level& level);

  Area area;
};


#endif /* end of include guard: DIG_HPP_QC4BRTMB */

