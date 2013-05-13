#ifndef VEC_HPP_B681GLMJ
#define VEC_HPP_B681GLMJ


template <typename T>
struct Vec {
  T x;
  T y;

  Vec (const T x, const T y) : x(x), y(y) {
  }

  Vec () : Vec(0, 0) {}

  Vec operator* (const T factor) const {
    return Vec(x * factor, y * factor);
  }

  Vec operator/ (const T divisor) const {
    return Vec(x / divisor, y / divisor);
  }


  bool operator== (const Vec<T>& other) const {
    return x == other.x && y == other.y;
  }


  bool operator< (const Vec<T>& other) const {
    return y < other.y || (y == other.y && x < other.x);
  }


  bool equals (const T x2, const T y2) const {
    return x == x2 && y == y2;
  }

  Vec clamped (const T x_max, const T y_max) const {
    return Vec(x < x_max ? x : x_max, y < y_max ? y : y_max);
  }

};


template <typename T>
T d_squared (const Vec<T>& a, const Vec<T>& b) {
  const int dx = a.x - b.x;
  const int dy = a.y - b.y;
  return dx*dx + dy*dy;
}


typedef Vec<int> Point;


#endif /* end of include guard: VEC_HPP_B681GLMJ */

