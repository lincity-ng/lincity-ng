/* ---------------------------------------------------------------------- *
 * screen.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include "lcconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "lcstring.h"
#include "common.h"
#include "lctypes.h"
#include "lin-city.h"
#include "screen.h"
#include "engglobs.h"
#include "clistubs.h"
#include "pixmap.h"
#include "lchelp.h"
#include "mouse.h"
#include "mps.h"
#include "lcintl.h"
#include "ldsvgui.h"
#include "pbar.h"
#include "dialbox.h"

/* ---------------------------------------------------------------------- *
 * External Global Variables
 * ---------------------------------------------------------------------- */
extern int network_game;
extern int tfood_in_markets, tjobs_in_markets;
extern int tcoal_in_markets, tgoods_in_markets;
extern int tore_in_markets, tsteel_in_markets;
extern int time_multiplex_stats;

/* ---------------------------------------------------------------------- *
 * Public Global Variables
 * ---------------------------------------------------------------------- */
unsigned char main_font[2048];
unsigned char start_font1[2048];
unsigned char start_font2[4096];
unsigned char start_font3[4096];
Update_Scoreboard update_scoreboard;

int monthgraph_style = MONTHGRAPH_STYLE_MIN;
int mps_global_style = MPS_GLOBAL_STYLE_MIN;

char screen_refreshing = 0; 

/* ---------------------------------------------------------------------- *
 * Private Global Variables
 * ---------------------------------------------------------------------- */
static int monthgraph_style_timeout = 1;
static int mps_global_style_timeout = 1;
static float time_for_year;

/* ---------------------------------------------------------------------- *
 * Private Function Prototypes
 * ---------------------------------------------------------------------- */
void draw_yellow_bezel (int x, int y, int h, int w);
void draw_small_yellow_bezel (int x, int y, int h, int w);
void print_time_for_year (void);
void calculate_time_for_year (void);
void clear_monthgraph (void);
void draw_ms_buttons (void);
void draw_select_buttons (void);
static void do_monthgraph (int full_refresh);
static void do_history_linegraph (int draw);
static void do_sust_barchart (int draw);
static void draw_sustline (int yoffset, int count, int max, int col);
void monthgraph_full_refresh (void);
void draw_mini_pol_in_main_win ();
void mini_full_refresh (void);
void update_main_screen_normal (void);
void update_main_screen_pollution (void);



/* ---------------------------------------------------------------------- *
 * Public Functions
 * ---------------------------------------------------------------------- */
void
draw_background (void)
{
    /* XXX: we don't need to draw the whole background! */
    /* GCS: but this routine is only called on a full refresh, so it's OK */
#if defined (LC_X11) || defined (WIN32)
    /* Draw border region, but don't put into pixmap */
    draw_border ();

    /* Draw main area */
    Fgl_fillbox (0, 0, pixmap_width, pixmap_height, TEXT_BG_COLOUR);
#else /* SVGALIB */
    Fgl_fillbox (0, 0, 640, 480, TEXT_BG_COLOUR);
#endif
}

void
refresh_main_screen ()
{
    Rect* b = &scr.main_win;
    if (!(market_cb_flag || port_cb_flag))
    /* XXX: Don't resize the screen now! */
    {
	connect_transport (main_screen_originx, main_screen_originy,
			   b->w / 16, b->h / 16);
	screen_refresh_flag = 1;
	update_main_screen ();
	update_mini_screen ();
    }
}

void 
clip_main_window ()
{
    Rect* b = &scr.main_win;
    Fgl_enableclipping ();
    Fgl_setclippingwindow (b->x, b->y, b->x + b->w - 1, b->y + b->h - 1);
}

void 
unclip_main_window ()
{
    Fgl_disableclipping ();
}

void
update_main_screen (void)
{
    switch (main_screen_flag)
    {
    case MINI_SCREEN_NORMAL_FLAG:
	update_main_screen_normal ();
	break;
    case MINI_SCREEN_POL_FLAG:
	update_main_screen_pollution ();
	break;
    };

#if defined (WIN32)
    if (screen_refresh_flag) {
	UpdateWindow (display.hWnd);
    }
#else
    if (mouse_type == MOUSE_TYPE_SQUARE)
	redraw_mouse ();
#endif
    screen_refresh_flag = 0;
}

void
update_main_screen_normal (void)
{
    Rect* mw = &scr.main_win;
    int x, y, xm, ym;
    short typ, grp;
#ifdef USE_PIXMAPS
    int sx, sy, dx, dy, x1, y1;
#endif
    /*  main_screen_origin[x|y] contain the mappoint of the top left of win */
#ifdef DEBUG_MAIN_SCREEN
    printf ("Updating main screen\n");
#endif
    if (help_flag || load_flag || save_flag)
	return;

    /* GCS: I moved the code to reset the main_screen_origin to the 
       function load_city(), where I think is more appropriate. */

    xm = main_screen_originx;
    if (xm > 3)
	xm = 3;
    ym = main_screen_originy;
    if (ym > 3)
	ym = 3;
#if !defined (WIN32)		/* For speed */
    if (mouse_type == MOUSE_TYPE_SQUARE)
	hide_mouse ();
#endif
    clip_main_window ();
    for (y = main_screen_originy - ym; y < main_screen_originy
		 + (mw->h / 16); y++)
	for (x = main_screen_originx - xm; x < main_screen_originx
		     + (mw->w / 16); x++)
	{
	    typ = MP_TYPE(x,y);
	    if (typ != mappointoldtype[x][y] || screen_refresh_flag)
	    {
		mappointoldtype[x][y] = typ;
		if (typ == CST_USED) {
		    continue;
		}
		grp = get_group_of_type(typ);
#ifdef USE_PIXMAPS
		if (icon_pixmap[typ] != 0)
		{
		    x1 = y1 = 0;
		    if (x < main_screen_originx)
			x1 = (main_screen_originx - x) * 16;
		    if (y < main_screen_originy)
			y1 = (main_screen_originy - y) * 16;
		    sx = sy = main_groups[grp].size;
		    if ((sx + x) > (main_screen_originx + (mw->w / 16)))
			sx = (main_screen_originx + (mw->w / 16)) - x;
		    if ((sy + y) > (main_screen_originy + (mw->h / 16)))
			sy = (main_screen_originy + (mw->h / 16)) - y;
		    sx = (sx << 4) - x1;
		    sy = (sy << 4) - y1;
		    dx = mw->x + (x - main_screen_originx) * 16 + x1;
		    dy = mw->y + (y - main_screen_originy) * 16 + y1;
		    if (sx > 0 && sy > 0)
		    {
#if defined (LC_X11)
#ifdef ALLOW_PIX_DOUBLING
			if (pix_double)
			    XCopyArea (display.dpy
				       ,icon_pixmap[typ]
				       ,display.win
				       ,display.pixcolour_gc[0]
				       ,x1 * 2, y1 * 2, sx * 2, sy * 2
				       ,dx * 2, dy * 2);
			else
#endif /* ALLOW_PIX_DOUBLING */
			    XCopyArea (display.dpy
				       ,icon_pixmap[typ]
				       ,display.win
				       ,display.pixcolour_gc[0]
				       ,x1, y1, sx, sy
				       ,dx + borderx, dy + bordery);
#elif defined (WIN32)
			CopyPixmapToScreen (typ, x1, y1, sx, sy, dx, dy);
#endif /* LC_X11 or WIN32 */
			update_pixmap (x1, y1, sx, sy, dx, dy,
				       main_groups[grp].size,
				       main_types[typ].graphic);
		    }
		}
		else
#endif /* USE_PIXMAPS */
		    Fgl_putbox (mw->x + (x - main_screen_originx) * 16,
				mw->y + (y - main_screen_originy) * 16,
				16 * main_groups[grp].size,
				16 * main_groups[grp].size,
				main_types[typ].graphic); 
	    }
	}
    unclip_main_window ();
}

void
update_main_screen_pollution (void)
{
    Rect* mw = &scr.main_win;
    int x, y, col;

    for (y = main_screen_originy;
	 y < main_screen_originy + (mw->h / 16); y++) {
	for (x = main_screen_originx;
	     x < main_screen_originx + (mw->w / 16); x++) {
	    if (MP_POL(x,y) < 4) {
		col = green (24);
	    } else if (MP_POL(x,y) < 600) {
		col = green (23 - (MP_POL(x,y) / 45));
	    } else {
		col = (int) sqrt ((float) (MP_POL(x,y) - 600)) / 9;
		if (col > 20)
		    col = 20;
		col += red (11);
	    }
	    Fgl_fillbox (mw->x + (x - main_screen_originx) * 16,
			 mw->y + (y - main_screen_originy) * 16,
			 16, 16, col);
	}
    }
}

/* *******************  SCREEN SETUP  ******************* */

/* XXX: WCK: All of the drawing should be done already in screen_full_refresh;
   Why do it here? */
/* GCS: Actually, this function loads the graphics from disk */
/* WCK: Yes, but it used to refresh everything after doing that. */
void
screen_setup (void)
{
    int i;

    /* draw the graph boxes */
    monthgraph_full_refresh ();
    monthgraph_style_timeout = real_time + 10000;
    mps_full_refresh ();
    mps_global_style_timeout = real_time + 10000;

    /* load the pbar graphics */
    /* XXX: WCK: pbar_setup? */
    up_pbar1_graphic = load_graphic ("pbarup1.csi");
    up_pbar2_graphic = load_graphic ("pbarup2.csi");
    down_pbar1_graphic = load_graphic ("pbardown1.csi");
    down_pbar2_graphic = load_graphic ("pbardown2.csi");
    pop_pbar_graphic = load_graphic ("pbarpop.csi");
    tech_pbar_graphic = load_graphic ("pbartech.csi");
    food_pbar_graphic = load_graphic ("pbarfood.csi");
    jobs_pbar_graphic = load_graphic ("pbarjobs.csi");
    coal_pbar_graphic = load_graphic ("pbarcoal.csi");
    goods_pbar_graphic = load_graphic ("pbargoods.csi");
    ore_pbar_graphic = load_graphic ("pbarore.csi");
    steel_pbar_graphic = load_graphic ("pbarsteel.csi");
    money_pbar_graphic = load_graphic ("pbarmoney.csi");
    init_pbars ();

    /* draw the box around the main window */
    draw_main_window_box (green (8));
    /* load the checked and unchecked box graphics */
    checked_box_graphic = load_graphic ("checked_box.csi");
    unchecked_box_graphic = load_graphic ("unchecked_box.csi");

    /* load minimap buttons (but don't draw) */
    ms_normal_button_graphic = load_graphic ("ms-normal-button.csi");
    ms_pollution_button_graphic = load_graphic ("ms-pollution-button.csi");
    ms_fire_cover_button_graphic = load_graphic ("ms-fire-cover-button.csi");
    ms_health_cover_button_graphic
	    = load_graphic ("ms-health-cover-button.csi");
    ms_cricket_cover_button_graphic
	    = load_graphic ("ms-cricket-cover-button.csi");
    ms_ub40_button_graphic = load_graphic ("ms-ub40-button.csi");
    ms_coal_button_graphic = load_graphic ("ms-coal-button.csi");
    ms_starve_button_graphic = load_graphic ("ms-starve-button.csi");
    ms_power_button_graphic = load_graphic ("ms-power-button.csi");
    ms_ocost_button_graphic = load_graphic ("ms-ocost-button.csi");

    /* draw the pause button */
    pause_button1_off = load_graphic ("pause-offl.csi");
    pause_button2_off = load_graphic ("pause-offr.csi");
    pause_button1_on = load_graphic ("pause-onl.csi");
    pause_button2_on = load_graphic ("pause-onr.csi");
    draw_pause (0);

    /* draw the slow button */
    slow_button1_off = load_graphic ("slow-offl.csi");
    slow_button2_off = load_graphic ("slow-offr.csi");
    slow_button1_on = load_graphic ("slow-onl.csi");
    slow_button2_on = load_graphic ("slow-onr.csi");
    draw_slow (0);

    /* draw the medium button */
    med_button1_off = load_graphic ("norm-offl.csi");
    med_button2_off = load_graphic ("norm-offr.csi");
    med_button1_on = load_graphic ("norm-onl.csi");
    med_button2_on = load_graphic ("norm-onr.csi");
    draw_med (0);

    /* draw the fast button */
    fast_button1_off = load_graphic ("fast-offl.csi");
    fast_button2_off = load_graphic ("fast-offr.csi");
    fast_button1_on = load_graphic ("fast-onl.csi");
    fast_button2_on = load_graphic ("fast-onr.csi");
    draw_fast (0);

    /* draw the results button */
    results_button1 = load_graphic ("results-l.csi");
    results_button2 = load_graphic ("results-r.csi");
    draw_results ();

    /* draw the t-overwrite button and load 'on button' */
#if defined (commentout)
    toveron_button1 = load_graphic ("tover1-on.csi");
    toveroff_button1 = load_graphic ("tover1-off.csi");
    toveron_button2 = load_graphic ("tover2-on.csi");
    toveroff_button2 = load_graphic ("tover2-off.csi");
    draw_tover (0);

#ifdef LC_X11
    /* draw the confine mouse button */
    confine_button = load_graphic ("mouse-confined.csi");
    unconfine_button = load_graphic ("mouse-free.csi");
    draw_confine (0);
#endif
#endif

    /* Load and draw menu buttons */
#if defined (commentout)
    menu_button_graphic = load_graphic ("menu-button.csi");
#endif
    draw_menu ();
    draw_help ();
#if defined (commentout)
    load_button_graphic = load_graphic ("load-button.csi");
    draw_load ();
    save_button_graphic = load_graphic ("save-button.csi");
    draw_save ();
    quit_button_graphic = load_graphic ("quit-button.csi");
    draw_quit ();
    help_button_graphic = load_graphic ("help-button.csi");
    draw_help ();
#endif

#ifdef SCREEN_SETUP_DRAWS
    mini_full_refresh ();

    redraw_mouse ();
#endif

    /* load select button graphics */
    select_button_graphic[sbut[0]] = load_graphic ("buldoze-button.csi");
    select_button_type[sbut[0]] = CST_GREEN;
    strcpy (select_button_help[sbut[0]], "bulldoze.hlp");

    select_button_graphic[sbut[1]] = load_graphic ("powerline-button.csi");
    select_button_type[sbut[1]] = CST_POWERL_H_L;
    strcpy (select_button_help[sbut[1]], "powerline.hlp");

    select_button_graphic[sbut[2]] = load_graphic ("powerssolar-button.csi");
    select_button_type[sbut[2]] = CST_POWERS_SOLAR;
    strcpy (select_button_help[sbut[2]], "powerssolar.hlp");

    select_button_graphic[sbut[3]] = load_graphic ("substation-button.csi");
    select_button_type[sbut[3]] = CST_SUBSTATION_R;
    strcpy (select_button_help[sbut[3]], "substation.hlp");

    select_button_graphic[sbut[4]] = load_graphic ("residence-button.csi");
    select_button_type[sbut[4]] = CST_RESIDENCE_LL;
    strcpy (select_button_help[sbut[4]], "residential.hlp");

    select_button_graphic[sbut[5]] = load_graphic ("organic-farm-button.csi");
    select_button_type[sbut[5]] = CST_FARM_O0;
    strcpy (select_button_help[sbut[5]], "farm.hlp");

    select_button_graphic[sbut[6]] = load_graphic ("market-button.csi");
    select_button_type[sbut[6]] = CST_MARKET_EMPTY;
    strcpy (select_button_help[sbut[6]], "market.hlp");

    select_button_help_flag[sbut[7]] = 1;        /* No help for track */
    select_button_graphic[sbut[7]] = load_graphic ("track-button.csi");
    select_button_type[sbut[7]] = CST_TRACK_LR;
    strcpy (select_button_help[sbut[7]], "track.hlp");

    select_button_graphic[sbut[8]] = load_graphic ("coalmine-button.csi");
    select_button_type[sbut[8]] = CST_COALMINE_EMPTY;
    strcpy (select_button_help[sbut[8]], "coalmine.hlp");

    select_button_graphic[sbut[9]] = load_graphic ("rail-button.csi");
    select_button_type[sbut[9]] = CST_RAIL_LR;
    strcpy (select_button_help[sbut[9]], "rail.hlp");

    select_button_graphic[sbut[10]] = load_graphic ("powerscoal-button.csi");
    select_button_type[sbut[10]] = CST_POWERS_COAL_EMPTY;
    strcpy (select_button_help[sbut[10]], "powerscoal.hlp");

    select_button_graphic[sbut[11]] = load_graphic ("road-button.csi");
    select_button_type[sbut[11]] = CST_ROAD_LR;
    strcpy (select_button_help[sbut[11]], "road.hlp");

    select_button_graphic[sbut[12]] = load_graphic ("industryl-button.csi");
    select_button_type[sbut[12]] = CST_INDUSTRY_L_C;
    strcpy (select_button_help[sbut[12]], "industryl.hlp");

    select_button_graphic[sbut[13]] = load_graphic ("university-button.csi");
    select_button_type[sbut[13]] = CST_UNIVERSITY;
    strcpy (select_button_help[sbut[13]], "university.hlp");

    select_button_graphic[sbut[14]] = load_graphic ("commune-button.csi");
    select_button_type[sbut[14]] = CST_COMMUNE_1;
    strcpy (select_button_help[sbut[14]], "commune.hlp");

    select_button_graphic[sbut[15]] = load_graphic ("oremine-button.csi");
    select_button_type[sbut[15]] = CST_OREMINE_1;
    strcpy (select_button_help[sbut[15]], "oremine.hlp");

    select_button_graphic[sbut[16]] = load_graphic ("tip-button.csi");
    select_button_type[sbut[16]] = CST_TIP_0;
    strcpy (select_button_help[sbut[16]], "tip.hlp");

    select_button_graphic[sbut[17]] = load_graphic ("port-button.csi");
    select_button_type[sbut[17]] = CST_EX_PORT;
    strcpy (select_button_help[sbut[17]], "port.hlp");

    select_button_graphic[sbut[18]] = load_graphic ("industryh-button.csi");
    select_button_type[sbut[18]] = CST_INDUSTRY_H_C;
    strcpy (select_button_help[sbut[18]], "industryh.hlp");

    select_button_graphic[sbut[19]] = load_graphic ("parkland-button.csi");
    select_button_type[sbut[19]] = CST_PARKLAND_PLANE;
    strcpy (select_button_help[sbut[19]], "park.hlp");

    select_button_graphic[sbut[20]] = load_graphic ("recycle-button.csi");
    select_button_type[sbut[20]] = CST_RECYCLE;
    strcpy (select_button_help[sbut[20]], "recycle.hlp");

    select_button_graphic[sbut[21]] = load_graphic ("water-button.csi");
    select_button_type[sbut[21]] = CST_WATER;
    strcpy (select_button_help[sbut[21]], "river.hlp");

    select_button_graphic[sbut[22]] = load_graphic ("health-button.csi");
    select_button_type[sbut[22]] = CST_HEALTH;
    strcpy (select_button_help[sbut[22]], "health.hlp");

    select_button_graphic[sbut[23]] = load_graphic ("rocket-button.csi");
    select_button_type[sbut[23]] = CST_ROCKET_1;
    strcpy (select_button_help[sbut[23]], "rocket.hlp");

    select_button_graphic[sbut[24]] = load_graphic ("windmill-button.csi");
    select_button_type[sbut[24]] = CST_WINDMILL_1_R;
    strcpy (select_button_help[sbut[24]], "windmill.hlp");

    select_button_graphic[sbut[25]] = load_graphic ("monument-button.csi");
    select_button_type[sbut[25]] = CST_MONUMENT_0;
    strcpy (select_button_help[sbut[25]], "monument.hlp");

    select_button_graphic[sbut[26]] = load_graphic ("school-button.csi");
    select_button_type[sbut[26]] = CST_SCHOOL;
    strcpy (select_button_help[sbut[26]], "school.hlp");

    select_button_graphic[sbut[27]] = load_graphic ("blacksmith-button.csi");
    select_button_type[sbut[27]] = CST_BLACKSMITH_0;
    strcpy (select_button_help[sbut[27]], "blacksmith.hlp");

    select_button_graphic[sbut[28]] = load_graphic ("mill-button.csi");
    select_button_type[sbut[28]] = CST_MILL_0;
    strcpy (select_button_help[sbut[28]], "mill.hlp");

    select_button_graphic[sbut[29]] = load_graphic ("pottery-button.csi");
    select_button_type[sbut[29]] = CST_POTTERY_0;
    strcpy (select_button_help[sbut[29]], "pottery.hlp");

    select_button_graphic[sbut[30]] = load_graphic ("firestation-button.csi");
    select_button_type[sbut[30]] = CST_FIRESTATION_1;
    strcpy (select_button_help[sbut[30]], "firestation.hlp");

    select_button_graphic[sbut[31]] = load_graphic ("cricket-button.csi");
    select_button_type[sbut[31]] = CST_CRICKET_1;
    strcpy (select_button_help[sbut[31]], "cricket.hlp");

#ifdef SCREEN_SETUP_DRAWS
    draw_select_buttons ();
#endif

    /* disable all the buttons 
       then enable the ones that are available at the start
    */
    for (i = 0; i < NUMOF_SELECT_BUTTONS; i++) {
	select_button_tflag[i] = 0;
    }

#ifdef SCREEN_SETUP_DRAWS
    update_main_screen ();
#endif

}

void
screen_full_refresh (void)
{
    screen_refreshing = 1;
    draw_background ();

    monthgraph_full_refresh ();
    mps_full_refresh ();
    pbars_full_refresh ();
    mini_full_refresh ();

    /* GCS FIX: what about during MT? */
    if (selected_type == CST_GREEN)
	draw_main_window_box (red (8));
    else
	draw_main_window_box (green (8));

    draw_menu ();
    draw_help ();
#if defined (commentout)
    draw_load ();
    draw_save ();
    draw_quit ();
    draw_help ();
#endif
    draw_pause (pause_flag);
    draw_slow (slow_flag & !pause_flag);
    draw_med (med_flag & !pause_flag);
    draw_fast (fast_flag & !pause_flag);
    draw_results ();

    draw_select_buttons ();

    /* GCS:  What about resize during load/save/prefs? */
    /* WCK:  We could just lock resize off when we enter them. */
    if (help_flag) {
	refresh_help_page ();
    }
    refresh_main_screen ();
    print_date();
    print_time_for_year();

    draw_module_cost(get_group_of_type(selected_type));

    refresh_pbars();
    dialog_refresh();
    redraw_mouse();  /* screen_setup used to do this */
    screen_refreshing = 0;
}

#if defined (commentout)
void
draw_ms_buttons (void)
{
    draw_ms_button (&scr.ms_normal_button, ms_normal_button_graphic);
    draw_ms_button (&scr.ms_pollution_button, ms_pollution_button_graphic);
    draw_ms_button (&scr.ms_fire_cover_button, ms_fire_cover_button_graphic);
    draw_ms_button (&scr.ms_health_cover_button, 
		    ms_health_cover_button_graphic);
    draw_ms_button (&scr.ms_cricket_cover_button, 
		    ms_cricket_cover_button_graphic);
    draw_ms_button (&scr.ms_ub40_button, ms_ub40_button_graphic);
    draw_ms_button (&scr.ms_coal_button, ms_coal_button_graphic);
    draw_ms_button (&scr.ms_starve_button, ms_starve_button_graphic);
    draw_ms_button (&scr.ms_power_button, ms_power_button_graphic);
    draw_ms_button (&scr.ms_ocost_button, ms_ocost_button_graphic);
}
#endif

void
draw_select_buttons (void)
{
    int i;
    Rect* sbw = &scr.select_buttons;
    Fgl_fillbox (sbw->x, sbw->y, sbw->w, sbw->h, white (20));

    for (i = 0; i < NUMOF_SELECT_BUTTONS; i++) 
      draw_select_button_graphic (sbut[i], select_button_graphic[sbut[i]]);

    highlight_select_button(old_selected_button);

}



/* XXX: WCK: this is engine code - what is it doing here?  Should be in
   either (e.g.) tech.c or sbut.c and call draw_select_buttons. */
void
update_select_buttons (void)
{
    int i, f;
    for (i = 0; i < NUMOF_SELECT_BUTTONS; i++)
    {
	int g = inv_sbut(i);
	f = select_button_tflag[i];
	if (tech_level >= main_groups[g].tech * MAX_TECH_LEVEL/1000)
	{
	    if (select_button_tflag[i] == 0)
		call_select_change_up (i);
	    f = 1;
	}
	else if (select_button_tflag[i] != 0 &&
		 tech_level
                 < ((main_groups[g].tech - (main_groups[g].tech/10)) * MAX_TECH_LEVEL/1000) )
	    f = 0;
	if (select_button_tflag[i] != f)
	{
	    select_button_tflag[i] = f;
	    draw_select_button_graphic (i, select_button_graphic[i]);
	}
    }
    /* XXX: Why is this here? Should be with rest of tech gained messages! */
    if (tech_level > MODERN_WINDMILL_TECH && modern_windmill_flag == 0)
    {
	ok_dial_box ("mod_wind_up.mes", GOOD, 0L);
	modern_windmill_flag = 1;
    }
}

void
draw_main_window_box (int colour)
{
    Rect* b = &scr.main_win;
    int x;
    for (x = 0; x < 8; x++)
    {
	Fgl_hline (b->x - 1 - x, b->y - 1 - x,
		   b->x + b->w + x, colour + x + x);
	Fgl_hline (b->x - 1 - x, b->y + b->h + x,
		   b->x + b->w + x, colour + x + x);
	Fgl_line (b->x - 1 - x, b->y - x, b->x - 1 - x,
		  b->y + b->h + x, colour + x + x);
	Fgl_line (b->x + b->w + x, b->y - x,
		  b->x + b->w + x, b->y + b->h + x,
		  colour + x + x);
    }
}

void
draw_yellow_bezel (int x, int y, int h, int w)
{
    int i;
    for (i = 1; i < 8; i++) {
	Fgl_hline (x - 1 - i, y - 1 - i, x + w + 1 + i, yellow (16));
	Fgl_line (x - 1 - i, y - 1 - i, x - 1 - i, y + h + 1 + i, yellow (14));
	Fgl_hline (x - 1 - i, y + h + 1 + i, x + w + 1 + i, yellow (22));
	Fgl_line (x + w + 1 + i, y - 1 - i, x + w + 1 + i, y + h + 1 + i,
		  yellow (24));
    }
}

void
draw_small_yellow_bezel (int x, int y, int h, int w)
{
#if defined (commentout)
    int i;
    for (i = 1; i < 4; i++) {
	Fgl_hline (x - 1 - i, y - 1 - i, x + w + 1 + i, yellow (16));
	Fgl_line (x - 1 - i, y - 1 - i, x - 1 - i, y + h + 1 + i, yellow (14));
	Fgl_hline (x - 1 - i, y + h + 1 + i, x + w + 1 + i, yellow (22));
	Fgl_line (x + w + 1 + i, y - 1 - i, x + w + 1 + i, y + h + 1 + i,
		  yellow (24));
    }
#endif
    int i;
    for (i = 1; i < 4; i++) {
	Fgl_hline (x - 1 - i, y - 1 - i, x + w + i, yellow (16));
	Fgl_line (x - 1 - i, y - 1 - i, x - 1 - i, y + h + i, yellow (14));
	Fgl_hline (x - 1 - i, y + h + i, x + w + i, yellow (22));
	Fgl_line (x + w + i, y - 1 - i, x + w + i, y + h + i,
		  yellow (24));
    }
}

#if !defined (LC_X11) && !defined (WIN32)
void
setcustompalette (void)
{
    char s[100];
    int i, n, r, g, b, flag[256];
    FILE *inf;
    Palette pal;
    for (i = 0; i < 256; i++)
	flag[i] = 0;
    if ((inf = fopen (colour_pal_file, "r")) == 0)
    {
	printf ("The colour palette file <%s>... ", colour_pal_file);
	do_error ("Can't find it.");
    }
    while (feof (inf) == 0)
    {
	fgets (s, 99, inf);
	if (sscanf (s, "%d %d %d %d", &n, &r, &g, &b) == 4)
	{
	    pal.color[n].red = r;
	    pal.color[n].green = g;
	    pal.color[n].blue = b;
	    flag[n] = 1;
	}
    }
    fclose (inf);
    for (i = 0; i < 256; i++)
    {
	if (flag[i] == 0)
	{
	    printf ("Colour %d not loaded\n", i);
	    do_error ("Can't continue");
	}
	pal.color[i].red = (unsigned char) ((pal.color[i].red
					     * (1 - gamma_correct_red)) + (64 * sin ((float) pal.color[i].red
										     * M_PI / 128)) * gamma_correct_red);

	pal.color[i].green = (unsigned char) ((pal.color[i].green
					       * (1 - gamma_correct_green)) + (64 * sin ((float) pal.color[i].green
											 * M_PI / 128)) * gamma_correct_green);

	pal.color[i].blue = (unsigned char) ((pal.color[i].blue
					      * (1 - gamma_correct_blue)) + (64 * sin ((float) pal.color[i].blue
										       * M_PI / 128)) * gamma_correct_blue);
    }
    gl_setpalette (&pal);
}
#endif

void
load_fonts()
{
    char s[128];
    int i;
    FILE *inf;
    /* main_font */
    if ((inf = fopen (fontfile, "r")) == 0)
	HandleError ("Can't open the font file", FATAL);
    for (i = 0; i < 256 * 8; i++)
	main_font[i] = fgetc (inf);
    fclose (inf);
    /* start_font1 */
    sprintf (s, "%s%c%s", opening_path, PATH_SLASH, "8x8thin");
    if ((inf = fopen (s, "rb")) == NULL)
	do_error ("Can't open opening screen font 8x8thin");
    for (i = 0; i < 2048; i++)
	start_font1[i] = fgetc (inf);
    fclose (inf);
    /* start_font2 */
    sprintf (s, "%s%c%s", opening_path, PATH_SLASH, "scrawl_w.fnt");
    if ((inf = fopen (s, "rb")) == NULL)
	do_error ("Can't open opening screen font scrawl_w.fnt");
    for (i = 0; i < 4096; i++)
	start_font2[i] = fgetc (inf);
    fclose (inf);
    /* start_font3 */
    sprintf (s, "%s%c%s", opening_path, PATH_SLASH, "scrawl_s.fnt");
    if ((inf = fopen (s, "rb")) == NULL)
	do_error ("Can't open opening screen font scrawl_s.fnt");
    for (i = 0; i < 4096; i++)
	start_font3[i] = fgetc (inf);
    fclose (inf);
}

void
init_fonts (void)
{
#if defined (WIN32)
    init_windows_font();
#endif
    load_fonts();
#if defined (SVGALIB)
    gl_setwritemode (FONT_COMPRESSED);
#endif
    Fgl_setfont (8, 8, main_font);
    Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
}

void
rotate_mini_screen (void)
{
    switch (mini_screen_flags)
    {
    case MINI_SCREEN_NORMAL_FLAG:
	mini_screen_flags = MINI_SCREEN_POL_FLAG;
	break;
    case MINI_SCREEN_POL_FLAG:
	mini_screen_flags = MINI_SCREEN_UB40_FLAG;
	break;
    case MINI_SCREEN_UB40_FLAG:
	mini_screen_flags = MINI_SCREEN_STARVE_FLAG;
	break;
    case MINI_SCREEN_STARVE_FLAG:
	mini_screen_flags = MINI_SCREEN_POWER_FLAG;
	break;
#if defined (commentout)
    case MINI_SCREEN_PORT_FLAG:
	/* Not in the rotation schedule */
#endif
    case MINI_SCREEN_POWER_FLAG:
	mini_screen_flags = MINI_SCREEN_FIRE_COVER;
	break;
    case MINI_SCREEN_FIRE_COVER:
	mini_screen_flags = MINI_SCREEN_CRICKET_COVER;
	break;
    case MINI_SCREEN_CRICKET_COVER:
	mini_screen_flags = MINI_SCREEN_HEALTH_COVER;
	break;
    case MINI_SCREEN_HEALTH_COVER:
	mini_screen_flags = MINI_SCREEN_COAL_FLAG;
	break;
    case MINI_SCREEN_COAL_FLAG:
	mini_screen_flags = MINI_SCREEN_NORMAL_FLAG;
	break;
    }
    update_mini_screen ();
}

void
update_mini_screen (void)
{
    switch (mini_screen_flags)
    {
    case MINI_SCREEN_NORMAL_FLAG:
	draw_mini_screen ();
	break;
    case MINI_SCREEN_POL_FLAG:
	draw_mini_screen_pollution ();
	break;
    case MINI_SCREEN_UB40_FLAG:
	draw_mini_screen_ub40 ();
	break;
    case MINI_SCREEN_STARVE_FLAG:
	draw_mini_screen_starve ();
	break;
#if defined (commentout)
    case MINI_SCREEN_PORT_FLAG:
	draw_mini_screen_port ();
	break;
#endif
    case MINI_SCREEN_POWER_FLAG:
	draw_mini_screen_power ();
	break;
    case MINI_SCREEN_FIRE_COVER:
	draw_mini_screen_fire_cover ();
	break;
    case MINI_SCREEN_CRICKET_COVER:
	draw_mini_screen_cricket_cover ();
	break;
    case MINI_SCREEN_HEALTH_COVER:
	draw_mini_screen_health_cover ();
	break;
    case MINI_SCREEN_COAL_FLAG:
	draw_mini_screen_coal ();
	break;
    }
}

void
mini_screen_help (void)
{
    switch (mini_screen_flags)
    {
    case MINI_SCREEN_NORMAL_FLAG:
	activate_help ("msb-normal.hlp");
	break;
    case MINI_SCREEN_POL_FLAG:
	activate_help ("msb-pol.hlp");
	break;
    case MINI_SCREEN_UB40_FLAG:
	activate_help ("msb-ub40.hlp");
	break;
    case MINI_SCREEN_STARVE_FLAG:
	activate_help ("msb-starve.hlp");
	break;
    case MINI_SCREEN_POWER_FLAG:
	activate_help ("msb-power.hlp");
	break;
    case MINI_SCREEN_FIRE_COVER:
	activate_help ("msb-fire.hlp");
	break;
    case MINI_SCREEN_CRICKET_COVER:
	activate_help ("msb-cricket.hlp");
	break;
    case MINI_SCREEN_HEALTH_COVER:
	activate_help ("msb-health.hlp");
	break;
    case MINI_SCREEN_COAL_FLAG:
	activate_help ("msb-coal.hlp");
	break;
    }
}

void
mini_full_refresh (void)
{
    Rect* mm = &scr.mini_map;
    Rect* mmaux = &scr.mini_map_aux;
    Rect* mmarea = &scr.mini_map_area;

    /* draw border around mini screen */
    draw_small_yellow_bezel (mmarea->x, mmarea->y, mmarea->h, mmarea->w);
    /* draw line between mini_map and mini_map_aux */
    Fgl_hline (mmaux->x, mmaux->y + mmaux->h, mmaux->x + mmaux->w, 
	       yellow (16));
    Fgl_hline (mmaux->x, mmaux->y + mmaux->h + 1, mmaux->x + mmaux->w, 
	       yellow (16));
    /* Black out area l/r of mini_map w/in mini_map_area */
    Fgl_fillbox (mmarea->x, mm->y, mm->x - mmarea->x, mm->h,
		 GRAPHS_B_COLOUR);
    Fgl_fillbox (mm->x + mm->w, mm->y, mmarea->x + mmarea->w - mm->x - mm->w,
		 mm->h, GRAPHS_B_COLOUR);

    /* now draw the mini_screen contents */
    draw_mini_screen ();
}

void
draw_mini_screen (void)
{
    int x, y, xx, yy;
    Rect* mm = &scr.mini_map;

    mini_screen_flags = MINI_SCREEN_NORMAL_FLAG;
    draw_ms_button (ms_normal_button_graphic);
    draw_ms_text ("Land Use    ");
    for (y = 0; y < WORLD_SIDE_LEN; y++) {
	for (x = 0; x < WORLD_SIDE_LEN; x++) {
	    if (MP_TYPE(x,y) == CST_USED) {
		xx = MP_INFO(x,y).int_1;
		yy = MP_INFO(x,y).int_2;

		/* WCK: I think this is what is blowing up */
		if ((xx < 0 || xx > WORLD_SIDE_LEN) ||
		    (yy < 0 || yy > WORLD_SIDE_LEN)) {
		  printf("Argh!  mini_screen out of range on CST_USED!\n");
		  printf("xx=%d,yy=%d.  Continuing\n",xx,yy);
		}

		Fgl_setpixel (mm->x + x, mm->y + y, main_groups[MP_GROUP(xx,yy)].colour);
	    } else {
		Fgl_setpixel (mm->x + x, mm->y + y,  main_groups[MP_GROUP(x,y)].colour);
	    }
	}
    }
    draw_mini_screen_cursor ();

#if defined (WIN32)
    RefreshArea (mm->x, mm->y, mm->x + x, mm->y + y);
#endif
}

void
draw_big_mini_screen (void)
{
    Rect* b = &scr.main_win;
    int x, y, xx, yy;
    for (y = 0; y < WORLD_SIDE_LEN; y++) {
	for (x = 0; x < WORLD_SIDE_LEN; x++) {
	    if (MP_TYPE(x,y) == CST_USED) {
		xx = MP_INFO(x,y).int_1;
		yy = MP_INFO(x,y).int_2;
		Fgl_fillbox (b->x + x * 4, b->y + y * 4, 4, 4
			     ,main_groups[MP_GROUP(xx,yy)].colour);
	    } else {
		Fgl_fillbox (b->x + x * 4, b->y + y * 4, 4, 4
			     ,main_groups[MP_GROUP(x,y)].colour);
	    }
	}
    }
}


void
draw_mini_screen_pollution (void)
{
    int x, y, col;
    Rect* mm = &scr.mini_map;

    mini_screen_flags = MINI_SCREEN_POL_FLAG;
    draw_ms_text ("Pollution   ");
    draw_ms_button (ms_pollution_button_graphic);
    for (y = 0; y < WORLD_SIDE_LEN; y++) {
	for (x = 0; x < WORLD_SIDE_LEN; x++) {
	    if (MP_POL(x,y) < 4) {
		col = green (24);
	    } else if (MP_POL(x,y) < 600) {
		col = green (23 - (MP_POL(x,y) / 45));
	    } else {
		col = (int) sqrt ((float) (MP_POL(x,y) - 600)) / 9;
		if (col > 20)
		    col = 20;
		col += red (11);
	    }
	    Fgl_setpixel (mm->x + x, mm->y + y, col);
	}
    }
    draw_mini_screen_cursor ();
#if defined (WIN32)
    RefreshArea (mm->x, mm->y, mm->x + x, mm->y + y);
#endif
}

void
draw_mini_screen_fire_cover (void)
{
    int x, y, xx, yy, col;
    Rect* mm = &scr.mini_map;

    /* GCS: This sort of works.  Might be better to return to "normal"
       when user clicks on mini-screen, or else draw outline box. */
    mini_screen_flags = MINI_SCREEN_FIRE_COVER;
    draw_ms_text ("Fire cover  ");
    draw_ms_button (ms_fire_cover_button_graphic);
    for (y = 0; y < WORLD_SIDE_LEN; y++) {
	for (x = 0; x < WORLD_SIDE_LEN; x++) {
	    if (MP_TYPE(x,y) == CST_USED)
	    {
		xx = MP_INFO(x,y).int_1;
		yy = MP_INFO(x,y).int_2;
		if ((MP_INFO(xx,yy).flags & FLAG_FIRE_COVER) == 0)
		    col = main_groups[MP_GROUP(xx,yy)].colour;
		else
		    col = green (10);
	    }
	    else
	    {
		if ((MP_INFO(x,y).flags & FLAG_FIRE_COVER) == 0)
		    col = main_groups[MP_GROUP(x,y)].colour;
		else
		    col = green (10);
	    }
	    Fgl_setpixel (mm->x + x, mm->y + y, col);
	}
    }
    draw_mini_screen_cursor ();
#if defined (WIN32)
    RefreshArea (mm->x, mm->y, mm->x + x, mm->y + y);
#endif
}

void
draw_mini_screen_cricket_cover (void)
{
    int x, y, xx, yy, col;
    Rect* mm = &scr.mini_map;

    mini_screen_flags = MINI_SCREEN_CRICKET_COVER;
    draw_ms_text ("Crickt cover");
    draw_ms_button (ms_cricket_cover_button_graphic);
    for (y = 0; y < WORLD_SIDE_LEN; y++) {
	for (x = 0; x < WORLD_SIDE_LEN; x++) {
	    if (MP_TYPE(x,y) == CST_USED)
	    {
		xx = MP_INFO(x,y).int_1;
		yy = MP_INFO(x,y).int_2;
		if ((MP_INFO(xx,yy).flags & FLAG_CRICKET_COVER) == 0)
		    col = main_groups[MP_GROUP(xx,yy)].colour;
		else
		    col = green (10);
	    }
	    else
	    {
		if ((MP_INFO(x,y).flags & FLAG_CRICKET_COVER) == 0)
		    col = main_groups[MP_GROUP(x,y)].colour;
		else
		    col = green (10);
	    }
	    Fgl_setpixel (mm->x + x, mm->y + y, col);
	}
    }
    draw_mini_screen_cursor ();
#if defined (WIN32)
    RefreshArea (mm->x, mm->y, mm->x + x, mm->y + y);
#endif
}

void
draw_mini_screen_health_cover (void)
{
    int x, y, xx, yy, col;
    Rect* mm = &scr.mini_map;

    mini_screen_flags = MINI_SCREEN_HEALTH_COVER;
    draw_ms_text ("Health cover");
    draw_ms_button (ms_health_cover_button_graphic);
    for (y = 0; y < WORLD_SIDE_LEN; y++) {
	for (x = 0; x < WORLD_SIDE_LEN; x++) {
	    if (MP_TYPE(x,y) == CST_USED)
	    {
		xx = MP_INFO(x,y).int_1;
		yy = MP_INFO(x,y).int_2;
		if ((MP_INFO(xx,yy).flags & FLAG_HEALTH_COVER) == 0)
		    col = main_groups[MP_GROUP(xx,yy)].colour;
		else
		    col = green (10);
	    }
	    else
	    {
		if ((MP_INFO(x,y).flags & FLAG_HEALTH_COVER) == 0)
		    col = main_groups[MP_GROUP(x,y)].colour;
		else
		    col = green (10);
	    }
	    Fgl_setpixel (mm->x + x, mm->y + y, col);
	}
    }
    draw_mini_screen_cursor ();
#if defined (WIN32)
    RefreshArea (mm->x, mm->y, mm->x + x, mm->y + y);
#endif
}

void
draw_mini_screen_ub40 (void)
{
    int x, y, col;
    Rect* mm = &scr.mini_map;

    mini_screen_flags = MINI_SCREEN_UB40_FLAG;
    draw_ms_text ("Unemployment");
    draw_ms_button (ms_ub40_button_graphic);
    Fgl_fillbox (mm->x, mm->y, WORLD_SIDE_LEN, WORLD_SIDE_LEN, green (14));
    for (y = 0; y < WORLD_SIDE_LEN; y++) {
	for (x = 0; x < WORLD_SIDE_LEN; x++) {
	    if (MP_GROUP_IS_RESIDENCE(x,y)) {
		if (MP_INFO(x,y).int_1 < -20)
		    col = red (28);
		else if (MP_INFO(x,y).int_1 < 10)
		    col = red (14);
		else
		    col = green (20);
		Fgl_fillbox (mm->x + x, mm->y + y, 3, 3, col);
	    }
	}
    }
    draw_mini_screen_cursor ();
#if defined (WIN32)
    RefreshArea (mm->x, mm->y, mm->x + x, mm->y + y);
#endif
}

void
draw_mini_screen_starve (void)
{
    int x, y, col;
    Rect* mm = &scr.mini_map;

    mini_screen_flags = MINI_SCREEN_STARVE_FLAG;
    draw_ms_text ("Starvation  ");
    draw_ms_button (ms_starve_button_graphic);
    Fgl_fillbox (mm->x, mm->y,
		 WORLD_SIDE_LEN, WORLD_SIDE_LEN, green (14));
    for (y = 0; y < WORLD_SIDE_LEN; y++) {
	for (x = 0; x < WORLD_SIDE_LEN; x++) {
	    if (MP_GROUP_IS_RESIDENCE(x,y)) {
		if ((total_time - MP_INFO(x,y).int_2) < 20)
		    col = red (28);
		else if ((total_time - MP_INFO(x,y).int_2) < 100)
		    col = red (14);
		else
		    col = green (20);
		Fgl_fillbox (mm->x + x, mm->y + y, 3, 3, col);
	    }
	}
    }
    draw_mini_screen_cursor ();
#if defined (WIN32)
    RefreshArea (mm->x, mm->y, mm->x + x, mm->y + y);
#endif
}

void
draw_mini_screen_coal (void)
{
    int x, y, col;
    Rect* mm = &scr.mini_map;

    mini_screen_flags = MINI_SCREEN_COAL_FLAG;
    draw_ms_text ("Coal Reserve");
    draw_ms_button (ms_coal_button_graphic);
    if (coal_survey_done) {
	for (y = 0; y < WORLD_SIDE_LEN; y++) {
	    for (x = 0; x < WORLD_SIDE_LEN; x++) {
		if (MP_INFO(x,y).coal_reserve == 0)
		    col = white (4);
		else if (MP_INFO(x,y).coal_reserve >= COAL_RESERVE_SIZE / 2)
		    col = white (18);
		else if (MP_INFO(x,y).coal_reserve < COAL_RESERVE_SIZE / 2)
		    col = white (28);
		Fgl_setpixel (mm->x + x, mm->y + y, col);
	    }
	}
	draw_mini_screen_cursor ();
    } else {
	Fgl_setfontcolors (white(4), white(28));
	Fgl_fillbox (mm->x, mm->y, WORLD_SIDE_LEN, WORLD_SIDE_LEN, white(4));
	Fgl_write (mm->x + 4, mm->y + 20, "Click here");
	Fgl_write (mm->x + 4, mm->y + 32, "   to do  ");
	Fgl_write (mm->x + 4, mm->y + 44, "coal survey");
	Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
    }
#if defined (WIN32)
    RefreshArea (mm->x, mm->y, mm->x + x, mm->y + y);
#endif
}

void
draw_mini_screen_power (void)
{
    int x, y, xx, yy, col;
    int have_power = 0;
    Rect* mm = &scr.mini_map;

    mini_screen_flags = MINI_SCREEN_POWER_FLAG;
    draw_ms_button (ms_power_button_graphic);
    for (y = 0; y < WORLD_SIDE_LEN; y++) {
	for (x = 0; x < WORLD_SIDE_LEN; x++) {
	    short grp = MP_GROUP(x,y);
	    if (grp == GROUP_USED) {
		xx = MP_INFO(x,y).int_1;
		yy = MP_INFO(x,y).int_2;
		grp = MP_GROUP(xx,yy);
	    } else {
		xx = x;
		yy = y;
	    }
	    if (get_power (xx, yy, 1, 1) != 0) {
		have_power = 1;
		col = green (14);
	    } else if (get_power (xx, yy, 1, 0) != 0) {
		have_power = 1;
		col = green (10);
	    } else {
		col = main_groups[grp].colour;
	    }
	    Fgl_setpixel (mm->x + x, mm->y + y, col);
	}
    }
    if (have_power) {
	draw_ms_text ("Power       ");
    } else {
	draw_ms_text ("Power (none)");
    }
    draw_mini_screen_cursor ();
#if defined (WIN32)
    RefreshArea (mm->x, mm->y, mm->x + x, mm->y + y);
#endif
}

void
draw_mini_screen_ocost (void)
{
    char s[100];
    Rect* b = &scr.mini_map;
    mini_screen_flags = MINI_SCREEN_NORMAL_FLAG;
    draw_ms_text ("Other Costs");
    draw_ms_button (ms_ocost_button_graphic);
    Fgl_fillbox (b->x, b->y,
		 WORLD_SIDE_LEN, WORLD_SIDE_LEN, green (12));
    Fgl_setfontcolors (green (12), TEXT_FG_COLOUR);
    sprintf (s, "OC yr %04d", (total_time / NUMOF_DAYS_IN_YEAR) - 1);
    Fgl_write (b->x + 10, b->y + 2, s);

    if (ly_interest > 19999)
    {
	sprintf (s, "Int %4dK", ly_interest / 1000);
    }
    else
	sprintf (s, "Int %5d", ly_interest);
    Fgl_write (b->x + 10, b->y + 16, s);

    if (ly_school_cost > 19999)
    {
	if (ly_school_cost > 1999999)
	    sprintf (s, "Scl %4dM", ly_school_cost / 1000000);
	else
	    sprintf (s, "Scl %4dK", ly_school_cost / 1000);
    }
    else
	sprintf (s, "Scl %5d", ly_school_cost);
    Fgl_write (b->x + 10, b->y + 24, s);

    if (ly_university_cost > 19999)
    {
	if (ly_university_cost > 1999999)
	    sprintf (s, "Uni %4dM", ly_university_cost / 1000000);
	else
	    sprintf (s, "Uni %4dK", ly_university_cost / 1000);
    }
    else
	sprintf (s, "Uni %5d", ly_university_cost);
    Fgl_write (b->x + 10, b->y + 32, s);

    if (ly_deaths_cost > 19999)
    {
	if (ly_deaths_cost > 1999999)
	    sprintf (s, "UnD %4dM", ly_deaths_cost / 1000000);
	else
	    sprintf (s, "UnD %4dK", ly_deaths_cost / 1000);
    }
    else
	sprintf (s, "UnD %5d", ly_deaths_cost);
    Fgl_write (b->x + 10, b->y + 40, s);

    if (ly_windmill_cost > 19999)
    {
	if (ly_windmill_cost > 1999999)
	    sprintf (s, "WiM %4dM", ly_windmill_cost / 1000000);
	else
	    sprintf (s, "WiM %4dK", ly_windmill_cost / 1000);
    }
    else
	sprintf (s, "WiM %5d", ly_windmill_cost);
    Fgl_write (b->x + 10, b->y + 48, s);

    if (ly_recycle_cost > 19999)
    {
	if (ly_recycle_cost > 1999999)
	    sprintf (s, "Rcy %4dM", ly_recycle_cost / 1000000);
	else
	    sprintf (s, "Rcy %4dK", ly_recycle_cost / 1000);
    }
    else
	sprintf (s, "Rcy %5d", ly_recycle_cost);
    Fgl_write (b->x + 10, b->y + 56, s);

    if (ly_health_cost > 19999)
    {
	if (ly_health_cost > 1999999)
	    sprintf (s, "Hth %4dM", ly_health_cost / 1000000);
	else
	    sprintf (s, "Hth %4dK", ly_health_cost / 1000);
    }
    else
	sprintf (s, "Hth %5d", ly_health_cost);
    Fgl_write (b->x + 10, b->y + 64, s);

    if (ly_rocket_pad_cost > 19999)
    {
	if (ly_rocket_pad_cost > 1999999)
	    sprintf (s, "Rok %4dM", ly_rocket_pad_cost / 1000000);
	else
	    sprintf (s, "Rok %4dK", ly_rocket_pad_cost / 1000);
    }
    else
	sprintf (s, "Rok %5d", ly_rocket_pad_cost);
    Fgl_write (b->x + 10, b->y + 72, s);

    if (ly_fire_cost > 19999)
    {
	if (ly_fire_cost > 1999999)
	    sprintf (s, "Fst %4dM", ly_fire_cost / 1000000);
	else
	    sprintf (s, "Fst %4dK", ly_fire_cost / 1000);
    }
    else
	sprintf (s, "Fst %5d", ly_fire_cost);
    Fgl_write (b->x + 10, b->y + 80, s);

    if (ly_cricket_cost > 19999)
    {
	if (ly_cricket_cost > 1999999)
	    sprintf (s, "Ckt %4dM", ly_cricket_cost / 1000000);
	else
	    sprintf (s, "Ckt %4dK", ly_cricket_cost / 1000);
    }
    else
	sprintf (s, "Ckt %5d", ly_cricket_cost);
    Fgl_write (b->x + 10, b->y + 88, s);

    Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
}

#if defined (commentout)
void
draw_mini_screen_port (void)
{
    char buy[256], sell[256], s[256];
    int l, i, *p1, *p2;
    Rect* b = &scr.mini_map;
    mini_screen_flags = MINI_SCREEN_PORT_FLAG;

    /* GCS FIX: This causes flashing on my SGI, and is really not 
       necessary, since we will completely overwrite the numbers below. */
    Fgl_fillbox (b->x, b->y, WORLD_SIDE_LEN, WORLD_SIDE_LEN, green (12));
    Fgl_setfontcolors (green (12), TEXT_FG_COLOUR);
    sprintf (s, " Port %2d %2d", mini_screen_port_x, mini_screen_port_y);
    Fgl_write (b->x + 4, b->y + 2, s);
    Fgl_write (b->x + 4, b->y + 16, " F C O G S");
    Fgl_write (b->x + 4, b->y + 26, " Buy   Sell");

    p1 = &(MP_INFO(mini_screen_port_x,mini_screen_port_y + 1).int_3);
    p2 = &(MP_INFO(mini_screen_port_x,mini_screen_port_y + 2).int_3);
    for (i = 0; i < 5; i++)
    {
	l = *(p1++) / 100;
	if (l > 19999)
	{
	    if (l > 1999999)
		sprintf (buy, "%4dM", l / 1000000);
	    else
		sprintf (buy, "%4dK", l / 1000);
	}
	else
	    sprintf (buy, "%5d", l);
	l = *(p2++) / 100;
	if (l > 19999)
	{
	    if (l > 1999999)
		sprintf (sell, "%4dM", l / 1000000);
	    else
		sprintf (sell, "%4dK", l / 1000);
	}
	else
	    sprintf (sell, "%5d", l);
	sprintf (s, "%s %s", buy, sell);
	Fgl_write (b->x + 10, b->y + 40 + i * 8, s);
    }

    Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
}
#endif

void
draw_mini_screen_cursor (void)
{
    Rect* mini = &scr.mini_map;
    Rect* mw = &scr.main_win;
    Fgl_hline (mini->x + main_screen_originx
	       ,mini->y + main_screen_originy
	       ,mini->x + main_screen_originx + mw->w / 16 - 1
	       ,green (30));
    Fgl_hline (mini->x + main_screen_originx
	       ,mini->y + main_screen_originy + mw->h / 16 - 1
	       ,mini->x + main_screen_originx + mw->w / 16 - 1
	       ,green (30));
    Fgl_line (mini->x + main_screen_originx
	      ,mini->y + main_screen_originy
	      ,mini->x + main_screen_originx
	      ,mini->y + main_screen_originy + mw->h / 16 - 1
	      ,green (30));
    Fgl_line (mini->x + main_screen_originx + mw->w / 16 - 1
	      ,mini->y + main_screen_originy
	      ,mini->x + main_screen_originx + mw->w / 16 - 1
	      ,mini->y + main_screen_originy + mw->h / 16 - 1
	      ,green (30));

}

void 
initialize_print_stats (void)
{
#if !defined (WIN32)
    hide_mouse ();
#endif
    update_scoreboard.mps = 0;
    update_scoreboard.mini = 0;
    update_scoreboard.date = 0;
    update_scoreboard.money = 0;
    update_scoreboard.monthly = 0;
    update_scoreboard.yearly_1 = 0;
    update_scoreboard.yearly_2 = 0;

#if defined (STATS_WINDOW)
    draw_yellow_bezel (STATS_X, STATS_Y, STATS_H, STATS_W);
    Fgl_write (STATS_X, STATS_Y, "Population ");
    Fgl_write (STATS_X, STATS_Y + 8, "% Starving ");
    Fgl_write (STATS_X, STATS_Y + 16, "Foodstore ");
    Fgl_write (STATS_X, STATS_Y + 24, "% u-ployed ");
    Fgl_write (STATS_X + 18 * 8, STATS_Y, "Tech level   ");
    Fgl_write (STATS_X + 18 * 8, STATS_Y + 8, "Un-nat deaths ");
    Fgl_write (STATS_X + 18 * 8, STATS_Y + 16, "Rockets  ");
    Fgl_write (STATS_X + 18 * 8, STATS_Y + 24, "Shanty towns");
#endif

#if defined (FINANCE_WINDOW)
    draw_yellow_bezel (FINANCE_X, FINANCE_Y, FINANCE_H, FINANCE_W);
    Fgl_write (FINANCE_X, FINANCE_Y, "Income  Expend");
    Fgl_write (FINANCE_X, FINANCE_Y + 8, "IT");
    Fgl_write (FINANCE_X, FINANCE_Y + 16, "CT");
    Fgl_write (FINANCE_X, FINANCE_Y + 24, "GT");
    Fgl_write (FINANCE_X, FINANCE_Y + 32, "XP");
    Fgl_write (FINANCE_X + 64, FINANCE_Y + 8, "OC");
    Fgl_write (FINANCE_X + 64, FINANCE_Y + 16, "UC");
    Fgl_write (FINANCE_X + 64, FINANCE_Y + 24, "TC");
    Fgl_write (FINANCE_X + 64, FINANCE_Y + 32, "IP");
    Fgl_write (FINANCE_X, FINANCE_Y + 48, "Tot");
#endif

#if !defined (WIN32)
    redraw_mouse ();
#endif
}

void
advance_mps_style (void)
{
    mps_global_style = mps_global_style++;
    if (mps_global_style > MPS_GLOBAL_STYLE_MAX) {
	mps_global_style = MPS_GLOBAL_STYLE_MIN;
    }
    mps_global_style_timeout = real_time + 6000;
}

void
refresh_mps (void)
{
    mappoint_stats (-2, -2, mps_global_style);
}

void
advance_monthgraph_style (void)
{
    monthgraph_style = (monthgraph_style % 2) + 1;
    if (monthgraph_style == MONTHGRAPH_STYLE_ECONOMY) {
	monthgraph_style_timeout = real_time + 10000;
    } else {
	monthgraph_style_timeout = real_time + 4000;
    }
}

void
refresh_monthgraph (void)
{
    do_monthgraph (1);
}

void
monthgraph_full_refresh (void)
{
    Rect* mg = &scr.monthgraph;
    draw_small_yellow_bezel (mg->x, mg->y, mg->h, mg->w);
    do_monthgraph (1);
}

void
print_stats (void)
{
    static int flag = 0;
    int monthgraph_full_update = 0;

    if (flag == 0) {
	initialize_print_stats ();
	flag = 1;
    }

#if !defined (WIN32)
    hide_mouse ();
#endif

    /* Networked games refresh the screen in response to server messages.
       The message handler sets flags in the update_scoreboard struct, which 
       are checked below.  Non-networked games set the flags periodically
       based on the current timestep. */
    if (!network_game) {
	if (total_time % NUMOF_DAYS_IN_MONTH == (NUMOF_DAYS_IN_MONTH - 1)) {
	    update_scoreboard.monthly = 1;
	}
	if (total_time % NUMOF_DAYS_IN_YEAR == (NUMOF_DAYS_IN_YEAR - 1)) {
	    update_scoreboard.yearly_1 = 1;
	    update_scoreboard.money = 1;
	}
	if ((total_time % NUMOF_DAYS_IN_YEAR) == 0) {
	    update_scoreboard.yearly_2 = 1;
	}
	if (real_time > mappoint_stats_time) {
	    update_scoreboard.mps = 1;
	    mappoint_stats_time = real_time + 1000;
	}
	if (mini_screen_flags != MINI_SCREEN_NORMAL_FLAG 
	    && real_time > mini_screen_time) {
	    update_scoreboard.mini = 1;
	    mini_screen_time = real_time + 1000;
	}
	if ((total_time % NUMOF_DAYS_IN_YEAR) == 0) {
	    calculate_time_for_year ();
	    print_time_for_year ();
	}
	if (total_time % NUMOF_DAYS_IN_MONTH == 1) {
	    update_scoreboard.date = 1;
	}
    }

    /* Decode and perform update requests according to scoreboard */
    if (update_scoreboard.mps) {
	update_scoreboard.mps = 0;
	/* Update the global style if timeout has occurred */
	if (help_flag || load_flag || save_flag || mappoint_stats_flag) {
	    mps_global_style_timeout = real_time + 6000;
	}
	if (real_time > mps_global_style_timeout) {
	    if (time_multiplex_stats) {
		advance_mps_style ();
	    }
	}
	if (!mappoint_stats_flag) {
	    mappoint_stats (-2, -2, mps_global_style);
	} else {
	    mappoint_stats (-1, -1, -1);
	}
    }
  
    if (update_scoreboard.mini) {
	update_scoreboard.mini = 0;
	update_mini_screen ();
    }

    if (update_scoreboard.date) {
	update_scoreboard.date = 0;
	print_date ();
    }

    if (update_scoreboard.money) {
	update_scoreboard.money = 0;
	print_total_money ();
    }

    if (update_scoreboard.monthly) {
	update_scoreboard.monthly = 0;

	/* Update the global style if timeout has occurred */
	if (help_flag || load_flag || save_flag) {
	    if (monthgraph_style == MONTHGRAPH_STYLE_ECONOMY) {
		monthgraph_style_timeout = real_time + 10000;
	    } else {
		monthgraph_style_timeout = real_time + 4000;
	    }
	}
	if (real_time > monthgraph_style_timeout) {
	    if (time_multiplex_stats) {
		monthgraph_full_update = 1;
		monthgraph_style = (monthgraph_style % 2) + 1;
	    }
	    if (monthgraph_style == MONTHGRAPH_STYLE_ECONOMY) {
		monthgraph_style_timeout = real_time + 10000;
	    } else {
		monthgraph_style_timeout = real_time + 4000;
	    }
	}

	do_monthgraph (monthgraph_full_update);

#if defined (STATS_WINDOW)
	sprintf (s, "%5d ", housed_population + people_pool);
	Fgl_write (STATS_X + 8 * 11, STATS_Y, s);
	i = ((tstarving_population / NUMOF_DAYS_IN_MONTH)
	     * 1000) / ((tpopulation / NUMOF_DAYS_IN_MONTH) + 1);
	sprintf (s, "%3d.%1d ", i / 10, i % 10);
	Fgl_write (STATS_X + 8 * 11, STATS_Y + 8, s);
	sprintf (s, "%5d ", tfood_in_markets / NUMOF_DAYS_IN_MONTH);
	Fgl_write (STATS_X + 8 * 11, STATS_Y + 16, s);
	i = ((tunemployed_population / NUMOF_DAYS_IN_MONTH)
	     * 1000) / ((tpopulation / NUMOF_DAYS_IN_MONTH) + 1);
	sprintf (s, "%3d.%1d ", i / 10, i % 10);
	Fgl_write (STATS_X + 8 * 11, STATS_Y + 24, s);
	sprintf (s, "%5d ", numof_shanties);
	Fgl_write (STATS_X + 8 * (12 + 20), STATS_Y + 24, s);
	sprintf (s, "%5.1f ", (float) tech_level * 100.0 / MAX_TECH_LEVEL);
	Fgl_write (STATS_X + (20 + 12) * 8, STATS_Y, s);
	sprintf (s, "%5d ", unnat_deaths);
	Fgl_write (STATS_X + (20 + 12) * 8, STATS_Y + 8, s);
#endif

	update_pbar_pop (housed_population + people_pool);
	update_pbar_tech (tech_level);
	update_pbar_food (tfood_in_markets / NUMOF_DAYS_IN_MONTH);
	update_pbar_jobs (tjobs_in_markets / NUMOF_DAYS_IN_MONTH);
	update_pbar_money (total_money / 400);
	update_pbar_coal (tcoal_in_markets / NUMOF_DAYS_IN_MONTH);
	update_pbar_goods (tgoods_in_markets / NUMOF_DAYS_IN_MONTH);
	update_pbar_ore (tore_in_markets / NUMOF_DAYS_IN_MONTH);
	update_pbar_steel (tsteel_in_markets / NUMOF_DAYS_IN_MONTH);

	refresh_pbars();

	update_select_buttons ();
    }

    if (update_scoreboard.yearly_1) {
	update_scoreboard.yearly_1 = 0;
    }

#if defined (WIN32)
    UpdateWindow (display.hWnd);
#else
    redraw_mouse ();
#endif
}

void
print_total_money (void)
{
/* GCS FIX:  This should be done in pbar and ms_global/FINANCE */
#if defined (FINANCE_WINDOW)
    char s[100], s2[100], i;
    if (total_money > 2000000000)
	total_money = 2000000000;
    else if (total_money < -2000000000)
	total_money = -2000000000;
    if (total_money / 1000000 == 0)
	strcpy (s, "     ");
    else
	sprintf (s, "%5d", total_money / 1000000);
    if ((total_money < 0) && (total_money / 1000000 != 0))
	sprintf (s2, " %06d", -total_money % 1000000);
    else if (total_money / 1000000 != 0)
	sprintf (s2, " %06d", total_money % 1000000);
    else
	sprintf (s2, "%7d", total_money % 1000000);
    if (total_money / 1000000 != 0)
	s2[0] = MONEY_SEPARATOR;
    strcat (s, s2);
    i = 3;
    if (total_money < 0)
	Fgl_setfontcolors (TEXT_BG_COLOUR, red (30));
    Fgl_write (FINANCE_X + i * 8, FINANCE_Y + 48, s);
    if (total_money < 0)
	Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
#endif
}

void
print_date (void)
{
    char s[50];
    Rect* b = &scr.date;
    sprintf (s, "Date %s %04d ", current_month(total_time),
	     current_year(total_time));
    Fgl_write (b->x, b->y, s);
#if defined (WIN32)
    UpdateWindow (display.hWnd);
#endif
}

void 
calculate_time_for_year (void)
{
    static int time_last_year = 0;
 
    time_for_year = (float) (real_time - time_last_year) / 1000.0;

    time_last_year = real_time;
}

void
print_time_for_year (void)
{
    char s[100];
    Rect* b = &scr.time_for_year;


    if (time_for_year > 3600.0)
	sprintf (s, "%5.1f MINS/year  V %s", time_for_year / 60.0, VERSION);
    else
	sprintf (s, "%5.1f secs/year  V %s ", time_for_year, VERSION);
    Fgl_write (b->x, b->y, s);
}

/* Write a message in the status area of the screen */

void 
status_message(char * message, int colour) 
{
    Rect* b = &scr.status_message;

    if (message == NULL) {
	Fgl_fillbox(b->x, b->y, b->w, b->h, TEXT_BG_COLOUR);
	return;
    }

    Fgl_write (b->x, b->y, message);
#if defined (WIN32)
    UpdateWindow (display.hWnd);
#endif
}

void
init_monthgraph (void)
{
    Rect* mg = &scr.monthgraph;
    Fgl_fillbox (mg->x, mg->y, mg->w + 1, mg->h + 1, GRAPHS_B_COLOUR);
}

void
clear_monthgraph (void)
{
    Rect* mg = &scr.monthgraph;
    Fgl_fillbox (mg->x, mg->y, mg->w + 1, mg->h + 1, GRAPHS_B_COLOUR);
}

static void
do_monthgraph (int full_refresh)
{
    if (full_refresh) {
	clear_monthgraph();
    }
    do_history_linegraph (monthgraph_style == MONTHGRAPH_STYLE_ECONOMY);
    do_sust_barchart (monthgraph_style == MONTHGRAPH_STYLE_SUSTAIN);

#if defined (WIN32)
    {
	Rect* mg = &scr.monthgraph;
	RefreshArea (mg->x, mg->y, mg->x + mg->w,
		     mg->y + mg->h);
    }
#endif
}

/* Must be called after initialize_geometry */
void
initialize_monthgraph (void)
{
    int i;

    monthgraph_size = scr.monthgraph.w;

    monthgraph_pop = (int*) malloc (sizeof(int) * monthgraph_size);
    if (monthgraph_pop == 0) {
	malloc_failure ();
    }
    monthgraph_starve = (int*) malloc (sizeof(int) * monthgraph_size);
    if (monthgraph_starve == 0) {
	malloc_failure ();
    }
    monthgraph_nojobs = (int*) malloc (sizeof(int) * monthgraph_size);
    if (monthgraph_nojobs == 0) {
	malloc_failure ();
    }
    monthgraph_ppool = (int*) malloc (sizeof(int) * monthgraph_size);
    if (monthgraph_ppool == 0) {
	malloc_failure ();
    }
    for (i = 0; i < monthgraph_size; i++) {
	monthgraph_pop[i] = 0;
	monthgraph_starve[i] = 0;
	monthgraph_nojobs[i] = 0;
	monthgraph_ppool[i] = 0;
    }
}

static void
do_history_linegraph (int draw)
{
    Rect* mg = &scr.monthgraph;
    int i;
    float f;

    for (i = mg->w - 1; i > 0; i--) {
	if (draw) {
	    Fgl_line (mg->x + mg->w - i,
		      mg->y + mg->h - monthgraph_nojobs[i],
		      mg->x + mg->w - i,
		      mg->y + mg->h,
		      GRAPHS_B_COLOUR);
	    Fgl_line (mg->x + mg->w - i,
		      mg->y + mg->h - monthgraph_nojobs[i-1],
		      mg->x + mg->w - i,
		      mg->y + mg->h,
		      yellow (24));

	    Fgl_line (mg->x + mg->w - i,
		      mg->y + mg->h - monthgraph_starve[i],
		      mg->x + mg->w - i,
		      mg->y + mg->h,
		      GRAPHS_B_COLOUR);
	    Fgl_line (mg->x + mg->w - i,
		      mg->y + mg->h - monthgraph_starve[i-1],
		      mg->x + mg->w - i,
		      mg->y + mg->h,
		      red (24));

	    Fgl_setpixel (mg->x + mg->w - i,
			  mg->y + mg->h - monthgraph_pop[i],
			  GRAPHS_B_COLOUR);
	    Fgl_setpixel (mg->x + mg->w - i,
			  mg->y + mg->h - monthgraph_pop[i-1], 7);

	    Fgl_setpixel (mg->x + mg->w - i,
			  mg->y + mg->h - monthgraph_ppool[i],
			  GRAPHS_B_COLOUR);
	    Fgl_setpixel (mg->x + mg->w - i,
			  mg->y + mg->h - monthgraph_ppool[i-1],
			  cyan (24));
	}
	monthgraph_pop[i] = monthgraph_pop[i-1];
	monthgraph_ppool[i] = monthgraph_ppool[i-1];
	monthgraph_nojobs[i] = monthgraph_nojobs[i-1];
	monthgraph_starve[i] = monthgraph_starve[i-1];
    }
    if (tpopulation > 0)
    {
	/* Zero out old value of leftmost pixel */
	if (draw) {
	    Fgl_setpixel (mg->x + mg->w,
			  mg->y + mg->h - monthgraph_pop[0], 
			  GRAPHS_B_COLOUR);
	    Fgl_setpixel (mg->x + mg->w, 
			  mg->y + mg->h - monthgraph_starve[0], 
			  GRAPHS_B_COLOUR);
	    Fgl_setpixel (mg->x + mg->w,
			  mg->y + mg->h - monthgraph_nojobs[0], 
			  GRAPHS_B_COLOUR);
	    Fgl_setpixel (mg->x + mg->w,
			  mg->y + mg->h - monthgraph_ppool[0], 
			  GRAPHS_B_COLOUR);
	}
	monthgraph_pop[0] = ((int) (log ((tpopulation / NUMOF_DAYS_IN_MONTH)
					 + 1) * mg->h / 15)) - 5;
	if (monthgraph_pop[0] < 0)
	    monthgraph_pop[0] = 0;
	f = ((float) tstarving_population 
	     / ((float) tpopulation + 1.0)) * 100.0;
	if (tpopulation > 3000)	/* double the scale if pop > 3000 */
	    f += f;
	if (tpopulation > 6000)	/* double it AGAIN if pop > 6000 */
	    f += f;
	monthgraph_starve[0] = (int) f;
	/* max out at 32 */
	if (monthgraph_starve[0] >= mg->h)
	    monthgraph_starve[0] = mg->h - 1;
	f = ((float) tunemployed_population
	     / ((float) tpopulation + 1.0)) * 100.0;
	if (tpopulation > 3000)	/* double the scale if pop > 3000 */
	    f += f;
	if (tpopulation > 6000)	/* double it AGAIN if pop > 6000 */
	    f += f;
	monthgraph_nojobs[0] = (int) f;
	/* max out at 32  */
	if (monthgraph_nojobs[0] >= mg->h)
	    monthgraph_nojobs[0] = mg->h - 1;
	monthgraph_ppool[0] = ((int) (sqrt (people_pool + 1) * mg->h) / 35);
	if (monthgraph_ppool[0] < 0)
	    monthgraph_ppool[0] = 0;
	if (monthgraph_ppool[0] >= mg->h)
	    monthgraph_ppool[0] = mg->h - 1;
	if (draw) {
	    Fgl_setpixel (mg->x + mg->w,
			  mg->y + mg->h - monthgraph_pop[0], 7);
	    Fgl_setpixel (mg->x + mg->w,
			  mg->y + mg->h - monthgraph_starve[0], red (24));
	    Fgl_setpixel (mg->x + mg->w,
			  mg->y + mg->h - monthgraph_nojobs[0], yellow (24));
	    Fgl_setpixel (mg->x + mg->w,
			  mg->y + mg->h - monthgraph_ppool[0], cyan (24));
	}
    }
}

void
clicked_market_cb (int x, int y)
{
    market_cb_flag = 1;
    mcbx = x;
    mcby = y;
}


void
draw_cb_box (int row, int col, int checked)
{
    int x, y;
    char* graphic;
    Rect* mcb = &scr.market_cb;

    y = mcb->y + 4 + (4 * 8) + (row * CB_SPACE);
    x = mcb->x + 12 + (col * 12) * 8;
    graphic = checked ? checked_box_graphic : unchecked_box_graphic;
    Fgl_putbox (x, y, 16, 16, graphic);
}

void
draw_cb_template (int is_market_cb)
{
    int x, y, flags;
    char s[100];
    Rect* mcb = &scr.market_cb;

    x = mcbx;	/* x & y are the market's pos. */
    y = mcby;
    flags = MP_INFO(x,y).flags;
    Fgl_getbox (mcb->x, mcb->y, mcb->w, mcb->h, market_cb_gbuf);
    Fgl_fillbox (mcb->x, mcb->y, mcb->w, mcb->h, 28);
    draw_small_bezel(mcb->x+4, mcb->y+4, mcb->w-8, mcb->h-8, blue(0));
    Fgl_setfontcolors (28,TEXT_FG_COLOUR);
#ifdef USE_EXPANDED_FONT
    gl_setwritemode (WRITEMODE_MASKED | FONT_EXPANDED);
#endif

    if (is_market_cb) {
	Fgl_write (mcb->x + 1 * 8, mcb->y + 8, _("Market"));
    } else {
	Fgl_write (mcb->x + 3 * 8, mcb->y + 8, _("Port"));
    }
    sprintf (s, "%3d,%3d", x, y);
    Fgl_write (mcb->x + 7 * 8, mcb->y + 8, s);
    Fgl_hline (mcb->x + 2, mcb->y + 20, mcb->x + mcb->w - 2, 
	       TEXT_FG_COLOUR);

    Fgl_write (mcb->x +  1 * 8, mcb->y + 3 * 8, _("Buy"));
    Fgl_write (mcb->x + 12 * 8 + 4, mcb->y + 3 * 8 + 1, _("Sell"));
    if (is_market_cb) {
	Fgl_write (mcb->x + 6 * 8 + 4, mcb->y + 40 +(CB_SPACE * 0), _("JOBS"));
    }
    Fgl_write (mcb->x + 6 * 8 + 4, mcb->y + 40 + (CB_SPACE * 1), _("FOOD"));
    Fgl_write (mcb->x + 6 * 8 + 4, mcb->y + 40 + (CB_SPACE * 2), _("COAL"));
    Fgl_write (mcb->x + 7 * 8, mcb->y + 40 + (CB_SPACE * 3), _("ORE"));
    Fgl_write (mcb->x + 6 * 8, mcb->y + 40 + (CB_SPACE * 4), _("GOODS"));
    Fgl_write (mcb->x + 6 * 8, mcb->y + 40 + (CB_SPACE * 5), _("STEEL"));

#ifdef USE_EXPANDED_FONT
    gl_setwritemode (WRITEMODE_OVERWRITE | FONT_EXPANDED);
#endif

    if (is_market_cb) {
	draw_cb_box (0, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_JOBS);
	draw_cb_box (0, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_JOBS);
    }
    draw_cb_box (1, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_FOOD);
    draw_cb_box (1, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_FOOD);
    draw_cb_box (2, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_COAL);
    draw_cb_box (2, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_COAL);
    draw_cb_box (3, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_ORE);
    draw_cb_box (3, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_ORE);
    draw_cb_box (4, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_GOODS);
    draw_cb_box (4, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_GOODS);
    draw_cb_box (5, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_STEEL);
    draw_cb_box (5, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_STEEL);
}

void
draw_market_cb (void)
{
#if defined (LC_X11) || defined (WIN32)
    market_cb_drawn_flag = 1;
#endif
    draw_cb_template (1);
}

#if defined (commentout)
void
draw_market_cb (void)		/* x & y are the market's pos. */
{
    int x, y, flags;
    char s[100];
    Rect* mcb = &scr.market_cb;

#if defined (LC_X11) || defined (WIN32)
    market_cb_drawn_flag = 1;
#endif
    x = mcbx;
    y = mcby;
    flags = MP_INFO(x,y).flags;
    Fgl_getbox (mcb->x, mcb->y, mcb->w, mcb->h, market_cb_gbuf);
    Fgl_fillbox (mcb->x, mcb->y, mcb->w, mcb->h, 28);
    Fgl_setfontcolors (28,TEXT_FG_COLOUR);
#ifdef USE_EXPANDED_FONT
    gl_setwritemode (WRITEMODE_MASKED | FONT_EXPANDED);
#endif

    Fgl_write (mcb->x + 5 * 8, mcb->y + 4, "Market");
    sprintf (s, "%3d,", x);
    Fgl_write (mcb->x + 5 * 8 + 4, mcb->y + 2 * 8, s);
    sprintf (s, "%3d", y);
    Fgl_write (mcb->x + 9 * 8 + 4, mcb->y + 2 * 8, s);

    Fgl_write (mcb->x + 6 * 8, mcb->y + 5 * 8, "FOOD");
    Fgl_write (mcb->x + 6 * 8, mcb->y + 8 * 8, "JOBS");
    Fgl_write (mcb->x + 6 * 8, mcb->y + 11 * 8, "COAL");
    Fgl_write (mcb->x + 6 * 8 + 4, mcb->y + 14 * 8, "ORE");
    Fgl_write (mcb->x + 4 * 8 + 4, mcb->y + 17 * 8, "GOODS");
    Fgl_write (mcb->x + 4 * 8 + 4, mcb->y + 20 * 8, "STEEL");
    Fgl_write (mcb->x + 6, mcb->y + 2 * 8, "Buy");
    Fgl_write (mcb->x + 12 * 8, mcb->y + 2 * 8, "Sell");
#ifdef USE_EXPANDED_FONT
    gl_setwritemode (WRITEMODE_OVERWRITE | FONT_EXPANDED);
#endif

    draw_cb_box (0, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_FOOD);
    draw_cb_box (0, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_FOOD);
    draw_cb_box (1, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_JOBS);
    draw_cb_box (1, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_JOBS);
    draw_cb_box (2, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_COAL);
    draw_cb_box (2, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_COAL);
    draw_cb_box (3, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_ORE);
    draw_cb_box (3, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_ORE);
    draw_cb_box (4, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_GOODS);
    draw_cb_box (4, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_GOODS);
    draw_cb_box (5, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_STEEL);
    draw_cb_box (5, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_STEEL);
}
#endif

void
close_market_cb (void)
{
    Rect* mcb = &scr.market_cb;

    market_cb_flag = 0;
#if defined (LC_X11) || defined (WIN32)
    market_cb_drawn_flag = 0;
#endif
    Fgl_putbox (mcb->x, mcb->y, mcb->w, mcb->h
		,market_cb_gbuf);
    Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);

    /* when exiting market cb, stop the mouse repeating straight away */
    cs_mouse_button = LC_MOUSE_LEFTBUTTON;
#ifdef MOUSE_REPEAT
    cs_mouse_button_repeat = real_time + 1000;
#endif
}

void
clicked_port_cb (int x, int y)
{
    port_cb_flag = 1;
    mcbx = x;
    mcby = y;
}

void
draw_port_cb (void)
{
#if defined (LC_X11) || defined (WIN32)
    port_cb_drawn_flag = 1;
#endif
    draw_cb_template (0);
}

#if defined (commentout)
void
draw_port_cb (void)
{
    int x, y, flags;
    char s[100];
    Rect* mcb = &scr.market_cb;

#if defined (LC_X11) || defined (WIN32)
    port_cb_drawn_flag = 1;
#endif
    x = mcbx;
    y = mcby;

    flags = MP_INFO(x,y).flags;
    /* use the market cb resources where possible. */
    Fgl_getbox (mcb->x, mcb->y, mcb->w, mcb->h, market_cb_gbuf);
    Fgl_fillbox (mcb->x, mcb->y, mcb->w, mcb->h, 28);
#ifdef USE_EXPANDED_FONT
    gl_setwritemode (WRITEMODE_MASKED | FONT_EXPANDED);
#endif
    Fgl_write (mcb->x + 7 * 8, mcb->y + 1, "Port");
    Fgl_write (mcb->x + 7 * 8, mcb->y + 7 * 8, "FOOD");
    Fgl_write (mcb->x + 7 * 8, mcb->y + 10 * 8, "COAL");
    Fgl_write (mcb->x + 7 * 8 + 4, mcb->y + 13 * 8, "ORE");
    Fgl_write (mcb->x + 6 * 8 + 4, mcb->y + 16 * 8, "GOODS");
    Fgl_write (mcb->x + 6 * 8 + 4, mcb->y + 19 * 8, "STEEL");
    Fgl_write (mcb->x + 12, mcb->y + 2 * 8, "Buy");
    Fgl_write (mcb->x + 14 * 8, mcb->y + 2 * 8, "Sell");
    sprintf (s, "%3d", x);
    s[3] = ',';
    s[4] = 0;
    Fgl_write (mcb->x + 5 * 8 + 4, mcb->y + 2 * 8, s);
    sprintf (s, "%3d", y);
    Fgl_write (mcb->x + 9 * 8 + 4, mcb->y + 2 * 8, s);
#ifdef USE_EXPANDED_FONT
    gl_setwritemode (WRITEMODE_OVERWRITE | FONT_EXPANDED);
#endif

    draw_cb_box (1, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_FOOD);
    draw_cb_box (1, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_FOOD);
    draw_cb_box (2, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_COAL);
    draw_cb_box (2, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_COAL);
    draw_cb_box (3, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_ORE);
    draw_cb_box (3, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_ORE);
    draw_cb_box (4, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_GOODS);
    draw_cb_box (4, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_GOODS);
    draw_cb_box (5, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_STEEL);
    draw_cb_box (5, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_STEEL);
}
#endif

void
close_port_cb (void)
{
    Rect* mcb = &scr.market_cb;

    port_cb_flag = 0;
#if defined (LC_X11) || defined (WIN32)
    port_cb_drawn_flag = 0;
#endif
    Fgl_putbox (mcb->x, mcb->y, mcb->w, mcb->h, market_cb_gbuf);
    /* when exiting port cb, stop the mouse repeating straight away */
    cs_mouse_button = LC_MOUSE_LEFTBUTTON;
#ifdef MOUSE_REPEAT
    cs_mouse_button_repeat = real_time + 1000;
#endif
}

#ifdef __dialbox_h__
int
yn_dial_box (char * s1, char * s2, char * s3, char *s4)
{
    int result;
    result = dialog_box(red(10),7,
			0,0,s1,
			0,0,"",
			0,0,s2,
			0,0,s3,
			0,0,s4,
			1,'y',"Yes",
			1,'n',"No");

    return (result == 'y') ? 1 : 0;
}

#else


int
yn_dial_box (char *title, char *s1, char *s2, char *s3)
{
    int x, y, h, i;
    unsigned int w = 0;
    char *ss;

    unrequest_mappoint_stats ();
    unrequest_main_screen ();

#ifdef MOUSE_REPEAT
    cs_mouse_button_delay = 0; 
#endif

    /* find len of longest string */
    if (strlen (s1) > w)
	w = strlen (s1);
    if (strlen (s2) > w)
	w = strlen (s2);
    if (strlen (s3) > w)
	w = strlen (s3);
    w += 4;			/* add a few spaces to the sides */
    if (w < 20)			/* min width */
	w = 20;
    w *= 8;			/* convert chars to pixels */
    h = 9 * 8;

    x = (scr.client_w / 2) - (w / 2);
    y = (scr.client_h / 2) - (h / 2);

    ss = (char *) malloc ((w + 16) * (h + 16) * sizeof (char));
    if (ss == 0) {
	malloc_failure ();
    }
    hide_mouse ();
    Fgl_getbox (x - 8, y - 8, w + 16, h + 16, ss);
    Fgl_fillbox (x, y, w, h, YN_DIALBOX_BG_COLOUR);
    for (i = 1; i < 8; i++)
    {
	Fgl_hline (x - i, y - i, x + w + i - 1, YN_DIALBOX_BG_COLOUR + i + i);
	Fgl_hline (x - i, y + h + i - 1, x + w + i - 1, YN_DIALBOX_BG_COLOUR + i + i);
	Fgl_line (x - i, y - i, x - i, y + h + i - 1, YN_DIALBOX_BG_COLOUR + i + i);
	Fgl_line (x + w + i - 1, y - i, x + w + i - 1, y + h + i - 1, YN_DIALBOX_BG_COLOUR + i + i);
    }
    db_yesbox_x1 = x + w / 4 - (2 * 8);
    db_yesbox_x2 = db_yesbox_x1 + 4 * 8;
    db_yesbox_y1 = (y + h - 14 - 1);
    db_yesbox_y2 = db_yesbox_y1 + 10 + 2;
    db_nobox_x1 = x + ((3 * w) / 4) - (2 * 8);
    db_nobox_x2 = db_nobox_x1 + 4 * 8;
    db_nobox_y1 = (y + h - 14 - 1);
    db_nobox_y2 = db_nobox_y1 + 10 + 2;
    Fgl_fillbox (db_yesbox_x1, db_yesbox_y1, 4 * 8, 10 + 2, white (0));
    Fgl_fillbox (db_nobox_x1, db_nobox_y1, 4 * 8, 10 + 2, white (0));
    Fgl_write (db_yesbox_x1 + 4, db_yesbox_y1 + 2, "YES");
    Fgl_write (db_nobox_x1 + 8, db_nobox_y1 + 2, "NO");
#ifdef USE_EXPANDED_FONT
    gl_setwritemode (WRITEMODE_MASKED | FONT_EXPANDED);
#else
    Fgl_setfontcolors (YN_DIALBOX_BG_COLOUR, TEXT_FG_COLOUR);
#endif
    Fgl_write ((x + w / 2) - (strlen (title) * 4), y + 4, title);
    Fgl_write ((x + w / 2) - (strlen (s1) * 4), y + 20, s1);
    Fgl_write ((x + w / 2) - (strlen (s2) * 4), y + 30, s2);
    Fgl_write ((x + w / 2) - (strlen (s3) * 4), y + 40, s3);
#ifdef USE_EXPANDED_FONT
    gl_setwritemode (WRITEMODE_OVERWRITE | FONT_EXPANDED);
#else
    Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
#endif
    redraw_mouse ();
    db_flag = 1;
    /* GCS FIX:  I'll bet this is the cause of the mouse droppings bug */
    /* shake the mouse a bit to make sure we have the correct cursor. */
    //    cs_mouse_handler (0, -1, 0);
    //    cs_mouse_handler (0, 1, 0);
    db_yesbox_clicked = 0;
    db_nobox_clicked = 0;
    db_yesbox_clicked = 0;
    db_nobox_clicked = 0;
    do
    {
	lc_usleep (1000);

#ifdef LC_X11
	call_event ();
	i = x_key_value;
	x_key_value = 0;
#elif defined (WIN32)
	HandleMouse ();
	i = GetKeystroke ();
#else
	mouse_update ();
	i = vga_getkey ();
#endif
	if (i == 10 || i == 13 || i == ' ' || i == 'y' || i == 'Y')
	    db_yesbox_clicked = 1;
	else if (i == 'n' || i == 127 || i == 'N')
	    db_nobox_clicked = 1;
    }
    while (db_yesbox_clicked == 0 && db_nobox_clicked == 0);
    db_flag = 0;
    hide_mouse ();
    Fgl_putbox (x - 8, y - 8, w + 16, h + 16, ss);
    redraw_mouse ();
    /* shake the mouse a bit to make sure we have the correct cursor. */
    //    cs_mouse_handler (0, -1, 0);
    //cs_mouse_handler (0, 1, 0);

    free (ss);
    /*
      // this flag is there to reset the mouse button, mouse_update() is not
      // 'very' re-entrant, so I've got to clean it up after we get out of here.
      // Yet another hack that I don't like! I need to get these dial boxes
      // in the main loop!
    */
    reset_mouse_flag = 1;
#ifdef MOUSE_REPEAT
    cs_mouse_button_repeat = real_time + 1000;
#endif

    request_main_screen ();

    if (db_yesbox_clicked != 0)
	return (1);
    return (0);
}
#endif // #ifdef __dialbox_h__

#ifdef __dialbox_h__
void
ok_dial_box (char *fn, int good_bad, char *xs)
{
    FILE *inf;
    struct stat statbuf;
    int colour;
    char * ss;
    char s[100];
    int retval;

    if (suppress_ok_buttons != 0)
	return;
    if (good_bad == GOOD || good_bad == RESULTS)
	colour = green (14);
    else if (good_bad == BAD)
	colour = red (12);
    else
	colour = white (12);
    if (good_bad == RESULTS)
	strcpy (s, fn);
    else
    {
	strcpy (s, message_path);
	strcat (s, fn);
    }

    if ((inf = fopen (s, "r")) == NULL)
    {
	printf ("Can't open message <%s> for OK dialog box\n", s);
	strcpy (s, message_path);
	strcat (s, "error.mes");
	if ((inf = fopen (s, "r")) == NULL)
	{
	    fprintf (stderr,
		     "Can't open default message <%s> either\n", s);
	    fprintf (stderr, " ...it was not displayed");
	    return;
	}

	stat(s,&statbuf);

    } else 
	stat(s,&statbuf);
    

    ss = lcalloc(statbuf.st_size + 1);
    retval = fread(ss,sizeof(char),statbuf.st_size,inf);
    ss[statbuf.st_size] = '\0';

    if (xs != 0)
	dialog_box(colour,3,
		   0,0,ss,
		   0,0,xs,
		   2,' ',"OK");
    else
	dialog_box(colour,2,
		   0,0,ss,
		   2,' ',"OK");
    fclose(inf);
}

#else


void
ok_dial_box (char *fn, int good_bad, char *xs)
{
    char s[100];
    int i, l, x, y, h, w, colour;
    FILE *inf;
    char *ss;

    unrequest_mappoint_stats ();
    unrequest_main_screen ();

#ifdef MOUSE_REPEAT
    cs_mouse_button_delay = 0;
#endif
    /* select which colour to draw the box in. */
    if (suppress_ok_buttons != 0)
	return;
    if (good_bad == GOOD || good_bad == RESULTS)
	colour = green (14);
    else if (good_bad == BAD)
	colour = red (12);
    else
	colour = white (12);
    if (good_bad == RESULTS)
	strcpy (s, fn);
    else
    {
	strcpy (s, message_path);
	strcat (s, fn);
    }
    if ((inf = fopen (s, "r")) == NULL)
    {
	printf ("Can't open message <%s> for OK dialog box\n", s);
	strcpy (s, message_path);
	strcat (s, "error.mes");
	if ((inf = fopen (s, "r")) == NULL)
	{
	    fprintf (stderr,
		     "Can't open default message <%s> either\n", s);
	    fprintf (stderr, " ...it was not displayed");
	    return;
	}
    }
    /* static 74*22 char array for the message info array */
    l = 0;
    while (feof (inf) == 0 && l < 20) {
	if (fgets (okmessage[l], 70, inf) == 0)
	    break;
	l++;
    }
    fclose (inf);
    if (xs != 0) {
	strncpy (okmessage[l], xs, 70);
	l++;
    }
    /* 'l' is now the number of lines. Work out the height of the box. */
    /* Need half a line above and below the title and 2 lines for the 
       ok button.  10 pixels per line. */
    h = (l + 3) * 10;

    /* Get rid of new line and work out the width of the longest line. */
    w = 0;
    for (i = 0; i < l; i++) {
	/* get rid of the newline */
	if (okmessage[i][strlen (okmessage[i]) - 1] == 0xa)
	    okmessage[i][strlen (okmessage[i]) - 1] = 0;
	if (strlen (okmessage[i]) > (unsigned int) w)
	    w = strlen (okmessage[i]);
    }
    w = (w + 2) * 8;		/* leave a space at either side. */

    /* now we can work out the x and y points. */
    x = (scr.client_w / 2) - (w / 2);
    y = (scr.client_h / 2) - (h / 2);
    ss = (char *) malloc ((w + 16) * (h + 16) * sizeof (char));
    if (ss == 0)
    {
	malloc_failure ();
    }
    hide_mouse ();
    Fgl_getbox (x - 8, y - 8, w + 16, h + 16, ss);
    Fgl_fillbox (x, y, w, h, colour);
    for (i = 1; i < 8; i++) {
	Fgl_hline (x - i, y - i, x + w + i - 1, colour + i + i);
	Fgl_hline (x - i, y + h + i - 1, x + w + i - 1, colour + i + i);
	Fgl_line (x - i, y - i, x - i, y + h + i - 1, colour + i + i);
	Fgl_line (x + w + i - 1, y - i, x + w + i - 1, y + h + i - 1, colour + i + i);
    }
    db_okbox_x1 = x + w / 2 - (2 * 8);
    db_okbox_x2 = db_okbox_x1 + 4 * 8;
    db_okbox_y1 = y + h - 15;
    db_okbox_y2 = db_okbox_y1 + 12;
    Fgl_fillbox (db_okbox_x1, db_okbox_y1, 4 * 8, 10 + 2, white (0));
    Fgl_write (db_okbox_x1 + 8, db_okbox_y1 + 2, "OK");
#ifdef USE_EXPANDED_FONT
    gl_setwritemode (WRITEMODE_MASKED | FONT_EXPANDED);
#else
    Fgl_setfontcolors (colour, TEXT_FG_COLOUR);
#endif
    Fgl_write ((x + w / 2) - (strlen (okmessage[0]) * 4), y + 4, okmessage[0]);
    for (i = 1; i < l; i++)
	Fgl_write ((x + w / 2) - (strlen (okmessage[i]) * 4)
		   ,y + 10 + i * 10, okmessage[i]);
#ifdef USE_EXPANDED_FONT
    gl_setwritemode (WRITEMODE_OVERWRITE | FONT_EXPANDED);
#else
    Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
#endif
    redraw_mouse ();
    db_okflag = 1;
    /* shake the mouse a bit to make sure we have the correct cursor. */
    //    cs_mouse_handler (0, -1, 0);
    //    cs_mouse_handler (0, 1, 0);

#if defined (LC_X11) || defined (WIN32)
    call_event ();
#else
    mouse_update ();
#endif
    db_okbox_clicked = 0;
    cs_mouse_button = LC_MOUSE_LEFTBUTTON;
#if defined (LC_X11) || defined (WIN32)
    call_event ();
#else
    mouse_update ();
#endif
    db_okbox_clicked = 0;
    do
    {
	lc_usleep (1000);

#ifdef LC_X11
	call_wait_event ();
	i = x_key_value;
	x_key_value = 0;
#elif defined (WIN32)
	HandleMouse ();
	i = GetKeystroke ();
#else
	mouse_update ();
	i = vga_getkey ();
#endif
	if (i == 10 || i == 13 || i == ' ')
	    db_okbox_clicked = 1;
    }
    while (db_okbox_clicked == 0);
    db_okflag = 0;
    hide_mouse ();
    Fgl_putbox (x - 8, y - 8, w + 16, h + 16, ss);
    redraw_mouse ();
    /* shake the mouse a bit to make sure we have the correct cursor. */
    //    cs_mouse_handler (0, -1, 0);
    //    cs_mouse_handler (0, 1, 0);

    free (ss);
    /* when exiting dial box, stop the mouse repeating straight away */
    reset_mouse_flag = 1;
#ifdef MOUSE_REPEAT
    cs_mouse_button_repeat = real_time + 1000;
#endif

    request_main_screen ();
}

#endif // #ifdef __dialbox_h__

void
order_select_buttons (void)
{
    /* sbut converts a group into a column major index of the button array. */
    sbut[0] = 16;		/* buldoze */
    sbut[1] = 13;		/* powerline */
    sbut[2] = 15;		/* solar power */
    sbut[3] = 14;		/* substation */
    sbut[4] = 0;		/* residence */
    sbut[5] = 1;		/* farm */
    sbut[6] = 2;		/* market */
    sbut[7] = 19;		/* track */
    sbut[8] = 10;		/* coalmine */
    sbut[9] = 28;		/* rail */
    sbut[10] = 29;		/* coal power */
    sbut[11] = 25;		/* road */
    sbut[12] = 27;		/* light industry */
    sbut[13] = 11;		/* university */
    sbut[14] = 3;		/* commune */
    sbut[15] = 4;		/* oremine */
    sbut[16] = 5;		/* tip */
    sbut[17] = 9;		/* export */
    sbut[18] = 12;		/* heavy industry */
    sbut[19] = 6;		/* parkland */
    sbut[20] = 30;		/* recycle */
    sbut[21] = 20;		/* water */
    sbut[22] = 26;		/* health */
    sbut[23] = 31;		/* rocket */
    sbut[24] = 24;		/* windmill */
    sbut[25] = 17;		/* monument */
    sbut[26] = 21;		/* school */
    sbut[27] = 22;		/* blacksmith */
    sbut[28] = 8;		/* mill */
    sbut[29] = 18;		/* pottery */
    sbut[30] = 23;		/* fire station */
    sbut[31] = 7;		/* cricket  */
}

int
inv_sbut (int button)
{
    int i, j;
    for (i = 0; i < 32; i++)	/* just a test recode later */
	if (sbut[i] == button)
	{
	    j = i;
	    return (j);
	}
    printf ("Button=%d\n", button);
    for (i = 0; i < 32; i++)
	printf ("%5d", sbut[i]);
    printf ("\n");
    do_error ("An inv_sbut error has happened. This is impossible!!");
    return (-1);			/* can't get here */

}


void
call_select_change_up (int button)
{
    button = inv_sbut (button);

    if (button == GROUP_WINDMILL)
	ok_dial_box ("windmillup.mes", GOOD, 0L);
    else if (button == GROUP_COAL_POWER)
	ok_dial_box ("coalpowerup.mes", GOOD, 0L);
    else if (button == (GROUP_SOLAR_POWER - 1))
      /* XXX: */
	/* -1 a hack to make it work. Really dirty :( 
	   Caused by the fact that groups and buttons are different until 
	   after the bulldoze button, then they are the same.
	*/
	ok_dial_box ("solarpowerup.mes", GOOD, 0L);
    else if (button == GROUP_COALMINE)
	ok_dial_box ("coalmineup.mes", GOOD, 0L);
    else if (button == GROUP_RAIL)
	ok_dial_box ("railwayup.mes", GOOD, 0L);
    else if (button == GROUP_ROAD)
	ok_dial_box ("roadup.mes", GOOD, 0L);
    else if (button == GROUP_INDUSTRY_L)
	ok_dial_box ("ltindustryup.mes", GOOD, 0L);
    else if (button == GROUP_UNIVERSITY)
	ok_dial_box ("universityup.mes", GOOD, 0L);
    else if (button == GROUP_OREMINE)
    {
	if (GROUP_OREMINE_TECH > 0)
	    ok_dial_box ("oremineup.mes", GOOD, 0L);
    }
    else if (button == GROUP_PORT)	/* exports are the same */
	ok_dial_box ("import-exportup.mes", GOOD, 0L);
    else if (button == GROUP_INDUSTRY_H)
	ok_dial_box ("hvindustryup.mes", GOOD, 0L);
    else if (button == GROUP_PARKLAND)
    {
	if (GROUP_PARKLAND_TECH > 0)
	    ok_dial_box ("parkup.mes", GOOD, 0L);
    }
    else if (button == GROUP_RECYCLE)
	ok_dial_box ("recycleup.mes", GOOD, 0L);
    else if (button == GROUP_RIVER)
    {
	if (GROUP_WATER_TECH > 0)
	    ok_dial_box ("riverup.mes", GOOD, 0L);
    }
    else if (button == GROUP_HEALTH)
	ok_dial_box ("healthup.mes", GOOD, 0L);
    else if (button == GROUP_ROCKET)
	ok_dial_box ("rocketup.mes", GOOD, 0L);
    else if (button == GROUP_SCHOOL)
    {
	if (GROUP_SCHOOL_TECH > 0)
	    ok_dial_box ("schoolup.mes", GOOD, 0L);
    }
    else if (button == GROUP_BLACKSMITH)
    {
	if (GROUP_BLACKSMITH_TECH > 0)
	    ok_dial_box ("blacksmithup.mes", GOOD, 0L);
    }
    else if (button == GROUP_MILL)
    {
	if (GROUP_MILL_TECH > 0)
	    ok_dial_box ("millup.mes", GOOD, 0L);
    }
    else if (button == GROUP_POTTERY)
    {
	if (GROUP_POTTERY_TECH > 0)
	    ok_dial_box ("potteryup.mes", GOOD, 0L);
    }
    else if (button == GROUP_FIRESTATION)
	ok_dial_box ("firestationup.mes", GOOD, 0L);
    else if (button == GROUP_CRICKET)
	ok_dial_box ("cricketup.mes", GOOD, 0L);
}



void
prog_box (char *title, int percent)
{
    static int flag = 0, oldpercent = 0;
    char s[100];
    int i;
    if (flag == 0) {
	hide_mouse ();
	Fgl_getbox (PROGBOXX - 8, PROGBOXY - 8, PROGBOXW + 16,
		    PROGBOXH + 16, progbox);
	Fgl_fillbox (PROGBOXX, PROGBOXY,
		     PROGBOXW, PROGBOXH, PROGBOX_BG_COLOUR);
	for (i = 1; i < 8; i++) {
	    Fgl_hline (PROGBOXX - i, PROGBOXY - i,
		       PROGBOXX + PROGBOXW + i - 1,
		       PROGBOX_BG_COLOUR + i + i);
	    Fgl_hline (PROGBOXX - i, PROGBOXY + PROGBOXH + i - 1,
		       PROGBOXX + PROGBOXW + i - 1,
		       PROGBOX_BG_COLOUR + i + i);
	    Fgl_line (PROGBOXX - i, PROGBOXY - i,
		      PROGBOXX - i, PROGBOXY + PROGBOXH + i - 1,
		      PROGBOX_BG_COLOUR + i + i);
	    Fgl_line (PROGBOXX + PROGBOXW + i - 1, PROGBOXY - i,
		      PROGBOXX + PROGBOXW + i - 1,
		      PROGBOXY + PROGBOXH + i - 1,
		      PROGBOX_BG_COLOUR + i + i);
	}


	Fgl_setfontcolors (PROGBOX_BG_COLOUR, TEXT_FG_COLOUR);
	Fgl_write ((PROGBOXX + PROGBOXW / 2)
		   - (strlen (title) * 4), PROGBOXY + 8, title);
	Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);

	flag = 1;
    }
    if (percent != oldpercent)
    {
	Fgl_fillbox (PROGBOXX + 20, PROGBOXY + 60,
		     ((PROGBOXW - 40) * percent) / 100,
		     20, PROGBOX_DONE_COL);
	Fgl_fillbox (PROGBOXX + 20 + (((PROGBOXW - 40) * percent) / 100),
		     PROGBOXY + 60,
		     PROGBOXW - 40 - (((PROGBOXW - 40) * percent) / 100),
		     20, PROGBOX_NOTDONE_COL);
	oldpercent = percent;
	Fgl_fillbox (PROGBOXX + (PROGBOXW / 2) - 20,
		     PROGBOXY + 24, 40, 16, PROGBOX_BG_COLOUR);
	sprintf (s, "%3d%%", percent);
	Fgl_setfontcolors (PROGBOX_BG_COLOUR, TEXT_FG_COLOUR);
	Fgl_write (PROGBOXX + (PROGBOXW / 2) - 20, PROGBOXY + 24, s);
	Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
    }
#ifdef LC_X11
    XSync (display.dpy, FALSE);
#endif
    if (percent < 100)
    {
#if defined (WIN32) /* GCS FIX:  Do I need this ????? */
	/* GCS: Fgl_putbox does a refresh; no need to refresh twice */
	RefreshArea (PROGBOXX - 8, PROGBOXY - 8, PROGBOXW + 16, PROGBOXH + 16);
#endif
	return;
    }

    Fgl_putbox (PROGBOXX - 8, PROGBOXY - 8, PROGBOXW + 16, PROGBOXH + 16, progbox);
    redraw_mouse ();
    flag = 0;
}


static void
do_sust_barchart (int draw)
{
#define SUST_BAR_H      5
#define SUST_BAR_GAP_Y  5
  Rect* mg = &scr.monthgraph;
  static int flag = 0;
  if (flag == 0)
    {
      /*	flag = 1; */
      /* draw border around the sustainable screen */
#if defined (commentout)
      draw_small_yellow_bezel (mg->x, mg->y, mg->h, mg->w);
#endif
      if (draw) {
	/* black out the inside */
	Fgl_fillbox (mg->x, mg->y, mg->w, mg->h, 0);
	/* write the "informative" text */
	Fgl_setfontcolors (0, TEXT_FG_COLOUR);
	Fgl_write (mg->x+3,
		   mg->y + SUST_BAR_GAP_Y - 1,
		   "MIN");
	Fgl_write (mg->x+3,
		   mg->y + SUST_BAR_GAP_Y + (SUST_BAR_H + SUST_BAR_GAP_Y) - 1,
		   "PRT");
	Fgl_write (mg->x+3,
		   mg->y + SUST_BAR_GAP_Y + 2 * (SUST_BAR_H + SUST_BAR_GAP_Y) - 1,
		   "MNY");
	Fgl_write (mg->x+3,
		   mg->y + SUST_BAR_GAP_Y + 3 * (SUST_BAR_H + SUST_BAR_GAP_Y) - 1,
		   "POP");
	Fgl_write (mg->x+3,
		   mg->y + SUST_BAR_GAP_Y + 4 * (SUST_BAR_H + SUST_BAR_GAP_Y) - 1,
		   "TEC");
	Fgl_write (mg->x+3,
		   mg->y + SUST_BAR_GAP_Y + 5 * (SUST_BAR_H + SUST_BAR_GAP_Y) - 1,
		   "FIR");
	Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
	/* draw the starting line */
	Fgl_line (mg->x + 38, mg->y, mg->x + 18, mg->y + mg->h,
		  yellow (24));
	/* ore coal */
	Fgl_fillbox (mg->x + 36, mg->y + SUST_BAR_GAP_Y,
		     3, SUST_BAR_H, SUST_ORE_COAL_COL);
	/* import export */
	Fgl_fillbox (mg->x + 36, 
		     mg->y + SUST_BAR_GAP_Y + (SUST_BAR_H + SUST_BAR_GAP_Y),
		     3, SUST_BAR_H, SUST_PORT_COL);
	/* money */
	Fgl_fillbox (mg->x + 36, 
		     mg->y + SUST_BAR_GAP_Y + 2 * (SUST_BAR_H + SUST_BAR_GAP_Y),
		     3, SUST_BAR_H, SUST_MONEY_COL);
	/* population */
	Fgl_fillbox (mg->x + 36, 
		     mg->y + SUST_BAR_GAP_Y + 3 * (SUST_BAR_H + SUST_BAR_GAP_Y),
		     3, SUST_BAR_H, SUST_POP_COL);
	/* tech */
	Fgl_fillbox (mg->x + 36, 
		     mg->y + SUST_BAR_GAP_Y + 4 * (SUST_BAR_H + SUST_BAR_GAP_Y),
		     3, SUST_BAR_H, SUST_TECH_COL);
	/* fire */
	Fgl_fillbox (mg->x + 36, 
		     mg->y + SUST_BAR_GAP_Y + 5 * (SUST_BAR_H + SUST_BAR_GAP_Y),
		     3, SUST_BAR_H, SUST_FIRE_COL);
      }
    }
  if (sust_dig_ore_coal_count >= SUST_ORE_COAL_YEARS_NEEDED
      && sust_port_count >= SUST_PORT_YEARS_NEEDED
      && sust_old_money_count >= SUST_MONEY_YEARS_NEEDED
      && sust_old_population_count >= SUST_POP_YEARS_NEEDED
      && sust_old_tech_count >= SUST_TECH_YEARS_NEEDED
      && sust_fire_count >= SUST_FIRE_YEARS_NEEDED)
    {
      if (sustain_flag == 0)
	ok_dial_box ("sustain.mes", GOOD, 0L);
      sustain_flag = 1;
    }
  else
    sustain_flag = 0;
  if (draw) {
    draw_sustline (0, sust_dig_ore_coal_count,
		   SUST_ORE_COAL_YEARS_NEEDED, SUST_ORE_COAL_COL);
    draw_sustline ((SUST_BAR_H + SUST_BAR_GAP_Y),
		   sust_port_count,
		   SUST_PORT_YEARS_NEEDED, SUST_PORT_COL);
    draw_sustline (2*(SUST_BAR_H + SUST_BAR_GAP_Y),
		   sust_old_money_count,
		   SUST_MONEY_YEARS_NEEDED, SUST_MONEY_COL);
    draw_sustline (3*(SUST_BAR_H + SUST_BAR_GAP_Y),
		   sust_old_population_count,
		   SUST_POP_YEARS_NEEDED, SUST_POP_COL);
    draw_sustline (4*(SUST_BAR_H + SUST_BAR_GAP_Y),
		   sust_old_tech_count,
		   SUST_TECH_YEARS_NEEDED, SUST_TECH_COL);
    draw_sustline (5*(SUST_BAR_H + SUST_BAR_GAP_Y),
		   sust_fire_count,
		   SUST_FIRE_YEARS_NEEDED, SUST_FIRE_COL);
  }

}

static void
draw_sustline (int yoffset, int count, int max, int col)
{
  Rect* mg = &scr.monthgraph;
  int split;
  if (count >= max)
    split = 60;
  else
    split = 60 * count / max;
  Fgl_fillbox (mg->x + 38, 
	       mg->y + SUST_BAR_GAP_Y + yoffset, split, SUST_BAR_H, col);
  if (split < 60)
    Fgl_fillbox (mg->x + 38 + split + 1, 
		 mg->y + SUST_BAR_GAP_Y + yoffset, 60 - split, 
		 SUST_BAR_H, 0);
}

void
dump_screen (void)
{
#if !defined (LC_X11) && !defined (WIN32)
    int x, y, r, g, b;
    FILE *outf;
    if ((outf = fopen ("screendump.raw", "wb")) == NULL)
	do_error ("Can't open screendump.raw");
    for (y = 0; y < 480; y++)
	for (x = 0; x < 640; x++)
	{
	    gl_getpixelrgb (x, y, &r, &g, &b);
	    fputc (r, outf);
	    fputc (g, outf);
	    fputc (b, outf);
	}
    fclose (outf);
#endif
}

void
debug_writeval (int v)
{
    char s[100];
    sprintf (s, "%d  ", v);
    Fgl_write (280, 471, s);
}

int
ask_launch_rocket_now (int x, int y)
{
    return yn_dial_box ("Rocket ready to launch",
			"You can launch it now or wait until later.",
			"If you wait it will continue costing you money.",
			"Launch it later by clicking on the rocket area.");
}

void 
display_rocket_result_dialog (int result)
{
    switch (result) {
    case ROCKET_LAUNCH_BAD:
	ok_dial_box ("launch-fail.mes", BAD, 0L);
	break;
    case ROCKET_LAUNCH_GOOD:
	ok_dial_box ("launch-good.mes", GOOD, 0L);
	break;
    case ROCKET_LAUNCH_EVAC:
	ok_dial_box ("launch-evac.mes", GOOD, 0L);
	break;
    }
}

