/* ---------------------------------------------------------------------- *
 * src/gui/Vector2.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2024-2025 David Bears <dbear4q@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#include "Vector2.hpp"

#include <ostream>

#include "Rect2D.hpp"

const Vector2&
Vector2::constrain(const Rect2D &bounds) {
  x = x < bounds.p1.x ? bounds.p1.x : x > bounds.p2.x ? bounds.p2.x : x;
  y = y < bounds.p1.y ? bounds.p1.y : y > bounds.p2.y ? bounds.p2.y : y;
  return *this;
}

std::ostream& operator<<(std::ostream& os, const Vector2& vec) {
  return os << "(" << vec.x << "," << vec.y << ")";
}
