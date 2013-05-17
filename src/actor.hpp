#ifndef ACTOR_HPP_PQZUSCSL
#define ACTOR_HPP_PQZUSCSL


#include "vec.hpp"
#include "item.hpp"


class Actor {
  public:
    Vec<int> p;

    Actor (int x, int y);

    bool has_space () const;
    void get (const Item& item);
    bool has_any (const Item& item) const;
    int remove_items_of_type (const Item& item);

    static const int ITEM_LIMIT = 5;
    Itemlist items;
};


#endif /* end of include guard: ACTOR_HPP_PQZUSCSL */

