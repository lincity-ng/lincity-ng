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
#include "mouse.h"
#include "screen.h"
#include "power.h"
#include "engine.h"
#include "lclib.h"
#include "lcintl.h"
#include "stats.h"
#include <modules.h>
#include <mouse.h>
#include <lclib.h>

/* ---------------------------------------------------------------------- *
 * Private Fn Prototypes
 * ---------------------------------------------------------------------- */
void mps_global_setup (int);
void mps_global (int);
void mps_setup(int x, int y);

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

char mps_info[MAPPOINT_STATS_LINES][MPS_INFO_CHARS];
int mps_global_style;

static int mps_style;

static int mps_x;
static int mps_y;

static Mouse_Handle * mps_handle;


/*
 * ----------------------------------------------------------------------
 * * New, simplified mps routines.  All drawing is offloaded to
 * mps_refresh and mps_redraw, with the various mps_module routines -
 * called from mps_update - merely updating an array of strings:
 * mps_info (see above)
 * ----------------------------------------------------------------------
 * */

void
mps_handler(int x, int y, int button)
{
    if (button == LC_MOUSE_LEFTBUTTON) {
	if (mps_style == MPS_GLOBAL) {
	    mps_global_advance();
	} else {
	    mps_set(0,0,MPS_GLOBAL);
	}
    } else if (button == LC_MOUSE_RIGHTBUTTON) {
	/* XXX: Pop help here, depending on selected style */
    }
}

/* mps_init: register mouse handles */

void
mps_init() 
{
    mps_handle = mouse_register(&scr.mappoint_stats,&mps_handler);
}



void
mps_set(int x, int y, int style) {
    mps_style = style;
    switch(style) {
    case MPS_MAP:
    case MPS_ENV: 
	mps_x = x;
	mps_y = y;
	break;
    default:
	mps_x = 0;
	mps_y = 0;
    }

    mps_update();
    mps_refresh();
}


void
mps_redraw(void)
{
    Rect* mps = &scr.mappoint_stats;

    draw_small_bezel (mps->x, mps->y, mps->w, mps->h, yellow(0));
    mps_refresh();
}

void
mps_refresh(void)
{
    int i;

    Rect * mps = &scr.mappoint_stats;

    Fgl_fillbox (mps->x, mps->y, mps->w + 1, mps->h + 1, 14);
    Fgl_setfontcolors (14, TEXT_FG_COLOUR);

    for (i = 0; i < MAPPOINT_STATS_LINES; i++) {
	Fgl_write (mps->x + 4, mps->y + (i * 8) + 4, mps_info[i]);
    }
}

void
mps_update(void)
{
    int i;
    
    for (i = 0; i < MAPPOINT_STATS_LINES; i++) {
	strcpy(mps_info[i],"");
    }

    switch (mps_style) {
    case MPS_MAP:
	{
	    switch(MP_GROUP(mps_x, mps_y)) {
	    case (GROUP_COALMINE):
		mps_coalmine (mps_x, mps_y);
		break;
	    case GROUP_COAL_POWER:
		mps_coal_power (mps_x, mps_y);
		break;
	    case (GROUP_INDUSTRY_H):
		mps_heavy_industry (mps_x, mps_y);
		break;
	    case (GROUP_INDUSTRY_L):
		mps_light_industry (mps_x, mps_y);
		break;
	    case (GROUP_OREMINE):
	        mps_oremine (mps_x, mps_y);
		break;
	    case GROUP_ORGANIC_FARM: 
		mps_organic_farm(mps_x, mps_y);
		break;
	    case GROUP_POWER_LINE:
	        mps_power_line (mps_x, mps_y);
	        break;
	    case (GROUP_RAIL):
		mps_rail (mps_x, mps_y);
		break;
	    case (GROUP_RECYCLE):
	        mps_recycle (mps_x, mps_y);
		break;
	    case GROUP_RESIDENCE_LL:
	    case GROUP_RESIDENCE_ML:
	    case GROUP_RESIDENCE_HL:
	    case GROUP_RESIDENCE_LH:
	    case GROUP_RESIDENCE_MH:
	    case GROUP_RESIDENCE_HH:
		mps_residence(mps_x, mps_y);
		break;
	    case (GROUP_ROAD):
		mps_road (mps_x, mps_y);
		break;
	    case (GROUP_ROCKET):
	        mps_rocket (mps_x, mps_y);
		break;
	    case GROUP_SOLAR_POWER:
	        mps_solar_power (mps_x, mps_y);
	        break;
	    case (GROUP_SUBSTATION):
	        mps_substation (mps_x, mps_y);
	        break;
	    case (GROUP_TRACK):
		mps_track(mps_x, mps_y);
		break;
	    case (GROUP_MARKET):
		mps_market (mps_x, mps_y);
		break;
	    case (GROUP_UNIVERSITY):
	        mps_university (mps_x, mps_y);
	        break;
	    case (GROUP_WINDMILL):
	        mps_windmill (mps_x, mps_y);
		break;
	    default: 
		printf("MPS unimplemented for that module\n");
	    }
	}
        break;
    case MPS_ENV:
	printf("MPS unimplemented for right clicks\n");
	break;
    case MPS_GLOBAL: 
	{
	    switch (mps_global_style) {
	    case MPS_GLOBAL_FINANCE:
		mps_global_finance();
		break;
	    default:
		printf("MPS unimplemented for global display\n");
		break;
	    }
	}
	break;
    }

    mps_refresh();
}

/* Cycle through the various global styles, but only update and display
   if global info display is active */

void
mps_global_advance(void)
{
    mps_global_style++;
    mps_global_style %= MPS_GLOBAL_STYLES;

    if (mps_style == MPS_GLOBAL) {
      mps_update();
    }
}

/* MPS String storage routines.
   These handle the tedium of formatting strings for mps display.
   store_title centers its single argument, while the others 
   left justify the first arg and right justify the second.
*/

void
mps_store_title(int i, char * t)
{
  int c;
  int l;

  l = strlen(t);
  c = (int)((MPS_INFO_CHARS - l) / 2) + l;
  snprintf(mps_info[i],MPS_INFO_CHARS,"%*s", c, t);
}

void
mps_store_fp(int i, double f)
{
  int c;
  int l;
  char s[12];
  
  snprintf(s, sizeof(s), "%.1f%%",f);
  l = strlen(s);
  c = (int)((MPS_INFO_CHARS - l) / 2) + l;
  snprintf(mps_info[i],MPS_INFO_CHARS,"%*s", c, s);
}

/* mps_store_??: Store two items, with the second one right justified.
   By writing the second string first and removing the null after
   the first string, we can ensure proper layout even after i18n.
   There may be a better way to do this...
*/

void
mps_store_ss(int i, char * s1, char * s2)
{
    int l;
    l = snprintf(mps_info[i], MPS_INFO_CHARS, "%s", s1);
    snprintf(&mps_info[i][l], MPS_INFO_CHARS - l, "%*s", 
	     (MPS_INFO_CHARS - l - 1), s2);
}

void
mps_store_sd(int i, char * s, int d)
{
    int l;
    l = snprintf(mps_info[i], MPS_INFO_CHARS, "%s", s);
    snprintf(&mps_info[i][l], MPS_INFO_CHARS, "%*d", 
	     (MPS_INFO_CHARS - 1 - l), d);
}

void
mps_store_sfp(int i, char * s, double fl)
{
    int l;
    l = snprintf(mps_info[i], MPS_INFO_CHARS, "%s", s); 
    snprintf(&mps_info[i][l], MPS_INFO_CHARS, "%*.1f%%",
	     MPS_INFO_CHARS - 2 - l, fl);
}

/* MPS Global routines */

void mps_global_finance(void) {
    int i = 0;
    char s[12];

    int cashflow = 0;

    mps_store_title(i++,_("Tax Income"));

    cashflow += ly_income_tax;
    num_to_ansi (s, 12, ly_income_tax);
    mps_store_ss(i++,_("Income"), s);

    cashflow += ly_coal_tax;
    num_to_ansi(s, 12, ly_coal_tax);
    mps_store_ss(i++,_("Coal"), s);

    cashflow += ly_goods_tax;
    num_to_ansi(s, 12, ly_goods_tax);
    mps_store_ss(i++,_("Goods"), s);

    cashflow += ly_export_tax;
    num_to_ansi(s, 12, ly_export_tax);
    mps_store_ss(i++,_("Export"), s);

    i++;

    mps_store_title(i++,_("Expenses"));

    cashflow -= ly_unemployment_cost;
    num_to_ansi(s, 12, ly_unemployment_cost);
    mps_store_ss(i++,_("Unemp."), s);

    cashflow -= ly_transport_cost;
    num_to_ansi(s, 12, ly_transport_cost);
    mps_store_ss(i++,_("Transport"), s);

    cashflow -= ly_import_cost;
    num_to_ansi(s, 12, ly_import_cost);
    mps_store_ss(i++,_("Imports"), s);

    cashflow -= ly_other_cost;
    num_to_ansi(s, 12, ly_other_cost);
    mps_store_ss(i++,_("Others"), s);

    i++;

    num_to_ansi(s, 12, cashflow);
    mps_store_ss(i++,_("Net"), s);
}    


#ifdef old_mps
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

    /* mappoint_stats (-3,-3,-3) means continue using old values 
       for the map coordinates and button, but do a full refresh 
       of setup strings */
    /* mappoint_stats (-2,-2,b) means mps should display "global 
       statistics" rather than "map-point statistics".
       The "b" parameter identifies which kind of global statistics 
       should be displayed (e.g. finance, population, etc.) */
    /* mappoint_stats (-1,-1,-1) means continue using old values */
    if (x == -3) {
	Fgl_fillbox (mps->x, mps->y,
		     mps->w + 1, mps->h + 1, 14);
	Fgl_setfontcolors (14, TEXT_FG_COLOUR);
	
	if (xx == -2)
	    mps_global_setup (oldbut);
	else {
	    /* this should be part of *_setup, but is needed by
	       mps_right too */
	    strcpy (s, _(main_groups[MP_GROUP(xx,yy)].name));
	    Fgl_write (mps->x + (14 - strlen (s)) * 4,
		       mps->y, s);
	    
	    mps_setup (xx,yy);
	}

	button = oldbut;
	x = xx;
	y = yy;
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
	xx = x;
	yy = y;
	oldbut = button;
	mappoint_stats_flag = 1;
	/* draw centre of box */
	Fgl_fillbox (mps->x, mps->y,
		     mps->w + 1, mps->h + 1, 14);
	/* write static stuff */

	Fgl_setfontcolors (14, TEXT_FG_COLOUR);

	strcpy (s, _(main_groups[MP_GROUP(x,y)].name));
	Fgl_write (mps->x + (14 - strlen (s)) * 4,
		   mps->y, s);

	if (button == LC_MOUSE_RIGHTBUTTON)
	    mps_right_setup ();
	else
	    mps_setup(x,y);
    }

    Fgl_setfontcolors (14, TEXT_FG_COLOUR);

    if (x == -2 || x == -3) {
	mps_global (button);
    } else if (button == LC_MOUSE_RIGHTBUTTON) {
	mps_right (x, y);
    } else {
	switch (MP_GROUP(x,y))
	{

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

    Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);

}

void 
mps_setup (int x, int y)
{
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


void
mps_monument_setup (void)
{
    Rect* mps = &scr.mappoint_stats;
    Fgl_write (mps->x, mps->y + 40, _("Built"));
    Fgl_write (mps->x, mps->y + 48, _("T made"));
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
    Fgl_write (mps->x, mps->y + 40, _("Jobs"));
    Fgl_write (mps->x, mps->y + 48, _("Goods"));
    Fgl_write (mps->x, mps->y + 56, _("T made"));
    Fgl_write (mps->x, mps->y + 64, _("Capacity"));
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
    Fgl_write (mps->x, mps->y + 40, _("G store"));
    Fgl_write (mps->x, mps->y + 48, _("C store"));
    Fgl_write (mps->x, mps->y + 56, _("Capacity"));
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
    Fgl_write (mps->x, mps->y + 40, _("G store"));
    Fgl_write (mps->x, mps->y + 48, _("F store"));
    Fgl_write (mps->x, mps->y + 56, _("C store"));
    Fgl_write (mps->x, mps->y + 64, _("Capacity"));
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
    Fgl_write (mps->x, mps->y + 40, _("G store"));
    Fgl_write (mps->x, mps->y + 48, _("O store"));
    Fgl_write (mps->x, mps->y + 56, _("C store"));
    Fgl_write (mps->x, mps->y + 64, _("Capacity"));
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
	Fgl_write (mps->x + 1 * 8, mps->y + 40,
		   _("  CONNECTED  "));
    else
	Fgl_write (mps->x + 1 * 8, mps->y + 40,
		   _("NOT CONNECTED"));
    Fgl_write (mps->x, mps->y + 48, _("to river sytem"));
}

void
mps_port_setup (int x, int y)
{
    Rect* mps = &scr.mappoint_stats;
    /* TRANSLATORS F=food, C=coal, O=ore, G=goods, S=steel */
    Fgl_write (mps->x + 4, mps->y + 1*8+4, _("     Buy   Sell"));
    Fgl_write (mps->x + 4, mps->y + 3*8, _("F"));
    Fgl_write (mps->x + 4, mps->y + 4*8, _("C"));
    Fgl_write (mps->x + 4, mps->y + 5*8, _("O"));
    Fgl_write (mps->x + 4, mps->y + 6*8, _("G"));
    Fgl_write (mps->x + 4, mps->y + 7*8, _("S"));
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
    Fgl_write (mps->x, mps->y + 32, _("  Waste stored"));
    Fgl_write (mps->x, mps->y + 40, _("   last month"));
    Fgl_write (mps->x, mps->y + 64, _("       % full"));
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
    Fgl_write (mps->x, mps->y + 12, _("   Activity"));
    Fgl_write (mps->x, mps->y + 20, _("  last month"));
    Fgl_write (mps->x, mps->y + 36, _("  Coal"));
    Fgl_write (mps->x, mps->y + 44, _("   Ore"));
    Fgl_write (mps->x, mps->y + 52, _(" Steel"));
    Fgl_write (mps->x, mps->y + 60, _(" Waste"));
}

void
mps_commune (int x, int y)
{
    Rect* mps = &scr.mappoint_stats;
    if ((MP_INFO(x,y).int_5 & 1) != 0)
	Fgl_write (mps->x + 7 * 8, mps->y + 36, _("YES"));
    else
	Fgl_write (mps->x + 7 * 8, mps->y + 36, _("NO "));
    if ((MP_INFO(x,y).int_5 & 2) != 0)
	Fgl_write (mps->x + 7 * 8, mps->y + 44, _("YES"));
    else
	Fgl_write (mps->x + 7 * 8, mps->y + 44, _("NO "));
    if ((MP_INFO(x,y).int_5 & 4) != 0)
	Fgl_write (mps->x + 7 * 8, mps->y + 52, _("YES"));
    else
	Fgl_write (mps->x + 7 * 8, mps->y + 52, _("NO "));
    if ((MP_INFO(x,y).int_5 & 8) != 0)
	Fgl_write (mps->x + 7 * 8, mps->y + 60, _("YES"));
    else
	Fgl_write (mps->x + 7 * 8, mps->y + 60, _("NO "));
}


void
mps_right_setup (void)
{
    Rect* mps = &scr.mappoint_stats;
    Fgl_write (mps->x + 16, mps->y + 8,  _("Grid:"));
    Fgl_write (mps->x + 16, mps->y + 16, _("Coverages:"));
    Fgl_write (mps->x + 16, mps->y + 24, _("Fire     "));
    Fgl_write (mps->x + 16, mps->y + 32, _("Health   "));
    Fgl_write (mps->x + 16, mps->y + 40, _("Cricket  "));
    Fgl_write (mps->x + 16, mps->y + 48, _("Pollution"));
    Fgl_write (mps->x + 16, mps->y + 76, _(" Bull"));
}

void
mps_right (int x, int y)
{
    Rect* mps = &scr.mappoint_stats;
    char s[100];
    int g;
    snprintf(s,100,"%d,%d",x,y);
    Fgl_write (mps->x + 8 * 8, mps->y + 8, s);
    Fgl_write (mps->x + 8 * 12, mps->y + 24, 
	       (MP_INFO(x,y).flags & FLAG_FIRE_COVER) ? _("YES") : _("NO "));
    Fgl_write (mps->x + 8 * 12, mps->y + 32, 
	       (MP_INFO(x,y).flags & FLAG_HEALTH_COVER) ? _("YES") : _("NO "));
    Fgl_write (mps->x + 8 * 12, mps->y + 40, 
	       (MP_INFO(x,y).flags & FLAG_CRICKET_COVER) ? _("YES") : _("NO "));

    sprintf (s, "%5d ", MP_POL(x,y));
    if (MP_POL(x,y) < 10)
	strcat (s, _("(clear) "));
    else if (MP_POL(x,y) < 25)
	strcat (s, _("(good)  "));
    else if (MP_POL(x,y) < 70)
	strcat (s, _("(fair)  "));
    else if (MP_POL(x,y) < 190)
	strcat (s, _("(smelly)"));
    else if (MP_POL(x,y) < 450)
	strcat (s, _("(smokey)"));
    else if (MP_POL(x,y) < 1000)
	strcat (s, _("(smoggy)"));
    else if (MP_POL(x,y) < 1700)
	strcat (s, _("(bad)   "));
    else if (MP_POL(x,y) < 3000)
	strcat (s, _("(v bad) "));
    else
	strcat (s, _("(death!)"));
    Fgl_write (mps->x + 8, mps->y + 56, s);

    g = MP_GROUP(x,y);
    if (g == 0) {
	sprintf (s, _("  (N/A)"));	/* Can't bulldoze grass. */
    } else {
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
    Fgl_write (mps->x, mps->y + 40, _("Jobs"));
    Fgl_write (mps->x, mps->y + 48, _("Goods"));
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
    Fgl_write (mps->x, mps->y + 40, _("Jobs"));
    Fgl_write (mps->x, mps->y + 48, _("Goods"));
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
    Fgl_write (mps->x, mps->y + 40, _("Jobs"));
    Fgl_write (mps->x, mps->y + 48, _("Goods"));
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
mps_global_other_costs_setup (void)
{
    Rect* mps = &scr.mappoint_stats;
    Fgl_write (mps->x + 20, mps->y, _("OTHER COSTS"));

    /* TRANSLATORS: 
       It=Interest costs.
       Sc=School costs.
       Un=University costs.
       Dt=Unnatural deaths costs.
       Wn=Windmill costs.
       Hl=Health costs.
       Rk=Rocket costs.
       Fr=Fire station costs.
       Ck=Cricket costs.
       Rc=Recycling costs.
    */
    Fgl_write (mps->x, mps->y + 4*8, _("It"));
    Fgl_write (mps->x, mps->y + 5*8, _("Sc"));
    Fgl_write (mps->x, mps->y + 6*8, _("Un"));
    Fgl_write (mps->x, mps->y + 7*8, _("Dt"));
    Fgl_write (mps->x, mps->y + 8*8, _("Wn"));
    Fgl_write (mps->x + 8*8, mps->y + 4*8, _("Hl"));
    Fgl_write (mps->x + 8*8, mps->y + 5*8, _("Rk"));
    Fgl_write (mps->x + 8*8, mps->y + 6*8, _("Fr"));
    Fgl_write (mps->x + 8*8, mps->y + 7*8, _("Ck"));
    Fgl_write (mps->x + 8*8, mps->y + 8*8, _("Rc"));
}

void 
mps_global_pop_setup (void)
{
    Rect* mps = &scr.mappoint_stats;
    Fgl_write (mps->x + 32, mps->y + 2, _("PEOPLE"));
    Fgl_write (mps->x + 4, mps->y + 14, _("Pop"));
    Fgl_write (mps->x + 4, mps->y + 30, _("Unnat death"));
}

void 
mps_global_housing_setup (void)
{
    Rect* mps = &scr.mappoint_stats;
    Fgl_write (mps->x + 32, mps->y + 2, _("PEOPLE"));
    Fgl_write (mps->x + 4, mps->y + 1*8+6, _("Pop"));
    Fgl_write (mps->x + 4, mps->y + 2*8+6, _("Housed"));
    Fgl_write (mps->x + 4, mps->y + 3*8+6, _("Housed %"));
    Fgl_write (mps->x + 4, mps->y + 4*8+6, _("Shanties"));

    Fgl_write (mps->x + 4, mps->y + 6*8+6, _("Unn Dths"));
    Fgl_write (mps->x + 4, mps->y + 7*8+6, _("Unemp %"));
    Fgl_write (mps->x + 4, mps->y + 8*8+6, _("Starv %"));
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
    sprintf (s, " %5d", numof_shanties);
    Fgl_write (mps->x + offset, mps->y + 4*8+6, s);

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
mps_global_other_costs (void)
{
    char s[100];
    int yr;
    Rect* mps = &scr.mappoint_stats;

    /* Don't write year if its negative. */
    yr = (total_time / NUMOF_DAYS_IN_YEAR) - 1;
    if (yr >= 0) {
	sprintf (s, _("For year %04d"), yr);
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

#endif
