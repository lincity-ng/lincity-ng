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
#include "lcintl.h"
#include "lin-city.h"
#include "mouse.h"
#include "engglobs.h"
#include "cliglobs.h"
#include "engine.h"
#include "screen.h"
#include "mps.h"
#include "fileutil.h"
#include "lchelp.h"
#include "pbar.h"
#include "lclib.h"
#include "module_buttons.h"

#define DEBUG_MT_CODE 1

extern Update_Scoreboard update_scoreboard;

/* ---------------------------------------------------------------------- *
 * Private global variables
 * ---------------------------------------------------------------------- */
static struct mouse_button_struct buttons[NUM_BUTTONS];
static int mt_length; 
static int mt_grp;
static char mt_name[20];
static short mouse_buffer_fresh = 0;

void check_bulldoze_area (int x, int y);

/* Mouse registry */

static int mhandle_count;

static Mouse_Handle * mhandle_first;
static Mouse_Handle * mhandle_last;
static Mouse_Handle * mhandle_current;

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

    /* see if we are all the way up */
    if (!mouse_initialized)
	return;

    /* button press */
    if (button_pressed) {

	/* maintain button press status */
	pixel_to_mappoint(cs_mouse_x, cs_mouse_y, 
			  &buttons[button_idx].mappoint_x, 
			  &buttons[button_idx].mappoint_y);
	buttons[button_idx].x = cs_mouse_x;
	buttons[button_idx].y = cs_mouse_y;
	buttons[button_idx].pressed = 1;

	/* Try the event list before moving on to special cases */
	if (!mouse_handle_click(x, y, button)) {
	    switch (button) {
	    case LC_MOUSE_LEFTBUTTON:
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

		else if (load_flag || save_flag) 
		    return;

		/* This is the main screen */
		if (mouse_in_rect(&scr.main_win,x,y)) {
		    do_mouse_main_win(x, y, button);
		    refresh_main_screen ();
		    break;
		}

		/* GCS Remove overlay  */
		/* This use of mouse clicks seems to contradict 
		    Corey's mouse handler code */
		if (main_screen_flag == MAIN_SCREEN_EQUALS_MINI) {
		    main_screen_flag = MAIN_SCREEN_NORMAL_FLAG;
		    refresh_main_screen ();
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

		/* GCS Remove overlay  */
		if (main_screen_flag == MAIN_SCREEN_EQUALS_MINI) {
		    main_screen_flag = MAIN_SCREEN_NORMAL_FLAG;
		    refresh_main_screen ();
		}

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
      
	buttons[button_idx].r_x = cs_mouse_x;
	buttons[button_idx].r_y = cs_mouse_y;
	buttons[button_idx].pressed = 0;
      
	switch (button) {
	case LC_MOUSE_LEFTBUTTON:
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
	    && GROUP_IS_TRANSPORT(selected_module_group))
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

    size = (main_groups[selected_module_group].size) * 16;

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
	hide_mouse ();
	mouse_hide_count--;
	draw_normal_mouse (x, y);
	if (mappoint_stats_flag != 0 && market_cb_flag == 0
	    && port_cb_flag == 0)
	{
	    mappoint_stats_flag = 0;
	    update_scoreboard.mps = 1;
	}
    }
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

    size = (main_groups[selected_module_group].size) * 16;
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

    size = (main_groups[selected_module_group].size) * 16;

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
    if (x > cs_mouse_xmax - 8 || y > cs_mouse_ymax - 8) {
	Fgl_enableclipping ();
	Fgl_setclippingwindow (0, 0, cs_mouse_xmax, cs_mouse_ymax);
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
    if (mox > cs_mouse_xmax - 8 || moy > cs_mouse_ymax - 8) {
	Fgl_enableclipping ();
	Fgl_setclippingwindow (0, 0, cs_mouse_xmax, cs_mouse_ymax);
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
    if (mox > cs_mouse_xmax - 8 || moy > cs_mouse_ymax - 8) {
	Fgl_enableclipping ();
	Fgl_setclippingwindow (0, 0, cs_mouse_xmax, cs_mouse_ymax);
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
do_mouse_main_win (int px, int py, int button)
{
    Rect* mw = &scr.main_win;
    int size;
    int x, y; /* mappoint */
    int mod_x, mod_y; /* upper left coords of module clicked on */
    int mps_result;

    if (button == LC_MOUSE_MIDDLEBUTTON)
	return;

    pixel_to_mappoint(px, py, &x, &y);

    if (MP_TYPE(x,y) == CST_USED) {
	mod_x = MP_INFO(x,y).int_1;
	mod_y = MP_INFO(x,y).int_2;
    } else {
	mod_x = x;
	mod_y = y;
    }

    /* Bring up mappoint_stats for any right mouse click */
    /* XXX: Environmental (right click) MPS should show the clicked 
       square, not the master square */
    if (button == LC_MOUSE_RIGHTBUTTON) {
	mps_set(mod_x, mod_y, MPS_ENV);
	return;
    }

    /* Check rocket launches */
    /* XXX: put this in modules/rocket.c */
    /* XXX: wait for second click to ask for launch */
    if (button == LC_MOUSE_LEFTBUTTON) {
	if (MP_TYPE(mod_x,mod_y) >= CST_ROCKET_5 && 
	    MP_TYPE(mod_x,mod_y) <= CST_ROCKET_7) {
	  if (ask_launch_rocket_click (mod_x,mod_y)) {
	    launch_rocket (mod_x, mod_y);
	  }
	}
    }

    /* Handle multitransport */
    if (button == LC_MOUSE_LEFTBUTTON && 
	GROUP_IS_TRANSPORT(selected_module_group)) {
	if (mt_draw (px, py, MT_START)) {
	    /* We need to set mps to current location, since the user might 
	       click on the transport to see the mps */

	    mps_set(mod_x, mod_y, MPS_MAP);
	    return;
	}
    }

    /* Handle bulldozing */
    if (selected_module_type == CST_GREEN && button != LC_MOUSE_RIGHTBUTTON) {
	check_bulldoze_area (x, y);
	return;
    }

    /* Bring up mappoint_stats for certain left mouse clicks */
    /* XXX: Need to check market and port double-clicks here */
    if (MP_TYPE(x,y) != CST_GREEN) {
	mps_result = mps_set(mod_x, mod_y, MPS_MAP);

	if (mps_result >= 1) {
	    if (MP_GROUP(mod_x,mod_y) == GROUP_MARKET)
	    {
		clicked_market_cb (mod_x, mod_y);
		return;
	    }
	    else if (MP_GROUP(mod_x,mod_y) == GROUP_PORT)
	    {
		clicked_port_cb (mod_x, mod_y);
		return;
	    }
	}
	return;
    }

    /* OK, by now we are certain that the user wants to place the item.  
       Set the origin based on the size of the selected_module_type, and 
       see if the selected item will fit. */
    size = main_groups[selected_module_group].size;
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
    switch (place_item (x, y, selected_module_type)) {
    case 0:
	/* Success */
	break;
    case -1:
	/* Not enough money */
	no_credit_build_msg (selected_module_group);
	break;
    case -2:
	/* Improper port placement */
	/* TRANSLATORS: The part about the cup of tea is one of Ian's 
	   jokes, but the part about ports needing to be connected 
	   to rivers is true.  */
	if (yn_dial_box (_("WARNING"),
			 _("Ports need to be"),
			 _("connected to rivers!"),
			 _("Want to make a cup of tea?")) != 0)
	    while (yn_dial_box (_("TEA BREAK"),
				_("Boil->pour->wait->stir"),
				_("stir->pour->stir->wait->drink...ahhh"),
				_("Have you finished yet?")) == 0);
	break;
    }
}

void
do_mouse_other_buttons (int x, int y, int button)
{
    Rect* mw = &scr.main_win;

    if (0) {} /* XXX: Cute, very cute */

    /* main screen border scroll areas */

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

    /* This is the mini window. Clicking here move the main window 
       to this point 
    */

/*** Miniscreen selector buttons removed in CVS Revision 1.24 ***/

    /* this is the menu button */
    else if (mouse_in_rect (&scr.menu_button,x,y)) {
	activate_help ("menu.hlp");
    }

    /* this is the help button */
    else if (mouse_in_rect (&scr.help_button,x,y)) {
	activate_help ("index.hlp");
    }

    /* this is the results (stats) button */
    else if (mouse_in_rect (&scr.results_button,x,y)) {
	if (button == LC_MOUSE_RIGHTBUTTON)
	    return;
	window_results ();
    }

    /* XXX: WCK: This is broken; should be a mouse handler in 
       mps.c anyway */
    /* Advance mps screen if clicked on */
    else if (mouse_in_rect (&scr.mappoint_stats,x,y)) {
	if (button == LC_MOUSE_RIGHTBUTTON) {
	    switch (mps_global_style) {
	    case MPS_GLOBAL_FINANCE:
		activate_help ("finance.hlp");
		break;
	    case MPS_GLOBAL_OTHER_COSTS:
		activate_help ("other-costs.hlp");
		break;
	    case MPS_GLOBAL_HOUSING:
		activate_help ("housing.hlp");
		break;
	    }
	    return;
	}
	mps_global_advance();
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
    if (g == GROUP_MONUMENT && monument_bul_flag == 0) {
	if (yn_dial_box (_("WARNING"),
			 _("Bulldozing a monument costs"),
			 _("a lot of money."),
			 _("Want to bulldoze?")) == 0)
	    return;
	monument_bul_flag = 1;
    }
    else if (g == GROUP_RIVER && river_bul_flag == 0) {
	if (yn_dial_box (_("WARNING"),
			 _("Bulldozing a section of river"),
			 _("costs a lot of money."),
			 _("Want to bulldoze?")) == 0)
	    return;
	river_bul_flag = 1;
    }
    else if (g == GROUP_SHANTY && shanty_bul_flag == 0) {
	if (yn_dial_box (_("WARNING"),
			 _("Bulldozing a shanty town costs a"),
			 _("lot of money and may cause a fire."),
			 _("Want to bulldoze?")) == 0)
	    return;
	shanty_bul_flag = 1;
    }
    else if (g == GROUP_TIP) {
	ok_dial_box ("nobull-tip.mes", BAD, 0L);
	return;
    }

    bulldoze_item (xx,yy);
}

/* Called from event handler when middle button is held while moving
   the mouse.  Will probably handle multi-transport eventually */

/* XXX: Need to put this in a header somewhere */

void 
drag_screen (void) 
{
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

    adjust_main_origin (origin_x, origin_y, 1);
}

void
do_market_cb_template (int x, int y, int is_market_cb)
{
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

#if defined (NETWORK_ENABLE)
    if (MP_INFO(mcbx,mcby).flags != old_flags) {
	send_flags (mcbx,mcby);
    }
#endif
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
	     /* TRANSLATORS: Don't translate the leading "text" */
	     _("text -1 20 Choose the type of residence you want\n"));
    fprintf (tempfile,
	     "text -1 30 =====================================\n");
    fprintf (tempfile,
	     _("text -1 45 (LB=Low Birthrate HB=High Birthrate)\n"));
    fprintf (tempfile,
	     _("text -1 55 (LD=Low Deathrate HD=High Deathrate)\n"));
    fprintf (tempfile, 
	     _("text -1 85 Low Tech\n"));

    cost = get_group_cost(GROUP_RESIDENCE_LL);
    fprintf (tempfile, _("text 68 106 Cost %4d\n"), cost);
    fprintf (tempfile, "icon 85 120 reslowlow.csi\n");
    fprintf (tempfile, "button 83 118 52 52 return1\n");
    /* TRANSLATORS: Only translate "pop" <<for population>> */
    fprintf (tempfile, _("tbutton 82 180 return1 pop 50\n"));
    /* TRANSLATORS: Only translate LB, HD (low birth, high death) */
    fprintf (tempfile, _("text 89 195 LB HD\n"));

    cost = get_group_cost(GROUP_RESIDENCE_ML);
    fprintf (tempfile, _("text 155 106 Cost %4d\n"), cost);
    fprintf (tempfile, "icon 170 120 resmedlow.csi\n");
    fprintf (tempfile, "button 168 118 52 52 return2\n");
    fprintf (tempfile, _("tbutton 164 180 return2 pop 100\n"));
    fprintf (tempfile, _("text 175 195 HB LD\n"));

    cost = get_group_cost(GROUP_RESIDENCE_HL);
    fprintf (tempfile, _("text 238 106 Cost %4d\n"), cost);
    fprintf (tempfile, "icon 255 120 reshilow.csi\n");
    fprintf (tempfile, "button 253 118 52 52 return3\n");
    fprintf (tempfile, _("tbutton 250 180 return3 pop 200\n"));
    fprintf (tempfile, _("text 261 195 HB HD\n"));

    fprintf (tempfile, _("text -1 215 Click on one to select\n"));
    fprintf (tempfile, "text -1 225 ======================\n");

    if (((tech_level * 10) / MAX_TECH_LEVEL) > 2) {
	fprintf (tempfile, _("text -1 255 High Tech\n"));

	cost = get_group_cost(GROUP_RESIDENCE_LH);
	fprintf (tempfile, _("text 68 276 Cost %4d\n"), cost);
	fprintf (tempfile, "icon 85 290 reslowhi.csi\n");
	fprintf (tempfile, "button 83 288 52 52 return4\n");
	fprintf (tempfile, _("tbutton 78 350 return4 pop 100\n"));
	fprintf (tempfile, _("text 89 365 LB HD\n"));

	cost = get_group_cost(GROUP_RESIDENCE_MH);
	fprintf (tempfile, _("text 155 276 Cost %4d\n"), cost);
	fprintf (tempfile, "icon 170 290 resmedhi.csi\n");
	fprintf (tempfile, "button 168 288 52 52 return5\n");
	fprintf (tempfile, _("tbutton 164 350 return5 pop 200\n"));
	fprintf (tempfile, _("text 175 365 HB LD\n"));

	cost = get_group_cost(GROUP_RESIDENCE_HH);
	fprintf (tempfile, _("text 238 276 Cost %4d\n"), cost);
	fprintf (tempfile, "icon 255 290 reshihi.csi\n");
	fprintf (tempfile, "button 253 288 52 52 return6\n");
	fprintf (tempfile, _("tbutton 250 350 return6 pop 400\n"));
	fprintf (tempfile, _("text 261 365 HB HD\n"));
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
	    mt_length++;
	    MP_TYPE(x,y) = selected_module_type;
	    MP_GROUP(x,y) = mt_grp;
	    MP_INFO(x,y).flags |= FLAG_MULTI_TRANSPORT;
	    return 1;
	}
    } else if (MP_GROUP(x,y) == GROUP_BARE) { 
	mt_length++;
	MP_TYPE(x,y) = selected_module_type;
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
	    place_item (x,y,selected_module_type);
	    return 1;
	}
    } else if (MP_GROUP(x,y) == GROUP_BARE) { 
	place_item (x,y,selected_module_type);
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

    mt_length = 0;

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

#define STATUS_MESSAGE_LENGTH 80
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
	    status_message(0,0);
	}
	else if ((draw_ret = do_mt_draw(ox, cx, oy, cy, mt_perm))) {

	    print_total_money ();
	    mt_flag = 0;
	    draw_main_window_box (green (8)); 
	    status_message(0,0);
	} else {
	    /* This shouldn't happen.  Clean up and continue anyway.  */
	    mt_flag = 0;
	    status_message(0,0);
	    draw_main_window_box (green (8)); 
	}
	dx = 0; dy = 0;
	ox = 0; oy = 0;
	break;
      
    case MT_FAIL:       
	mt_flag = 0;
	draw_main_window_box (green (8));
	draw_ret = do_mt_draw(ox, dx, oy, dy, mt_erase);
	status_message(0,0);

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
	    snprintf(s,STATUS_MESSAGE_LENGTH-1,
		     _("Can't build %s over that!"), mt_name);
	} else {
	    snprintf(s,STATUS_MESSAGE_LENGTH-1,
		     _("%d sections of %s will cost %3d to build"),
		     mt_length, mt_name, 
		     mt_length * get_type_cost(selected_module_type));
	}

	status_message(0,s);
	dx = cx; dy = cy;
	break;

    case MT_START:
	/* XXX: we assume that a transport type is selected.   */

        if ((mt_grp = get_group_of_type(selected_module_type)) < 0 )
	  return 0;

	get_type_name(selected_module_type,mt_name);

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

    connect_transport_main_screen ();
    update_main_screen (0);
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
	} else {
	    mhandle_first = NULL;
	}
    } else if (mhandle->next == NULL) {
	mhandle->prev->next = NULL;
    } else {
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
    
