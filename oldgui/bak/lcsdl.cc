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
#include "fileutil.h"
#include "lclib.h"

#include <SDL/SDL.h>

#include "lin-city.h"
#include "lctypes.h"
#include "cliglobs.h"
#include "lcsdl.h"
#include "pixmap.h"
#include "mouse.h"
#include "screen.h"

unsigned char *open_font;
int open_font_height;

int xclip_x1, xclip_y1, xclip_x2, xclip_y2;
int clipping_flag;

#define P1(p) (*(Uint8 *) (p))
#define P2(p) (*(Uint16 *) (p))
#define P4(p) (*(Uint32 *) (p))

#define PI1(p,i) (((Uint8 *) (p))[i])
#define PI2(p,i) (((Uint16 *) (p))[i])
#define PI4(p,i) (((Uint32 *) (p))[i])

#include <execinfo.h>

static void do_backtrace ()
{
	void *array[10];
	char **strings;
	size_t size, i;

	size = backtrace(array, 10);
	strings = backtrace_symbols(array, size);
	for (i = 0; i < size; i++) {
		printf("%s\n", strings[i]);
	}
	free(strings);
	abort();
}

static Uint32 sdl_map_triple (Uint32 col)
{
	if (!(col & 0x80000000)) {
		do_backtrace();
	}
	return sdl_map_rgb((col) >> 16, ((col) >> 8) & 0xff, (col) & 0xff);
}

void sdl_gl_setfontcolors (Uint32 bg, Uint32 fg)
{
	sdl_map_triple(bg);
	sdl_map_triple(fg);

	display.font_bg = bg;
	display.font_fg = fg;
}

void sdl_gl_setfont (int fw, int fh, void *fp)
{
  open_font = (unsigned char*)fp;
	open_font_height = fh;
}

void sdl_gl_setpixel (int x, int y, Uint32 col)
{
	SDL_Surface *surface;
	Uint32 pixel;
	char *pixels;
	int bpp;

	if (clipping_flag)
		if (x < xclip_x1 || x > xclip_x2 || y < xclip_y1 || y > xclip_y2)
			return;

	col = sdl_map_triple(col);

	surface = display.surface;
	pixels = (char*)surface->pixels;
	bpp = surface->format->BytesPerPixel;
	pixels += y * surface->pitch + x * bpp;
	switch (bpp) {

	case 1:
		P1(pixels) = col;
		break;

	case 2:
		P2(pixels) = col;
		break;

	case 3:
		P1(pixels) = col >> 16;
		P1(pixels + 1) = (col >> 8) & 0xff;
		P1(pixels + 2) = col & 0xff;
		break;

	case 4:
		P4(pixels) = col;
		break;
	}
}

static Uint32 sdl_gl_getpixel (int x, int y)
{
	SDL_Surface *surface;
	Uint32 pixel;
	char *pixels;
	int bpp;

	surface = display.surface;
	pixels = (char*)surface->pixels;
	bpp = surface->format->BytesPerPixel;
	pixels += y * surface->pitch + x * bpp;
	switch (bpp) {

	case 1:
		return P1(pixels);

	case 2:
		return P2(pixels);

	case 3:
		pixel = P1(pixels);
		pixel <<= 8;
		pixel |= P1(pixels + 1);
		pixel <<= 8;
		pixel |= P1(pixels + 2);
		return pixel;

	case 4:
		return P4(pixels);
	}
	return 0; /* Oops */
}

static void
sdl_putchar (int xx, int yy, unsigned char c)
{
	Uint32 pixel;
	int x, y, b;

	for (y = 0; y < 8; y++) {
		b = main_font[c * 8 + y];
		for (x = 0; x < 8; x++) {
			pixel = ((b & 0x80) == 0) ? display.font_bg : display.font_fg;
			sdl_gl_setpixel(xx + x, yy + y, pixel);
			b = b << 1;
		}
	}
}

/* Write string */
void sdl_gl_write (int x, int y, char *s)
{
	int i;

	for (i = 0; i < (int) (strlen (s)); i++)
		sdl_putchar (x + i * 8, y, s[i]);
}

/* Copy area to buffer */
void sdl_gl_getbox (int x1, int y1, int w, int h, void *buf)
{
	SDL_Surface *surface;
	Uint8 *pixels, *d;
	int x, y, bpp, len;

	surface = sdl_video();
	bpp = surface->format->BytesPerPixel;
	pixels = ((Uint8*)surface->pixels) + x1 * bpp + y1 * surface->pitch;
	len = w * bpp;
	d =(Uint8*) buf;
	do {
		memcpy(d, pixels, len);
		d += len;
		pixels += surface->pitch;
	} while (--h > 0);
}

/* Copy buffer to area */
void sdl_gl_putbox (int x1, int y1, int w, int h, void *buf)
{
	SDL_Surface *surface;
	Uint8 *pixels, *d;
	int x, y, bpp, len, delta;

	surface = sdl_video();
	bpp = surface->format->BytesPerPixel;
	pixels = (Uint8*)surface->pixels;
	pixels += x1 * bpp + y1 * surface->pitch;
	len = w * bpp;
	d = (Uint8*)buf;

	if (clipping_flag) {
		delta = xclip_y1 - y1;
		if (delta > 0) {
			pixels += surface->pitch * delta;
			d += len * delta;
			y1 += delta;
			h -= delta;
		}
		delta = xclip_x1 - x1;
		if (delta > 0) {
			delta *= bpp;
			pixels += delta;
			d += delta;
			len -= delta;
		}
		delta = y1 + h - xclip_y2 - 1;
		if (delta > 0) {
			h -= delta;
		}
		delta = x1 + w - xclip_x2 - 1;
		if (delta > 0) {
			len -= delta * bpp;
		}

		if (!len || !h)
			return;
	}

	do {
		memcpy(pixels, d, len);
		d += len;
		pixels += surface->pitch;
	} while (--h > 0);

}

void sdl_blit (int x, int y, int w, int h, SDL_Surface *surface)
{
	SDL_Rect srect, drect;

	srect.x = 0;
	srect.y = 0;
	srect.w = w;
	srect.h = h;

	drect.x = x;
	drect.y = y;
	drect.w = w;
	drect.h = h;

	if (clipping_flag) {
		SDL_Rect clip;

		clip.x = xclip_x1;
		clip.y = xclip_y1;
		clip.w = xclip_x2 - xclip_x1 + 1;
		clip.h = xclip_y2 - xclip_y1 + 1;

		SDL_SetClipRect(sdl_video(), &clip);
		SDL_BlitSurface(surface, &srect, sdl_video(), &drect);
		SDL_SetClipRect(sdl_video(), NULL);
	} else {
		SDL_BlitSurface(surface, &srect, sdl_video(), &drect);
	}
}

void sdl_gl_fillbox (int x, int y, int w, int h, Uint32 col)
{
	SDL_Rect rect;

	col = sdl_map_triple(col);

	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	SDL_FillRect(display.surface, &rect, col);
}

void sdl_gl_vline (int x1, int y1, int y2, Uint32 col)
{
	if (clipping_flag) {
		if (x1 < xclip_x1 || x1 > xclip_x2)
			return;

		if (y1 < xclip_y1)
			y1 = xclip_y1;

		if (y2 > xclip_y2)
			y2 = xclip_y2;
	}
	do {
		sdl_gl_setpixel(x1, y1, col);
	} while (++y1 <= y2);
}

void sdl_gl_hline (int x1, int y1, int x2, Uint32 col)
{
	SDL_PixelFormat *format;
	SDL_Surface *surface;
	Uint8 *pixels;
	Uint8 r, g, b;
	int bpp;

	if (clipping_flag) {
		if (y1 < xclip_y1 || y1 > xclip_y2)
			return;

		if (x1 < xclip_x1)
			x1 = xclip_x1;

		if (x2 > xclip_x2)
			x2 = xclip_x2;
	}

	col = sdl_map_triple(col);

	surface = sdl_video();
	format = surface->format;
	bpp = format->BytesPerPixel;
	pixels = ((Uint8*)surface->pixels) + y1 * surface->pitch;

	switch (bpp) {

	case 1:
		memset(pixels + x1, col, x2 - x1);
		return;

	case 2:
		do {
			PI2(pixels, x1) = col;
		} while (++x1 <= x2);
		return;

	case 3:
		x1 *= 3;
		x2 *= 3;
		r = col >> 16;
		g = (col >> 8) & 0xff;
		b = col & 0xff;
		do {
			PI1(pixels, x1) = b;
			PI1(pixels, x1 + 1) = g;
			PI1(pixels, x1 + 2) = r;
		} while ((x1 += 3) <= x2);
		return;

	case 4:
		do {
			PI4(pixels, x1) = col;
		} while (++x1 <= x2);
		return;
	}
}

void sdl_gl_line (int x1, int y1, int x2, int y2, Uint32 c)
{
	int dy, dx, iy, ix, e;

	if (x1 == x2) {
		sdl_gl_vline(x1, y1, y2, c);
		return;
	}
	if (y1 == y2) {
		sdl_gl_hline(x1, y1, x2, c);
		return;
	}

	e = 0;
	dy = y2 - y1;
	dx = x2 - x1;
	if (dy < 0) {
		y1 += dy;
		y2 -= dy;
		dy = -dy;
		x1 += dx;
		x2 -= dx;
		dx = -dx;
	}
	if (dx < 0) {

		/* SW */
		dx = -dx;
		if (dy >= dx) {
			ix = x1;
			for (iy = 0; iy <= dy; iy++) {
				sdl_gl_setpixel(ix, y1, c);
				if (((e += dx) << 1) >= dy) {
					sdl_gl_setpixel(--ix, y1, c);
					e -= dy;
				}
				y1++;
			}
		} else {
			for (ix = -dx; ix <= 0; ix++) {
				sdl_gl_setpixel(ix + x1, y1, c);
				if (((e += dy) << 1) >= dx) {
					sdl_gl_setpixel(ix + x1, ++y1, c);
					e -= dx;
				}
			}
		}
	} else {

		/* SE */
		if (dy >= dx) {
			ix = x1;
			for (iy = 0; iy <= dy; iy++) {
				sdl_gl_setpixel(ix, y1, c);
				if (((e += dx) << 1) >= dy) {
					sdl_gl_setpixel(++ix, y1, c);
					e -= dy;
				}
				y1++;
			}
		} else {
			for (ix = 0; ix <= dx; ix++) {
				sdl_gl_setpixel(ix + x1, y1, c);
				if (((e += dy) << 1) >= dx) {
					sdl_gl_setpixel(ix + x1, ++y1, c);
					e -= dx;
				}
			}
		}
	}
}

void sdl_gl_enableclipping (void)
{
	clipping_flag = 1;
}

void sdl_gl_setclippingwindow (int x1, int y1, int x2, int y2)
{
	xclip_x1 = x1;
	xclip_y1 = y1;
	xclip_x2 = x2;
	xclip_y2 = y2;
}

void sdl_gl_disableclipping (void)
{
	clipping_flag = 0;
}

void
setcustompalette (void)
{
	printf("setcustompalette ignored\n");
	return;

  char s[100];
  int n, r, g, b, i, flag[256];
  SDL_Color pal[256];
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
		if ((n & 0xff) != n || (r & 0x3f) != r || (g & 0x3f) != g || (b & 0x3f) != b) {
			HandleError("Invalid palette", FATAL);
		}

	  pal[n].r = r << 2;
	  pal[n].g = g << 2;
	  pal[n].b = b << 2;
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
    }

  SDL_SetColors(display.surface, pal, 0, 256);
}

void sdl_clear (Uint32 col)
{
	col = sdl_map_triple(col);

	SDL_FillRect(display.surface, NULL, col);
}
