/* ---------------------------------------------------------------------- *
 * mouse.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __mouse_h__
#define __mouse_h__

#include "cliglobs.h"

#define NUM_BUTTONS 3

// Lin City mouse defines
#define LC_MOUSE_LEFTBUTTON	1
#define LC_MOUSE_MIDDLEBUTTON	2
#define LC_MOUSE_RIGHTBUTTON	3
#if defined (commentout)
#define LC_MOUSE_LEFTBUTTON	0
#define LC_MOUSE_MIDDLEBUTTON	1
#define LC_MOUSE_RIGHTBUTTON	2
#endif

#define LC_MOUSE_RELEASE        0x10
#define LC_MOUSE_PRESS          0x20

/* 
mappoint_[xy] are non-zero if the button was pressed inside the main window.
x and y are pixel coordinates.
r_* are values for when the button was released.
*/
struct mouse_button_struct {
  int mappoint_x, mappoint_y;
  int x, y;
  int r_mappoint_x, r_mappoint_y;
  int r_x, r_y;
  int pressed;
};

void cs_mouse_handler (int enc_button, int dx, int dy);
void mouse_setup (void);
void cs_mouse_repeat (void);
void cs_mouse_handler (int, int, int);
void move_mouse (int, int); 
void do_mouse_main_win (int, int, int);
int cmp(int, int);
void drag_screen (void);

/* WCK: These were in lin-city.h */

void no_credit_build_msg (int selected_type);
extern void hide_mouse (void);
extern void redraw_mouse (void);
extern void draw_square_mouse (int, int, int);
extern void hide_square_mouse (void);
extern void redraw_square_mouse (void);
extern void draw_normal_mouse (int, int);
extern void hide_normal_mouse (void);
extern void redraw_normal_mouse (void);
extern void do_mouse_select_buttons (int, int, int);
extern void do_select_button (int, int);
extern void highlight_select_button (int);
extern void unhighlight_select_button (int);
extern void do_mouse_other_buttons (int, int, int);
extern void bulldoze_area (int, int);
extern void fire_area (int, int);
extern void do_market_cb_mouse (int, int);
extern void do_port_cb_mouse (int, int);
extern void do_db_mouse (int, int);
extern void do_db_okmouse (int, int);
extern void connect_rivers (void);
extern int is_real_river (int, int);
extern void select_pause (void);
extern void select_fast (void);
extern void select_medium (void);
extern void select_slow (void);
extern void choose_residence (void);
extern void do_multi_transport (int, int, int);
extern int mt_draw (int, int, int); /* wcoreyk */

#endif
