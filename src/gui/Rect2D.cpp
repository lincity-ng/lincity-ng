/* ---------------------------------------------------------------------- *
 * src/gui/Rect2D.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2005      Matthias Braun <matze@braunis.de>
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

/** @file gui/Rect2D.cpp */
