 /* ---------------------------------------------------------------------- *
 * engine.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * Portions copyright (c) Corey Keasling 2001.
 * ---------------------------------------------------------------------- */

#include <stdio.h>
#include "lclib.h"
#include "common.h"
#include "lctypes.h"
#include "lin-city.h"
#include "engine.h"
#include "engglobs.h"
#include "cliglobs.h"
#include "simulate.h"
#include "sernet.h"
#include "clistubs.h"
#include "lcintl.h"
#include "power.h"

/* reset per map_power_grid run; how many different grids */
int grid_num = 0;

/* grid map time stampt.  Incremented per map_power_grid run,
   used to determine if a square has been mapped */
int grid_inc = 0;

Grid * grid[MAX_GRIDS];

#define POWER_MODULUS 25 // Controls how often we see a packet


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

    for (gi = 1; gi <= grid_num; gi++) {
	grid[gi]->total_power = grid[gi]->avail_power - 
		(grid[gi]->power_lines * POWER_LINE_LOSS);

	net = (grid[gi]->total_power - grid[gi]->demand);
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
    for (gi = 0; gi < numof_substations; gi++) 
	MP_INFO(substationx[gi],substationy[gi]).int_5 = 0;
}


void 
map_power_grid ()
{
    int mapx, mapy;
    grid_num = 0;  /* how many grids found so far */
    grid_inc++; /* how many times have we run map_power_grid */

    for (mapx = 0; mapx < WORLD_SIDE_LEN; mapx++) {
	for (mapy = 0; mapy < WORLD_SIDE_LEN; mapy++) {
	    if (XY_IS_GRID(mapx,mapy)) {
		if (MP_INFO(mapx,mapy).int_7 != grid_inc) {
		    if (grid_num == MAX_GRIDS) {
			printf("You have too many power grids.  Join some of them\n");
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
      if (MP_INFO(x,y).int_6 != grid_num)
	/* XXX: This can occur if connecting to a power source at different
	   locations.  Need a clean way to resolve this, either connect
	   the two grids by treating a power source as a power line, or 
	   let the power source be multihomed and figure out power distribution
	*/
	printf("recurse_power_grid insane: %d, %d on a different grid!\n",
	       x,y);
    } else if (!IS_POWER_LINE(x,y)) {
      if (IS_POWER_SOURCE(x,y)) {
	project_power(x,y); 
	grid[grid_num]->total_power += MP_INFO(x,y).int_1;
      }
      
      MP_INFO(x,y).int_6 = grid_num;
      MP_INFO(x,y).int_7 = grid_inc;
      
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
	    if (inc = check_grid(mapx - 1, mapy, -1, 0))
		if (dir < 1) 
		    dir = WEST;
		else
		    recurse_power_grid(mapx - inc, mapy, count + 1);


	/* North */
	if (mapy >= 1)
	    if (inc = check_grid(mapx, mapy - 1, 0, -1))
		if (dir < 1) 
		    dir = NORTH;
		else 
		    recurse_power_grid(mapx, mapy - inc, count + 1);


	/* East */    
	if (mapx < WORLD_SIDE_LEN)
	    if (inc = check_grid(mapx + 1, mapy, 1, 0))
		if (dir < 1) 
		    dir = EAST;
		else 
		    recurse_power_grid(mapx + inc, mapy, count + 1);


	/* South */    
	if (mapy < WORLD_SIDE_LEN)
	    if (inc = check_grid(mapx, mapy + 1, 0, 1))
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
	case WEST: 
	    {
		if (mapx >= 1) 
		    if (inc = check_grid(mapx - 1, mapy, -1, 0)) {
			mapx -= inc;
			dir = -1;
		    } else 
			dir = 0;
	    } break;

	case NORTH:
	    {
		if (mapy >= 1)
		    if (inc = check_grid(mapx, mapy - 1, 0, -1)) {
			mapy -= inc;
			dir = -1;
		    } else
			dir = 0;
	    } break;

	case EAST:
	    {
		if (mapx < WORLD_SIDE_LEN)
		    if (inc = check_grid(mapx + 1, mapy, 1, 0)) {
			mapx += inc;
			dir = -1;
		    } else
			dir = 0;
	    } break;

	case SOUTH:
	    { if (mapy < WORLD_SIDE_LEN)
		if (inc = check_grid(mapx, mapy + 1, 0, 1)) {
		    mapy += inc;
		    dir = -1;
		} else
		    dir = 0;
	    } break;
	}
    }

    level--;
    /*  printf("exiting recurse_power_grid:level %d\n",level);*/
}




/* project_power
   Get the appropriate number from the proper variable */

void 
project_power(int mapx, int mapy) 
{
  switch (MP_GROUP(mapx,mapy)) {
  case GROUP_COAL_POWER: 
    {
      grid[grid_num]->max_power += MP_INFO(mapx,mapy).int_1;
    } break;
  case GROUP_SOLAR_POWER: 
    {
      grid[grid_num]->max_power += MP_INFO(mapx,mapy).int_3;
    } break;
  case GROUP_WINDMILL: 
    { 
      grid[grid_num]->max_power += MP_INFO(mapx,mapy).int_1;
    } break;
  default: 
    {
      printf("default case in project_power");
      printf("\tMP_GROUP = %d\n",MP_GROUP(mapx,mapy));
    } break;
  }
}


/* get_power
   get power for thing at x, y.  Don't use windmills if industry.
   We go through a list (ugly, I know) until we find a substation in range
   and then try and get power from it's grid.  If we can't, continue.
*/

int 
get_power (int x, int y, int power, int block_industry)
{

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

	grid[grid_tmp]->demand += power;
	if (grid[grid_tmp]->total_power >= power) {
	  grid[grid_tmp]->total_power -= power;
	  MP_INFO(xi,yi).int_5 += power;
	  return 1;
	}
	
      }
    }
  return 0;
}
	  

/*** Substations ***/
/*
  int_5 is the power demand at this substation
  int_6 is the grid its connected to
  int_7 is a grid timestamp
*/

void 
do_power_substation (int x, int y) 
{
    switch(grid[MP_INFO(x,y).int_6]->powered) {
    case -1: {
	MP_TYPE(x,y) = CST_SUBSTATION_R; 
    } break;
    case 0 : {
	MP_TYPE(x,y) = CST_SUBSTATION_RG;
    } break;
    case 1 : {
	MP_TYPE(x,y) = CST_SUBSTATION_G;
    } break;
    default : {
	printf("Default case in do_power_substation\n");
    } break;
    }
}

int
add_a_substation (int x, int y)	/* add to substationx substationy to list */
{
  if (numof_substations >= MAX_NUMOF_SUBSTATIONS)
    return (0);
  substationx[numof_substations] = x;
  substationy[numof_substations] = y;
  numof_substations++;
  return (1);
}

void
remove_a_substation (int x, int y)
{
  int q;
  for (q = 0; q < numof_substations; q++)
    if (substationx[q] == x && substationy[q] == y)
      break;
  for (; q < numof_substations; q++)
    {
      substationx[q] = substationx[q + 1];
      substationy[q] = substationy[q + 1];
    }
  numof_substations--;
}

void
shuffle_substations (void)
{
  int q, x, r, m;
  m = (numof_substations / 2) + 1;
  for (x = 0; x < m; x++)
    {
      r = rand () % numof_substations;
      if (r == x)
	continue;
      q = substationx[x];
      substationx[x] = substationx[r];
      substationx[r] = q;
      q = substationy[x];
      substationy[x] = substationy[r];
      substationy[r] = q;
    }
}


/*** Windmills ***/
/*
  // int_1 is the rated capacity 
  // int_2 is the tech level when built
  // int_3 is the sail count - to choose the right sail.
  // int_4 is the last real time that a sail was turned
  // int_5 is the power produced (basically _if_ power produced)
  // int_6 is the grid it's on
  // int_7 is a timestamp for mapping
*/
void
do_windmill (int x, int y) 
{
  int anim_tile; 

  if (get_jobs (x, y, WINDMILL_JOBS) != 0) {
    MP_INFO(x,y).int_5 = MP_INFO(x,y).int_1;
    grid[MP_INFO(x,y).int_6]->avail_power += MP_INFO(x,y).int_1;
  } else {
    MP_INFO(x,y).int_4 = real_time + MODERN_WINDMILL_ANIM_SPEED;
    return;
  }

  /* update animation */
  if (real_time > MP_INFO(x,y).int_4) {
    MP_INFO(x,y).int_3++;
    if (MP_INFO(x,y).int_2 < MODERN_WINDMILL_TECH) {
      MP_INFO(x,y).int_4 = real_time + ANTIQUE_WINDMILL_ANIM_SPEED;
    } else {
      MP_INFO(x,y).int_4 = real_time + MODERN_WINDMILL_ANIM_SPEED;
    }
  }

  /* figure out which tile to use */
  anim_tile = (MP_INFO(x,y).int_3 % 3);

  if (MP_INFO(x,y).int_2 < MODERN_WINDMILL_TECH)
    MP_TYPE(x,y) = CST_WINDMILL_1_W + anim_tile;
  else
    switch(grid[MP_INFO(x,y).int_6]->powered) 
      {
      case -1: 
	MP_TYPE(x,y) = CST_WINDMILL_1_R + anim_tile; 
	break;
      case 0 : 
	MP_TYPE(x,y) = CST_WINDMILL_1_RG + anim_tile;
	break;
      case 1 : 
	MP_TYPE(x,y) = CST_WINDMILL_1_G + anim_tile;
	break;
      default : 
	printf("Default case in do_power_substation\n");
	break;
      }      
}


/*** Solar Power ***/

/*
  int_1 is the power to give away,  this must be >= POWER_LINE_CAPACITY
  for it to pass it to a power line. 
  Not added to if > POWER_LINE_CAPACITY
  int_2 is the tech level when it was built.
  int_3 is the rated output
  int_5 is the current output
  int_6 is the grid it's on
  int_7 is a grid mapping timestamp
*/

void
do_power_source (int x, int y)
{
    if (get_jobs(x, y, SOLAR_POWER_JOBS)) {
	MP_INFO(x,y).int_5 = MP_INFO(x,y).int_3;
	grid[MP_INFO(x,y).int_6]->avail_power += MP_INFO(x,y).int_3;
    } else {
	MP_INFO(x,y).int_5 = 0;
    }
}


/*** Coal Power ***/
/*
  // int_1 is the maximum possible power
  // int_2 is the coal at the power station
  // int_3 is the stored jobs... Must be an interesting warehouse
  // int_4 is the tech level when built
  // int_5 is the projected output.
*/


void
do_power_source_coal (int x, int y)
{

    /* Need coal?  Try transport. */
    if (MP_INFO(x,y).int_2 < MAX_COAL_AT_POWER_STATION) {

	/* left side */
	if (XY_IS_TRANSPORT(x-1, y+1) && MP_INFO(x-1, y+1).int_3 > 0) {
	    if (get_jobs (x, y, JOBS_LOAD_COAL) != 0)
	    {
		MP_INFO(x,y).int_2 += (MP_INFO(x-1, y+1).int_3 / 2
				       + ((MP_INFO(x-1, y+1).int_3) % 2));
		MP_INFO(x-1, y+1).int_3 /= 2;
		MP_POL(x,y)++;
	    }
	}
	/* top side */
	else if (XY_IS_TRANSPORT(x+1, y-1) && MP_INFO(x+1, y-1).int_3 > 0) {
	    if (get_jobs (x, y, JOBS_LOAD_COAL) != 0)
		MP_INFO(x,y).int_2 += (MP_INFO(x+1, y-1).int_3 / 2
				       + ((MP_INFO(x+1, y-1).int_3) % 2));
	    MP_INFO(x + 1,y - 1).int_3 /= 2;
	    MP_POL(x,y)++;
	}
    }

    /* Need jobs?  get_jobs. */
    if ((MP_INFO(x,y).int_3 + JOBS_COALPS_GENERATE + 10)
	< MAX_JOBS_AT_COALPS)
	if (get_jobs (x, y, JOBS_COALPS_GENERATE + 10) != 0)
	    MP_INFO(x,y).int_3 += JOBS_COALPS_GENERATE + 10;

    /* Generate Power */
    if (MP_INFO(x,y).int_2 > POWERS_COAL_OUTPUT / 500 &&
	MP_INFO(x,y).int_3 > JOBS_COALPS_GENERATE) 
    {
	MP_INFO(x,y).int_5 = MP_INFO(x,y).int_1;
	MP_INFO(x,y).int_3 -= JOBS_COALPS_GENERATE;
	MP_INFO(x,y).int_2 -= POWERS_COAL_OUTPUT / 500;
	coal_used += POWERS_COAL_OUTPUT / 500;
	MP_POL(x,y) += POWERS_COAL_POLLUTION;
	grid[MP_INFO(x,y).int_6]->avail_power += MP_INFO(x,y).int_1;
    }

    /* Animation */
    /* choose a graphic */
    if (MP_INFO(x,y).int_2 > (MAX_COAL_AT_POWER_STATION
			      - (MAX_COAL_AT_POWER_STATION / 5)))
	MP_TYPE(x,y) = CST_POWERS_COAL_FULL;
    else if (MP_INFO(x,y).int_2 > (MAX_COAL_AT_POWER_STATION / 2))
	MP_TYPE(x,y) = CST_POWERS_COAL_MED;
    else if (MP_INFO(x,y).int_2 > (MAX_COAL_AT_POWER_STATION / 10))
	MP_TYPE(x,y) = CST_POWERS_COAL_LOW;
    else
	MP_TYPE(x,y) = CST_POWERS_COAL_EMPTY;
}


/*** Power Lines ***/
/*
  int_5 is animation schedule
  int_6 is the grid it is on
  int_7 is a grid timestamp
*/
void
do_power_line (int x, int y)
{
    if (grid[MP_INFO(x,y).int_6]->powered == -1)
	return;

    switch(MP_INFO(x,y).int_5) 
    {
    case 0: 
	MP_INFO(x,y).int_5 = POWER_MODULUS;
	break;
    case 1:
	if (!(MP_TYPE(x,y) <= 11 && MP_TYPE(x,y) >= 1))
	    break;
	MP_TYPE(x,y) += 11;
	break;
    case 2:
	if (!(MP_TYPE(x,y) >= 11 && MP_TYPE(x,y) <= 22))
	    break;
	MP_TYPE(x,y) -= 11;
	break;
    }

    MP_INFO(x,y).int_5--;
}
