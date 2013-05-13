#ifndef ACTOR_HPP_PQZUSCSL
#define ACTOR_HPP_PQZUSCSL


#include "vec.hpp"


class Actor {
  public:
    Vec<int> p;
    int radius;
    int heading;

    Actor (int x, int y, int radius, int heading = 0);
};


#endif /* end of include guard: ACTOR_HPP_PQZUSCSL */

