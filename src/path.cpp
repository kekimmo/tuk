
#include "path.hpp"
#include <set>
#include <map>


using namespace std;


int h (const Point& a, const Point& b) {
  return d_squared(a, b);
}


bool neighbors (list<Point>& out, const Level& level,
    const Point& point) {
  bool p[8];

  // 4 0 5
  // 1   2
  // 6 3 7
  
  const int x = point.x;
  const int y = point.y;

  struct {
    int x;
    int y;
  } coords[8] = {
    {  0, -1 },
    { -1,  0 },
    {  1,  0 },
    {  0,  1 },

    { -1, -1 },
    {  1, -1 },
    { -1,  1 },
    {  1,  1 },
  };

  // Drop nonexistent tiles
  p[0] = y > 0;
  p[1] = x > 0;
  p[2] = x < level.w;
  p[3] = y < level.h;
  p[4] = p[0] && p[1];
  p[5] = p[0] && p[2];
  p[6] = p[1] && p[3];
  p[7] = p[2] && p[3];

  // Drop unpassable side tiles
  for (int i = 0; i < 4; ++i) {
    p[i] = p[i] &&
      level.passable(x + coords[i].x, y + coords[i].y);
  }

  // Drop corner tiles like X here:
  // .#X
  // .@#
  // ...
  /* p[4] = p[0] || p[1]; */ 
  /* p[5] = p[0] || p[2]; */ 
  /* p[6] = p[1] || p[3]; */ 
  /* p[7] = p[2] || p[3]; */ 
  // Actually, drop these too:
  // .#X
  // .@.
  // ...
  p[4] = p[0] && p[1]; 
  p[5] = p[0] && p[2]; 
  p[6] = p[1] && p[3]; 
  p[7] = p[2] && p[3]; 

  // Drop unpassable corner tiles
  for (int i = 4; i < 8; ++i) {
    p[i] = p[i] &&
      level.passable(x + coords[i].x, y + coords[i].y);
  }

  // Make the final list
  bool found = false;
  for (int i = 0; i < 8; ++i) {
    if (p[i]) {
      out.push_back(Point(x + coords[i].x, y + coords[i].y));
      found = true;
    }
  }

  return found;
}


bool find_path (list<Point>& path, const Level& level, const Point& a, const Point& b) {
  set<Point> closed;
  set<Point> open;
  map<Point, Point> from;

  map<Point, int> g;
  map<Point, int> f;

  open.insert(a);

  g[a] = 0;
  f[a] = 0 + h(a, b);

  while (!open.empty()) {
    Point current = *open.cbegin();
    if (current == b) {
      path.push_front(current);
      while (from.count(current)) {
        current = from[current];
        path.push_front(current);
      }
      return true;
    }

    open.erase(open.begin());
    closed.insert(current);

    list<Point> neigh;
    if (neighbors(neigh, level, current)) {
      for (Point n : neigh) {
        const int tent_g = g[current] + d_squared(current, n);
        if (closed.count(n) && tent_g >= g[n]) {
          continue;
        }

        if (!open.count(n) || tent_g < g[n]) {
          from[n] = current;
          g[n] = tent_g;
          f[n] = tent_g + h(a, b);
          open.insert(n);
        }
      }
    }
  }

  // No path
  return false;
}

