#ifndef AREA_HPP_STZ4WJXO
#define AREA_HPP_STZ4WJXO

#include "vec.hpp"


struct Area {
  Area (int x, int y, int w, int h);

  int x;
  int y;
  int w;
  int h;

  bool contains (const Point& p) const; 
};


#endif /* end of include guard: AREA_HPP_STZ4WJXO */

