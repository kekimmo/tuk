
#include "exception.hpp"
#include "actor.hpp"
#include <algorithm>


Actor::Actor (int x, int y)
  : p(x, y) {
}


bool Actor::has_space () const {
  return items.size() < ITEM_LIMIT;
}


void Actor::get (const Item& item) {
  if (!has_space()) {
    raise("Tried to get an item when already full");
  }
  items.push_back(item);
}


bool Actor::has_any (const Item& item) const {
  return std::find(items.cbegin(), items.cend(), item) != items.cend();
}


int Actor::remove_items_of_type (const Item& item) {
  int count = 0;
  for (auto it = items.begin(); it != items.end(); ) {
    if (*it == item) {
      count += 1;
      items.erase(it++);
    }
    else {
      ++it;
    }
  }

  return count;
}

