/* ---------------------------------------------------------------------- *
 * timer.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#if defined (HAVE_CONFIG_H)
#include "config.h"
#elif defined (WIN32)
#include "confw32.h"
#endif

#if defined (TIME_WITH_SYS_TIME)
#include <time.h>
#include <sys/time.h>
#else
#if defined (HAVE_SYS_TIME_H)
#include <sys/time.h>
#else
#include <time.h>
#endif
#endif
#if defined(AIX) || defined(__EMX__)
#include <sys/select.h>
#endif

#include "cliglobs.h"
#include "mouse.h"
#include "geometry.h"
#include "lchelp.h"
#include "timer.h"


/* ---------------------------------------------------------------------- *
 * Private Global Variables
 * ---------------------------------------------------------------------- */
#if defined (WIN32)
int usleep_counter = 0;
#else
struct timeval lc_timeval;
int real_start_time;
#endif

Mouse_Handle * pause_handle;
Mouse_Handle * slow_handle;
Mouse_Handle * medium_handle;
Mouse_Handle * fast_handle;

/* ---------------------------------------------------------------------- *
 * Public Global Variables
 * ---------------------------------------------------------------------- */
long real_time = 0;    /* In milliseconds */


/* ---------------------------------------------------------------------- *
 * Private Function Declarations
 * ---------------------------------------------------------------------- */

void pause_handler(int x, int y, int button);
void slow_handler(int x, int y, int button);
void medium_handler(int x, int y, int button);
void fast_handler(int x, int y, int button);

/* ---------------------------------------------------------------------- *
 * Function Definitions
 * ---------------------------------------------------------------------- */
void
lc_usleep (unsigned long t)
{
#if defined (WIN32)
  /* This function will usually sleep too long.  For example, if t == 0 
     (e.g. 1 usec), thread will sleep for the remainder of its timeslice, 
     which might be 20 ms.
   */
  SleepEx (t / 1000, FALSE);
#else
  struct timeval timeout;
  timeout.tv_sec = t / 1000000;
  timeout.tv_usec = t - 1000000 * timeout.tv_sec;
  select (1, NULL, NULL, NULL, &timeout);
#endif
}

void 
reset_start_time (void)
{
#if !defined (WIN32)
  if (gettimeofday (&lc_timeval, 0) != 0)
    do_error ("Can't get timeofday");
  real_start_time = lc_timeval.tv_sec;
#endif
}


void
get_real_time (void)
{
#if defined (WIN32)
  const int CLOCKS_PER_MILLISECOND = CLOCKS_PER_SEC / 1000;
  real_time = (long) (clock () / CLOCKS_PER_MILLISECOND);
#else
  gettimeofday (&lc_timeval, 0);
  real_time = (lc_timeval.tv_sec - real_start_time) * 1000
    + (lc_timeval.tv_usec / 1000);
#endif
}

/* Game speed functions */

void
init_timer_buttons (void)
{
    pause_handle = mouse_register(&scr.pause_button, &pause_handler);
    slow_handle = mouse_register(&scr.slow_button, &slow_handler);
    medium_handle = mouse_register(&scr.med_button, &medium_handler);
    fast_handle = mouse_register(&scr.fast_button, &fast_handler);
}

/* Mouse handlers */

void
pause_handler(int x, int y, int button)
{
    if (button == LC_MOUSE_RIGHTBUTTON) {
	activate_help ("pause.hlp");
    } else {
	select_pause ();
    }
}

void
slow_handler(int x, int y, int button)
{
    if (button == LC_MOUSE_RIGHTBUTTON) {
	activate_help ("slow.hlp");
    } else {
	select_slow ();
    }
}

void
medium_handler(int x, int y, int button)
{
    if (button == LC_MOUSE_RIGHTBUTTON) {
	activate_help ("medium.hlp");
    } else {
	select_medium ();
    }
}

void
fast_handler(int x, int y, int button)
{
    if (button == LC_MOUSE_RIGHTBUTTON) {
	activate_help ("fast.hlp");
    } else {
	select_fast ();
    }
}

void
select_fast (void)
{
    hide_mouse ();
    pause_flag = 0;
    draw_pause (0);
    slow_flag = 0;
    draw_slow (0);
    med_flag = 0;
    draw_med (0);
    fast_flag = 1;
    draw_fast (1);
    redraw_mouse ();
}

void
select_medium (void)
{
    hide_mouse ();
    pause_flag = 0;
    draw_pause (0);
    slow_flag = 0;
    draw_slow (0);
    med_flag = 1;
    draw_med (1);
    fast_flag = 0;
    draw_fast (0);
    redraw_mouse ();
}

void
select_slow (void)
{
    hide_mouse ();
    pause_flag = 0;
    draw_pause (0);
    slow_flag = 1;
    draw_slow (1);
    med_flag = 0;
    draw_med (0);
    fast_flag = 0;
    draw_fast (0);
    redraw_mouse ();
}

void
select_pause (void)
{
    if (pause_flag) {
	/* unpause it */
	if (fast_flag)
	    select_fast ();
	else if (med_flag)
	    select_medium ();
	else if (slow_flag)
	    select_slow ();
	else
	    select_medium ();
    } else {
	/* pause it */
	hide_mouse ();
	pause_flag = 1;
	draw_pause (1);
	draw_slow (0);
	draw_med (0);
	draw_fast (0);
	redraw_mouse ();
    }
}
