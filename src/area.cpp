

#include "area.hpp"


Area::Area (int x, int y, int w, int h) : x(x), y(y), w(w), h(h) {};


bool Area::contains (const Point& p) const {
  return p.x >= x &&
         p.y >= y &&
         p.x < x + w &&
         p.y < y + h;
}

