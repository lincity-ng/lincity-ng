/* ---------------------------------------------------------------------- *
 * lcx11.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include "lcconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "lcstring.h"
#include "lcintl.h"

/* this is for OS/2 - RVI */
#ifdef __EMX__
#include <float.h>
#endif

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include "lin-city.h"
#include "lctypes.h"
#include "cliglobs.h"
#include "lcx11.h"
#include "pixmap.h"
#include "mouse.h"
#include "screen.h"

#define USE_IMAGES 1

void
set_pointer_confinement (void)
{
    if (confine_flag) {
	XGrabPointer (display.dpy, display.win, 0,
		      ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
		      GrabModeAsync, GrabModeAsync, 
		      display.win, None, CurrentTime);
    } else {
	XUngrabPointer (display.dpy, CurrentTime);
    }
}

void
setcustompalette (void)
{
  char s[100];
  int n, r, g, b, i, flag[256];
  XColor pal[256];
  FILE *inf;
  for (i = 0; i < 256; i++)
    flag[i] = 0;
  if ((inf = fopen (colour_pal_file, "r")) == 0)
    HandleError ("Can't find the colour pallet file", FATAL);

  while (feof (inf) == 0)
    {
      fgets (s, 99, inf);
      if (sscanf (s, "%d %d %d %d", &n, &r, &g, &b) == 4)
	{
	  pal[n].red = r;
	  pal[n].green = g;
	  pal[n].blue = b;
	  pal[n].flags = DoRed | DoGreen | DoBlue;
	  pal[n].pixel = colour_table[n];	/* ??? */

	  flag[n] = 1;
	}
    }
  fclose (inf);
  for (i = 0; i < 256; i++)
    {
      if (flag[i] == 0)
	{
	  printf ("Colour %d not loaded\n", i);
	  HandleError ("Can't continue", FATAL);
	}
      pal[i].red = (unsigned char) ((pal[i].red
		  * (1 - gamma_correct_red)) + (64 * sin ((float) pal[i].red
					* M_PI / 128)) * gamma_correct_red);

      pal[i].green = (unsigned char) ((pal[i].green
	      * (1 - gamma_correct_green)) + (64 * sin ((float) pal[i].green
				      * M_PI / 128)) * gamma_correct_green);

      pal[i].blue = (unsigned char) ((pal[i].blue
		* (1 - gamma_correct_blue)) + (64 * sin ((float) pal[i].blue
				       * M_PI / 128)) * gamma_correct_blue);
    }

  do_setcustompalette (pal);
}

void
open_setcustompalette (XColor * inpal)
{
  do_setcustompalette (inpal);
}

void
do_setcustompalette (XColor * inpal)
{
  int i, n, me = 0, flag[256], vid;
  int depth;
  long unsigned int plane_masks[3];
  XColor pal[256];
  int writeable_p;

  display.cmap = XDefaultColormap (display.dpy, display.screen);
  depth = DefaultDepth (display.dpy, display.screen);

  /* Decide, if the colormap is writable */
  {
    Visual *visual = DefaultVisual (display.dpy, display.screen);
#if defined(__cplusplus) || defined(c_plusplus)
    int visual_class = visual->c_class;
#else
    int visual_class = visual->class;
#endif
    writeable_p = (visual_class == PseudoColor || visual_class == GrayScale);
  }

  if (writeable_p)
    {
      if (XAllocColorCells (display.dpy, display.cmap, 0
			    ,plane_masks, 0, colour_table, 256) == 0)
	{
	  me = (*DefaultVisual (display.dpy, display.screen)).map_entries;
	  vid = (*DefaultVisual (display.dpy, display.screen)).visualid;
	  display.cmap = XCreateColormap (display.dpy, display.win
				,DefaultVisual (display.dpy, display.screen)
	  /*      ,PseudoColor */
					  ,AllocNone);
	  if (me == 256 && depth != 24)
	    {
	      if (XAllocColorCells (display.dpy, display.cmap, 0
				    ,plane_masks, 0, colour_table, 256) != 0) {
		  /* printf ("Allocated 256 cells\n"); */
	      }
	      else {
		  printf ("Couldn't allocate 256 cells\n");
	      }
	    }
	  else
	    for (i = 0; i < 256; i++)
	      colour_table[i] = i;
	}
      if (!display.cmap)
	HandleError ("No default colour map", FATAL);
    }

  for (i = 0; i < 256; i++)
    flag[i] = 0;

  for (n = 0; n < 256; n++)
    {
      pal[n].red = inpal[n].red << 10;
      pal[n].green = inpal[n].green << 10;
      pal[n].blue = inpal[n].blue << 10;
      pal[n].flags = DoRed | DoGreen | DoBlue;
      if (writeable_p)
	pal[n].pixel = colour_table[n];
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

  if (writeable_p)
    {
      XStoreColors (display.dpy, display.cmap, pal, 256);
      XFlush (display.dpy);
    }
  XSetWindowColormap (display.dpy, display.win, display.cmap);
}

#if defined (commentout)
void
initfont ()
{
  int i;
  FILE *finf;
  if ((finf = fopen (fontfile, "r")) == 0)
    HandleError ("Can't open the font file", FATAL);
  for (i = 0; i < 256 * 8; i++)
    myfont[i] = fgetc (finf);
  fclose (finf);
}
#endif

void
Fgl_setfontcolors (int bg, int fg)
{
  text_fg = fg;
  text_bg = bg;
}

void
Fgl_setfont (int fw, int fh, void *fp)
{
  open_font = fp;
  open_font_height = fh;
}

void
parse_xargs (int argc, char **argv, char **geometry)
{
    int option;
    extern char *optarg;

#ifdef ALLOW_PIX_DOUBLING
//    printf ("Options include Pix doubling\n");
    while ((option = getopt (argc, argv, "vbrndg:wR:G:B:")) != EOF)
#else
	while ((option = getopt (argc, argv, "vbrng:wR:G:B:")) != EOF)
#endif
	{
	    switch (option)
	    {
	    case 'v':
		verbose = TRUE;
		break;
	    case 'g':
		*geometry = optarg;
		break;
#ifdef ALLOW_PIX_DOUBLING
	    case 'd':
		pix_double = 1;
		/* Fall through.  We are not allowed a border with pix doubling */
#endif
	    case 'b':
		borderx = 0;
		bordery = 0;
		break;
	    case 'r':
		borderx = BORDERX;
		bordery = BORDERY;
		break;
	    case 'n':
		no_init_help = TRUE;
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
    if (verbose)
	printf (_("Version %s\n"), VERSION);
    if (!(display.dpy = XOpenDisplay (display.dname)))
    {
	printf (" Can't open the dispay!\n");
	HandleError ("Cannot open display.\n", FATAL);
	exit (-1);
    }
    /* Record the screen number and root window. */
    display.screen = DefaultScreen (display.dpy);
    display.root = RootWindow (display.dpy, display.screen);

    display.winW = WINWIDTH + borderx * 2 + pix_double * WINWIDTH;
    display.winH = WINHEIGHT + bordery * 2 + pix_double * WINHEIGHT;
    winX = (DisplayWidth (display.dpy, display.screen) - display.winW) / 2;
    winY = (DisplayHeight (display.dpy, display.screen) - display.winH) / 2;
    if (*geometry != NULL)
	XParseGeometry (*geometry, &winX, &winY, &display.winW, &display.winH);
}


void
Create_Window (char *geometry)
{
    short q;
    Visual *vid;
    XSetWindowAttributes xswa;
    XSizeHints sizehint;
    XWMHints wmhints;
    int depth;
    unsigned char wname[256];	/* Window Name */
    unsigned long vmask = CWEventMask | CWBackPixel | CWBackingStore;

    depth = DefaultDepth (display.dpy, display.screen);
    xswa.event_mask = 0;
    xswa.background_pixel = display.bg;
    xswa.backing_store = WhenMapped;
    printf ("DefaultVisual id=%d bp-rgb=%d map-entries=%d\n"
	    ,(int) (*DefaultVisual (display.dpy, display.screen)).visualid
	    ,(*DefaultVisual (display.dpy, display.screen)).bits_per_rgb
	    ,(*DefaultVisual (display.dpy, display.screen)).map_entries);
    vid = DefaultVisual (display.dpy, display.screen);
    display.cmap
	    = XDefaultColormap (display.dpy, display.screen);
    display.win = XCreateWindow (display.dpy, display.root,
				 winX, winY,
				 display.winW, display.winH, 0, depth,
				 InputOutput,	/* vid , */
				 DefaultVisual (display.dpy, display.screen),
				 /*      PseudoColor,  */
				 vmask, &xswa);

    sizehint.x = winX - 100;
    sizehint.y = winY;
    sizehint.width = display.winW;
    sizehint.height = display.winH;
    sizehint.min_width = display.winW;
    sizehint.min_height = display.winH;
    sizehint.max_width = display.winW;
    sizehint.max_height = display.winH;
    /* GCS FIX:  Be careful about resizing the opening screen */
    /* WCK: Fixed.  We lock it now, and unlock it after the opening screen.
       not gorgeous, but it works for now.  Still need to clean up.*/
#define NO_RESIZABLE_WINDOWS 1
    if (geometry != NULL) {
#if defined (NO_RESIZABLE_WINDOWS)
	sizehint.flags = USPosition | USSize | PMinSize | PMaxSize;
#else
	sizehint.flags = USPosition | USSize | PMinSize;
#endif
    } else {
#if defined (NO_RESIZABLE_WINDOWS)
	sizehint.flags = PPosition | PSize | PMinSize | PMaxSize;
#else
	sizehint.flags = PPosition | PSize | PMinSize;
#endif
    }
    XSetNormalHints (display.dpy, display.win, &sizehint);

    display.protocol_atom = XInternAtom (display.dpy, "WM_PROTOCOLS",
					 False);
    display.kill_atom = XInternAtom (display.dpy, "WM_DELETE_WINDOW",
				     False);

    /* Title */
    sprintf ((char *) wname,
	     _("xlincity, Version %s, "
	     "(Copyright) IJ Peters - copying policy GNU GPL"),
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
	XSetGraphicsExposures (display.dpy, display.pixcolour_gc[q]
			       ,False);
    }
}

void 
unlock_window_size (void) 
{
    XSizeHints sizehint;

    sizehint.x = winX - 100;
    sizehint.y = winY;
    sizehint.width = display.winW;
    sizehint.height = display.winH;
    sizehint.min_width = display.winW;
    sizehint.min_height = display.winH;
    sizehint.max_width = display.winW;
    sizehint.max_height = display.winH;

    sizehint.flags = USPosition | USSize | PMinSize;

    XSetNormalHints (display.dpy, display.win, &sizehint);
}



void
HandleError (char *description, int degree)
{
  fprintf (stderr,
	   "An error has occurred.  The description is below...\n");
  fprintf (stderr, "%s\n", description);

  if (degree == FATAL) {
      fprintf (stderr, _("Program aborting...\n"));
      exit (-1);
    }
}

void
Fgl_setpixel (int x, int y, int col)
{
    int i;
    if (clipping_flag)
	if (x < xclip_x1 || x > xclip_x2 || y < xclip_y1 || y > xclip_y2)
	    return;
    col &= 0xff;

    i = pixmap_index(x,y);

#ifdef ALLOW_PIX_DOUBLING
    if (pix_double) {
	if ((int) pixmap[i] != col)
	{
	    pixmap[i] = (unsigned char) col;
	    XFillRectangle (display.dpy, display.win,
			    display.pixcolour_gc[col], x * 2, y * 2, 2, 2);
	}
    } else {
#endif
	if ((int) pixmap[i] != col)
	{
	    pixmap[i] = (unsigned char) col;
	    XDrawPoint (display.dpy, display.win,
			display.pixcolour_gc[col], x + borderx, y + bordery);
	}
#ifdef ALLOW_PIX_DOUBLING
    }
#endif
}

int
Fgl_getpixel (int x, int y)
{
    return pixmap_getpixel (x, y);
}

void
Fgl_hline (int x1, int y1, int x2, int col)
{
    col &= 0xff;
    pixmap_hline (x1, y1, x2, col);
#ifdef ALLOW_PIX_DOUBLING
    if (pix_double)
	XFillRectangle (display.dpy, display.win
			,display.pixcolour_gc[col], x1 * 2, y1 * 2
			,(x2 - x1) * 2 + 1, 2);
    else
#endif
	XDrawLine (display.dpy, display.win
		   ,display.pixcolour_gc[col], x1 + borderx
		   ,y1 + bordery, x2 + borderx, y1 + bordery);
}

void
Fgl_line (int x1, int y1, int dummy, int y2, int col)
     /* vertical lines only. */
{
    col &= 0xff;
    pixmap_vline (x1, y1, y2, col);
#ifdef ALLOW_PIX_DOUBLING
    if (pix_double)
	XFillRectangle (display.dpy, display.win
			,display.pixcolour_gc[col], x1 * 2, y1 * 2
			,2, (y2 - y1) * 2 + 1);
    else
#endif
	XDrawLine (display.dpy, display.win
		   ,display.pixcolour_gc[col], x1 + borderx
		   ,y1 + bordery, x1 + borderx, y2 + bordery);
}

void
Fgl_write (int x, int y, char *s)
{
  int i;
  for (i = 0; i < (int) (strlen (s)); i++)
    my_x_putchar (x + i * 8, y, s[i]);
}

void
open_write (int x, int y, char *s)
{
  int i;
  for (i = 0; i < (int) (strlen (s)); i++)
    open_x_putchar (x + i * 8, y, s[i]);
}

void
my_x_putchar (int xx, int yy, unsigned char c)
{
  int x, y, b;
  for (y = 0; y < 8; y++)
    {
      b = main_font[c * 8 + y];
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
open_x_putchar (int xx, int yy, unsigned char c)
{
  int x, y, b;
  for (y = 0; y < open_font_height; y++)
    {
      b = open_font[c * open_font_height + y];
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
    if (clipping_flag) {
	if (x1 < xclip_x1)
	    x1 = xclip_x1;
	if (x1 + w - 1 > xclip_x2)
	    w = xclip_x2 - x1 + 1;
	if (y1 < xclip_y1)
	    y1 = xclip_y1;
	if (y1 + h - 1 > xclip_y2)
	    h = xclip_y2 - y1 + 1;
    }
    col &= 0xff;
    pixmap_fillbox (x1, y1, w, h, col);

#ifdef ALLOW_PIX_DOUBLING
    if (pix_double)
	XFillRectangle (display.dpy, display.win,
			display.pixcolour_gc[col], x1 * 2, y1 * 2, w * 2, h * 2);
    else
#endif
	XFillRectangle (display.dpy, display.win,
			display.pixcolour_gc[col], x1 + borderx, y1 + bordery, w, h);
}

#ifdef USE_IMAGES

/*
 * Instead of transfering a pixmap pixel by pixel, it is much more
 * efficient to build an XImage in core and send it in one piece off
 * to the X server.
 *
 * -- GB.
 */

static int
clamp (int x, int low, int high)
     /* clamp x to the interval [low, high] */
{
  if (x < low)
    x = low;
  else if (x > high)
    x = high;
  return x;
}

/*
 * Copy the the sub-image (src_x, src_y, w, h) to the screen at 
 * (x0 + x1, y0 + y1).
 * `src' is a pointer to the array of pixels.
 * `bpl' is the width of that array.
 * No clipping performed.
 */
void
Fgl_putbox_low (Drawable dst, int x0, int y0, int x1, int y1,
		int w, int h, unsigned char *src, int bpl,
		int src_x, int src_y)
{
    XImage *im;
    int x, y;

#ifdef ALLOW_PIX_DOUBLING
    const int pmult = pix_double ? 2 : 1;
#else
    const int pmult = 1;
#endif

    im = XCreateImage (display.dpy, 0,	/* display and visual */
		       DefaultDepth (display.dpy, display.screen),	/* depth */
		       ZPixmap,	/* format */
		       0,		/* offset */
		       0,		/* data */
		       pmult * w, pmult * h,	/* width and height */
		       32, 0);	/* bitmap_pad and bytes_per_line */
    /* XXX: assert is not the right way to check for errors - wck */
    assert (im != 0);
    im->data = (char *) malloc (im->bytes_per_line * pmult * h);
    assert (im->data != 0);

    src += src_x + src_y * bpl;

    if (pmult == 1) {
	for (y = 0; y < h; y++)
	    for (x = 0; x < w; x++)
		XPutPixel (im, x, y, colour_table[src[x + y * bpl]]);
    } else {
	for (y = 0; y < h; y++)
	    for (x = 0; x < w; x++)
	    {
		unsigned long c = colour_table[src[x + y * bpl]];

		XPutPixel (im, 2 * x + 0, 2 * y + 0, c);
		XPutPixel (im, 2 * x + 0, 2 * y + 1, c);
		XPutPixel (im, 2 * x + 1, 2 * y + 0, c);
		XPutPixel (im, 2 * x + 1, 2 * y + 1, c);
	    }
    }

    if (dst == display.win) {
	pixmap_putbox (src, 0, 0, bpl, x1, y1, w, h);
    }

    XPutImage (display.dpy, dst, display.pixcolour_gc[0], im, 0, 0,
	       x0 + pmult * x1, y0 + pmult * y1, pmult * w, pmult * h);

    XDestroyImage (im);
}

void
Fgl_putbox (int x, int y, int w, int h, void *buf)
{
    int c_x0 = clipping_flag ? xclip_x1 : 0;
    int c_x1 = clipping_flag ? xclip_x2 : display.winW - 1;
    int c_y0 = clipping_flag ? xclip_y1 : 0;
    int c_y1 = clipping_flag ? xclip_y2 : display.winH - 1;
    int x1 = clamp (x, c_x0, c_x1);
    int y1 = clamp (y, c_y0, c_y1);
    int x2 = clamp (x + w, c_x0, c_x1 + 1);
    int y2 = clamp (y + h, c_y0, c_y1 + 1);

    if (x2 > x1 && y2 > y1)
	Fgl_putbox_low (display.win, borderx, bordery, x1, y1, x2 - x1, 
			y2 - y1, (unsigned char *) buf, w, x1 - x, y1 - y);
}

#else

void
Fgl_putbox (int x1, int y1, int w, int h, void *buf)
{
    unsigned char *b;
    b = (unsigned char *) buf;
    int x, y;
    for (y = y1; y < y1 + h; y++)
	for (x = x1; x < x1 + w; x++)
	    Fgl_setpixel (x, y, *(b++));
}

#endif

void
Fgl_getbox (int x1, int y1, int w, int h, void *buf)
{
    unsigned char *b;
    int x, y;
    b = (unsigned char *) buf;
    for (y = y1; y < y1 + h; y++)
	for (x = x1; x < x1 + w; x++)
	    *(b++) = (unsigned char) Fgl_getpixel (x, y);
}


void
Fgl_getrect(Rect * r, void * buffer)
{
    Fgl_getbox(r->x,r->y,r->w,r->h,buffer);
}

void 
Fgl_putrect(Rect *r, void * buffer)
{
    Fgl_putbox(r->x,r->y,r->w,r->h,buffer);
}


void
HandleEvent (XEvent * event)
{

    XEvent loop_ev; /* for clearing the queue of events */

    switch (event->type)
    {
    case (KeyPress):
	{
	    XKeyEvent *key_event = (XKeyEvent *) event;
	    char buf[128];
	    KeySym ks;
	    XComposeStatus status;
	    XLookupString (key_event, buf, 128, &ks, &status);
	    x_key_shifted = ShiftMask & key_event->state;
	    x_key_value = buf[0];
	    switch (ks) {
	    case XK_Left:
		x_key_value = 1;
		break;
	    case XK_Down:
		x_key_value = 2;
		break;
	    case XK_Up:
		x_key_value = 3;
		break;
	    case XK_Right:
		x_key_value = 4;
		break;
	    case XK_BackSpace:
	    case XK_Delete:
		x_key_value = 127;
		break;
	    }
	}
	break;

    case (MotionNotify): /* added by WCK */
	{
	    XMotionEvent *ev = (XMotionEvent *) event;

	    while (XCheckMaskEvent(display.dpy,PointerMotionMask,&loop_ev)) {
		ev = (XMotionEvent *) &loop_ev;
	    }

	    /* XXX: how will grabbing events out of the queue affect the world? */
#ifdef DEBUG_X11_MOUSE
	    printf("pointer motion event\n");
#endif

	    if (ev->state & Button2Mask)
		drag_screen();

	}
	break;

    case (ButtonPress):
	{
	    XButtonEvent *ev = (XButtonEvent *) event;
	    if ((ev->state & ShiftMask) != 0)
		cs_mouse_shifted = 1;
	    else
		cs_mouse_shifted = 0;
#ifdef DEBUG_X11_MOUSE
	    printf("button press: ev->button = %d\n",ev->button);
#endif
#if defined (commentout)
	    mouse_button = ev->button;
#endif
	    switch (ev->button) {
	    case Button1:
		mouse_button = LC_MOUSE_LEFTBUTTON | LC_MOUSE_PRESS;
		break;
	    case Button2:
		mouse_button = LC_MOUSE_MIDDLEBUTTON | LC_MOUSE_PRESS;
		break;
	    case Button3:
		mouse_button = LC_MOUSE_RIGHTBUTTON | LC_MOUSE_PRESS;
		break;
	    }
	    cs_mouse_handler (mouse_button, 0, 0);
	    mouse_button = 0;
	}
	break;

    case (ButtonRelease):
	{
	    XButtonEvent *ev = (XButtonEvent *) event;
	    mouse_button = ev->button; 
#ifdef DEBUG_X11_MOUSE
	    printf("button release: ev->button = %d\n",ev->button);
#endif
	    switch (ev->button) {
	    case Button1:
		mouse_button = LC_MOUSE_LEFTBUTTON | LC_MOUSE_RELEASE;
		break;
	    case Button2:
		mouse_button = LC_MOUSE_MIDDLEBUTTON | LC_MOUSE_RELEASE;
		break;
	    case Button3:
		mouse_button = LC_MOUSE_RIGHTBUTTON | LC_MOUSE_RELEASE;
		break;
	    }
	    cs_mouse_handler (mouse_button, 0, 0);
	    mouse_button = 0;
	}
	break;

    case (Expose):
	{
	    XExposeEvent *ev = (XExposeEvent *) event;

	    while (XCheckMaskEvent(display.dpy,ExposureMask,&loop_ev)) 
	    {
	      ev = (XExposeEvent *) &loop_ev;
	    }
	    if (suppress_next_expose) {
		suppress_next_expose = 0;
		break;
	    }
	    refresh_screen (ev->x, ev->y, ev->x + ev->width, ev->y + ev->height);
	}
	break;
    case ConfigureNotify:
	{
	    XConfigureEvent *ev = (XConfigureEvent *) event;

#if defined (commentout)
            /* Not sure yet, but need ResizeRedirectMask or 
	       VisibilityChangeMask */
	    while (XCheckMaskEvent(display.dpy,StructureNotifyMask, &loop_ev)) 
	    {
	        if (loop_ev.type != ConfigureNotify) 
		{
                    /* We don't seem to do anything with the others anyway,
		       but I want to know for now */
                    fprintf(stderr,"StructureNotifyMask != ConfigureNotify, "
			    "loop_ev.type = %d\n", loop_ev.type);
		    return;
		}
		ev = (XConfigureEvent *) &loop_ev;
	    }
#endif
	    while (XCheckTypedEvent(display.dpy, ConfigureNotify, &loop_ev)) {
		ev = (XConfigureEvent *) &loop_ev;
	    }
	    resize_geometry (ev->width, ev->height);
	}
	break;
    }
    /*fprintf(stderr,"Handler fell through, event->type = %d\n",event->type);*/
}

void
refresh_screen (int x1, int y1, int x2, int y2)		/* bounds of refresh area */
{
#ifdef USE_IMAGES
  int wx1 = x1-borderx < 0 ? 0 : x1-borderx;
  int wy1 = y1-bordery < 0 ? 0 : y1-bordery;
  int wx2 = x2-borderx > pixmap_width ? pixmap_width : x2-borderx;
  int wy2 = y2-bordery > pixmap_height ? pixmap_height : y2-bordery;
  if (wx2-wx1 <= 0 || wy2-wy1 <= 0) {
      /* Note: the "< 0" part can happen for when x1 is in the right border, 
	 or when y1 is in the left border. */
      return;
  }
  Fgl_putbox_low (display.win, borderx, bordery, wx1, wy1, 
		  wx2 - wx1, wy2 - wy1, (unsigned char*) pixmap, 
		  pixmap_width, wx1, wy1);
#else
  int x, y;
#ifdef ALLOW_PIX_DOUBLING
  if (pix_double)
    {
      for (y = y1; y < y2; y++)
	for (x = x1; x < x2; x++)
	  XFillRectangle (display.dpy, display.win
			  ,display.pixcolour_gc[*(pixmap + x + y
			    * (640 + BORDERX)) & 0xff], x * 2, y * 2, 2, 2);
    }
  else
    {
#endif
      for (y = y1; y < y2; y++)
	for (x = x1; x < x2; x++)
	  XDrawPoint (display.dpy, display.win
		      ,display.pixcolour_gc[*(pixmap
				  + x + y * (640 + BORDERX)) & 0xff], x, y);
#ifdef ALLOW_PIX_DOUBLING
    }
#endif /* ALLOW_PIX_DOUBLING */
#endif /* USE_IMAGES */
}

void
Fgl_enableclipping (void)
{
  clipping_flag = 1;
}

void
Fgl_setclippingwindow (int x1, int y1, int x2, int y2)
{
  xclip_x1 = x1;
  xclip_y1 = y1;
  xclip_x2 = x2;
  xclip_y2 = y2;
}

void
Fgl_disableclipping (void)
{
  clipping_flag = 0;
}

void
do_call_event (int wait)
{
  int dummy_int, x, y;
  Window dummy_win;
  XEvent xev;
  if (XPending (display.dpy))
    {
      XNextEvent (display.dpy, &xev);
      HandleEvent (&xev);
    }
  else if (wait)
    lc_usleep (1000);

  /* WCK: this could possibly be better handled with a MotionEvent */
  XQueryPointer (display.dpy, display.win, &dummy_win, &dummy_win
	      ,&dummy_int, &dummy_int, &x, &y, (unsigned int *) &dummy_int);
#ifdef ALLOW_PIX_DOUBLING
  if (pix_double)
    {
      x /= 2;
      y /= 2;
    }
  else
    {
#endif
      x -= borderx;
      y -= bordery;
#ifdef ALLOW_PIX_DOUBLING
    }
#endif
  if (x != cs_mouse_x || y != cs_mouse_y)
  cs_mouse_handler (0, x - cs_mouse_x, y - cs_mouse_y);  
  /* WCK: no longer passing mouse_button, 0 used instead. Presses are handled
   in the event loop */
}

void
call_event (void)
{
  do_call_event (0);
}

void
call_wait_event (void)
{
  do_call_event (1);
}

int 
lc_get_keystroke (void)
{
    int q;
    call_event ();
    q = x_key_value;
    x_key_value = 0;
    return q;
}


/* init_full_mouse is called just before the main client loop. */
   
void 
init_full_mouse (void) /* added by WCK */
{
  XSelectInput (display.dpy, display.win,
		KeyPressMask | ButtonPressMask | ButtonReleaseMask
		| ExposureMask | StructureNotifyMask | Button2MotionMask);
  /* This should be done better, only specifying Button2MotionMask when
     button 2 is pressed. */
}

void
draw_border (void)
{
    int col = TEXT_BG_COLOUR & 0xff;
    int pd = pix_double + 1;
    if (borderx > 0) {
	XFillRectangle (display.dpy, display.win, display.pixcolour_gc[col],
			0, bordery, borderx, display.winH - 2*bordery);
	XFillRectangle (display.dpy, display.win, display.pixcolour_gc[col],
			display.winW - borderx, bordery,
			borderx, display.winH - 2*bordery);
    }
    if (bordery > 0) {
	XFillRectangle (display.dpy, display.win, display.pixcolour_gc[col],
			0, 0, display.winW, bordery);
	XFillRectangle (display.dpy, display.win, display.pixcolour_gc[col],
			0, display.winH - bordery, display.winW, bordery);
    }
}

#ifdef USE_PIXMAPS

void
init_icon_pixmap (short type)
{
    unsigned char *g;
#if !defined USE_IMAGES
    int x, y;
#endif
    int grp;

    grp = get_group_of_type(type);

#ifdef ALLOW_PIX_DOUBLING
    if (pix_double) {
	icon_pixmap[type] 
		= XCreatePixmap (display.dpy, display.win,
				 main_groups[grp].size * 16 * 2,
				 main_groups[grp].size * 16 * 2,
				 DefaultDepth (display.dpy,
					       display.screen));
    } else {
#endif
	icon_pixmap[type] 
		= XCreatePixmap (display.dpy, display.win,
				 main_groups[grp].size * 16,
				 main_groups[grp].size * 16,
				 DefaultDepth (display.dpy,
					       display.screen));
#ifdef ALLOW_PIX_DOUBLING
    }
#endif

    g = (unsigned char *) main_types[type].graphic;

#ifdef USE_IMAGES
    Fgl_putbox_low (icon_pixmap[type],
		    0, 0, 0, 0, main_groups[grp].size * 16, 
		    main_groups[grp].size * 16,
		    g, main_groups[grp].size * 16,
		    0, 0);
#else
#ifdef ALLOW_PIX_DOUBLING
    if (pix_double)	{
	for (y = 0; y < main_groups[grp].size * 16; y++)
	    for (x = 0; x < main_groups[grp].size * 16; x++)
		XFillRectangle (display.dpy, icon_pixmap[type]
				,display.pixcolour_gc[*(g++)]
				,x * 2, y * 2, 2, 2);
    } else {
#endif
	for (y = 0; y < main_groups[grp].size * 16; y++)
	    for (x = 0; x < main_groups[grp].size * 16; x++)
		XDrawPoint (display.dpy, icon_pixmap[type]
			    ,display.pixcolour_gc[*(g++)], x, y);
#ifdef ALLOW_PIX_DOUBLING
    }
#endif /* ALLOW_PIX_DOUBLING */
#endif /* USE_IMAGES */
}

#endif /* USE_PIXMAPS */
