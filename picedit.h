/* ---------------------------------------------------------------------- *
 * picedit.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#define VERSION "0.11"

#define TRUE 1
#define FALSE 0
#define WINWIDTH 640		/* default window width */
#define WINHEIGHT 480		/* default window height */
#define BORDERX 30
#define BORDERY 30
#define BORDER	50		/* wasp won't go closer than this to the edges */
#define ALIVE_MASK	(SubstructureNotifyMask | KeyPressMask | PointerMotionMask)

/* Error Codes */
#define FATAL	-1
#define WARNING	-2


/* Type Definitions */
typedef struct _disp
  {
    Window win;
    Display *dpy;
    int screen;
    Window root;
    char *dname;

    long bg;			/* colors */

    XColor bg_xcolor;

    GC pixcolour_gc[256];
    Atom kill_atom, protocol_atom;
    Colormap cmap;
  }
disp;
