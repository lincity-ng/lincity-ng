/* ---------------------------------------------------------------------- *
 * splash.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include "lcconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "lcstring.h"

#if defined (WIN32)
#include <winsock.h>
#include <io.h>
#include <direct.h>
#include <process.h>
#endif

#include "common.h"
#ifdef LC_X11
#include <X11/cursorfont.h>
#endif
#include "lctypes.h"
#include "lin-city.h"
#include "cliglobs.h"
#include "engglobs.h"
#include "screen.h"
#include "fileutil.h"

#define SI_BLACK 252
#define SI_RED 253
#define SI_GREEN 254
#define SI_YELLOW 255

/* ---------------------------------------------------------------------- *
 * Private Fn Prototypes
 * ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *
 * Private Global Variables
 * ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *
 * Public Functions
 * ---------------------------------------------------------------------- */
void
load_start_image (void)
{
#ifdef LC_X11
  XColor pal[256];
  XEvent xev;
#endif
  long x, y, l, r, g, b;
  FILE *fp;

  fp = fopen_read_gzipped (opening_pic);
  if (fp == NULL) {
      return;
  }

  for (x = 0; x < 7; x++)
    l = fgetc (fp);
  l &= 0xff;
  if (l == 0)
    l = 256;
  for (x = 0; x < l; x++)
    {
      r = fgetc (fp);
      g = fgetc (fp);
      b = fgetc (fp);
#ifdef LC_X11
      pal[x].red = r;
      pal[x].green = g;
      pal[x].blue = b;
      pal[x].flags = DoRed | DoGreen | DoBlue;
#else
      gl_setpalettecolor (x, r, g, b);
#endif
    }
  /* use last 4 colours for text */
#ifdef LC_X11
  pal[SI_BLACK].red = 0;
  pal[SI_BLACK].green = 0;
  pal[SI_BLACK].blue = 0;
  pal[SI_BLACK].flags = DoRed | DoGreen | DoBlue;
  pal[SI_RED].red = 60;
  pal[SI_RED].green = 0;
  pal[SI_RED].blue = 0;
  pal[SI_RED].flags = DoRed | DoGreen | DoBlue;
  pal[SI_GREEN].red = 0;
  pal[SI_GREEN].green = 60;
  pal[SI_GREEN].blue = 0;
  pal[SI_GREEN].flags = DoRed | DoGreen | DoBlue;
  pal[SI_YELLOW].red = 60;
  pal[SI_YELLOW].green = 60;
  pal[SI_YELLOW].blue = 0;
  pal[SI_YELLOW].flags = DoRed | DoGreen | DoBlue;
  open_setcustompalette (pal);
  suppress_next_expose = 1;
  do
    {
      while (XPending (display.dpy) == 0);
      XNextEvent (display.dpy, &xev);
      HandleEvent (&xev);
    }
  while (xev.type != MapNotify);

#else
  gl_setpalettecolor (SI_BLACK, 0, 0, 0);
  gl_setpalettecolor (SI_RED, 60, 0, 0);
  gl_setpalettecolor (SI_GREEN, 0, 60, 0);
  gl_setpalettecolor (SI_YELLOW, 60, 60, 0);
#endif
#if defined (WIN32)
  UpdatePalette ();
#endif
  for (y = 0; y < 480; y++)
    for (x = 0; x < 640; x++)
      {
	l = fgetc (fp);
	/*
	   //      printf("l=%d x=%d y=%d ",l,x,y);
	   // octree doesn't seem to want to generate images with 252 colours!
	   // So the next best thing (well the easyest) is to just map the pixels
	   // coloured as the last 4 colours to the 4 before that.
	   // If it looks OK, leave it.
	 */
	if (l == SI_BLACK)
	  l = SI_BLACK - 4;
	if (l == SI_RED)
	  l = SI_RED - 4;
	if (l == SI_GREEN)
	  l = SI_GREEN - 4;
	if (l == SI_YELLOW)
	  l = SI_YELLOW - 4;
	Fgl_setpixel (x, y, l);
      }

  fclose_read_gzipped (fp);

#if defined (WIN32)
  RefreshScreen ();
#endif
  start_image_text ();
}

void
start_image_text (void)
{
#if defined (SVGALIB)
  gl_setwritemode (FONT_COMPRESSED);
#endif
  Fgl_setfont (8, 8, start_font1);
#if defined (SVGALIB)
  gl_setwritemode (FONT_COMPRESSED);
#endif
  Fgl_setfontcolors (SI_BLACK, SI_RED);
  si_scroll_text ();
}

void
si_scroll_text (void)
{
  char s[100], line1[100], line2[100], line3[100], c;
  int i, t, l1c = 0, l2c = 0, l3c = 0;
  FILE *inf1, *inf2, *inf3;
#ifdef LC_X11
  XEvent xev;
#endif
  Fgl_enableclipping ();
  sprintf (s, "%s%c%s", opening_path, PATH_SLASH, "text1");
  if ((inf1 = fopen (s, "rb")) == NULL)
    do_error ("Can't open opening/text1");
  for (i = 0; i < 52; i++)
    line1[i] = si_next_char (inf1);
  line1[52] = 0;
  sprintf (s, "%s%c%s", opening_path, PATH_SLASH, "text2");
  if ((inf2 = fopen (s, "rb")) == NULL)
    do_error ("Can't open opening/text2");
  for (i = 0; i < 52; i++)
    line2[i] = si_next_char (inf2);
  line2[52] = 0;
  sprintf (s, "%s%c%s", opening_path, PATH_SLASH, "text3");
  if ((inf3 = fopen (s, "rb")) == NULL)
    do_error ("Can't open opening/text3");
  for (i = 0; i < 52; i++)
    line3[i] = si_next_char (inf3);
  line3[52] = 0;
  do
    {
      get_real_time ();
      t = real_time + SPLASH_SCROLL_DELAY;
#ifdef LC_X11
      if (XPending (display.dpy))

	{
	  XNextEvent (display.dpy, &xev);
	  HandleEvent (&xev);
	}

      c = x_key_value;
#elif defined (WIN32)
      c = GetKeystroke ();
#else
      c = vga_getkey ();
#endif
      if (l1c >= 8)
	{
	  for (i = 0; i < 51; i++)
	    line1[i] = line1[i + 1];
	  line1[51] = si_next_char (inf1);
	  l1c = 0;
	}
      Fgl_setfont (8, 8, start_font1);
      Fgl_setclippingwindow (120, 30, 520, 40);
      Fgl_setfontcolors (SI_BLACK, SI_RED);
#if defined (LC_X11) || defined (WIN32)
      open_write (120 - l1c, 31, line1);
#else
      Fgl_write (120 - l1c, 31, line1);
#endif
      l1c++;

      if (l2c >= 8)
	{
	  for (i = 0; i < 51; i++)
	    line2[i] = line2[i + 1];
	  line2[51] = si_next_char (inf2);
	  l2c = 0;
	}
      Fgl_setfont (8, 16, start_font2);
      Fgl_setclippingwindow (120, 55, 520, 73);
      Fgl_setfontcolors (SI_BLACK, SI_GREEN);
#if defined (LC_X11) || defined (WIN32)
      open_write (120 - l2c, 57, line2);
#else
      Fgl_write (120 - l2c, 57, line2);
#endif
      l2c += 2;

      if (l3c >= 8)
	{
	  for (i = 0; i < 51; i++)
	    line3[i] = line3[i + 1];
	  line3[51] = si_next_char (inf3);
	  l3c = 0;
	}
      Fgl_setfont (8, 16, start_font3);
      Fgl_setclippingwindow (120, 88, 520, 106);
      Fgl_setfontcolors (SI_BLACK, SI_YELLOW);
#if defined (LC_X11) || defined (WIN32)
      open_write (120 - l3c, 90, line3);
#else
      Fgl_write (120 - l3c, 90, line3);
#endif
      l3c += 2;
#if defined (WIN32)		/* Scroll a little faster for WIN32 */
      if (pix_double)
	{
	  l1c += 10;
	  l2c += 10;
	  l3c += 10;
	}
      else
	{
	  l1c += 2;
	  l2c += 4;
	  l3c += 4;
	}
#endif
      while (real_time < t)
	{
	  lc_usleep (1);
	  get_real_time ();
	}
    }
  while (c == 0);
  fclose (inf1);
  fclose (inf2);
  fclose (inf3);
  Fgl_disableclipping ();
}

char
si_next_char (FILE * inf)
{
  char c;
  if (feof (inf) != 0)
    fseek (inf, 0L, SEEK_SET);
  c = fgetc (inf);
  if (c == 0xa || c == 0xd)
    c = ' ';
  return (c);
}

