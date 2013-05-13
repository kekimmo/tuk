
#include <cmath>
#include <cassert>

#include "collide.hpp"


int find_voronoi (int tile_size, const int tx, const int ty, const int ax, const int ay) {
  auto pc = [tile_size](int coord) {
    return coord * tile_size + tile_size / 2;
  };

  auto pc_corner = [tile_size](int coord) {
    return coord * tile_size;
  };

  // Middle (really rare but can happen e.g. with doors
  if (ax == pc(tx) && ay == pc(ty)) {
    return 5;
  }

  const double left = pc_corner(tx);
  const double right = left + tile_size;
  const double top = pc_corner(ty);
  const double bottom = top + tile_size;

  const bool in_left = ax < left;
  const bool in_top = ay < top;
  const bool in_right = ax > right;
  const bool in_bottom = ay > bottom;

  // Vertices
  if (in_top && in_left) return 1;
  else if (in_top && in_right) return 3;
  else if (in_bottom && in_left) return 7;
  else if (in_bottom && in_right) return 9;

  // Edges (must check distance to center)
  // NOTE: assume square tile
  const int dx = ax - pc(tx);
  const int dy = ay - pc(ty);

  if (std::abs(dx) > std::abs(dy)) {
    if (dx <= 0) {
      return 4;
    }
    else {
      return 6;
    }
  }
  else {
    if (dy <= 0) {
      return 2;
    }
    else {
      return 8;
    }
  }
}


int sign (const double number) {
  return (number > 0.0) - (number < 0.0);
}


double dot_product (double x1, double y1, double x2, double y2) {
  return x1 * x2 + y1 * y2;
}


// dirx, diry: push direction unit vector
// thwx, thwy: tile halfwidth vector
// r: circle radius 
// tp_len: tile-circle centerpoint distance
Vec<double> find_push (double dirx, double diry, double thwx, double thwy, double r, double tp_len) {
  const double dp = dot_product(dirx, diry, thwx, thwy);

  double push_len = r + dp - tp_len;
  if (push_len < 0.0) {
    push_len = 0.0;
  }

  return Vec<double>(dirx * push_len, diry * push_len); 
}


bool collide_level_actor (const Level& level, Actor& actor, int tile_size) {
  auto tc = [tile_size](int coord) {
    return coord / tile_size;
  };

  auto pc_corner = [tile_size](int coord) {
    return coord * tile_size;
  };

  auto pc = [tile_size](int coord) {
    return coord * tile_size + tile_size / 2;
  };

  auto length = [](double x, double y) {
    return sqrt(x*x + y*y);
  };

  const int left = tc(actor.p.x - actor.radius);
  const int right = tc(actor.p.x + actor.radius);
  const int top = tc(actor.p.y - actor.radius);
  const int bottom = tc(actor.p.y + actor.radius);

  const int tr = tile_size / 2;

  bool moved = false;

  for (int y = top; y <= bottom; ++y) {
    for (int x = left; x <= right; ++x) {
      const int tcx = pc_corner(x);
      const int tcy = pc_corner(y);

      const int al = actor.p.x - actor.radius;
      const int at = actor.p.y - actor.radius;
      const int ar = actor.p.x + actor.radius;
      const int ab = actor.p.y + actor.radius;

      if (ar <= tcx) continue;
      if (ab <= tcy) continue;
      if (al >= tcx + tile_size) continue;
      if (at >= tcy + tile_size) continue;

      if (!level.tile(x, y).passable) {
        const double tpx = actor.p.x - pc(x);
        const double tpy = actor.p.y - pc(y);

        const double tp_len = length(tpx, tpy);

        const double dirx = tpx / tp_len;
        const double diry = tpy / tp_len;

        Vec<double> push(0.0, 0.0);

        const int voronoi = find_voronoi(tile_size, x, y, actor.p.x, actor.p.y);
        switch (voronoi) {
          case 5:
            push.x = 1;
            break;

          case 2:
            push.y = tcy - ab;
            assert(push.y < 0);
            break;

          case 4:
            push.x = tcx - ar;
            assert(push.x < 0);
            break;

          case 6:
            push.x = tcx + tile_size - al;
            assert(push.x > 0);
            break;

          case 8:
            push.y = tcy + tile_size - at;
            assert(push.y > 0);
            break;

          case 1:
            push = find_push(dirx, diry, -tr, -tr, actor.radius, tp_len);
            assert(push.x <= 0);
            assert(push.y <= 0);
            break;

          case 3:
            push = find_push(dirx, diry, tr, -tr, actor.radius, tp_len);
            assert(push.x >= 0);
            assert(push.y <= 0);
            break;

          case 7:
            push = find_push(dirx, diry, -tr, tr, actor.radius, tp_len);
            assert(push.x <= 0);
            assert(push.y >= 0);
            break;

          case 9:
            push = find_push(dirx, diry, tr, tr, actor.radius, tp_len);
            assert(push.x >= 0);
            assert(push.y >= 0);
            break;
        }
        
        const int ix = round(push.x + 0.5 * sign(push.x)); 
        const int iy = round(push.y + 0.5 * sign(push.y));

        if (ix != 0 || iy != 0) {
          actor.p.x += ix;
          actor.p.y += iy;
          moved = true;
        }
      }
    }
  }

  return moved;
}


bool collide_actor_actor (Actor& a, Actor& b) {
  double dx = b.p.x - a.p.x;
  double dy = b.p.y - a.p.y;

  if (std::abs(dx) < 0.1 && std::abs(dy) < 0.1) {
    dx = 0.1;
  }

  const double d_len = sqrt(dx*dx + dy*dy);
  const double overlap = a.radius + b.radius - d_len;

  if (overlap > 0.0) {
    const double push_x = 0.5 * overlap * dx / d_len;
    const double push_y = 0.5 * overlap * dy / d_len;

    a.p.x -= round(push_x + 0.5 * sign(push_x));
    a.p.y -= round(push_y + 0.5 * sign(push_y));
    b.p.x += round(push_x + 0.5 * sign(push_x));
    b.p.y += round(push_y + 0.5 * sign(push_y));

    return true;
  }

  return false;
}

