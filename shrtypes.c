/* ---------------------------------------------------------------------- *
 * shrtypes.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include "lcconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include "lcstring.h"
#include "common.h"
#include "lctypes.h"
#include "lin-city.h"
#include "typeinit.h"
#include "cliglobs.h"
#include "engglobs.h"


void
set_mappoint_used (int fromx, int fromy, int x, int y)
{
    MP_TYPE(x,y) = CST_USED;
    MP_GROUP(x,y) = GROUP_USED;
    MP_INFO(x,y).int_1 = fromx;
    MP_INFO(x,y).int_2 = fromy;
}

  
void
set_mappoint (int x, int y, short selected_type)
{
  int grp;

  if ((grp = get_group_of_type(selected_type)) < 0) return;

  MP_TYPE(x,y) = selected_type;
  MP_GROUP(x,y) = grp;

  if (main_groups[grp].size == 2)
    {
      set_mappoint_used (x, y, x + 1, y);
      set_mappoint_used (x, y, x, y + 1);
      set_mappoint_used (x, y, x + 1, y + 1);
    }
  else if (main_groups[grp].size == 3)
    {
      set_mappoint_used (x, y, x + 1, y);
      set_mappoint_used (x, y, x + 2, y);
      set_mappoint_used (x, y, x + 1, y + 1);
      set_mappoint_used (x, y, x + 2, y + 1);
      set_mappoint_used (x, y, x + 1, y + 2);
      set_mappoint_used (x, y, x + 2, y + 2);
      set_mappoint_used (x, y, x, y + 1);
      set_mappoint_used (x, y, x, y + 2);
    }
  else if (main_groups[grp].size == 4)
    {
      set_mappoint_used (x, y, x + 1, y);
      set_mappoint_used (x, y, x + 2, y);
      set_mappoint_used (x, y, x + 1, y + 1);
      set_mappoint_used (x, y, x + 2, y + 1);
      set_mappoint_used (x, y, x + 1, y + 2);
      set_mappoint_used (x, y, x + 2, y + 2);
      set_mappoint_used (x, y, x, y + 1);
      set_mappoint_used (x, y, x, y + 2);

      set_mappoint_used (x, y, x + 3, y);
      set_mappoint_used (x, y, x + 3, y + 1);
      set_mappoint_used (x, y, x + 3, y + 2);
      set_mappoint_used (x, y, x + 3, y + 3);
      set_mappoint_used (x, y, x, y + 3);
      set_mappoint_used (x, y, x + 1, y + 3);
      set_mappoint_used (x, y, x + 2, y + 3);
    }
}


void
connect_transport (int originx, int originy, int w, int h)
{
  int x, y, mask, tflags;
  short group, type;

  static const short power_table[16] =
  {
    CST_POWERL_H_D, CST_POWERL_V_D, CST_POWERL_H_D, CST_POWERL_RD_D,
    CST_POWERL_H_D, CST_POWERL_LD_D, CST_POWERL_H_D, CST_POWERL_LDR_D,
    CST_POWERL_V_D, CST_POWERL_V_D, CST_POWERL_RU_D, CST_POWERL_UDR_D,
    CST_POWERL_LU_D, CST_POWERL_LDU_D, CST_POWERL_LUR_D, CST_POWERL_LUDR_D
  };
  static const short track_table[16] =
  {
    CST_TRACK_LR, CST_TRACK_LR, CST_TRACK_UD, CST_TRACK_LU,
    CST_TRACK_LR, CST_TRACK_LR, CST_TRACK_UR, CST_TRACK_LUR,
    CST_TRACK_UD, CST_TRACK_LD, CST_TRACK_UD, CST_TRACK_LUD,
    CST_TRACK_DR, CST_TRACK_LDR, CST_TRACK_UDR, CST_TRACK_LUDR
  };
  static const short road_table[16] =
  {
    CST_ROAD_LR, CST_ROAD_LR, CST_ROAD_UD, CST_ROAD_LU,
    CST_ROAD_LR, CST_ROAD_LR, CST_ROAD_UR, CST_ROAD_LUR,
    CST_ROAD_UD, CST_ROAD_LD, CST_ROAD_UD, CST_ROAD_LUD,
    CST_ROAD_DR, CST_ROAD_LDR, CST_ROAD_UDR, CST_ROAD_LUDR
  };
  static const short rail_table[16] =
  {
    CST_RAIL_LR, CST_RAIL_LR, CST_RAIL_UD, CST_RAIL_LU,
    CST_RAIL_LR, CST_RAIL_LR, CST_RAIL_UR, CST_RAIL_LUR,
    CST_RAIL_UD, CST_RAIL_LD, CST_RAIL_UD, CST_RAIL_LUD,
    CST_RAIL_DR, CST_RAIL_LDR, CST_RAIL_UDR, CST_RAIL_LUDR
  }; 
  static const short water_table[16] =
  {
    CST_WATER, CST_WATER_D, CST_WATER_R, CST_WATER_RD,
    CST_WATER_L, CST_WATER_LD, CST_WATER_LR, CST_WATER_LRD,
    CST_WATER_U, CST_WATER_UD, CST_WATER_UR, CST_WATER_URD,
    CST_WATER_LU, CST_WATER_LUD, CST_WATER_LUR, CST_WATER_LURD
  };

  /* Adjust originx,originy,w,h to proper range */
  if (originx <= 0) {
    w -= 1 - originx;
    originx = 1;
  }
  if (originy <= 0) {
    h -= 1 - originy;
    originy = 1;
  }
  if (originx + w >= WORLD_SIDE_LEN) {
    w = WORLD_SIDE_LEN - originx;
  }
  if (originy + h >= WORLD_SIDE_LEN) {
    h = WORLD_SIDE_LEN - originy;
  }
  
  for (x = originx; x < originx + w; x++) {
    for (y = originy; y < originy + h; y++) {
      switch (MP_GROUP(x,y))
	{
	case GROUP_POWER_LINE:
	  /* First, set up a mask indicating into which directions 
	     power may be transferred */
	  mask = 0;
#ifdef	THOMMY_MAY_BE_WRONG	/* just in case -- (ThMO) */
	  if (y > 0)
#endif
	    {		/* up -- (ThMO) */
	      group = MP_GROUP(x,y-1);

	      /* see if dug under track, rail or road */

	      if (y > 1 && (group == GROUP_TRACK
			    || group == GROUP_RAIL
			    || group == GROUP_ROAD
			    || group == GROUP_WATER))
		group = MP_GROUP(x,y-2);
	      switch (group)
		{
		case GROUP_POWER_LINE:
		case GROUP_SOLAR_POWER:
		case GROUP_SUBSTATION:
		case GROUP_COAL_POWER:
		  mask |= 8;
		  break;
		}
	    }
#ifdef	THOMMY_MAY_BE_WRONG	/* just in case -- (ThMO) */
	  if (x > 0)
#endif
	    {		/* left -- (ThMO) */
	      group = MP_GROUP(x-1,y);
	      if (x > 1 && (group == GROUP_TRACK
			    || group == GROUP_RAIL
			    || group == GROUP_ROAD
			    || group == GROUP_WATER))
		group = MP_GROUP(x-2,y);
	      switch (group)
		{
		case GROUP_POWER_LINE:
		case GROUP_SOLAR_POWER:
		case GROUP_SUBSTATION:
		case GROUP_COAL_POWER:
		  mask |= 4;
		  break;
		}
	    }
#ifdef	THOMMY_MAY_BE_WRONG	/* just in case -- (ThMO) */
	  if (x < WORLD_SIDE_LEN - 1)
#endif
	    {		/* right -- (ThMO) */
	      group = MP_GROUP(x+1,y);
	      if (x < WORLD_SIDE_LEN - 2 && (group == GROUP_TRACK
					     || group == GROUP_RAIL
					     || group == GROUP_ROAD
					     || group == GROUP_WATER))
		group = MP_GROUP(x+2,y);
	      switch (group)
		{
		case GROUP_WINDMILL:
		  if (MP_INFO(x + 1,y).int_2 < MODERN_WINDMILL_TECH)
		    break;
		case GROUP_POWER_LINE:
		case GROUP_SOLAR_POWER:
		case GROUP_SUBSTATION:
		case GROUP_COAL_POWER:
		  mask |= 2;
		  break;
		}
	    }
#ifdef	THOMMY_MAY_BE_WRONG	/* just in case -- (ThMO) */
	  if (y < WORLD_SIDE_LEN - 1)
#endif
	    {		/* down -- (ThMO) */
	      group = MP_GROUP(x,y+1);
	      if (y < WORLD_SIDE_LEN - 2 && (group == GROUP_TRACK
					     || group == GROUP_RAIL
					     || group == GROUP_ROAD
					     || group == GROUP_WATER))
		group = MP_GROUP(x,y+2);
	      switch (group)
		{
		case GROUP_WINDMILL:
		  if (MP_INFO(x,y + 1).int_2 < MODERN_WINDMILL_TECH)
		    break;
		case GROUP_POWER_LINE:
		case GROUP_SOLAR_POWER:
		case GROUP_SUBSTATION:
		case GROUP_COAL_POWER:
		  ++mask;
		  break;
		}
	    }
	  /* Next, set the connectivity into MP_TYPE */
	  MP_TYPE(x,y) = power_table[mask];
	  /* Finally, adjust MP_TYPE to show electon bolt */
	  if (MP_INFO(x,y).int_1 != 0)
	    MP_TYPE(x,y) -= 11;
	  break;

	case GROUP_TRACK:
#if	FLAG_LEFT != 1 || FLAG_UP != 2 || FLAG_RIGHT != 4 || FLAG_DOWN != 8
#error  check_track_graphics(): you loose
#error  this algorithm depends on proper flag settings -- (ThMO)
#endif
	  mask = 0;
#ifdef	THOMMY_MAY_BE_WRONG	/* just in case -- (ThMO) */
	  if (y > 0)
#endif
	    {
	      if (MP_GROUP(x,y-1) == GROUP_TRACK)
		mask |= FLAG_UP;
	    }
#ifdef	THOMMY_MAY_BE_WRONG	/* just in case -- (ThMO) */
	  if (x > 0)
#endif
	    {
	      if (MP_GROUP(x-1,y) == GROUP_TRACK)
		mask |= FLAG_LEFT;
	    }
	  tflags = mask;
#ifdef	THOMMY_MAY_BE_WRONG	/* just in case -- (ThMO) */
	  if (x < WORLD_SIDE_LEN - 1)
#endif
	    {
	      switch (MP_GROUP(x+1,y))
		{
		case GROUP_TRACK:
		  tflags |= FLAG_RIGHT;
		case GROUP_COMMUNE:
		case GROUP_COALMINE:
		case GROUP_OREMINE:
		case GROUP_INDUSTRY_L:
		case GROUP_INDUSTRY_H:
		case GROUP_RECYCLE:
		case GROUP_TIP:
		case GROUP_PORT:
		  mask |= FLAG_RIGHT;
		  break;
		default:
#ifdef	THOMMY_MAY_BE_WRONG	/* just in case -- (ThMO) */
		  if (y > 0)
#endif
		    if (MP_GROUP(x+1,y-1) == GROUP_COAL_POWER)
		      mask |= FLAG_RIGHT;
		  break;
		}
	    }
#ifdef	THOMMY_MAY_BE_WRONG	/* just in case -- (ThMO) */
	  if (y < WORLD_SIDE_LEN - 1)
#endif
	    {
	      switch (MP_GROUP(x,y+1))
		{
		case GROUP_TRACK:
		  tflags |= FLAG_DOWN;
		case GROUP_COMMUNE:
		case GROUP_COALMINE:
		case GROUP_OREMINE:
		case GROUP_INDUSTRY_L:
		case GROUP_INDUSTRY_H:
		case GROUP_RECYCLE:
		case GROUP_TIP:
		case GROUP_PORT:
		  mask |= FLAG_DOWN;
		  break;
		default:
#ifdef	THOMMY_MAY_BE_WRONG	/* just in case -- (ThMO) */
		  if (x > 0)
#endif
		    if (MP_GROUP(x-1,y+1) == GROUP_COAL_POWER)
		      mask |= FLAG_DOWN;
		  break;
		}
	    }
	  MP_INFO(x,y).flags &= ~(FLAG_UP | FLAG_DOWN | FLAG_LEFT
				  | FLAG_RIGHT);
	  MP_INFO(x,y).flags |= tflags;
	  MP_TYPE(x,y) = track_table[mask];
	  break;

	case GROUP_ROAD:
#if	FLAG_LEFT != 1 || FLAG_UP != 2 || FLAG_RIGHT != 4 || FLAG_DOWN != 8
#error  check_road_graphics(): you loose
#error  this algorithm depends on proper flag settings -- (ThMO)
#endif
	  mask = 0;
#ifdef	THOMMY_MAY_BE_WRONG	/* just in case -- (ThMO) */
	  if (y > 0)
#endif
	    {
	      if (MP_GROUP(x,y-1) == GROUP_ROAD)
		mask |= FLAG_UP;
	    }
#ifdef	THOMMY_MAY_BE_WRONG	/* just in case -- (ThMO) */
	  if (x > 0)
#endif
	    {
	      if (MP_GROUP(x-1,y) == GROUP_ROAD)
		mask |= FLAG_LEFT;
	    }
	  tflags = mask;
#ifdef	THOMMY_MAY_BE_WRONG	/* just in case -- (ThMO) */
	  if (x < WORLD_SIDE_LEN - 1)
#endif
	    {
	      switch (MP_GROUP(x+1,y))
		{
		case GROUP_ROAD:
		  tflags |= FLAG_RIGHT;
		case GROUP_COMMUNE:
		case GROUP_COALMINE:
		case GROUP_OREMINE:
		case GROUP_INDUSTRY_L:
		case GROUP_INDUSTRY_H:
		case GROUP_RECYCLE:
		case GROUP_TIP:
		case GROUP_PORT:
		  mask |= FLAG_RIGHT;
		  break;
		default:
#ifdef	THOMMY_MAY_BE_WRONG	/* just in case -- (ThMO) */
		  if (y > 0)
#endif
		    if (MP_GROUP(x+1,y-1) == GROUP_COAL_POWER)
		      mask |= FLAG_RIGHT;
		  break;
		}
	    }
#ifdef	THOMMY_MAY_BE_WRONG	/* just in case -- (ThMO) */
	  if (y < WORLD_SIDE_LEN - 1)
#endif
	    {
	      switch (MP_GROUP(x,y+1))
		{
		case GROUP_ROAD:
		  tflags |= FLAG_DOWN;
		case GROUP_COMMUNE:
		case GROUP_COALMINE:
		case GROUP_OREMINE:
		case GROUP_INDUSTRY_L:
		case GROUP_INDUSTRY_H:
		case GROUP_RECYCLE:
		case GROUP_TIP:
		case GROUP_PORT:
		  mask |= FLAG_DOWN;
		  break;
		default:
#ifdef	THOMMY_MAY_BE_WRONG	/* just in case -- (ThMO) */
		  if (x > 0)
#endif
		    if (MP_GROUP(x-1,y+1) == GROUP_COAL_POWER)
		      mask |= FLAG_DOWN;
		  break;
		}
	    }
	  MP_INFO(x,y).flags &= ~(FLAG_UP | FLAG_DOWN | FLAG_LEFT
				  | FLAG_RIGHT);
	  MP_INFO(x,y).flags |= tflags;
	  MP_TYPE(x,y) = road_table[mask];
	  break;

	case GROUP_RAIL:
#if	FLAG_LEFT != 1 || FLAG_UP != 2 || FLAG_RIGHT != 4 || FLAG_DOWN != 8
#error  check_rail_graphics(): you loose
#error  this algorithm depends on proper flag settings -- (ThMO)
#endif
	  mask = 0;
#ifdef	THOMMY_MAY_BE_WRONG	/* just in case -- (ThMO) */
	  if (y > 0)
#endif
	    {
	      if (MP_GROUP(x,y-1) == GROUP_RAIL)
		mask |= FLAG_UP;
	    }
#ifdef	THOMMY_MAY_BE_WRONG	/* just in case -- (ThMO) */
	  if (x > 0)
#endif
	    {
	      if (MP_GROUP(x-1,y) == GROUP_RAIL)
		mask |= FLAG_LEFT;
	    }
	  tflags = mask;
#ifdef	THOMMY_MAY_BE_WRONG	/* just in case -- (ThMO) */
	  if (x < WORLD_SIDE_LEN - 1)
#endif
	    {
	      switch (MP_GROUP(x+1,y))
		{
		case GROUP_RAIL:
		  tflags |= FLAG_RIGHT;
		case GROUP_COMMUNE:
		case GROUP_COALMINE:
		case GROUP_OREMINE:
		case GROUP_INDUSTRY_L:
		case GROUP_INDUSTRY_H:
		case GROUP_RECYCLE:
		case GROUP_TIP:
		case GROUP_PORT:
		  mask |= FLAG_RIGHT;
		  break;
		default:
#ifdef	THOMMY_MAY_BE_WRONG	/* just in case -- (ThMO) */
		  if (y > 0)
#endif
		    if (MP_GROUP(x+1,y-1) == GROUP_COAL_POWER)
		      mask |= FLAG_RIGHT;
		  break;
		}
	    }
#ifdef	THOMMY_MAY_BE_WRONG	/* just in case -- (ThMO) */
	  if (y < WORLD_SIDE_LEN - 1)
#endif
	    {
	      switch (MP_GROUP(x,y + 1))
		{
		case GROUP_RAIL:
		  tflags |= FLAG_DOWN;
		case GROUP_COMMUNE:
		case GROUP_COALMINE:
		case GROUP_OREMINE:
		case GROUP_INDUSTRY_L:
		case GROUP_INDUSTRY_H:
		case GROUP_RECYCLE:
		case GROUP_TIP:
		case GROUP_PORT:
		  mask |= FLAG_DOWN;
		  break;
		default:
#ifdef	THOMMY_MAY_BE_WRONG	/* just in case -- (ThMO) */
		  if (x > 0)
#endif
		    if (MP_GROUP(x - 1,y + 1)
			== GROUP_COAL_POWER)
		      mask |= FLAG_DOWN;
		  break;
		}
	    }
	  MP_INFO(x,y).flags &= ~(FLAG_UP | FLAG_DOWN | FLAG_LEFT
				  | FLAG_RIGHT);
	  MP_INFO(x,y).flags |= tflags;
	  MP_TYPE(x,y) = rail_table[mask];
	  break;

	case GROUP_WATER:
	  mask = 0;
#ifdef	THOMMY_MAY_BE_WRONG	/* just in case -- (ThMO) */
	  if (y > 0)
#endif
	    {		/* up -- (ThMO) */
	      if (MP_GROUP(x,y - 1)
		  == GROUP_WATER)
		mask |= 8;
	    }
#ifdef	THOMMY_MAY_BE_WRONG	/* just in case -- (ThMO) */
	  if (x > 0)
#endif
	    {		/* left -- (ThMO) */
	      type = MP_TYPE(x - 1,y);
	      if ((type == CST_USED && 
		   MP_GROUP(MP_INFO(x-1,y).int_1,MP_INFO(x-1,y).int_2)
		   == GROUP_PORT)
		  || get_group_of_type(type) == GROUP_WATER)
		mask |= 4;
	    }
#ifdef	THOMMY_MAY_BE_WRONG	/* just in case -- (ThMO) */
	  if (x < WORLD_SIDE_LEN - 1)
#endif
	    {		/* right -- (ThMO) */
	      if (MP_GROUP(x + 1,y)
		  == GROUP_WATER)
		mask |= 2;
	    }
#ifdef	THOMMY_MAY_BE_WRONG	/* just in case -- (ThMO) */
	  if (y < WORLD_SIDE_LEN - 1)
#endif
	    {		/* down -- (ThMO) */
	      if (MP_GROUP(x,y + 1)
		  == GROUP_WATER)
		++mask;
	    }
	  MP_TYPE(x,y) = water_table[mask];
	  break;
	} /* end switch */
    } /* end for */
  } /* end for */
}

