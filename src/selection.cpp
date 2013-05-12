
#include "selection.hpp"


void Selection::start (const Vec<int>& p) {
  from = p;
  to = p;
  started = true;
  finished = false;
}

void Selection::update (const Vec<int>& p) {
  to = p;
}

void Selection::finish () {
  finished = true;
  started = false;
}


void Selection::foreach (std::function<void (int, int)> callback) const {
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

