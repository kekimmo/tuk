#ifndef ACTOR_HPP_PQZUSCSL
#define ACTOR_HPP_PQZUSCSL


#include "vec.hpp"


class Actor {
  public:
    Vec<int> p;
    int radius;

    Actor (int x, int y, int radius);
};


#endif /* end of include guard: ACTOR_HPP_PQZUSCSL */

