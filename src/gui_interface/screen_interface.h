/* ---------------------------------------------------------------------- *
 * screen_interface.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __screen_interface_h__
#define __screen_interface_h__

//#include "cliglobs.h"

#define MONTHGRAPH_STYLE_ECONOMY      1
#define MONTHGRAPH_STYLE_SUSTAIN      2

#define MONTHGRAPH_STYLE_MIN  MONTHGRAPH_STYLE_ECONOMY
#define MONTHGRAPH_STYLE_MAX  MONTHGRAPH_STYLE_SUSTAIN


void HandleError (const char *, int);


extern unsigned char main_font[2048];
extern unsigned char start_font1[2048];
extern unsigned char start_font2[4096];
extern unsigned char start_font3[4096];

extern int monthgraph_style;


int getMainWindowWidth();
int getMainWindowHeight();


/* This is on in screen_full_refresh, used in *_refresh() */
extern char screen_refreshing;

void draw_cb_box (int row, int col, int checked);
int ask_launch_rocket_click (int x, int y);
int ask_launch_rocket_now (int x, int y);
void display_rocket_result_dialog (int result);
void draw_background (void);
void screen_full_refresh (void);
void init_fonts (void);
void initialize_monthgraph (void);
void rotate_mini_screen (void);
void advance_mps_style (void);
void update_main_screen (int full_refresh);
void connect_transport_main_screen (void);
void refresh_main_screen (void);
void refresh_mps (void);
void advance_monthgraph_style (void);
void refresh_monthgraph (void);
void draw_small_yellow_bezel (int x, int y, int h, int w);
void init_pbars (void);
void mini_screen_help (void);
void print_time_for_year (void);
void rotate_main_screen (void);
void screen_setup (void);
void refresh_main_screen (void);
/* Miniscreen */
void init_mini_map_mouse(void);
void mini_map_handler(int x, int y, int button);
void mini_aux_handler(int x, int y, int button);

#define CB_SPACE 20 // space between checkbuttons for market/port  // AL1 : unused in 2.x

#endif	/* __screen_h__ */


/** @file gui_interface/screen_interface.h */

