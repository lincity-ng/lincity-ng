/* ---------------------------------------------------------------------- *
 * lcsdl.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __lcsdl_h__
#define __lcsdl_h__

#include "lin-city.h"

#include <SDL/SDL.h>

#define TRUE 1
#define FALSE 0

#define SDL_WIDTH 640		/* default window width */
#define SDL_HEIGHT 480		/* default window height */

typedef struct _disp
{
	SDL_Surface *surface;
	Uint32 font_bg, font_fg;
	unsigned int winH;
	unsigned int winW;
} disp;

extern disp display;

void HandleError (char *, int);

int lc_get_keystroke (void);

#ifdef USE_PIXMAPS
extern Pixmap icon_pixmap[];
#endif

extern int pix_double;
extern unsigned char *open_font;
extern int open_font_height;

extern int xclip_x1, xclip_y1, xclip_x2, xclip_y2;
extern int clipping_flag;

#endif /* __lcsdl_h__ */
