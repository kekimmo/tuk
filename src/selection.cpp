
#include "selection.hpp"


void Selection::start (const Point& p) {
  from = p;
  to = p;
  started = true;
  finished = false;
}

void Selection::update (const Point& p) {
  to = p;
}

void Selection::finish () {
  finished = true;
  started = false;
}


void Selection::foreach (const std::function<void (int, int)>& callback) const {
  int x1, y1, x2, y2;

  if (from.x < to.x) {
    x1 = from.x;
    x2 = to.x;
  }
  else {
    x1 = to.x;
    x2 = from.x;
  }

  if (from.y < to.y) {
    y1 = from.y;
    y2 = to.y;
  }
  else {
    y1 = to.y;
    y2 = from.y;
  }

  for (int y = y1; y <= y2; ++y) {
    for (int x = x1; x <= x2; ++x) {
      callback(x, y);
    }
  }
}


void Selection::foreach (const std::function<void (const Point&)>& callback) const {
  foreach([&callback](int x, int y) {
    callback(Point(x, y));
  });
}


int min (int a, int b) {
  return a < b ? a : b;
}


int abs (int a) {
  return a < 0 ? -a : a;
}

  
Area Selection::area () const {
  const int x = min(from.x, to.x);
  const int y = min(from.y, to.y);

  const int w = abs(from.x - to.x) + 1;
  const int h = abs(from.y - to.y) + 1;

  return Area { x, y, w, h }; 
}


std::set<Point> Selection::points () const {
  std::set<Point> ps;
  this->foreach([&ps](int x, int y) {
    ps.insert(Point(x, y));
  });
  return ps;
}


