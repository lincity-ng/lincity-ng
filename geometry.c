/* ---------------------------------------------------------------------- *
 * geometry.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include "lcconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lcstring.h"
#include "lin-city.h"
#include "common.h"
#include "geometry.h"
#include "cliglobs.h"
#include "pixmap.h"
#include "screen.h"
#include "clistubs.h"
#include "lcintl.h"
#include "pbar.h"



/* ---------------------------------------------------------------------- *
 * Private function prototypes
 * ---------------------------------------------------------------------- */
void resize_main_win (int new_width, int new_height);


/* ---------------------------------------------------------------------- *
 * General functions
 * ---------------------------------------------------------------------- */
void 
initialize_geometry (Screen_Geometry* scr)
{
    scr->border_x = 0;
    scr->border_y = 0;

    scr->client_w = 640;
    scr->client_h = 480;

    scr->client_win.x = 0;
    scr->client_win.y = 0;
    scr->client_win.w = 640;
    scr->client_win.h = 480;

    scr->main_win.x = MAIN_WIN_X;
    scr->main_win.y = MAIN_WIN_Y;
    scr->main_win.h = MAIN_WIN_H;
    scr->main_win.w = MAIN_WIN_W;

    scr->pause_button.x = PAUSE_BUTTON_X;
    scr->pause_button.y = PAUSE_BUTTON_Y;
    scr->pause_button.h = PAUSE_BUTTON_H;
    scr->pause_button.w = PAUSE_BUTTON_W;

    scr->slow_button.x = SLOW_BUTTON_X;
    scr->slow_button.y = SLOW_BUTTON_Y;
    scr->slow_button.h = SLOW_BUTTON_H;
    scr->slow_button.w = SLOW_BUTTON_W;

    scr->med_button.x = MED_BUTTON_X;
    scr->med_button.y = MED_BUTTON_Y;
    scr->med_button.h = MED_BUTTON_H;
    scr->med_button.w = MED_BUTTON_W;

    scr->fast_button.x = FAST_BUTTON_X;
    scr->fast_button.y = FAST_BUTTON_Y;
    scr->fast_button.h = FAST_BUTTON_H;
    scr->fast_button.w = FAST_BUTTON_W;

    scr->menu_button.x = MENU_BUTTON_X;
    scr->menu_button.y = MENU_BUTTON_Y;
    scr->menu_button.h = MENU_BUTTON_H;
    scr->menu_button.w = MENU_BUTTON_W;

    scr->load_button.x = LOAD_BUTTON_X;
    scr->load_button.y = LOAD_BUTTON_Y;
    scr->load_button.h = LOAD_BUTTON_H;
    scr->load_button.w = LOAD_BUTTON_W;

    scr->save_button.x = SAVE_BUTTON_X;
    scr->save_button.y = SAVE_BUTTON_Y;
    scr->save_button.h = SAVE_BUTTON_H;
    scr->save_button.w = SAVE_BUTTON_W;

    scr->quit_button.x = QUIT_BUTTON_X;
    scr->quit_button.y = QUIT_BUTTON_Y;
    scr->quit_button.h = QUIT_BUTTON_H;
    scr->quit_button.w = QUIT_BUTTON_W;

    scr->help_button.x = HELP_BUTTON_X;
    scr->help_button.y = HELP_BUTTON_Y;
    scr->help_button.h = HELP_BUTTON_H;
    scr->help_button.w = HELP_BUTTON_W;

    scr->results_button.x = RESULTS_BUTTON_X;
    scr->results_button.y = RESULTS_BUTTON_Y;
    scr->results_button.h = RESULTS_BUTTON_H;
    scr->results_button.w = RESULTS_BUTTON_W;

    scr->tover_button.x = TOVER_BUTTON_X;
    scr->tover_button.y = TOVER_BUTTON_Y;
    scr->tover_button.h = TOVER_BUTTON_H;
    scr->tover_button.w = TOVER_BUTTON_W;

    scr->confine_button.x = CONFINE_BUTTON_X;
    scr->confine_button.y = CONFINE_BUTTON_Y;
    scr->confine_button.h = CONFINE_BUTTON_H;
    scr->confine_button.w = CONFINE_BUTTON_W;

    scr->select_buttons.x = SELECT_BUTTON_WIN_X;
    scr->select_buttons.y = SELECT_BUTTON_WIN_Y;
    scr->select_buttons.h = SELECT_BUTTON_WIN_H;
    scr->select_buttons.w = SELECT_BUTTON_WIN_W;

    scr->pbar_area.x = PBAR_AREA_X;
    scr->pbar_area.y = PBAR_AREA_Y;
    scr->pbar_area.h = PBAR_AREA_H;
    scr->pbar_area.w = PBAR_AREA_W;

    scr->pbar_pop.x = PBAR_POP_X;
    scr->pbar_pop.y = PBAR_POP_Y;
    scr->pbar_pop.h = PBAR_H;
    scr->pbar_pop.w = PBAR_W;

    scr->pbar_tech.x = PBAR_TECH_X;
    scr->pbar_tech.y = PBAR_TECH_Y;
    scr->pbar_tech.h = PBAR_H;
    scr->pbar_tech.w = PBAR_W;

    scr->pbar_food.x = PBAR_FOOD_X;
    scr->pbar_food.y = PBAR_FOOD_Y;
    scr->pbar_food.h = PBAR_H;
    scr->pbar_food.w = PBAR_W;

    scr->pbar_jobs.x = PBAR_JOBS_X;
    scr->pbar_jobs.y = PBAR_JOBS_Y;
    scr->pbar_jobs.h = PBAR_H;
    scr->pbar_jobs.w = PBAR_W;

    scr->pbar_money.x = PBAR_MONEY_X;
    scr->pbar_money.y = PBAR_MONEY_Y;
    scr->pbar_money.h = PBAR_H;
    scr->pbar_money.w = PBAR_W;

    scr->pbar_coal.x = PBAR_COAL_X;
    scr->pbar_coal.y = PBAR_COAL_Y;
    scr->pbar_coal.h = PBAR_H;
    scr->pbar_coal.w = PBAR_W;

    scr->pbar_goods.x = PBAR_GOODS_X;
    scr->pbar_goods.y = PBAR_GOODS_Y;
    scr->pbar_goods.h = PBAR_H;
    scr->pbar_goods.w = PBAR_W;

    scr->pbar_ore.x = PBAR_ORE_X;
    scr->pbar_ore.y = PBAR_ORE_Y;
    scr->pbar_ore.h = PBAR_H;
    scr->pbar_ore.w = PBAR_W;

    scr->pbar_steel.x = PBAR_STEEL_X;
    scr->pbar_steel.y = PBAR_STEEL_Y;
    scr->pbar_steel.h = PBAR_H;
    scr->pbar_steel.w = PBAR_W;

    scr->sust.x = SUST_SCREEN_X;
    scr->sust.y = SUST_SCREEN_Y;
    scr->sust.h = SUST_SCREEN_H;
    scr->sust.w = SUST_SCREEN_W;

    scr->select_message.x = SELECT_BUTTON_MESSAGE_X;
    scr->select_message.y = SELECT_BUTTON_MESSAGE_Y;
    scr->select_message.h = 0;  /* unused */
    scr->select_message.w = 0;  /* unused */

    scr->date.w = DATE_W;  
    scr->date.x = scr->main_win.x + ((scr->main_win.w - scr->date.w) / 2);
    scr->date.y = scr->main_win.y + scr->main_win.h + 16;
    scr->date.h = 0;  /* unused */


    scr->time_for_year.x = scr->main_win.x;
    scr->time_for_year.y = TIME_FOR_YEAR_Y;
    scr->time_for_year.h = 0;  /* unused */
    scr->time_for_year.w = 0;  /* unused */

    scr->status_message.x = scr->main_win.x;
    scr->status_message.y = STATUS_MESSAGE_Y;
    scr->status_message.h = 8;
    scr->status_message.w = scr->main_win.w;

    scr->mini_map.x = MINI_SCREEN_X;
    scr->mini_map.y = MINI_SCREEN_Y;
    scr->mini_map.h = MINI_SCREEN_H;
    scr->mini_map.w = MINI_SCREEN_W;

    scr->mini_map_aux.x = MINI_MAP_AUX_X;
    scr->mini_map_aux.y = MINI_MAP_AUX_Y;
    scr->mini_map_aux.h = MINI_MAP_AUX_H;
    scr->mini_map_aux.w = MINI_MAP_AUX_W;

    scr->mini_map_area.x = MINI_MAP_AREA_X;
    scr->mini_map_area.y = MINI_MAP_AREA_Y;
    scr->mini_map_area.h = MINI_MAP_AREA_H;
    scr->mini_map_area.w = MINI_MAP_AREA_W;

#if defined (commentout)
    scr->ms_normal_button.x = MS_NORMAL_BUTTON_X;
    scr->ms_normal_button.y = MS_NORMAL_BUTTON_Y;
    scr->ms_normal_button.h = MS_BUTTON_H;
    scr->ms_normal_button.w = MS_BUTTON_W;

    scr->ms_pollution_button.x = MS_POLLUTION_BUTTON_X;
    scr->ms_pollution_button.y = MS_POLLUTION_BUTTON_Y;
    scr->ms_pollution_button.h = MS_BUTTON_H;
    scr->ms_pollution_button.w = MS_BUTTON_W;

    scr->ms_fire_cover_button.x = MS_FIRE_COVER_BUTTON_X;
    scr->ms_fire_cover_button.y = MS_FIRE_COVER_BUTTON_Y;
    scr->ms_fire_cover_button.h = MS_BUTTON_H;
    scr->ms_fire_cover_button.w = MS_BUTTON_W;

    scr->ms_ub40_button.x = MS_UB40_BUTTON_X;
    scr->ms_ub40_button.y = MS_UB40_BUTTON_Y;
    scr->ms_ub40_button.h = MS_BUTTON_H;
    scr->ms_ub40_button.w = MS_BUTTON_W;

    scr->ms_coal_button.x = MS_COAL_BUTTON_X;
    scr->ms_coal_button.y = MS_COAL_BUTTON_Y;
    scr->ms_coal_button.h = MS_BUTTON_H;
    scr->ms_coal_button.w = MS_BUTTON_W;

    scr->ms_health_cover_button.x = MS_HEALTH_COVER_BUTTON_X;
    scr->ms_health_cover_button.y = MS_HEALTH_COVER_BUTTON_Y;
    scr->ms_health_cover_button.h = MS_BUTTON_H;
    scr->ms_health_cover_button.w = MS_BUTTON_W;

    scr->ms_cricket_cover_button.x = MS_CRICKET_COVER_BUTTON_X;
    scr->ms_cricket_cover_button.y = MS_CRICKET_COVER_BUTTON_Y;
    scr->ms_cricket_cover_button.h = MS_BUTTON_H;
    scr->ms_cricket_cover_button.w = MS_BUTTON_W;

    scr->ms_power_button.x = MS_POWER_BUTTON_X;
    scr->ms_power_button.y = MS_POWER_BUTTON_Y;
    scr->ms_power_button.h = MS_BUTTON_H;
    scr->ms_power_button.w = MS_BUTTON_W;

    scr->ms_starve_button.x = MS_STARVE_BUTTON_X;
    scr->ms_starve_button.y = MS_STARVE_BUTTON_Y;
    scr->ms_starve_button.h = MS_BUTTON_H;
    scr->ms_starve_button.w = MS_BUTTON_W;

    scr->ms_ocost_button.x = MS_OCOST_BUTTON_X;
    scr->ms_ocost_button.y = MS_OCOST_BUTTON_Y;
    scr->ms_ocost_button.h = MS_BUTTON_H;
    scr->ms_ocost_button.w = MS_BUTTON_W;
#endif

    scr->monthgraph.x = MONTHGRAPH_X;
    scr->monthgraph.y = MONTHGRAPH_Y;
    scr->monthgraph.h = MONTHGRAPH_H;
    scr->monthgraph.w = MONTHGRAPH_W;

    scr->mappoint_stats.x = MAPPOINT_STATS_X;
    scr->mappoint_stats.y = MAPPOINT_STATS_Y;
    scr->mappoint_stats.h = MAPPOINT_STATS_H;
    scr->mappoint_stats.w = MAPPOINT_STATS_W;

    scr->market_cb = scr->pbar_area;
    /*    scr->market_cb.x = MARKET_CB_X;
    scr->market_cb.y = MARKET_CB_Y;
    scr->market_cb.h = MARKET_CB_H;
    scr->market_cb.w = MARKET_CB_W; */
}

#if !defined (SVGALIB)
void 
resize_geometry (int new_width, int new_height)
{
    if (display.winW == new_width && display.winH == new_height) {
	/* Not a resize event. */
	return;
    }

    /* Reset geometry back to default */
    initialize_geometry (&scr);

    scr.client_win.w = new_width;
    scr.client_win.h = new_height;

    if (pix_double) {
	new_width = new_width / 2;
	new_height = new_height / 2;
    }

    /* Update display info */
    display.winW = new_width;
    display.winH = new_height;

    /* Expand pixmap if necessary */
    scr.client_w = new_width - 2*borderx;
    scr.client_h = new_height - 2*bordery;
    resize_pixmap (scr.client_w, scr.client_h);

    /* Adjust items that need adjusting */

    resize_main_win (scr.client_w, scr.client_h);

    scr.select_message.y = SELECT_BUTTON_MESSAGE_Y + (scr.client_h - 480);
    scr.time_for_year.y = TIME_FOR_YEAR_Y + (scr.client_h - 480);
    scr.status_message.y = scr.client_win.h - 30;
    scr.status_message.w = scr.main_win.w;

    scr.date.y = scr.main_win.y + scr.main_win.h + 16;
    scr.date.x = ((scr.main_win.w - scr.date.w) / 2) + scr.main_win.x;

    scr.pbar_area.x = 56 + scr.main_win.w + 16 + 2;
    scr.pbar_pop.x = scr.pbar_area.x + 4;
    scr.pbar_tech.x = scr.pbar_area.x + 4;
    scr.pbar_food.x = scr.pbar_area.x + 4;
    scr.pbar_jobs.x = scr.pbar_area.x + 4;
    scr.pbar_money.x = scr.pbar_area.x + 4;
    scr.pbar_coal.x = scr.pbar_area.x + 4;
    scr.pbar_goods.x = scr.pbar_area.x + 4;
    scr.pbar_ore.x = scr.pbar_area.x + 4;
    scr.pbar_steel.x = scr.pbar_area.x + 4;

    scr.monthgraph.x = scr.pbar_area.x + 4;
    scr.mappoint_stats.x = scr.pbar_area.x + 4;
    scr.market_cb.x = scr.pbar_area.x;

    scr.mini_map_aux.x = scr.pbar_area.x + 4;
    scr.mini_map_area.x = scr.pbar_area.x + 4;
    scr.mini_map.x = scr.mini_map_aux.x 
	    + ((scr.mini_map_aux.w - scr.mini_map.w) / 2);

    scr.help_button.x = scr.client_w - 56;
    scr.help_button.y = scr.client_h - 24;
    scr.results_button.x = scr.client_w - 2*56;
    scr.results_button.y = scr.client_h - 24;

    /* Complete refresh of the screen required here */
    screen_full_refresh ();
}
#endif

int 
mouse_in_rect (Rect* b, int x, int y)
{
    return ((x > b->x) && (x < b->x + b->w) && 
	    (y > b->y) && (y < b->y + b->h));
}


/* ---------------------------------------------------------------------- *
 * Main win functions
 * ---------------------------------------------------------------------- */
void
resize_main_win (int new_width, int new_height)
{
    Rect* mw = &scr.main_win;
    mw->w = new_width - 640 + MAIN_WIN_W;
    mw->w = (mw->w/16)*16;
    mw->h = new_height - 480 + MAIN_WIN_H;
    mw->h = (mw->h/16)*16;
    adjust_main_origin (main_screen_originx, main_screen_originy,0);
}


/* pixel_to_mappoint: convert a pixel value into a mappoint value.
   beware, values for mpx _can_ be negative */
int
pixel_to_mappoint(int px, int py, int *mpx, int *mpy)
{
    Rect* mw = &scr.main_win;
    *mpx = (px - mw->x) / 16;
    *mpy = (py - mw->y) / 16;
    *mpx += main_screen_originx;
    *mpy += main_screen_originy; 

    if ((px > (mw->x + mw->w)) || (py > (mw->y + mw->h)) 
	|| (px < mw->x) || (py < mw->y)) {
	return 0;
    }
  
    return 1;
}

/* same as above, only with the upper left corner of the visible map being
   1,1 */
int
pixel_to_winpoint(int px, int py, int *wpx, int *wpy)
{
    Rect* mw = &scr.main_win;
    *wpx = (px - mw->x) / 16;
    *wpy = (py - mw->y) / 16;


    if ((px > (mw->x + mw->w)) || (py > (mw->y + mw->h)) 
	|| (px < mw->x) || (py < mw->y)) {
	return 0;
    }
  
    return 1;
}

/* Adjust the main window origin, and refresh if necessary */
void
adjust_main_origin (int new_origin_x, int new_origin_y, int refresh)
{
    Rect* mw = &scr.main_win;

    if (new_origin_x < 1) {
	new_origin_x = 1;
    } else if (new_origin_x > WORLD_SIDE_LEN - mw->w / 16 - 1) {
	new_origin_x = WORLD_SIDE_LEN - mw->w / 16 - 1;
    }
    if (new_origin_y < 1) {
	new_origin_y = 1;
    } else if (new_origin_y > WORLD_SIDE_LEN - mw->h / 16 - 1) {
	new_origin_y = WORLD_SIDE_LEN - mw->h / 16 - 1;
    }

    if (new_origin_x == main_screen_originx
	&& new_origin_y == main_screen_originy)
    {
	return;
    }

    main_screen_originx = new_origin_x;
    main_screen_originy = new_origin_y;

    if (refresh) {
	request_main_screen ();
	hide_mouse ();
	refresh_main_screen ();
	redraw_mouse ();
    }
}

/* ---------------------------------------------------------------------- *
 * Button drawing functions
 * ---------------------------------------------------------------------- */
void 
draw_pause (int active)
{
    Rect* b = &scr.pause_button;
    if (active) {
	Fgl_putbox (b->x, b->y, 16, 16, pause_button1_on);
	Fgl_putbox (b->x + 16, b->y, 16, 16, pause_button2_on);
    } else {
	Fgl_putbox (b->x, b->y, 16, 16, pause_button1_off);
	Fgl_putbox (b->x + 16, b->y, 16, 16, pause_button2_off);
    }
}

void 
draw_slow (int active)
{
    Rect* b = &scr.slow_button;
    if (active) {
	Fgl_putbox (b->x, b->y, 16, 16, slow_button1_on);
	Fgl_putbox (b->x + 16, b->y, 16, 16, slow_button2_on);
    } else {
	Fgl_putbox (b->x, b->y, 16, 16, slow_button1_off);
	Fgl_putbox (b->x + 16, b->y, 16, 16, slow_button2_off);
    }
}

void 
draw_med (int active)
{
    Rect* b = &scr.med_button;
    if (active) {
	Fgl_putbox (b->x, b->y, 16, 16, med_button1_on);
	Fgl_putbox (b->x + 16, b->y, 16, 16, med_button2_on);
    } else {
	Fgl_putbox (b->x, b->y, 16, 16, med_button1_off);
	Fgl_putbox (b->x + 16, b->y, 16, 16, med_button2_off);
    }
}

void 
draw_fast (int active)
{
    Rect* b = &scr.fast_button;
    if (active) {
	Fgl_putbox (b->x, b->y, 16, 16, fast_button1_on);
	Fgl_putbox (b->x + 16, b->y, 16, 16, fast_button2_on);
    } else {
	Fgl_putbox (b->x, b->y, 16, 16, fast_button1_off);
	Fgl_putbox (b->x + 16, b->y, 16, 16, fast_button2_off);
    }
}

void 
draw_menu (void)
{
    /*    int menu_bg_color = white(20);*/
    /*    int menu_bg_color = TEXT_BG_COLOUR; */
    /*    int menu_bg_color = 14;*/
    /*    int menu_fg_color = TEXT_FG_COLOUR;*/
    int menu_bg_color = 80;
    int menu_fg_color = 226;

    Rect* b = &scr.menu_button;

    Fgl_fillbox (b->x, b->y, b->w, b->h, white(20));
    Fgl_fillbox (b->x+5, b->y+4, b->w-10, b->h-5, menu_bg_color);
    
    Fgl_hline (b->x+5, b->y+4, b->x + b->w-5, white(8));
    Fgl_line (b->x+5, b->y+4, b->x+5, b->y + b->h-2, white(8));
    Fgl_hline (b->x+5, b->y + b->h-2, b->x + b->w-5, white(8));
    Fgl_line (b->x + b->w-5, b->y+4, b->x + b->w-5, b->y + b->h-2, white(8));

    Fgl_setfontcolors (menu_bg_color,menu_fg_color);
    Fgl_write (b->x + 12, b->y + 9, _("Menu"));
    Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
}

void 
draw_help (void)
{
    /*    int menu_bg_color = white(20);*/
    /*    int menu_bg_color = TEXT_BG_COLOUR; */
    /*    int menu_bg_color = 14;*/
    /*    int menu_fg_color = TEXT_FG_COLOUR;*/
    int menu_bg_color = 80;
    int menu_fg_color = 226;

    Rect* b = &scr.help_button;

    Fgl_fillbox (b->x, b->y, b->w, b->h, white(20));
    Fgl_fillbox (b->x+5, b->y+4, b->w-10, b->h-5, menu_bg_color);
    
    Fgl_hline (b->x+5, b->y+4, b->x + b->w-5, white(8));
    Fgl_line (b->x+5, b->y+4, b->x+5, b->y + b->h-2, white(8));
    Fgl_hline (b->x+5, b->y + b->h-2, b->x + b->w-5, white(8));
    Fgl_line (b->x + b->w-5, b->y+4, b->x + b->w-5, b->y + b->h-2, white(8));

    Fgl_setfontcolors (menu_bg_color,menu_fg_color);
    Fgl_write (b->x + 12, b->y + 9, _("Help"));
    Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
}

void 
draw_results (void)
{
    /*    int menu_bg_color = white(20);*/
    /*    int menu_bg_color = TEXT_BG_COLOUR; */
    /*    int menu_bg_color = 14;*/
    /*    int menu_fg_color = TEXT_FG_COLOUR;*/
    int menu_bg_color = 80;
    int menu_fg_color = 226;

    Rect* b = &scr.results_button;

    Fgl_fillbox (b->x, b->y, b->w, b->h, white(20));
    Fgl_fillbox (b->x+5, b->y+4, b->w-10, b->h-5, menu_bg_color);
    
    Fgl_hline (b->x+5, b->y+4, b->x + b->w-5, white(8));
    Fgl_line (b->x+5, b->y+4, b->x+5, b->y + b->h-2, white(8));
    Fgl_hline (b->x+5, b->y + b->h-2, b->x + b->w-5, white(8));
    Fgl_line (b->x + b->w-5, b->y+4, b->x + b->w-5, b->y + b->h-2, white(8));

    Fgl_setfontcolors (menu_bg_color,menu_fg_color);
    Fgl_write (b->x + 12, b->y + 9, _("Stats"));
    Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
}

#if defined (commentout)
void 
draw_load (void)
{
    Rect* b = &scr.load_button;
    Fgl_putbox (b->x, b->y, 32, 32, load_button_graphic);
}

void 
draw_save (void)
{
    Rect* b = &scr.save_button;
    Fgl_putbox (b->x, b->y, 32, 32, save_button_graphic);
}

void 
draw_quit (void)
{
    Rect* b = &scr.quit_button;
    Fgl_putbox (b->x, b->y, 32, 32, quit_button_graphic);
}

void 
draw_help (void)
{
    Rect* b = &scr.help_button;
    Fgl_putbox (b->x, b->y, 32, 32, help_button_graphic);
}

void 
draw_results (void)
{
    Rect* b = &scr.results_button;
    Fgl_putbox (b->x, b->y, 16, 16, results_button1);
    Fgl_putbox (b->x + 16, b->y, 16, 16, results_button2);
}
#endif

void
draw_select_button_graphic (int button, char *graphic)
{
    Rect* b = &scr.select_buttons;
    int x, y, xx, yy;
    if (button < NUMOF_SELECT_BUTTONS_DOWN) {
	x = 8;
	y = 8 + (button * 24);
    } else {
	x = 8 + 24;
	y = 8 + ((button - NUMOF_SELECT_BUTTONS_DOWN) * 24);
    }

    Fgl_putbox (x + b->x, y + b->y, 16, 16, graphic);
    unhighlight_select_button (button);
    hide_mouse ();
    if (select_button_tflag[button] == 0)
    {
	for (yy = -3; yy < 19; yy++)
	    for (xx = -3; xx < 19; xx += 2)
		Fgl_setpixel (x + xx + (yy % 2) + b->x,
			      y + yy + b->y, white (15));
    }
    redraw_mouse ();
}


/* ---------------------------------------------------------------------- *
 * Button click functions
 * ---------------------------------------------------------------------- */
void
select_fast (void)
{
    hide_mouse ();
    pause_flag = 0;
    draw_pause (0);
    slow_flag = 0;
    draw_slow (0);
    med_flag = 0;
    draw_med (0);
    fast_flag = 1;
    draw_fast (1);
    redraw_mouse ();
}

void
select_medium (void)
{
    hide_mouse ();
    pause_flag = 0;
    draw_pause (0);
    slow_flag = 0;
    draw_slow (0);
    med_flag = 1;
    draw_med (1);
    fast_flag = 0;
    draw_fast (0);
    redraw_mouse ();
}

void
select_slow (void)
{
    hide_mouse ();
    pause_flag = 0;
    draw_pause (0);
    slow_flag = 1;
    draw_slow (1);
    med_flag = 0;
    draw_med (0);
    fast_flag = 0;
    draw_fast (0);
    redraw_mouse ();
}

void
select_pause (void)
{
    if (pause_flag) {
	/* unpause it */
	if (fast_flag)
	    select_fast ();
	else if (med_flag)
	    select_medium ();
	else if (slow_flag)
	    select_slow ();
	else
	    select_medium ();
    } else {
	/* pause it */
	hide_mouse ();
	pause_flag = 1;
	draw_pause (1);
	draw_slow (0);
	draw_med (0);
	draw_fast (0);
	redraw_mouse ();
    }
}

/* ---------------------------------------------------------------------- *
 * Mini map button functions
 * ---------------------------------------------------------------------- */
#if defined (commentout)
void
draw_ms_button (Rect* b, char* graphic)
{
    Fgl_putbox (b->x, b->y, 16, 16, graphic);
}
#endif
void
draw_ms_button (char* graphic)
{
    Rect* mma = &scr.mini_map_aux;
    Fgl_putbox (mma->x + 4, mma->y, 16, 16, graphic);
}

void
draw_ms_text (char* txt)
{
    Rect* mma = &scr.mini_map_aux;
    Fgl_write (mma->x + 28, mma->y + 4, txt);
}

void
draw_small_bezel (int x, int y, int w, int h, int colour)
{
    int i;
    for (i = 1; i < 4; i++) {
	Fgl_hline (x - 1 - i, y - 1 - i, x + w + i, colour + 16);
	Fgl_line (x - 1 - i, y - 1 - i, x - 1 - i, y + h + i, colour + 14);
	Fgl_hline (x - 1 - i, y + h + i, x + w + i, colour + 22);
	Fgl_line (x + w + i, y - 1 - i, x + w + i, y + h + i, colour + 24);
    }
}

void 
draw_bezel (Rect r, short width, int color)
{
  int i;
  int c;
  for (i = 0; i < width; i++)
    {
      c = color + (width - i) * 2;
      Fgl_hline (r.x + i, r.y + i, r.x + r.w - i - 1, c);
      Fgl_hline (r.x + i, r.y + r.h - i - 1, r.x + r.w - i - 1, c);
      Fgl_line (r.x + i, r.y + i, r.x + i, r.y + r.h - i - 1, c);
      Fgl_line (r.x + r.w - i - 1, r.y + i, r.x + r.w - i - 1, 
		r.y + r.h - i - 1, c);
    }
}
