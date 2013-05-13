#ifndef PATH_HPP_WQNO3E8M
#define PATH_HPP_WQNO3E8M

#include <list>
#include "level.hpp"
#include "vec.hpp"


bool find_path (std::list<Point>& path, const Level& level, const Point& a, const Point& b, bool corners_on_first = true);

int find_path_length (const Level& level, const Point& a, const Point& b);

#endif /* end of include guard: PATH_HPP_WQNO3E8M */

