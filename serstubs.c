/* ---------------------------------------------------------------------- *
 * serstubs.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include <stdlib.h>
#include "clinet.h"
#include "lctypes.h"
#include "engine.h"

void
HandleError (char *description, int degree)
{
  printf ("Error: %s\n", description);
  if (degree == FATAL)
    {
      exit (-1);
    }
}

void 
update_main_screen (void)
{
}

void 
refresh_main_screen (void)
{
}

int 
yn_dial_box (char *title, char *s1, char *s2, char *s3)
{
    printf ("YN-DIAL-BOX: %s\n%s\n%s\n",s1,s2,s3);
    return 0;   /* Default to "no" */
}

void
init_pbars ()
{
}

void
prog_box (char* msg, int percent)
{
    printf (msg);
    printf (".");
    if (percent == 100) {
	printf ("\n");
    }
    fflush (stdout);
}

void 
ok_dial_box (char *fn, int good_bad, char *xs)
{
    printf ("OK-DIAL-BOX: %s\n",fn);
}

void
fire_area (int x, int y)
{
  do_bulldoze_area (CST_FIRE_1, x, y);
}

void
do_error (char *s)
{
  printf ("%s\n", s);
  exit (1);
}

void
print_cheat (void)
{
}

void
unprint_cheat (void)
{
}

void
draw_sustainable_window (void)
{
}

void
highlight_select_button (int button)
{
}

void
unhighlight_select_button (int button)
{
}

void
print_total_money (void)
{
}

int 
ask_launch_rocket_now (int x, int y)
{
    return 0;  /* Don't launch immediately */
}
