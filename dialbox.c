/* ---------------------------------------------------------------------- *
 * dialbox.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * Portions copyright (c) Corey Keasling, 2001.
 * ---------------------------------------------------------------------- */

#include "lcconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h> /* XXX: WCK: What does configure need to know? */
#include "screen.h"
#include "geometry.h"
#include "dialbox.h"

static Dialog_Box dbox_entry[MAX_DBOX_ENTRIES];
static Dialog_Box * b[MAX_DBOX_ENTRIES];
static Dialog_Box * l[MAX_DBOX_ENTRIES];

static int dbn; /* number of dbox entries */
static int bn;  /* number of buttons */
static int ln;  /* number of lines */

static int bwt;                   /* total width of all buttons, pixels */
static int lwt;                   /* pixel width of longest line */

static int bw[MAX_DBOX_ENTRIES];  /* pixel width of each button */
static int lw[MAX_DBOX_ENTRIES];  /* pixel width of each line */

static int bs; /* button spacing */
static int bse; /* extra spacing, to be added at beginning and end of line */

static int wt; /* Total width of text area, including spacing */
static int ht; /* Total height of text area, including spacing */

static int bx[MAX_DBOX_ENTRIES]; /* X location of button */
static int by[MAX_DBOX_ENTRIES]; /* Y location */

static int lx[MAX_DBOX_ENTRIES]; /* X location of line */
static int ly[MAX_DBOX_ENTRIES]; /* Y location */

static int color;

Rect dialog_window;   /* Describes position of window on screen */
Rect text_window;     /* Describes position of text area on screen */

int
dialog_box(int arg_color, char argc, ...)
{
  va_list ap;
  int i;

  bn = 0; ln = 0; dbn = 0;
  bwt = 0; lwt = 0;
  bs = 0; bse = 0;
  wt = 0; ht = 0;
  color = arg_color;

  if (argc > MAX_DBOX_ENTRIES) {
    fprintf(stderr,"Too many buttons in dialog_box!\n"
	    "Tweak MAX_DBOX_ENTRIES\n");
    exit(212);
  }  

  va_start(ap, argc);

  /* For each argument pair, get the arguments, determine line or button,
     calculate width/length, adjust total size accordingly, increment type
     count. */
  
  for (i = 0; i < argc; i++) {
    dbn++;

    dbox_entry[i].type = (short) va_arg(ap, int);
    dbox_entry[i].text = va_arg(ap, char *);

    if (dbox_entry[i].type) {
      b[bn] = &dbox_entry[i];
      bw[bn] = (strlen(dbox_entry[i].text) * CHAR_WIDTH);
      bwt += bw[bn];
      bn++;
    } else {
      lw[ln] = (strlen(dbox_entry[i].text) * CHAR_WIDTH);
      l[ln] = &dbox_entry[i];

      if (lw[ln] > lwt) 
	lwt = lw[ln];
      ln++;

    }
  }

  va_end(ap);

  /* figure out how high and wide the box needs to be */
  ht = ((ln * (CHAR_HEIGHT + DB_V_SPACE)) + BUTTON_HEIGHT + DB_V_SPACE);

  if ((bwt + (bn * BUTTON_MIN_SPACING)) > (lwt + LINE_MIN_SPACING))
    wt = (bwt + (bn * BUTTON_MIN_SPACING));
  else
    wt = (lwt + LINE_MIN_SPACING);
      

  /* Determine button spacing */
  bs = (wt - bwt) / bn;
  bse = ((wt - bwt) % bn) / 2;  /* This gets added to begin and end of line */


  /* Figure out window size */

  dialog_window.w = (wt + BORDER_SIZE*2);
  dialog_window.h = (ht + BORDER_SIZE*2);
  
  text_window.w = wt;
  text_window.h = ht;

  refresh_dialog_box();
}

void refresh_dialog_box(void)
{
  int li, bi;  /* Line, Button incrementors */

  /* Determine screen position */
  dialog_window.x = (scr.client_w / 2) - (dialog_window.w / 2);
  dialog_window.y = (scr.client_h / 2) - (dialog_window.h / 2);

  text_window.x = dialog_window.x + BORDER_SIZE;
  text_window.y = dialog_window.y + BORDER_SIZE;

  hide_mouse();

  /* Draw the border, and fill the background */
  draw_bezel(dialog_window,BORDER_SIZE,color);

  Fgl_fillbox(text_window.x,text_window.y,text_window.w,text_window.h,color);

#ifdef USE_EXPANDED_FONT
    gl_setwritemode (WRITEMODE_MASKED | FONT_EXPANDED);
#else
    Fgl_setfontcolors (color, TEXT_FG_COLOUR);
#endif

    /* Loop calculating line position, and drawing the line */
  for (li = 0; li < ln; li++)
    {
      lx[li] = ((wt - lw[li]) / 2);
      ly[li] = (li * (CHAR_HEIGHT + DB_V_SPACE));
      Fgl_write(lx[li] + text_window.x, ly[li] + text_window.y, l[li]->text);
    }

  /* Loop, Figure out button position including button spacing, then draw the
     button */
  for (bi = 0; bi < bn; bi++)
    {
      if (bi == 0)
	bx[bi] = (bs + bse) / 2;
      else
	bx[bi] = (bx[bi-1] + bw[bi-1] + bs);

      by[bi] = (ln * (CHAR_HEIGHT + DB_V_SPACE) + DB_V_SPACE);
      Fgl_fillbox(bx[bi] + text_window.x - BUTTON_BORDER,
		  by[bi] + text_window.y - BUTTON_BORDER,
		  bw[bi] + (BUTTON_BORDER * 2),
		  CHAR_HEIGHT + (BUTTON_BORDER * 2),
		  white(0));
      Fgl_write(bx[bi] + text_window.x, by[bi] + text_window.y, b[bi]->text);
    }

  redraw_mouse();

}



