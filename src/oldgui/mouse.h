/* ---------------------------------------------------------------------- *
 * mouse.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __mouse_h__
#define __mouse_h__

#include "cliglobs.h"

#define NUM_BUTTONS 3

#define LC_MOUSE_LEFTBUTTON	1
#define LC_MOUSE_MIDDLEBUTTON	2
#define LC_MOUSE_RIGHTBUTTON	3

/* Mouse event flags */
#define LC_MOUSE_RELEASE        0x10
#define LC_MOUSE_PRESS          0x20

/* Public interface variables and structures */

/* Mouse registry stuff.  */

struct mouse_handle_struct
{
    Rect * r;
    void (* handler)(int, int, int);
    struct mouse_handle_struct * prev;
    struct mouse_handle_struct * next;
};

typedef struct mouse_handle_struct Mouse_Handle;

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

/* Public functions */

void mouse_setup (void);
void init_mouse_registry();
int mouse_handle_click(int x, int y, int button); 

Mouse_Handle * mouse_register(Rect * r, void (*handler)(int, int, int));
void mouse_unregister(Mouse_Handle * mhandle);

void cs_mouse_handler (int enc_button, int dx, int dy);

void cs_mouse_handler (int, int, int);
void move_mouse (int, int); 
void do_mouse_main_win (int, int, int);

void drag_screen (void);

/* Private functions */
int cmp(int, int);

/* WCK: These were in lin-city.h */

void no_credit_build_msg (int selected_type);
void hide_mouse (void);
void redraw_mouse (void);
void draw_square_mouse (int, int, int);
void hide_square_mouse (void);
void redraw_square_mouse (void);
void draw_normal_mouse (int, int);
void hide_normal_mouse (void);
void redraw_normal_mouse (void);
void do_mouse_select_buttons (int, int, int);
void do_select_button (int, int);
void highlight_select_button (int);
void unhighlight_select_button (int);
void do_mouse_other_buttons (int, int, int);
void bulldoze_area (int, int);
void fire_area (int, int);
void do_market_cb_mouse (int, int);
void do_port_cb_mouse (int, int);
int is_real_river (int, int);
void select_pause (void);
void select_fast (void);
void select_medium (void);
void select_slow (void);
void choose_residence (void);
void do_multi_transport (int, int, int);
int mt_draw (int, int, int); /* wcoreyk */
void draw_module_cost (int grp);




#endif

