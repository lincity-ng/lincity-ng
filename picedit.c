/* ---------------------------------------------------------------------- *
 * picedit.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * Thanks to Gerben V for X true color support.
 * ---------------------------------------------------------------------- */
#include "lcconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "lcstring.h"
#ifdef LC_X11
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include "xpicedit.h"
#elif defined (SVGALIB)
#include <vga.h>
#include <vgagl.h>
#include <vgamouse.h>
#endif

#define LIBDIR DEFAULT_LIBDIR

#ifdef __EMX__
#include <X11/Xlibint.h>      /* required for __XOS2RedirRoot */
#define chown(x,y,z)
#ifdef LIBDIR
#undef LIBDIR   /* yes, I know I shouldn't ;-) */
#endif
char LIBDIR[256];
#define OS2_DEFAULT_LIBDIR "/XFree86/lib/X11/lincity"
#endif

#define COLOUR_PAL_FILE "/colour.pal"
#define FONTFILE "/opening/iso8859-1-8x8.raw"

#define WINSTARTX 240
#define WINSTARTY ((14*16)-1)
#define SWINSTARTX 56
#define SWINSTARTY 260
#define MAXIMAGESIZE 4

#define SIZEBUTTONX1x1 540
#define SIZEBUTTONY1x1 260
#define SIZEBUTTONX2x2 540
#define SIZEBUTTONY2x2 276
#define SIZEBUTTONX3x3 540
#define SIZEBUTTONY3x3 292
#define SIZEBUTTONX4x4 540
#define SIZEBUTTONY4x4 308
#define SIZEBUTTONSIZEX 32
#define SIZEBUTTONSIZEY 16

#define FOCUSWINX 40
#define FOCUSWINY 360
/* FOCUSWINSIZE must be a multiple of 4 */
#define FOCUSWINSIZE 56

#define ZOOMBUTX 540
#define ZOOMBUTY 390
#define ZOOMBUTSIZEX 40
#define ZOOMBUTSIZEY 16

#define HALFBUTX 540
#define HALFBUTY 370
#define HALFBUTSIZEX 40
#define HALFBUTSIZEY 16

#define NORMBUTX 540
#define NORMBUTY 350
#define NORMBUTSIZEX 40
#define NORMBUTSIZEY 16

#define DOTBUTX 150
#define DOTBUTY 350
#define DOTBUTSIZEX 40
#define DOTBUTSIZEY 16

#define HFILBUTX 150
#define HFILBUTY 370
#define HFILBUTSIZEX 40
#define HFILBUTSIZEY 16

#define VFILBUTX 150
#define VFILBUTY 390
#define VFILBUTSIZEX 40
#define VFILBUTSIZEY 16

#define red(x) (32 + x)
#define green(x) (64 + x)
#define yellow(x) (96 + x)
#define blue(x) (128 + x)
#define magenta(x) (160 + x)
#define cyan(x) (192 + x)
#define white(x) (224 + x)

#define FOCUSWINCOLOUR green(30)
#define UNFOCUSWINCOLOUR magenta(20)
#define XFOCUSWINCOLOUR red(15)

unsigned char mouse_pointer[] =
{
  255, 255, 255, 255, 1, 1, 1, 1,
  255, 1, 1, 1, 255, 255, 1, 1,
  255, 1, 255, 255, 255, 1, 255, 1,
  255, 1, 255, 1, 1, 255, 1, 1,
  1, 255, 255, 1, 1, 1, 255, 1,
  1, 255, 1, 255, 1, 1, 1, 1,
  1, 1, 255, 1, 255, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1};

unsigned char under_mouse_pointer[8 * 8], last16[16];
unsigned char image[16 * MAXIMAGESIZE][16 * MAXIMAGESIZE];
char imagefile[100];

int colour = 0, mx, my, mox, moy, zoom_flag = 1, hfill_flag = 0, vfill_flag = 0;
int imagesize, newimagesize, focus, focusx, focusy;
int old_focusx = 0, old_focusy = 0, stopflag, lc_mouse_type;

void do_buttons (int, int);
void draw_dot (void);
void draw_hfill (void);
void draw_vfill (void);
void full_zoom (void);
void half_zoom (void);
void norm_zoom (void);
void do_error (char *);
void choose_colour (int, int);
void fill_colour (int, int);
void hfill (int, int);
void vfill (int, int);
void fill_colourzoom (int, int);
void fill_colour1 (int, int);
void fill_colour2 (int, int);
void fill_colour3 (int, int);
void fill_colour4 (int, int);
void setcustompalette (void);
void initfont (void);
void save_image (void);
void load_image (void);
void setup (void);
void draw_focus (void);
void draw_swin (void);
void draw_big_image (void);
void draw_big_imagezoom (void);
void draw_big_image1 (void);
void draw_big_image2 (void);
void draw_big_image3 (void);
void draw_big_image4 (void);
void change_imagesize (int);
void change_focus (int, int);
void hide_mouse (void);
void reshow_mouse (void);

#ifdef LC_X11

void Fgl_hline (int, int, int, int);
void Fgl_line (int, int, int, int, int);
void Fgl_setpixel (int, int, int);
int Fgl_getpixel (int, int);
void Fgl_write (int, int, char *);
void Fgl_fillbox (int, int, int, int, int);
void Fgl_putbox (int, int, int, int, unsigned char *);
void Fgl_getbox (int, int, int, int, unsigned char *);

#else

void lincityrc ();

#define Fgl_setpixel(a,b,c)    gl_setpixel(a,b,c)
#define Fgl_getpixel(a,b)      gl_getpixel(a,b)
#define Fgl_hline(a,b,c,d)     gl_hline(a,b,c,d)
#define Fgl_line(a,b,c,d,e)    gl_line(a,b,c,d,e)
#define Fgl_write(a,b,c)       gl_write(a,b,c)
#define Fgl_fillbox(a,b,c,d,e) gl_fillbox(a,b,c,d,e)
#define Fgl_putbox(a,b,c,d,e)  gl_putbox(a,b,c,d,e)
#define Fgl_getbox(a,b,c,d,e)  gl_getbox(a,b,c,d,e)

#endif

#ifdef LC_X11

int winX, winY, mouse_button;
unsigned int winW, winH;
disp display;

char *bg_color = NULL;
char pixmap[(640 + BORDERX * 2) * (480 + BORDERY * 2)];
char myfont[256 * 8];
int verbose = FALSE;
int text_bg = 0;
int text_fg = 255;
int borderx, bordery;
long unsigned int colour_table[256];

void parse_args (int, char **, char **);
void Create_Window (char *);
void HandleError (char *, int);
void HandleEvent (XEvent *);
void refresh_screen (int, int, int, int);
void my_x_putchar (int, int, int);

#endif
int
main (int argc, char *argv[])
{
  int x, y, c, m, ox, oy;
#ifdef LC_X11
  XEvent xev;
  char *geometry = NULL;
#endif
  c = 0;
  stopflag = 0;
  newimagesize = 1;

#ifdef __EMX__
  strcpy(LIBDIR, __XOS2RedirRoot(OS2_DEFAULT_LIBDIR));
#endif

#ifdef LC_X11
  borderx = BORDERX;
  bordery = BORDERY;
  parse_args (argc, argv, &geometry);
  printf ("Got past parse args\n");
  Create_Window (geometry);
  XSync (display.dpy, FALSE);

  while (XPending (display.dpy) == 0);
  XNextEvent (display.dpy, &xev);
  HandleEvent (&xev);

  printf ("Got past create window\n");
#else
  lincityrc ();
  vga_init ();
  vga_setmode (G640x480x256);
  gl_setcontextvga (G640x480x256);
#endif

  setcustompalette ();
  initfont ();
  if (argc < 2)
    strcpy (imagefile, "imagefile.default");
  else
    {
      strcpy (imagefile, argv[1]);
      load_image ();
    }
#ifndef LC_X11
  m = mouse_init ("/dev/mouse", lc_mouse_type, MOUSE_DEFAULTSAMPLERATE);
  mouse_setxrange (0, 640 - 1);
  mouse_setyrange (0, 480 - 1);
  mouse_setwrap (MOUSE_NOWRAP);
#endif
  setup ();
#ifdef LC_X11
  XSync (display.dpy, FALSE);
#endif
  printf ("Got past setup\n");
  if (newimagesize != imagesize)
    change_imagesize (newimagesize);
  change_focus (0, 0);
  printf ("Got to do loop\n");
  do
    {
#ifdef LC_X11
      if (XPending (display.dpy))
	{
	  XNextEvent (display.dpy, &xev);
	  HandleEvent (&xev);
	}
      if (mouse_button != 0)
	do_buttons (0, 0);
#else
      c = vga_getkey ();
      mouse_waitforupdate ();
      mx = mouse_getx ();
      my = mouse_gety ();
      if (mx != mox || my != moy)
	{
	  Fgl_putbox (mox, moy, 8, 8, under_mouse_pointer);
	  Fgl_getbox (mx, my, 8, 8, under_mouse_pointer);
	  Fgl_putbox (mx, my, 8, 8, mouse_pointer);
	  mox = mx;
	  moy = my;
	}
      do_buttons (mx, my);
#endif
    }
  while (c == 0 && stopflag == 0);
  printf ("Normal exit\n");
#ifndef LC_X11
  mouse_close ();
  vga_setmode (TEXT);
  printf ("c=%d mouse %d\n", c, m);
#endif
  save_image ();
  exit (0);
}

void
do_error (char *s)
{
#ifdef LC_X11
  HandleError (s, FATAL);
#else
  vga_setmode (TEXT);
  printf ("%s\n", s);
  exit (1);
#endif
}

void
do_buttons (int x, int y)
{
  int b, left = 0, right = 0;
#ifdef LC_X11
  int dummy_int;
  Window dummy_win;
  if (mouse_button == Button1)
    left = 1;
  else if (mouse_button == Button3)
    right = 1;
  XQueryPointer (display.dpy, display.win, &dummy_win, &dummy_win
	      ,&dummy_int, &dummy_int, &x, &y, (unsigned int *) &dummy_int);
  x -= borderx;
  y -= bordery;
#else
  b = mouse_getbutton ();
  left = b & MOUSE_LEFTBUTTON;
  right = b & MOUSE_RIGHTBUTTON;
#endif
  if (right != 0 && x < 560)
    choose_colour (x, y);
  if (left != 0)
    {
      if (y < 16 * 14 - 2)
	choose_colour (x, y);
      else if (x < WINSTARTX + 256 && x >= WINSTARTX
	       && y < WINSTARTY + 256 && y > WINSTARTY)
	fill_colour (x, y);
      else if (x > SIZEBUTTONX1x1
	       && x < SIZEBUTTONX1x1 + SIZEBUTTONSIZEX
	       && y > SIZEBUTTONY1x1
	       && y < SIZEBUTTONY1x1 + SIZEBUTTONSIZEY
	       && imagesize != 1)
	change_imagesize (1);
      else if (x > SIZEBUTTONX2x2
	       && x < SIZEBUTTONX2x2 + SIZEBUTTONSIZEX
	       && y > SIZEBUTTONY2x2
	       && y < SIZEBUTTONY2x2 + SIZEBUTTONSIZEY
	       && imagesize != 2)
	change_imagesize (2);
      else if (x > SIZEBUTTONX3x3
	       && x < SIZEBUTTONX3x3 + SIZEBUTTONSIZEX
	       && y > SIZEBUTTONY3x3
	       && y < SIZEBUTTONY3x3 + SIZEBUTTONSIZEY
	       && imagesize != 3)
	change_imagesize (3);
      else if (x > SIZEBUTTONX4x4
	       && x < SIZEBUTTONX4x4 + SIZEBUTTONSIZEX
	       && y > SIZEBUTTONY4x4
	       && y < SIZEBUTTONY4x4 + SIZEBUTTONSIZEY
	       && imagesize != 4)
	change_imagesize (4);
      else if (x > FOCUSWINX && x < FOCUSWINX + FOCUSWINSIZE
	       && y > FOCUSWINY && FOCUSWINY + FOCUSWINSIZE
	       && zoom_flag == 1)
	change_focus ((x - FOCUSWINX) / (FOCUSWINSIZE / 4)
		      ,(y - FOCUSWINY) / (FOCUSWINSIZE / 4));
      else if (x > ZOOMBUTX && x < ZOOMBUTX + ZOOMBUTSIZEX
	       && y > ZOOMBUTY && y < ZOOMBUTY + ZOOMBUTSIZEY)
	full_zoom ();
      else if (x > HALFBUTX && x < HALFBUTX + HALFBUTSIZEX
	       && y > HALFBUTY && y < HALFBUTY + HALFBUTSIZEY)
	half_zoom ();
      else if (x > NORMBUTX && x < NORMBUTX + NORMBUTSIZEX
	       && y > NORMBUTY && y < NORMBUTY + NORMBUTSIZEY)
	norm_zoom ();
      else if (x > DOTBUTX && x < DOTBUTX + DOTBUTSIZEX
	       && y > DOTBUTY && y < DOTBUTY + DOTBUTSIZEY)
	draw_dot ();
      else if (x > HFILBUTX && x < HFILBUTX + HFILBUTSIZEX
	       && y > HFILBUTY && y < HFILBUTY + HFILBUTSIZEY)
	draw_hfill ();
      else if (x > VFILBUTX && x < VFILBUTX + VFILBUTSIZEX
	       && y > VFILBUTY && y < VFILBUTY + VFILBUTSIZEY)
	draw_vfill ();
    }
}

void
draw_dot (void)
{
  Fgl_write (DOTBUTX - 10, DOTBUTY + 3, ">");
  Fgl_write (HFILBUTX - 10, HFILBUTY + 3, " ");
  Fgl_write (VFILBUTX - 10, VFILBUTY + 3, " ");
  hfill_flag = 0;
  vfill_flag = 0;
}

void
draw_hfill (void)
{
  Fgl_write (DOTBUTX - 10, DOTBUTY + 3, " ");
  Fgl_write (HFILBUTX - 10, HFILBUTY + 3, ">");
  Fgl_write (VFILBUTX - 10, VFILBUTY + 3, " ");
  hfill_flag = 1;
  vfill_flag = 0;
}

void
draw_vfill (void)
{
  Fgl_write (DOTBUTX - 10, DOTBUTY + 3, " ");
  Fgl_write (HFILBUTX - 10, HFILBUTY + 3, " ");
  Fgl_write (VFILBUTX - 10, VFILBUTY + 3, ">");
  vfill_flag = 1;
  hfill_flag = 0;
}


void
full_zoom (void)
{
  int y;
  if (zoom_flag == 0)
    return;
  Fgl_write (ZOOMBUTX - 10, ZOOMBUTY + 3, ">");
  Fgl_write (HALFBUTX - 10, HALFBUTY + 3, " ");
  Fgl_write (NORMBUTX - 10, NORMBUTY + 3, " ");
  if (imagesize == 3)
    {
      Fgl_fillbox (WINSTARTX - 1, WINSTARTY - 1
		   ,258, 258, 0);
      Fgl_hline (WINSTARTX - 1, WINSTARTY - 1
		 ,WINSTARTX + 240, 255);
      Fgl_hline (WINSTARTX - 1, WINSTARTY + 240
		 ,WINSTARTX + 240, 255);
      Fgl_line (WINSTARTX - 1, WINSTARTY - 1, WINSTARTX - 1
		,WINSTARTY + 240, 255);
      Fgl_line (WINSTARTX + 240, WINSTARTY - 1, WINSTARTX + 240
		,WINSTARTY + 240, 255);
    }
  zoom_flag = 0;
  old_focusx = focusx;
  old_focusy = focusy;
  change_focus (0, 0);

  draw_big_image ();
}

void
half_zoom (void)
{
  int y;
  if (zoom_flag == 2 || imagesize < 2)
    return;
  zoom_flag = 2;
  Fgl_write (ZOOMBUTX - 10, ZOOMBUTY + 3, " ");
  Fgl_write (HALFBUTX - 10, HALFBUTY + 3, ">");
  Fgl_write (NORMBUTX - 10, NORMBUTY + 3, " ");
  if (zoom_flag != 0)
    {
      old_focusx = focusx;
      old_focusy = focusy;
    }
  if (focusx >= (imagesize - 1))
    focusx = imagesize - 2;
  if (focusy >= (imagesize - 1))
    focusy = imagesize - 2;
  change_focus (focusx, focusy);
  draw_big_image ();
}


void
norm_zoom (void)
{
  int y;
  if (zoom_flag == 1)
    return;
  Fgl_write (ZOOMBUTX - 10, ZOOMBUTY + 3, " ");
  Fgl_write (HALFBUTX - 10, HALFBUTY + 3, " ");
  Fgl_write (NORMBUTX - 10, NORMBUTY + 3, ">");
  Fgl_hline (WINSTARTX, WINSTARTY - 1, WINSTARTX + 256, 255);
  Fgl_hline (WINSTARTX, WINSTARTY + 256, WINSTARTX + 256, 255);
  Fgl_line (WINSTARTX - 1, WINSTARTY, WINSTARTX - 1, WINSTARTY + 256, 255);
  Fgl_line (WINSTARTX + 256, WINSTARTY
	    ,WINSTARTX + 256, WINSTARTY + 256, 255);
  for (y = 0; y < 16; y++)
    {
      Fgl_hline (WINSTARTX, WINSTARTY + y * 16 + 15
		 ,WINSTARTX + 255, 253);
      Fgl_line (WINSTARTX + y * 16 + 15, WINSTARTY, WINSTARTX + y * 16 + 15
		,WINSTARTY + 255, 253);
    }
  zoom_flag = 1;
  change_focus (old_focusx, old_focusy);
  draw_big_image ();
}

void
choose_colour (int x, int y)
{
  char s[100];
  int q, newcolour;
  newcolour = Fgl_getpixel (x - 1, y - 1);
  if (newcolour == colour)
    return;
  colour = newcolour;
  for (q = 15; q > 0; q--)
    last16[q] = last16[q - 1];
  last16[0] = colour;
  for (q = 0; q < 16; q++)
    {
      Fgl_fillbox (600, 14 * 16 + q * 16, 39, 15, (int) last16[q]);
      sprintf (s, "%d", (int) last16[q]);
      Fgl_write (600 + 3, 14 * 16 + q * 16 + 3, s);
    }
}

void
fill_colour (int x, int y)
{
  if (hfill_flag != 0)
    hfill (x, y);
  else if (vfill_flag != 0)
    vfill (x, y);
  else if (zoom_flag == 1)
    fill_colourzoom (x, y);
  else if (zoom_flag == 2)
    fill_colour2 (x, y);
  else if (imagesize == 1)
    fill_colour1 (x, y);
  else if (imagesize == 2)
    fill_colour2 (x, y);
  else if (imagesize == 3)
    fill_colour3 (x, y);
  else if (imagesize == 4)
    fill_colour4 (x, y);
}

void
hfill (int x, int y)
{
  int xx = 0, col, d;
  if (zoom_flag == 1)
    d = 16;
  else if (zoom_flag == 2)
    d = 8;
  else if (imagesize == 1)
    d = 16;
  else if (imagesize == 2)
    d = 8;
  else if (imagesize == 3)
    d = 5;
  else if (imagesize == 4)
    d = 4;
  col = image[((x - WINSTARTX) / d) + (focusx * 16)]
    [((y - (WINSTARTY)) / d) + (focusy * 16)];

  while (((x - WINSTARTX) / d) + (focusx * 16) + xx < 16 * MAXIMAGESIZE
	 && (image[((x - WINSTARTX) / d) + (focusx * 16) + xx]
	     [((y - (WINSTARTY)) / d) + (focusy * 16)] == col))
    {
      image[((x - WINSTARTX) / d) + (focusx * 16) + xx]
	[((y - (WINSTARTY)) / d) + (focusy * 16)] = colour;
      xx++;
    }
  xx = -1;
  while (((x - WINSTARTX) / d) + (focusx * 16) + xx >= 0
	 && (image[((x - WINSTARTX) / d) + (focusx * 16) + xx]
	     [((y - (WINSTARTY)) / d) + (focusy * 16)] == col))
    {
      image[((x - WINSTARTX) / d) + (focusx * 16) + xx]
	[((y - (WINSTARTY)) / d) + (focusy * 16)] = colour;
      xx--;
    }
  Fgl_putbox (mox, moy, 8, 8, under_mouse_pointer);
  draw_swin ();
  draw_big_image ();
  Fgl_getbox (mx, my, 8, 8, under_mouse_pointer);
  Fgl_putbox (mx, my, 8, 8, mouse_pointer);
}

void
vfill (int x, int y)
{
  int yy = 0, col, d;
  if (zoom_flag == 1)
    d = 16;
  else if (zoom_flag == 2)
    d = 8;
  else if (imagesize == 1)
    d = 16;
  else if (imagesize == 2)
    d = 8;
  else if (imagesize == 3)
    d = 5;
  else if (imagesize == 4)
    d = 4;
  col = image[((x - WINSTARTX) / d) + (focusx * 16)]
    [((y - (WINSTARTY)) / d) + (focusy * 16)];

  while (((y - WINSTARTY) / d) + (focusy * 16) + yy < 16 * MAXIMAGESIZE
	 && (image[((x - WINSTARTX) / d) + (focusx * 16)]
	     [((y - (WINSTARTY)) / d) + (focusy * 16) + yy] == col))
    {
      image[((x - WINSTARTX) / d) + (focusx * 16)]
	[((y - (WINSTARTY)) / d) + (focusy * 16) + yy] = colour;
      yy++;
    }
  yy = -1;
  while (((y - WINSTARTY) / d) + (focusy * 16) + yy >= 0
	 && (image[((x - WINSTARTX) / d) + (focusx * 16)]
	     [((y - (WINSTARTY)) / d) + (focusy * 16) + yy] == col))
    {
      image[((x - WINSTARTX) / d) + (focusx * 16)]
	[((y - (WINSTARTY)) / d) + (focusy * 16) + yy] = colour;
      yy--;
    }
  Fgl_putbox (mox, moy, 8, 8, under_mouse_pointer);
  draw_swin ();
  draw_big_image ();
  Fgl_getbox (mx, my, 8, 8, under_mouse_pointer);
  Fgl_putbox (mx, my, 8, 8, mouse_pointer);
}


void
fill_colourzoom (int x, int y)
{
  int xx, yy;
  xx = (x - WINSTARTX) / 16;
  xx = WINSTARTX + (xx * 16);
  yy = (y - WINSTARTY) / 16;
  yy = WINSTARTY + (yy * 16);
  Fgl_putbox (mox, moy, 8, 8, under_mouse_pointer);
  Fgl_fillbox (xx, yy, 15, 15, colour);
  Fgl_getbox (mx, my, 8, 8, under_mouse_pointer);
  Fgl_putbox (mx, my, 8, 8, mouse_pointer);

  Fgl_setpixel (((SWINSTARTX + 8) - imagesize * 8) + ((x - WINSTARTX) / 16)
		+ (focusx * 16)
		,((SWINSTARTY + 8) - imagesize * 8) + ((y - (WINSTARTY)) / 16) + (focusy * 16)
		,colour);
  image[((x - WINSTARTX) / 16) + (focusx * 16)]
    [((y - (WINSTARTY)) / 16) + (focusy * 16)] = colour;
}

void
fill_colour1 (int x, int y)
{
  int xx, yy;
  xx = (x - WINSTARTX) / 16;
  xx = WINSTARTX + (xx * 16);
  yy = (y - WINSTARTY) / 16;
  yy = WINSTARTY + (yy * 16);
  Fgl_putbox (mox, moy, 8, 8, under_mouse_pointer);
  Fgl_fillbox (xx, yy, 16, 16, colour);
  Fgl_getbox (mx, my, 8, 8, under_mouse_pointer);
  Fgl_putbox (mx, my, 8, 8, mouse_pointer);

  Fgl_setpixel ((SWINSTARTX) + ((x - WINSTARTX) / 16)
		+ (focusx * 16)
		,(SWINSTARTY) + ((y - (WINSTARTY)) / 16) + (focusy * 16)
		,colour);
  image[((x - WINSTARTX) / 16) + (focusx * 16)]
    [((y - (WINSTARTY)) / 16) + (focusy * 16)] = colour;
}

void
fill_colour2 (int x, int y)
{
  int xx, yy;
  xx = (x - WINSTARTX) / 8;
  xx = WINSTARTX + (xx * 8);
  yy = (y - WINSTARTY) / 8;
  yy = WINSTARTY + (yy * 8);
  Fgl_putbox (mox, moy, 8, 8, under_mouse_pointer);
  Fgl_fillbox (xx, yy, 8, 8, colour);
  Fgl_getbox (mx, my, 8, 8, under_mouse_pointer);
  Fgl_putbox (mx, my, 8, 8, mouse_pointer);


  image[((x - WINSTARTX) / 8) + (focusx * 16)]
    [((y - (WINSTARTY)) / 8) + (focusy * 16)] = colour;
  draw_swin ();
}

void
fill_colour3 (int x, int y)
{
  int xx, yy;
  xx = (x - WINSTARTX) / 5;
  if (xx > 47)
    return;
  xx = WINSTARTX + (xx * 5);
  yy = (y - WINSTARTY) / 5;
  if (yy > 47)
    return;
  yy = WINSTARTY + (yy * 5);
  Fgl_putbox (mox, moy, 8, 8, under_mouse_pointer);
  Fgl_fillbox (xx, yy, 5, 5, colour);
  Fgl_getbox (mx, my, 8, 8, under_mouse_pointer);
  Fgl_putbox (mx, my, 8, 8, mouse_pointer);

  Fgl_setpixel ((SWINSTARTX - 16) + ((x - WINSTARTX) / 5)
		+ (focusx * 5)
		,(SWINSTARTY - 16) + ((y - (WINSTARTY)) / 5) + (focusy * 5)
		,colour);
  image[((x - WINSTARTX) / 5) + (focusx * 5)]
    [((y - (WINSTARTY)) / 5) + (focusy * 5)] = colour;
}

void
fill_colour4 (int x, int y)
{
  int xx, yy;
  xx = (x - WINSTARTX) / 4;
  xx = WINSTARTX + (xx * 4);
  yy = (y - WINSTARTY) / 4;
  yy = WINSTARTY + (yy * 4);
  Fgl_putbox (mox, moy, 8, 8, under_mouse_pointer);
  Fgl_fillbox (xx, yy, 4, 4, colour);
  Fgl_getbox (mx, my, 8, 8, under_mouse_pointer);
  Fgl_putbox (mx, my, 8, 8, mouse_pointer);

  Fgl_setpixel ((SWINSTARTX - 24) + ((x - WINSTARTX) / 4)
		+ (focusx * 4)
		,(SWINSTARTY - 24) + ((y - (WINSTARTY)) / 4) + (focusy * 4)
		,colour);
  image[((x - WINSTARTX) / 4) + (focusx * 4)]
    [((y - (WINSTARTY)) / 4) + (focusy * 4)] = colour;
}

#ifdef LC_X11
/* fixed the problem with xpicedit by checking if the colormap is writable*/
void
setcustompalette (void)
{
  char s[100], cpf[100];
  int i, n, r, g, b, me = 0, flag[256];
  long unsigned int plane_masks[3];
  FILE *inf;
  XColor pal[256];
  int is_writeable;     
  
  /* See if the colormap is writable.
   * Only PseudoColor, GrayScale and DirectColor are writable TrueColor is not.
   */
  Visual *visual = DefaultVisual (display.dpy, display.screen);
  int visual_class = visual->class;
  is_writeable = (visual_class == PseudoColor || visual_class == GrayScale || visual_class == DirectColor);
  
  printf ("In setcustompalette\n");
  display.cmap = XDefaultColormap (display.dpy, display.screen);
  if (is_writeable) {
    if (XAllocColorCells (display.dpy, display.cmap, 0
			,plane_masks, 0, colour_table, 256) == 0)
    {
    me = (*DefaultVisual (display.dpy, display.screen)).map_entries;
    printf ("DefaultVisual id=%d bp-rgb=%d map-entries=%d\n"
	      ,(*DefaultVisual (display.dpy, display.screen)).visualid
	      ,(*DefaultVisual (display.dpy, display.screen)).bits_per_rgb
	      ,(*DefaultVisual (display.dpy, display.screen)).map_entries);
    display.cmap = XCreateColormap (display.dpy, display.win
				,DefaultVisual (display.dpy, display.screen)
				,AllocNone);
      if (me == 256 && XAllocColorCells (display.dpy, display.cmap, 0
				   ,plane_masks, 0, colour_table, 256) != 0)
	printf ("Allocated 256 cells\n");
      else
	for (i = 0; i < 256; i++)
	  colour_table[i] = i;
      }
    if (!display.cmap)
      HandleError ("No default colour map", FATAL);
  }
  printf ("Got past create colourmap\n");

  for (i = 0; i < 256; i++)
    flag[i] = 0;
  
  strcpy (cpf, LIBDIR);
  strcat (cpf, COLOUR_PAL_FILE);
  if ((inf = fopen (cpf, "r")) == 0)
    HandleError ("Can't find the colour pallet file", FATAL);
  while (feof (inf) == 0)
    {
      fgets (s, 99, inf);
      if (sscanf (s, "%d %d %d %d", &n, &r, &g, &b) == 4)
	{
	  pal[n].red = r << 10;
	  pal[n].green = g << 10;
	  pal[n].blue = b << 10;
	  pal[n].flags = DoRed | DoGreen | DoBlue;
	  if (is_writeable) {
	    pal[n].pixel = colour_table[n];
	  }
	  else
	    {
	      if (XAllocColor (display.dpy
			       ,display.cmap, &(pal[n])) == 0)
		HandleError ("alloc colour failed"
			     ,FATAL);
	      colour_table[n] = pal[n].pixel;
	      XSetForeground (display.dpy
			      ,display.pixcolour_gc[n]
			      ,colour_table[n]);
	    }
	  flag[n] = 1;
	}
    }
  fclose (inf);
  
  for (i = 0; i < 256; i++)
    if (flag[i] == 0)
      {
	printf ("Colour %d not loaded\n", i);
	do_error ("Can't continue");
      }
  if(is_writeable) {
    XStoreColors (display.dpy, display.cmap, pal, 256);
    XFlush(display.dpy);
  }
  XSetWindowColormap (display.dpy, display.win, display.cmap);
}

#else
void
setcustompalette (void)
{
  char s[100], cpf[100];
  int i, n, r, g, b, flag[256];
  FILE *inf;
  Palette pal;
  for (i = 0; i < 256; i++)
    flag[i] = 0;
  strcpy (cpf, LIBDIR);
  strcat (cpf, COLOUR_PAL_FILE);
  if ((inf = fopen (cpf, "r")) == 0)
    do_error ("Can't find the colour pallet file :(");

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
    if (flag[i] == 0)
      {
	printf ("Colour %d not loaded\n", i);
	do_error ("Can't continue");
      }
  gl_setpalette (&pal);
}
#endif


void
initfont ()
{
#ifdef LC_X11
  char cpf[100];
  int i;
  FILE *finf;
  strcpy (cpf, LIBDIR);
  strcat (cpf, FONTFILE);
  if ((finf = fopen (cpf, "r")) == 0)
    HandleError ("Can't open the font file", FATAL);
  for (i = 0; i < 256 * 8; i++)
    myfont[i] = fgetc (finf);
  fclose (finf);
#else
  void *font;
  font = malloc (256 * 8 * 8 * BYTESPERPIXEL);
  gl_expandfont (8, 8, white (24), gl_font8x8, font);
  gl_setfont (8, 8, font);
#endif
}


void
save_image (void)
{
  int x, y;
  FILE *outf;
  if ((outf = fopen (imagefile, "wb")) == NULL)
    {
      printf ("Can't write image file %s \n", imagefile);
      return;
    }
  for (y = 0; y < 16 * imagesize; y++)
    for (x = 0; x < 16 * imagesize; x++)
      putc (image[x][y], outf);
  fclose (outf);
}

void
load_image (void)
{
  int x, y, l;
  FILE *inf;
  if ((inf = fopen (imagefile, "rb")) == NULL)
    return;
  fseek (inf, 0L, SEEK_END);
  l = ftell (inf);
  fseek (inf, 0L, SEEK_SET);
  if (l == 256)
    newimagesize = 1;
  else if (l == 1024)
    newimagesize = 2;
  else if (l == 2304)
    newimagesize = 3;
  else if (l == 4096)
    newimagesize = 4;
  else
    {
      printf ("Can't load file %s, maybe wrong size %d\n"
	      ,imagefile, l);
      exit (1);
    }
  for (y = 0; y < 16 * newimagesize; y++)
    for (x = 0; x < 16 * newimagesize; x++)
      image[x][y] = fgetc (inf);
  fclose (inf);
}




void
setup (void)
{
  char s[50];
  short x, y;
  Fgl_hline (WINSTARTX, WINSTARTY - 1, WINSTARTX + 256, 255);
  Fgl_hline (WINSTARTX, WINSTARTY + 256, WINSTARTX + 256, 255);
  Fgl_line (WINSTARTX - 1, WINSTARTY, WINSTARTX - 1, WINSTARTY + 256, 255);
  Fgl_line (WINSTARTX + 256, WINSTARTY, WINSTARTX + 256, WINSTARTY + 256, 255);
  for (y = 0; y < 16; y++)
    {
      Fgl_hline (WINSTARTX, WINSTARTY + y * 16 + 15, WINSTARTX + 255, 253);
      Fgl_line (WINSTARTX + y * 16 + 15, WINSTARTY, WINSTARTX + y * 16 + 15
		,WINSTARTY + 255, 253);
      for (x = 0; x < 16; x++)
	{
	  Fgl_fillbox (x * 40, y * 14, 38, 12, y * 16 + x);
	  sprintf (s, "%d", y * 16 + x);
	  Fgl_write (x * 40, y * 14, s);
	}
    }
  change_imagesize (1);
  Fgl_fillbox (SIZEBUTTONX1x1, SIZEBUTTONY1x1, SIZEBUTTONSIZEX - 1
	       ,SIZEBUTTONSIZEY - 1, 240);
  Fgl_write (SIZEBUTTONX1x1 + 3, SIZEBUTTONY1x1 + 3, "1x1");
  Fgl_fillbox (SIZEBUTTONX2x2, SIZEBUTTONY2x2, SIZEBUTTONSIZEX - 1
	       ,SIZEBUTTONSIZEY - 1, 240);
  Fgl_write (SIZEBUTTONX2x2 + 3, SIZEBUTTONY2x2 + 3, "2x2");
  Fgl_fillbox (SIZEBUTTONX3x3, SIZEBUTTONY3x3, SIZEBUTTONSIZEX - 1
	       ,SIZEBUTTONSIZEY - 1, 240);
  Fgl_write (SIZEBUTTONX3x3 + 3, SIZEBUTTONY3x3 + 3, "3x3");
  Fgl_fillbox (SIZEBUTTONX4x4, SIZEBUTTONY4x4, SIZEBUTTONSIZEX - 1
	       ,SIZEBUTTONSIZEY - 1, 240);
  Fgl_write (SIZEBUTTONX4x4 + 3, SIZEBUTTONY4x4 + 3, "4x4");
  /* this is the focus change window */
  Fgl_fillbox (FOCUSWINX - 1, FOCUSWINY - 1, FOCUSWINSIZE + 1, FOCUSWINSIZE + 1, 240);
  draw_focus ();
  /* this is the zoom button */
  Fgl_fillbox (ZOOMBUTX, ZOOMBUTY, ZOOMBUTSIZEX, ZOOMBUTSIZEY, 240);
  Fgl_write (ZOOMBUTX + 3, ZOOMBUTY + 3, "ZOOM");
  /* this is the half zoom button */
  Fgl_fillbox (HALFBUTX, HALFBUTY, HALFBUTSIZEX, HALFBUTSIZEY, 240);
  Fgl_write (HALFBUTX + 3, HALFBUTY + 3, "HALF");
  /* this is the normal zoom button */
  Fgl_fillbox (NORMBUTX, NORMBUTY, NORMBUTSIZEX, NORMBUTSIZEY, 240);
  Fgl_write (NORMBUTX + 3, NORMBUTY + 3, "NORM");
  /* this is the dot button */
  Fgl_fillbox (DOTBUTX, DOTBUTY, DOTBUTSIZEX, DOTBUTSIZEY, 240);
  Fgl_write (DOTBUTX + 3, DOTBUTY + 3, "DOT");
  /* this id the hline button */
  Fgl_fillbox (HFILBUTX, HFILBUTY, HFILBUTSIZEX, HFILBUTSIZEY, 240);
  Fgl_write (HFILBUTX + 3, HFILBUTY + 3, "HFIL");
  /* this is the vline button */
  Fgl_fillbox (VFILBUTX, VFILBUTY, VFILBUTSIZEX, VFILBUTSIZEY, 240);
  Fgl_write (VFILBUTX + 3, VFILBUTY + 3, "VFIL");
  Fgl_write (0, 480 - 9, imagefile);
  Fgl_write (0, 423, " q - to save and quit...");
  Fgl_write (0, 433, "   - (then move the mouse)");
  Fgl_write (0, 448, "^C - to quit without saving");
}

void
draw_focus (void)
{
  int x, y;
  hide_mouse ();
  for (y = 0; y < 4; y++)
    for (x = 0; x < 4; x++)
      {
	if (x == focusx && y == focusy)
	  Fgl_fillbox (FOCUSWINX + (FOCUSWINSIZE / 4) * focusx
		       ,FOCUSWINY + (FOCUSWINSIZE / 4) * focusy
		       ,(FOCUSWINSIZE / 4) - 1
		       ,(FOCUSWINSIZE / 4) - 1, FOCUSWINCOLOUR);
	else if (zoom_flag == 2)
	  {
	    if ((x == focusx + 1 && y == focusy)
		|| (x == focusx && y == focusy + 1)
		|| (x == focusx + 1 && y == focusy + 1))
	      Fgl_fillbox (FOCUSWINX + (FOCUSWINSIZE / 4) * x
			   ,FOCUSWINY + (FOCUSWINSIZE / 4) * y
			   ,(FOCUSWINSIZE / 4) - 1
			   ,(FOCUSWINSIZE / 4) - 1, FOCUSWINCOLOUR);
	  }
	else if (x < imagesize && y < imagesize)
	  Fgl_fillbox (FOCUSWINX + (FOCUSWINSIZE / 4) * x
		       ,FOCUSWINY + (FOCUSWINSIZE / 4) * y
		       ,(FOCUSWINSIZE / 4) - 1
		       ,(FOCUSWINSIZE / 4) - 1, UNFOCUSWINCOLOUR);
	else
	  Fgl_fillbox (FOCUSWINX + (FOCUSWINSIZE / 4) * x
		       ,FOCUSWINY + (FOCUSWINSIZE / 4) * y
		       ,(FOCUSWINSIZE / 4) - 1
		       ,(FOCUSWINSIZE / 4) - 1, XFOCUSWINCOLOUR);
      }
  reshow_mouse ();
}

void
draw_swin (void)
{
  int x, y;
  Fgl_fillbox ((SWINSTARTX + 7) - 4 * 8, (SWINSTARTY + 7) - 4 * 8
	       ,16 * 4 + 2, 16 * 4 + 2, 0);
  Fgl_hline ((SWINSTARTX + 7) - imagesize * 8, (SWINSTARTY + 7) - imagesize * 8
	     ,((SWINSTARTX + 8) - imagesize * 8) + 16 * imagesize, 235);
  Fgl_hline ((SWINSTARTX + 7) - imagesize * 8
	     ,((SWINSTARTY + 8) - imagesize * 8) + 16 * imagesize
	     ,((SWINSTARTX + 8) - imagesize * 8) + 16 * imagesize, 235);
  Fgl_line ((SWINSTARTX + 7) - imagesize * 8, (SWINSTARTY + 7) - imagesize * 8
	    ,(SWINSTARTX + 7) - imagesize * 8
	    ,((SWINSTARTY + 8) - imagesize * 8) + 16 * imagesize, 235);
  Fgl_line (((SWINSTARTX + 8) - imagesize * 8) + 16 * imagesize
	    ,(SWINSTARTY + 7) - imagesize * 8
	    ,((SWINSTARTX + 8) - imagesize * 8) + 16 * imagesize
	    ,((SWINSTARTY + 8) - imagesize * 8) + 16 * imagesize, 235);
  for (y = 0; y < 16 * imagesize; y++)
    for (x = 0; x < 16 * imagesize; x++)
      Fgl_setpixel (((SWINSTARTX + 8) - imagesize * 8) + x
		    ,((SWINSTARTY + 8) - imagesize * 8) + y, image[x][y]);
}

void
draw_big_image (void)
{
  if (zoom_flag == 1)
    draw_big_imagezoom ();
  else if (zoom_flag == 2)
    draw_big_image2 ();
  else if (imagesize == 1)
    draw_big_image1 ();
  else if (imagesize == 2)
    draw_big_image2 ();
  else if (imagesize == 3)
    draw_big_image3 ();
  else if (imagesize == 4)
    draw_big_image4 ();
}

void
draw_big_imagezoom (void)
{
  int x, y;
  for (y = 0; y < 16; y++)
    for (x = 0; x < 16; x++)
      Fgl_fillbox (WINSTARTX + x * 16, (WINSTARTY) + y * 16, 15, 15
		   ,image[x + focusx * 16][y + focusy * 16]);
}

void
draw_big_image1 (void)
{
  int x, y;
  for (y = 0; y < 16; y++)
    for (x = 0; x < 16; x++)
      Fgl_fillbox (WINSTARTX + x * 16, (WINSTARTY) + y * 16, 16, 16
		   ,image[x + focusx * 16][y + focusy * 16]);
}

void
draw_big_image2 (void)
{
  int x, y;
  for (y = 0; y < 32; y++)
    for (x = 0; x < 32; x++)
      Fgl_fillbox (WINSTARTX + x * 8, (WINSTARTY) + y * 8, 8, 8
		   ,image[x + focusx * 16][y + focusy * 16]);
}

void
draw_big_image3 (void)
{
  int x, y;
  for (y = 0; y < 48; y++)
    for (x = 0; x < 48; x++)
      Fgl_fillbox (WINSTARTX + x * 5, (WINSTARTY) + y * 5, 5, 5
		   ,image[x + focusx * 5][y + focusy * 5]);
}

void
draw_big_image4 (void)
{
  int x, y;
  for (y = 0; y < 64; y++)
    for (x = 0; x < 64; x++)
      Fgl_fillbox (WINSTARTX + x * 4, (WINSTARTY) + y * 4, 4, 4
		   ,image[x + focusx * 4][y + focusy * 4]);
}

void
change_imagesize (int n)
{
  char s[50];
  imagesize = n;
  focus = 0;
  focusx = 0;
  focusy = 0;
  draw_swin ();
  draw_focus ();
  sprintf (s, "Image size %dx%d", n, n);
  Fgl_write (0, 480 - 20, s);

}

void
change_focus (int x, int y)
{
  if (x >= imagesize || y >= imagesize)
    return;
  focusx = x;
  focusy = y;
  draw_focus ();
  draw_big_image ();
}

void
hide_mouse (void)
{
  Fgl_putbox (mox, moy, 8, 8, under_mouse_pointer);
}

void
reshow_mouse (void)
{
  Fgl_getbox (mx, my, 8, 8, under_mouse_pointer);
  Fgl_putbox (mx, my, 8, 8, mouse_pointer);
}


#ifdef LC_X11

void
parse_args (int argc, char **argv, char **geometry)
{
  /* These variables are used to analyze the command line parameters. */
  int option;
  extern int optind;
  extern char *optarg;

  int bits;			/* Used to set window geometry. */


  /* Check the command line. */
  while ((option = getopt (argc, argv, "vbg:")) != EOF)
    {
      switch (option)
	{
	case 'v':
	  verbose = TRUE;
	  break;
	case 'g':
	  *geometry = optarg;
	  break;
	case 'b':
	  borderx = 0;
	  bordery = 0;
	  break;
	}
    }
  if (verbose)
    printf ("Version %s\n", VERSION);
  if (!(display.dpy = XOpenDisplay (display.dname)))
    {
      HandleError ("Cannot open display.\n", FATAL);
      exit (-1);
    }
/* Record the screen number and root window. */
  display.screen = DefaultScreen (display.dpy);
  display.root = RootWindow (display.dpy, display.screen);

  winW = WINWIDTH + borderx * 2;
  winH = WINHEIGHT + bordery * 2;
  winX = (DisplayWidth (display.dpy, display.screen)
	  - winW) / 2;
  winY = (DisplayHeight (display.dpy, display.screen)
	  - winH) / 2;
  if (*geometry != NULL)
    XParseGeometry (*geometry, &winX, &winY, &winW, &winH);
}

void
Create_Window (char *geometry)
{
  short q;
  XSetWindowAttributes xswa;
  XSizeHints sizehint;
  XWMHints wmhints;
  unsigned char wname[256];	/* Window Name */

  xswa.event_mask = 0;
  xswa.background_pixel = display.bg;
  printf ("DefaultVisual id=%d bp-rgb=%d map-entries=%d\n"
	  ,(*DefaultVisual (display.dpy, display.screen)).visualid
	  ,(*DefaultVisual (display.dpy, display.screen)).bits_per_rgb
	  ,(*DefaultVisual (display.dpy, display.screen)).map_entries);
  display.win = XCreateWindow (display.dpy, display.root,
			       winX, winY,
			       winW, winH, 0,
			       DefaultDepth (display.dpy, display.screen),
		   InputOutput, DefaultVisual (display.dpy, display.screen),
  /*      PseudoColor,  */
			       CWEventMask | CWBackPixel, &xswa);

  sizehint.x = winX;
  sizehint.y = winY;
  sizehint.width = winW;
  sizehint.height = winH;
  sizehint.min_width = winW;
  sizehint.min_height = winH;
  if (geometry != NULL)
    sizehint.flags = USPosition | USSize | PMinSize;
  else
    sizehint.flags = PPosition | PSize | PMinSize;
  XSetNormalHints (display.dpy, display.win, &sizehint);

  display.protocol_atom = XInternAtom (display.dpy, "WM_PROTOCOLS",
				       False);
  display.kill_atom = XInternAtom (display.dpy, "WM_DELETE_WINDOW",
				   False);

  /* Title */
  sprintf ((char *) wname, "xpicedit, Version %s, by IJ Peters",
	   VERSION);
  XChangeProperty (display.dpy, display.win,
		   XA_WM_NAME, XA_STRING, 8, PropModeReplace, wname,
		   strlen ((char *) wname));

  /* Window Manager Hints (This is supposed to make input work.) */
  wmhints.flags = InputHint;
  wmhints.input = True;
  XSetWMHints (display.dpy, display.win, &wmhints);
  XMapWindow (display.dpy, display.win);
  XSelectInput (display.dpy, display.win,
		KeyPressMask | ButtonPressMask | ButtonReleaseMask
		| ExposureMask | StructureNotifyMask);
  for (q = 0; q < 256; q++)
    {
      display.pixcolour_gc[q] = XCreateGC (display.dpy
					   ,display.win, 0, NULL);
      XSetForeground (display.dpy, display.pixcolour_gc[q], q);
      XSetBackground (display.dpy, display.pixcolour_gc[q]
		      ,display.bg);
    }
}

void
HandleError (char *description, int degree)
{
  fprintf (stderr, "An error has occurred.  The description is below... \n");
  fprintf (stderr, "%s\n", description);

  if (degree == FATAL)
    {
      fprintf (stderr, "Program aborting...\n");
      exit (-1);
    }
}

void
Fgl_setpixel (int x, int y, int col)
{
  int i;
  col &= 0xff;
  i = (y + bordery) * (640 + BORDERX) + x + borderx;
  if ((int) *(pixmap + i) != col)
    {
      *(pixmap + i) = (unsigned char) col;
      XDrawPoint (display.dpy, display.win
		  ,display.pixcolour_gc[col], x + borderx, y + bordery);
    }
}

int
Fgl_getpixel (int x, int y)
{
  return (*(pixmap + (y + bordery) * (640 + BORDERX) + x + borderx));
}

void
Fgl_hline (int x1, int y1, int x2, int col)
{
  int x, i;
  col &= 0xff;
  i = (y1 + bordery) * (640 + BORDERX);
  for (x = x1 + borderx; x < x2 + borderx; x++)
    *(pixmap + i + x) = col;
  XDrawLine (display.dpy, display.win
	     ,display.pixcolour_gc[col], x1 + borderx
	     ,y1 + bordery, x2 + borderx, y1 + bordery);
}

void
Fgl_line (int x1, int y1, int dummy, int y2, int col)
     /* vertical lines only. */
{
  int y, i;
  col &= 0xff;
  for (y = y1 + bordery; y < y2 + bordery; y++)
    *(pixmap + x1 + borderx + (y * (640 + BORDERX))) = col;
  XDrawLine (display.dpy, display.win
	     ,display.pixcolour_gc[col], x1 + borderx
	     ,y1 + bordery, x1 + borderx, y2 + bordery);
}

void
Fgl_write (int x, int y, char *s)
{
  int i;
  for (i = 0; i < strlen (s); i++)
    my_x_putchar (x + i * 8, y, s[i]);
}

void
my_x_putchar (int xx, int yy, int c)
{
  int x, y, b;
  for (y = 0; y < 8; y++)
    {
      b = myfont[c * 8 + y];
      for (x = 0; x < 8; x++)
	{
	  if ((b & 0x80) == 0)
	    Fgl_setpixel (xx + x, yy + y, text_bg);
	  else
	    Fgl_setpixel (xx + x, yy + y, text_fg);
	  b = b << 1;
	}
    }
}


void
Fgl_fillbox (int x1, int y1, int w, int h, int col)
{
  int x, y;
  col &= 0xff;
  for (y = y1 + bordery; y < y1 + h + bordery; y++)
    {
      for (x = x1 + borderx; x < x1 + w + borderx; x++)
	*(pixmap + y * (640 + BORDERX) + x) = col;
      XDrawLine (display.dpy, display.win, display.pixcolour_gc[col]
		 ,x1 + borderx, y, x1 + w - 1 + borderx, y);
    }
}

void
Fgl_putbox (int x1, int y1, int x2, int y2, unsigned char *buf)
{
}

void
Fgl_getbox (int x1, int y1, int x2, int y2, unsigned char *buf)
{
}

void
HandleEvent (XEvent * event)
{
  switch (event->type)
    {
    case (KeyPress):
      {
	XKeyEvent *key_event = (XKeyEvent *) event;
	char buf[128];
	KeySym ks;
	XComposeStatus status;
	XLookupString (key_event, buf, 128, &ks, &status);
	if (buf[0] == 'q' || buf[0] == 'Q')
	  stopflag = TRUE;
	else if (buf[0] == 'r' || buf[0] == 'R')
	  refresh_screen (0, 0, 640, 480);
      }
      break;

    case (ButtonPress):
      {
	XButtonEvent *ev = (XButtonEvent *) event;
	mouse_button = ev->button;
      }
      break;

    case (ButtonRelease):
      {
	mouse_button = 0;
      }
      break;

    case (Expose):
      {
	XExposeEvent *ev = (XExposeEvent *) event;
	refresh_screen (ev->x, ev->y
			,ev->x + ev->width, ev->y + ev->height);
      }
      break;

    default:
      printf ("Unknown event %d\n", event->type);
    }
}

void
refresh_screen (int x1, int y1, int x2, int y2)		/* bounds of refresh area */
{
  int x, y;
  for (y = y1; y < y2; y++)
    for (x = x1; x < x2; x++)
      XDrawPoint (display.dpy, display.win
		  ,display.pixcolour_gc[*(pixmap
				  + x + y * (640 + BORDERX)) & 0xff], x, y);
}

#endif

void
lincityrc (void)
{
  char s[256], s1[256];
  int i;
  FILE *rc;

  strcpy (s, getenv ("HOME"));
  strcat (s, "/");
  strcat (s, ".lincityrc");
  if ((rc = fopen (s, "r")) == 0)
    {
      do
	{
	  strcpy (s1, "cat ");
	  strcat (s1, LIBDIR);
	  strcat (s1, "/messages/mousetype.mes");
	  system (s1);
	  i = getchar ();
	}
      while (i < '0' || i > '6');
      if ((rc = fopen (s, "w")) == 0)
	{
	  printf ("Can't open %s for writing, can't continue \n ", s);
	  exit (1);
	}
      fprintf (rc, "mouse=%d\n", i - '0');
      fclose (rc);
      chown (s, getuid (), getgid ());
      if ((rc = fopen (s, "r")) == 0)
	{
	  printf ("What!! can't open %s for reading after writing???\n", s);
	  exit (1);
	}
    }
  while (feof (rc) == 0)
    {
      fgets (s, 99, rc);
      if (sscanf (s, "mouse=%d", &i) != 0)
	lc_mouse_type = i;
    }
  fclose (rc);
}
