/* ---------------------------------------------------------------------- *
 * dialbox.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * Portions copyright (c) Corey Keasling 2001.
 * ---------------------------------------------------------------------- */

struct dialog_box_struct
{
  short type; /* 0 if line,  button return value (!0) if button */
  char * text;
};

typedef struct dialog_box_struct Dialog_Box;

/* Public functions */
int dialog_box(int colour, char argc, ...); 
void refresh_dialog_box();

#define MAX_DBOX_ENTRIES 16 /* Small, but entirely adequate for current use */

#define LINE_MIN_SPACING 10 /* Extra space on both sides of Line items */

#define CHAR_HEIGHT 8 
#define CHAR_WIDTH 8
#define BUTTON_BORDER 2
#define BUTTON_HEIGHT CHAR_HEIGHT + BUTTON_BORDER
#define BUTTON_MIN_SPACING 8

#define BORDER_SIZE 7
#define DB_V_SPACE 2 /* Extra space above and below text */



