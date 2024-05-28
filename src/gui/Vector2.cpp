
#include "Vector2.hpp"

#include "Rect2D.hpp"

const Vector2&
Vector2::constrain(const Rect2D &bounds) {
  x = x < bounds.p1.x ? bounds.p1.x : x > bounds.p2.x ? bounds.p2.x : x;
  y = y < bounds.p1.y ? bounds.p1.y : y > bounds.p2.y ? bounds.p2.y : y;
  return *this;
}
