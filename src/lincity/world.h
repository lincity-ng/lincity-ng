/* ---------------------------------------------------------------------- *
 * world.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __world_h__
#define __world_h__

#include "lintypes.h"
#include <vector>
#include <deque>
#include <set>


template <class T>
class Array2D
{
public:
    Array2D(int lenx, int leny)
    {
        this->lenx = lenx;
        this->leny = leny;
        matrix.resize(lenx * leny, (T)0);
    }
    ~Array2D()
    {
        matrix.clear();
    }
    void initialize( T init)
    {
        for(int index = 0; index < lenx * leny; index++)
        {
            matrix[index] = init;
        }
    }
    T* operator()(int x, int y)
    {
        return &(matrix[x + y * lenx]);
    }
    T* operator()(int index)
    {
        return &(matrix[index]);
    }
    bool is_inside(int x, int y)
    {
        return x >= 0 && x < lenx && y >= 0 && y < leny;
    }
    bool is_inside(int index)
    {
        return index >= 0 && index < lenx * leny;
    }
protected:
    int lenx, leny;
    std::vector<T> matrix;
};
class MapTile;

class World
{
public:
    World(int map_len);
    ~World();
    MapTile* operator()(int x, int y);
    MapTile* operator()(int index);
    bool is_inside(int x, int y);
    bool is_inside(int index);
    bool is_border(int x, int y);
    bool is_border(int index);
    bool is_edge(int x, int y);
    bool is_visible(int x, int y);
    int map_x(MapTile *tile);// returns x
    int map_y(MapTile *tile);// returns y
    int map_index(MapTile *tile);// returns index
    int len(void); //tells the actual world.side_len
    void len(int new_len); //resizes the world by edge
    bool maximum(int x , int y);
    bool minimum(int x , int y);
    bool saddlepoint(int x , int y);
    bool checkEdgeMin(int x , int y);
    int count_altered();
    bool dirty;
    int seed(void); //tells recreation seed
    void seed(int new_seed); //sets the seed
    int old_setup_ground;
    int climate;
    std::set<int> polluted;

protected:
    int id;
    int side_len;
    std::vector<MapTile> maptile;
};





#endif /* __world_h__ */

/** @file lincity/world.h */

