/* ---------------------------------------------------------------------- *
 * ldsvgui.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include "lcconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include "lcstring.h"
#include "ldsvgui.h"
#include "lcintl.h"
#include "screen.h"
#include "pbar.h"
#include "module_buttons.h"

/* this is for OS/2 - RVI */
#ifdef __EMX__
#include <sys/select.h>
#include <X11/Xlibint.h>      /* required for __XOS2RedirRoot */
#define chown(x,y,z)
#define OS2_DEFAULT_LIBDIR "/XFree86/lib/X11/lincity"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#if defined (TIME_WITH_SYS_TIME)
#include <time.h>
#include <sys/time.h>
#else
#if defined (HAVE_SYS_TIME_H)
#include <sys/time.h>
#else
#include <time.h>
#endif
#endif

#if defined (WIN32)
#include <winsock.h>
#if defined (__BORLANDC__)
#include <dir.h>
#include <dirent.h>
#include <dos.h>
#endif
#include <io.h>
#include <direct.h>
#include <process.h>
#endif

#if defined (HAVE_DIRENT_H)
#include <dirent.h>
#define NAMLEN(dirent) strlen((dirent)->d_name)
#else
#define dirent direct
#define NAMLEN(dirent) (dirent)->d_namlen
#if defined (HAVE_SYS_NDIR_H)
#include <sys/ndir.h>
#endif
#if defined (HAVE_SYS_DIR_H)
#include <sys/dir.h>
#endif
#if defined (HAVE_NDIR_H)
#include <ndir.h>
#endif
#endif

#include <ctype.h>
#include "common.h"
#ifdef LC_X11
#include <X11/cursorfont.h>
#endif
#include "lctypes.h"
#include "lin-city.h"
#include "cliglobs.h"
#include "engglobs.h"
#include "ldsvguts.h"
#include "fileutil.h"
#include "mouse.h"
#include "stats.h"

/* ---------------------------------------------------------------------- *
 * Private Fn Prototypes
 * ---------------------------------------------------------------------- */
int verify_city (char *cname);
void input_network_host (char *s);
void input_network_port (char *s);

/* ---------------------------------------------------------------------- *
 * Private Global Variables
 * ---------------------------------------------------------------------- */
#if defined (WIN32)
char LIBDIR[_MAX_PATH];
#elif defined (__EMX__)
#ifdef LIBDIR
#undef LIBDIR   /* yes, I know I shouldn't ;-) */
#endif
char LIBDIR[256];
#endif

char *lc_save_dir;
char save_names[10][42];

int modal_flag = 1;                   /* Not yet implemented */
int time_multiplex_stats = 0;

/* ---------------------------------------------------------------------- *
 * Public Functions
 * ---------------------------------------------------------------------- */
void
draw_prefs_cb (void)
{
    Rect* mw = &scr.main_win;
    int x, y;
    char* graphic;

    x = mw->x + 50;
    y = mw->y + 30;
    graphic = overwrite_transport_flag ? 
	    checked_box_graphic : unchecked_box_graphic;
    Fgl_putbox (x, y, 16, 16, graphic);

    y += 16;
    graphic = modal_flag ? checked_box_graphic : unchecked_box_graphic;
    Fgl_putbox (x, y, 16, 16, graphic);

    y += 16;
    graphic = time_multiplex_stats ? 
	    checked_box_graphic : unchecked_box_graphic;
    Fgl_putbox (x, y, 16, 16, graphic);

#if defined (LC_X11)
    y += 16;
    graphic = confine_flag ? checked_box_graphic : unchecked_box_graphic;
    Fgl_putbox (x, y, 16, 16, graphic);
#endif
}

void
do_prefs_buttons (int x, int y)
{
    int outx, outy, outh, outw;
    Rect* mw = &scr.main_win;
    if (x > mw->x + 50 && x < mw->x + 50 + 16) {
        if (y > mw->y + 30 && y < mw->y + 30 + 16) {
	    hide_mouse ();
	    overwrite_transport_flag = !overwrite_transport_flag;
	    draw_prefs_cb ();
	    redraw_mouse ();
	} else if (y > mw->y + 30 + 16 && y < mw->y + 30 + 2*16) {
	    hide_mouse ();
	    modal_flag = !modal_flag;
	    draw_prefs_cb ();
	    redraw_mouse ();
	} else if (y > mw->y + 30 + 2*16 && y < mw->y + 30 + 3*16) {
	    hide_mouse ();
	    time_multiplex_stats = !time_multiplex_stats;
	    draw_prefs_cb ();
	    redraw_mouse ();
#if defined (LC_X11)
	} else if (y > mw->y + 30 + 3*16 && y < mw->y + 30 + 4*16) {
	    hide_mouse ();
	    confine_flag = !confine_flag;
	    draw_prefs_cb ();
	    set_pointer_confinement ();
	    redraw_mouse ();
#endif
	}
    }
    outx = 370;
    outy = 387;
    outh = 12;
    outw = 3*8 + 4;
    if (x > mw->x + outx && x < mw->x + outx + outw &&
	y > mw->y + outy && y < mw->y + outy + outh)
    {
	close_prefs_screen ();
#ifdef USE_EXPANDED_FONT
	Fgl_setwritemode (WRITEMODE_OVERWRITE | FONT_EXPANDED);
#else
	Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
#endif
	refresh_main_screen ();
    }
}

void
do_prefs_mouse (int x, int y, int mbutton)
{
    Rect* mw = &scr.main_win;
    if (mouse_in_rect(mw, x, y)) {
	do_prefs_buttons (x, y);
	return;
    }
    /* If the user clicks outside of main window, cancel prefs?? */
    close_prefs_screen ();
#ifdef USE_EXPANDED_FONT
    Fgl_setwritemode (WRITEMODE_OVERWRITE | FONT_EXPANDED);
#else
    Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
#endif
    refresh_main_screen ();
}

void
do_prefs_screen (void)
{
    int x,y,w,h;
    Rect* mw = &scr.main_win;

#if defined (LC_X11) || defined (WIN32)
    prefs_drawn_flag = 1;
#endif

    hide_mouse ();
    Fgl_fillbox (mw->x, mw->y, mw->w, mw->h, LOAD_BG_COLOUR);
    Fgl_setfontcolors (LOAD_BG_COLOUR, TEXT_FG_COLOUR);
    Fgl_write (mw->x + 80, mw->y + 4*8, "Transport overwrite");
    Fgl_write (mw->x + 80, mw->y + 6*8, "Popup info to dialog boxes");
    Fgl_write (mw->x + 80, mw->y + 8*8, "Time multiplexed stats windows");
#if defined (LC_X11)
    Fgl_write (mw->x + 80, mw->y + 10*8, "Confine X pointer");
#endif

    x = 370;
    y = 387;
    h = 12;
    w = 3*8 + 4;
    Fgl_hline (mw->x + x, mw->y + y,
	       mw->x + x + w, HELPBUTTON_COLOUR);
    Fgl_hline (mw->x + x, mw->y + y + h, 
	       mw->x + x + w, HELPBUTTON_COLOUR);
    Fgl_line (mw->x + x, mw->y + y, 
	      mw->x + x, mw->y + y + h, HELPBUTTON_COLOUR);
    Fgl_line (mw->x + x + w, mw->y + y,
	      mw->x + x + w, mw->y + y + h, HELPBUTTON_COLOUR);
    Fgl_write (mw->x + x + 2, mw->y + y + 2, "OUT");

    draw_prefs_cb ();

    redraw_mouse ();
}

void
close_prefs_screen (void)
{
    prefs_flag = 0;
    prefs_drawn_flag = 0;
}

#if defined (NETWORK_ENABLE)
void
do_network_screen (void)
{
    Rect* mw = &scr.main_win;
    char s[200], t[200];
    unsigned short p;
    hide_mouse ();
    Fgl_fillbox (mw->x, mw->y, mw->w, mw->h
		 ,SAVE_BG_COLOUR);
    Fgl_setfontcolors (SAVE_BG_COLOUR, TEXT_FG_COLOUR);
    Fgl_write (mw->x + 100, mw->y + 15, "Connect to network game");
    Fgl_write (mw->x + 100, mw->y + 25, "Select host and port");

#if !defined (WIN32)
    redraw_mouse ();
#endif

    strcpy (s, DEFAULT_SOCK_HOST);
    input_network_host (s);
    sprintf (t, "%d", DEFAULT_SOCK_PORT);
    input_network_port (t);
    p = (unsigned short) atoi (t);
    join_network_game (s,p);

    db_flag = 0;
    cs_mouse_handler (0, -1, 0);
    cs_mouse_handler (0, 1, 0);
    hide_mouse ();
    Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
    refresh_main_screen ();
    redraw_mouse ();
}
#endif

void
do_save_city ()
{
    Rect* mw = &scr.main_win;
    char s[200], c;
    hide_mouse ();
    Fgl_fillbox (mw->x, mw->y, mw->w, mw->h
		 ,SAVE_BG_COLOUR);
    Fgl_setfontcolors (SAVE_BG_COLOUR, TEXT_FG_COLOUR);
    Fgl_write (mw->x + 100, mw->y + 15, _("Save a scene"));
    Fgl_write (mw->x + 8, mw->y + 35
	       ,_("Choose the number of the scene you want to save"));
    Fgl_write (mw->x + 110, mw->y + 210
	       ,_("Press space to cancel."));
    draw_save_dir (SAVE_BG_COLOUR);
#ifdef LC_X11
    redraw_mouse ();
    db_flag = 1;
    cs_mouse_handler (0, -1, 0);
    cs_mouse_handler (0, 1, 0);
    do
    {
	call_event ();
	c = x_key_value;
    }
    while (c == 0);
    x_key_value = 0;
#elif defined (WIN32)
    redraw_mouse ();
    while (0 == (c = GetKeystroke ()));	/* Wait for keystroke */
#else
    c = getchar ();
    redraw_mouse ();
#endif
    if (c > '0' && c <= '9')
    {
	Fgl_write (mw->x + 40, mw->y + 300
		   ,_("Type comment for the saved scene"));
	Fgl_write (mw->x + 16, mw->y + 310
		   ,_("The comment may be up to 40 characters"));
	Fgl_write (mw->x + 40, mw->y + 320
		   ,_("and may contain spaces or % . - + ,"));
	strcpy (s, &(save_names[c - '0'][2]));
	input_save_filename (s);
	remove_scene (save_names[c - '0']);
	sprintf (save_names[c - '0'], "%d_", c - '0');
	strcat (save_names[c - '0'], s);
	Fgl_fillbox (mw->x + 5, mw->y + 300
		     ,360, 30, SAVE_BG_COLOUR);
	Fgl_write (mw->x + 70, mw->y + 310
		   ,_("Saving city scene... please wait"));
	save_city (save_names[c - '0']);
    }
    db_flag = 0;
    cs_mouse_handler (0, -1, 0);
    cs_mouse_handler (0, 1, 0);
    hide_mouse ();
    Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
    save_flag = 0;
    refresh_main_screen ();
    redraw_mouse ();
}

void 
load_opening_city (char *s)
{
  char *cname = (char *) malloc (strlen (opening_path) + strlen (s) + 2);
  sprintf (cname, "%s%c%s", opening_path, PATH_SLASH, s);
  load_city (cname);
  free (cname);

  strcpy (given_scene, s);
  db_flag = 0;
  cs_mouse_handler (0, -1, 0);
  cs_mouse_handler (0, 1, 0);
  /* GCS:  Should I hide_mouse() here, as is done in do_load_city above? */
  hide_mouse ();
  Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
  refresh_main_screen ();
  suppress_ok_buttons = 1;
  update_avail_modules ();
  suppress_ok_buttons = 0;
  /* GCS: ?? */
  redraw_mouse ();
}

void
do_load_city (void)
{
    Rect* mw = &scr.main_win;
    char c;
    hide_mouse ();
    Fgl_fillbox (mw->x, mw->y, mw->w, mw->h
		 ,LOAD_BG_COLOUR);
    Fgl_setfontcolors (LOAD_BG_COLOUR, TEXT_FG_COLOUR);
    Fgl_write (mw->x + 140, mw->y + 15, _("Load a file"));
    Fgl_write (mw->x + 40, mw->y + 35
	       ,_("Choose the number of the scene you want"));
    Fgl_write (mw->x + 40, mw->y + 50
	       ,_("Entries coloured red are either not there,"));
    Fgl_write (mw->x + 44, mw->y + 60
	       ,_("or they are from an earlier version, they"));
    Fgl_write (mw->x + 110, mw->y + 70
	       ,_("might not load properly."));
    Fgl_write (mw->x + 110, mw->y + 210
	       ,_("Press space to cancel."));
    draw_save_dir (LOAD_BG_COLOUR);
    do
    {
#ifdef LC_X11
	db_flag = 1;
	redraw_mouse ();
	cs_mouse_handler (0, -1, 0);
	cs_mouse_handler (0, 1, 0);
	do
	{
	    call_event ();
	    c = x_key_value;
	}
	while (c == 0);
	x_key_value = 0;
#elif defined (WIN32)
	while (0 == (c = GetKeystroke ()));	/* Wait for keystroke */
#else
	c = getchar ();
#endif
	if (c > '0' && c <= '9')
	    if (strlen (save_names[c - '0']) < 1)
	    {
		redraw_mouse ();
		if (yn_dial_box (_("No scene.")
				 ,_("There is no save scene with this number.")
				 ,_("Do you want to")
				 ,_("try again?")) != 0)
		    c = 0;
		else
		    c = ' ';
		hide_mouse ();
	    }
    }
    while ((c <= '0' || c > '9') && c != ' ');
    redraw_mouse ();
    if (c > '0' && c <= '9')
    {
	if (yn_dial_box (_("Loading Scene")
			 ,_("Do you want to load the scene")
			 ,save_names[c - '0']
			 ,_("and forget the current game?")) != 0)
	{
	    Fgl_write (mw->x + 70, mw->y + 310
		       ,_("Loading scene...  please wait"));
	    load_saved_city (save_names[c - '0']);
	    refresh_pbars();
	}
    }
    db_flag = 0;
    cs_mouse_handler (0, -1, 0);
    cs_mouse_handler (0, 1, 0);
    hide_mouse ();
    Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
    load_flag = 0;
    refresh_main_screen ();
    suppress_ok_buttons = 1;
    update_avail_modules ();
    suppress_ok_buttons = 0;
    redraw_mouse ();
}

void
draw_save_dir (int bg_colour)
{
    Rect* mw = &scr.main_win;
    char *s, s2[200];
    int i, j, l;
#if defined (WIN32)
    char filespec[4];
#if defined(_MSC_VER)
    struct _finddata_t fileinfo;
#elif defined (__BORLANDC__)
    struct ffblk fileinfo;
#endif
    long fh;
#else
    struct dirent *ep;
    DIR *dp;
#endif
    if ((s = (char *) malloc (lc_save_dir_len + strlen (LC_SAVE_DIR) + 64)) == 0)
	malloc_failure ();
    strcpy (s, lc_save_dir);
    if (!directory_exists (s))
    {
	printf (_("Couldn't find the save directory %s\n"), s);
	free (s);
	return;
    }
    /* GCS FIX:  Technically speaking, there is a race condition here. */
#if defined (WIN32)
    _chdir (s);
#else
    dp = opendir (s);
#endif
    for (i = 1; i < 10; i++)
    {
	save_names[i][0] = 0;
#if defined (WIN32)
	sprintf (filespec, "%d_*", i);
#if defined (_MSC_VER)
	fh = _findfirst (filespec, &fileinfo);
#elif defined (__BORLANDC__)
	fh = findfirst (filespec, &fileinfo, FA_ARCH);
#endif
	if (fh != -1)
	{
#else
	    while ((ep = readdir (dp)))	/* extra brackets to stop warning */

	    {
		if (*(ep->d_name) == (i + '0')
		    && *((ep->d_name) + 1) == '_')
		{
#endif
		    sprintf (s2, "%2d ", i);
#if defined (WIN32)
#if defined (_MSC_VER)
		    strncpy (save_names[i], fileinfo.name, 40);
#elif defined (__BORLANDC__)
		    strncpy (save_names[i], fileinfo.ff_name, 40);
#endif
#else /* UNIX */
		    strncpy (save_names[i], ep->d_name, 40);
#endif
		    if (strlen (save_names[i]) > 2)
			strncat (s2, &(save_names[i][2]), 40);
		    else
			strcat (s2, "???");
#if defined (WIN32)
#if defined (_MSC_VER)
		    _findclose (fh);
#elif defined (__BORLANDC__)
		    findclose(&fileinfo);
#endif
		}
#else
	    }
	}
#endif
	if (strlen (save_names[i]) < 1)
	    sprintf (s2, " %d .....", i);
	else
	{
	    l = strlen (s2);
	    for (j = 0; j < l; j++)
		if (s2[j] == '_')
		    s2[j] = ' ';
	}
	if (verify_city (save_names[i]) == 0)
	    Fgl_setfontcolors (bg_colour, red (28));
	else
	    Fgl_setfontcolors (bg_colour, green (28));
	Fgl_write (mw->x + 24, mw->y + 10 * (10 + i), s2);
#if !defined (WIN32)
	rewinddir (dp);
#endif
    }
#if defined (WIN32)
    _chdir (LIBDIR);		/* go back... */
#else
    closedir (dp);
#endif
    Fgl_setfontcolors (bg_colour, TEXT_FG_COLOUR);
    free (s);
}

void
edit_string (char* s, unsigned int maxlen, int xpos, int ypos)
{
    char c;
    int i, t, on;
    c = 0;
    s[maxlen+1] = 0;
    t = strlen (s);
    for (i = 0; i < t; i++)
	if (s[i] == '_')
	    s[i] = ' ';
    while (c != 0xd && c != 0xa)
    {
	Fgl_write (xpos, ypos, s);
	Fgl_write (xpos + (strlen (s) * 8), ypos, "_");
	on = 1;
	get_real_time ();
	t = real_time;
#ifdef LC_X11
	call_event ();
	while ((c = x_key_value) == 0)
#elif defined (WIN32)
	    while ((c = GetKeystroke ()) == 0)
#else
		while ((c = vga_getkey ()) == 0)
#endif
		{
#ifdef LC_X11
		    call_event ();
#endif
		    get_real_time ();
		    if (real_time > t + 200) {
			if (on == 1) {
			    Fgl_write (xpos + (strlen (s) * 8),
				       ypos, " ");
			    on = 0;
			} else {
			    Fgl_write (xpos + (strlen (s) * 8),
				       ypos, "_");
			    on = 1;
			}
			get_real_time ();
			t = real_time;
		    }
		}
#ifdef LC_X11
	x_key_value = 0;
#endif
	if ((isalnum (c) || c == ' ' || c == '.' || c == '%' || c == ','
	     || c == '-' || c == '+') && strlen (s) < maxlen)
	{
	    t = strlen (s);
	    s[t] = c;
	    s[t + 1] = 0;
	} 
	else if (c == 0x7f && strlen (s) > 0) 
	{
	    Fgl_write (xpos + (strlen (s) * 8), ypos, " ");
	    s[strlen (s) - 1] = 0;
	}
    }
    t = strlen (s);
    for (i = 0; i < t; i++)
	if (s[i] == ' ')
	    s[i] = '_';
}

void
input_save_filename (char *s)
{
    Rect* mw = &scr.main_win;
    edit_string (s, 40, mw->x + 24, mw->y + 340);
}

void
input_network_host (char *s)
{
    Rect* mw = &scr.main_win;
    Fgl_write (mw->x + 50, mw->y + 240, "Host:");
    edit_string (s, 40, mw->x + 124, mw->y + 240);
}

void
input_network_port (char *s)
{
    Rect* mw = &scr.main_win;
    Fgl_write (mw->x + 50, mw->y + 280, "Port:");
    edit_string (s, 40, mw->x + 124, mw->y + 280);
}

#if 0
void
dump_tcore (void)
{
#ifdef ALLOW_TCORE_DUMP
  char s[200];
  int x, y, z, q, n;
  FILE *ofile;
  strcpy (s, getenv ("HOME"));
  strcat (s, "/");
  strcat (s, LC_SAVE_DIR);
  strcat (s, "/");
  strcat (s, "tcore.txt");
  if ((ofile = fopen (s, "w")) == NULL)
    {
      printf (_("Tcore file <%s> - "), s);
      do_error _("Can't open it!");
    }
  fprintf (ofile, _("Version=%d\n"), (int) VERSION_INT);
  q = sizeof (struct MAPPOINT);
  prog_box (_("Saving tcore"), 0);
  for (x = 0; x < WORLD_SIDE_LEN; x++)
    {
      for (y = 0; y < WORLD_SIDE_LEN; y++)
	{
	  for (z = 0; z < q; z++)
	    {
	      n = *(((unsigned char *) &mappoint[x][y]) + z);
	      fprintf (ofile, _("mappoint[%d][%d]b%d=%d\n")
		       ,x, y, z, n);
	    }
	  fprintf (ofile, _("mappointpol[%d][%d]=%d\n")
		   ,x, y, (int) mappointpol[x][y]);
	  fprintf (ofile, _("mappoint[%d][%d].type=%d\n")
		   ,x, y, (int) mappoint[x][y].type);
	}
      prog_box ("", (90 * x) / WORLD_SIDE_LEN);
    }
  fprintf (ofile, _("Origx=%d\n"), main_screen_originx);
  fprintf (ofile, _("Origy=%d\n"), main_screen_originy);
  fprintf (ofile, _("Total time=%d\n"), total_time);
  for (x = 0; x < MAX_NUMOF_SUBSTATIONS; x++)
    {
      fprintf (ofile, _("SustationX[%d]=%d\n"), x, substationx[x]);
      fprintf (ofile, _("Substation[%d]=%d\n"), x, substationy[x]);
    }
  prog_box ("", 92);
  fprintf (ofile, _("Num of substations=%d\n"), numof_substations);
  for (x = 0; x < MAX_NUMOF_MARKETS; x++)
    {
      fprintf (ofile, _("MarketX[%d]=%d\n"), x, marketx[x]);
      fprintf (ofile, _("MarketY[%d]=%d\n"), x, markety[x]);
    }
  prog_box ("", 94);
  fprintf (ofile, _("numof_markets=%d\n"), numof_markets);
  fprintf (ofile, _("people_pool=%d\n"), people_pool);
  fprintf (ofile, _("total_money=%d\n"), total_money);
  fprintf (ofile, _("income_tax_rate=%d\n"), income_tax_rate);
  fprintf (ofile, _("coal_tax_rate=%d\n"), coal_tax_rate);
  fprintf (ofile, _("dole_rate=%d\n"), dole_rate);
  fprintf (ofile, _("transport_cost_rate=%d\n"), transport_cost_rate);
  fprintf (ofile, _("goods_tax_rate=%d\n"), goods_tax_rate);
  fprintf (ofile, _("export_tax=%d\n"), export_tax);
  fprintf (ofile, _("export_tax_rate=%d\n"), export_tax_rate);
  fprintf (ofile, _("import_cost=%d\n"), import_cost);
  fprintf (ofile, _("import_cost_rate=%d\n"), import_cost_rate);
  fprintf (ofile, _("tech_level=%d\n"), tech_level);
  fprintf (ofile, _("tpopulation=%d\n"), tpopulation);
  fprintf (ofile, _("tstarving_population=%d\n"), tstarving_population);
  fprintf (ofile, _("tunemployed_population=%d\n"), tunemployed_population);
  fprintf (ofile, _("waste_goods=%d\n"), waste_goods);
  fprintf (ofile, _("power_made=%d\n"), power_made);
  fprintf (ofile, _("power_used=%d\n"), power_used);
  fprintf (ofile, -("coal_made=%d\n"), coal_made);
  fprintf (ofile, _("coal_used=%d\n"), coal_used);
  fprintf (ofile, _("goods_made=%d\n"), goods_made);
  fprintf (ofile, _("goods_used=%d\n"), goods_used);
  fprintf (ofile, _("ore_made=%d\n"), ore_made);
  fprintf (ofile, _("ore_used=%d\n"), ore_used);
  fprintf (ofile, _("diff_old_population=%d\n"), diff_old_population);
  prog_box ("", 96);
  prog_box ("", 98);
  fprintf (ofile, _("rockets_launched=%d\n"), rockets_launched);
  fprintf (ofile, _("rockets_launched_success=%d\n"), rockets_launched_success);
  fprintf (ofile, _("coal_survey_done=%d\n"), coal_survey_done);
  prog_box ("", 99);

  fclose (ofile);
  prog_box ("", 100);
#endif
}
#endif /* 0 */


void
do_get_nw_server (void)
{
    Rect* mw = &scr.main_win;
    char c;
    hide_mouse ();
    Fgl_fillbox (mw->x, mw->y, mw->w, mw->h, NW_BG_COLOUR);
    Fgl_setfontcolors (LOAD_BG_COLOUR, TEXT_FG_COLOUR);
    Fgl_write (mw->x + 140, mw->y + 15, _("Choose network server"));
    Fgl_write (mw->x + 40, mw->y + 35
	       ,_("Please enter the address and port of the server"));
    Fgl_write (mw->x + 110, mw->y + 210
	       ,_("Press space to cancel."));
    draw_save_dir (NW_BG_COLOUR);
    do
    {
#ifdef LC_X11
	db_flag = 1;
	redraw_mouse ();
	cs_mouse_handler (0, -1, 0);
	cs_mouse_handler (0, 1, 0);
	do
	{
	    call_event ();
	    c = x_key_value;
	}
	while (c == 0);
	x_key_value = 0;
#elif defined (WIN32)
	while (0 == (c = GetKeystroke ()));	/* Wait for keystroke */
#else
	c = getchar ();
#endif
	if (c > '0' && c <= '9')
	    if (strlen (save_names[c - '0']) < 1)
	    {
		redraw_mouse ();
		if (yn_dial_box (_("No scene.")
				 ,_("There is no save scene with this number.")
				 ,_("Do you want to")
				 ,_("try again?")) != 0)
		    c = 0;
		else
		    c = ' ';
		hide_mouse ();
	    }
    }
    while ((c <= '0' || c > '9') && c != ' ');
    redraw_mouse ();
    if (c > '0' && c <= '9')
    {
	if (yn_dial_box (_("Loading Scene")
			 ,_("Do you want to load the scene")
			 ,save_names[c - '0']
			 ,_("and forget the current game?")) != 0)
	{
	    Fgl_write (mw->x + 70, mw->y + 310
		       ,_("Loading scene...  please wait"));
	    load_saved_city (save_names[c - '0']);
	}
    }
    db_flag = 0;
    cs_mouse_handler (0, -1, 0);
    cs_mouse_handler (0, 1, 0);
    hide_mouse ();
    Fgl_setfontcolors (TEXT_BG_COLOUR, TEXT_FG_COLOUR);
    refresh_main_screen ();
    suppress_ok_buttons = 1;
    update_avail_modules ();
    suppress_ok_buttons = 0;
    redraw_mouse ();
}
