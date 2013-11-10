/* ----------------------------------------------------------------------
 * init_game.cpp
 * This file is part of lincity-ng
 * see COPYING for license, and CREDITS for authors
 * ----------------------------------------------------------------------
 */

// This was part of simulate.cpp.
// Moved in new file for clarification
//
// (re)initialise engine and UI data when
//  - load a saved game (or a scenario)
//  - start a random village (or a  void map)
//

//#define DEBUG_EXPERIMENTAL

#include <math.h>
#include <cstdlib>
#include <vector>
#include "init_game.h"
#include "fileutil.h"
#include "simulate.h"
#include "gui_interface/shared_globals.h"
#include "lctypes.h"
#include "lin-city.h"
#include "engglobs.h"
#include "gui_interface/screen_interface.h"
#include "stats.h"
#include "gui_interface/pbar_interface.h"
#include "modules/modules_interfaces.h"
#include "modules/all_modules.h"
#include "transport.h"
#include "all_buildings.h"
#include "engine.h"
#include <deque>




/* Private functions prototypes */

//static void init_mappoint_array(void);
static void initialize_tax_rates(void);
static void nullify_mappoint(int x, int y);
static void random_start(int *originx, int *originy);
static void coal_reserve_setup(void);
static void ore_reserve_setup(void);
static void setup_river(void);
//static void remove_river(void);
static void setup_river2(int x, int y, int d, int alt, int mountain);
static void setup_ground(void);
static void new_setup_river_ground(void);
static void new_setup_river(void);
//static void sort_by_altitude(int n, std::vector <int> *tabx, std::vector <int> *taby);
//static int new_setup_one_river(int x, int y, int lake_id, Shoreline *shore);
static int quick_river( int x, int y);
void set_river_tile( int i, int j); //also used in loadsave.cpp
static void do_rand_ecology(int x, int y, int r);
//static Shoreline * init_shore(void);
//static void free_shore(Shoreline *shore);
static int overfill_lake(int xl, int yl);//, Shoreline *shore, int lake_id);


/* ---------------------------------------------------------------------- *
 * Public Functions
 * ---------------------------------------------------------------------- */
void clear_game(void)
{
    int i;//x, p;
    const int len = world.len();
#ifdef DEBUG
    assert(len > 0);
#endif
    const int area = len * len;
    //std::cout << "clearing Game" << std::endl;
    //init_mappoint_array ();
    ConstructionManager::clearRequests();
    initialize_tax_rates ();
    init_inventory();
    //std::cout << "whiping game with " << world.len() << " side length" << std::endl;
    // Clear engine and UI data.
    for (int index = 0; index < area; ++index)
    {
        int xx = index % world.len();
        int yy = index / world.len();
        nullify_mappoint(xx, yy);
    }

    world.dirty = false;
    constructionCount.size(100);
    total_time = 0;
    coal_survey_done = 0;
    max_pop_ever = 0;
    total_evacuated = 0;
    total_births = 0;
    total_money = 0;
    tech_level = 0;
    highest_tech_level = 0;
    rockets_launched = 0;
    rockets_launched_success = 0;
    update_avail_modules(0);

    use_waterwell = true; // NG 1.91 : AL1
                          // unused now (it was used in branch waterwell)
                          // but useful to know how to add an optional module, so keep it for a while.

    global_aridity = 0;
    global_mountainity =0;

    highest_tech_level = 0;
    total_pollution_deaths = 0;
    pollution_deaths_history = 0;
    total_starve_deaths = 0;
    starve_deaths_history = 0;
    total_unemployed_years = 0;
    unemployed_history = 0;

    given_scene[0] = 0;
    for( i = 0; i < monthgraph_size; i++ )
    {
        monthgraph_pop[i] = 0;
        monthgraph_starve[i] = 0;
        monthgraph_nojobs[i] = 0;
        monthgraph_ppool[i] = 0;
    }

    people_pool = 100;
    housed_population = 0;
    tech_level = 0;
    total_money = 0;

    sust_dig_ore_coal_tip_flag = 1;
    sust_dig_ore_coal_count = 0;
    sust_port_flag = 1;
    sust_port_count = 0;
    sust_old_money = total_money;
    sust_old_money_count = 0;
    //redo this if a village is generated
    sust_old_population = (housed_population + people_pool);
    sust_old_population_count = 0;
    sust_old_tech_count = 0;
    sust_old_tech = tech_level;
    sust_fire_count = 0;

    init_pbars();
    refresh_pbars();

}


void new_city(int *originx, int *originy, int random_village)
{
    world.old_setup_ground = true;
    world.climate = 0;
    world.seed(rand());
    create_new_city( originx, originy, random_village, true, 0);
}

void new_desert_city(int *originx, int *originy, int random_village)
{
    world.old_setup_ground = false;
    world.climate = 1;
    world.seed(rand());
    create_new_city( originx, originy, random_village, false, 1);
}

void new_temperate_city(int *originx, int *originy, int random_village)
{
    world.old_setup_ground = false;
    world.climate = 2;
    world.seed(rand());
    create_new_city( originx, originy, random_village, false, 2);
}

void new_swamp_city(int *originx, int *originy, int random_village)
{
    world.old_setup_ground = false;
    world.climate = 3;
    world.seed(rand());
    create_new_city( originx, originy, random_village, false, 3);
}

void setup_land()
{
    std::cout << "setting up ecology ";
    std::cout.flush();
    const int len = world.len();
    const int area = len * len;
    std::deque <int> line;
    Array2D <int> dist(len,len);
    Array2D <int> water(len,len);
    int arid = global_aridity;

    line.clear();
    std::cout << ".";
    std::cout.flush();
    for (int index = 0; index < area; index++)
    {
        int xx = index % len;
        int yy = index / len;
        if(!world.is_visible(xx,yy))
        {  continue;}
        if(world(xx,yy)->is_water())
        {
            *dist(xx,yy) = 0;
            *water(xx,yy) = world(xx,yy)->ground.water_alt;
            line.push_back(xx + yy * len);
        }
        else
        {
            *dist(xx,yy) = 50;
            *water(xx,yy) = 3*world(xx,yy)->ground.altitude/4;
        }
    }
    //std::cout << "detected " << line.size() << " river tiles" << std::endl;
    std::cout << ".";
    std::cout.flush();
    while (line.size())
    {
        int index = line.front();
        line.pop_front();
        int xx = index % len;
        int yy = index / len;
        int next_dist = *dist(xx,yy) + 1;
        int water_alt = *water(xx,yy);
        for (int i=0; i<4 ; i++)
        {
            int xt = xx + dx[i];
            int yt = yy + dy[i];
      if(!world.is_visible(xt,yt))
      {  continue;}
            int old_eco = (*dist(xt,yt) * *dist(xt,yt)/5 + 1) + arid +
            (world(xt, yt)->ground.altitude - *water(xt,yt)) * 50 / alt_step;
            int next_eco = (next_dist * next_dist/5 + 1) + arid +
                (world(xt, yt)->ground.altitude - water_alt) * 50 / alt_step;
            if (world.is_visible(xt,yt) && next_eco < old_eco )
            {
                *dist(xt,yt) = next_dist;
                *water(xt,yt) = water_alt;
                line.push_back(xt + yt * len);
            }
        }

    }
    //line.clear();
    std::cout << ".";
    std::cout.flush();
    for (int index = 0; index < area; index++)
    {
        int xx = index % len;
        int yy = index / len;
        int d2w_min = 2 * area;
        int r;
        int alt0 = 0;

        /* test against IS_RIVER to prevent terrible recursion */
        if ( (world(xx, yy)->flags & FLAG_IS_RIVER) || !world(xx, yy)->is_bare())
            continue;
        r = *dist(xx,yy);
        d2w_min = r * r;
        alt0 = *water(xx,yy);

        /* near river lower aridity */
        if (arid > 0)
        {
            if (d2w_min < 5)
            {   arid = global_aridity / 3;}
            else if (d2w_min < 17)
            {   arid = (global_aridity * 2) / 3;}
        }
        /* Altitude has same effect as distance */
        r = rand()%(d2w_min/5 + 1) + arid +
                (world(xx, yy)->ground.altitude - alt0) * 50 / alt_step;
        do_rand_ecology(xx,yy,r);

    }
    std::cout << " done" << std::endl;
    /*smooth all edges in fresh map*/
    //std::cout << "smoothing graphics edges ...";
    //std::cout.flush();
    connect_transport(1, 1, world.len() - 2, world.len() - 2);
    desert_frontier(0, 0, world.len(), world.len());
    //std::cout << " done" << std::endl;
}

/* ---------------------------------------------------------------------- *
 * Private Functions
 * ---------------------------------------------------------------------- */

void create_new_city(int *originx, int *originy, int random_village, int old_setup_ground, int climate)
{

    srand(world.seed());
    if(random_village == -1) //newline in case of reading savegame
    {   std::cout << std::endl;}
    std::cout << "world id: " << world.seed() << std::endl;

    if (random_village != -1) //Only if we are not reconstructing from seed
    {   clear_game();}

    coal_reserve_setup();

    global_mountainity= 100 + rand () % 300; // roughly water slope = 25m / 1km (=from N to S)
    //global_mountainity = 200; //  nearly useless to have a random one (only impacts do_rand_ecology through world(x, y)->ground.altitude)

    switch (climate) {
        case 0:
            //old style map, with Y river: lets be very random on climate
            global_aridity = rand() % 450 - 150;
            break;
        case 1:
            // asked for desert
            global_aridity = rand()%200 + 200;
            break;
        case 2:
            // temperate
            global_aridity = rand()%200;
            break;
        case 3:
            //swamp
            global_aridity = rand()%200 - 200;
            global_mountainity = global_mountainity / 5; // swamps are flat lands
            break;

    }

    if (old_setup_ground)
    {
        setup_river();
        setup_ground();
    }
    else
    {   new_setup_river_ground();}

    setup_land();
    ore_reserve_setup();
    init_pbars();

    if (random_village == 1)
    {   random_start(originx, originy);}
    else if (random_village != -1)
    {   *originx = *originy = world.len() / 2;}

    update_pbar (PPOP, housed_population + people_pool, 1);
    connect_transport(1, 1, world.len() - 2, world.len() - 2);
    desert_frontier(0, 0, world.len(), world.len());
}
static void initialize_tax_rates(void)
{
    income_tax_rate = INCOME_TAX_RATE;
    coal_tax_rate = COAL_TAX_RATE;
    goods_tax_rate = GOODS_TAX_RATE;
    dole_rate = DOLE_RATE;
    transport_cost_rate = TRANSPORT_COST_RATE;
    import_cost_rate = IM_PORT_COST_RATE;
    // AL1 : export_tax_rate;  is not used in 2.X  always equal to zero ? FIXME ?
}

static void coal_reserve_setup(void)
{
    int i, j, x, y, xx, yy;
    const int len = world.len();
    for (i = 0; i < NUMOF_COAL_RESERVES; i++)
    {
        x = (rand() % (len - 12)) + 6;
        y = (rand() % (len - 10)) + 6;
        do {
            xx = (rand() % 3) - 1;
            yy = (rand() % 3) - 1;
        }
        while (xx == 0 && yy == 0);
        for (j = 0; j < 5; j++) {
            world(x, y)->coal_reserve += rand() % COAL_RESERVE_SIZE;
            x += xx;
            y += yy;
        }
    }
}

static void ore_reserve_setup(void)
{
    const int area = world.len() * world.len();
    for (int index=0; index < area; index++ )
    {   world(index)->ore_reserve = ORE_RESERVE;}
}

static void new_setup_river_ground(void)
{
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
    const int len = world.len();
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
    const int SHIFT = (sz - world.len()) / 2; // center the visible map in the big one
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
            Smin += *f1(SHIFT + i,SHIFT + world.len());
            Wmin += *f1(SHIFT,SHIFT + i);
            Emin += *f1(SHIFT + world.len(),SHIFT + i);
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


    alt_min =  int (min);
    sea_level -= alt_min;
    alt_max = 0;
    // pick our map in the fractal one
    for (int index=0; index < area ; index++ )
    {
        i = index % len;
        j = index / len;
        world(i, j)->ground.altitude += int (*f1(SHIFT + i, SHIFT + j)) - alt_min + 1;// + (len-j*j/len)*global_mountainity/2;
        if (  world(i, j)->ground.altitude > alt_max)
            alt_max =  world(i, j)->ground.altitude;
    }

    // take visible value for maximum color dynamic
    alt_min = 0; // visible alt_min is 0, we will use -1 for gray border
    alt_step = (alt_max - alt_min)/10;
#ifdef DEBUG
    fprintf(stderr," alt min = %i; max = %i\n", alt_min, alt_max);
#endif
    new_setup_river();


    if (sea_level > alt_max/3)
    {   sea_level = alt_max/3;}
    //now flood everything below sea_level
    for (int index=0; index < area ; index++ )
    {
        i = index % len;
        j = index / len;
        if (  world.is_visible(i,j) && world(i, j)->ground.altitude < sea_level)
        {
            world(i, j)->ground.altitude = sea_level;
            set_river_tile(i,j);
        }
    }
    //put water at invisible borders in the sea
    for (int index=1; index < area ; index++ )
    //always skip (0,0) ensures brown background
    {
        i = index % len;
        j = index / len;
        if (((i == 0) && world(1,j)->is_river())
        || (i == len-1 && world(len-2,j)->is_river())
        || (j == 0 && world(1,j)->is_river())
        || (j == len-1 && world(i,len-2)->is_river()))
        {   set_river_tile(i,j);}
    }

}

static void new_setup_river(void)
{
    // brute search of local minimum
    const int len = world.len();
    const int area = len * len;
    std::vector <int> lkidx;

    int i, j, k, l, m;
    // Put the gray border (not visible) at alt_min - 1, for easier rivers handling.
    for ( i = 0; i < len; i++)
    {
        world(i, 0)->ground.altitude = alt_min ;
        world(i, world.len() - 1)->ground.altitude = alt_min ;
        world(0, i)->ground.altitude = alt_min ;
        world(world.len() - 1, i)->ground.altitude = alt_min ;
    }

    l = 0;
    for ( int index=0; index < area; index++)
    {
        int x = index % len;
        int y = index / len;
        if (world.is_visible(x,y) && world.minimum(x,y))
        {
            l++;
            lkidx.push_back(x + y * len);
        }
    }

    // fill lake until it overfills and creates a river
    m = ((100 - global_aridity/4)*l) / len; // ugly hardcoded values correpsonding to "climate" switch in create_new_city
    if (m==0)
        m=1;

    if (m>l)
        m = l;

    if (m>area/400)
        m = area/400;

    Permutator * permutator;
    permutator = new Permutator(l,m);
    for (i = 0; i<rand()%10; i++)
    {
        permutator->shuffle();
    }
    /*
    //In case we want extra random rivers instead of connected lakes
    for(i=0;i<m;++i)
    {
        lkidx[i]=rand()%len + rand()%len * len;
    }
    */
    std::cout << "pooring " << m << " lakes into " << l << " random local minima ...";
    std::cout.flush();
    //sort_by_altitude(m, &lakx, &laky);
    for (i = 0; i < m; i++)
    {
        j = permutator->getIndex(i);
        if (world.minimum(lkidx[j] % len, lkidx[j] / len ))
        {
            k = overfill_lake(lkidx[j] % len, lkidx[j] / len );
        }
        else
        {
            k=quick_river(lkidx[j] % len, lkidx[j] / len);
            set_river_tile(lkidx[j] % len, lkidx[j] / len);
        }
        if (k != -1)
        {
            int x = k % len;
            int y = k / len;
            if (world(x,y)->is_visible() && world(x,y)->is_river())
            {
                lkidx[j] = x + y * len;
                i--;
                //std::cout << "attaching lake x, y: " << x << ", " << y << std::endl;
            }
        }
    }
    std::cout << " done" << std::endl;
    delete permutator;
}

static int overfill_lake(int start_x, int start_y)//, Shoreline *shore, int lake_id)
{
    // Starting point is a local minimum

    if ( !world.is_visible(start_x, start_y) )
    {   return -1;}
    const int len = world.len();
    int index, level;
    int flooding_level;
    std::vector<int> river_dests;
    std::vector<int> river_starts;
    Array2D <int> i1(len,len);
    std::deque<int> line;
    std::vector<int> lake;
    river_dests.clear();
    river_starts.clear();
    line.clear();
    lake.clear();
    //make sure a shallow min is an actual min
    level = --world(start_x,start_y)->ground.altitude;
    //int level0 = level;
    *i1(start_x,start_y) = 1;
    line.push_back(start_x + start_y * len);
    lake.push_back(start_x + start_y * len);
    int new_level = alt_max+1;
    int lowest_exit_level = alt_max+1;
    //int max_back_flooding = 10;
    //int back_flooding = max_back_flooding;
    //std::cout << std::endl;
    //std::cout << "new lake x, y = " << start_x << ", "<< start_y <<  " alt : " << level << std::endl;
    while (line.size())
    {
        bool reset_lake = false;
        index = line.front();
        line.pop_front();
        int xx = index % len;
        int yy = index / len;
        //assert(*i1(xx,yy) == 1);
        level = world(xx,yy)->ground.altitude;
/*      /Not needed if altitude == flooding level
        if (world(xx,yy)->ground.water_alt > level)
        {
            level = world(xx,yy)->ground.water_alt;
        }
*/
        //dont grow lakes in diagonal steps,
        //it might result in disconnected rivers
        for (int i=0; i<8; i++)
        {
            int x = xx + dx[i];
            int y = yy + dy[i];
            if (!world.is_visible(x,y) || *i1(x,y)==1)
            {
                continue;
            }
            new_level = world(x,y)->ground.altitude;
/*          //Not needed if altitude == flooding level
            if (world(x,y)->ground.water_alt > new_level)
            {
                new_level = world(x,y)->ground.water_alt;
            }
*/
            if ((new_level >= level && new_level < lowest_exit_level))
            {
                //rain drop test
                int min_alt = new_level;
                int loc_x = x;
                int loc_y = y;
                int s = 0;
                bool done = false;

                // Let the drop run downhill a bit
                while (world.is_visible(loc_x,loc_y) && (s < 16) && !done)
                {
                    s++;
                    int x_min = loc_x;
                    int y_min = loc_y;
                    for (int ii=0; ii<8; ii++)
                    {
                        int tx = x_min + dxo[ii];
                        int ty = y_min + dyo[ii];
                        if (!world.is_inside(tx,ty))
                        {   continue;}
                        //Allow a little walking along edge of a plateau
                        if(min_alt >= world(tx, ty)->ground.altitude)
                        {
                            //dont go back into lake at first step i.e test if there is any second exit
                            if ( s == 1 && *i1(tx,ty))
                            {   continue;}
                            min_alt = world(tx, ty)->ground.altitude;
                            x_min = tx;
                            y_min = ty;
                        }
                    }
                    done = ((s>1 && (loc_x == x_min) && (loc_y == y_min)) || *i1(loc_x,loc_y));
                    loc_x = x_min;
                    loc_y = y_min;
/*
                    if (done && *i1(loc_x,loc_y) && (s > 5))
                    {
                        std::cout << "rain drop reached lake (x,y): steps:(x,y)\t(" << x << "," << y << "): "<< s-1;
                        std::cout << ":(" << loc_x << "," << loc_y << "): "<< std::endl;
                    }
                    if (done && !*i1(loc_x,loc_y))
                    {
                        std::cout << "rain drop runs away (x,y): steps:(x,y)\t(" << x << "," << y << "): "<< s;
                        std::cout << ":(" << loc_x << "," << loc_y << "): "<< std::endl;
                    }

                    if (!world.is_visible(loc_x,loc_y))
                    {
                        std::cout << "rain drop reached edge (x,y): steps:(x,y)\t(" << x << "," << y << "): "<< s;
                        std::cout << ":(" << loc_x << "," << loc_y << "): "<< std::endl;
                    }
*/
                }

                // Now test if we reached the same or a different lake
                if (!(*i1(loc_x,loc_y))) // overspill detected
                {
                    reset_lake = true;
                    lowest_exit_level = new_level;
                }
                else // no overspill add tile to lake
                {
                    *i1(x,y) = 1;
                    line.push_back(x + y * len);
                    lake.push_back(x + y * len);
                }
            }
            else if (new_level == lowest_exit_level)
            {
                river_starts.push_back(x + y * len);
                if (i>3 && !*i1(xx,y) && !*i1(x,yy)) //we went diagonal
                {
                    int xt = x;
                    int yt = y;
                    if (world(xx,y)->ground.altitude > world(x,yy)->ground.altitude)
                    {
                        yt = yy;
                    }
                    else
                    {
                        xt = xx;
                    }
                    world(xt,yt)->ground.altitude = world(x,y)->ground.altitude;
                    set_river_tile(xt,yt);
                }
            }
        } //end for
        if (reset_lake)
        {
            //std::cout << "new water level: " << lowest_exit_level << std::endl;
            line.clear();
            i1.initialize(0);
            river_starts.clear();
            for (size_t i = 0; i < lake.size() ;i++)
            {
                int idx = lake[i];
                if (idx == -1)
                {   continue;}
                int tx = idx % len;
                int ty = idx / len;
                if (world(tx,ty)->ground.altitude < lowest_exit_level)
                {
                    line.push_back(tx + ty * len);
                    *i1(tx,ty) = 1;
                }
                else
                {   lake[i] = -1;}
            }
        }
    } //endwhile
    if (lowest_exit_level == alt_max+1)
    {
        std::cout << std::endl << "cancelled lake x, y = " << start_x << ", "<< start_y <<  " alt : " << level << std::endl;
        return -1;
    }
    flooding_level = lowest_exit_level;

    for (size_t it=0; it<lake.size(); it++)
    {
        index = lake[it];
        if(index==-1)
        {   continue;}
        int x = index % len;
        int y = index / len;
        world(x,y)->ground.altitude = flooding_level;
        set_river_tile(x,y);
    } //end for lake.size()
    //last_lake = start_x + start_y * len;
    for (size_t i=0; i<river_starts.size(); i++)
    {
        index = river_starts[i];
        int x = index % len;
        int y = index / len;
        if (!world(x,y)->is_river())
        {
            river_dests.push_back(quick_river(x,y));
            set_river_tile(x,y);
        }
        //else
        //std::cout << "useless river start encountered" << std::endl;
    }

    if (river_dests.size())
    {
        lowest_exit_level = alt_max+1;
        size_t i_min = 0;
        for(size_t i = 0; i < river_dests.size(); i++)
        {
            if (river_dests[i] != -1)
            {
                int index = river_dests[i];
                int x = index % len;
                int y = index / len;

                if(lowest_exit_level > world(x,y)->ground.altitude)
                //lowest naked river end
                {
                    lowest_exit_level = world(x,y)->ground.altitude;
                    i_min = i;
                }
            }
        }
        return river_dests[i_min];
    }
    return -1;
}

void set_river_tile( int x, int y)
{
    world(x, y)->type = CST_WATER;
    world(x, y)->group = GROUP_WATER;
    world(x, y)->flags |= FLAG_IS_RIVER;
    world(x, y)->flags |= FLAG_HAS_UNDERGROUND_WATER;
    world(x, y)->ground.water_alt = world(x, y)->ground.altitude;
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
            if (world((*tabx)[j],(*taby)[j])->ground.altitude < world((*tabx)[j-1], (*taby)[j-1])->ground.altitude) {
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
static int quick_river( int xx, int yy)
{
    int  x_now, y_now, x_new, y_new, new_alt;
    // start a river from point (xx, yy)
    int max_len = 2 * world.len();
    int river_len = 0;
    x_now = xx;
    y_now = yy;
    x_new = xx;
    y_new = yy;
    new_alt = world(x_now,y_now)->ground.altitude;
    //std::cout << "new river: " << x_now << ", " << y_now;// << " alt = " <<  new_alt;
    do
    {
        x_now = x_new;
        y_now = y_new;
        ++river_len;
        int j = 0;
        for(int i=0; i < 8; i++)
        {
            int x = x_now + dx[i];
            int y = y_now + dy[i];
            if (world.is_inside(x,y) && world(x,y)->ground.altitude < new_alt && !world(x,y)->is_river())
            {
                new_alt = world(x,y)->ground.altitude;
                x_new = x;
                y_new = y;
                j = i;

            }
        }
        if (j>3) //we moved in the diagonal
        {
            //std::cout << ".";
            if (world(x_now + dx[j], y_now)->ground.altitude > world(x_now, y_now + dy[j])->ground.altitude)
                set_river_tile(x_now, y_now + dy[j]);
            else
                set_river_tile(x_now + dx[j], y_now);

        }
        //std::cout << ".";
        set_river_tile(x_new, y_new);
        //std::cout << "next water: "<< x_new << ", " << y_new << " alt = " <<  new_alt << std::endl;
    }
    while ((x_new != x_now || y_new != y_now) && river_len < max_len);
    //std::cout << x_new << ", " << y_new << std::endl;
    //look for minimum around end (maybe the second last tile)
    for(int i=0; i < 8; i++)
    {
        int x = x_now + dx[i];
        int y = y_now + dy[i];
        if (world.is_visible(x,y) && world(x,y)->ground.altitude < new_alt)
        {
            new_alt = world(x,y)->ground.altitude;
            x_new = x;
            y_new = y;
        }
    }
    return world.is_visible(x_new,y_new)? x_new + y_new * world.len(): -1;
}


static void setup_river(void)
{
    std::cout << "carving river ...";
    std::cout.flush();
    const int len = world.len();
    int x, y, i, j;
    int alt = 1; //lowest altitude in the map = surface of the river at mouth.
    x = (1 * len + rand() % len) / 3;
    y = len - 1;
    world(x, y)->ground.water_alt = alt; // 1 unit = 1 cm ,
                        //for rivers .water_alt = .altitude = surface of the water
                        //for "earth tile" .water_alt = alt of underground water
                        //                 .altitude = alt of the ground
                        //            so .water_alt <= .altitude

    /* Mouth of the river, 3 tiles wide, 6 + %12 long */
    i = (rand() % (len/8)) + len/18;
    for (j = 0; j < i; j++) {
        x += (rand() % 3) - 1;
        set_river_tile(x , y );
        set_river_tile(x + 1 , y );
        set_river_tile(x - 1 , y );
        y--;
    }
    set_river_tile(x , y );
    set_river_tile(x + 1, y );
    set_river_tile(x - 1 , y );
#ifdef DEBUG
    fprintf(stderr," x= %d, y=%d, altitude = %d, mountainity = %d\n", x, y, alt, global_mountainity);
#endif
    setup_river2(x - 1, y, -1, alt, global_mountainity); /* left tributary */
    setup_river2(x + 1, y, 1, alt, global_mountainity);  /* right tributary */
    std::cout << " done" << std::endl;
}

static void setup_river2(int x, int y, int d, int alt, int mountain)
{
    const int len = world.len();
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
        if ( (world.is_inside(x + 2*d,y) && !world(x + 2*d, y)->is_bare())
         || (world.is_inside(x + 3*d,y) && !world(x + 3*d, y)->is_bare()) )
            return;
        if (x > 5 && x < world.len() - 5)
        {
            set_river_tile(x , y );
            alt += rand() % (mountain / 10);
            set_river_tile(x + d, y );
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
            setup_river2(x, y, -1, alt, (mountain * 3)/2 );
        }
        if (x > 5 && x < len - 5) {
#ifdef DEBUG
            fprintf(stderr," x= %d, y=%d, altitude = %d\n", x, y, alt);
#endif
            setup_river2(x, y, 1, alt, (mountain *3)/2 );
        }
    }
}

static void setup_ground(void)
{
    const int len = world.len();
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
        if (!world.is_visible(x,y))
        {   continue;}
        if ( world(x, y)->is_river())
        {
            *i1(x,y) = 0;
            world(x,y)->ground.water_alt = world(x,y)->ground.altitude = (len-y*y/len) * slope;
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
            if ( !world.is_visible(tx,ty) || world(tx,ty)->is_river() || new_dist <= dist)
            {   continue;}
            world(tx,ty)->ground.altitude = ((len-ty*ty/len + dist/2 + 2*(len*len - (len-dist)*(len-dist))/len) * slope);
            *i1(tx,ty) = dist;
            line.push_back(tx + ty * len);
        }

    }

    //std::cout << ".";
    //std::cout.flush();
    alt_min = 2000000000;
    alt_max = -alt_min;
    for (int index=0; index<area; index++)
    {
        int x = index % len;
        int y = index / len;

        if (!world.is_visible(x,y))
        {
            continue;
        }
        if (alt_min > world(x, y)->ground.altitude)
        {   alt_min = world(x, y)->ground.altitude;}
        if (alt_max < world(x, y)->ground.altitude)
        {   alt_max = world(x, y)->ground.altitude;}

     }
    alt_step = (alt_max - alt_min) /10;
    //std::cout << ". done" << std::endl;
}
/*
static void remove_river(void)
{
    const int len = world.len();
    const int area = len * len;
    for (int index = 0; index < area ; ++ index)
    {
        if(world(index)->flags & FLAG_IS_RIVER)
        {
            world(index)->type = CST_GREEN;
            world(index)->group = GROUP_BARE;
            world(index)->flags &= ~FLAG_IS_RIVER;
            world(index)->flags &= ~FLAG_HAS_UNDERGROUND_WATER;
            world(index)->ground.water_alt = 0;
        }
    }
}
*/

static void nullify_mappoint(int x, int y)
{
    if(world(x,y)->construction)
    {
        do_bulldoze_area(x, y);
        //turn fresh desert into grass
        world(x,y)->type = CST_GREEN;
        world(x,y)->group = GROUP_BARE;
    }
    else
    {
        world(x,y)->reportingConstruction = NULL;
        world(x,y)->type = CST_GREEN;
        world(x,y)->group = GROUP_BARE;
    }
    world(x,y)->flags = 0;
    world(x,y)->coal_reserve = 0;
    world(x,y)->ore_reserve = 0;
    world(x,y)->pollution = 0;

    world(x,y)->ground.altitude = 0;
    world(x,y)->ground.ecotable = 0;
    world(x,y)->ground.wastes = 0;
    world(x,y)->ground.pollution = 0;
    world(x,y)->ground.water_alt = 0;
    world(x,y)->ground.water_pol = 0;
    world(x,y)->ground.water_wast = 0;
    world(x,y)->ground.water_next = 0;
    world(x,y)->ground.int1 = 0;
    world(x,y)->ground.int2 = 0;
    world(x,y)->ground.int3 = 0;
    world(x,y)->ground.int4 = 0;

}

static void random_start(int *originx, int *originy)
{
    int x, y, xx, yy, flag, watchdog;

    /* first find a place that has some water. */
    watchdog = 500;              /* if too many tries, random placement. */
    do {
        do {
            xx = rand() % (world.len() - 25);
            yy = rand() % (world.len() - 25);
            flag = 0;
            for (y = yy + 2; y < yy + 23; y++)
                for (x = xx + 2; x < xx + 23; x++)
                    if (world(x, y)->flags & FLAG_IS_RIVER)
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
                if (world(x, y)->flags & FLAG_IS_RIVER)
                {
                    flag = 0;
                    x = xx + 22;        /* break out of loop */
                    y = yy + 22;        /* break out of loop */
                }
    } while (flag == 0 && (--watchdog) > 1);
#ifdef DEBUG
    fprintf(stderr, "random village watchdog = %i\n", watchdog);
#endif

    /* These are going to be the main_screen_origin? vars */
    *originx = xx;
    *originy = yy;

    /*  Draw the start scene. */

    /* The first two farms have more underground water */
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (rand() > RAND_MAX/2)
            {
                world(xx + 6 + i, yy + 5 + j)->flags |= FLAG_HAS_UNDERGROUND_WATER;
            }
    organic_farmConstructionGroup.placeItem(xx + 6, yy + 5); //first Farm
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4 ; j++)
            if (rand() > RAND_MAX/2)
            {
                world(xx + 17 + i, yy + 5 + j)->flags |= FLAG_HAS_UNDERGROUND_WATER;
            }
    organic_farmConstructionGroup.placeItem(xx + 17, yy + 5); //second Farm
    residenceMLConstructionGroup.placeItem(xx + 10, yy + 6);
    dynamic_cast < Residence * > (world(xx + 10, yy + 6)->construction) ->local_population = 50;
    potteryConstructionGroup.placeItem(xx + 9, yy + 9);

    world(xx + 16, yy + 9 )->flags |= FLAG_HAS_UNDERGROUND_WATER;
    waterwellConstructionGroup.placeItem(xx + 16, yy + 9);

    residenceMLConstructionGroup.placeItem(xx + 14, yy + 6);
    dynamic_cast < Residence * > (world(xx + 14, yy + 6)->construction) ->local_population = 50;

    marketConstructionGroup.placeItem(xx + 14, yy + 9);
    /* build tracks */
    for (x = 2; x < 23; x++)
    {
        world(xx + x, yy + 11)->setTerrain(GROUP_DESERT);
        trackConstructionGroup.placeItem(xx + x, yy + 11);
    }
    for (y = 2; y < 11; y++)
    {
        world(xx + 13, yy + y)->setTerrain(GROUP_DESERT);
        trackConstructionGroup.placeItem(xx + 13, yy + y);
    }
    for (y = 12; y < 23; y++)
    {
        world(xx + 15, yy + y)->setTerrain(GROUP_DESERT);
        trackConstructionGroup.placeItem(xx + 15, yy + y);
    }

    /* build communes */
    communeConstructionGroup.placeItem(xx + 6, yy + 12);
    communeConstructionGroup.placeItem(xx + 6, yy + 17);
    communeConstructionGroup.placeItem(xx + 11, yy + 12);
    communeConstructionGroup.placeItem(xx + 11, yy + 17);
    communeConstructionGroup.placeItem(xx + 16, yy + 12);
    communeConstructionGroup.placeItem(xx + 16, yy + 17);

    sust_old_population = (housed_population + people_pool);
}

static void do_rand_ecology(int x, int y, int r)
{
    int r3 = rand();
    if (r >= 300)
    {
        /* very dry land */
        int r2 = r3 % 10;
        if (r2 <= 6)
            world(x, y)->setTerrain(GROUP_DESERT);
        else if (r2 <= 8)
            world(x, y)->setTerrain(GROUP_BARE);
        else
            world(x, y)->setTerrain(GROUP_TREE);
    }
    else if (r >= 160)
    {
        int r2 = r3 % 10;
        if (r2 <= 2)
            world(x, y)->setTerrain(GROUP_DESERT);
        else if (r2 <= 6)
            world(x, y)->setTerrain(GROUP_BARE);
        else
            world(x, y)->setTerrain(GROUP_TREE);
    }
    else if (r >= 80)
    {
        int r2 = r3 % 10;
        if (r2 <= 1)
            world(x, y)->setTerrain(GROUP_DESERT);
        else if (r2 <= 4)
            world(x, y)->setTerrain(GROUP_BARE);
        else if (r2 <= 6)
            world(x, y)->setTerrain(GROUP_TREE);
        else
            world(x, y)->setTerrain(GROUP_TREE2);
    }
    else if (r >= 40)
    {
        int r2 = r3 % 40;
        if (r2 == 0)
            world(x, y)->setTerrain(GROUP_DESERT);
        else if (r2 <= 12)
            world(x, y)->setTerrain(GROUP_BARE);
        else if (r2 <= 24)
            world(x, y)->setTerrain(GROUP_TREE);
        else if (r2 <= 36)
            world(x, y)->setTerrain(GROUP_TREE2);
        else
            world(x, y)->setTerrain(GROUP_TREE3);
    }
    else if (r >= 0)
    {
        /* normal land */
        int r2 = r3 % 40;
        if (r2 <= 10)
            world(x, y)->setTerrain(GROUP_BARE);
        else if (r2 <= 20)
            world(x, y)->setTerrain(GROUP_TREE);
        else if (r2 <= 30)
            world(x, y)->setTerrain(GROUP_TREE2);
        else
            world(x, y)->setTerrain(GROUP_TREE3);
    }
    else if (r >= -40)
    {
        /* forest */
        int r2 = r3 % 40;
        if (r2 <= 5)
            world(x, y)->setTerrain(GROUP_BARE);
        else if (r2 <= 10)
            world(x, y)->setTerrain(GROUP_TREE);
        else if (r2 <= 25)
            world(x, y)->setTerrain(GROUP_TREE2);
        else
            world(x, y)->setTerrain(GROUP_TREE3);
    }
    else if (r >= -80)
    {
        int r2 = r3 % 40;
        if (r2 <= 0)
            world(x, y)->setTerrain(GROUP_WATER);
        else if (r2 <= 6)
            world(x, y)->setTerrain(GROUP_BARE);
        else if (r2 <= 15)
            world(x, y)->setTerrain(GROUP_TREE);
        else if (r2 <= 28)
            world(x, y)->setTerrain(GROUP_TREE2);
        else
            world(x, y)->setTerrain(GROUP_TREE3);
    }
    else if (r >= -120)
    {
        int r2 = r3 % 40;
        if (r2 <= 1)
            world(x, y)->setTerrain(GROUP_WATER);
        else if (r2 <= 6)
            world(x, y)->setTerrain(GROUP_BARE);
        else if (r2 <= 16)
            world(x, y)->setTerrain(GROUP_TREE);
        else if (r2 <= 30)
            world(x, y)->setTerrain(GROUP_TREE2);
        else
            world(x, y)->setTerrain(GROUP_TREE3);
    }
    else
    {
        /* wetland */
        int r2 = r3 % 40;
        if (r2 <= 3)
            world(x, y)->setTerrain(GROUP_WATER);
        else if (r2 <= 8)
            world(x, y)->setTerrain(GROUP_BARE);
        else if (r2 <= 20)
            world(x, y)->setTerrain(GROUP_TREE);
        else if (r2 <= 35)
            world(x, y)->setTerrain(GROUP_TREE2);
        else
            world(x, y)->setTerrain(GROUP_TREE3);
    }
    if (world(x, y)->getGroup() != GROUP_DESERT)
    {
        world(x, y)->flags |= FLAG_HAS_UNDERGROUND_WATER;
    }
}

/** @file lincity/init_game.cpp */

