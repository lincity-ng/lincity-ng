/* ---------------------------------------------------------------------- *
 * dialbox.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * Portions copyright (c) Corey Keasling 2001.
 * ---------------------------------------------------------------------- */

#ifndef __dialbox_h__
#define __dialbox_h__

/* External argument typing constants */
#define DB_PARA 0 /* Paragraph/line arguments; non-button */

struct dialog_box_struct
{
    short type; /* 0 if line/paragraph,  button return value (!0) if button */
    short retval; /* 0 for no return, character (keyboard hotkey) for others */
    char * text;
};

typedef struct dialog_box_struct Dialog_Box;

/* Public functions */
int dialog_box(int colour, char argc, ...); 
void dialog_refresh();
void dialog_close(int return_value);

#define MAX_DBOX_ENTRIES 64 /* Huge, but necessary for current use */

#define LINE_MIN_SPACING 10 /* Extra space on both sides of Line items */

#define CHAR_HEIGHT 8 
#define CHAR_WIDTH 8
#define BUTTON_BORDER 2
#define BUTTON_HEIGHT CHAR_HEIGHT + BUTTON_BORDER
#define BUTTON_MIN_SPACING 8

#define BORDER_SIZE 7
#define DB_V_SPACE 2 /* Extra space above and below text */



#endif
