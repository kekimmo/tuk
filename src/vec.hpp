#ifndef VEC_HPP_B681GLMJ
#define VEC_HPP_B681GLMJ


template <typename T>
struct Vec {
  T x;
  T y;

  Vec (const T x, const T y) : x(x), y(y) {
  }

  Vec operator* (const T factor) const {
    return Vec(x * factor, y * factor);
  }

  Vec operator/ (const T divisor) const {
    return Vec(x / divisor, y / divisor);
  }

  bool equals (const T x2, const T y2) const {
    return x == x2 && y == y2;
  }
};


#endif /* end of include guard: VEC_HPP_B681GLMJ */

