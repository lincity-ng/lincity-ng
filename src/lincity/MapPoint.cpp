/* ---------------------------------------------------------------------- *
 * src/lincity/MapPoint.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2025      David Bears <dbear4q@gmail.com>
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

#include "MapPoint.hpp"

#include <functional>  // for hash

MapPoint::MapPoint(int x, int y)
  : x(x), y(y)
{}

bool
MapPoint::operator==(const MapPoint other) const {
    return x == other.x && y == other.y;
}

std::ostream& operator<<(std::ostream& os, const MapPoint& point) {
  return os << "(" << point.x << "," << point.y << ")";
}

std::size_t std::hash<MapPoint>::operator()(MapPoint point) const {
  const std::size_t hx = std::hash<int>()(point.x);
  const std::size_t hy = std::hash<int>()(point.y);
  return hx ^ hy + 0x9e3779b9 + (hx << 6) + (hx >> 2);
}
