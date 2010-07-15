/*
Copyright (C) 2005 Matthias Braun <matze@braunis.de>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef __MAPPOINT_H__
#define __MAPPOINT_H__

/** Represents a coordinate on the (internal) lincity map */
class MapPoint
{
public:
    MapPoint()
        : x(0), y(0)
    { }
    MapPoint(int _x, int _y)
        : x(_x), y(_y)
    { }
    
    int x, y;

    bool operator==(const MapPoint& other) const
    {
        return other.x == x && other.y == y;
    }
    bool operator!=(const MapPoint& other) const
    {
        return other.x != x || other.y != y;
    }
};

#endif


/** @file lincity-ng/MapPoint.hpp */

