/* ---------------------------------------------------------------------- *
 * src/gui/Vector2.hpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2004      Matthias Braun <matze@braunis.de>
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

#ifndef __VECTOR_HPP__
#define __VECTOR_HPP__

#include <ostream>

class Rect2D;

/**
 * @class Vector2
 * @brief Simple two dimensional vector.
 */
class Vector2 {
public:
  constexpr Vector2(float nx, float ny)
    : x(nx), y(ny)
  { }
  constexpr Vector2(const Vector2& other)
    : x(other.x), y(other.y)
  { }
  constexpr Vector2()
    : x(0), y(0)
  { }

  constexpr bool operator ==(const Vector2& other) const
  {
    return x == other.x && y == other.y;
  }

  constexpr bool operator !=(const Vector2& other) const
  {
    return !(x == other.x && y == other.y);
  }

  const Vector2& operator=(const Vector2& other)
  {
    x = other.x;
    y = other.y;
    return *this;
  }

  constexpr Vector2 operator+(const Vector2& other) const
  {
    return Vector2(x + other.x, y + other.y);
  }

  constexpr Vector2 operator-(const Vector2& other) const
  {
    return Vector2(x - other.x, y - other.y);
  }

  constexpr Vector2 operator*(float s) const
  {
    return Vector2(x * s, y * s);
  }

  constexpr Vector2 operator/(float s) const
  {
    return Vector2(x / s, y / s);
  }

  constexpr Vector2 operator-() const
  {
    return Vector2(-x, -y);
  }

  const Vector2& operator +=(const Vector2& other)
  {
    x += other.x;
    y += other.y;
    return *this;
  }

  const Vector2& operator -=(const Vector2& other)
  {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  const Vector2& operator *=(float val)
  {
    x *= val;
    y *= val;
    return *this;
  }

  const Vector2& operator /=(float val)
  {
    x /= val;
    y /= val;
    return *this;
  }

  /** Scalar product of 2 vectors */
  constexpr float operator*(const Vector2& other) const
  {
    return x*other.x + y*other.y;
  }

  // ... add the other operators as needed, I'm too lazy now ...

  const Vector2& constrain(const Rect2D &bounds);

  friend std::ostream& operator<<(std::ostream& os, const Vector2& vec);

  float x, y; // leave this public, get/set methods just give me headaches
              // for such simple stuff :)
};

#endif


/** @file gui/Vector2.hpp */
