#ifndef _POWER_H
#define _POWER_H

#undef DEBUG_POWER

struct grid_struct {
    int power_lines;
    long total_power;
    long avail_power;           /* Capacity available */
    long max_power;
    long demand;
    short powered;
};

/* public */
#define MAX_GRIDS 128           // How many grids in the array, not how many to allocate
extern grid_struct *grid[MAX_GRIDS];

void map_power_grid(bool resetgrids = false);
int get_power(int x, int y, int power, int block_industry);
void do_power_substation(int x, int y);
void do_power_source(int x, int y);
void do_power_source_coal(int x, int y);
void do_power_line(int x, int y);
void power_time_step();

/* intended private */

#define POWER_LINE_LOSS 1       /* one KW */
#define POWER_MODULUS 25        /* Controls how often we see a packet in anim */

#define WEST 1
#define NORTH 2
#define EAST 3
#define SOUTH 4

#define XY_IS_GRID(x,y) \
((MP_GROUP(x,y) == GROUP_COAL_POWER) ||\
 (MP_GROUP(x,y) == GROUP_WINDMILL) || \
 (MP_GROUP(x,y) == GROUP_POWER_LINE) || \
 (MP_GROUP(x,y) == GROUP_SOLAR_POWER) || \
 (MP_GROUP(x,y) == GROUP_SUBSTATION))

#define XY_IS_WATER(x,y) \
((MP_GROUP(x,y) == GROUP_WATER) || \
 (MP_GROUP(x,y) == GROUP_TRACK_BRIDGE) || \
 (MP_GROUP(x,y) == GROUP_ROAD_BRIDGE) || \
 (MP_GROUP(x,y) == GROUP_RAIL_BRIDGE))

#define IS_POWER_SOURCE(x,y) \
((MP_GROUP(x,y) == GROUP_COAL_POWER) || \
 (MP_GROUP(x,y) == GROUP_SOLAR_POWER) || \
 (MP_GROUP(x,y) == GROUP_WINDMILL))

#define IS_POWER_LINE(x,y) (MP_GROUP(x,y) == GROUP_POWER_LINE)

#define IS_OLD_WINDMILL(x,y) \
((MP_GROUP(x,y) == GROUP_WINDMILL) && \
 (MP_TYPE(x,y) != CST_USED) && \
 (MP_TECH(x,y) < MODERN_WINDMILL_TECH))
// (MP_INFO(x,y).int_2 < MODERN_WINDMILL_TECH))

#define GRID_CURRENT(x,y) (MP_INFO(x,y).int_7 == grid_inc)


#endif
