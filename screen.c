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
#if defined (HAVE_UNISTD_H)
#include <unistd.h>
#endif
#include "lcstring.h"
#include "common.h"
#include "lctypes.h"
#include "lin-city.h"
#include "screen.h"
#include "engglobs.h"
#include "cliglobs.h"
#include "pixmap.h"
#include "lchelp.h"
#include "mouse.h"
#include "mps.h"
#include "lcintl.h"
#include "ldsvgui.h"
#include "pbar.h"
#include "dialbox.h"
#include "lclib.h"
#include "module_buttons.h"
#include "stats.h"
#include "engine.h"
#include "fileutil.h"

/* ---------------------------------------------------------------------- *
 * External Global Variables
 * ---------------------------------------------------------------------- */
extern int network_game;

/* ---------------------------------------------------------------------- *
 * Public Global Variables
 * ---------------------------------------------------------------------- */
unsigned char main_font[2048];
unsigned char start_font1[2048];
unsigned char start_font2[4096];
unsigned char start_font3[4096];
Update_Scoreboard update_scoreboard;

int monthgraph_style = MONTHGRAPH_STYLE_MIN;

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
static void do_monthgraph (int full_refresh);
static void do_history_linegraph (int draw);
static void do_sust_barchart (int draw);
static void draw_sustline (int yoffset, int count, int max, int col);
void monthgraph_full_refresh (void);
void draw_mini_pol_in_main_win ();
void mini_full_refresh (void);
void update_main_screen_normal (int full_refresh);
void update_main_screen_pollution (void);
void update_main_screen_ub40 (void);
void update_main_screen_starve (void);
void update_main_screen_power (void);
void update_main_screen_fire_cover (void);
void update_main_screen_cricket_cover (void);
void update_main_screen_health_cover (void);
void update_main_screen_coal (void);



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
    Fgl_fillbox (0, 0, display.winW, display.winH, TEXT_BG_COLOUR);
#endif
}

void
connect_transport_main_screen (void)
{
    Rect* b = &scr.main_win;

    connect_transport (main_screen_originx, main_screen_originy,
		       b->w / 16, b->h / 16);
}

void
refresh_main_screen (void)
{
    connect_transport_main_screen ();
    update_main_screen (1);
    update_mini_screen ();
    dialog_refresh();
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
rotate_main_screen (void)
{
    if (main_screen_flag == MAIN_SCREEN_NORMAL_FLAG) {
	main_screen_flag = MAIN_SCREEN_EQUALS_MINI;
    } else {
	main_screen_flag = MAIN_SCREEN_NORMAL_FLAG;
    }
    refresh_main_screen ();
}

void 
update_main_screen (int full_refresh)
{
    if (main_screen_flag == MAIN_SCREEN_NORMAL_FLAG) {
	update_main_screen_normal (full_refresh);
    } else {
	switch (mini_screen_flags) {
	case MINI_SCREEN_NORMAL_FLAG:
	    update_main_screen_normal (full_refresh);
	    break;
	case MINI_SCREEN_POL_FLAG:
	    update_main_screen_pollution ();
	    break;
	case MINI_SCREEN_UB40_FLAG:
	    update_main_screen_ub40 ();
	    break;
	case MINI_SCREEN_STARVE_FLAG:
	    update_main_screen_starve ();
	    break;
	case MINI_SCREEN_POWER_FLAG:
	    update_main_screen_power ();
	    break;
	case MINI_SCREEN_FIRE_COVER:
	    update_main_screen_fire_cover ();
	    break;
	case MINI_SCREEN_CRICKET_COVER:
	    update_main_screen_cricket_cover ();
	    break;
	case MINI_SCREEN_HEALTH_COVER:
	    update_main_screen_health_cover ();
	    break;
	case MINI_SCREEN_COAL_FLAG:
	    if (coal_survey_done) {
		update_main_screen_coal ();
	    } else {
		update_main_screen_normal (full_refresh);
	    }
	    break;
	}
    }
#if defined (WIN32)
    /* GCS -- I have a feeling this is wrong ... */
    if (full_refresh) {
	UpdateWindow (display.hWnd);
    }
#else
    if (mouse_type == MOUSE_TYPE_SQUARE)
	redraw_mouse ();
#endif
}

void
update_main_screen_normal (int full_refresh)
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
	    if (typ != mappointoldtype[x][y] || full_refresh)
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

void 
update_main_screen_ub40 (void)
{
    Rect* mw = &scr.main_win;
    int x, y, col;

    for (y = main_screen_originy;
	 y < main_screen_originy + (mw->h / 16); y++) {
	for (x = main_screen_originx;
	     x < main_screen_originx + (mw->w / 16); x++) {
	    int xx = x;
	    int yy = y;
	    if (MP_TYPE(x,y) == CST_USED) {
		xx = MP_INFO(x,y).int_1;
		yy = MP_INFO(x,y).int_2;
	    }
	    if (MP_GROUP_IS_RESIDENCE(xx,yy)) {
		if (MP_INFO(xx,yy).int_1 < -20)
		    col = red (28);
		else if (MP_INFO(xx,yy).int_1 < 10)
		    col = red (14);
		else
		    col = green (20);
	    } else {
		col = green (14);
	    }
	    Fgl_fillbox (mw->x + (x - main_screen_originx) * 16,
			 mw->y + (y - main_screen_originy) * 16,
			 16, 16, col);
	}
    }
}

void 
update_main_screen_starve (void)
{
    Rect* mw = &scr.main_win;
    int x, y, col;

    for (y = main_screen_originy;
	 y < main_screen_originy + (mw->h / 16); y++) {
	for (x = main_screen_originx;
	     x < main_screen_originx + (mw->w / 16); x++) {
	    int xx = x;
	    int yy = y;
	    if (MP_TYPE(x,y) == CST_USED) {
		xx = MP_INFO(x,y).int_1;
		yy = MP_INFO(x,y).int_2;
	    }
	    if (MP_GROUP_IS_RESIDENCE(xx,yy)) {
		if ((total_time - MP_INFO(x,y).int_2) < 20)
		    col = red (28);
		else if ((total_time - MP_INFO(x,y).int_2) < 100)
		    col = red (14);
		else
		    col = green (20);
	    } else {
		col = green (14);
	    }
	    Fgl_fillbox (mw->x + (x - main_screen_originx) * 16,
			 mw->y + (y - main_screen_originy) * 16,
			 16, 16, col);
	}
    }
}

void 
update_main_screen_power (void)
{
    Rect* mw = &scr.main_win;
    int x, y, col;

    for (y = main_screen_originy;
	 y < main_screen_originy + (mw->h / 16); y++) {
	for (x = main_screen_originx;
	     x < main_screen_originx + (mw->w / 16); x++) {
	    int xx = x;
	    int yy = y;
	    if (MP_TYPE(x,y) == CST_USED) {
		xx = MP_INFO(x,y).int_1;
		yy = MP_INFO(x,y).int_2;
	    }
	    if (get_power (xx, yy, 1, 1) != 0) {
		col = green (14);
	    } else if (get_power (xx, yy, 1, 0) != 0) {
		col = green (10);
	    } else {
		// col = MP_COLOR(xx,yy);
		col = green (20);
	    }
	    Fgl_fillbox (mw->x + (x - main_screen_originx) * 16,
			 mw->y + (y - main_screen_originy) * 16,
			 16, 16, col);
	}
    }
}

void 
update_main_screen_fire_cover (void)
{
    Rect* mw = &scr.main_win;
    int x, y, col;

    for (y = main_screen_originy;
	 y < main_screen_originy + (mw->h / 16); y++) {
	for (x = main_screen_originx;
	     x < main_screen_originx + (mw->w / 16); x++) {
	    int xx = x;
	    int yy = y;
	    if (MP_TYPE(x,y) == CST_USED) {
		xx = MP_INFO(x,y).int_1;
		yy = MP_INFO(x,y).int_2;
	    }

	    if ((MP_INFO(xx,yy).flags & FLAG_FIRE_COVER) == 0)
		// col = MP_COLOR(xx,yy);
		col = green (20);
	    else
		col = green (10);
	    Fgl_fillbox (mw->x + (x - main_screen_originx) * 16,
			 mw->y + (y - main_screen_originy) * 16,
			 16, 16, col);
	}
    }
}

void 
update_main_screen_health_cover (void)
{
    Rect* mw = &scr.main_win;
    int x, y, col;

    for (y = main_screen_originy;
	 y < main_screen_originy + (mw->h / 16); y++) {
	for (x = main_screen_originx;
	     x < main_screen_originx + (mw->w / 16); x++) {
	    int xx = x;
	    int yy = y;
	    if (MP_TYPE(x,y) == CST_USED) {
		xx = MP_INFO(x,y).int_1;
		yy = MP_INFO(x,y).int_2;
	    }
	    if ((MP_INFO(xx,yy).flags & FLAG_HEALTH_COVER) == 0)
		// col = MP_COLOR(xx,yy);
		col = green (20);
	    else
		col = green (10);
	    Fgl_fillbox (mw->x + (x - main_screen_originx) * 16,
			 mw->y + (y - main_screen_originy) * 16,
			 16, 16, col);
	}
    }
}

void 
update_main_screen_cricket_cover (void)
{
    Rect* mw = &scr.main_win;
    int x, y, col;

    for (y = main_screen_originy;
	 y < main_screen_originy + (mw->h / 16); y++) {
	for (x = main_screen_originx;
	     x < main_screen_originx + (mw->w / 16); x++) {
	    int xx = x;
	    int yy = y;
	    if (MP_TYPE(x,y) == CST_USED) {
		xx = MP_INFO(x,y).int_1;
		yy = MP_INFO(x,y).int_2;
	    }
	    if ((MP_INFO(xx,yy).flags & FLAG_CRICKET_COVER) == 0)
		// col = MP_COLOR(xx,yy);
		col = green (20);
	    else
		col = green (10);
	    Fgl_fillbox (mw->x + (x - main_screen_originx) * 16,
			 mw->y + (y - main_screen_originy) * 16,
			 16, 16, col);
	}
    }
}

void 
update_main_screen_coal (void)
{
    Rect* mw = &scr.main_win;
    int x, y, col;

    for (y = main_screen_originy;
	 y < main_screen_originy + (mw->h / 16); y++) {
	for (x = main_screen_originx;
	     x < main_screen_originx + (mw->w / 16); x++) {
	    if (MP_INFO(x,y).coal_reserve == 0)
		col = white (4);
	    else if (MP_INFO(x,y).coal_reserve >= COAL_RESERVE_SIZE / 2)
		col = white (18);
	    else if (MP_INFO(x,y).coal_reserve < COAL_RESERVE_SIZE / 2)
		col = white (28);
	    Fgl_fillbox (mw->x + (x - main_screen_originx) * 16,
			 mw->y + (y - main_screen_originy) * 16,
			 16, 16, col);
	}
    }
}


/* *******************  SCREEN SETUP  ******************* */

void
screen_setup (void)
{
    /* draw the graph boxes */
    monthgraph_full_refresh ();
    monthgraph_style_timeout = real_time + 10000;
    mps_redraw ();
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

    /* GCS moved selection to here */
    set_selected_module (CST_TRACK_LR);

#ifdef SCREEN_SETUP_DRAWS
    mini_full_refresh ();

    redraw_mouse ();

    update_main_screen ();
#endif

}

void
screen_full_refresh (void)
{
    screen_refreshing++;
    draw_background ();

    monthgraph_full_refresh ();
    mps_redraw ();
    pbars_full_refresh ();
    mini_full_refresh ();

    /* GCS FIX: what about during MT? */
    if (selected_module_type == CST_GREEN)
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

    draw_modules ();

    /* GCS:  What about resize during load/save/prefs? */
    /* WCK:  We could just lock resize off when we enter them. */
    if (help_flag) {
	refresh_help_page ();
    }
    refresh_main_screen ();

/*  Text status area */
    print_date();
    print_time_for_year();
    print_total_money();
    draw_selected_module_cost();

    refresh_pbars();
    redraw_mouse();  /* screen_setup used to do this */
    screen_refreshing--;
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

void
load_fonts()
{
    char s[LC_PATH_MAX];
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

/* Miniscreen mouse handlers */

static Mouse_Handle * mini_map_handle;
static Mouse_Handle * mini_aux_handle;

void 
init_mini_map_mouse (void)
{
    mini_map_handle = mouse_register(&scr.mini_map,&mini_map_handler);
    mini_aux_handle = mouse_register(&scr.mini_map_aux,&mini_aux_handler);
}


void
mini_map_handler(int x, int y, int button)
{
    if (button == LC_MOUSE_RIGHTBUTTON) {
	mini_screen_help ();
	return;
    }
    if (mini_screen_flags == MINI_SCREEN_COAL_FLAG && !coal_survey_done) {
	if (yn_dial_box (_("Coal survey"),
			 _("This will cost you 1 million"),
			 _("After that it's is free to call again"),
			 _("Do coal survey?")) == 0)
	{
	    return;
	}
	do_coal_survey ();
	print_total_money ();
	return;
    }
    adjust_main_origin (x - scr.main_win.w / 32, 
			y - scr.main_win.h / 32, 1);
    
    if (mini_screen_flags == MINI_SCREEN_PORT_FLAG)
	draw_mini_screen ();
}

void
mini_aux_handler(int x, int y, int button)
{
    if (button == LC_MOUSE_MIDDLEBUTTON) {
	rotate_main_screen ();
	return;
    } else if (button == LC_MOUSE_RIGHTBUTTON) {
	mini_screen_help ();
	return;
    }
    rotate_mini_screen ();
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

    if (main_screen_flag == MAIN_SCREEN_EQUALS_MINI) {
	refresh_main_screen ();
    }
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
    draw_ms_text (_("Land Use    "));
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
    draw_ms_text (_("Pollution   "));
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
    draw_ms_text (_("Fire cover  "));
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
    draw_ms_text (_("Crickt cover"));
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
    draw_ms_text (_("Health cover"));
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
    draw_ms_text (_("Unemployment"));
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
    draw_ms_text (_("Starvation  "));
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
    draw_ms_text (_("Coal Reserve"));
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
	Fgl_write (mm->x + 4, mm->y + 20, _("Click here"));
	Fgl_write (mm->x + 4, mm->y + 32, _("   to do  "));
	Fgl_write (mm->x + 4, mm->y + 44, _("coal survey"));
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
	draw_ms_text (_("Power       "));
    } else {
	draw_ms_text (_("Power (none)"));
    }
    draw_mini_screen_cursor ();
#if defined (WIN32)
    RefreshArea (mm->x, mm->y, mm->x + x, mm->y + y);
#endif
}

/* GCS -- This is obsolete, right?? */
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
	       ,255);
    Fgl_hline (mini->x + main_screen_originx
	       ,mini->y + main_screen_originy + mw->h / 16 - 1
	       ,mini->x + main_screen_originx + mw->w / 16 - 1
	       ,255);
    Fgl_line (mini->x + main_screen_originx
	      ,mini->y + main_screen_originy
	      ,mini->x + main_screen_originx
	      ,mini->y + main_screen_originy + mw->h / 16 - 1
	      ,255);
    Fgl_line (mini->x + main_screen_originx + mw->w / 16 - 1
	      ,mini->y + main_screen_originy
	      ,mini->x + main_screen_originx + mw->w / 16 - 1
	      ,mini->y + main_screen_originy + mw->h / 16 - 1
	      ,255);

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
    update_scoreboard.message_area = 0;

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

    /* Decode and perform update requests according to scoreboard */
    if (update_scoreboard.mps) {
	update_scoreboard.mps = 0;
	/* Update the global style if timeout has occurred */
	if (help_flag || load_flag || save_flag || mappoint_stats_flag) {
	    mps_global_style_timeout = real_time + 6000;
	}
	if (real_time > mps_global_style_timeout) {
	    if (time_multiplex_stats) {
		mps_global_advance ();
	    }
	}

	mps_update();

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

	update_pbars_monthly();

	refresh_pbars();

	/* GCS, May 11, 2003.  Quick hack for SVGALIB.  Basically,
	 * I need to workaround the fact that a dialog box might pop 
	 * up, causing move_mouse() to nest hide_mouse() calls,
	 * which means that the cursor will leave a trail.		*/
#if !defined (WIN32)
        redraw_mouse ();
#endif
	update_avail_modules (1);
#if !defined (WIN32)
        hide_mouse ();
#endif
    }

    if (update_scoreboard.yearly_1) {
	update_scoreboard.yearly_1 = 0;
    }

    if (update_scoreboard.message_area > 0
	 && real_time > update_scoreboard.message_area) {
	reset_status_message ();
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
    Rect* b = &scr.money;
    char str[MONEY_W / CHAR_WIDTH + 1];
    size_t count;

    count = sprintf(str, _("Money: "));
    count += commify(str + count, (MONEY_W / CHAR_WIDTH) - count, total_money);
    count += snprintf(str + count, (MONEY_W / CHAR_WIDTH) - count, 
		      "                                            ");
    str[MONEY_W / CHAR_WIDTH] = '\0';

    if (total_money < 0)
	Fgl_setfontcolors (TEXT_BG_COLOUR, red (30));

/*     Fgl_putbox (b->x, b->y, 16, 16, money_pbar_graphic); */
    //Fgl_write (b->x + 16, b->y, str);
    Fgl_write (b->x + 32, b->y, str);

    if (total_money < 0)
	Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
}

void
print_date (void)
{
    char s[50];
    Rect* b = &scr.date;
    sprintf (s, _("Date %s %04d "), current_month(total_time),
	     current_year(total_time));
    Fgl_write (b->x, b->y, s);
#if defined (WIN32)
    UpdateWindow (display.hWnd);
#endif
}

void
print_population (void)
{
    draw_pbar (&scr.pbar_pop, pop_pbar_graphic);
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
	sprintf (s, _("%s%5.1f MINS/year  V %s"), 
		 cheat_flag ? _("TEST MODE") : "",
		 time_for_year / 60.0, VERSION);
    else
	sprintf (s, _("%s%5.1f secs/year  V %s "), 
		 cheat_flag ? _("TEST MODE") : "",
		 time_for_year, VERSION);
    Fgl_write (b->x, b->y, s);
}

/* Write a message in the status area of the screen */
void 
status_message_1 (char * message) 
{
    Rect* b = &scr.status_message_1;

    Fgl_fillbox(b->x, b->y, b->w, b->h, TEXT_BG_COLOUR);
    if (message == NULL)
	return;
    Fgl_write (b->x, b->y, message);
}

void 
status_message_2 (char * message) 
{
    Rect* b = &scr.status_message_2;

    Fgl_fillbox(b->x, b->y, b->w, b->h, TEXT_BG_COLOUR);
    if (message == NULL)
	return;
    Fgl_write (b->x, b->y, message);
}

void
status_message (char* m1, char* m2)
{
    status_message_1(m1);
    status_message_2(m2);
#if defined (WIN32)
    UpdateWindow (display.hWnd);
#endif
    update_scoreboard.message_area = real_time + 10000;
}

void
reset_status_message (void)
{
    status_message_1(0);
    status_message_2(0);
#if defined (WIN32)
    UpdateWindow (display.hWnd);
#endif
    update_scoreboard.message_area = 0;
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

    debug_printf ("In draw_cb_template...\n");
    debug_printf ("Coords are %d %d %d %d\n",mcb->x,mcb->y,mcb->w,mcb->h);
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
    market_cb_drawn_flag = 1;
    draw_cb_template (1);
}

void
close_market_cb (void)
{
    Rect* mcb = &scr.market_cb;

    market_cb_flag = 0;
    market_cb_drawn_flag = 0;

    Fgl_putbox (mcb->x, mcb->y, mcb->w, mcb->h
		,market_cb_gbuf);
    Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);

    /* when exiting market cb, stop the mouse repeating straight away */
    cs_mouse_button = LC_MOUSE_LEFTBUTTON;
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
    port_cb_drawn_flag = 1;
    draw_cb_template (0);
}


void
close_port_cb (void)
{
    Rect* mcb = &scr.market_cb;

    port_cb_flag = 0;
    port_cb_drawn_flag = 0;

    Fgl_putbox (mcb->x, mcb->y, mcb->w, mcb->h, market_cb_gbuf);
    /* when exiting port cb, stop the mouse repeating straight away */
    cs_mouse_button = LC_MOUSE_LEFTBUTTON;
}

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
			1,'y',_("Yes"),
			1,'n',_("No"));

    return (result == 'y') ? 1 : 0;
}

void
ok_dial_box (char *fn, int good_bad, char *xs)
{
    FILE *inf;
    struct stat statbuf;
    int colour;
    char * ss;
    char s[LC_PATH_MAX];
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

    if ((inf = fopen (s, "rb")) == NULL)
    {
	printf ("Can't open message <%s> for OK dialog box\n", s);
	strcpy (s, message_path);
	strcat (s, "error.mes");
	if ((inf = fopen (s, "rb")) == NULL)
	{
	    fprintf (stderr,
		     "Can't open default message <%s> either\n", s);
	    fprintf (stderr, " ...it was not displayed");
	    return;
	}
    } 
    stat(s,&statbuf);

    ss = (char *)lcalloc(statbuf.st_size + 1);
    retval = fread(ss,sizeof(char),statbuf.st_size,inf);
    ss[statbuf.st_size] = '\0';

    undosify_string (ss);

#if defined (commentout)
    if (xs != 0)
	dialog_box(colour,3,
		   0,0,ss,
		   0,0,xs,
		   2,' ',_("OK"));
    else
	dialog_box(colour,2,
		   0,0,ss,
		   2,' ',_("OK"));
#endif
    fclose(inf);

    display_info_message (colour, ss, xs);
}

void
format_status_message (char* sm1, char* sm2, int num_char, char* ss, char* xs)
{
    char* src = ss;
    char* tgt = sm1;
    int did_xs = 0;
    int did_sm2 = 0;
    int chars_done = 0;
    *sm2 = *sm1 = '\0';\

    while (src) {
	char src_c = *src++;
	/* note: already undosified */
	switch (src_c) {
	case '\0':
	    if (did_xs || !xs) {
		*tgt = '\0';
		return;
	    } else {
		src = xs;
		did_xs = 1;
	    }
	    /* fall through */
	case '\n':
	    *tgt++ = ' ';
	    break;
	default:
	    *tgt++ = src_c;
	}
	if (++chars_done == num_char) {
	    *tgt = '\0';
	    if (did_sm2) return;
	    tgt = sm2;
	    did_sm2 = 1;
	    chars_done = 0;
	}
    }
}

/* Call this routine instead of dialog_box() */
void
display_info_message (int colour, char* ss, char* xs)
{
    if (suppress_popups) {
	/* display in the message area */
        Rect* b = &scr.status_message_1;
	int num_char = b->w / 9 - 1;
	char *sm1, *sm2;
	if ((sm1 = (char *) malloc (num_char+1)) == 0)
	    malloc_failure ();
	if ((sm2 = (char *) malloc (num_char+1)) == 0)
	    malloc_failure ();

	format_status_message (sm1,sm2,num_char,ss,xs);
	status_message(sm1,sm2);
	free (sm1);
	free (sm2);
    } else {
	/* display as dialog box */
	if (xs) {
	dialog_box(colour,3,
		   0,0,ss,
		   0,0,xs,
		   2,' ',_("OK"));
	} else {
	dialog_box(colour,2,
		   0,0,ss,
		   2,' ',_("OK"));
	}
    }
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
		   /* TRANSLATORS: 
		      MIN=Mining, PRT=Import/export from port,
		      MNY=Money, POP=Population, TEC=Technology,
		      FIR=Fire coverage
		   */
		   _("MIN"));
	Fgl_write (mg->x+3,
		   mg->y + SUST_BAR_GAP_Y + (SUST_BAR_H + SUST_BAR_GAP_Y) - 1,
		   _("PRT"));
	Fgl_write (mg->x+3,
		   mg->y + SUST_BAR_GAP_Y + 2 * (SUST_BAR_H + SUST_BAR_GAP_Y) - 1,
		   _("MNY"));
	Fgl_write (mg->x+3,
		   mg->y + SUST_BAR_GAP_Y + 3 * (SUST_BAR_H + SUST_BAR_GAP_Y) - 1,
		   _("POP"));
	Fgl_write (mg->x+3,
		   mg->y + SUST_BAR_GAP_Y + 4 * (SUST_BAR_H + SUST_BAR_GAP_Y) - 1,
		   _("TEC"));
	Fgl_write (mg->x+3,
		   mg->y + SUST_BAR_GAP_Y + 5 * (SUST_BAR_H + SUST_BAR_GAP_Y) - 1,
		   _("FIR"));
	Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
	/* draw the starting line */
	Fgl_line (mg->x + 38, mg->y, mg->x + 38, mg->y + mg->h,
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
ask_launch_rocket_click (int x, int y)
{
  return yn_dial_box (_("ROCKET LAUNCH"),
		      _("You can launch the rocket now or wait until later."),
		      _("If you wait, it costs you *only* money to keep the"),
		      _("rocket ready.    Launch?"));
}

int
ask_launch_rocket_now (int x, int y)
{
    return yn_dial_box (_("Rocket ready to launch"),
			_("You can launch it now or wait until later."),
			_("If you wait it will continue costing you money."),
			_("Launch it later by clicking on the rocket area."));
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

void
Fgl_getrect(Rect * r, void * buffer)
{
    Fgl_getbox(r->x,r->y,r->w,r->h,buffer);
}

void 
Fgl_putrect(Rect *r, void * buffer)
{
    Fgl_putbox(r->x,r->y,r->w,r->h,buffer);
}

