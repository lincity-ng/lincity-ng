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

/* mps_init(): Initialize mps vars and mouse handles */

void
mps_init() 
{
    mps_style = MPS_GLOBAL;
    mps_global_style = MPS_GLOBAL_FINANCE;

    mps_x = 0;
    mps_y = 0;

    mps_handle = mouse_register(&scr.mappoint_stats,&mps_handler);
}


/* mps_set(): Main entry to mps system.  Sets mps to display status
   for a square or global information.  If we are setting mps to
   the same square it was set for, return 1, otherwise 0.
*/

int
mps_set(int x, int y, int style) 
{
    int same_square = 0;
    mps_style = style;
    switch(style) {
    case MPS_MAP:
    case MPS_ENV: 
	if (mps_x == x && mps_y == y) {
	    same_square = 1;
	}
	mps_x = x;
	mps_y = y;
	break;
    default:
	mps_x = 0;
	mps_y = 0;
    }

    mps_update();
    mps_refresh();
    return same_square;
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

    Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);


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
	    case (GROUP_BLACKSMITH):
	        mps_blacksmith (mps_x, mps_y);
		break;
	    case (GROUP_COALMINE):
		mps_coalmine (mps_x, mps_y);
		break;
	    case GROUP_COAL_POWER:
		mps_coal_power (mps_x, mps_y);
		break;
	    case (GROUP_COMMUNE):
	        mps_commune (mps_x, mps_y);
		break;
	    case (GROUP_CRICKET):
		mps_cricket (mps_x, mps_y);
		break;
	    case (GROUP_FIRESTATION):
	        mps_firestation (mps_x, mps_y);
		break;
	    case (GROUP_HEALTH):
		mps_health_centre (mps_x, mps_y);
		break;
	    case (GROUP_INDUSTRY_H):
		mps_heavy_industry (mps_x, mps_y);
		break;
	    case (GROUP_INDUSTRY_L):
		mps_light_industry (mps_x, mps_y);
		break;
	    case (GROUP_MILL):
	        mps_mill (mps_x, mps_y);
		break;
	    case (GROUP_MONUMENT):
	        mps_monument (mps_x, mps_y);
		break;
	    case (GROUP_OREMINE):
	        mps_oremine (mps_x, mps_y);
		break;
	    case GROUP_ORGANIC_FARM: 
		mps_organic_farm(mps_x, mps_y);
		break;
	    case (GROUP_PORT):
	        mps_port (mps_x, mps_y);
		break;
	    case (GROUP_POTTERY):
	        mps_pottery (mps_x, mps_y);
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
	    case (GROUP_SCHOOL):
	        mps_school (mps_x, mps_y);
		break;
	    case GROUP_SOLAR_POWER:
	        mps_solar_power (mps_x, mps_y);
	        break;
	    case (GROUP_SUBSTATION):
	        mps_substation (mps_x, mps_y);
	        break;
	    case (GROUP_TIP):
	        mps_tip (mps_x, mps_y);
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
	    case (GROUP_WATER):
	        mps_water (mps_x, mps_y);
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
	mps_right (mps_x, mps_y);
	break;
    case MPS_GLOBAL: 
	{
	    switch (mps_global_style) {
	    case MPS_GLOBAL_FINANCE:
		mps_global_finance();
		break;
	    case MPS_GLOBAL_OTHER_COSTS:
		mps_global_other_costs();
		break;
	    case MPS_GLOBAL_HOUSING:
		mps_global_housing();
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
   These handle the tedium of formatting
   strings for mps display.  Single argument forms center that
   argument.  Double arg forms offset them.  Triple arg forms divide
   the space into three columns.

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

void
mps_store_f(int i, double f)
{
  int c;
  int l;
  char s[12];
  
  snprintf(s, sizeof(s), "%.1f",f);
  l = strlen(s);
  c = (int)((MPS_INFO_CHARS - l) / 2) + l;
  snprintf(mps_info[i],MPS_INFO_CHARS,"%*s", c, s);
}

void
mps_store_d(int i, int d)
{
  int c;
  int l;
  char s[12];
  
  snprintf(s, sizeof(s), "%d",d);
  l = strlen(s);
  c = (int)((MPS_INFO_CHARS - l) / 2) + l;
  snprintf(mps_info[i],MPS_INFO_CHARS,"%*s", c, s);
}

void
mps_store_ss(int i, char * s1, char * s2)
{
    int l;
    l = snprintf(mps_info[i], MPS_INFO_CHARS, "%s", s1);
    snprintf(&mps_info[i][l], MPS_INFO_CHARS - l, "%*s", 
	     (MPS_INFO_CHARS - l - 1), s2);
}

void
mps_store_sss(int i, char * s1, char * s2, char * s3)
{

    int l, e;  /* Length and End of the strings */
    int c = (MPS_INFO_CHARS) / 3;
    int m = (MPS_INFO_CHARS) % 3;

    if (i > MAPPOINT_STATS_LINES) {
	return;
    }

    l = snprintf(mps_info[i], c + m, "%s", s1);
    e = l;
    l = snprintf(&mps_info[i][e], (c * 2) + m - e, "%*s", 
		 (c * 2) + m - e - 1, s2);
    e += l;
    snprintf(&mps_info[i][e],  (c * 3) + m - e, "%*s", 
	     (c * 3) + m - e - 1, s3);
}

void
mps_store_sd(int i, char * s, int d)
{
    int l;

    if (i > MAPPOINT_STATS_LINES) {
	return;
    }

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

void
mps_right (int x, int y)
{
    int i = 0;
    char s[12];
    char * p;
    int g;

    snprintf(s,sizeof(s),"%d,%d",x,y);
    mps_store_title(i++,s);
    i++;
    mps_store_title(i++,_("Coverage"));
    p = (MP_INFO(x,y).flags & FLAG_FIRE_COVER) ? _("Yes") : _("No");
    mps_store_ss(i++,_("Fire"),p);

    p = (MP_INFO(x,y).flags & FLAG_HEALTH_COVER) ? _("Yes") : _("No");
    mps_store_ss(i++,_("Health"),p);

    p = (MP_INFO(x,y).flags & FLAG_CRICKET_COVER) ? _("Yes") : _("No");
    mps_store_ss(i++,_("Cricket"),p);
    i++;
    mps_store_title(i++,_("Pollution"));

    if (MP_POL(x,y) < 10)
	p = _("clear");
    else if (MP_POL(x,y) < 25)
	p = _("good");
    else if (MP_POL(x,y) < 70)
	p = _("fair");
    else if (MP_POL(x,y) < 190)
	p = _("smelly");
    else if (MP_POL(x,y) < 450)
	p = _("smokey");
    else if (MP_POL(x,y) < 1000)
	p = _("smoggy");
    else if (MP_POL(x,y) < 1700)
	p = _("bad");
    else if (MP_POL(x,y) < 3000)
	p = _("very bad");
    else
	p = _("death!");

    mps_store_sd(i++,p,MP_POL(x,y));
    i++;

    mps_store_title(i++,_("Bulldoze Cost"));
    g = MP_GROUP(x,y);
    if (g == 0) {	/* Can't bulldoze grass. */
	mps_store_title(i++,_("N/A"));
    } else {
	if (g < 7)
	    g--;			/* translate into button type */
	mps_store_d(i++,main_groups[g].bul_cost);
    }
}



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

void 
mps_global_other_costs (void)
{
    int i = 0;
    int year;
    char s[12];

    mps_store_title(i++,_("Other Costs"));

    /* Don't write year if it's negative. */
    year = (total_time / NUMOF_DAYS_IN_YEAR) - 1;
    if (year >= 0) {
	mps_store_sd(i++, _("For year"), year);
    }
    i++;
    num_to_ansi(s,sizeof(s),ly_interest);
    mps_store_ss(i++,_("Interest"),s);
    num_to_ansi(s,sizeof(s),ly_school_cost);
    mps_store_ss(i++,_("Schools"),s);
    num_to_ansi(s,sizeof(s),ly_university_cost);
    mps_store_ss(i++,_("Univers."),s);
    num_to_ansi(s,sizeof(s),ly_deaths_cost);
    mps_store_ss(i++,_("Deaths"),s);
    num_to_ansi(s,sizeof(s),ly_windmill_cost);
    mps_store_ss(i++,_("Windmill"),s);
    num_to_ansi(s,sizeof(s),ly_health_cost);
    mps_store_ss(i++,_("Hospital"),s);
    num_to_ansi(s,sizeof(s),ly_rocket_pad_cost);
    mps_store_ss(i++,_("Rockets"),s);
    num_to_ansi(s,sizeof(s),ly_fire_cost);
    mps_store_ss(i++,_("Fire Stn"),s);
    num_to_ansi(s,sizeof(s),ly_cricket_cost);
    mps_store_ss(i++,_("Cricket"),s);
    num_to_ansi(s,sizeof(s),ly_recycle_cost);
    mps_store_ss(i++,_("Recycle"),s);
}


void 
mps_global_housing (void)
{
    int i = 0;
    int tp = housed_population + people_pool;

    mps_store_title(i++,_("Population"));
    i++;
    mps_store_sd(i++,_("Total"),tp);
    mps_store_sd(i++,_("Housed"),housed_population);
    mps_store_sd(i++,_("Homeless"),people_pool);
    mps_store_sd(i++,_("Shanties"),numof_shanties);
    mps_store_sd(i++,_("Unn Dths"),unnat_deaths);
    mps_store_title(i++,_("Unemployment"));
    mps_store_sd(i++,_("Claims"),tunemployed_population);
    mps_store_sfp(i++,_("Rate"),
		  ((tunemployed_population * 100.0) / tp));
    mps_store_title(i++,_("Starvation"));
    mps_store_sd(i++,_("Cases"),tstarving_population);

    mps_store_sfp(i++,_("Rate"),
		  ((tstarving_population * 100.0) / tp));
}


#ifdef old_mps

void 
mps_global_pop_setup (void)
{
    Rect* mps = &scr.mappoint_stats;
    Fgl_write (mps->x + 32, mps->y + 2, _("PEOPLE"));
    Fgl_write (mps->x + 4, mps->y + 14, _("Pop"));
    Fgl_write (mps->x + 4, mps->y + 30, _("Unnat death"));
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


#endif




