/* ---------------------------------------------------------------------- *
 * lcsvga.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __lcsvga_h__
#define __lcsvga_h__

typedef struct _disp
{
    unsigned int winH;
    unsigned int winW;
}
disp;

extern unsigned char mouse_pointer[];
extern unsigned char under_mouse_pointer[];
extern disp display;
extern int pix_double;

void HandleError (char *, int);
int lc_get_keystroke (void);
void init_mouse (void);
void mouse_set_range (int width, int height);

#endif /* __lcsvga_h__ */
