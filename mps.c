/* ---------------------------------------------------------------------- *
 * mps.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include "lcconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lcstring.h"
#include "common.h"
#include "lctypes.h"
#include "lin-city.h"
#include "mps.h"
#include "engglobs.h"
#include "clistubs.h"
#include "mouse.h"
#include "screen.h"
#include "power.h"
#include "lcintl.h"

/* ---------------------------------------------------------------------- *
 * Private Fn Prototypes
 * ---------------------------------------------------------------------- */
void mps_global_setup (int);
void mps_global (int);

void mps_res_setup (void);
void mps_res (int, int);
void mps_transport_setup (void);
void mps_road (int, int);
void mps_rail (int, int);
void mps_track (int, int);
void mps_farm_setup (void);
void mps_farm (int, int);
void mps_market_setup (void);
void mps_market (int, int);
void mps_indl_setup (void);
void mps_indl (int, int);
void mps_indh_setup (void);
void mps_indh (int, int);
void mps_coalmine_setup (void);
void mps_coalmine (int, int);
void mps_power_source_coal_setup (void);
void mps_power_source_coal (int, int);
void mps_power_source_setup (void);
void mps_power_source (int, int);
void mps_power_line_setup (void);
void mps_power_line (int, int);
void mps_university_setup (void);
void mps_university (int, int);
void mps_recycle_setup (void);
void mps_recycle (int, int);
void mps_oremine_setup (void);
void mps_oremine (int, int);
void mps_substation_setup (void);
void mps_substation (int, int);
void mps_rocket_setup (void);
void mps_rocket (int, int);
void mps_windmill_setup (int, int);
void mps_windmill (int, int);
void mps_monument_setup (void);
void mps_monument (int, int);
void mps_school_setup (void);
void mps_school (int, int);
void mps_blacksmith_setup (void);
void mps_blacksmith (int, int);
void mps_mill_setup (void);
void mps_mill (int, int);
void mps_pottery_setup (void);
void mps_pottery (int, int);
void mps_water (int, int);
void mps_port_setup (int, int);
void mps_port (int, int);
void mps_tip_setup (void);
void mps_tip (int, int);
void mps_commune_setup (void);
void mps_commune (int, int);
void mps_right_setup (void);
void mps_right (int, int);
void mps_firestation_setup (void);
void mps_firestation (int, int);
void mps_cricket_setup (void);
void mps_cricket (int, int);
void mps_health_setup (void);
void mps_health (int, int);

/* ---------------------------------------------------------------------- *
 * Private Global Variables
 * ---------------------------------------------------------------------- */

/* ---------------------------------------------------------------------- *
 * Public Functions
 * ---------------------------------------------------------------------- */
void
mps_full_refresh (void)
{
    Rect* mps = &scr.mappoint_stats;

    draw_small_bezel (mps->x, mps->y, mps->w, mps->h, yellow(0));
    mappoint_stats (-3, -3, -3);
}

void
mappoint_stats (int x, int y, int button)
{
    static int xx = 0, yy = 0, oldbut = 0;
    Rect* mps = &scr.mappoint_stats;
    char s[100];

    /* mappoint_stats (-3,-3,-3) means continue using old values,
       but do a full refresh of setup strings */
    /* mappoint_stats (-2,-2,b) means global information 
       the "b" parameter identifies which kind of global style 
       should be displayed */
    /* mappoint_stats (-1,-1,-1) means continue using old values */
    if (x == -3) {
	Fgl_fillbox (mps->x, mps->y,
		     mps->w + 1, mps->h + 1, 14);
	Fgl_setfontcolors (14, TEXT_FG_COLOUR);
	mps_global_setup (oldbut);
	button = oldbut;
	Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
    } else if (x == -2) {
	if (button != oldbut) {
	    xx = x;
	    yy = y;
	    oldbut = button;
	    Fgl_fillbox (mps->x, mps->y,
			 mps->w + 1, mps->h + 1, 14);
	    Fgl_setfontcolors (14, TEXT_FG_COLOUR);
	    mps_global_setup (button);
	    Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
	}
    } else if (x == -1) {
	x = xx;
	y = yy;
	button = oldbut;
    } else {
	if (mappoint_stats_flag == 1 && x == xx && y == yy
	    && oldbut == LC_MOUSE_LEFTBUTTON
	    && button == LC_MOUSE_LEFTBUTTON)
	{
	    if (MP_GROUP(xx,yy) == GROUP_MARKET)
	    {
		clicked_market_cb (xx, yy);
		return;
	    }
	    else if (MP_GROUP(xx,yy) == GROUP_PORT)
	    {
		clicked_port_cb (xx, yy);
		return;
	    }
	}
	request_mappoint_stats (x, y);   /* Ask engine to forward MPS data */
	xx = x;
	yy = y;
	oldbut = button;
	mappoint_stats_flag = 1;
	/* draw centre of box */
	Fgl_fillbox (mps->x, mps->y,
		     mps->w + 1, mps->h + 1, 14);
	/* write static stuff */
#ifdef USE_EXPANDED_FONT
	gl_setwritemode (WRITEMODE_MASKED | FONT_EXPANDED);
#else
	Fgl_setfontcolors (14, TEXT_FG_COLOUR);
#endif
	strcpy (s, main_groups[MP_GROUP(x,y)].name);
	Fgl_write (mps->x + (14 - strlen (s)) * 4,
		   mps->y, s);

	if (button == LC_MOUSE_RIGHTBUTTON)
	    mps_right_setup ();
	else
	    switch (MP_GROUP(x,y))
	    {
	    case GROUP_POWER_LINE:
	      mps_power_line_setup ();
	      break;
	    case GROUP_RESIDENCE_LL:
	    case GROUP_RESIDENCE_ML:
	    case GROUP_RESIDENCE_HL:
	    case GROUP_RESIDENCE_LH:
	    case GROUP_RESIDENCE_MH:
	    case GROUP_RESIDENCE_HH:
		mps_res_setup ();
		break;
	    case (GROUP_ROAD):
		mps_transport_setup ();
		break;
	    case (GROUP_RAIL):
		mps_transport_setup ();
		break;
	    case (GROUP_TRACK):
		mps_transport_setup ();
		break;
	    case (GROUP_ORGANIC_FARM):
		mps_farm_setup ();
		break;
	    case (GROUP_MARKET):
		mps_market_setup ();
		break;
	    case (GROUP_INDUSTRY_L):
		mps_indl_setup ();
		break;
	    case (GROUP_INDUSTRY_H):
		mps_indh_setup ();
		break;
	    case (GROUP_COALMINE):
		mps_coalmine_setup ();
		break;
	    case GROUP_COAL_POWER:
		mps_power_source_coal_setup ();
		break;
	    case GROUP_SOLAR_POWER:
		mps_power_source_setup ();
		break;
	    case (GROUP_UNIVERSITY):
		mps_university_setup ();
		break;
	    case (GROUP_OREMINE):
		mps_oremine_setup ();
		break;
	    case (GROUP_RECYCLE):
		mps_recycle_setup ();
		break;
	    case (GROUP_SUBSTATION):
		mps_substation_setup ();
		break;
	    case (GROUP_ROCKET):
		mps_rocket_setup ();
		break;
	    case (GROUP_WINDMILL):
		mps_windmill_setup (x, y);
		break;
	    case (GROUP_MONUMENT):
		mps_monument_setup ();
		break;
	    case (GROUP_SCHOOL):
		mps_school_setup ();
		break;
	    case (GROUP_BLACKSMITH):
		mps_blacksmith_setup ();
		break;
	    case (GROUP_MILL):
		mps_mill_setup ();
		break;
	    case (GROUP_POTTERY):
		mps_pottery_setup ();
		break;
	    case (GROUP_PORT):
		mps_port_setup (x, y);
		break;
	    case (GROUP_TIP):
		mps_tip_setup ();
		break;
	    case (GROUP_COMMUNE):
		mps_commune_setup ();
		break;
	    case (GROUP_FIRESTATION):
		mps_firestation_setup ();
		break;
	    case (GROUP_CRICKET):
		mps_cricket_setup ();
		break;
	    case (GROUP_HEALTH):
		mps_health_setup ();
		break;
	    }
    }
#ifdef USE_EXPANDED_FONT
    Fgl_fillbox (mps->x + 7 * 8, mps->y + 8,
		 mps->w - 7 * 8, mps->h - 8, 14);
    gl_setwritemode (WRITEMODE_MASKED | FONT_EXPANDED);
#else
    Fgl_setfontcolors (14, TEXT_FG_COLOUR);
#endif
    if (x == -2 || x == -3) {
	mps_global (button);
    } else if (button == LC_MOUSE_RIGHTBUTTON) {
	mps_right (x, y);
    } else {
	switch (MP_GROUP(x,y))
	{
	case GROUP_POWER_LINE:
            mps_power_line (x,y);
	    break;
	case GROUP_RESIDENCE_LL:
	case GROUP_RESIDENCE_ML:
	case GROUP_RESIDENCE_HL:
	case GROUP_RESIDENCE_LH:
	case GROUP_RESIDENCE_MH:
	case GROUP_RESIDENCE_HH:
	    mps_res (x, y);
	    break;
	case (GROUP_ROAD):
	    mps_road (x, y);
	    break;
	case (GROUP_RAIL):
	    mps_rail (x, y);
	    break;
	case (GROUP_TRACK):
	    mps_track (x, y);
	    break;
	case (GROUP_ORGANIC_FARM):
	    mps_farm (x, y);
	    break;
	case (GROUP_MARKET):
	    mps_market (x, y);
	    break;
	case (GROUP_INDUSTRY_L):
	    mps_indl (x, y);
	    break;
	case (GROUP_INDUSTRY_H):
	    mps_indh (x, y);
	    break;
	case (GROUP_COALMINE):
	    mps_coalmine (x, y);
	    break;
	case GROUP_COAL_POWER:
	    mps_power_source_coal (x, y);
	    break;
	case GROUP_SOLAR_POWER:
	    mps_power_source (x, y);
	    break;
	case (GROUP_UNIVERSITY):
	    mps_university (x, y);
	    break;
	case (GROUP_OREMINE):
	    mps_oremine (x, y);
	    break;
	case (GROUP_RECYCLE):
	    mps_recycle (x, y);
	    break;
	case (GROUP_SUBSTATION):
	    mps_substation (x, y);
	    break;
	case (GROUP_ROCKET):
	    mps_rocket (x, y);
	    break;
	case (GROUP_WINDMILL):
	    mps_windmill (x, y);
	    break;
	case (GROUP_MONUMENT):
	    mps_monument (x, y);
	    break;
	case (GROUP_SCHOOL):
	    mps_school (x, y);
	    break;
	case (GROUP_BLACKSMITH):
	    mps_blacksmith (x, y);
	    break;
	case (GROUP_MILL):
	    mps_mill (x, y);
	    break;
	case (GROUP_POTTERY):
	    mps_pottery (x, y);
	    break;
	case (GROUP_WATER):
	    mps_water (x, y);
	    break;
	case (GROUP_PORT):
	    mps_port (x, y);
	    break;
	case (GROUP_TIP):
	    mps_tip (x, y);
	    break;
	case (GROUP_COMMUNE):
	    mps_commune (x, y);
	    break;
	case (GROUP_FIRESTATION):
	    mps_firestation (x, y);
	    break;
	case (GROUP_CRICKET):
	    mps_cricket (x, y);
	    break;
	case (GROUP_HEALTH):
	    mps_health (x, y);
	    break;
	}
    }
#ifdef USE_EXPANDED_FONT
    gl_setwritemode (WRITEMODE_OVERWRITE | FONT_EXPANDED);
#else
    Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
#endif
}

void
mps_res_setup (void)
{
  Rect* mps = &scr.mappoint_stats;
  Fgl_write (mps->x, mps->y + 8, "People");
  Fgl_write (mps->x, mps->y + 16, "Power");
  Fgl_write (mps->x, mps->y + 24, "Fed");
  Fgl_write (mps->x, mps->y + 32, "Empld");
  Fgl_write (mps->x, mps->y + 40, "H cov");
  Fgl_write (mps->x, mps->y + 48, "F cov");
  Fgl_write (mps->x, mps->y + 56, "C cov");
  Fgl_write (mps->x, mps->y + 64, "Poll'n");
  Fgl_write (mps->x, mps->y + 80, "Job pro");
}

void
mps_res (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  char s[100];
  sprintf (s, "%d ", MP_INFO(x,y).population);
  Fgl_write (mps->x + 7 * 8, mps->y + 8, s);
  if ((MP_INFO(x,y).flags & FLAG_POWERED) != 0)
    strcpy (s, "YES");
  else
    strcpy (s, "NO ");
  Fgl_write (mps->x + 7 * 8, mps->y + 16, s);

  if ((MP_INFO(x,y).flags & FLAG_FED) != 0)
    strcpy (s, "YES");
  else
    strcpy (s, "NO ");
  Fgl_write (mps->x + 7 * 8, mps->y + 24, s);

  if ((MP_INFO(x,y).flags & FLAG_EMPLOYED) != 0)
    strcpy (s, "YES");
  else
    strcpy (s, "NO ");
  Fgl_write (mps->x + 7 * 8, mps->y + 32, s);

  if ((MP_INFO(x,y).flags & FLAG_HEALTH_COVER) != 0)
    strcpy (s, "YES");
  else
    strcpy (s, "NO ");
  Fgl_write (mps->x + 7 * 8, mps->y + 40, s);

  if ((MP_INFO(x,y).flags & FLAG_FIRE_COVER) != 0)
    strcpy (s, "YES");
  else
    strcpy (s, "NO ");
  Fgl_write (mps->x + 7 * 8, mps->y + 48, s);

  if ((MP_INFO(x,y).flags & FLAG_CRICKET_COVER) != 0)
    strcpy (s, "YES");
  else
    strcpy (s, "NO ");
  Fgl_write (mps->x + 7 * 8, mps->y + 56, s);

  /* pollution */
  sprintf (s, "%7d", MP_POL(x,y));
  Fgl_write (mps->x + 7 * 8, mps->y + 64, s);

  /* job prospects */
  if (MP_INFO(x,y).int_1 >= 10)
    sprintf (s, "   good");
  else
    sprintf (s, "%7d", MP_INFO(x,y).int_1);
  Fgl_write (mps->x + 7 * 8, mps->y + 80, s);
}

void
mps_transport_setup (void)
{
  Rect* mps = &scr.mappoint_stats;
  Fgl_write (mps->x, mps->y + 32, "Food");
  Fgl_write (mps->x, mps->y + 40, "Jobs");
  Fgl_write (mps->x, mps->y + 48, "Coal");
  Fgl_write (mps->x, mps->y + 56, "Goods");
  Fgl_write (mps->x, mps->y + 64, "Ore");
  Fgl_write (mps->x, mps->y + 72, "Steel");
  Fgl_write (mps->x, mps->y + 80, "Waste");
}

void
mps_road (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  char s[100];
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_1 * 100.0
	   / MAX_FOOD_ON_ROAD);
  Fgl_write (mps->x + 8 * 8, mps->y + 32, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_2 * 100.0
	   / MAX_JOBS_ON_ROAD);
  Fgl_write (mps->x + 8 * 8, mps->y + 40, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_3 * 100.0
	   / MAX_COAL_ON_ROAD);
  Fgl_write (mps->x + 8 * 8, mps->y + 48, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_4 * 100.0
	   / MAX_GOODS_ON_ROAD);
  Fgl_write (mps->x + 8 * 8, mps->y + 56, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_5 * 100.0
	   / MAX_ORE_ON_ROAD);
  Fgl_write (mps->x + 8 * 8, mps->y + 64, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_6 * 100
	   / MAX_STEEL_ON_ROAD);
  Fgl_write (mps->x + 8 * 8, mps->y + 72, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_7 * 100
	   / MAX_WASTE_ON_ROAD);
  Fgl_write (mps->x + 8 * 8, mps->y + 80, s);

}

void
mps_rail (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  char s[100];
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_1 * 100.0
	   / MAX_FOOD_ON_RAIL);
  Fgl_write (mps->x + 8 * 8, mps->y + 32, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_2 * 100.0
	   / MAX_JOBS_ON_RAIL);
  Fgl_write (mps->x + 8 * 8, mps->y + 40, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_3 * 100.0
	   / MAX_COAL_ON_RAIL);
  Fgl_write (mps->x + 8 * 8, mps->y + 48, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_4 * 100.0
	   / MAX_GOODS_ON_RAIL);
  Fgl_write (mps->x + 8 * 8, mps->y + 56, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_5 * 100.0
	   / MAX_ORE_ON_RAIL);
  Fgl_write (mps->x + 8 * 8, mps->y + 64, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_6 * 100
	   / MAX_STEEL_ON_RAIL);
  Fgl_write (mps->x + 8 * 8, mps->y + 72, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_7 * 100
	   / MAX_WASTE_ON_RAIL);
  Fgl_write (mps->x + 8 * 8, mps->y + 80, s);

}

void
mps_track (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  char s[100];
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_1 * 100.0
	   / MAX_FOOD_ON_TRACK);
  Fgl_write (mps->x + 8 * 8, mps->y + 32, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_2 * 100.0
	   / MAX_JOBS_ON_TRACK);
  Fgl_write (mps->x + 8 * 8, mps->y + 40, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_3 * 100.0
	   / MAX_COAL_ON_TRACK);
  Fgl_write (mps->x + 8 * 8, mps->y + 48, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_4 * 100.0
	   / MAX_GOODS_ON_TRACK);
  Fgl_write (mps->x + 8 * 8, mps->y + 56, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_5 * 100.0
	   / MAX_ORE_ON_TRACK);
  Fgl_write (mps->x + 8 * 8, mps->y + 64, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_6 * 100
	   / MAX_STEEL_ON_TRACK);
  Fgl_write (mps->x + 8 * 8, mps->y + 72, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_7 * 100
	   / MAX_WASTE_ON_TRACK);
  Fgl_write (mps->x + 8 * 8, mps->y + 80, s);

}

void
mps_market_setup (void)
{
  mps_transport_setup ();
  /* put flags in */
}

void
mps_market (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  char s[100];
  float f;
  f = (float) MP_INFO(x,y).int_1 * 100.0 / MAX_FOOD_IN_MARKET;
  if (f > 100.0)
    f = 100.0;
  sprintf (s, "%5.1f%%", f);
  Fgl_write (mps->x + 8 * 8, mps->y + 32, s);
  f = (float) MP_INFO(x,y).int_2 * 100.0 / MAX_JOBS_IN_MARKET;
  if (f > 100.0)
    f = 100.0;
  sprintf (s, "%5.1f%%", f);
  Fgl_write (mps->x + 8 * 8, mps->y + 40, s);
  f = (float) MP_INFO(x,y).int_3 * 100.0 / MAX_COAL_IN_MARKET;
  if (f > 100.0)
    f = 100.0;
  sprintf (s, "%5.1f%%", f);
  Fgl_write (mps->x + 8 * 8, mps->y + 48, s);
  f = (float) MP_INFO(x,y).int_4 * 100.0 / MAX_GOODS_IN_MARKET;
  if (f > 100.0)
    f = 100.0;
  sprintf (s, "%5.1f%%", f);
  Fgl_write (mps->x + 8 * 8, mps->y + 56, s);
  f = (float) MP_INFO(x,y).int_5 * 100.0 / MAX_ORE_IN_MARKET;
  if (f > 100.0)
    f = 100.0;
  sprintf (s, "%5.1f%%", f);
  Fgl_write (mps->x + 8 * 8, mps->y + 64, s);
  f = (float) MP_INFO(x,y).int_6 * 100 / MAX_STEEL_IN_MARKET;
  if (f > 100.0)
    f = 100.0;
  sprintf (s, "%5.1f%%", f);
  Fgl_write (mps->x + 8 * 8, mps->y + 72, s);
  f = (float) MP_INFO(x,y).int_7 * 100 / MAX_WASTE_IN_MARKET;
  if (f > 100.0)
    f = 100.0;
  sprintf (s, "%5.1f%%", f);
  Fgl_write (mps->x + 8 * 8, mps->y + 80, s);

}

void
mps_farm_setup (void)
{
  Rect* mps = &scr.mappoint_stats;
  Fgl_write (mps->x, mps->y + 16, "Power");
  Fgl_write (mps->x, mps->y + 40, "Tech");
  Fgl_write (mps->x, mps->y + 48, "Prod");
}

void
mps_farm (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  char s[100];
  if ((MP_INFO(x,y).flags & FLAG_POWERED) != 0)
    strcpy (s, "YES");
  else
    strcpy (s, "NO ");
  Fgl_write (mps->x + 7 * 8, mps->y + 16, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_1 * 100.0
	   / MAX_TECH_LEVEL);
  Fgl_write (mps->x + 8 * 8, mps->y + 40, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_4 * 100.0
	   / 1200.0);
  Fgl_write (mps->x + 8 * 8, mps->y + 48, s);

}

void
mps_indl_setup (void)
{
  Rect* mps = &scr.mappoint_stats;
  Fgl_write (mps->x, mps->y + 16, "Power");
  Fgl_write (mps->x, mps->y + 40, "Output");
  Fgl_write (mps->x, mps->y + 48, "Store");
  Fgl_write (mps->x, mps->y + 56, "Ore");
  Fgl_write (mps->x, mps->y + 64, "Steel");
  Fgl_write (mps->x, mps->y + 80, "Capacity");
}

void
mps_indl (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  char s[100];
  float f;
  if ((MP_INFO(x,y).flags & FLAG_POWERED) != 0)
    strcpy (s, "YES");
  else
    strcpy (s, "NO ");
  Fgl_write (mps->x + 7 * 8, mps->y + 16, s);
  sprintf (s, "%7d", MP_INFO(x,y).int_1);
  Fgl_write (mps->x + 7 * 8, mps->y + 40, s);
  f = (float) MP_INFO(x,y).int_2 * 100.0 / MAX_GOODS_AT_INDUSTRY_L;
  if (f > 100.0)
    f = 100.0;
  sprintf (s, "%3.1f%%", f);
  Fgl_write (mps->x + 9 * 8, mps->y + 48, s);
  f = (float) MP_INFO(x,y).int_3 * 100.0 / MAX_ORE_AT_INDUSTRY_L;
  if (f > 100.0)
    f = 100.0;
  sprintf (s, "%3.1f%%", f);
  Fgl_write (mps->x + 9 * 8, mps->y + 56, s);
  f = (float) MP_INFO(x,y).int_4 * 100.0 / MAX_STEEL_AT_INDUSTRY_L;
  if (f > 100.0)
    f = 100.0;
  sprintf (s, "%3.1f%%", f);
  Fgl_write (mps->x + 9 * 8, mps->y + 64, s);
  sprintf (s, "%4d%%", MP_INFO(x,y).int_6);
  Fgl_write (mps->x + 9 * 8, mps->y + 80, s);

}

void
mps_indh_setup (void)
{
  Rect* mps = &scr.mappoint_stats;
  Fgl_write (mps->x, mps->y + 16, "Power");
  Fgl_write (mps->x, mps->y + 40, "Output");
  Fgl_write (mps->x, mps->y + 48, "Store");
  Fgl_write (mps->x, mps->y + 56, "Ore");
  Fgl_write (mps->x, mps->y + 64, "Coal");
  Fgl_write (mps->x, mps->y + 80, "Capacity");
}

void
mps_indh (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  char s[100];
  float f;
  if ((MP_INFO(x,y).flags & FLAG_POWERED) != 0)
    strcpy (s, "YES");
  else
    strcpy (s, "NO ");
  Fgl_write (mps->x + 7 * 8, mps->y + 16, s);
  sprintf (s, "%7d", MP_INFO(x,y).int_1);
  Fgl_write (mps->x + 7 * 8, mps->y + 40, s);
  f = (float) MP_INFO(x,y).int_2 * 100.0 / MAX_STEEL_AT_INDUSTRY_H;
  if (f > 100.0)
    f = 100.0;
  sprintf (s, "%5.1f%%", f);
  Fgl_write (mps->x + 8 * 8, mps->y + 48, s);
  f = (float) MP_INFO(x,y).int_3 * 100.0 / MAX_ORE_AT_INDUSTRY_H;
  if (f > 100.0)
    f = 100.0;
  sprintf (s, "%5.1f%%", f);
  Fgl_write (mps->x + 8 * 8, mps->y + 56, s);
  f = (float) MP_INFO(x,y).int_4 * 100.0 / MAX_COAL_AT_INDUSTRY_H;
  if (f > 100.0)
    f = 100.0;
  sprintf (s, "%5.1f%%", f);
  Fgl_write (mps->x + 8 * 8, mps->y + 64, s);
  sprintf (s, "%4d%%", MP_INFO(x,y).int_5);
  Fgl_write (mps->x + 8 * 8, mps->y + 80, s);
}

void
mps_coalmine_setup (void)
{
  Rect* mps = &scr.mappoint_stats;
  Fgl_write (mps->x, mps->y + 40, "Stock");
  Fgl_write (mps->x, mps->y + 48, "Reserve");
}

void
mps_coalmine (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  char s[100];
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_1 * 100
	   / MAX_COAL_AT_MINE);
  Fgl_write (mps->x + 8 * 8, mps->y + 40, s);
  if (MP_INFO(x,y).int_2 > 0)
    sprintf (s, "%7d", MP_INFO(x,y).int_2);
  else
    sprintf (s, " EMPTY ");
  Fgl_write (mps->x + 7 * 8, mps->y + 48, s);
}

void
mps_power_source_coal_setup (void)
{
  Rect* mps = &scr.mappoint_stats;
  Fgl_write (mps->x, mps->y + 40, "Cycle");
  Fgl_write (mps->x, mps->y + 48, "Coal");
  Fgl_write (mps->x, mps->y + 56, "Jobs");
  Fgl_write (mps->x, mps->y + 64, "Tech");
  Fgl_write (mps->x, mps->y + 72, "Grid");
  Fgl_write (mps->x, mps->y + 80, "Capacity");
}

void
mps_power_source_coal (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  char s[100];
  float f;
  f = (float) MP_INFO(x,y).int_1 * 100 / POWER_LINE_CAPACITY;
  if (f > 100.0)
    f = 100.0;
  sprintf (s, "%5.1f%%", f);
  Fgl_write (mps->x + 8 * 8, mps->y + 40, s);
  f = (float) MP_INFO(x,y).int_2 * 100 / MAX_COAL_AT_POWER_STATION;
  if (f > 100.0)
    f = 100.0;
  sprintf (s, "%5.1f%%", f);
  Fgl_write (mps->x + 8 * 8, mps->y + 48, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_3 * 100
	   / MAX_JOBS_AT_COALPS);
  Fgl_write (mps->x + 8 * 8, mps->y + 56, s);
  /* tech level is int_4 */
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_4 * 100
	   / MAX_TECH_LEVEL);
  Fgl_write (mps->x + 8 * 8, mps->y + 64, s);
  /* Grid number is int_6 */
  sprintf (s, "%d", MP_INFO(x,y).int_6);
  Fgl_write (mps->x + 8 * 8, mps ->y + 72, s);

  sprintf (s, "%d", MP_INFO(x,y).int_1);
  Fgl_write (mps->x + 8 * 8, mps ->y + 80, s);
}

void
mps_power_line_setup (void)
{
  Rect* mps = &scr.mappoint_stats;
  Fgl_write (mps->x, mps->y + 40, "Debug");
}

void
mps_power_line (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  char s[100];

  sprintf (s, "%d",MP_INFO(x,y).int_5);
  Fgl_write (mps->x + 8 * 8, mps->y + 40, s);
}

void
mps_power_source_setup (void)
{
  Rect* mps = &scr.mappoint_stats;
  Fgl_write (mps->x, mps->y + 40, "Cycle");
  Fgl_write (mps->x, mps->y + 48, "Tech");
  Fgl_write (mps->x, mps->y + 56, "Capacity");
  Fgl_write (mps->x, mps->y + 72, "Grid");
}

void
mps_power_source (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  char s[100];
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_1 * 100
	   / POWER_LINE_CAPACITY);
  Fgl_write (mps->x + 8 * 8, mps->y + 40, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_2 * 100
	   / MAX_TECH_LEVEL);
  Fgl_write (mps->x + 8 * 8, mps->y + 48, s);
  sprintf (s, "%d", MP_INFO(x,y).int_3);
  Fgl_write (mps->x + 8 * 8, mps ->y + 56, s);
  /* Grid number is int_6 */
  sprintf (s, "%d", MP_INFO(x,y).int_6);
  Fgl_write (mps->x + 8 * 8, mps ->y + 72, s);
}

void
mps_university_setup (void)
{
  Rect* mps = &scr.mappoint_stats;
  Fgl_write (mps->x, mps->y + 40, "Jobs");
  Fgl_write (mps->x, mps->y + 48, "Goods");
  Fgl_write (mps->x, mps->y + 56, "T made");
  Fgl_write (mps->x, mps->y + 64, "Capacity");
}

void
mps_university (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  char s[100];
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_1 * 100
	   / UNIVERSITY_JOBS_STORE);
  Fgl_write (mps->x + 8 * 8, mps->y + 40, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_2 * 100
	   / UNIVERSITY_GOODS_STORE);
  Fgl_write (mps->x + 8 * 8, mps->y + 48, s);
  sprintf (s, "%6.1f", (float) MP_INFO(x,y).int_3 * 100.0
	   / MAX_TECH_LEVEL);
  Fgl_write (mps->x + 8 * 8, mps->y + 56, s);
  sprintf (s, "%4d%%", MP_INFO(x,y).int_5);
  Fgl_write (mps->x + 9 * 8, mps->y + 64, s);
}

void
mps_recycle_setup (void)
{
  Rect* mps = &scr.mappoint_stats;
  Fgl_write (mps->x, mps->y + 16, "Power");
  Fgl_write (mps->x, mps->y + 40, "O stock");
  Fgl_write (mps->x, mps->y + 48, "W store");
  Fgl_write (mps->x, mps->y + 56, "S store");
  Fgl_write (mps->x, mps->y + 64, "Tech");
  Fgl_write (mps->x, mps->y + 80, "Capacity");
}

void
mps_recycle (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  char s[100];
  if ((MP_INFO(x,y).flags & FLAG_POWERED) != 0)
    strcpy (s, "YES");
  else
    strcpy (s, "NO ");
  Fgl_write (mps->x + 7 * 8, mps->y + 16, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_1 * 100
	   / MAX_ORE_AT_RECYCLE);
  Fgl_write (mps->x + 8 * 8, mps->y + 40, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_2 * 100
	   / MAX_WASTE_AT_RECYCLE);
  Fgl_write (mps->x + 8 * 8, mps->y + 48, s);
  Fgl_write (mps->x + 9 * 8, mps->y + 56, "-");
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_4 * 100
	   / MAX_TECH_LEVEL);
  Fgl_write (mps->x + 8 * 8, mps->y + 64, s);
  sprintf (s, "%4d%%", MP_INFO(x,y).int_6);
  Fgl_write (mps->x + 8 * 8, mps->y + 80, s);
}

void
mps_oremine_setup (void)
{
  Rect* mps = &scr.mappoint_stats;
  Fgl_write (mps->x, mps->y + 40, "Stock");
  Fgl_write (mps->x, mps->y + 48, "Reserve");
}

void
mps_oremine (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  char s[100];
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_1 * 100
	   / DIG_MORE_ORE_TRIGGER);
  Fgl_write (mps->x + 8 * 8, mps->y + 40, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_2 * 100
	   / (ORE_RESERVE * 16));
  Fgl_write (mps->x + 8 * 8, mps->y + 48, s);
}

void
mps_substation_setup (void)
{
  Rect* mps = &scr.mappoint_stats;
  Fgl_write (mps->x, mps->y + 40, _("Grid"));
  Fgl_write (mps->x, mps->y + 48, _("Max"));
  Fgl_write (mps->x, mps->y + 56, _("Avail"));
  Fgl_write (mps->x, mps->y + 64, _("Demand"));
  Fgl_write (mps->x, mps->y + 72, _("Here"));
}

void
mps_substation (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  char s[100];
  /* Grid number is int_6 */
  sprintf (s, "%d", MP_INFO(x,y).int_6);
  Fgl_write (mps->x + 8 * 8, mps ->y + 40, s);

  format_power (s, 100, grid[MP_INFO(x,y).int_6]->max_power);
  Fgl_write (mps->x + 8 * 8, mps->y + 48, s);

  format_power (s, 100, grid[MP_INFO(x,y).int_6]->avail_power);
  Fgl_write (mps->x + 8 * 8, mps->y + 56, s);

  format_power (s, 100, grid[MP_INFO(x,y).int_6]->demand);
  Fgl_write (mps->x + 8 * 8, mps->y + 64, s);
  
  format_power (s, 100, MP_INFO(x,y).int_5);
  Fgl_write (mps->x + 8 * 8, mps->y + 72, s);
	       
}

void
mps_rocket_setup (void)
{
  Rect* mps = &scr.mappoint_stats;
  Fgl_write (mps->x, mps->y + 40, "Jobs");
  Fgl_write (mps->x, mps->y + 48, "Goods");
  Fgl_write (mps->x, mps->y + 56, "Steel");
  Fgl_write (mps->x, mps->y + 64, "Launch");
}

void
mps_rocket (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  char s[100];
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_1 * 100
	   / ROCKET_PAD_JOBS_STORE);
  Fgl_write (mps->x + 8 * 8, mps->y + 40, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_2 * 100
	   / ROCKET_PAD_GOODS_STORE);
  Fgl_write (mps->x + 8 * 8, mps->y + 48, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_3 * 100
	   / ROCKET_PAD_STEEL_STORE);
  Fgl_write (mps->x + 8 * 8, mps->y + 56, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_4 * 100
	   / ROCKET_PAD_LAUNCH);
  Fgl_write (mps->x + 8 * 8, mps->y + 64, s);
  if (MP_TYPE(x,y) == CST_ROCKET_5)
    {
      if (yn_dial_box ("ROCKET LAUNCH"
		       ,"You can launch the rocket now or wait until later."
		       ,"If you wait, it costs you *only* money to keep the"
		       ,"rocket ready.    Launch?") != 0)
	  launch_rocket (x, y);
    }
}

void
mps_windmill_setup (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  Fgl_write (mps->x, mps->y + 24, "Tech");

  if (MP_INFO(x,y).int_2 >= MODERN_WINDMILL_TECH) {
    Fgl_write (mps->x, mps->y + 32, "Power");
    Fgl_write (mps->x, mps->y + 40, "Grid");
    Fgl_write (mps->x, mps->y + 48, "Max");
    Fgl_write (mps->x, mps->y + 56, "Avail");
    Fgl_write (mps->x, mps->y + 64, "Demand");
    Fgl_write (mps->x, mps->y + 72, "Here");
  }
}

void
mps_windmill (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  char s[10];

  snprintf (s, 10, "%5.1f%%", (float) MP_INFO(x,y).int_2 * 100
	   / MAX_TECH_LEVEL);
  Fgl_write (mps->x + 8 * 8, mps->y + 24, s);

  if (MP_INFO(x,y).int_2 >= MODERN_WINDMILL_TECH)
    { /* power level */
      format_power(s, 10, MP_INFO(x,y).int_1);
      Fgl_write (mps->x + 8 * 8, mps->y + 32, s);

      snprintf (s, 10, "%d", MP_INFO(x,y).int_6);
      Fgl_write (mps->x + 8 * 8, mps ->y + 40, s);
      
      format_power (s, 10, grid[MP_INFO(x,y).int_6]->max_power);
      Fgl_write (mps->x + 8 * 8, mps->y + 48, s);
      
      format_power (s, 10, grid[MP_INFO(x,y).int_6]->avail_power);
      Fgl_write (mps->x + 8 * 8, mps->y + 56, s);
      
      format_power (s, 10, grid[MP_INFO(x,y).int_6]->demand);
      Fgl_write (mps->x + 8 * 8, mps->y + 64, s);

      format_power (s, 10, MP_INFO(x,y).int_5);
      Fgl_write (mps->x + 8 * 8, mps->y + 72, s);

    }
}

void
mps_monument_setup (void)
{
  Rect* mps = &scr.mappoint_stats;
  Fgl_write (mps->x, mps->y + 40, "Built");
  Fgl_write (mps->x, mps->y + 48, "T made");
}


void
mps_monument (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  char s[100];
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_1 * 100
	   / BUILD_MONUMENT_JOBS);
  Fgl_write (mps->x + 8 * 8, mps->y + 40, s);
  sprintf (s, "%5.1f", (float) MP_INFO(x,y).int_2 * 100
	   / MAX_TECH_LEVEL);
  Fgl_write (mps->x + 8 * 8, mps->y + 48, s);
}

void
mps_school_setup (void)
{
  Rect* mps = &scr.mappoint_stats;
  Fgl_write (mps->x, mps->y + 40, "Jobs");
  Fgl_write (mps->x, mps->y + 48, "Goods");
  Fgl_write (mps->x, mps->y + 56, "T made");
  Fgl_write (mps->x, mps->y + 64, "Capacity");
}

void
mps_school (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  char s[100];
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_1 * 100
	   / MAX_JOBS_AT_SCHOOL);
  Fgl_write (mps->x + 8 * 8, mps->y + 40, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_2 * 100
	   / MAX_GOODS_AT_SCHOOL);
  Fgl_write (mps->x + 8 * 8, mps->y + 48, s);
  sprintf (s, "%6.1f", (float) MP_INFO(x,y).int_3 * 100
	   / MAX_TECH_LEVEL);
  Fgl_write (mps->x + 8 * 8, mps->y + 56, s);
  sprintf (s, "%5d%%", MP_INFO(x,y).int_5 * 4);
  Fgl_write (mps->x + 8 * 8, mps->y + 64, s);

}

void
mps_blacksmith_setup (void)
{
  Rect* mps = &scr.mappoint_stats;
  Fgl_write (mps->x, mps->y + 40, "G store");
  Fgl_write (mps->x, mps->y + 48, "C store");
  Fgl_write (mps->x, mps->y + 56, "Capacity");
}

void
mps_blacksmith (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  char s[100];
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_1 * 100
	   / MAX_GOODS_AT_BLACKSMITH);
  Fgl_write (mps->x + 8 * 8, mps->y + 40, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_3 * 100
	   / MAX_COAL_AT_BLACKSMITH);
  Fgl_write (mps->x + 8 * 8, mps->y + 48, s);
  sprintf (s, "%5d%%", MP_INFO(x,y).int_6);
  Fgl_write (mps->x + 8 * 8, mps->y + 56, s);
}

void
mps_mill_setup (void)
{
  Rect* mps = &scr.mappoint_stats;
  Fgl_write (mps->x, mps->y + 40, "G store");
  Fgl_write (mps->x, mps->y + 48, "F store");
  Fgl_write (mps->x, mps->y + 56, "C store");
  Fgl_write (mps->x, mps->y + 64, "Capacity");
}

void
mps_mill (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  char s[100];
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_1 * 100
	   / MAX_GOODS_AT_MILL);
  Fgl_write (mps->x + 8 * 8, mps->y + 40, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_2 * 100
	   / MAX_FOOD_AT_MILL);
  Fgl_write (mps->x + 8 * 8, mps->y + 48, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_3 * 100
	   / MAX_COAL_AT_MILL);
  Fgl_write (mps->x + 8 * 8, mps->y + 56, s);
  sprintf (s, "%5d%%", MP_INFO(x,y).int_6);
  Fgl_write (mps->x + 8 * 8, mps->y + 64, s);
}

void
mps_pottery_setup (void)
{
  Rect* mps = &scr.mappoint_stats;
  Fgl_write (mps->x, mps->y + 40, "G store");
  Fgl_write (mps->x, mps->y + 48, "O store");
  Fgl_write (mps->x, mps->y + 56, "C store");
  Fgl_write (mps->x, mps->y + 64, "Capacity");
}

void
mps_pottery (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  char s[100];
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_1 * 100
	   / MAX_GOODS_AT_POTTERY);
  Fgl_write (mps->x + 8 * 8, mps->y + 40, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_2 * 100
	   / MAX_ORE_AT_POTTERY);
  Fgl_write (mps->x + 8 * 8, mps->y + 48, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_3 * 100
	   / MAX_COAL_AT_POTTERY);
  Fgl_write (mps->x + 8 * 8, mps->y + 56, s);
  sprintf (s, "%5d%%", MP_INFO(x,y).int_6);
  Fgl_write (mps->x + 8 * 8, mps->y + 64, s);
}

void
mps_water (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  if (MP_INFO(x,y).flags & FLAG_IS_RIVER)
    Fgl_write (mps->x + 1 * 8, mps->y + 40
	       ,"  CONNECTED  ");
  else
    Fgl_write (mps->x + 1 * 8, mps->y + 40
	       ,"NOT CONNECTED");
  Fgl_write (mps->x, mps->y + 48, "to river sytem");
}

void
mps_port_setup (int x, int y)
{
    Rect* mps = &scr.mappoint_stats;

    Fgl_write (mps->x + 4, mps->y + 1*8+4, "     Buy   Sell");
    Fgl_write (mps->x + 4, mps->y + 3*8, "F");
    Fgl_write (mps->x + 4, mps->y + 4*8, "C");
    Fgl_write (mps->x + 4, mps->y + 5*8, "O");
    Fgl_write (mps->x + 4, mps->y + 6*8, "G");
    Fgl_write (mps->x + 4, mps->y + 7*8, "S");
}

void
mps_port (int x, int y)
{
    Rect* mps = &scr.mappoint_stats;
    int i, l;
    int *p1, *p2;
    char buy[256], sell[256], s[256];

    p1 = &(MP_INFO(x,y + 1).int_3);
    p2 = &(MP_INFO(x,y + 2).int_3);
    for (i = 0; i < 5; i++)
    {
	l = *(p1++) / 100;
	format_number5 (buy, l);
	l = *(p2++) / 100;
	format_number5 (sell, l);
	sprintf (s, " %s  %s", buy, sell);
	Fgl_write (mps->x + 20, mps->y + (3+i)*8, s);
    }

    /* Totals */
    l = MP_INFO(x,y).int_5 / 100;
    format_number5 (buy, l);
    l = MP_INFO(x,y).int_2 / 100;
    format_number5 (sell, l);
    sprintf (s, " %s  %s", buy, sell);
    Fgl_write (mps->x + 20, mps->y + (3+i)*8 + 4, s);
}

void
mps_tip_setup (void)
{
  Rect* mps = &scr.mappoint_stats;
  Fgl_write (mps->x, mps->y + 32, "  Waste stored");
  Fgl_write (mps->x, mps->y + 40, "   last month");
  Fgl_write (mps->x, mps->y + 64, "       % full");
}

void
mps_tip (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  char s[100];
  sprintf (s, "%7d", MP_INFO(x,y).int_3);
  Fgl_write (mps->x + 3 * 8, mps->y + 52, s);
  sprintf (s, "%4.1f", (float) (MP_INFO(x,y).int_1 * 100)
	   / (float) MAX_WASTE_AT_TIP);
  Fgl_write (mps->x + 3 * 8, mps->y + 64, s);
}

void
mps_commune_setup (void)
{
  Rect* mps = &scr.mappoint_stats;
  Fgl_write (mps->x, mps->y + 12, "   Activity");
  Fgl_write (mps->x, mps->y + 20, "  last month");
  Fgl_write (mps->x, mps->y + 36, "  Coal");
  Fgl_write (mps->x, mps->y + 44, "   Ore");
  Fgl_write (mps->x, mps->y + 52, " Steel");
  Fgl_write (mps->x, mps->y + 60, " Waste");
}

void
mps_commune (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  if ((MP_INFO(x,y).int_5 & 1) != 0)
    Fgl_write (mps->x + 7 * 8, mps->y + 36, "YES");
  else
    Fgl_write (mps->x + 7 * 8, mps->y + 36, "NO ");
  if ((MP_INFO(x,y).int_5 & 2) != 0)
    Fgl_write (mps->x + 7 * 8, mps->y + 44, "YES");
  else
    Fgl_write (mps->x + 7 * 8, mps->y + 44, "NO ");
  if ((MP_INFO(x,y).int_5 & 4) != 0)
    Fgl_write (mps->x + 7 * 8, mps->y + 52, "YES");
  else
    Fgl_write (mps->x + 7 * 8, mps->y + 52, "NO ");
  if ((MP_INFO(x,y).int_5 & 8) != 0)
    Fgl_write (mps->x + 7 * 8, mps->y + 60, "YES");
  else
    Fgl_write (mps->x + 7 * 8, mps->y + 60, "NO ");
}


void
mps_right_setup (void)
{
  Rect* mps = &scr.mappoint_stats;
  Fgl_write (mps->x, mps->y + 8, "  Fire cover");
  Fgl_write (mps->x, mps->y + 24, " Health cover");
  Fgl_write (mps->x, mps->y + 40, "Cricket cover");
  Fgl_write (mps->x, mps->y + 56, "  Pollution");
  Fgl_write (mps->x, mps->y + 76, " Bull");
}

void
mps_right (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  char s[100];
  int g;
  if ((MP_INFO(x,y).flags & FLAG_FIRE_COVER) != 0)
    Fgl_write (mps->x + 8 * 8, mps->y + 16, "YES");
  else
    Fgl_write (mps->x + 8 * 8, mps->y + 16, "NO ");
  if ((MP_INFO(x,y).flags & FLAG_HEALTH_COVER) != 0)
    Fgl_write (mps->x + 8 * 8, mps->y + 32, "YES");
  else
    Fgl_write (mps->x + 8 * 8, mps->y + 32, "NO ");
  if ((MP_INFO(x,y).flags & FLAG_CRICKET_COVER) != 0)
    Fgl_write (mps->x + 8 * 8, mps->y + 48, "YES");
  else
    Fgl_write (mps->x + 8 * 8, mps->y + 48, "NO ");
  sprintf (s, "%5d ", MP_POL(x,y));
  if (MP_POL(x,y) < 10)
    strcat (s, "(clear) ");
  else if (MP_POL(x,y) < 25)
    strcat (s, "(good)  ");
  else if (MP_POL(x,y) < 70)
    strcat (s, "(fair)  ");
  else if (MP_POL(x,y) < 190)
    strcat (s, "(smelly)");
  else if (MP_POL(x,y) < 450)
    strcat (s, "(smokey)");
  else if (MP_POL(x,y) < 1000)
    strcat (s, "(smoggy)");
  else if (MP_POL(x,y) < 1700)
    strcat (s, "(bad)   ");
  else if (MP_POL(x,y) < 3000)
    strcat (s, "(v bad) ");
  else
    strcat (s, "(death!)");
  Fgl_write (mps->x + 8, mps->y + 64, s);

  g = MP_GROUP(x,y);
  if (g == 0)
    {
      sprintf (s, "  (N/A)");	/* Can't bulldoze grass. */
    }
  else
    {
      if (g < 7)
	g--;			/* translate into button type */
      sprintf (s, "%7d", main_groups[g].bul_cost);
    }
  Fgl_write (mps->x + 48, mps->y + 76, s);
}

void
mps_firestation_setup (void)
{
  Rect* mps = &scr.mappoint_stats;
  Fgl_write (mps->x, mps->y + 40, "Jobs");
  Fgl_write (mps->x, mps->y + 48, "Goods");
}

void
mps_firestation (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  char s[100];
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_1 * 100
	   / MAX_JOBS_AT_FIRESTATION);
  Fgl_write (mps->x + 8 * 8, mps->y + 40, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_2 * 100
	   / MAX_GOODS_AT_FIRESTATION);
  Fgl_write (mps->x + 8 * 8, mps->y + 48, s);

}

void
mps_cricket_setup (void)
{
  Rect* mps = &scr.mappoint_stats;
  Fgl_write (mps->x, mps->y + 40, "Jobs");
  Fgl_write (mps->x, mps->y + 48, "Goods");
}

void
mps_cricket (int x, int y)
{
  Rect* mps = &scr.mappoint_stats;
  char s[100];
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_1 * 100
	   / MAX_JOBS_AT_CRICKET);
  Fgl_write (mps->x + 8 * 8, mps->y + 40, s);
  sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_2 * 100
	   / MAX_GOODS_AT_CRICKET);
  Fgl_write (mps->x + 8 * 8, mps->y + 48, s);

}

void
mps_health_setup (void)
{
  Rect* mps = &scr.mappoint_stats;
  Fgl_write (mps->x, mps->y + 40, "Jobs");
  Fgl_write (mps->x, mps->y + 48, "Goods");
}

void
mps_health (int x, int y)
{
    Rect* mps = &scr.mappoint_stats;
    char s[100];
    sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_1 * 100
	     / MAX_JOBS_AT_HEALTH_CENTRE);
    Fgl_write (mps->x + 8 * 8, mps->y + 40, s);
    sprintf (s, "%5.1f%%", (float) MP_INFO(x,y).int_2 * 100
	     / MAX_GOODS_AT_HEALTH_CENTRE);
    Fgl_write (mps->x + 8 * 8, mps->y + 48, s);

}

void 
mps_global_finance_setup (void)
{
    Rect* mps = &scr.mappoint_stats;
    Fgl_write (mps->x + 32, mps->y, "FINANCE");

    Fgl_write (mps->x, mps->y + 8, "Income   Expend");
    Fgl_write (mps->x, mps->y + 16, "IT");
    Fgl_write (mps->x, mps->y + 24, "CT");
    Fgl_write (mps->x, mps->y + 32, "GT");
    Fgl_write (mps->x, mps->y + 40, "XP");
    Fgl_write (mps->x + 8*8, mps->y + 16, "OC");
    Fgl_write (mps->x + 8*8, mps->y + 24, "UC");
    Fgl_write (mps->x + 8*8, mps->y + 32, "TC");
    Fgl_write (mps->x + 8*8, mps->y + 40, "IP");
    Fgl_write (mps->x, mps->y + 80, "Tot");
}

void 
mps_global_other_costs_setup (void)
{
    Rect* mps = &scr.mappoint_stats;
    Fgl_write (mps->x + 20, mps->y, "OTHER COSTS");

    Fgl_write (mps->x, mps->y + 4*8, "It");
    Fgl_write (mps->x, mps->y + 5*8, "Sc");
    Fgl_write (mps->x, mps->y + 6*8, "Un");
    Fgl_write (mps->x, mps->y + 7*8, "Dt");
    Fgl_write (mps->x, mps->y + 8*8, "Wn");
    Fgl_write (mps->x + 8*8, mps->y + 4*8, "Hl");
    Fgl_write (mps->x + 8*8, mps->y + 5*8, "Rk");
    Fgl_write (mps->x + 8*8, mps->y + 6*8, "Fr");
    Fgl_write (mps->x + 8*8, mps->y + 7*8, "Ck");
    Fgl_write (mps->x + 8*8, mps->y + 8*8, "Rc");
}

void 
mps_global_pop_setup (void)
{
    Rect* mps = &scr.mappoint_stats;
    Fgl_write (mps->x + 32, mps->y + 2, "PEOPLE");
    Fgl_write (mps->x + 4, mps->y + 14, "Pop");
    Fgl_write (mps->x + 4, mps->y + 30, "Unnat death");
}

void 
mps_global_housing_setup (void)
{
    Rect* mps = &scr.mappoint_stats;
    Fgl_write (mps->x + 32, mps->y + 2, "PEOPLE");
    Fgl_write (mps->x + 4, mps->y + 1*8+6, "Pop");
    Fgl_write (mps->x + 4, mps->y + 2*8+6, "Housed");
    Fgl_write (mps->x + 4, mps->y + 3*8+6, "Housed %");
    Fgl_write (mps->x + 4, mps->y + 4*8+6, "Residenc");
    Fgl_write (mps->x + 4, mps->y + 5*8+6, "Shanties");
    Fgl_write (mps->x + 4, mps->y + 6*8+6, "Unn Dths");
    Fgl_write (mps->x + 4, mps->y + 7*8+6, "Unemp %");
    Fgl_write (mps->x + 4, mps->y + 8*8+6, "Starv %");
}

void 
mps_global_tech_setup (void)
{
    Rect* mps = &scr.mappoint_stats;
    Fgl_write (mps->x + 32, mps->y + 2, "TECH");
    Fgl_write (mps->x + 4, mps->y + 14, "Tech lvl");
    Fgl_write (mps->x + 4, mps->y + 22, "Schools");
    Fgl_write (mps->x + 4, mps->y + 30, "Univers");
    Fgl_write (mps->x + 4, mps->y + 38, "Rockets");
}

void 
mps_global_food_setup (void)
{
    Rect* mps = &scr.mappoint_stats;
    Fgl_write (mps->x + 32, mps->y + 2, "FOOD");
    Fgl_write (mps->x + 4, mps->y + 14, "Foodstore");
    Fgl_write (mps->x + 4, mps->y + 22, "Farms");
    Fgl_write (mps->x + 4, mps->y + 30, "Starv %");
}

void 
mps_global_jobs_setup (void)
{
    Rect* mps = &scr.mappoint_stats;
    Fgl_write (mps->x + 32, mps->y + 2, "JOBS");
    Fgl_write (mps->x + 4, mps->y + 14, "Jobs");
    Fgl_write (mps->x + 4, mps->y + 22, "% Unemp");
}

void 
mps_global_setup (int style)
{
    switch (style) {
    case MPS_GLOBAL_FINANCE:
	mps_global_finance_setup ();
	break;
    case MPS_GLOBAL_OTHER_COSTS:
	mps_global_other_costs_setup ();
	break;
    case MPS_GLOBAL_HOUSING:
	mps_global_housing_setup ();
	break;
    }
}

void 
mps_global_housing (void)
{
    int hp = housed_population;
    int tp = housed_population + people_pool;
    int i;
    char s[100];
    int offset = 70;
    Rect* mps = &scr.mappoint_stats;

    sprintf (s, "%6d", tp);
    Fgl_write (mps->x + offset, mps->y + 1*8+6, s);
    sprintf (s, "%6d", hp);
    Fgl_write (mps->x + offset, mps->y + 2*8+6, s);
    if (tp != 0) {
        sprintf (s, " %3d.%1d", (hp * 100) / tp, ((hp * 1000) / tp) % 10);
    } else {
        sprintf (s, " %3d.%1d", 0, 0);
    }
    Fgl_write (mps->x + offset, mps->y + 3*8+6, s);
    sprintf (s, "    ??");
    Fgl_write (mps->x + offset, mps->y + 4*8+6, s);
    sprintf (s, " %5d", numof_shanties);
    Fgl_write (mps->x + offset, mps->y + 5*8+6, s);
    sprintf (s, " %5d", unnat_deaths);
    Fgl_write (mps->x + offset, mps->y + 6*8+6, s);
    i = ((tunemployed_population / NUMOF_DAYS_IN_MONTH) * 1000)
	    / ((tpopulation / NUMOF_DAYS_IN_MONTH) + 1);
    sprintf (s, " %3d.%1d", i / 10, i % 10);
    Fgl_write (mps->x + offset, mps->y + 7*8+6, s);
    i = ((tstarving_population / NUMOF_DAYS_IN_MONTH) * 1000)
	    / ((tpopulation / NUMOF_DAYS_IN_MONTH) + 1);
    sprintf (s, " %3d.%1d", i / 10, i % 10);
    Fgl_write (mps->x + offset, mps->y + 8*8+6, s);
}

void 
mps_global_finance (void)
{
    char s[100];
    Rect* mps = &scr.mappoint_stats;

    format_pos_number4 (s, ly_income_tax);
    Fgl_write (mps->x + 3*8, mps->y + 16, s);
    format_pos_number4 (s, ly_coal_tax);
    Fgl_write (mps->x + 3*8, mps->y + 24, s);
    format_pos_number4 (s, ly_goods_tax);
    Fgl_write (mps->x + 3*8, mps->y + 32, s);
    format_pos_number4 (s, ly_export_tax);
    Fgl_write (mps->x + 3*8, mps->y + 40, s);
    format_pos_number4 (s, ly_other_cost);
    Fgl_write (mps->x + 11*8, mps->y + 16, s);
    format_pos_number4 (s, ly_unemployment_cost);
    Fgl_write (mps->x + 11*8, mps->y + 24, s);
    format_pos_number4 (s, ly_transport_cost);
    Fgl_write (mps->x + 11*8, mps->y + 32, s);
    format_pos_number4 (s, ly_import_cost);
    Fgl_write (mps->x + 11*8, mps->y + 40, s);

    format_money (s);
    if (total_money < 0)
	Fgl_setfontcolors (14, red (30));
    Fgl_write (mps->x + 3 * 8, mps->y + 80, s);
    if (total_money < 0)
	Fgl_setfontcolors (14, TEXT_FG_COLOUR);
}


void
mps_global_other_costs (void)
{
    char s[100];
    int yr;
    Rect* mps = &scr.mappoint_stats;

    /* Don't write year if its negative. */
    yr = (total_time / NUMOF_DAYS_IN_YEAR) - 1;
    if (yr >= 0) {
	sprintf (s, "For year %04d", yr);
	Fgl_write (mps->x + 12, mps->y + 8, s);
    }
    format_pos_number4 (s, ly_interest);
    Fgl_write (mps->x + 3 * 8, mps->y + 4*8, s);
    format_pos_number4 (s, ly_school_cost);
    Fgl_write (mps->x + 3 * 8, mps->y + 5*8, s);
    format_pos_number4 (s, ly_university_cost);
    Fgl_write (mps->x + 3 * 8, mps->y + 6*8, s);
    format_pos_number4 (s, ly_deaths_cost);
    Fgl_write (mps->x + 3 * 8, mps->y + 7*8, s);
    format_pos_number4 (s, ly_windmill_cost);
    Fgl_write (mps->x + 3 * 8, mps->y + 8*8, s);
    format_pos_number4 (s, ly_health_cost);
    Fgl_write (mps->x + 11 * 8, mps->y + 4*8, s);
    format_pos_number4 (s, ly_rocket_pad_cost);
    Fgl_write (mps->x + 11 * 8, mps->y + 5*8, s);
    format_pos_number4 (s, ly_fire_cost);
    Fgl_write (mps->x + 11 * 8, mps->y + 6*8, s);
    format_pos_number4 (s, ly_cricket_cost);
    Fgl_write (mps->x + 11 * 8, mps->y + 7*8, s);
    format_pos_number4 (s, ly_recycle_cost);
    Fgl_write (mps->x + 11 * 8, mps->y + 8*8, s);
}

void 
mps_global (int style)
{
    switch (style) {
    case MPS_GLOBAL_FINANCE:
	mps_global_finance ();
	break;
    case MPS_GLOBAL_OTHER_COSTS:
	mps_global_other_costs ();
	break;
    case MPS_GLOBAL_HOUSING:
	mps_global_housing ();
	break;
    }
}
