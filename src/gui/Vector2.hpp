//  $Id: vector.h,v 1.7 2004/11/20 22:14:35 matzebraun Exp $
//
//  SuperTux -  A Jump'n Run
//  Copyright (C) 2004 Matthias Braun <matze@braunis.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

/**
 * @author Matthias Braun
 * @file Vector2.hpp
 */

#ifndef __VECTOR_HPP__
#define __VECTOR_HPP__

#include <math.h>

/**
 * @class Vector2
 * @brief Simple two dimensional vector.
 */
class Vector2
{
public:
    Vector2(float nx, float ny)
        : x(nx), y(ny)
    { }
    Vector2(const Vector2& other)
        : x(other.x), y(other.y)
    { }
    Vector2()
        : x(0), y(0)
    { }

    bool operator ==(const Vector2& other) const
        {
            return x == other.x && y == other.y;
        }

    bool operator !=(const Vector2& other) const
        {
            return !(x == other.x && y == other.y);
        }

    const Vector2& operator=(const Vector2& other)
        {
            x = other.x;
            y = other.y;
            return *this;
        }

    Vector2 operator+(const Vector2& other) const
        {
            return Vector2(x + other.x, y + other.y);
        }

    Vector2 operator-(const Vector2& other) const
        {
            return Vector2(x - other.x, y - other.y);
        }

    Vector2 operator*(float s) const
        {
            return Vector2(x * s, y * s);
        }

    Vector2 operator/(float s) const
        {
            return Vector2(x / s, y / s);
        }
    
    Vector2 operator-() const
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
    float operator*(const Vector2& other) const
        {
            return x*other.x + y*other.y;
        }

        // ... add the other operators as needed, I'm too lazy now ...
    
    float x, y; // leave this public, get/set methods just give me headaches
        // for such simple stuff :)
};

#endif


/** @file gui/Vector2.hpp */

