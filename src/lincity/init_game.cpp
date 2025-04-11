/* ---------------------------------------------------------------------- *
 * src/lincity/init_game.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
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

#include "init_game.hpp"

#include <cassert>                  // for assert
#include <cmath>                    // for pow, exp
#include <cstdlib>                  // for rand, RAND_MAX
#include <deque>                    // for deque
#include <iostream>                 // for basic_ostream, operator<<, char_t...
#include <memory>                   // for unique_ptr
#include <random>                   // for uniform_int_distribution
#include <vector>                   // for vector
#include <optional>
#include <queue>

#include "MapPoint.hpp"             // for MapPoint
#include "all_buildings.hpp"        // for COAL_RESERVE_SIZE, ORE_RESERVE
#include "engglobs.hpp"             // for dx, dy, dxo, dyo
#include "groups.hpp"               // for GROUP_BARE, GROUP_TREE, GROUP_DESERT
#include "lctypes.hpp"              // for CST_WATER
#include "lin-city.hpp"             // for FLAG_HAS_UNDERGROUND_WATER, FLAG_...
#include "lintypes.hpp"             // for Construction, NUMOF_COAL_RESERVES
#include "modules/all_modules.hpp"  // for CommuneConstructionGroup, commune...
#include "stats.hpp"                // for Stats
#include "util.hpp"                 // for LcUrbg
#include "world.hpp"                // for Map, MapTile, Ground, World

#ifdef DEBUG
// #include <assert.h>                        // for assert
#include <stdio.h>                  // for fprintf, size_t, stderr
#endif


/* Private functions prototypes */

//static void init_mappoint_array(void);
static void setup_land(Map& map, int global_aridity, bool without_trees);
static void random_start(World& world,
  bool without_trees);
static void coal_reserve_setup(Map& map);
static void ore_reserve_setup(Map& map);
static void setup_river(Map& map, int global_mountainity);
//static void remove_river(void);
static void setup_river2(Map& map, int x, int y, int d, int alt, int mountain);
static void setup_ground(Map& map, int global_mountainity);
static void new_setup_river_ground(Map& map,
  int global_mountainity, int global_aridity);
static void new_setup_river(Map& map, int global_aridity);
//static void sort_by_altitude(int n, std::vector <int> *tabx, std::vector <int> *taby);
//static int new_setup_one_river(int x, int y, int lake_id, Shoreline *shore);
static std::optional<MapPoint> quick_river(Map& map, MapPoint start);
static void set_river_tile(MapTile& tile); //also used in loadsave.cpp
static void do_rand_ecology(MapTile& tile, int r, bool without_trees);
//static Shoreline * init_shore(void);
//static void free_shore(Shoreline *shore);
static std::optional<MapPoint> overfill_lake(Map& map, MapPoint start);//, Shoreline *shore, int lake_id);


/* ---------------------------------------------------------------------- *
 * Public Functions
 * ---------------------------------------------------------------------- */

std::unique_ptr<World>
new_city(city_settings *city, int mapSize) {
  return create_new_city(city, mapSize, true, 0);
}

std::unique_ptr<World>
new_desert_city(city_settings *city, int mapSize) {
  return create_new_city(city, mapSize, false, 1);
}

std::unique_ptr<World>
new_temperate_city(city_settings *city, int mapSize
) {
    return create_new_city(city, mapSize, false, 2);
}

std::unique_ptr<World>
new_swamp_city(city_settings *city, int mapSize) {
  return create_new_city(city, mapSize, false, 3);
}

template <class T>
class Array2D {
public:
  Array2D(int lenx, int leny) {
    this->lenx = lenx;
    this->leny = leny;
    matrix.resize(lenx * leny, (T)0);
  }
  ~Array2D() {
    matrix.clear();
  }
  void initialize(T init) {
    for(int index = 0; index < lenx * leny; index++) {
      matrix[index] = init;
    }
  }
  T* operator()(int x, int y) {
    return &(matrix[x + y * lenx]);
  }
  T* operator()(int index) {
    return &(matrix[index]);
  }
  bool is_inside(int x, int y) {
    return x >= 0 && x < lenx && y >= 0 && y < leny;
  }
  bool is_inside(int index) {
    return index >= 0 && index < lenx * leny;
  }

protected:
  int lenx, leny;
  std::vector<T> matrix;
};

void setup_land(Map& map, int global_aridity, bool without_trees) {
  std::cout << "setting up ecology ";
  std::cout.flush();
  const int len = map.len();
  const int area = len * len;
  std::deque<MapPoint> line;
  Array2D<int> dist(len,len);
  Array2D<int> water(len,len);
  int arid = global_aridity;

  std::cout << ".";
  std::cout.flush();
  for(MapPoint p; p.y < len; p.y++)
  for(p.x = 0; p.x < len; p.x++) {
    if(!map.is_visible(p))
      continue;
    if(map(p)->is_water()) {
      *dist(p.x,p.y) = 0;
      *water(p.x,p.y) = map(p)->ground.water_alt;
      line.push_back(p);
    }
    else {
      *dist(p.x,p.y) = 50;
      *water(p.x,p.y) = 3*map(p)->ground.altitude/4;
    }
  }
  //std::cout << "detected " << line.size() << " river tiles" << std::endl;
  std::cout << ".";
  std::cout.flush();
  while(line.size()) {
    MapPoint p = line.front();
    line.pop_front();
    int next_dist = *dist(p.x,p.y) + 1;
    int water_alt = *water(p.x,p.y);
    for(MapPoint pt : {p.w(), p.n(), p.e(), p.s()}) {
      if(!map.is_visible(pt)) continue;
      int old_eco = (*dist(pt.x,pt.y) * *dist(pt.x,pt.y)/5 + 1) + arid +
        (map(pt)->ground.altitude - *water(pt.x,pt.y)) * 50 / map.alt_step;
      int next_eco = (next_dist * next_dist/5 + 1) + arid +
          (map(pt)->ground.altitude - water_alt) * 50 / map.alt_step;
      if(map.is_visible(pt) && next_eco < old_eco) {
        *dist(pt.x,pt.y) = next_dist;
        *water(pt.x,pt.y) = water_alt;
        line.push_back(pt);
      }
    }
  }

  std::cout << ".";
  std::cout.flush();
  for(MapPoint p; p.y < len; p.y++)
  for(p.x = 0; p.x < len; p.x++) {
    int d2w_min = 2 * area;
    int r;
    int alt0 = 0;

    /* test against IS_RIVER to prevent terrible recursion */
    if((map(p)->flags & FLAG_IS_RIVER) || !map(p)->is_bare())
      continue;
    r = *dist(p.x,p.y);
    d2w_min = r * r;
    alt0 = *water(p.x,p.y);

    /* near river lower aridity */
    if (arid > 0) {
      if (d2w_min < 5)
        arid = global_aridity / 3;
      else if (d2w_min < 17)
        arid = (global_aridity * 2) / 3;
    }
    /* Altitude has same effect as distance */
    r = rand()%(d2w_min/5 + 1) + arid +
      (map(p)->ground.altitude - alt0) * 50 / map.alt_step;
    do_rand_ecology(*map(p), r, without_trees);
  }

  std::cout << " done" << std::endl;
  /*smooth all edges in fresh map*/
  //std::cout << "smoothing graphics edges ...";
  //std::cout.flush();
  map.connect_transport(1, 1, map.len() - 2, map.len() - 2);
  map.desert_water_frontiers(0, 0, map.len()-1, map.len()-1);
  //std::cout << " done" << std::endl;
}

/* ---------------------------------------------------------------------- *
 * Private Functions
 * ---------------------------------------------------------------------- */

std::unique_ptr<World>
create_new_city(city_settings *city, int mapSize, int old_setup_ground,
  int climate
) {
  assert(city);

  std::unique_ptr<World> worldPtr(new World(mapSize));
  World& world = *worldPtr;

  coal_reserve_setup(world.map);

  int global_mountainity = 100 + rand() % 300;
  int global_aridity = -1;
  switch(climate) {
  case 0: //old style map, with Y river: lets be very random on climate
    global_aridity = rand() % 450 - 150;
    break;
  case 1: // asked for desert
    global_aridity = rand() % 200 + 200;
    break;
  case 2: // temperate
    global_aridity = rand() % 200 + 0;
    break;
  case 3: //swamp
    global_aridity = rand() % 200 - 200;
    global_mountainity /= 5; // swamps are flat lands
    break;
  }

  if(old_setup_ground) {
    setup_river(world.map, global_mountainity);
    setup_ground(world.map, global_mountainity);
  }
  else
    new_setup_river_ground(world.map, global_mountainity, global_aridity);

  setup_land(world.map, global_aridity, city && city->without_trees);
  ore_reserve_setup(world.map);

  if(city->with_village)
    random_start(world, city->without_trees);

  // TODO: this was already done in setup_land. Need it be done again?
  world.map.connect_transport(1, 1, world.map.len() - 2, world.map.len() - 2);
  world.map.desert_water_frontiers(0, 0, world.map.len()-1, world.map.len()-1);

  return worldPtr;
}

static void coal_reserve_setup(Map& map) {
  int i, j, xx, yy;
  MapPoint p;
  const int len = map.len();
  for(i = 0; i < NUMOF_COAL_RESERVES; i++) {
    p.x = (rand() % (len - 12)) + 6;
    p.y = (rand() % (len - 10)) + 6;
    do {
      xx = (rand() % 3) - 1;
      yy = (rand() % 3) - 1;
    }
    while(xx == 0 && yy == 0);
    for (j = 0; j < 5; j++) {
      map(p)->coal_reserve += rand() % COAL_RESERVE_SIZE;
      p.x += xx;
      p.y += yy;
    }
  }
}

static void ore_reserve_setup(Map& map) {
  for(MapTile& t : map)
    t.ore_reserve = ORE_RESERVE;
}

static void new_setup_river_ground(Map& map,
  int global_mountainity, int global_aridity
) {
    /* Principle of land generation :
     *     we start with large blocks of land SZ x SZ, and take random height for each
     *     at each iteration we divide the block size by 2, and Altitude += rand() * fract^N_iter
     * when fract > 1 the additional height is getting smaller at each iteration
     *         this prevent too many pics and holes
     * then we smooth the land, in order to have several local maxima and minima, but not hudreds
     *     (else we go crazy with river path)
     *
     * Additional refinement: we rotate/mirror the map, in order to have the 2 lowest borders in front of us
     *      and the "mountains" far away  (just in case we go to 3d view later :-) )
     */
     /* Square Diamond Algorithm
      * cman79 May 2013
      * we start from SZ x SZ (SZ=s^n+1)
      * 0.) put 4 random heights in the corner
      * 1.) put average height + random offset at central tile (square step)
      * 2.) put average height + rand offset at the centers of the edges
      * repeat 1.) and 2.) for every small (half size) square
      * The heigt of every tile is only set ONCE
      *
      * The alorith can be guided to follow any shape,
      * in particular to join at the edges smoothly to another map
      *
      */
    const int len = map.len();
    const int area = len * len;
    const int mask_size = 7; // useless to be larger than 3*sigma && Must be < SHIFT
    int ii = 2;
    int sz = 4;
    while (sz <= len + mask_size)
    {
        sz *= 2;
        ii++;
    }
    const int NLOOP = ii;
    //const int SZ = sz; // must be = 2^NLOOP
    //std::cout << "temporary map with SZ = " << SZ << std::endl;
    const int SHIFT = (sz - map.len()) / 2; // center the visible map in the big one
    const float sigma = 3.5; // gaussian smoothing
    const float ods2 = 1. / (2. * sigma * sigma);

    const float fract = 0.7; // will be up to  fract ^ NLOOP : be careful to stay near 1.0
                              //  if fract > 1.0, then small scale variations are getting bigger  (recent mountain)
                              //  if fract < 1.0  then small scales varaitions are getting smaller (old mountain)
                              // this is smoothed by the gaussian filter

    const int Keco = 30;  // order of magnitude of each iteration is Keco * mountainity.
    /* good values:
     *      sigma = 3.5   // sigma =2.5 => lots of local minima = small lakes  ;
     *                       maybe will be allowed later, when the problem of finding path to the sea and doing erosion has been solved
     *
     *      fract = 0.9 && Keco = 20 is fine;
     *      fract = 0.8  && Keco = 30   // drawback big structure from first iteration can be visible
     * fract and Keco have impact altitude => also on ecology where the table entry denpends on value of altitude
     *
     * FIXME : mountainity is not needed and causes too much variability
     * FIXME : fract = 1.2  && Keco = 20
     *              with high values of fract Keco and mountanity, we have integer overflow or sth alike in ecotable,
     *              so we loop in the table, doing crazy stripes on the land, or puttind desert in low altitude and swamp on top of mountain :-)
     */

    float mat[2 * mask_size + 1][2 * mask_size + 1];
    Array2D <int> g1(sz,sz);
    Array2D <int> g2(sz,sz);
    Array2D <float> f1(sz+1,sz+1);
    Array2D <float> f2(sz,sz);
    float min = 10000000000000000000.;

    float norm;//,norm1d;
    int i,j,k,l,m,n,size;

    // build gaussian mask for smoothing
    norm = 0;
    //norm1d=0;
    for ( i = 0; i < 2 * mask_size + 1; i++) {
        for ( j = 0; j < 2 * mask_size + 1; j++) {
            float r2 = (i - mask_size) * (i - mask_size) + (j - mask_size) * (j - mask_size);
            mat[i][j] = exp(-r2 * ods2);
            norm += mat[i][j];
        }
    }
    norm = 1. / norm;
   // intialisation
#ifdef DEBUG_EXPERIMENTAL
    // Fix random seed for easier debug
    srand(1234);
#endif

    //inialization for classic Block algorithm
    //int h = ( rand() % Keco + rand() % Keco ) * global_mountainity ;
    //f1.initialize(h);

    //initialization for Diamond Square Algorithm
    f1.initialize(0);
    *f1(0,0) = (float)(rand() % Keco + rand() % Keco ) * global_mountainity ;
    *f1(0,sz) = (float)(rand() % Keco + rand() % Keco ) * global_mountainity ;
    *f1(sz,0) = (float)(rand() % Keco + rand() % Keco ) * global_mountainity ;
    *f1(sz,sz) = (float)(rand() % Keco + rand() % Keco ) * global_mountainity ;
    // 1/5.3 gives roughly half of the times a little sea
    int sea_level = (int)((*f1(0,0)+*f1(0,sz)+*f1(sz,0)+*f1(sz,sz))/5.3);
    //square diamond Algorithm for lanscape generation
    /* fractal iteration for height */
    n = 1;
    for (k = 1; k <= NLOOP; k++)
    {
        //n *= 2;//here in case of Block Algorithm
        size = sz / n;
        // n x n block of size
        //std::cout << "diamonds" << std::endl;
        //Diamond Step
        for ( l = 0; l < n; l++ )
        {
            for ( m = 0; m < n; m++ )
            {
/*
                //old block algorithm
                // one block
                h = int ( double((rand() % Keco + rand() % Keco - Keco) * global_mountainity) * pow(fract,k) );
                for (i = 0 ; i < size; i++)
                    for (j = 0 ; j < size; j++)
                        *f1(l * size + i, m * size + j ) += h;
*/
                int startx = l * size;
                int starty = m * size;
                int midx = startx + size/2;
                int midy = starty + size/2;
                int endx = startx + size;
                int endy = starty + size;
                //std::cout << "x " << startx <<"--" << midx << "--" << endx << std::endl;
                //std::cout << "y " << starty <<"--" << midy << "--" << endy << std::endl;
                if (!*f1(midx,midy))
                {
                    float left_top = *f1(startx, starty);
                    float left_down = *f1(startx, endy);
                    float right_top = *f1(endx, starty);
                    float right_down = *f1(endx, endy);
                    float center = (left_top + left_down + right_top + right_down)/4
                    + float((rand() % Keco - Keco/2 ) * global_mountainity) * pow(fract,k);
                    *f1(midx, midy) = center;
                }
            }
        }
        //Square Step with periodic boundaries.
        for ( l = 0; l < n; l++ )
        {
            for ( m = 0; m < n; m++ )
            {

                int startx = l * size;
                int starty = m * size;
                int midx = startx + size/2;
                int midy = starty + size/2;
                int endx = startx + size;
                int endy = starty + size;

                float left_top = *f1(startx, starty);
                float left_down = *f1(startx, endy);
                float right_top = *f1(endx, starty);
                float right_down = *f1(endx, endy);
                float center = *f1(midx, midy);
                //up center
                if ((m==0) && !*f1(midx, starty))
                {
                    float up_center = (left_top + right_top + center + *f1(midx, sz-size/2))/4;
                    *f1(midx, starty) =
                    up_center + float((rand()%Keco - Keco/2 ) * global_mountainity) * pow(fract,k);
                }
                //right edge
                if(!*f1(endx, midy))
                {
                    float right_center;
                    if ( l < k-1) //inside map
                    {                                                         //to the right             //middle
                        right_center = (right_top + right_down + center + *f1(midx + size, midy))/4;
                    }
                    else //at right edge
                    {
                        right_center = (right_top + right_down + center + *f1(size/2, midy))/4;
                    }
                    *f1(endx, midy) =
                    right_center + float((rand()%Keco - Keco/2) * global_mountainity) * pow(fract,k);
                }
                //down edge
                if (!*f1(midx, endy))
                {
                    float down_center;
                    if (m < k-1) //inside map
                    {                                                         //middle            // downwards
                        down_center = (left_down + right_down + center + *f1(midx, midy + size ))/4;
                    }
                    else //at lower edge
                    {
                        down_center = (left_down + right_down + center + *f1(midx, size/2 ))/4;
                    }
                    *f1(midx, endy) =
                    down_center + float((rand()%Keco - Keco/2 ) * global_mountainity) * pow(fract,k);
                }
                //left edge
                if ((l==0) && !*f1(startx, midy))
                {
                    float left_center = (left_top + left_down + center +*f1(sz - size/2, midy) )/4;
                    *f1(startx, midy) =
                    left_center + float((rand()%Keco - Keco/2) * global_mountainity) * pow(fract,k);
                    //std::cout << "left " << *f1(startx, midy) << std::endl;
                }
            }
        }
    n *= 2; //here in case of Square-Diamond Alogorithm
    }

    //smooth is iterated to propagate a little the lowering of borders
    for (n = 0; n < 2; n++)
    {
        // apply the mask
        for (i = mask_size; i < sz - mask_size; i++)
            for (j = mask_size; j < sz - mask_size; j++) {
                for ( k = -mask_size; k <= mask_size; k++ )
                    for ( l = -mask_size; l <= mask_size; l++ )
                        *f2(i,j) += *f1(i + k,j + l) * mat[mask_size + k][mask_size + l];
            }
         for (i = mask_size; i< sz - mask_size; i++)
            for (j = mask_size; j< sz - mask_size; j++)
                *f1(i,j) = *f2(i,j) * norm;

    }
    {
        // find the lowest borders
        // switch the map to have lowest borders in SE an SW in ISO view
        float Nmin = 0;
        float Smin = 0;
        float Emin = 0;
        float Wmin = 0;
        for ( i = 0; i < len ; i++)
        {
            Nmin += *f1(SHIFT + i,SHIFT);
            Smin += *f1(SHIFT + i,SHIFT + map.len());
            Wmin += *f1(SHIFT,SHIFT + i);
            Emin += *f1(SHIFT + map.len(),SHIFT + i);
        }
        if (Nmin < Smin)
        {
            for ( i = 0; i < sz; i++)
                for ( j = 0; j < sz; j++)
                    *f2(i,j) = *f1(i,sz - j -1);

             for ( i = 0; i < sz; i++)
                for ( j = 0; j < sz; j++)
                    *f1(i,j) = *f2(i,j);
        }

        if (Wmin < Emin)
        {
            for ( i = 0; i < sz; i++)
                for ( j = 0; j < sz; j++)
                   *f2(i,j) = *f1(sz -i -1,j);

            for ( i = 0; i < sz; i++)
                for ( j = 0; j < sz; j++)
                    *f1(i,j) = *f2(i,j);
        }
    }

    // put the south and east border of the "big" map at the minimum visible height
    for ( i = 0; i < len ; i++)
        for ( j = 0; j < len ; j++)
            if ( *f1(SHIFT + i, SHIFT + j) < min)
                min = *f1(SHIFT + i, SHIFT + j);


    for ( i = 0; i < sz; i++)
        for (j = 0; j < (sz - SHIFT - len); j++)
        {
            *f1(i, sz -1 -j) = min;
            *f1(sz -1 -j, i ) = min;
        }


    map.alt_min = (int)min;
    sea_level -= map.alt_min;
    map.alt_max = 0;
    // pick our map in the fractal one
    for(MapTile& t : map) {
      t.ground.altitude +=
        (int)*f1(SHIFT + t.point.x, SHIFT + t.point.y) - map.alt_min + 1;
        // + (len-j*j/len)*global_mountainity/2;
      if(t.ground.altitude > map.alt_max)
        map.alt_max =  t.ground.altitude;
    }

    // take visible value for maximum color dynamic
    map.alt_min = 0; // visible alt_min is 0, we will use -1 for gray border
    map.alt_step = (map.alt_max - map.alt_min)/10;
#ifdef DEBUG
    fprintf(stderr," alt min = %i; max = %i\n", map.alt_min, map.alt_max);
#endif
    new_setup_river(map, global_aridity);


    if (sea_level > map.alt_max/3)
    {   sea_level = map.alt_max/3;}
    //now flood everything below sea_level
    for(MapTile& t : map) {
      if(map.is_visible(t.point) && t.ground.altitude < sea_level) {
        t.ground.altitude = sea_level;
        set_river_tile(t);
      }
    }

    // put water at invisible borders in the sea
    // skiping (0,0) ensures brown background (TODO: is this needed)
    for(MapPoint n(1,0), s(1,len-1), e(len-1,1), w(0,1); n.x < len;) {
      if(map(n.s())->is_river()) set_river_tile(*map(n));
      if(map(s.n())->is_river()) set_river_tile(*map(s));
      if(map(e.w())->is_river()) set_river_tile(*map(e));
      if(map(w.e())->is_river()) set_river_tile(*map(w));
      n.x++; s.x++; e.y++; w.y++;
    }
}

static void new_setup_river(Map& map, int global_aridity) {
  int r = 5 + std::binomial_distribution(10, 0.5)(LcUrbg::get());
  int c = (100 - global_aridity/4) * r * map.len()*map.len() / 1000000;

  std::cout << "pooring " << c << " lakes ..." << std::endl;
  std::cout.flush();

  for(int i = c; i > 0; i--) {
    MapPoint start(
      std::uniform_int_distribution(0, map.len()-1)(LcUrbg::get()),
      std::uniform_int_distribution(0, map.len()-1)(LcUrbg::get())
    );
    #ifdef DEBUG
    std::cerr << "river starting at " << start << std::endl;
    #endif
    auto cmpLower = [](const MapTile *a, const MapTile *b) {
      return a->ground.altitude > b->ground.altitude;
    };
    std::priority_queue<MapTile *, std::vector<MapTile *>, decltype(cmpLower)>
      edge(cmpLower); // will always give us the lowest-altitude tile
    edge.push(map(start));
    edge.push(map(start)); // need to push twice so we have a runner up
    int level = map(start)->ground.altitude - 1; // previous altitude
    std::unordered_set<MapPoint> visited;
    std::vector<MapTile *> downstream; // for backtracking later

    // follow the water flow downhill
    while(true) {
      // get the lowest tile and make sure it's not already visited
      MapTile *next;
      do {
        next = edge.top();
        edge.pop();
      } while(visited.count(next->point));
      // get the runner up (second lowest) because we use it to move diagonal
      MapTile *runnerUp = edge.top();
      while(visited.count(runnerUp->point)) {
        edge.pop();
        runnerUp = edge.top();
      }

      // this helps with going down hills that are not straight
      int d1 = level - next->ground.altitude;
      int d2 = level - runnerUp->ground.altitude;
      if(d2 >= 0 && std::bernoulli_distribution(
        !d1 ? .5 : (double)d2 / (d1 + d2))(LcUrbg::get())
      ) {
        edge.pop();
        edge.push(next);
        next = runnerUp;
      }

      MapTile& t = *next;
      MapPoint p = t.point;
      // check for being done
      if(map.is_border(p) || t.is_river()) {
        #ifdef DEBUG
        std::cerr << "river ending at " << p
          << " alt " << t.ground.altitude
          << (t.is_river() ? " at river" : "") << std::endl;
        #endif
        break; // reached map edge or another river/lake
      }

      // river flows through this tile
      set_river_tile(t);
      visited.insert(p);
      downstream.push_back(&t);
      level = t.ground.altitude;

      // add adjacent tiles to the queue
      for(MapPoint q : {p.n(), p.s(), p.e(), p.w()})
        if(!visited.count(q))
          edge.push(map(q));
    }

    // now backtrack upstream to compute the water level of lakes
    level = INT_MIN;
    for(auto it = downstream.rbegin(); it != downstream.rend(); it++) {
      MapTile& t = **it;
      if(t.ground.altitude <= level) {
        t.ground.altitude = level;
        t.flags |= FLAG_IS_LAKE;
      }
      else {
        level = t.ground.altitude;
      }
    }
  }
  std::cout << " done" << std::endl;
}

static void set_river_tile(MapTile& tile) {
  tile.type = 0;
  tile.group = GROUP_WATER;
  tile.flags |= FLAG_IS_RIVER;
  tile.flags |= FLAG_HAS_UNDERGROUND_WATER;
  tile.ground.water_alt = tile.ground.altitude;
}

/*
static void sort_by_altitude(int n, std::vector <int> *tabx, std::vector <int> *taby)
{
    // sort ascending
    int tmp_x, tmp_y;
    bool sorted = false;

    // bubble sort. n is near 10 so ...
    for (int i = 0; i < n && !sorted ; i++) {
        sorted = true;
        for (int j=1; j < n - i; j++)
            if (map((*tabx)[j],(*taby)[j])->ground.altitude < map((*tabx)[j-1], (*taby)[j-1])->ground.altitude) {
                tmp_x = (*tabx)[j-1];
                tmp_y = (*taby)[j-1];
                (*tabx)[j-1] =  (*tabx)[j];
                (*taby)[j-1] =  (*taby)[j];
                (*tabx)[j] = tmp_x;
                (*taby)[j] = tmp_y;
                sorted = false;
            }
    }
}
*/


static void setup_river(Map& map, int global_mountainity)
{
    std::cout << "carving river ...";
    std::cout.flush();
    const int len = map.len();
    int x, y, i, j;
    int alt = 1; //lowest altitude in the map = surface of the river at mouth.
    x = (1 * len + rand() % len) / 3;
    y = len - 1;
    map(x, y)->ground.water_alt = alt; // 1 unit = 1 cm ,
                        //for rivers .water_alt = .altitude = surface of the water
                        //for "earth tile" .water_alt = alt of underground water
                        //                 .altitude = alt of the ground
                        //            so .water_alt <= .altitude

    /* Mouth of the river, 3 tiles wide, 6 + %12 long */
    i = (rand() % (len/8)) + len/18;
    for (j = 0; j < i; j++) {
        x += (rand() % 3) - 1;
        set_river_tile(*map(x, y));
        set_river_tile(*map(x + 1, y));
        set_river_tile(*map(x - 1, y));
        y--;
    }
    set_river_tile(*map(x, y));
    set_river_tile(*map(x + 1, y));
    set_river_tile(*map(x - 1, y));
#ifdef DEBUG
    fprintf(stderr," x= %d, y=%d, altitude = %d, mountainity = %d\n", x, y, alt, global_mountainity);
#endif
    setup_river2(map, x - 1, y, -1, alt, global_mountainity); /* left tributary */
    setup_river2(map, x + 1, y, 1, alt, global_mountainity);  /* right tributary */
    std::cout << " done" << std::endl;
}

static void setup_river2(Map& map, int x, int y, int d, int alt, int mountain)
{
    const int len = map.len();
    int i, j, r;
    i = (rand() % (len/2)) + len/6;
    for (j = 0; j < i; j++) {
        r = (rand() % 3) - 1 + (d * (rand() % 3));
        if (r < -1) {
            //alt += rand() % (mountain / 10);
            r = -1;
        } else if (r > 1) {
            //alt += rand() % (mountain / 10);
            r = 1;
        }
        x += r;
        if ( (map.is_inside(x + 2*d,y) && !map(x + 2*d, y)->is_bare())
         || (map.is_inside(x + 3*d,y) && !map(x + 3*d, y)->is_bare()) )
            return;
        if (x > 5 && x < map.len() - 5)
        {
            set_river_tile(*map(x , y));
            alt += rand() % (mountain / 10);
            set_river_tile(*map(x + d, y));
        }
        if (--y < 10 || x < 5 || x > len - 5)
            break;
    }
#ifdef DEBUG
    fprintf(stderr," x= %d, y=%d, altitude = %d\n", x, y, alt);
#endif

    if (y > 20) {
        if (x > 5 && x < len - 5) {
#ifdef DEBUG
            fprintf(stderr," x= %d, y=%d, altitude = %d\n", x, y, alt);
#endif
            setup_river2(map, x, y, -1, alt, (mountain * 3)/2 );
        }
        if (x > 5 && x < len - 5) {
#ifdef DEBUG
            fprintf(stderr," x= %d, y=%d, altitude = %d\n", x, y, alt);
#endif
            setup_river2(map, x, y, 1, alt, (mountain *3)/2 );
        }
    }
}

static void setup_ground(Map& map, int global_mountainity)
{
    const int len = map.len();
    const int area = len * len;

    int slope = global_mountainity/10;
    if (slope == 0)
    {   slope = 1;}
    //std::cout << "creating topology ";
    std::cout.flush();
    Array2D <int> i1(len,len);
    std::deque<int> line;
    for (int index=0; index<area; index++)
    {
        int x = index % len;
        int y = index / len;
        if (!map.is_visible(x,y))
        {   continue;}
        if ( map(x, y)->is_river())
        {
            *i1(x,y) = 0;
            map(x,y)->ground.water_alt = map(x,y)->ground.altitude = (len-y*y/len) * slope;
            line.push_back(index);
        }
        else
        {
            *i1(x,y) = len;
        }
    }
    //std::cout << ".";
    //std::cout.flush();
    while (line.size())
    {
        int index = line.front();
        line.pop_front();
        int x = index % len;
        int y = index / len;
        int dist = *i1(x,y)+1;
        for (int i=0; i<8 ; ++i)
        {
            int tx = x + dx[i];
            int ty = y + dy[i];
            int new_dist = *i1(tx,ty);
            if ( !map.is_visible(tx,ty) || map(tx,ty)->is_river() || new_dist <= dist)
            {   continue;}
            map(tx,ty)->ground.altitude = ((len-ty*ty/len + dist/2 + 2*(len*len - (len-dist)*(len-dist))/len) * slope);
            *i1(tx,ty) = dist;
            line.push_back(tx + ty * len);
        }

    }

    //std::cout << ".";
    //std::cout.flush();
    map.alt_min = 2000000000;
    map.alt_max = -map.alt_min;
    for (int index=0; index<area; index++)
    {
        int x = index % len;
        int y = index / len;

        if (!map.is_visible(x,y))
        {
            continue;
        }
        if (map.alt_min > map(x, y)->ground.altitude)
        {   map.alt_min = map(x, y)->ground.altitude;}
        if (map.alt_max < map(x, y)->ground.altitude)
        {   map.alt_max = map(x, y)->ground.altitude;}

    }
    map.alt_step = (map.alt_max - map.alt_min) /10;
    //std::cout << ". done" << std::endl;
}
/*
static void remove_river(void)
{
    const int len = map.len();
    const int area = len * len;
    for (int index = 0; index < area ; ++ index)
    {
        if(map(index)->flags & FLAG_IS_RIVER)
        {
            map(index)->type = CST_GREEN;
            map(index)->group = GROUP_BARE;
            map(index)->flags &= ~FLAG_IS_RIVER;
            map(index)->flags &= ~FLAG_HAS_UNDERGROUND_WATER;
            map(index)->ground.water_alt = 0;
        }
    }
}
*/

static void random_start(World& world, bool without_trees) {
    Map& map = world.map;
    int x, y, xx, yy, flag, watchdog;

    /* first find a place that has some water. */
    watchdog = 500;              /* if too many tries, random placement. */
    do {
        do {
            xx = rand() % (map.len() - 25);
            yy = rand() % (map.len() - 25);
            flag = 0;
            for (y = yy + 2; y < yy + 23; y++)
                for (x = xx + 2; x < xx + 23; x++)
                    if (map(x, y)->flags & FLAG_IS_RIVER)
                    {
                        flag = 1;
                        x = xx + 23;    /* break out of loop */
                        y = yy + 23;    /* break out of loop */
                    }
        } while (flag == 0 && (--watchdog) > 1);
        for (y = yy + 4; y < yy + 22; y++)
            for (x = xx + 4; x < xx + 22; x++)
                /* Don't put the village on a river, but don't care of
                 * isolated random water tiles putted by setup_land
                 */
                if (map(x, y)->flags & FLAG_IS_RIVER)
                {
                    flag = 0;
                    x = xx + 22;        /* break out of loop */
                    y = yy + 22;        /* break out of loop */
                }
    } while (flag == 0 && (--watchdog) > 1);
#ifdef DEBUG
    fprintf(stderr, "random village watchdog = %i\n", watchdog);
#endif

    /*  Draw the start scene. */

    /* The first two farms have more underground water */
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (rand() > RAND_MAX/2)
            {
                map(xx + 6 + i, yy + 5 + j)->flags |= FLAG_HAS_UNDERGROUND_WATER;
            }
    organic_farmConstructionGroup.placeItem(world, MapPoint(xx + 6, yy + 5));
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4 ; j++)
            if (rand() > RAND_MAX/2)
            {
                map(xx + 17 + i, yy + 5 + j)->flags |= FLAG_HAS_UNDERGROUND_WATER;
            }
    organic_farmConstructionGroup.placeItem(world, MapPoint(xx + 17, yy + 5));
    residenceMLConstructionGroup.placeItem(world, MapPoint(xx + 10, yy + 6));
    dynamic_cast < Residence * > (map(xx + 10, yy + 6)->construction) ->local_population = 50;
    potteryConstructionGroup.placeItem(world, MapPoint(xx + 9, yy + 9));

    map(xx + 16, yy + 9 )->flags |= FLAG_HAS_UNDERGROUND_WATER;
    waterwellConstructionGroup.placeItem(world, MapPoint(xx + 16, yy + 9));

    residenceMLConstructionGroup.placeItem(world, MapPoint(xx + 14, yy + 6));
    dynamic_cast < Residence * > (map(xx + 14, yy + 6)->construction) ->local_population = 50;

    marketConstructionGroup.placeItem(world, MapPoint(xx + 14, yy + 9));
    /* build tracks */
    for (x = 2; x < 23; x++)
    {
        map(xx + x, yy + 11)->setTerrain(GROUP_DESERT);
        trackConstructionGroup.placeItem(world, MapPoint(xx + x, yy + 11));
    }
    for (y = 2; y < 11; y++)
    {
        map(xx + 13, yy + y)->setTerrain(GROUP_DESERT);
        trackConstructionGroup.placeItem(world, MapPoint(xx + 13, yy + y));
    }
    for (y = 12; y < 23; y++)
    {
        map(xx + 15, yy + y)->setTerrain(GROUP_DESERT);
        trackConstructionGroup.placeItem(world, MapPoint(xx + 15, yy + y));
    }

    /* build communes */
    communeConstructionGroup.placeItem(world, MapPoint(xx + 6, yy + 12));
    communeConstructionGroup.placeItem(world, MapPoint(xx + 6, yy + 17));
    communeConstructionGroup.placeItem(world, MapPoint(xx + 11, yy + 12));
    communeConstructionGroup.placeItem(world, MapPoint(xx + 11, yy + 17));
    communeConstructionGroup.placeItem(world, MapPoint(xx + 16, yy + 12));
    communeConstructionGroup.placeItem(world, MapPoint(xx + 16, yy + 17));

    world.stats.sustainability.old_population = world.people_pool;
}

static void do_rand_ecology(MapTile& tile, int r, bool without_trees) {
  int r3 = rand();
  if(r >= 300) {
    /* very dry land */
    int r2 = r3 % 10;
    if(r2 <= 6)
      tile.setTerrain(GROUP_DESERT);
    else if(r2 <= 8 || without_trees)
      tile.setTerrain(GROUP_BARE);
    else
      tile.setTerrain(GROUP_TREE);
  }
  else if(r >= 160) {
    int r2 = r3 % 10;
    if(r2 <= 2)
      tile.setTerrain(GROUP_DESERT);
    else if(r2 <= 6 || without_trees)
      tile.setTerrain(GROUP_BARE);
    else
      tile.setTerrain(GROUP_TREE);
  }
  else if (r >= 80) {
    int r2 = r3 % 10;
    if     (r2 <= 1)
      tile.setTerrain(GROUP_DESERT);
    else if(r2 <= 4 || without_trees)
      tile.setTerrain(GROUP_BARE);
    else if(r2 <= 6)
      tile.setTerrain(GROUP_TREE);
    else
      tile.setTerrain(GROUP_TREE2);
  }
  else if (r >= 40) {
    int r2 = r3 % 40;
    if     (r2 == 0)
      tile.setTerrain(GROUP_DESERT);
    else if(r2 <= 12 || without_trees)
      tile.setTerrain(GROUP_BARE);
    else if(r2 <= 24)
      tile.setTerrain(GROUP_TREE);
    else if(r2 <= 36)
      tile.setTerrain(GROUP_TREE2);
    else
      tile.setTerrain(GROUP_TREE3);
  }
  else if (r >= 0) {
    /* normal land */
    int r2 = r3 % 40;
    if     (r2 <= 10 || without_trees)
      tile.setTerrain(GROUP_BARE);
    else if(r2 <= 20)
      tile.setTerrain(GROUP_TREE);
    else if(r2 <= 30)
      tile.setTerrain(GROUP_TREE2);
    else
      tile.setTerrain(GROUP_TREE3);
  }
  else if (r >= -40) {
    /* forest */
    int r2 = r3 % 40;
    if     (r2 <= 5 || without_trees)
      tile.setTerrain(GROUP_BARE);
    else if(r2 <= 10)
      tile.setTerrain(GROUP_TREE);
    else if(r2 <= 25)
      tile.setTerrain(GROUP_TREE2);
    else
      tile.setTerrain(GROUP_TREE3);
  }
  else if (r >= -80) {
    int r2 = r3 % 40;
    if     (r2 <= 0)
      tile.setTerrain(GROUP_WATER);
    else if(r2 <= 6 || without_trees)
      tile.setTerrain(GROUP_BARE);
    else if(r2 <= 15)
      tile.setTerrain(GROUP_TREE);
    else if(r2 <= 28)
      tile.setTerrain(GROUP_TREE2);
    else
      tile.setTerrain(GROUP_TREE3);
  }
  else if (r >= -120) {
    int r2 = r3 % 40;
    if     (r2 <= 1)
      tile.setTerrain(GROUP_WATER);
    else if(r2 <= 6 || without_trees)
      tile.setTerrain(GROUP_BARE);
    else if(r2 <= 16)
      tile.setTerrain(GROUP_TREE);
    else if(r2 <= 30)
      tile.setTerrain(GROUP_TREE2);
    else
      tile.setTerrain(GROUP_TREE3);
  }
  else {
    /* wetland */
    int r2 = r3 % 40;
    if     (r2 <= 3)
      tile.setTerrain(GROUP_WATER);
    else if(r2 <= 8 || without_trees)
      tile.setTerrain(GROUP_BARE);
    else if(r2 <= 20)
      tile.setTerrain(GROUP_TREE);
    else if(r2 <= 35)
      tile.setTerrain(GROUP_TREE2);
    else
      tile.setTerrain(GROUP_TREE3);
  }

  if(tile.getGroup() != GROUP_DESERT) {
    tile.flags |= FLAG_HAS_UNDERGROUND_WATER;
  }
}

/** @file lincity/init_game.cpp */
