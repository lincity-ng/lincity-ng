/* ---------------------------------------------------------------------- *
 * pixmap.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __pixmap_h__
#define __pixmap_h__

/* ---------------------------------------------------------------------- *
 * Public Global Variables
 * ---------------------------------------------------------------------- */
extern char* pixmap;
extern int pixmap_height;
extern int pixmap_width;

/* ---------------------------------------------------------------------- *
 * Public Fn Prototypes
 * ---------------------------------------------------------------------- */
void initialize_pixmap (void);
void resize_pixmap (int new_width, int new_height);
int pixmap_getpixel (int x, int y);
int pixmap_index (int x, int y);
int pixmap_getpixel (int x, int y);
void pixmap_setpixel (int x, int y, int col);
void pixmap_hline (int x1, int y1, int x2, int col);
void pixmap_vline (int x1, int y1, int y2, int col);
void pixmap_fillbox (int x, int y, int w, int h, int col);
void pixmap_putbox (char* src, int src_x, int src_y, int src_w,
		    int dst_x, int dst_y, int w, int h);
void free_pixmap (void);

/* ---------------------------------------------------------------------- *
 * Public Inline Functions
 * ---------------------------------------------------------------------- */
#if defined (WIN32)
inline int 
pixmap_index (int x, int y) {
    return y*pixmap_width + x;
}
#endif

#endif	/* __pixmap_h__ */
