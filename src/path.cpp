
#include "path.hpp"
#include <cmath>
#include <set>
#include <map>


using namespace std;


bool neighbors (list<Point>& out, const Level& level,
    const Point& point, bool corners = true) {
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
  p[2] = x < level.w - 1;
  p[3] = y < level.h - 1;

  if (corners) {
    p[4] = p[0] && p[1];
    p[5] = p[0] && p[2];
    p[6] = p[1] && p[3];
    p[7] = p[2] && p[3];
  }

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
  if (corners) {
    p[4] = p[0] && p[1]; 
    p[5] = p[0] && p[2]; 
    p[6] = p[1] && p[3]; 
    p[7] = p[2] && p[3]; 

    // Drop unpassable corner tiles
    for (int i = 4; i < 8; ++i) {
      p[i] = p[i] &&
        level.passable(x + coords[i].x, y + coords[i].y);
    }
  }

  // Make the final list
  bool found = false;
  const int amount = corners ? 8 : 4;

  for (int i = 0; i < amount; ++i) {
    if (p[i]) {
      out.push_back(Point(x + coords[i].x, y + coords[i].y));
      found = true;
    }
  }

  return found;
}


bool find_path (list<Point>& path, const Level& level, const Point& a, const Point& b, bool corners_on_first) {
  set<Point> closed;
  map<Point, Point> from;

  map<Point, int> g;
  map<Point, int> f;

  auto h = d_squared<int>;

  auto f_less = [&f](const Point& p1, const Point& p2) -> bool {
    return f[p1] < f[p2];
  };
  multiset<Point, decltype(f_less)> open(f_less);

  open.insert(a);

  g[a] = 0;
  f[a] = 0 + h(a, b);

  bool first = true;

  list<Point> neigh;
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

    neigh.clear();
    if (neighbors(neigh, level, current, (corners_on_first || !first))) {
      for (const Point& n : neigh) {
        const int tent_g = g[current] + h(current, n);
        if (closed.count(n) && tent_g >= g[n]) {
          continue;
        }

        if (!open.count(n) || tent_g < g[n]) {
          from[n] = current;
          g[n] = tent_g;
          f[n] = tent_g + h(n, b);
          open.insert(n);
        }
      }
    }

    first = false;
  }

  // No path
  return false;
}


int find_path_length (const Level& level, const Point& a, const Point& b) {
  list<Point> path;
  if (find_path(path, level, a, b)) {
    return path.size();
  }
  else {
    return -1;
  }
}

