 /* ---------------------------------------------------------------------- *
 * 
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling 2001-2004.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "lclib.h"
//#include "common.h"
#include "lctypes.h"
#include "lin-city.h"
#include "engine.h"
#include "engglobs.h"
//#include "cliglobs.h"
#include "simulate.h"
#include "lcintl.h"
#include "power.h"
#include "transport.h" /* for XY_IS_TRANSPORT */

#include "gui_interface/dialbox_interface.h"

/* reset per map_power_grid run; how many different grids */
int grid_num = 0;

/* grid map time stampt.  Incremented per map_power_grid run,
   used to determine if a square has been mapped */
int grid_inc = 0;

Grid * grid[MAX_GRIDS];

/* power_time_step 
   Take the avail_power from last timestep, and move in into the 
   total_power, which will be used during this timestep on a first-come 
   first-served basis.
*/
void
power_time_step () 
{
    int gi;
    int net; /* net power */

    if (grid_num == 0)
	return;

    /* AL1: Do we want to shuffle substations ? It is not done in NG 1.1
     *     shuffle_substations();
     */
    for (gi = 1; gi <= grid_num; gi++) {
	grid[gi]->total_power = grid[gi]->avail_power - 
		(grid[gi]->power_lines * POWER_LINE_LOSS);

	net = (grid[gi]->total_power - grid[gi]->demand);
        /* ->powered is used only for animation of power_lines,
         * substations and windmills
         */
	if (net < 0)
	    grid[gi]->powered = -1;
	else if (net < (grid[gi]->avail_power / 4))
	    grid[gi]->powered = 0;
	else 
	    grid[gi]->powered = 1;

	grid[gi]->avail_power = 0;
	grid[gi]->demand = 0;
    }

    /* Clear substation 'Here' counter */
    /*  int_4 is the local power demand in the substation */
    for (gi = 0; gi < numof_substations; gi++) 
	MP_INFO(substationx[gi],substationy[gi]).int_4 = 0;
}

void 
map_power_grid (bool resetgrids)
{
    int mapx, mapy;
    grid_num = 0;  /* how many grids found so far */
    grid_inc++; /* how many times have we run map_power_grid */

    // used to fix up bad int_7 values after loading a map
    if(resetgrids) {
        for(int mapx = 0; mapx < WORLD_SIDE_LEN; mapx++) {
            for(int mapy = 0; mapy < WORLD_SIDE_LEN; mapy++) {
	    	if (XY_IS_GRID(mapx,mapy))
                	MP_INFO(mapx,mapy).int_7 = grid_inc-1;
            }
        }
    }
    for (mapx = 0; mapx < WORLD_SIDE_LEN; mapx++) {
	for (mapy = 0; mapy < WORLD_SIDE_LEN; mapy++) {
	    if (XY_IS_GRID(mapx,mapy)) {
		if (MP_INFO(mapx,mapy).int_7 != grid_inc) {
		    if (grid_num == MAX_GRIDS) {
			ok_dial_box("warning.mes", BAD,
                                _("You have too many power grids. Join some of them"));
			return;
		    }
		    grid[++grid_num] = (Grid *)lcalloc(sizeof(Grid));
		    grid[grid_num]->total_power = 0;
		    grid[grid_num]->power_lines = 0;
		    grid[grid_num]->demand = 0;
		    grid[grid_num]->max_power = 0;

		    recurse_power_grid(mapx,mapy,0);
		}
	    }
	}
    }
#ifdef DEBUG_POWER
    printf("grid_inc: %d found %d grids\n",grid_inc, grid_num);
#endif
}

/* 
check_grid(x, y, xi, yi) - coordinates, ?i being which one to increment if we
need to step over transport

Check connection to power grid things and check if they've been
mapped.  If they have, we perform a sanity check.  If it is a
power source, project the power for it and add that to our
total.  Now set it to our grid.  If it is a power line, return
1, otherwise 0. */

int 
check_grid(int x, int y, int xi, int yi) 
{
  if (XY_IS_GRID(x,y) && !IS_OLD_WINDMILL(x,y)) {
    if (GRID_CURRENT(x,y)) {
        if (MP_INFO(x,y).int_6 != grid_num) {
                /* XXX: This can occur if connecting to a power source at different
	         *   locations.
	         *   Treat a power source or a substation as a power line in order to
                 *   have both output with the same grid ID.
	         */
                return 1;
        }
    } else if (!IS_POWER_LINE(x,y)) {
        if (IS_POWER_SOURCE(x,y)) {
                /* Pick the produced power from power sources */
                grid[grid_num]->total_power += MP_INFO(x,y).int_5;
                grid[grid_num]->max_power += MP_INFO(x,y).int_1;

        } /* else = is pure substation */
        MP_INFO(x,y).int_6 = grid_num;
        MP_INFO(x,y).int_7 = grid_inc;
        return 1; /* say power sources and substation are power line */
    } else /* is a power line */
        return 1;

  } else if (XY_IS_TRANSPORT(x,y) || XY_IS_WATER(x,y)) { /* can we step over?*/
        if (xi == 0 && yi == 0) /* already stepped */
                return 0;
        if (x+xi >= 1 && x+xi < WORLD_SIDE_LEN &&
                        y+yi >= 1 && y+yi < WORLD_SIDE_LEN)
                return (check_grid(x+xi,y+yi,0,0) ? 2 : 0);
        else
                return 0;
  }
  return 0;
}
  
/* Go through the power grid and figure out what is connected.  This
should really handle the connect_transport bit for power lines.  That
would help perspicuity anyway. */

void 
recurse_power_grid (int startx, int starty, int steps) 
{
    static int level;             /* debug: levels of recursion encountered */
    int count = steps;            /* number of steps taken - for animation */
    short dir = -1;   /* -1 undetermined, 0 nothing left, Direction #defines */
    int mapx = startx, mapy = starty;                     /* to move about */
    int inc;           /* handles special case of stepping over transport */
  
    level++;
    if (count % POWER_MODULUS == 0)
	count = 0;

    /* Old windmills aren't grid connected, so they are on their own 'grid'.  We
       ignore them in the main loop.  This case should only be reached from a 
       call from map_power_grid with a new grid_num, not from a new path in the
       code below */
    if (IS_OLD_WINDMILL(mapx, mapy)) {
	MP_INFO(mapx,mapy).int_6 = grid_num;
	MP_INFO(mapx,mapy).int_7 = grid_inc;
	grid[grid_num]->max_power += MP_INFO(mapx,mapy).int_1;
	level--;
	return;
    }

    /* Crawl about the grid, finding paths and what not.  */
    while (dir != 0) {
	/* Set to current grid */
	/* figure out what we are on */
	if (IS_POWER_LINE(mapx,mapy)) {
	    grid[grid_num]->power_lines++;
	    MP_INFO(mapx,mapy).int_5 = (count++ % POWER_MODULUS);
	    if ((MP_TYPE(mapx,mapy) >= 1) && (MP_TYPE(mapx,mapy) <= 11))
		MP_TYPE(mapx,mapy) += 11;
	}
	MP_INFO(mapx,mapy).int_6 = grid_num;
	MP_INFO(mapx,mapy).int_7 = grid_inc;

	/* For each direction, check map bounds, check if there is power stuff
	   there, then either remember to follow it later or start a new
	   recursion to follow the path now */

	/* West */
	if (mapx >= 1)
	    if ((inc = check_grid(mapx - 1, mapy, -1, 0)))
		if (dir < 1)
		    dir = WEST;
		else
		    recurse_power_grid(mapx - inc, mapy, count + 1);
	/* North */
	if (mapy >= 1) 
	    if ((inc = check_grid(mapx, mapy - 1, 0, -1)))
		if (dir < 1)
		    dir = NORTH;
		else
		    recurse_power_grid(mapx, mapy - inc, count + 1);
	/* East */    
	if (mapx < WORLD_SIDE_LEN)
	    if ((inc = check_grid(mapx + 1, mapy, 1, 0)))
		if (dir < 1)
		    dir = EAST;
		else 
		    recurse_power_grid(mapx + inc, mapy, count + 1);
	/* South */    
	if (mapy < WORLD_SIDE_LEN)
	    if ((inc = check_grid(mapx, mapy + 1, 0, 1)))
		if (dir < 1)
		    dir = SOUTH;
		else 
		    recurse_power_grid(mapx, mapy + inc, count + 1);

	/* Move to a new square if the chosen direction is not already mapped. */
	switch (dir) {
	case (-1):  /* Didn't find one, must not be any.  Stop looping */ 
	    {
		dir = 0; 
	    } break;
	case WEST: {
		if (mapx >= 1)
		    if ((inc = check_grid(mapx - 1, mapy, -1, 0))) {
			mapx -= inc;
			dir = -1;
		    } else
			dir = 0;
	    } break;
	case NORTH: {
		if (mapy >= 1)
		    if ((inc = check_grid(mapx, mapy - 1, 0, -1))) {
			mapy -= inc;
			dir = -1;
		    } else
			dir = 0;
	    } break;
	case EAST: {
		if (mapx < WORLD_SIDE_LEN)
		    if ((inc = check_grid(mapx + 1, mapy, 1, 0))) {
			mapx += inc;
			dir = -1;
		    } else
			dir = 0;
	    } break;
	case SOUTH: { 
		if (mapy < WORLD_SIDE_LEN)
		    if ((inc = check_grid(mapx, mapy + 1, 0, 1))) {
			mapy += inc;
			dir = -1;
		    } else
			dir = 0;
	    } break;
	} /* switch dir */
    } /* while dir !=0 */

    level--;
    /*  printf("exiting recurse_power_grid:level %d\n",level);*/
}

/* get_power
   get power for thing at x, y.  Don't use windmills if industry.
   We go through a list (ugly, I know) until we find a substation in range
   and then try and get power from it's grid.  If we can't, continue.
*/

int 
get_power (int x, int y, int power, int block_industry)
{
  /* block_industry = 1 for industries and recyclers */
  int i;
  int xi, yi;
  int grid_tmp; /* for simplicity */

  if (numof_substations == 0)
    return(0);

  for (i = 0; i < numof_substations; i++) 
    {
      xi = substationx[i];
      yi = substationy[i];
      if (abs (xi - x) < SUBSTATION_RANGE && 
	  abs (yi - y) < SUBSTATION_RANGE) {

	if (block_industry != 0 && MP_GROUP(xi, yi) == GROUP_WINDMILL)
	  continue;

	grid_tmp = MP_INFO(xi,yi).int_6;

        /* FIXME: in case of unsatisfied demand, the demand may be counted 
         * several times in differents substations (in the same grid or not)
         * and thus diplayed informations are misleading.
         */
	grid[grid_tmp]->demand += power;

	if (grid[grid_tmp]->total_power >= power) {
	  grid[grid_tmp]->total_power -= power;
	  MP_INFO(xi,yi).int_4 += power; // local demand in substation xi yi
	  return 1;
	}
	
      }
    }
  return 0;
}
