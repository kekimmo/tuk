

#include "vec.hpp"


bool adjacent  (const Point& a, const Point& b) {
  if (a.x == b.x) {
    return a.y == b.y - 1 || a.y == b.y + 1;
  }
  else if (a.y == b.y) {
    return a.x == b.x - 1 || a.x == b.x + 1;
  }
  else {
    return false;
  }
}

