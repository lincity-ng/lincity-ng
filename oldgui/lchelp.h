/* ---------------------------------------------------------------------- *
 * help.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __lchelp_h__
#define __lchelp_h__

#include "cliglobs.h"

#define MAX_HELP_LINE     256  /* Max size of a line of a help file (+1) */

void do_help_mouse (int, int, int);
void do_prefs_mouse (int, int, int);
void refresh_help_page (void);
extern void activate_help (char *);
extern void draw_help_page (char *);
extern void parse_tcolourline (char *);
extern void parse_helpline (char *);
extern void parse_textline (char *);
extern void parse_iconline (char *);
extern void draw_help_icon (int, int, char *);
extern void parse_buttonline (char *);
extern void do_help_buttons (int, int);
extern void parse_tbuttonline (char *);

#endif	/* __lchelp_h__ */
