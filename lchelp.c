/* ---------------------------------------------------------------------- *
 * lchelp.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include "lcconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include "lcstring.h"
#include <ctype.h>
#include "common.h"
#include "lctypes.h"
#include "fileutil.h"
#include "lin-city.h"
#include "lchelp.h"
#include "mouse.h"
#include "geometry.h"
#include "lcintl.h"
#include "module_buttons.h"

/* About help history:
   History count starts at 0, but is immediately incremented to 1.
   Current page is stored at help_button_history[help_history_count-1],
   which is the top of the stack.  When "back" is clicked, history count 
   is decremented by 2, and then incremented by one.
   */

void
activate_help (char *hp)
{
    help_flag = 1;
    help_history_count = 0;
    help_return_val = 0;
#ifdef USE_EXPANDED_FONT
    Fgl_setwritemode (WRITEMODE_MASKED | FONT_EXPANDED);
#else
    Fgl_setfontcolors (HELPBACKGROUNDCOLOUR, TEXT_FG_COLOUR);
#endif
    draw_help_page (hp);
}

void
do_help_mouse (int x, int y, int mbutton)
{
    Rect* mw = &scr.main_win;
    if (mouse_in_rect(mw, x, y)) {
	do_help_buttons (x, y);
	return;
    }
    if (block_help_exit)
	return;

    help_flag = 0;
#ifdef USE_EXPANDED_FONT
    Fgl_setwritemode (WRITEMODE_OVERWRITE | FONT_EXPANDED);
#else
    Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
#endif
    refresh_main_screen ();
}

void
draw_help_page (char *helppage)
{
    Rect* mw = &scr.main_win;
    char *helppage_full, *helppage_short;
    int i, y;
    FILE *inf;
    char help_line[MAX_HELP_LINE];

    /* Return pages have arguments.  It is always true that "-2" means "Out" 
       and "-1" means "Back".  Semantics for other arguments depend upon
       the name of the source page (e.g. load game or choose residence). 
       Most of the times (except "Back"), this will exit the help system. */
    if (strncmp (helppage, "return", 6) == 0) {
	sscanf (&(helppage[6]), "%d", &help_return_val);

	/* If "Back" was clicked */
	if (help_return_val == -1 && help_history_count > 1) {
	    strcpy (helppage, help_button_history[help_history_count - 2]);
	    help_history_count -= 2;
	    goto continue_with_help;
	}

	/* XXX: WCK: residential selection is really ugly */
	if (help_history_count > 0 &&
	    strcmp (help_button_history[help_history_count - 1],
		    "res.tmp") == 0)
	{
	    switch (help_return_val) {
	    case (-2):
	    case (-1):
	    case (0):
#if defined (commentout)
	    case (1):
		selected_module_type = CST_RESIDENCE_LL;
		selected_module_cost = get_group_cost (GROUP_RESIDENCE_LL);
		break;
	    case (2):
		selected_module_type = CST_RESIDENCE_ML;
		selected_module_cost = get_group_cost (GROUP_RESIDENCE_ML);
		break;
	    case (3):
		selected_module_type = CST_RESIDENCE_HL;
		selected_module_cost = get_group_cost (GROUP_RESIDENCE_HL);
		break;
	    case (4):
		selected_module_type = CST_RESIDENCE_LH;
		selected_module_cost = get_group_cost (GROUP_RESIDENCE_LH);
		break;
	    case (5):
		selected_module_type = CST_RESIDENCE_MH;
		selected_module_cost = get_group_cost (GROUP_RESIDENCE_MH);
		break;
	    case (6):
		selected_module_type = CST_RESIDENCE_HH;
		selected_module_cost = get_group_cost (GROUP_RESIDENCE_HH);
		break;
#endif
	    case (1):
		set_selected_module (CST_RESIDENCE_LL);
		break;
	    case (2):
		set_selected_module (CST_RESIDENCE_ML);
		break;
	    case (3):
		set_selected_module (CST_RESIDENCE_HL);
		break;
	    case (4):
		set_selected_module (CST_RESIDENCE_LH);
		break;
	    case (5):
		set_selected_module (CST_RESIDENCE_MH);
		break;
	    case (6):
		set_selected_module (CST_RESIDENCE_HH);
		break;
	    }
	}
	else if (help_history_count > 0 &&
		 strcmp (help_button_history[help_history_count - 1],
			 "menu.hlp") == 0)
	{
	    switch (help_return_val) {
	    case 1:
		save_flag = 1;
		break;
	    case 2:
		prefs_flag = 1;
		break;
	    case 3:
		quit_flag = 1;
		break;
	    }
	}
	else if (help_history_count > 0 && 
		 strcmp (help_button_history[help_history_count - 1],
			 "opening.hlp") == 0)
	{
	    switch (help_return_val)
	    {
	    case (-2):
	    case (-1):
	    case (0):
		/* Random villiage */
		new_city (&main_screen_originx, &main_screen_originy, 1);
		adjust_main_origin (main_screen_originx,main_screen_originy,0);
		break;
	    case (1):
		/* Bare board */
		new_city (&main_screen_originx, &main_screen_originy, 0);
		adjust_main_origin (main_screen_originx,main_screen_originy,0);
		break;
	    case (2):
		/* Network start */
		network_flag = 1;
		break;
	    }
	}
	else if (help_history_count > 0 && 
		 strcmp (help_button_history[help_history_count - 1],
			 "newgame.hlp") == 0)
	{
	    switch (help_return_val)
	    {
	    case (0):
		/* Random villiage */
		new_city (&main_screen_originx, &main_screen_originy, 1);
		adjust_main_origin (main_screen_originx,main_screen_originy,0);
		break;
	    case (1):
		/* Bare board */
		new_city (&main_screen_originx, &main_screen_originy, 0);
		adjust_main_origin (main_screen_originx,main_screen_originy,0);
		break;
	    case (2):
		/* Network start */
		network_flag = 1;
		break;
	    }
	}
	else if (help_history_count > 0 &&
		 strcmp (help_button_history[help_history_count - 1],
			 "openload.hlp") == 0)
	{
	    switch (help_return_val)
	    {
	    case (-2):
	    case (-1):
	    case (0):
		new_city (&main_screen_originx, &main_screen_originy, 1);
		adjust_main_origin (main_screen_originx,main_screen_originy,0);
		break;
	    case (1):
		redraw_mouse ();
		load_opening_city ("good_times.scn");
		adjust_main_origin (main_screen_originx,main_screen_originy,0);
		hide_mouse ();
		break;
	    case (2):
		load_opening_city ("bad_times.scn");
		adjust_main_origin (main_screen_originx,main_screen_originy,0);
		break;
	    case (9):
		load_flag = 1;
		break;
	    }
	}
	else if (help_history_count > 0 &&
		 strcmp (help_button_history[help_history_count - 1],
			 "loadgame.hlp") == 0)
	{
	    switch (help_return_val)
	    {
	    case (1):
		redraw_mouse ();
		load_opening_city ("good_times.scn");
		adjust_main_origin (main_screen_originx,main_screen_originy,0);
		hide_mouse ();
		break;
	    case (2):
		load_opening_city ("bad_times.scn");
		adjust_main_origin (main_screen_originx,main_screen_originy,0);
		break;
	    case (9):
		load_flag = 1;
		break;
	    }
	}
	else if (help_history_count > 0 &&
		 strcmp (help_button_history[help_history_count - 1],
			 "ask-dir.hlp") == 0)
	{
	    if (help_return_val == 1 || help_return_val == -2)
		do_error ("User exited");
	}

	block_help_exit = 0;
	help_flag = 0;
	/* Fix origin */
#ifdef USE_EXPANDED_FONT
	Fgl_setwritemode (WRITEMODE_OVERWRITE | FONT_EXPANDED);
#else
	Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
#endif
	refresh_main_screen ();
	return;
    }

 continue_with_help:
    /* This buffer is just a copy of helppage.  Sometimes helppage is an 
       entry within help_button_s[], which gets overwritten when the page 
       is parsed. */
    if ((helppage_short = (char*) malloc (strlen(helppage) + 1)) == 0) {
	malloc_failure ();
    }
    strcpy (helppage_short, helppage);

    /* Right click on mini-screen */
    if (strncmp (helppage, "mini-screen.hlp", 15) == 0) {
	draw_big_mini_screen ();
    } else if (strncmp (helppage, "mini-in-main.hlp", 17) == 0) {
	/* do nothing */
    } else {
	/* This buffer is for the full path of the help file.
	   The file might be either in the help directory (most cases),
	   or in the temp directory (dynamically created pages). */
	if ((helppage_full = (char *) malloc (lc_save_dir_len 
					      + strlen (help_path) 
					      + strlen(helppage) + 2)) == 0) {
	    malloc_failure ();
	}

	/* Open the file */
	sprintf (helppage_full, "%s%s", help_path, helppage);
	if ((inf = fopen (helppage_full, "r")) == 0) {
	    sprintf (helppage_full, "%s%c%s", lc_save_dir, 
		     PATH_SLASH, helppage);
	    if ((inf = fopen (helppage_full, "r")) == 0) {
		sprintf (helppage_full, "%s%s", help_path, HELPERRORPAGE);
		if ((inf = fopen (helppage_full, "r")) == 0)
		    do_error ("Help error");
	    }
	}

	/* Parse and render help file */
	numof_help_buttons = 0;
	Fgl_fillbox (mw->x, mw->y, mw->w, mw->h, HELPBACKGROUNDCOLOUR);
	while (feof (inf) == 0) {
	    if (fgets (help_line, MAX_HELP_LINE, inf) == 0)
		break;
	    undosify_string (help_line);  /* GCS testing... */
	    parse_helpline (help_line);
	}
	fclose (inf);

	/* For ask-dir, we add path info */
	if (strncmp (helppage_short, "ask-dir.hlp", 11) == 0) {
	    parse_helpline ("tcolour 0 255");
	    y = 100;
	    for (i = 0; i < askdir_lines; i++) {
		sprintf (help_line, "text -1 %d %s", y, askdir_path[i]);
		parse_helpline (help_line);
		y += 14;
	    }
	}
	free (helppage_full);
    }

    /* At this point, most of the page has been rendered.  Now we have 
       to draw in the "BACK" and "OUT" buttons.  */
    if (help_history_count > 0) {
	parse_helpline ("tcolour 122 153");
	parse_helpline (_("tbutton 4 387 return-1 BACK"));
    }
    parse_helpline ("tcolour 188 153");
    parse_helpline (_("tbutton 370 387 return-2 OUT"));
    parse_helpline ("tcolour -1 -1");
#if defined (commentout)
    if (help_history_count > 0) {
	parse_helpline ("tcolour 122 153");
	snprintf (help_line, MAX_HELP_LINE, "tbutton %d %d return-1 %s",
		  4, mw->h - 13, _("BACK"));
	parse_helpline (help_line);
    }
    parse_helpline ("tcolour 188 153");
    snprintf (help_line, MAX_HELP_LINE, "tbutton %d %d return-2 %s",
	      mw->w - 34, mw->h - 13, _("OUT"));
    parse_helpline (help_line);
    parse_helpline ("tcolour -1 -1");
#endif

    /* Add help page to history.  If history is going to overflow, 
       throw out oldest page.  */
    strcpy (help_button_history[help_history_count], helppage_short);
    help_history_count++;
    if (help_history_count >= MAX_HELP_HISTORY) {
	for (i = 0; i < (MAX_HELP_HISTORY - 1); i++)
	    strcpy (help_button_history[i], help_button_history[i + 1]);
	help_history_count--;
    }

    free (helppage_short);
}

void
refresh_help_page (void)
{
    help_history_count -= 1;
    draw_help_page (help_button_history[help_history_count]);
}

void
parse_helpline (char *s)
{
    if (strncmp (s, "text", 4) == 0)
	parse_textline (s);
    else if (strncmp (s, "icon", 4) == 0)
	parse_iconline (s);
    else if (strncmp (s, "button", 6) == 0)
	parse_buttonline (s);
    else if (strncmp (s, "tbutton", 7) == 0)
	parse_tbuttonline (s);
    else if (strncmp (s, "tcolour", 7) == 0)
	parse_tcolourline (s);
}

void
parse_tcolourline (char *st)
{
  char s[100];
  int f, b;
  strcpy (s, st);		/* hpux fix?  we can live with this. */

  sscanf (s, "tcolour %d %d", &f, &b);
  if (f < 0 || b < 0)
    Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
  else
    Fgl_setfontcolors (b, f);
}

void
parse_textline (char *st)
{
    Rect* mw = &scr.main_win;
    int i, j, x, y;
    sscanf (st, "text %d %d", &x, &y);
    /* find start of string */
    i = 0;
    for (j = 0; j < 3; j++)
    {
	while (isspace (st[i]) == 0)
	{
	    if (st[i] == 0)
		return;		/* just silently ignore */

	    i++;
	}
	while (isspace (st[i]) != 0)
	{
	    if (st[i] == 0)
		return;
	    i++;
	}
    }
    st += i;
    /* get rid of the newline */
    if (st[strlen (st) - 1] == 0xa)
	st[strlen (st) - 1] = 0;
    if (x < 0) {
	/* centre text if x is negative */
	x = (mw->w / 2) - (strlen (st) * 4);
	if (x < 0)
	    return;			/* line was too long */
    } else {
	/* otherwise adjust x location to within center zone */
	x = x + (mw->w - 440) / 2;
    }
    /* check to see if text runs off the end */
    if ((int) (strlen (st) * 8) > (mw->w - x))
	return;
    Fgl_write (mw->x + x, mw->y + y, st);
}


void
parse_iconline (char *st)
{
    int i, j, x, y;
    sscanf (st, "icon %d %d", &x, &y);
    /* find start of string */
    i = 0;
    for (j = 0; j < 3; j++)
    {
	while (isspace (st[i]) == 0)
	{
	    if (st[i] == 0)
		return;		/* just silently ignore */

	    i++;
	}
	while (isspace (st[i]) != 0)
	{
	    if (st[i] == 0)
		return;
	    i++;
	}
    }
    st += i;
    /* get rid of the newline */
    if (st[strlen (st) - 1] == 0xa)
	st[strlen (st) - 1] = 0;
    draw_help_icon (x, y, st);
}

void
draw_help_icon (int x, int y, char *icon)
{
    Rect* mw = &scr.main_win;
    int i, l, w, h;
    char ss[100];
    FILE *inf;
    strcpy (ss, graphic_path);
    strcat (ss, icon);
    if ((inf = fopen (ss, "rb")) == NULL) {
	return;
    }
    fseek (inf, 0L, SEEK_END);
    l = ftell (inf);
    fseek (inf, 0L, SEEK_SET);
    if (l == 256)
	w = h = 16;
    else if (l == 1024)
	w = h = 32;
    else if (l == 2304)
	w = h = 48;
    else if (l == 4096)
	w = h = 64;
    else
    {
	fclose (inf);
	return;
    }
    for (i = 0; i < l; i++)
	*(help_graphic + i) = fgetc (inf);
    fclose (inf);

    /* Adjust x location to within center zone */
    x = x + (mw->w - 440) / 2;

    if (x > 0 && y > 0 && ((x + w) < mw->w) && ((y + h) < mw->h))
	Fgl_putbox (mw->x + x, mw->y + y, w, h, help_graphic);
    return;
}

void
parse_buttonline (char *st)
{
    Rect* mw = &scr.main_win;
    int i, j, x, y, w, h;
    sscanf (st, "button %d %d %d %d", &x, &y, &w, &h);
    /* find start of string */
    i = 0;
    for (j = 0; j < 5; j++)
    {
	while (isspace (st[i]) == 0)
	{
	    if (st[i] == 0)
		return;		/* just silently ignore */

	    i++;
	}
	while (isspace (st[i]) != 0)
	{
	    if (st[i] == 0)
		return;
	    i++;
	}
    }
    st += i;
    /* get rid of the newline */
    if (st[strlen (st) - 1] == 0xa)
	st[strlen (st) - 1] = 0;

    if (x < 0) {
	/* centre x of box if x is negative */
	x = (mw->w / 2) - (w / 2);
	if (x < 0)
	    return;			/* line was too long */
    } else {
	/* otherwise adjust x location to within center zone */
	x = x + (mw->w - 440) / 2;
    }

    /* see if the button runs off the end */
    if ((x + w) > mw->w)
	return;
    if (numof_help_buttons >= MAX_NUMOF_HELP_BUTTONS)
	return;
    help_button_x[numof_help_buttons] = x + mw->x;
    help_button_y[numof_help_buttons] = y + mw->y;
    help_button_w[numof_help_buttons] = w;
    help_button_h[numof_help_buttons] = h;
    if (strlen (st) >= MAX_LENOF_HELP_FILENAME)
	return;
    strcpy (help_button_s[numof_help_buttons], st);
    numof_help_buttons++;
    /* draw the box */
    Fgl_hline (mw->x + x, mw->y + y,
	       mw->x + x + w, HELPBUTTON_COLOUR);
    Fgl_hline (mw->x + x, mw->y + y + h, 
	       mw->x + x + w, HELPBUTTON_COLOUR);
    Fgl_line (mw->x + x, mw->y + y, 
	      mw->x + x, mw->y + y + h, HELPBUTTON_COLOUR);
    Fgl_line (mw->x + x + w, mw->y + y,
	      mw->x + x + w, mw->y + y + h, HELPBUTTON_COLOUR);
}

void
do_help_buttons (int x, int y)
{
  int i;
  if (numof_help_buttons <= 0)
    return;
  for (i = 0; i < numof_help_buttons; i++)
    if (x > help_button_x[i]
	&& x < (help_button_x[i] + help_button_w[i])
	&& y > help_button_y[i]
	&& y < (help_button_y[i] + help_button_h[i]))
      {
	hide_mouse ();
	draw_help_page (help_button_s[i]);
	redraw_mouse ();
	break;
      }
}

void
parse_tbuttonline (char *st)
{
    char s[100], ss[120], s1[100];
    int i, j, x, y;
    strcpy (s1, st);		/* hpux fix?  we can live with this. */

    sscanf (s1, "tbutton %d %d %s", &x, &y, s);
    /* find start of string */
    i = 0;
    for (j = 0; j < 4; j++) {
	while (isspace (st[i]) == 0) {
	    if (st[i] == 0)
		return;		/* just silently ignore */
	    i++;
	}
	while (isspace (st[i]) != 0) {
	    if (st[i] == 0)
		return;
	    i++;
	}
    }
    st += i;
    /* get rid of the newline */
    if (st[strlen (st) - 1] == 0xa)
	st[strlen (st) - 1] = 0;
    if (x < 0)
	x -= 2;			/* needed to keep text centred */

    sprintf (ss, "text %d %d ", x + 2, y + 2);
    strcat (ss, st);
    parse_textline (ss);
    sprintf (ss, "button %d %d %d %d %s", x, y, (strlen (st) * 8) + 4, 12, s);
    parse_buttonline (ss);
}
