/* ---------------------------------------------------------------------- *
 * substation.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include <lin-city.h>
#include <lctypes.h>
#include <engglobs.h>
#include <cliglobs.h>
#include <stats.h>
#include <substation.h>
#include <power.h>

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

