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
#include "mouse.h"

static Dialog_Box dbox_entry[MAX_DBOX_ENTRIES];
static Dialog_Box * button[MAX_DBOX_ENTRIES];
static Dialog_Box * line[MAX_DBOX_ENTRIES];

static int dbn; /* number of dbox entries */
static int bn;  /* number of buttons */
static int ln;  /* number of lines */

static int bwt;                   /* total width of all buttons, pixels */
static int lwt;                   /* pixel width of longest line */

static Rect button_rect[MAX_DBOX_ENTRIES];
static Rect line_rect[MAX_DBOX_ENTRIES];

static int bs; /* button spacing */
static int bse; /* extra spacing, to be added at beginning and end of line */

static int wt; /* Total width of text area, including spacing */
static int ht; /* Total height of text area, including spacing */

static int color;

static Rect dialog_window;   /* Describes position of window on screen */
static Rect text_window;     /* Describes position of text area on screen */

static short dialog_box_up = 0;

static Mouse_Handle * border_handle;
static Mouse_Handle * text_handle;

void 
border_handler(int x, int y, int button) 
{
//    printf("got it in border_handler!\n");
}


void
text_handler(int x, int y, int button)
{
    int i;
    for (i = 0; i < bn; i++) {
	if (mouse_in_rect(&button_rect[i], x, y)) {
	    printf("got it in text_handler!\t\t button = %d\n",i);
	    mouse_unregister(border_handle);
	    mouse_unregister(text_handle);
	    dialog_box_up = 0;
	}
    }


}

int
dialog_box(int arg_color, char argc, ...)
{
  va_list ap;
  int i;

  if (dialog_box_up) {
      printf("Already have a dialog box on screen!\n");
      return;
  } else
      dialog_box_up = 1;


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
      button[bn] = &dbox_entry[i];
      button_rect[bn].w = (strlen(dbox_entry[i].text) * CHAR_WIDTH);
      button_rect[bn].h = CHAR_HEIGHT;
      bwt += button_rect[bn].w;
      bn++;
    } else {
      line_rect[ln].w = (strlen(dbox_entry[i].text) * CHAR_WIDTH);
      line_rect[ln].h = CHAR_HEIGHT;
      line[ln] = &dbox_entry[i];

      if (line_rect[ln].w > lwt) 
	lwt = line_rect[ln].w;
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

  border_handle = mouse_register(&dialog_window,&border_handler);
  text_handle = mouse_register(&text_window,&text_handler);

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
      line_rect[li].x = ((wt - line_rect[li].w) / 2);
      line_rect[li].y = (li * (CHAR_HEIGHT + DB_V_SPACE));
      Fgl_write(line_rect[li].x + text_window.x, 
		line_rect[li].y + text_window.y, line[li]->text);
    }

  /* Loop, Figure out button position including button spacing, then draw the
     button */
  for (bi = 0; bi < bn; bi++)
    {
      if (bi == 0)
	button_rect[bi].x = (bs + bse) / 2;
      else
	button_rect[bi].x = (button_rect[bi-1].x + button_rect[bi-1].w + bs);

      button_rect[bi].y = (ln * (CHAR_HEIGHT + DB_V_SPACE) + DB_V_SPACE);
      Fgl_fillbox(button_rect[bi].x + text_window.x - BUTTON_BORDER,
		  button_rect[bi].y + text_window.y - BUTTON_BORDER,
		  button_rect[bi].w + (BUTTON_BORDER * 2),
		  button_rect[bi].h + (BUTTON_BORDER * 2),
		  white(0));
      Fgl_write(button_rect[bi].x + text_window.x, 
		button_rect[bi].y + text_window.y, 
		button[bi]->text);
    }

  redraw_mouse();

}


