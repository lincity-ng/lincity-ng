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


/* ---------------------------------------------------------------------- *
 * Private Global Variables
 * ---------------------------------------------------------------------- */
#if defined (WIN32)
int usleep_counter = 0;
#else
struct timeval lc_timeval;
int real_start_time;
#endif


/* ---------------------------------------------------------------------- *
 * Public Global Variables
 * ---------------------------------------------------------------------- */
long real_time = 0;    /* In milliseconds */


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
