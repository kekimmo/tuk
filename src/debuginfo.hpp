#ifndef DEBUGINFO_HPP_RACPDSGD
#define DEBUGINFO_HPP_RACPDSGD

#include <list>


struct DebugInfo {
  std::list<std::list<Point>> paths;
  std::list<Point> workable_tiles;
  std::list<Point> unworkable_tiles;
};


#endif /* end of include guard: DEBUGINFO_HPP_RACPDSGD */

