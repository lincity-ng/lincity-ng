/* ---------------------------------------------------------------------- *
 * lcx11.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __lcx11_h__
#define __lcx11_h__

#include "lin-city.h"

#define TRUE 1
#define FALSE 0

#if defined (commentout)
#define MOUSE_LEFTBUTTON Button1
#define MOUSE_MIDDLEBUTTON Button2
#define MOUSE_RIGHTBUTTON Button3
#endif

#define WINWIDTH 640		/* default window width */
#define WINHEIGHT 480		/* default window height */


/* Type Definitions */
typedef struct _disp
{
    Display *dpy;
    int screen;
    char *dname;

    Window win;
    Window confinewin;
    Window root;

    unsigned int winH;
    unsigned int winW;

    long bg;			/* colors */
    XColor bg_xcolor;
    GC pixcolour_gc[256];
    Atom kill_atom, protocol_atom;
    Colormap cmap;

    int pointer_confined;
}
disp;

extern int winX, winY, mouse_button, cs_mouse_shifted;
extern disp display;

extern char *bg_color;
extern int verbose;
extern int pix_double;
extern int text_bg;
extern int text_fg;
extern int x_key_value;
extern int x_key_shifted;
extern int borderx, bordery;
extern long unsigned int colour_table[256];
extern int xclip_x1, xclip_y1, xclip_x2, xclip_y2, clipping_flag;
extern unsigned char *open_font;
extern int open_font_height;
extern int suppress_next_expose;
extern int cs_mouse_x, cs_mouse_y;
extern float gamma_correct_red, gamma_correct_green, gamma_correct_blue;
extern Cursor pirate_cursor;

void parse_xargs (int, char **, char **);
void set_pointer_confinement (void);
void do_setcustompalette (XColor *);
void Create_Window (char *);
void HandleError (char *, int);
void HandleEvent (XEvent *);
void refresh_screen (int, int, int, int);
void my_x_putchar (int, int, unsigned char);
void open_x_putchar (int, int, unsigned char);
void do_call_event (int);
void call_event (void);
void call_wait_event (void);
void open_setcustompalette (XColor *);
void drag_screen(void); /* WCK */
void draw_border (void);
void init_mouse (void);

int lc_get_keystroke (void);

#ifdef USE_PIXMAPS
extern Pixmap icon_pixmap[];
#endif

#endif /* __lcx11_h__ */
