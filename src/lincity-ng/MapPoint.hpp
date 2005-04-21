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

