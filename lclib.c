/* ---------------------------------------------------------------------- *
 * screen.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * Portions copyright (c) Corey Keasling, 2001.
 * ---------------------------------------------------------------------- */

#include "lcconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include "lcstring.h"
#include "cliglobs.h"
#include "lcintl.h"

char* 
current_month (int current_time)
{
    return _(months[(current_time % NUMOF_DAYS_IN_YEAR) / NUMOF_DAYS_IN_MONTH]);
}

int 
current_year (int current_time)
{
    return current_time / NUMOF_DAYS_IN_YEAR;
}

void 
format_number5 (char* str, int num)
{
    int num_sign = num >= 0 ? 1 : -1;
    if (num_sign == 1) {
	if (num < 99999) {
	    sprintf (str, "%5d", num);
	} else if (num < 9999999) {
	    sprintf (str, "%4dK", num / 1000);
	} else {
	    sprintf (str, "%4dM", num / 1000000);
	}
    } else {
	int num_absval = num_sign * num;
	if (num_absval < 9999) {
	    sprintf (str, "%5d", num);
	} else if (num_absval < 999999) {
	    sprintf (str, "%4dK", num_sign * (num_absval / 1000));
	} else {
	    sprintf (str, "%4dM", num_sign * (num_absval / 1000000));
	}
    }
}

void 
num_to_ansi(char * s, size_t size, long num) 
{
  int triplets = 0;
  float numf = (float)num;  

  while (numf > 1000) {
    numf /= 1000;
    triplets++;
  }

  switch(triplets) 
    {
    case 0: triplets = ' '; break;
    case 1: triplets = 'k'; break; // kila
    case 2: triplets = 'm'; break; // mega
    case 3: triplets = 'g'; break; // giga 
    case 4: triplets = 't'; break; // tera 
    case 5: triplets = 'p'; break; // peta
    default: triplets = '?'; break;
    }
      
  if (size == 4)  /* to make up for format_pos_number4.  Eeewwwwwww. */ 
    if (numf < 10) 
      snprintf(s, size + 1, "%1.1f%c", numf, triplets);
    else 
      snprintf(s,size + 1, "%3.0f%c", numf, triplets);
  else
    snprintf(s, size, "%3.1f%c", numf, triplets);
}

void 
num_to_ansi_unit(char * s, size_t size, long num, char unit) 
{
  int triplets = 0;
  float numf = (float)num;

  while (numf > 1000) {
    numf /= 1000;
    triplets++;
  }

  switch(triplets) 
    {
    case 0: triplets = ' '; break;
    case 1: triplets = 'k'; break; // kila
    case 2: triplets = 'm'; break; // mega
    case 3: triplets = 'g'; break; // giga 
    case 4: triplets = 't'; break; // tera 
    case 5: triplets = 'p'; break; // peta
    default: triplets = '?'; break;
    }
      
  if (size == 4)  /* to make up for format_pos_number4 */
    if (numf < 10) 
      snprintf(s, size, "%4.1f%c%c", numf, triplets, unit);
    else 
      snprintf(s,size, "%4.0f%c%c", numf, triplets, unit);
  else
    snprintf(s, size, "%5.1f%c%c", numf, triplets, unit);
}

void 
format_pos_number4 (char* str, int num)
{
  num_to_ansi(str, 4, num);
}

void
format_money (char* str)
{
    int money_sign = total_money >= 0 ? 1 : -1;
    int money_absval = money_sign * total_money;
    int millions = money_absval / 1000000;
    int ones = money_absval % 1000000;

    if (millions) {
	sprintf (str, "%5d%c%06d", 
		 money_sign * millions, MONEY_SEPARATOR, ones);
    } else {
	sprintf (str, "     %7d", money_sign * ones);
    }
}

void 
format_power(char * str, size_t size, long power)
{
  num_to_ansi_unit(str, size, power, 'w');
}

void * 
lcalloc (size_t size) 
{
  void * tmp;
  tmp = malloc(size);
  if (tmp == NULL) {
    printf("couldn't malloc %d bytes!  Dying.\n",size);
    exit(-1);
  }
  
  return tmp;
}
