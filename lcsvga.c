/* ---------------------------------------------------------------------- *
 * lcsvga.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include "lcconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lcstring.h"
#include "common.h"
#include "lctypes.h"
#include "lin-city.h"
#include "mouse.h"
#include "engglobs.h"
/* #include "clistubs.h" */
#include "engine.h"
#include "screen.h"
#include "lcintl.h"
/* #include "climsg.h" */

unsigned char mouse_pointer[] = {
    255, 255, 255, 255, 1, 1, 1, 1,
    255, 1, 1, 1, 255, 255, 1, 1,
    255, 1, 255, 255, 255, 1, 255, 1,
    255, 1, 255, 1, 1, 255, 1, 1,
    1, 255, 255, 1, 1, 1, 255, 1,
    1, 255, 1, 255, 1, 1, 1, 1,
    1, 1, 255, 1, 255, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1
};
unsigned char under_mouse_pointer[8 * 8];

static int vga_mode = -1;

void lc_mouse_handler(int button, int dx, int dy, int dz,
		      int drx, int dry, int drz);
#if defined (commentout)
void lc_mouse_handler(int button, int dx, int dy);
#endif

/* ---------------------------------------------------------------------- *
 * Profiling routines
 * ---------------------------------------------------------------------- */
#ifdef CS_PROFILE
void
FPgl_write (int a, int b, char *s)
{
    gl_write (a, b, s);
}
void
FPgl_getbox (int a, int b, int c, int d, void *e)
{
    gl_getbox (a, b, c, d, e);
}
void
FPgl_putbox (int a, int b, int c, int d, void *e)
{
    gl_putbox (a, b, c, d, e);
}
void
FPgl_fillbox (int a, int b, int c, int d, int e)
{
    gl_fillbox (a, b, c, d, e);
}
void
FPgl_hline (int a, int b, int c, int d)
{
    gl_hline (a, b, c, d);
}
void
FPgl_line (int a, int b, int c, int d, int e)
{
    gl_line (a, b, c, d, e);
}
void
FPgl_setpixel (int a, int b, int c)
{
    gl_setpixel (a, b, c);
}
#endif

/* ---------------------------------------------------------------------- *
 * Mouse routines
 * ---------------------------------------------------------------------- */
void
mouse_setup(void)
{
    int m;
    m = mouse_init("/dev/mouse", vga_getmousetype(), MOUSE_DEFAULTSAMPLERATE);
    mouse_setxrange(0, 640 - 1);
    mouse_setyrange(0, 480 - 1);
    mouse_setwrap(MOUSE_NOWRAP);
    if (m != 0) {
	do_error("Can't initialise mouse");
    }
    cs_mouse_x = 0;
    cs_mouse_y = 0;
    cs_mouse_button = 0;
    cs_mouse_xmax = 640 - 1;
    cs_mouse_ymax = 480 - 1;

    /* GCS -- This is meant to fix the upper left cursor problem,
     *        but doesnt.  May 11, 2003 				*/
    mox = 1; moy = 1;
    Fgl_getbox(mox, moy, 8, 8, under_mouse_pointer);

    mouse_seteventhandler(lc_mouse_handler);
}

void
mouse_set_range (int width, int height)
{
    debug_printf ("setting mouse range: %d %d\n", width, height);
    mouse_setxrange(0, width - 1);
    mouse_setyrange(0, height - 1);
    cs_mouse_xmax = width - 1;
    cs_mouse_ymax = height - 1;
}

void
set_vga_mode (void)
{
  /* If vga_mode is not set by command line, use vga_getdefaultmode() 
     to get mode.  Supported modes are 10, 11, 12 or 13.
   */
  if (vga_mode == -1) {
    vga_mode = vga_getdefaultmode();
    if (vga_mode > 13 || vga_mode < 10)
      vga_mode = 10;
  }
  vga_setmode (vga_mode);
  gl_setcontextvga (vga_mode);
  init_mouse();
  switch (vga_mode) 
    {
    case 10:
      resize_geometry (640,480);
      break;
    case 11:
      resize_geometry (800,600);
      break;
    case 12:
      resize_geometry (1024,768);
      break;
    case 13:
      resize_geometry (1280,1024);
      break;
    default:
      do_error ("illegal vga mode");
    }
}

void
lc_mouse_handler(int button, int dx, int dy, int dz,
		 int drx, int dry, int drz)
{
    static int old_mouse_button = 0;
    int mouse_button_change = old_mouse_button ^ button;

    if (mouse_button_change & MOUSE_LEFTBUTTON) {
	if (button & MOUSE_LEFTBUTTON) {
	    cs_mouse_handler(LC_MOUSE_LEFTBUTTON | LC_MOUSE_PRESS, dx, dy);
	} else {
	    cs_mouse_handler(LC_MOUSE_LEFTBUTTON | LC_MOUSE_RELEASE, dx,
			     dy);
	}
	dx = dy = 0;
    }
    if (mouse_button_change & MOUSE_MIDDLEBUTTON) {
	if (button & MOUSE_MIDDLEBUTTON) {
	    cs_mouse_handler(LC_MOUSE_MIDDLEBUTTON | LC_MOUSE_PRESS, dx,
			     dy);
	} else {
	    cs_mouse_handler(LC_MOUSE_MIDDLEBUTTON | LC_MOUSE_RELEASE, dx,
			     dy);
	}
	dx = dy = 0;
    }
    if (mouse_button_change & MOUSE_RIGHTBUTTON) {
	if (button & MOUSE_RIGHTBUTTON) {
	    cs_mouse_handler(LC_MOUSE_RIGHTBUTTON | LC_MOUSE_PRESS, dx,
			     dy);
	} else {
	    cs_mouse_handler(LC_MOUSE_RIGHTBUTTON | LC_MOUSE_RELEASE, dx,
			     dy);
	}
	dx = dy = 0;
    }
    if (dx || dy) {
	cs_mouse_handler(0, dx, dy);
    }
    old_mouse_button = button;
}

int 
lc_get_keystroke (void)
{
    return vga_getkey ();
}


/* ---------------------------------------------------------------------- *
 * Other routines
 * ---------------------------------------------------------------------- */
void
parse_args (int argc, char **argv)
{
  int option;
  extern char *optarg;

  /* GCS FIX:  Need to print usage and exit when illegal option spec'd */
  while ((option = getopt (argc, argv, "wR:G:B:m:")) != EOF)
    {
      switch (option)
	{
	case 'm':
	  sscanf (optarg, "%d", &vga_mode);
	  if (vga_mode > 13 || vga_mode < 10)
	    vga_mode = -1;
	  break;
	case 'w':
	  gamma_correct_red = GAMMA_CORRECT_RED;
	  gamma_correct_green = GAMMA_CORRECT_GREEN;
	  gamma_correct_blue = GAMMA_CORRECT_BLUE;
	  break;
	case 'R':
	  sscanf (optarg, "%f", &gamma_correct_red);
	  break;
	case 'G':
	  sscanf (optarg, "%f", &gamma_correct_green);
	  break;
	case 'B':
	  sscanf (optarg, "%f", &gamma_correct_blue);
	  break;
	}
    }
}

void
HandleError (char *description, int degree)
{
  fprintf (stderr,
	   _("An error has occurred.  The description is below...\n"));
  fprintf (stderr, "%s\n", description);

  if (degree == FATAL) {
      fprintf (stderr, _("Program aborting...\n"));
      exit (-1);
    }
}

void
init_mouse (void)
{
    mouse_setup ();
}

void
setcustompalette (void)
{
    char s[100];
    int i, n, r, g, b, flag[256];
    FILE *inf;
    Palette pal;
    for (i = 0; i < 256; i++)
	flag[i] = 0;
    if ((inf = fopen (colour_pal_file, "r")) == 0)
    {
	printf ("The colour palette file <%s>... ", colour_pal_file);
	do_error ("Can't find it.");
    }
    while (feof (inf) == 0)
    {
	fgets (s, 99, inf);
	if (sscanf (s, "%d %d %d %d", &n, &r, &g, &b) == 4)
	{
	    pal.color[n].red = r;
	    pal.color[n].green = g;
	    pal.color[n].blue = b;
	    flag[n] = 1;
	}
    }
    fclose (inf);
    for (i = 0; i < 256; i++)
    {
	if (flag[i] == 0)
	{
	    printf ("Colour %d not loaded\n", i);
	    do_error ("Can't continue");
	}
	pal.color[i].red = (unsigned char) ((pal.color[i].red
					     * (1 - gamma_correct_red)) + (64 * sin ((float) pal.color[i].red
										     * M_PI / 128)) * gamma_correct_red);

	pal.color[i].green = (unsigned char) ((pal.color[i].green
					       * (1 - gamma_correct_green)) + (64 * sin ((float) pal.color[i].green
											 * M_PI / 128)) * gamma_correct_green);

	pal.color[i].blue = (unsigned char) ((pal.color[i].blue
					      * (1 - gamma_correct_blue)) + (64 * sin ((float) pal.color[i].blue
										       * M_PI / 128)) * gamma_correct_blue);
    }
    gl_setpalette (&pal);
}
