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

static Dialog_Box db_entry[MAX_DBOX_ENTRIES];
static Dialog_Box * button[MAX_DBOX_ENTRIES];
static Dialog_Box * line[MAX_DBOX_ENTRIES];

static Rect dialog_window;   /* Describes position of window on screen */
static Rect text_window;     /* Describes position of text area on screen */

static Rect db_rect[MAX_DBOX_ENTRIES];    /* region of each line/button */
// static Rect button_rect[MAX_DBOX_ENTRIES];  /* click area for buttons, lines */
// static Rect line_rect[MAX_DBOX_ENTRIES];    /* offset from text_window */

static Mouse_Handle * main_handle;
static Mouse_Handle * text_handle;

static int dbn; /* number of dbox entries */
static int bn;  /* number of buttons */
static int ln;  /* number of lines */

static int db_longest_button; /* total width of all buttons, pixels */
static int db_longest_line;   /* pixel width of longest line */

static int bs; /* button spacing */
static int bse; /* extra spacing, to be added at beginning and end of line */

static int color;

static short db_up = 0;
static int db_return_value;

char * db_screen_buffer; /* hold the screen we overwrite */
char db_screen_fresh;    /* does the buffer hold information? */

/* Mouse handling routines: main_handler() and text_handler()
   main_handler handles the main dialog window: the text area and border.  
   Clicks in the border are useless and ignored; text_handler takes the
   interesting ones
*/
void 
main_handler(int x, int y, int button) 
{
    
}

void
text_handler(int x, int y, int button)
{
    int i;
    for (i = 0; i < dbn; i++) {
	if (mouse_in_rect(&db_rect[i], x, y) && db_entry[i].retval)
	    dialog_close(db_entry[i].retval);
    }


}

/* Keypress handler: dialog_key_handler()
   Iterate through possible hotkeys, returning if key matches.
*/

void
dialog_key_handler (int key) 
{
    int i;

    if (key == 0) 
	return;

    /* CR, LF, and space all activate default button, type 2 */

    if (key == 10 || key == 13 || key == 32) 
	for (i = 0; i < dbn; i++) 
	    if (db_entry[i].type == 2) {
		dialog_close(db_entry[i].retval);
		return;
	    }
    else
	for (i = 0; i < dbn;  i++) 
	    if (key == db_entry[i].retval) {
		dialog_close(db_entry[i].retval);
		return;
	    }
}
	    

int
dialog_box(int arg_color, char argc, ...)
{
  va_list ap;
  int i;
  int db_last_button = -1;
  int key;
  char * working_str;

  /* Try the locks */
  if (db_up) {
      printf("Already have a dialog box on screen!\n");
      return;
  } else {
      db_up = 1; /* XXX: Need to reconcile these - don't need both flags */
      db_flag = 1;
  }

  bn = 0; ln = 0; dbn = 0;
  db_longest_button = 0; db_longest_line = 0;
  bs = 0; bse = 0;
  color = arg_color;
  db_screen_fresh = 0;

  va_start(ap, argc);

  /* For each argument pair, get the arguments, determine line or button,
     calculate width/length, adjust total size accordingly, increment type
     count. */
  
  for (i = 0; i < argc; i++) {

      if (dbn >= MAX_DBOX_ENTRIES) {
	  fprintf(stderr,"Too many buttons in dialog_box!\n"
		  "Tweak MAX_DBOX_ENTRIES\n");
	  exit(212);
      }  

    db_entry[dbn].type = (short) va_arg(ap, int);
    db_entry[dbn].retval = (short) va_arg(ap, int);

    if (db_entry[dbn].type == 0) { /* Text strings: Chop a paragraph into
				    individual lines.*/
	char * newline;
	working_str = va_arg(ap, char *);
	do {
	    newline = (char *)strchr(working_str,'\n');
	    if (newline) {
		int linelen = newline - working_str;
		db_entry[dbn].text = (char *)lcalloc(1 + linelen);
		strncpy(db_entry[dbn].text,working_str,linelen);
		db_entry[dbn].text[linelen] = '\0';
		working_str = (newline + 1) != '\0' ? newline + 1 : NULL;
	    } else {
		db_entry[dbn].text = (char *)lcalloc(1 + strlen(working_str));
		strncpy(db_entry[dbn].text,working_str,strlen(working_str));
		db_entry[dbn].text[strlen(working_str)] = '\0';
		working_str = NULL;
	    }

	    db_entry[dbn].type = 0;
	    db_entry[dbn].retval = 0;

	    db_rect[dbn].w = (strlen(db_entry[dbn].text) * CHAR_WIDTH);
	    db_rect[dbn].h = CHAR_HEIGHT;
	    if (db_rect[dbn].w > db_longest_line) 
		db_longest_line = db_rect[dbn].w;

	    ln++;
	    dbn++;
	} while ((working_str != NULL) && (strlen(working_str) >= 1));
    } else { 
	db_entry[dbn].text = va_arg(ap, char *);
	db_rect[dbn].w = ((strlen(db_entry[dbn].text) * CHAR_WIDTH)
			  + (BUTTON_BORDER * 2));
	db_rect[dbn].h = (CHAR_HEIGHT + (BUTTON_BORDER * 2));
	
	db_longest_button += db_rect[dbn].w;
	bn++;
	dbn++;
    }
  }

  va_end(ap);

  /* figure out how high and wide the box needs to be */
  text_window.h = 
      ((ln * (CHAR_HEIGHT + DB_V_SPACE)) + BUTTON_HEIGHT + DB_V_SPACE);

  if ((db_longest_button + (bn * BUTTON_MIN_SPACING)) > 
      (db_longest_line + LINE_MIN_SPACING)) {
      text_window.w = (db_longest_button + (bn * BUTTON_MIN_SPACING));
  } else {
      text_window.w = (db_longest_line + LINE_MIN_SPACING);
  }   

  /* Determine button spacing;
     add some extra in front and back */

  bs = (text_window.w - db_longest_button) / bn;
  bse = ((text_window.w - db_longest_line) % bn) / 2;  

  /* Position the buttons and lines */

  for (i = 0; i < dbn; i++)
  {
      if (db_entry[i].type) {                                  /* Buttons */
	  if (db_last_button == -1)
	      db_rect[i].x = ((bs + bse) / 2) - BUTTON_BORDER;
	  else
	      db_rect[i].x = ((db_rect[db_last_button].x  
			       + db_rect[db_last_button].w + bs)
			      - BUTTON_BORDER);
	  
	  db_rect[i].y = ((ln * (CHAR_HEIGHT + DB_V_SPACE) + DB_V_SPACE) 
			  - BUTTON_BORDER);
	  
	  db_last_button = i;
      } else {                                                   /* Lines */
	db_rect[i].x = ((text_window.w - db_rect[i].w) / 2);
	db_rect[i].y = (i * (CHAR_HEIGHT + DB_V_SPACE));
      }
  }
	  
  /* Figure out window size */

  dialog_window.w = (text_window.w + BORDER_SIZE*2);
  dialog_window.h = (text_window.h + BORDER_SIZE*2);
  
  main_handle = mouse_register(&scr.client_win,&main_handler);
  text_handle = mouse_register(&text_window,&text_handler);

  dialog_refresh();

  db_return_value = 0;

  /* Wait for the user to click on it or press an appropriate key */
  /* Mouse clicks arrive from the mouse handler and set db_return_value */

  while (!db_return_value)  {
#ifndef LC_X11
      lc_usleep (1000); /* call_wait_event does this for X11 */
#endif
      
#ifdef LC_X11
      call_wait_event ();
      key = x_key_value;
      x_key_value = 0;
      if (key == 0) 
	  continue;
#elif defined (WIN32)
      HandleMouse ();
      key = GetKeystroke ();
#else
      mouse_update ();
      key = vga_getkey ();
#endif
      if (key == 10 || key == 13 || key == ' ') /* default button */
	  for (i = 0; i <= dbn; i++) {
	      if (db_entry[i].type == 2) {
		  dialog_close(db_entry[i].retval);
		  break;
	      }
	  }
      else
	  for (i = 0; i <= dbn; i++) {
	      if (key == db_entry[i].retval) {
		  dialog_close(key);
		  break;
	      }
	  }
  }
  printf("returning %d\n",db_return_value);
  return (db_return_value);
}


void 
dialog_refresh(void)
{
  int i;  /* Line, Button incrementors */
  if (!db_up) 
      return;

  /* Determine screen position */
  dialog_window.x = (scr.client_w / 2) - (dialog_window.w / 2);
  dialog_window.y = (scr.client_h / 2) - (dialog_window.h / 2);

  text_window.x = dialog_window.x + BORDER_SIZE;
  text_window.y = dialog_window.y + BORDER_SIZE;

  unrequest_mappoint_stats();
  unrequest_main_screen();
  
  hide_mouse();

  if (screen_refreshing && db_screen_fresh) {
      free(db_screen_buffer);
      db_screen_fresh = 0;
  }

  if (!db_screen_fresh) {
      db_screen_buffer = (char *)lcalloc(dialog_window.w * dialog_window.h);
      Fgl_getrect(&dialog_window,db_screen_buffer);
      db_screen_fresh = 1;
  };


  /* Draw the border, and fill the background */
  draw_bezel(dialog_window,BORDER_SIZE,color);

  Fgl_fillbox(text_window.x,text_window.y,text_window.w,text_window.h,color);

#ifdef USE_EXPANDED_FONT
    gl_setwritemode (WRITEMODE_MASKED | FONT_EXPANDED);
#else
    Fgl_setfontcolors (color, TEXT_FG_COLOUR);
#endif

    /* Loop calculating line position, and drawing the line */
    for (i = 0; i < dbn; i++)
    {

	if (db_entry[i].type) {
	    Fgl_fillbox(db_rect[i].x + text_window.x,
			db_rect[i].y + text_window.y,
			db_rect[i].w,
			db_rect[i].h,
			white(0));
	}
	Fgl_write(db_rect[i].x + text_window.x + BUTTON_BORDER, 
		  db_rect[i].y + text_window.y + BUTTON_BORDER,
		  db_entry[i].text);
    }

  redraw_mouse();

}

/* dialog_close: close the mouse handle and remember we closed it;
   save the results; put the old screen back up and remember that too. */

void
dialog_close(int return_value) 
{
    int i;
    
    mouse_unregister(main_handle);
    mouse_unregister(text_handle);
    db_up = 0;
    db_return_value = return_value;

    for (i = 0; i < dbn; i++) 
	if (db_entry[i].type == DB_PARA) 
	    free(db_entry[i].text);
    
    if (db_screen_fresh) {
	Fgl_putrect(&dialog_window,db_screen_buffer);
	free(db_screen_buffer);
	db_screen_fresh = 0;
    }

    db_flag = 0;
}

