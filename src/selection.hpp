#ifndef SELECTION_HPP_FOBRQLRG
#define SELECTION_HPP_FOBRQLRG

#include <functional>
#include <set>
#include "vec.hpp"
#include "area.hpp"


struct Selection {
  Point from = Point(0, 0);
  Point to = Point(0, 0);
  bool started = false;
  bool finished = false;

  void start (const Point& p);
  void update (const Point& p);
  void finish ();
  void foreach (const std::function<void (int, int)>& callback) const;
  void foreach (const std::function<void (const Point&)>& callback) const;

  Area area () const;
  std::set<Point> points () const;
};


#endif /* end of include guard: SELECTION_HPP_FOBRQLRG */

