#ifndef SELECTION_HPP_FOBRQLRG
#define SELECTION_HPP_FOBRQLRG

#include <functional>
#include "vec.hpp"


struct Selection {
  Vec<int> from = Vec<int>(0, 0);
  Vec<int> to = Vec<int>(0, 0);
  bool started = false;
  bool finished = false;

  void start (const Vec<int>& p);
  void update (const Vec<int>& p);
  void finish ();
  void foreach (std::function<void (int, int)> callback) const;
};


#endif /* end of include guard: SELECTION_HPP_FOBRQLRG */

