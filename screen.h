/* ---------------------------------------------------------------------- *
 * screen.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __screen_h__
#define __screen_h__

#include "cliglobs.h"

#define MONTHGRAPH_STYLE_ECONOMY      1
#define MONTHGRAPH_STYLE_SUSTAIN      2

#define MONTHGRAPH_STYLE_MIN  MONTHGRAPH_STYLE_ECONOMY
#define MONTHGRAPH_STYLE_MAX  MONTHGRAPH_STYLE_SUSTAIN


extern unsigned char main_font[2048];
extern unsigned char start_font1[2048];
extern unsigned char start_font2[4096];
extern unsigned char start_font3[4096];

int monthgraph_style;
int mps_global_style;


char* current_month (int current_time);
void draw_cb_box (int row, int col, int checked);
int ask_launch_rocket_now (int x, int y);
void display_rocket_result_dialog (int result);
void draw_background (void);
void screen_full_refresh (void);
void init_fonts (void);
void initialize_monthgraph (void);
void rotate_mini_screen (void);
void advance_mps_style (void);
void refresh_mps (void);
void advance_monthgraph_style (void);
void refresh_monthgraph (void);
void draw_small_yellow_bezel (int x, int y, int h, int w);
void init_pbars (void);

#define CB_SPACE 20 // space between checkbuttons for market/port

#endif	/* __screen_h__ */

