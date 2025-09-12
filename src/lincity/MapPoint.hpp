/* ---------------------------------------------------------------------- *
 * src/lincity/MapPoint.hpp
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

#ifndef __LINCITYNG_LINCITY_MAPPOINT_HPP__
#define __LINCITYNG_LINCITY_MAPPOINT_HPP__

#include <fmt/base.h>     // for formatter
#include <fmt/ostream.h>  // for ostream_formatter
#include <cstddef>        // for size_t
#include <ostream>        // for ostream

namespace std {
template<class Key> struct hash; // IWYU pragma: keep
}  // namespace std

class MapPoint {
public:
  MapPoint(int x = 0, int y = 0);

  bool operator==(const MapPoint other) const;
  bool operator!=(const MapPoint other) const { return !(*this == other); }

  MapPoint n(int dist = 1) const { return MapPoint(x, y - dist); }
  MapPoint s(int dist = 1) const { return MapPoint(x, y + dist); }
  MapPoint e(int dist = 1) const { return MapPoint(x + dist, y); }
  MapPoint w(int dist = 1) const { return MapPoint(x - dist, y); }
  MapPoint ne() const { return MapPoint(x + 1, y - 1); }
  MapPoint nw() const { return MapPoint(x - 1, y - 1); }
  MapPoint se() const { return MapPoint(x + 1, y + 1); }
  MapPoint sw() const { return MapPoint(x - 1, y + 1); }

  int x, y;

  friend std::ostream& operator<<(std::ostream& os, const MapPoint&);
};

template<>
struct std::hash<MapPoint> {
  std::size_t operator()(MapPoint point) const;
};

template<>
struct fmt::formatter<MapPoint> : ostream_formatter {};

#endif // __LINCITYNG_LINCITY_MAPPOINT_HPP__
