/* ---------------------------------------------------------------------- *
 * mouse.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include "lcconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include "lcstring.h"
#include "common.h"
#include "lctypes.h"
#include "lin-city.h"
#include "mouse.h"
#include "engglobs.h"
#include "clistubs.h"
#include "cliglobs.h"
#include "engine.h"
#include "screen.h"
#include "climsg.h"
#include "mps.h"
#include "fileutil.h"
#include "lchelp.h"
#include "pbar.h"

#undef DEBUG_MT_CODE

extern Update_Scoreboard update_scoreboard;

/* ---------------------------------------------------------------------- *
 * Private global variables
 * ---------------------------------------------------------------------- */
static struct mouse_button_struct buttons[NUM_BUTTONS];
static int mt_cost;
static int mt_grp;
static char mt_name[20];
static short mouse_buffer_fresh = 0;

void check_bulldoze_area (int x, int y);


/* GCS -- we should get rid of mouse repeat. */
/* WCK -- agreed */
#ifdef MOUSE_REPEAT
void
cs_mouse_repeat (void)
{
    /* called from main loop when cs_mouse_button==LC_MOUSE_LEFTBUTTON */
    if (cs_mouse_button_repeat <= real_time)
	cs_mouse_handler (cs_mouse_button, 0, 0);
    /* cs_mouse_button_repeat zeroed by handler */
}
#endif

/* ---------------------------------------------------------------------- *
 * cs_mouse_handler
 * --
 * If the event was a mouse click or mouse release, enc_button contains 
 * the button identifier (e.g. LC_MOUSE_LEFTBUTTON) or'd with the 
 * action identifier (e.g. LC_MOUSE_RELEASE).  If the event was just a 
 * mouse move, then enc_button is 0.  You cannot specify multiple 
 * mouse buttons clicked at the same time using this interface.
 * ---------------------------------------------------------------------- */
void
cs_mouse_handler (int enc_button, int dx, int dy)
{
    int x, y;
    int group = 0;
    int button = enc_button & ~LC_MOUSE_RELEASE & ~LC_MOUSE_PRESS;
    int button_pressed = enc_button & LC_MOUSE_PRESS;
    int button_released = enc_button & LC_MOUSE_RELEASE;
    int button_idx = button - 1;

    x = cs_mouse_x;
    y = cs_mouse_y;

#if defined (SVGALIB)
    cs_mouse_x += dx * MOUSE_SENSITIVITY;
    cs_mouse_y += dy * MOUSE_SENSITIVITY;
#else
    cs_mouse_x += dx;
    cs_mouse_y += dy;
#endif

#if defined (SVGALIB)
    if (cs_mouse_x >= cs_mouse_xmax)
	cs_mouse_x = cs_mouse_xmax;
    if (cs_mouse_y >= cs_mouse_ymax)
	cs_mouse_y = cs_mouse_ymax;
    if (cs_mouse_x < 0)
	cs_mouse_x = 0;
    if (cs_mouse_y < 0)
	cs_mouse_y = 0;
#endif
 
    if (cs_mouse_x != x || cs_mouse_y != y)
	move_mouse (cs_mouse_x, cs_mouse_y);
    x = cs_mouse_x;
    y = cs_mouse_y;
#ifdef DEBUG_MOUSE
    printf ("button=%d enc=%d %d%d%d\n", button, enc_button, 
	    buttons[0].pressed, buttons[1].pressed, buttons[2].pressed);
    printf ("cs_mouse_button=%d  button=%d\n", cs_mouse_button, button);
#endif

    /* see if we are all the way up */
    if (!mouse_initialized)
	return;

    group = get_group_of_type(selected_type);
    
    /* button press */
    if (button_pressed) {

	/* maintain button press status */

	/* WCK: The button argument now comes encoded from lcx11, with release
	   events being the button number | 16.  I don't know of 
	   anyone who has a 16 button mouse, so this should be 
	   safe for a while.  The new
	   argument is enc_button, with button holding the value of the
	   pressed button, or 0 if a button was released.  The coordinates of
	   the presses and releases are stored in buttons */

	pixel_to_mappoint(cs_mouse_x, cs_mouse_y, 
			  &buttons[button_idx].mappoint_x, 
			  &buttons[button_idx].mappoint_y);
	buttons[button_idx].x = cs_mouse_x;
	buttons[button_idx].y = cs_mouse_y;
	buttons[button_idx].pressed = 1;

#ifdef DEBUG_MOUSE
	printf ("button %d pressed at %d , %d\t mappoint %d , %d\n",
		button, cs_mouse_x, cs_mouse_y, buttons[button_idx].r_mappoint_x,
		buttons[button_idx].r_mappoint_y);
#endif


	/* wcoreyk:
	   maybe we should wait for button releases when selecting 
	   something with the mouse.  When the release comes, 
	   we could compare press and
	   release coordinates, and use that to determine our action.  This
	   would allow an escape mechanism for placing expensive items.  e.g:
	   when placing a rocket you accidentally click 1 line off from where 
	   you really want it.  If you held the mouse button, you could move
	   the rocket to where you want it.  Or, moving the mouse off of the
	   position could simply cancel the build.  More interesting side
	   effects could be the ability to drop a menu down for residential 
	   units, or any other groups of units that could fit together 
	   appropriately.  Of course, all of this could be re-inventing the
	   wheel, but I feel like I should invent it just once and see how
	   well mine rolls. */

	if (!mouse_handle_click(x, y, button)) {
	    switch (button) {
	    case LC_MOUSE_LEFTBUTTON:
#ifdef MOUSE_REPEAT
		cs_mouse_button_repeat = real_time + 500;
#endif
	
		if (market_cb_flag) {
		    do_market_cb_mouse (x, y);
		    break;
		}
		else if (port_cb_flag) {
		    do_port_cb_mouse (x, y);
		    break;
		}
		else if (help_flag) {
		    do_help_mouse (x, y, button);
		    break;
		}
		else if (prefs_flag) {
		    do_prefs_mouse (x, y, button);
		    break;
		}
		else if (db_flag) {
		    do_db_mouse (x, y);
		    break;
		}
		else if (db_okflag) {
		    do_db_okmouse (x, y);
		    break;
		}
		else if (load_flag || save_flag) 
		    return;

		/* This is the main screen */
		if (mouse_in_rect(&scr.main_win,x,y)) {
		    do_mouse_main_win(x, y, button);
		    refresh_main_screen ();
		    break;
		}

		/* Other points too */
		do_mouse_other_buttons(x, y, button);
	
		break;
	
	    case LC_MOUSE_RIGHTBUTTON:
	    case LC_MOUSE_MIDDLEBUTTON:
		/* GCS FIX: This is my fix for right clicks on 
		   main screen during yn_dialogs causing dialog to 
		   be overwritten by screen content (similar effect
		   for market_cb overwritten by mps).  This fix could
		   be better, but will be better to confirm behavior
		   for X version before deciding final fix.
		*/
		if (market_cb_flag) {
		    // should dismiss cb?
		    break;
		}
		else if (port_cb_flag) {
		    // should dismiss cb?
		    break;
		}
		else if (help_flag) {
		    // do_help_mouse (x, y, button);   maybe should??
		    break;
		}
		else if (prefs_flag) {
		    break;
		}
		else if (db_flag) {
		    break;
		}
		else if (db_okflag) {
		    break;
		}
		else if (load_flag || save_flag) 
		    return;

		/* This is the main screen */
		if (mouse_in_rect(&scr.main_win,x,y)) {
		    do_mouse_main_win(x, y, button);
		    refresh_main_screen ();
		    break;
		}

		/* Other points too */
		do_mouse_other_buttons(x, y, button);

		break;

	    default: 
		printf("Unknown mouse button in cs_mouse_handler\n");
	    }
	} /* mouse_handle_click couldn't. */
    } else if (button_released) { 
	button = enc_button - 16; /* probably shouldn't use this temporarily */
      
	pixel_to_mappoint(cs_mouse_x, cs_mouse_y, 
			  &buttons[button_idx].r_mappoint_x, 
			  &buttons[button_idx].r_mappoint_y);
      
#ifdef DEBUG_MOUSE
	printf ("button %d released at %d , %d\t mappoint %d , %d\n",
		button, cs_mouse_x, cs_mouse_y, buttons[button_idx].r_mappoint_x,
		buttons[button_idx].r_mappoint_y);
#endif

	buttons[button_idx].r_x = cs_mouse_x;
	buttons[button_idx].r_y = cs_mouse_y;
	buttons[button_idx].pressed = 0;
      
	switch (button) {
	case LC_MOUSE_LEFTBUTTON:
#if defined (DEBUG_MT_CODE)
	    printf("calling mt_draw(MT_SUCCESS)\n");
#endif
	    mt_draw(cs_mouse_x, cs_mouse_y, MT_SUCCESS);
	    break;
	case LC_MOUSE_RIGHTBUTTON:
	    break;
	case LC_MOUSE_MIDDLEBUTTON:
	    break;
	default: 
	    break;
	};
	button = 0; /* backwards compatibility */      

	/* mouse moved */
    } else {
	if (buttons[LC_MOUSE_LEFTBUTTON-1].pressed 
	    && GROUP_IS_TRANSPORT(group))
	{
	    mt_draw(cs_mouse_x, cs_mouse_y, MT_CONTINUE);
	}
    }
    cs_mouse_button = button;
}

void
move_mouse (int x, int y)
{
    Rect* mw = &scr.main_win;
    int size;
    int grp;

    if( (grp = get_group_of_type(selected_type)) < 0 ) return;

#ifdef DEBUGMOUSE
    printf ("In move_mouse()\n");
#endif

    size = (main_groups[grp].size) * 16;

    /* GCS: we don't check for load_flag/save_flag because these guys
       set db_flag = 1 */
    if (mouse_in_rect(&scr.main_win,x,y)
	&& market_cb_flag == 0 && port_cb_flag == 0 
	&& prefs_flag == 0 && help_flag == 0 
	&& db_flag == 0 && db_okflag == 0)
    {
	int mwoffset_x;
	int mwoffset_y;
	if (x > (mw->x + mw->w) - size)
	    x = (mw->x + mw->w) - size;
	if (y > (mw->y + mw->h) - size)
	    y = (mw->y + mw->h) - size;
	mwoffset_x = mw->x % 16;
	mwoffset_y = mw->y % 16;
	x -= mwoffset_x;
	y -= mwoffset_y;
	x &= 0xff0;
	y &= 0xff0;
	x += mwoffset_x;
	y += mwoffset_y;

	hide_mouse ();
	mouse_hide_count--;
	draw_square_mouse (x, y, size);
    }
    else
    {
#ifdef DEBUGMOUSE
	printf ("mox=%d moy=%d x=%d y=%d\n", mox, moy, x, y);
#endif
	hide_mouse ();
	mouse_hide_count--;
	draw_normal_mouse (x, y);
	if (mappoint_stats_flag != 0 && market_cb_flag == 0
	    && port_cb_flag == 0)
	{
	    mappoint_stats_flag = 0;
	    unrequest_mappoint_stats ();
#if defined (commentout)
	    draw_diffgraph ();
#endif
	    update_scoreboard.mps = 1;
	}
    }
#ifdef DEBUGMOUSE
    printf ("Got to the end of move_mouse()\n");
#endif
}

void
hide_mouse (void)
{
    mouse_hide_count++;
    if (mouse_hide_count == 1) {
	if (mouse_type == MOUSE_TYPE_SQUARE)
	    hide_square_mouse ();
	else
	    hide_normal_mouse ();
    }
}

void
redraw_mouse (void)
{
    mouse_hide_count--;
    if (mouse_hide_count > 0)
	return;
    mouse_hide_count = 0;
    if (mouse_type == MOUSE_TYPE_SQUARE)
	redraw_square_mouse ();
    else
	redraw_normal_mouse ();
}

void
draw_square_mouse (int x, int y, int size)	/* size is pixels */
{
    if (mouse_type == MOUSE_TYPE_NORMAL) {
	hide_normal_mouse ();
	mouse_type = MOUSE_TYPE_SQUARE;
	kmouse_val = 16;
    }
    omx = x;
    omy = y;
#if defined (WIN32)
    cs_square_mouse_visible = 1;
    RefreshArea (omx - 2, omy - 2, omx + size + 1, omy + size + 1);
#else
    Fgl_getbox (x - 2, y - 2, size + 4, 2, under_square_mouse_pointer_top);
    Fgl_getbox (x - 2, y, 2, size, under_square_mouse_pointer_left);
    Fgl_getbox (x + size, y, 2, size, under_square_mouse_pointer_right);
    Fgl_getbox (x - 2, y + size, size + 4, 2, under_square_mouse_pointer_bottom);
    mouse_buffer_fresh = 1;

    Fgl_hline (x - 2, y - 2, x + size + 1, yellow (31));
    Fgl_hline (x - 1, y - 1, x + size, blue (31));
    Fgl_hline (x - 2, y + size + 1, x + size + 1, yellow (31));
    Fgl_hline (x - 1, y + size, x + size, blue (31));
    Fgl_line (x - 2, y - 1, x - 2, y + size + 1, yellow (31));
    Fgl_line (x - 1, y, x - 1, y + size, blue (31));
    Fgl_line (x + size + 1, y - 1, x + size + 1, y + size + 1, yellow (31));
    Fgl_line (x + size, y, x + size, y + size, blue (31));
#endif
}

void
hide_square_mouse (void)
{
    int size;
    int grp;

    if( (grp = get_group_of_type(selected_type)) < 0 ) return;

    size = (main_groups[grp].size) * 16;
#if defined (WIN32)
    cs_square_mouse_visible = 0;
    RefreshArea (omx - 3, omy - 3, omx + size + 2, omy + size + 2);
#else
    if (mouse_buffer_fresh) {
      Fgl_putbox (omx - 2, omy - 2, size + 4, 2, under_square_mouse_pointer_top);
      Fgl_putbox (omx - 2, omy, 2, size, under_square_mouse_pointer_left);
      Fgl_putbox (omx + size, omy, 2, size, under_square_mouse_pointer_right);
      Fgl_putbox (omx - 2, omy + size, size + 4, 2, 
		  under_square_mouse_pointer_bottom);
      mouse_buffer_fresh = 0;
    } else {
      //      printf ("Mouse buffer stale in hide_mouse!  Not putting back!\n");
    }
#endif
}

void
redraw_square_mouse (void)
{
    int size;
    int grp;

    if ((grp = get_group_of_type(selected_type)) < 0 ) return;

    size = (main_groups[grp].size) * 16;

#if defined (WIN32)
    cs_square_mouse_visible = 1;
    RefreshArea (omx - 2, omy - 2, omx + size + 1, omy + size + 1);
#else
    Fgl_getbox (omx - 2, omy - 2, size + 4, 2, under_square_mouse_pointer_top);
    Fgl_getbox (omx - 2, omy, 2, size, under_square_mouse_pointer_left);
    Fgl_getbox (omx + size, omy, 2, size, under_square_mouse_pointer_right);
    Fgl_getbox (omx - 2, omy + size, size + 4, 2, under_square_mouse_pointer_bottom);
    mouse_buffer_fresh = 1;

    Fgl_hline (omx - 2, omy - 2, omx + size + 1, yellow (31));
    Fgl_hline (omx - 1, omy - 1, omx + size, blue (31));
    Fgl_hline (omx - 2, omy + size + 1, omx + size + 1, yellow (31));
    Fgl_hline (omx - 1, omy + size, omx + size, blue (31));
    Fgl_line (omx - 2, omy - 1, omx - 2, omy + size + 1, yellow (31));
    Fgl_line (omx - 1, omy, omx - 1, omy + size, blue (31));
    Fgl_line (omx + size + 1, omy - 1, omx + size + 1, omy + size + 1, yellow (31));
    Fgl_line (omx + size, omy, omx + size, omy + size, blue (31));
#endif
}

void
draw_normal_mouse (int x, int y)
{
    if (mouse_type == MOUSE_TYPE_SQUARE) {
	hide_square_mouse ();
	mouse_type = MOUSE_TYPE_NORMAL;
	kmouse_val = 8;
    }
#if defined (SVGALIB)
    Fgl_getbox (x, y, 8, 8, under_mouse_pointer);
    if (x > 631 || y > 471) {
	Fgl_enableclipping ();
	Fgl_setclippingwindow (0, 0, 639, 479);
	Fgl_putbox (x, y, 8, 8, mouse_pointer);
	Fgl_disableclipping ();
    } else {
	Fgl_putbox (x, y, 8, 8, mouse_pointer);
    }
#endif
    mox = x;
    moy = y;
}

void
hide_normal_mouse (void)
{
#if defined (SVGALIB)
    if (mox > 631 || moy > 471) {
	Fgl_enableclipping ();
	Fgl_setclippingwindow (0, 0, 639, 479);
	Fgl_putbox (mox, moy, 8, 8, under_mouse_pointer);
	Fgl_disableclipping ();
    } else {
	Fgl_putbox (mox, moy, 8, 8, under_mouse_pointer);
    }
#endif
}

void
redraw_normal_mouse (void)
{
#if defined (SVGALIB)
    if (mox > 631 || moy > 471) {
	Fgl_enableclipping ();
	Fgl_setclippingwindow (0, 0, 639, 479);
	Fgl_getbox (mox, moy, 8, 8, under_mouse_pointer);
	Fgl_putbox (mox, moy, 8, 8, mouse_pointer);
	Fgl_disableclipping ();
    } else {
	/* may have changed */
	Fgl_getbox (mox, moy, 8, 8, under_mouse_pointer);
	Fgl_putbox (mox, moy, 8, 8, mouse_pointer);
    }
#endif
}



void 
do_mouse_select_buttons (int rawx, int rawy, int mbutton)
{
    Rect* sbw = &scr.select_buttons;
    int x, y;
    int button = -1;
    int rows;
    int row, col;
    x = rawx - sbw->x;
    y = rawy - sbw->y;

    rows = (sbw->h / (SELECT_BUTTON_INTERVAL));

#ifdef DEBUG_SELECT_BUTTONS    
    printf ("do_mouse_select_buttons() x=%d y=%d\n", x, y);
#endif

    row = (y / (SELECT_BUTTON_INTERVAL));
    if ((y % SELECT_BUTTON_INTERVAL) <= 8) 
      return;  /* in the gap */
    
    col = (x / (SELECT_BUTTON_INTERVAL));
    if ((x % SELECT_BUTTON_INTERVAL) <= 8) 
      return;
    
    button = row + (rows * col);

#ifdef DEBUG_SELECT_BUTTONS    
    printf("rows %d\n",rows);

    printf("do_mouse_select_buttons: row %d\tcol %d\tbutton %d\n",
	   row, col, button);
#endif    
    do_select_button (button, mbutton);

}

void
draw_module_cost (int grp)
{
  Rect* b = &scr.select_message;
  char s[100];

  get_group_cost(grp);
  if (grp == GROUP_BARE) 
    sprintf (s, "Bulldoze - cost POA");
  else
    sprintf (s, "%s %d  Bulldoze %d", main_groups[grp].name,
	     get_group_cost(grp), main_groups[grp].bul_cost);

#ifdef USE_EXPANDED_FONT
    Fgl_fillbox (SELECT_BUTTON_MESSAGE_X, SELECT_BUTTON_MESSAGE_Y,
		 44 * 8, 8, 0);
    Fgl_fillbox (SELECT_BUTTON_MESSAGE_X, SELECT_BUTTON_MESSAGE_Y,
		 44 * 8, 8, blue (10));
    gl_setwritemode (WRITEMODE_MASKED | FONT_EXPANDED);
#else
    Fgl_fillbox (b->x, b->y, 42 * 8, 8, TEXT_BG_COLOUR);
#endif

  Fgl_write (b->x, b->y, s);
}

void
do_select_button (int button, int mbutton)
{
    short grp;
    if (select_button_tflag[button] == 0 && mbutton != LC_MOUSE_RIGHTBUTTON) {
	ok_dial_box ("not_enough_tech.mes", BAD, 0L);
	return;
    }
    if (mbutton == LC_MOUSE_RIGHTBUTTON 
	|| select_button_help_flag[button] == 0)
    {
	activate_help (select_button_help[button]);
	if (mbutton != LC_MOUSE_RIGHTBUTTON)
	    select_button_help_flag[button] = 1;
	if (mbutton == LC_MOUSE_RIGHTBUTTON)
	    return;
    }
    selected_button = button;
    unhighlight_select_button (old_selected_button);
    highlight_select_button (button);
    old_selected_button = button;
    selected_type = select_button_type[button];
    if (selected_type == CST_RESIDENCE_LL) {
	choose_residence ();
    }
    grp = get_group_of_type(selected_type);
    if (grp < 0) return; /* XXX: WCK: When is this hit?  Should it hurt? */

#ifdef LC_X11
    if (grp == GROUP_BARE) 
	XDefineCursor (display.dpy, display.win, pirate_cursor);
    else
	XDefineCursor (display.dpy, display.win, None);
#endif

    draw_module_cost(grp);

#ifdef USE_EXPANDED_FONT
    gl_setwritemode (WRITEMODE_OVERWRITE | FONT_EXPANDED);
#endif
    if (selected_type == CST_GREEN) {
	draw_main_window_box (red (8));
    } else {
	draw_main_window_box (green (8));
	monument_bul_flag = 0;
	river_bul_flag = 0;
    }
}

void
highlight_select_button (int button)
{
    Rect* sbw = &scr.select_buttons;
    int x, y, q;
    if (button < NUMOF_SELECT_BUTTONS_DOWN)
    {
	x = 8 + sbw->x;
	y = 8 + (button * 24) + sbw->y;
    }
    else
    {
	x = 8 + 24 + sbw->x;
	y = 8 + ((button - NUMOF_SELECT_BUTTONS_DOWN) * 24) + sbw->y;
    }
    hide_mouse ();
    for (q = 0; q < 3; q++)
    {
	Fgl_hline (x - 1 - q, y - 1 - q, x + 16 + q, yellow (16 + q * 4));
	Fgl_hline (x - 1 - q, y + q + 16, x + 16 + q, yellow (16 + q * 4));
	Fgl_line (x - 1 - q, y - 1 - q, x - 1 - q, y + q + 16, yellow (16 + q * 4));
	Fgl_line (x + 16 + q, y - 1 - q, x + 16 + q, y + q + 16, yellow (16 + q * 4));
    }
    redraw_mouse ();
}

void
unhighlight_select_button (int button)
{
    Rect* sbw = &scr.select_buttons;
    int x, y, q;
    if (button < NUMOF_SELECT_BUTTONS_DOWN)
    {
	x = 8 + sbw->x;
	y = 8 + (button * 24) + sbw->y;
    }
    else
    {
	x = 8 + 24 + sbw->x;
	y = 8 + ((button - NUMOF_SELECT_BUTTONS_DOWN) * 24) + sbw->y;
    }
    hide_mouse ();
    for (q = 0; q < 3; q++)
    {
	Fgl_hline (x - 1 - q, y - 1 - q, x + 16 + q, blue (16 + q * 4));
	Fgl_hline (x - 1 - q, y + q + 16, x + 16 + q, blue (16 + q * 4));
	Fgl_line (x - 1 - q, y - 1 - q, x - 1 - q, y + q + 16, blue (16 + q * 4));
	Fgl_line (x + 16 + q, y - 1 - q, x + 16 + q, y + q + 16, blue (16 + q * 4));
    }
    redraw_mouse ();
}



void
do_mouse_main_win (int px, int py, int button)
{
    Rect* mw = &scr.main_win;
    int g, size;
    int x, y; /* mappoint */

#ifdef DEBUG_MAIN_SCREEN
    printf ("In do_mouse_main_win() mappoint x=%d y=%d type=%d\n",
	    (x - main->x) / 16, (y - main->y) / 16, selected_type);
#endif

    if (button == LC_MOUSE_MIDDLEBUTTON)
	return;

    g = get_group_of_type(selected_type);
    if (g < 0) return;

    pixel_to_mappoint(px, py, &x, &y);

    /* Bring up mappoint_stats for any right mouse click */
    if (button == LC_MOUSE_RIGHTBUTTON) {
	if (MP_TYPE(x,y) == CST_USED) {
	    mappoint_stats (MP_INFO(x,y).int_1, MP_INFO(x,y).int_2, button);
	} else {
	    mappoint_stats (x, y, button);
	}
	return;
    }

    /* Handle multitransport */
    if (button == LC_MOUSE_LEFTBUTTON && GROUP_IS_TRANSPORT(g)) {
	if (mt_draw (px, py, MT_START)) {
	    /* We need to set mps to current location, since the user might 
	       click on the transport to see the mps */
	    mappoint_stats (x, y, button);
	    return;
	}
    }

    /* Handle bulldozing */
    if (selected_type == CST_GREEN && button != LC_MOUSE_RIGHTBUTTON) {
	check_bulldoze_area (x, y);
	return;
    }

    /* Bring up mappoint_stats for certain left mouse clicks */
    if (MP_TYPE(x,y) != CST_GREEN) {
	if (MP_TYPE(x,y) == CST_USED) {
	    mappoint_stats (MP_INFO(x,y).int_1, MP_INFO(x,y).int_2, button);
	} else {
	    mappoint_stats (x, y, button);
	}
	return;
    }

    /* OK, by now we are certain that the user wants to place the item.  
       Set the origin based on the size of the selected_type, and 
       see if the selected item will fit. */
    size = main_groups[g].size;
    if (px > (mw->x + mw->w) - size*16)
	px = (mw->x + mw->w) - size*16;
    if (py > (mw->y + mw->h) - size*16)
	py = (mw->y + mw->h) - size*16;
    pixel_to_mappoint(px, py, &x, &y);

    if (size >= 2)
    {
	if (MP_TYPE(x + 1,y) != CST_GREEN
	    || MP_TYPE(x,y + 1) != CST_GREEN
	    || MP_TYPE(x + 1,y + 1) != CST_GREEN)
	    return;
    }
    if (size >= 3)
    {
	if (MP_TYPE(x + 2,y) != CST_GREEN
	    || MP_TYPE(x + 2,y + 1) != CST_GREEN
	    || MP_TYPE(x + 2,y + 2) != CST_GREEN
	    || MP_TYPE(x + 1,y + 2) != CST_GREEN
	    || MP_TYPE(x,y + 2) != CST_GREEN)
	    return;
    }
    if (size == 4)
    {
	if (MP_TYPE(x + 3,y) != CST_GREEN
	    || MP_TYPE(x + 3,y + 1) != CST_GREEN
	    || MP_TYPE(x + 3,y + 2) != CST_GREEN
	    || MP_TYPE(x + 3,y + 3) != CST_GREEN
	    || MP_TYPE(x + 2,y + 3) != CST_GREEN
	    || MP_TYPE(x + 1,y + 3) != CST_GREEN
	    || MP_TYPE(x,y + 3) != CST_GREEN)
	    return;
    }

    /* Place the selected item */
    switch (place_item (x, y, selected_type)) {
    case 0:
	/* Success */
	print_total_money ();
	break;
    case -1:
	/* Not enough money */
	no_credit_build_msg (g);
	break;
    case -2:
	/* Improper port placement */
	if (yn_dial_box ("WARNING",
			 "Ports need to be",
			 "connected to rivers!",
			 "Want to make a cup of tea?") != 0)
	    while (yn_dial_box ("TEA BREAK",
				"Boil->pour->wait->stir",
				"stir->pour->stir->wait->drink...ahhh",
				"Have you finished yet?") == 0);
	break;
    }
}

void
do_mouse_other_buttons (int x, int y, int button)
{
    Rect* mw = &scr.main_win;
#if defined (commentout)
    /* these must be checked for before the scroll areas. */
    /* this is the quit button */
    if (mouse_in_rect (&scr.quit_button,x,y)) {
	quit_flag = 1;
	if (pause_flag)
	    let_one_through = 1;
    }

    /* this is the load button */
    else if (mouse_in_rect (&scr.load_button,x,y)) {
	if (save_flag == 0)
	    load_flag = 1;
    }
#endif
    if (0) {}

    /* this is the pause button */
    else if (mouse_in_rect (&scr.pause_button,x,y)) {
	if (button == LC_MOUSE_RIGHTBUTTON) {
	    activate_help ("pause.hlp");
	    return;
	}
	select_pause ();
    }

    /* main screen border scroll areas */
#if defined (commentout)
    /* up */
    else if (x >= (mw->x - 8) && x < (mw->x + mw->w + 8)
	     && y >= (mw->y - 8) && y < mw->y
	     && main_screen_originy > 1)
    {
	if (button == LC_MOUSE_RIGHTBUTTON)
	{
	    main_screen_originy -= RIGHT_MOUSE_MOVE_VAL;
	    if (main_screen_originy < 1)
		main_screen_originy = 1;
	}
	else
	    main_screen_originy--;
	/*
	  // we have a normal mouse so it
	  // won't be taken care
	  // of in refresh_main_screen.
	*/
	request_main_screen ();
	hide_mouse ();
	refresh_main_screen ();
	redraw_mouse ();

    }
    /* down */
    else if (x >= (mw->x - 8) && x < (mw->x + mw->w + 8)
	     && y > (mw->y + mw->h)
	     && y < (mw->y + mw->h + 16)	/* 16 for bigger area */
	     && main_screen_originy < (WORLD_SIDE_LEN - mw->h / 16) - 1)
    {
	if (button == LC_MOUSE_RIGHTBUTTON)
	{
	    main_screen_originy += RIGHT_MOUSE_MOVE_VAL;
	    if (main_screen_originy
		> (WORLD_SIDE_LEN - mw->h / 16) - 2)
		main_screen_originy
			= (WORLD_SIDE_LEN - mw->h / 16) - 2;
	}
	main_screen_originy++;
	request_main_screen ();
	refresh_main_screen ();
    }
    /* left */
    else if (x >= (mw->x - 16) && x < mw->x
	     && y >= (mw->y - 8) && y < (mw->y + mw->h + 8)
	     && main_screen_originx > 1)
    {
	if (button == LC_MOUSE_RIGHTBUTTON) {
	    main_screen_originx -= RIGHT_MOUSE_MOVE_VAL;
	    if (main_screen_originx < 1)
		main_screen_originx = 1;
	} else {
	    main_screen_originx--;
	}
	request_main_screen ();
	hide_mouse ();
	refresh_main_screen ();
	redraw_mouse ();
    }
    /* right */
    else if (x > (mw->x + mw->w)
	     && x < (mw->x + mw->w + 8)
	     && y > (mw->y - 8) && y < (mw->y + mw->w + 8)
	     && main_screen_originx < (WORLD_SIDE_LEN - mw->w / 16) - 1)
    {
	if (button == LC_MOUSE_RIGHTBUTTON) {
	    main_screen_originx += RIGHT_MOUSE_MOVE_VAL;
	    if (main_screen_originx > (WORLD_SIDE_LEN - mw->w / 16) - 2)
		main_screen_originx = (WORLD_SIDE_LEN - mw->w / 16) - 2;
	}
	main_screen_originx++;
	request_main_screen ();
	refresh_main_screen ();
    }
#endif

    /* up */
    else if (x >= (mw->x - 8) && x < (mw->x + mw->w + 8)
	     && y >= (mw->y - 8) && y < mw->y) {
	int new_origin_y;
	if (button == LC_MOUSE_RIGHTBUTTON) {
	    new_origin_y = main_screen_originy - RIGHT_MOUSE_MOVE_VAL;
	} else {
	    new_origin_y = main_screen_originy - 1;
	}
	adjust_main_origin (main_screen_originx, new_origin_y, 1);
    }
    /* down */
    else if (x >= (mw->x - 8) && x < (mw->x + mw->w + 8)
	     && y > (mw->y + mw->h)
	     && y < (mw->y + mw->h + 16)) {	/* 16 for bigger area */
	int new_origin_y;
	if (button == LC_MOUSE_RIGHTBUTTON) {
	    new_origin_y = main_screen_originy + RIGHT_MOUSE_MOVE_VAL;
	} else {
	    new_origin_y = main_screen_originy + 1;
	}
	adjust_main_origin (main_screen_originx, new_origin_y, 1);
    }
    /* left */
    else if (x >= (mw->x - 16) && x < mw->x
	     && y >= (mw->y - 8) && y < (mw->y + mw->h + 8)) {
	int new_origin_x;
	if (button == LC_MOUSE_RIGHTBUTTON) {
	    new_origin_x = main_screen_originx - RIGHT_MOUSE_MOVE_VAL;
	} else {
	    new_origin_x = main_screen_originx - 1;
	}
	adjust_main_origin (new_origin_x, main_screen_originy, 1);
    }
    /* right */
    else if (x > (mw->x + mw->w)
	     && x < (mw->x + mw->w + 8)
	     && y > (mw->y - 8) && y < (mw->y + mw->w + 8)) {
	int new_origin_x;
	if (button == LC_MOUSE_RIGHTBUTTON) {
	    new_origin_x = main_screen_originx + RIGHT_MOUSE_MOVE_VAL;
	} else {
	    new_origin_x = main_screen_originx + 1;
	}
	adjust_main_origin (new_origin_x, main_screen_originy, 1);
    }

    /* These are the select buttons */
    else if (mouse_in_rect(&scr.select_buttons,x,y)) {
	do_mouse_select_buttons (x, y, button);
    }

    /* This is the mini window. Clicking here move the main window 
       to this point 
    */
    else if (mouse_in_rect (&scr.mini_map,x,y)) {
	Rect* b = &scr.mini_map;
	if (button == LC_MOUSE_RIGHTBUTTON) {
	    mini_screen_help ();
	    return;
	}
	if (mini_screen_flags == MINI_SCREEN_COAL_FLAG && !coal_survey_done) {
	    if (yn_dial_box ("Coal survey",
			     "This will cost you 1 million",
			     "After that it's is free to call again",
			     "Do coal survey?") == 0)
	    {
		return;
	    }
	    do_coal_survey ();
	    print_total_money ();
	    return;
	}
#if defined (commentout)
	main_screen_originx = x - b->x - mw->w / 32;
	if (main_screen_originx > (WORLD_SIDE_LEN - mw->w / 16) - 1)
	    main_screen_originx = (WORLD_SIDE_LEN - mw->w / 16) - 1;
	if (main_screen_originx <= 0)
	    main_screen_originx = 1;
	main_screen_originy = y - b->y - mw->h / 32;
	if (main_screen_originy > (WORLD_SIDE_LEN - mw->h / 16) - 1)
	    main_screen_originy = (WORLD_SIDE_LEN - mw->h / 16) - 1;
	if (main_screen_originy <= 0)
	    main_screen_originy = 1;
	request_main_screen ();
	hide_mouse ();
	refresh_main_screen ();
	redraw_mouse ();
#endif
	adjust_main_origin (x - b->x - mw->w / 32, y - b->y - mw->h / 32, 1);
	
	if (mini_screen_flags == MINI_SCREEN_PORT_FLAG)
	    draw_mini_screen ();
    }

    else if (mouse_in_rect (&scr.mini_map_aux,x,y)) {
	if (button == LC_MOUSE_RIGHTBUTTON) {
	    /* Quick hack here -- need to generalize later */
	    if (main_screen_flag == MINI_SCREEN_POL_FLAG)
		main_screen_flag = MINI_SCREEN_NORMAL_FLAG;
	    else
		main_screen_flag = MINI_SCREEN_POL_FLAG;
	    refresh_main_screen ();
#if defined (commentout)  /* Help-based drawing */
	    mini_screen_draw_in_main_win ();
#endif
#if defined (commentout)  /* LC v1.11 */
	    activate_help ("mini-screen.hlp");
#endif
	    return;
	}
	rotate_mini_screen ();
    }

#if defined (commentout)
    /* this is the normal button to return the mini screen to normal. */
    else if (mouse_in_rect (&scr.ms_normal_button,x,y)) {
	if (button == LC_MOUSE_RIGHTBUTTON) {
	    activate_help ("msb-normal.hlp");
	    return;
	}
	unrequest_mini_screen ();
	draw_mini_screen ();
    }
    /* this is the pollution button to activate the mini screen pol'n mode. */
    else if (mouse_in_rect (&scr.ms_pollution_button,x,y)) {
	if (button == LC_MOUSE_RIGHTBUTTON) {
	    activate_help ("msb-pol.hlp");
	    return;
	}
	request_mini_screen (MINI_SCREEN_POL_FLAG);
	draw_mini_screen_pollution ();
    }
    /* this is the fire cover button to activate the mini screen. */
    else if (mouse_in_rect (&scr.ms_fire_cover_button,x,y)) {
	if (button == LC_MOUSE_RIGHTBUTTON) {
	    activate_help ("msb-fire.hlp");
	    return;
	}
	request_mini_screen (MINI_SCREEN_FIRE_COVER);
	draw_mini_screen_fire_cover ();
    }
    /* this is the health cover button to activate the mini screen. */
    else if (mouse_in_rect (&scr.ms_health_cover_button,x,y)) {
	if (button == LC_MOUSE_RIGHTBUTTON) {
	    activate_help ("msb-health.hlp");
	    return;
	}
	request_mini_screen (MINI_SCREEN_HEALTH_COVER);
	draw_mini_screen_health_cover ();
    }
    /* this is the cricket cover button to activate the mini screen. */
    else if (mouse_in_rect (&scr.ms_cricket_cover_button,x,y)) {
	if (button == LC_MOUSE_RIGHTBUTTON) {
	    activate_help ("msb-cricket.hlp");
	    return;
	}
	request_mini_screen (MINI_SCREEN_CRICKET_COVER);
	draw_mini_screen_cricket_cover ();
    }
    /* this is the ub40 button */
    else if (mouse_in_rect (&scr.ms_ub40_button,x,y)) {
	if (button == LC_MOUSE_RIGHTBUTTON) {
	    activate_help ("msb-ub40.hlp");
	    return;
	}
	request_mini_screen (MINI_SCREEN_UB40_FLAG);
	draw_mini_screen_ub40 ();
    }
    /* this is the coal reserve button */
    else if (mouse_in_rect (&scr.ms_coal_button,x,y)) {
	if (button == LC_MOUSE_RIGHTBUTTON) {
	    activate_help ("msb-coal.hlp");
	    return;
	}
	if (coal_survey_done == 0) {
	    if (yn_dial_box ("Coal survey"
			     ,"This will cost you 1 million"
			     ,"After that it's is free to call again"
			     ,"Do coal survey?") != 0)
	    {
		do_coal_survey ();
		print_total_money ();
		request_mini_screen (MINI_SCREEN_COAL_FLAG);
		draw_mini_screen_coal ();
	    }
	    return;
	}
	else {
	    request_mini_screen (MINI_SCREEN_COAL_FLAG);
	    draw_mini_screen_coal ();
	}
    }
    /* this is the starving button */
    else if (mouse_in_rect (&scr.ms_starve_button,x,y)) {
	if (button == LC_MOUSE_RIGHTBUTTON) {
	    activate_help ("msb-starve.hlp");
	    return;
	}
	request_mini_screen (MINI_SCREEN_STARVE_FLAG);
	draw_mini_screen_starve ();
    }
    /* this is the other costs button */
    else if (mouse_in_rect (&scr.ms_ocost_button,x,y)) {
	if (button == LC_MOUSE_RIGHTBUTTON) {
	    activate_help ("msb-money.hlp");
	    return;
	}
	draw_mini_screen_ocost ();
    }
    /* this is the mini screen power button */
    else if (mouse_in_rect (&scr.ms_power_button,x,y)) {
	if (button == LC_MOUSE_RIGHTBUTTON) {
	    activate_help ("msb-power.hlp");
	    return;
	}
	draw_mini_screen_power ();
    }
#endif

    /* this is the menu button */
    else if (mouse_in_rect (&scr.menu_button,x,y)) {
	activate_help ("menu.hlp");
    }

    /* this is the help button */
    else if (mouse_in_rect (&scr.help_button,x,y)) {
	activate_help ("index.hlp");
    }

#if defined (commentout)
    /* this is the save button */
    else if (mouse_in_rect (&scr.save_button,x,y)) {
	if (load_flag == 0)
	    save_flag = 1;
    }
#endif

    /* this is the results (stats) button */
    else if (mouse_in_rect (&scr.results_button,x,y)) {
	if (button == LC_MOUSE_RIGHTBUTTON)
	    return;
	window_results ();
    }

#if defined (commentout)
    /* this is the overwrite transport button button */
    else if (mouse_in_rect (&scr.tover_button,x,y)) {
	if (button == LC_MOUSE_RIGHTBUTTON) {
	    activate_help ("tover.hlp");
	    return;
	}
	select_tover ();
    }

#ifdef LC_X11
    /* this is the confine mouse pointer button */
    else if (mouse_in_rect (&scr.confine_button,x,y)) {
	if (button == LC_MOUSE_RIGHTBUTTON) {
	    activate_help ("confinep.hlp");
	    return;
	}
	select_confine ();
    }
#endif
#endif

    /* this is the slow button */
    else if (mouse_in_rect (&scr.slow_button,x,y)) {
	if (button == LC_MOUSE_RIGHTBUTTON) {
	    activate_help ("slow.hlp");
	    return;
	}
	select_slow ();
    }

    /* this is the medium button */
    else if (mouse_in_rect (&scr.med_button,x,y)) {
	if (button == LC_MOUSE_RIGHTBUTTON) {
	    activate_help ("medium.hlp");
	    return;
	}
	select_medium ();
    }

    /* this is the fast button */
    else if (mouse_in_rect (&scr.fast_button,x,y)) {
	if (button == LC_MOUSE_RIGHTBUTTON) {
	    activate_help ("fast.hlp");
	    return;
	}
	select_fast ();
    }

    /* Advance mps screen if clicked on */
    else if (mouse_in_rect (&scr.mappoint_stats,x,y)) {
	if (button == LC_MOUSE_RIGHTBUTTON) {
	    activate_help ("stats.hlp");
	    return;
	}
	advance_mps_style ();
	refresh_mps ();
    }

   /* Advance monthgraph screen if clicked on */
    else if (mouse_in_rect (&scr.monthgraph,x,y)) {
	if (button == LC_MOUSE_RIGHTBUTTON) {
	    if (monthgraph_style == MONTHGRAPH_STYLE_ECONOMY) {
		activate_help ("economy.hlp");
	    } else {
		activate_help ("sustain.hlp");
	    }
	    return;
	}
	advance_monthgraph_style ();
	refresh_monthgraph ();
    }

    /*
      // no more buttons to click on, see if it's help for somewhere else.
      // ***************************
    */

    if (mouse_in_rect (&scr.pbar_area,x,y)) {
      pbar_mouse(x, y, button);
      return;
    }

    if (button == LC_MOUSE_RIGHTBUTTON) {

#if defined (FINANCE_WINDOW)

      /* now check for finance window */
      if (x >= FINANCE_X && x <= (FINANCE_X + FINANCE_W)
	  && y >= FINANCE_Y && y <= (FINANCE_Y + FINANCE_H))
	{
	  activate_help ("finance.hlp");
	  return;
	}
#endif
      
      
#if defined (commentout)
      /* GCS FIX -- This is obsolete, right??? */
      /*     -- If so, can delete scr.sust from geometry.c */
      /* now check for the sustain window */
      else if (mouse_in_rect (&scr.sust,x,y)) {
	activate_help ("sustain.hlp");
	return;
      }
#endif
    }
}


void
check_bulldoze_area (int x, int y)
{
  int xx, yy, g;
  if (MP_TYPE(x,y) == CST_USED) {
      xx = MP_INFO(x,y).int_1;
      yy = MP_INFO(x,y).int_2;
  } else {
      xx = x;
      yy = y;
  }
  g = MP_GROUP(xx,yy);

  /* GCS: Free bulldozing of most recently placed item is disabled.
     Still not sure how this can be done w/ multiplayer. */
  if (g == GROUP_MONUMENT && monument_bul_flag == 0)
    {
      if (yn_dial_box ("WARNING"
		       ,"Bulldozing a monument costs"
		       ,"a lot of money.", "Want to buldoze?") == 0)
	return;
      monument_bul_flag = 1;
    }
  else if (g == GROUP_RIVER && river_bul_flag == 0)
    {
      if (yn_dial_box ("WARNING"
		       ,"Bulldozing a section of river"
		       ,"costs a lot of money."
		       ,"Want to buldoze?") == 0)
	return;
      river_bul_flag = 1;
    }
  else if (g == GROUP_SHANTY)
    {
      if (yn_dial_box ("WARNING"
		       ,"Bulldozing a shanty town costs a"
		       ,"lot of money and may cause a fire."
		       ,"Want to buldoze?") == 0)
	return;
    }
  else if (g == GROUP_TIP)
    {
      ok_dial_box ("nobull-tip.mes", BAD, 0L);
      return;
    }

  bulldoze_item (xx,yy);
}

void
fire_area (int x, int y)
{
  do_bulldoze_area (CST_FIRE_1, x, y);
  refresh_main_screen ();
  /*
    // update transport or we get stuff put in
    // the area from connected tracks etc.
  */
}

/* Called from event handler when middle button is held while moving
   the mouse.  Will probably handle multi-transport eventually */

/* XXX: Need to put this in a header somewhere */

void 
drag_screen (void) 
{
    Rect* mw = &scr.main_win;
    int cur_mappoint_x = 0;
    int cur_mappoint_y = 0;

    int cur_winpoint_x = 0;
    int cur_winpoint_y = 0;
  
    int origin_x = 0;
    int origin_y = 0;

    int in_main_window;

    if (buttons[LC_MOUSE_MIDDLEBUTTON-1].mappoint_x == 0) 
	return;   /* Not pressed in main window */

    in_main_window = pixel_to_mappoint(cs_mouse_x, cs_mouse_y, 
				       &cur_mappoint_x, &cur_mappoint_y);
  
    pixel_to_winpoint(cs_mouse_x, cs_mouse_y, &cur_winpoint_x, &cur_winpoint_y);
  
    origin_x = 
	    buttons[LC_MOUSE_MIDDLEBUTTON-1].mappoint_x - cur_winpoint_x;  
    origin_y =  
	    buttons[LC_MOUSE_MIDDLEBUTTON-1].mappoint_y - cur_winpoint_y;  

#ifdef DEBUG_MOUSE_DRAG 
    printf("drag_screen: cwp_x = %d, cwp_y = %d, mappoint_x=%d\n",
	   cur_winpoint_x, cur_winpoint_y, buttons[LC_MOUSE_MIDDLEBUTTON-1].mappoint_x);
#endif

#if defined (commentout)
    if (origin_y < 1)
	origin_y = 1;
    if (origin_x < 1)
	origin_x = 1;

    if (origin_x > WORLD_SIDE_LEN - mw->w / 16 - 2)
	origin_x = WORLD_SIDE_LEN - mw->w / 16 - 2;
    if (origin_y > WORLD_SIDE_LEN - mw->h / 16 - 2)
	origin_y = WORLD_SIDE_LEN - mw->h / 16 - 2;

    if ((main_screen_originx != origin_x) 
	|| (main_screen_originy != origin_y))
    {
	main_screen_originx = origin_x;
	main_screen_originy = origin_y;

	request_main_screen ();
	hide_mouse ();
	refresh_main_screen ();
	redraw_mouse ();
    }
#endif
    adjust_main_origin (origin_x, origin_y, 1);
}

void
do_market_cb_template (int x, int y, int is_market_cb)
{
    int old_flags = MP_INFO(mcbx,mcby).flags;
    Rect* mcb = &scr.market_cb;
    int is_sell;

    hide_mouse ();
    if (!mouse_in_rect(mcb,x,y)) {
	if (is_market_cb) {
	    close_market_cb ();
	} else {
	    close_port_cb ();
	}
	redraw_mouse ();
	return;
    }

    if (x >= mcb->x + 8 && x <= mcb->x + 6 * 8) {
	is_sell = 0;
    } else if (x >= mcb->x + 10 * 8 && x <= mcb->x + mcb->w - 8) {
	is_sell = 1;
    } else {
	redraw_mouse ();
	return;
    }

    /* jobs */
    if (is_market_cb && y >= mcb->y + 36 + (CB_SPACE * 0) 
      && y <= mcb->y + 52 + (CB_SPACE * 0))	{
	int flag = is_sell ? FLAG_MS_JOBS : FLAG_MB_JOBS;
	MP_INFO(mcbx,mcby).flags ^= flag;
	draw_cb_box (0, is_sell, MP_INFO(mcbx,mcby).flags & flag);
    }
    /* food */
    else if (y >= mcb->y + 36 + (CB_SPACE * 1) 
      && y <= mcb->y + 52 + (CB_SPACE * 1)) {
	int flag = is_sell ? FLAG_MS_FOOD : FLAG_MB_FOOD;
	MP_INFO(mcbx,mcby).flags ^= flag;
	draw_cb_box (1, is_sell, MP_INFO(mcbx,mcby).flags & flag);
    }
    /* coal */
    else if (y >= mcb->y + 36 + (CB_SPACE * 2)
      && y <= mcb->y + 52 + (CB_SPACE * 2)) {
	int flag = is_sell ? FLAG_MS_COAL : FLAG_MB_COAL;
	MP_INFO(mcbx,mcby).flags ^= flag;
	draw_cb_box (2, is_sell, MP_INFO(mcbx,mcby).flags & flag);
    }
    /* ore */
    else if (y >= mcb->y + 36 + (CB_SPACE * 3) 
      && y <= mcb->y + 52 + (CB_SPACE * 3)) {
	int flag = is_sell ? FLAG_MS_ORE : FLAG_MB_ORE;
	MP_INFO(mcbx,mcby).flags ^= flag;
	draw_cb_box (3, is_sell, MP_INFO(mcbx,mcby).flags & flag);
    }
    /* goods */
    else if (y >= mcb->y + 36 + (CB_SPACE * 4) 
      && y <= mcb->y + 52 + (CB_SPACE * 4)) {
	int flag = is_sell ? FLAG_MS_GOODS : FLAG_MB_GOODS;
	MP_INFO(mcbx,mcby).flags ^= flag;
	draw_cb_box (4, is_sell, MP_INFO(mcbx,mcby).flags & flag);
    }
    /* steel */
    else if (y >= mcb->y + 36 + (CB_SPACE * 5)
      && y <= mcb->y + 52 + (CB_SPACE * 5)) {
	int flag = is_sell ? FLAG_MS_STEEL : FLAG_MB_STEEL;
	MP_INFO(mcbx,mcby).flags ^= flag;
	draw_cb_box (5, is_sell, MP_INFO(mcbx,mcby).flags & flag);
    }

    redraw_mouse ();

    if (MP_INFO(mcbx,mcby).flags != old_flags) {
	send_flags (mcbx,mcby);
    }
}

void
do_market_cb_mouse (int x, int y)
{
    do_market_cb_template (x, y, 1);
}

void
do_port_cb_mouse (int x, int y)
{
    do_market_cb_template (x, y, 0);
}

#if defined (commentout)
void
do_market_cb_mouse (int x, int y)
{
    int old_flags = MP_INFO(mcbx,mcby).flags;
    Rect* mcb = &scr.market_cb;

    hide_mouse ();
    /* buy col */
    if (x >= mcb->x + 2 * 8 && x <= mcb->x + 16 * 8)
    {
	/* food */
	if (y >= mcb->y + 3 * 8 + 4 && y <= mcb->y + 5 * 8 + 4)
	{
	    MP_INFO(mcbx,mcby).flags ^= FLAG_MB_FOOD;
	    draw_cb_box (0, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_FOOD);
	}
	/* jobs */
	else if (y >= mcb->y + 6 * 8 + 4 && y <= mcb->y + 8 * 8 + 4)
	{
	    MP_INFO(mcbx,mcby).flags ^= FLAG_MB_JOBS;
	    draw_cb_box (1, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_JOBS);
	}
	/* coal */
	else if (y >= mcb->y + 9 * 8 + 4 && y <= mcb->y + 11 * 8 + 4)
	{
	    MP_INFO(mcbx,mcby).flags ^= FLAG_MB_COAL;
	    draw_cb_box (2, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_COAL);
	}
	/* ore */
	else if (y >= mcb->y + 12 * 8 + 4 && y <= mcb->y + 14 * 8 + 4)
	{
	    MP_INFO(mcbx,mcby).flags ^= FLAG_MB_ORE;
	    draw_cb_box (3, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_ORE);
	}
	/* goods */
	else if (y >= mcb->y + 15 * 8 + 4 && y <= mcb->y + 17 * 8 + 4)
	{
	    MP_INFO(mcbx,mcby).flags ^= FLAG_MB_GOODS;
	    draw_cb_box (4, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_GOODS);
	}
	/* steel */
	else if (y >= mcb->y + 18 * 8 + 4 && y <= mcb->y + 20 * 8 + 4)
	{
	    MP_INFO(mcbx,mcby).flags ^= FLAG_MB_STEEL;
	    draw_cb_box (5, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_STEEL);
	}
	else
        {
	    close_market_cb ();
        }
    }

    /* sell col */
    if (x >= mcb->x + 2 * 8 && x <= mcb->x + 16 * 8)
    {
	/* food */
	if (y >= mcb->y + 3 * 8 + 4 && y <= mcb->y + 5 * 8 + 4)

	{
	    MP_INFO(mcbx,mcby).flags ^= FLAG_MS_FOOD;
	    draw_cb_box (0, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_FOOD);
	}
	/* jobs */
	else if (y >= mcb->y + 6 * 8 + 4 && y <= mcb->y + 8 * 8 + 4)
	{
	    MP_INFO(mcbx,mcby).flags ^= FLAG_MS_JOBS;
	    draw_cb_box (1, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_JOBS);
	}
	/* coal */
	else if (y >= mcb->y + 9 * 8 + 4 && y <= mcb->y + 11 * 8 + 4)
	{
	    MP_INFO(mcbx,mcby).flags ^= FLAG_MS_COAL;
	    draw_cb_box (2, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_COAL);
	}
	/* ore */
	else if (y >= mcb->y + 12 * 8 + 4 && y <= mcb->y + 14 * 8 + 4)
	{
	    MP_INFO(mcbx,mcby).flags ^= FLAG_MS_ORE;
	    draw_cb_box (3, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_ORE);
	}
	/* goods */
	else if (y >= mcb->y + 15 * 8 + 4 && y <= mcb->y + 17 * 8 + 4)
	{
	    MP_INFO(mcbx,mcby).flags ^= FLAG_MS_GOODS;
	    draw_cb_box (4, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_GOODS);
	}
	/* steel */
	else if (y >= mcb->y + 18 * 8 + 4 && y <= mcb->y + 20 * 8 + 4)
	{
	    MP_INFO(mcbx,mcby).flags ^= FLAG_MS_STEEL;
	    draw_cb_box (5, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_STEEL);
	}
	else
        {
	    close_market_cb ();
        }
    }
    else
    {
	close_market_cb ();
    }
    redraw_mouse ();

    if (MP_INFO(mcbx,mcby).flags != old_flags) {
	send_flags (mcbx,mcby);
    }
}

void
do_port_cb_mouse (int x, int y)
{
    int old_flags = MP_INFO(mcbx,mcby).flags;
    Rect* mcb = &scr.market_cb;

    hide_mouse ();
    /* buy col */
    if (x >= mcb->x + 2 * 8 && x <= mcb->x + 4 * 8)
    {
	/* food */
	if (y >= mcb->y + 6 * 8 + 4 && y <= mcb->y + 8 * 8 + 4)
        {
	    MP_INFO(mcbx,mcby).flags ^= FLAG_MB_FOOD;
	    draw_cb_box (1, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_FOOD);
	}
	/* coal */
	else if (y >= mcb->y + 9 * 8 + 4 && y <= mcb->y + 11 * 8 + 4)
        {
	    MP_INFO(mcbx,mcby).flags ^= FLAG_MB_COAL;
	    draw_cb_box (2, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_COAL);
	}
	/* ore */
	else if (y >= mcb->y + 12 * 8 + 4 && y <= mcb->y + 14 * 8 + 4)
	{
	    MP_INFO(mcbx,mcby).flags ^= FLAG_MB_ORE;
	    draw_cb_box (3, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_ORE);
	}
	/* goods */
	else if (y >= mcb->y + 15 * 8 + 4 && y <= mcb->y + 17 * 8 + 4)
	{
	    MP_INFO(mcbx,mcby).flags ^= FLAG_MB_GOODS;
	    draw_cb_box (4, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_GOODS);
	}
	/* steel */
	else if (y >= mcb->y + 18 * 8 + 4 && y <= mcb->y + 20 * 8 + 4)
	{
	    MP_INFO(mcbx,mcby).flags ^= FLAG_MB_STEEL;
	    draw_cb_box (5, 0, MP_INFO(mcbx,mcby).flags & FLAG_MB_STEEL);
	}
	else
        {
	    close_port_cb ();
        }
    }

    /* sell col */
    else if (x >= mcb->x + 14 * 8 && x <= mcb->x + 16 * 8)
    {
	/* food */
	if (y >= mcb->y + 6 * 8 + 4 && y <= mcb->y + 8 * 8 + 4)
	{
	    MP_INFO(mcbx,mcby).flags ^= FLAG_MS_FOOD;
	    draw_cb_box (1, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_FOOD);
	}
	/* coal */
	else if (y >= mcb->y + 9 * 8 + 4 && y <= mcb->y + 11 * 8 + 4)
	{
	    MP_INFO(mcbx,mcby).flags ^= FLAG_MS_COAL;
	    draw_cb_box (2, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_COAL);
	}
	/* ore */
	else if (y >= mcb->y + 12 * 8 + 4 && y <= mcb->y + 14 * 8 + 4)
	{
	    MP_INFO(mcbx,mcby).flags ^= FLAG_MS_ORE;
	    draw_cb_box (3, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_ORE);
	}
	/* goods */
	else if (y >= mcb->y + 15 * 8 + 4 && y <= mcb->y + 17 * 8 + 4)
	{
	    MP_INFO(mcbx,mcby).flags ^= FLAG_MS_GOODS;
	    draw_cb_box (4, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_GOODS);
	}
	/* steel */
	else if (y >= mcb->y + 18 * 8 + 4 && y <= mcb->y + 20 * 8 + 4)
	{
	    MP_INFO(mcbx,mcby).flags ^= FLAG_MS_STEEL;
	    draw_cb_box (5, 1, MP_INFO(mcbx,mcby).flags & FLAG_MS_STEEL);
	}
	else
        {
	    close_port_cb ();
        }
    }
    else
	close_port_cb ();
    redraw_mouse ();

    if (MP_INFO(mcbx,mcby).flags != old_flags) {
	send_flags (mcbx,mcby);
    }
}
#endif


void
do_db_mouse (int x, int y)
{
  if (x > db_yesbox_x1 && x < db_yesbox_x2
      && y > db_yesbox_y1 && y < db_yesbox_y2)
    db_yesbox_clicked = 1;
  else if (x > db_nobox_x1 && x < db_nobox_x2
	   && y > db_nobox_y1 && y < db_nobox_y2)
    db_nobox_clicked = 1;
}

void
do_db_okmouse (int x, int y)
{
  if (x > db_okbox_x1 && x < db_okbox_x2
      && y > db_okbox_y1 && y < db_okbox_y2)
    db_okbox_clicked = 1;
}

void 
no_credit_build_msg (int selected_group)
{
#ifdef GROUP_POWER_SOURCE_NO_CREDIT
  if (selected_group == GROUP_POWER_SOURCE) {
    ok_dial_box ("no-credit-solar-power.mes", BAD, 0L);
    return;
  }
#endif
#ifdef GROUP_UNIVERSITY_NO_CREDIT
  if (selected_group == GROUP_UNIVERSITY) {
    ok_dial_box ("no-credit-university.mes", BAD, 0L);
    return;
  }
#endif
#ifdef GROUP_PARKLAND_NO_CREDIT
  if (selected_group == GROUP_PARKLAND) {
    ok_dial_box ("no-credit-parkland.mes", BAD, 0L);
    return;
  }
#endif
#ifdef GROUP_RECYCLE_NO_CREDIT
  if (selected_group == GROUP_RECYCLE) {
    ok_dial_box ("no-credit-recycle.mes", BAD, 0L);
    return;
  }
#endif
#ifdef GROUP_ROCKET
  if (selected_group == GROUP_ROCKET) {
    ok_dial_box ("no-credit-rocket.mes", BAD, 0L);
    return;
  }
#endif
  return;
}

void
choose_residence (void)
{
    int cost;
    FILE* tempfile;
    char* s = (char*) malloc (lc_save_dir_len + 9);

    sprintf (s, "%s%c%s", lc_save_dir, PATH_SLASH, "res.tmp");
    if ((tempfile = fopen (s, "w")) == 0)
	do_error ("Can't write res.tmp");
    free (s);

    fprintf (tempfile,
	     "text -1 20 Choose the type of residence you want\n");
    fprintf (tempfile,
	     "text -1 30 =====================================\n");
    fprintf (tempfile,
	     "text -1 45 (LB=Low Birthrate HB=High Birthrate)\n");
    fprintf (tempfile,
	     "text -1 55 (LD=Low Deathrate HD=High Deathrate)\n");
    fprintf (tempfile, "text -1 85 Low Tech\n");

    cost = get_group_cost(GROUP_RESIDENCE_LL);
    fprintf (tempfile, "text 68 106 Cost %4d\n", cost);
    fprintf (tempfile, "icon 85 120 reslowlow.csi\n");
    fprintf (tempfile, "button 83 118 52 52 return1\n");
    fprintf (tempfile, "tbutton 82 180 return1 pop 50\n");
    fprintf (tempfile, "text 89 195 LB HD\n");

    cost = get_group_cost(GROUP_RESIDENCE_ML);
    fprintf (tempfile, "text 155 106 Cost %4d\n", cost);
    fprintf (tempfile, "icon 170 120 resmedlow.csi\n");
    fprintf (tempfile, "button 168 118 52 52 return2\n");
    fprintf (tempfile, "tbutton 164 180 return2 pop 100\n");
    fprintf (tempfile, "text 175 195 HB LD\n");

    cost = get_group_cost(GROUP_RESIDENCE_HL);
    fprintf (tempfile, "text 238 106 Cost %4d\n", cost);
    fprintf (tempfile, "icon 255 120 reshilow.csi\n");
    fprintf (tempfile, "button 253 118 52 52 return3\n");
    fprintf (tempfile, "tbutton 250 180 return3 pop 200\n");
    fprintf (tempfile, "text 261 195 HB HD\n");

    fprintf (tempfile, "text -1 215 Click on one to select\n");
    fprintf (tempfile, "text -1 225 ======================\n");

    if (((tech_level * 10) / MAX_TECH_LEVEL) > 2)
    {
	fprintf (tempfile, "text -1 255 High Tech\n");

	cost = get_group_cost(GROUP_RESIDENCE_LH);
	fprintf (tempfile, "text 68 276 Cost %4d\n", cost);
	fprintf (tempfile, "icon 85 290 reslowhi.csi\n");
	fprintf (tempfile, "button 83 288 52 52 return4\n");
	fprintf (tempfile, "tbutton 78 350 return4 pop 100\n");
	fprintf (tempfile, "text 89 365 LB HD\n");

	cost = get_group_cost(GROUP_RESIDENCE_MH);
	fprintf (tempfile, "text 155 276 Cost %4d\n", cost);
	fprintf (tempfile, "icon 170 290 resmedhi.csi\n");
	fprintf (tempfile, "button 168 288 52 52 return5\n");
	fprintf (tempfile, "tbutton 164 350 return5 pop 200\n");
	fprintf (tempfile, "text 175 365 HB LD\n");

	cost = get_group_cost(GROUP_RESIDENCE_HH);
	fprintf (tempfile, "text 238 276 Cost %4d\n", cost);
	fprintf (tempfile, "icon 255 290 reshihi.csi\n");
	fprintf (tempfile, "button 253 288 52 52 return6\n");
	fprintf (tempfile, "tbutton 250 350 return6 pop 400\n");
	fprintf (tempfile, "text 261 365 HB HD\n");
    }

    fclose (tempfile);
    block_help_exit = 1;
    activate_help ("res.tmp");
}


/* We can only draw temporary transport on GROUP_TRACK, GROUP_ROAD,
   GROUP_RAIL, or GROUP_BARE.  For temporary draws over GROUP_BARE, 
   FLAG_IS_TRANSPORT is not set.  For temporary draws over existing 
   transport of a different type, FLAG_IS_TRANSPORT is set, and 
   FLAG_MULTI_TRANSPORT_PREV will be set to 0 if the existing transport is 
   of the cheaper sort (e.g. GROUP_TRACK when overwriting with GROUP_ROAD), 
   and set to 1 if the existing transport if the more expensive sort
   (e.g. GROUP_RAIL when overwriting GROUP_ROAD).
*/
inline int
mt_erase(int x, int y)
{
    if (x < 0 || y < 0 || x >= WORLD_SIDE_LEN || y >= WORLD_SIDE_LEN)
	return 1;
    if (MP_INFO(x,y).flags & FLAG_MULTI_TRANSPORT) {
	if (MP_INFO(x,y).flags & FLAG_IS_TRANSPORT) {
	    if (!(MP_INFO(x,y).flags & FLAG_MULTI_TRANS_PREV)) {
		if (mt_grp == GROUP_TRACK) {
		    MP_TYPE(x,y) = CST_ROAD_LR;
		    MP_GROUP(x,y) = GROUP_ROAD;
		} else {
		    MP_TYPE(x,y) = CST_TRACK_LR;
		    MP_GROUP(x,y) = GROUP_TRACK;
		}
	    } else {
		if (mt_grp == GROUP_RAIL) {
		    MP_TYPE(x,y) = CST_ROAD_LR;
		    MP_GROUP(x,y) = GROUP_ROAD;
		} else {
		    MP_TYPE(x,y) = CST_RAIL_LR;
		    MP_GROUP(x,y) = GROUP_RAIL;
		}
	    }
	} else {
	    MP_TYPE(x,y) = CST_GREEN;
	    MP_GROUP(x,y) = GROUP_BARE;
	}
	MP_INFO(x,y).flags &= ~FLAG_MULTI_TRANSPORT;
	return 1;
    } else if ((MP_INFO(x,y).flags & FLAG_IS_TRANSPORT) 
	       && (mt_grp == MP_GROUP(x,y))) {
	return 1;
    }
    return 0;
}

inline int
mt_temp(int x, int y)
{
    if (x < 0 || y < 0 || x >= WORLD_SIDE_LEN || y >= WORLD_SIDE_LEN)
	return 1;
    if (MP_INFO(x,y).flags & FLAG_IS_TRANSPORT) {
	if (MP_GROUP(x,y) == mt_grp) {
	    return 1;
	} else if (!overwrite_transport_flag) {
	    return 0;
	} else {
	    if ((MP_GROUP(x,y) == GROUP_TRACK) ||
		((MP_GROUP(x,y) == GROUP_ROAD) && (mt_grp == GROUP_TRACK)))
	    {
		MP_INFO(x,y).flags &= ~FLAG_MULTI_TRANS_PREV;
	    } else {
		MP_INFO(x,y).flags |= FLAG_MULTI_TRANS_PREV;
	    }
	    mt_cost++;
	    MP_TYPE(x,y) = selected_type;
	    MP_GROUP(x,y) = mt_grp;
	    MP_INFO(x,y).flags |= FLAG_MULTI_TRANSPORT;
	    return 1;
	}
    } else if (MP_GROUP(x,y) == GROUP_BARE) { 
	mt_cost++;
	MP_TYPE(x,y) = selected_type;
	MP_GROUP(x,y) = mt_grp;
	MP_INFO(x,y).flags |= FLAG_MULTI_TRANSPORT;
	return 1;
    }
    return 0;
}

inline int
mt_perm(int x, int y)
{
    /* By now, it has already been mt_erase()'d */
    if (x < 0 || y < 0 || x >= WORLD_SIDE_LEN || y >= WORLD_SIDE_LEN)
	return 1;
    if (MP_INFO(x,y).flags & FLAG_IS_TRANSPORT) {
	if (MP_GROUP(x,y) == mt_grp) {
	    return 1;
	} else if (!overwrite_transport_flag) {
	    return 0;
	} else {
	    bulldoze_item(x,y);
	    place_item (x,y,selected_type);
	    return 1;
	}
    } else if (MP_GROUP(x,y) == GROUP_BARE) { 
	place_item (x,y,selected_type);
	return 1;
    }
    return 0;
}

int
do_mt_draw (int x1, int x2, int y1, int y2, int (*mode)())
{
    int dir = 1;
    int horiz = 1;
    int vert = 2;
    int x_dir = cmp(x1, x2);
    int y_dir = cmp(y1, y2);
    int ix = x1;
    int iy = y1;

    mt_cost = 0;

    if (!mode(ix, iy))
	return 0;

    while ((ix != x2 || iy != y2) && dir) {
	/* try horizontal, unless already vertical */
	if (dir == 1) {
	    if (ix == x2) {
		horiz = 0;
		dir = vert;
	    } else if (mode(ix+x_dir, iy)) {
		horiz = 1;
		vert = 2;
		ix += x_dir;
	    } else {
		horiz = 0;
		dir = vert;
	    }
	} else if (dir == 2) {
	    if (iy == y2) {
		vert = 0;
		dir = horiz;
	    } else if (mode(ix, iy+y_dir)) {
		horiz = 1;
		vert = 2;
		iy += y_dir;
	    } else {
		vert = 0;
		dir = horiz;
	    }
	}
    }
    if (dir) {  /* still had one direction to pick from, must have succeeded */
	return 1;
    } else {
	return 0;
    }
}

  
int
mt_draw (int cxp, int cyp, int flag) /* c[xy]p are pixel coordinates */
{

#define STATUS_MESSAGE_LENGTH 40
    static int dx, dy; /* old current point; drawn point */
    static int ox, oy; /* coordinates for original button press */
    int cx, cy;   /* current mappoint coordinates */
    int draw_ret;
    char s[STATUS_MESSAGE_LENGTH];

    if (flag != MT_START && !mt_flag) 
	return 0;
    
    pixel_to_mappoint(cxp, cyp,  &cx, &cy);

    switch(flag) {
    case MT_SUCCESS:
	if (ox == 0) 
	    return 0;
	draw_ret = do_mt_draw(ox, dx, oy, dy, mt_erase);

	if (!draw_ret) {
	    /* If mt_erase failed it is because we don't have clearance
	       to build the road.  So clean up and exit. */
	    mt_flag = 0;
	    draw_main_window_box (green (8)); 
	}
	else if (draw_ret = do_mt_draw(ox, cx, oy, cy, mt_perm)) {

	    print_total_money ();
	    mt_flag = 0;
	    draw_main_window_box (green (8)); 
	} else {
	    /* This shouldn't happen.  Clean up and continue anyway.  */
	    mt_flag = 0;
	    draw_main_window_box (green (8)); 
	}
	dx = 0; dy = 0;
	ox = 0; oy = 0;
	break;
      
    case MT_FAIL:       
	mt_flag = 0;
	draw_main_window_box (green (8));
	draw_ret = do_mt_draw(ox, dx, oy, dy, mt_erase);

	dx = 0; dy = 0;
	ox = 0; oy = 0;
	break;

    case MT_CONTINUE: 
	if ((dx == cx && dy == cy) || ox == 0) 
	    return 0;
	draw_ret = do_mt_draw(ox, dx, oy, dy, mt_erase);

	draw_ret = do_mt_draw(ox, cx, oy, cy, mt_temp);

	if (!draw_ret) {
	    draw_ret = do_mt_draw(ox, cx, oy, cy, mt_erase);
	};
	snprintf(s,STATUS_MESSAGE_LENGTH-1,"%10s: %3d * %20d",
		 mt_name,mt_cost,
		 mt_cost * get_type_cost(selected_type));
	status_message(s,25);
	dx = cx; dy = cy;
	break;

    case MT_START:
	/* we assume that a transport type is selected.   */

        if ((mt_grp = get_group_of_type(selected_type)) < 0 )
	  return 0;

	get_type_name(selected_type,mt_name);

	dx = dy = 0;
	ox = buttons[LC_MOUSE_LEFTBUTTON-1].mappoint_x;
	oy = buttons[LC_MOUSE_LEFTBUTTON-1].mappoint_y;
	draw_ret = do_mt_draw(ox, cx, oy, cy, mt_temp);

	if (!draw_ret) {
	    draw_ret = do_mt_draw(ox, cx, oy, cy, mt_erase);
	    return 0;
	}
	dx = cx;
	dy = cy;
	mt_flag = 1;
	draw_main_window_box (cyan (20));
	break;

    default:
	;
    }

    refresh_main_screen ();
    return (1);
}

int 
cmp(int n1, int n2)
{
    if (n1 != n2) 
	return (n1 < n2) ? 1 : -1;
    else
	return 0;
}



void
init_mouse_registry()
{
    mhandle_first = NULL;
    mhandle_last = NULL;
    mhandle_current = NULL;
    mhandle_count = 0;
}

/* Add and return an entry in the registry.  Add it at the beginning, so
   it supercedes earlier entries in mouse_handle_click() */

Mouse_Handle *
mouse_register(Rect * r, void (*function)(int, int, int)) 
{

    mhandle_current = (Mouse_Handle *)lcalloc(sizeof(Mouse_Handle));
    mhandle_count++;
    if (mhandle_first == NULL) {
	mhandle_current->next = NULL;
	mhandle_current->prev = NULL;
    } else {
	mhandle_current->next = mhandle_first;
	mhandle_first->prev = mhandle_current;
	mhandle_current->prev = NULL;
    }

    mhandle_first = mhandle_current;

    mhandle_current->r = r;
    mhandle_current->handler = function;

    return mhandle_current;
}


/* Remove an entry from the registry */

void 
mouse_unregister(Mouse_Handle * mhandle)
{

    if (mhandle->prev == NULL) {
	if (mhandle_first != mhandle) 
	    printf("debug: mhandle_first != mhandle\n");
	if (mhandle->next != NULL) {
	    mhandle_first = mhandle->next;
	    mhandle_first->prev = NULL;
	} else 
	    mhandle_first = NULL;
    } else if (mhandle->next == NULL) 
	mhandle->prev->next = NULL;
    else {
	mhandle->prev->next = mhandle->next;
	mhandle->next->prev = mhandle->prev;
    }

    free(mhandle);
    mhandle_count--;
}

/* Loop through the registry until we find a handler for an area.  
   BEWARE!!!  Some handlers unregister themselves when called.  Assume 
   mhandle_current is undefined after calling mhandle_current->handler()
*/

int 
mouse_handle_click(int x, int y, int button) 
{
    mhandle_current = mhandle_first;

    while (mhandle_current != NULL) {
	if (mouse_in_rect(mhandle_current->r,x,y)) {
	    mhandle_current->handler(x - mhandle_current->r->x, 
				     y - mhandle_current->r->y, button);
	    return 1;
	}
	
	mhandle_current = mhandle_current->next;
    }

    return 0;
}
    
