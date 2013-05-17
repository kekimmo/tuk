#ifndef ACTOR_HPP_PQZUSCSL
#define ACTOR_HPP_PQZUSCSL


#include "vec.hpp"
#include "item.hpp"


class Actor {
  public:
    Vec<int> p;

    Actor (int x, int y);
    Item item = Item::NOTHING;
};


#endif /* end of include guard: ACTOR_HPP_PQZUSCSL */

