#include <config.h>

#include "Rect2D.hpp"

#include <algorithm>

void
Rect2D::join(const Rect2D& other)
{
    p1.x = std::min(p1.x, other.p1.x);
    p1.y = std::min(p1.y, other.p1.y);
    p2.x = std::max(p2.x, other.p2.x);
    p2.y = std::max(p2.y, other.p2.y);
}
