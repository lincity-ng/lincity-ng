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
#include "lin-city.h"
#include "tinygettext/gettext.hpp"

char *months[] = { (char*)N_("Jan"), (char*)N_("Feb"), (char*)N_("Mar"), (char*)N_("Apr"),
    (char*)N_("May"), (char*)N_("Jun"), (char*)N_("Jul"), (char*)N_("Aug"),
    (char*)N_("Sep"), (char*)N_("Oct"), (char*)N_("Nov"), (char*)N_("Dec")
};

const char *current_month(int current_time)
{
    static bool doinit = true;
    if (doinit) {
        doinit = false;
        for (int i = 0; i < 12; ++i) {
            months[i] = strdup(_(months[i]));
        }
    }
    return months[(current_time % NUMOF_DAYS_IN_YEAR) / NUMOF_DAYS_IN_MONTH];
}

int current_year(int current_time)
{
    return current_time / NUMOF_DAYS_IN_YEAR;
}
/*
void format_number5(char *str, int num)
{
    int num_sign = num >= 0 ? 1 : -1;
    if (num_sign == 1) {
        if (num < 99999) {
            sprintf(str, "%5d", num);
        } else if (num < 9999999) {
            sprintf(str, "%4dK", num / 1000);
        } else {
            sprintf(str, "%4dM", num / 1000000);
        }
    } else {
        int num_absval = num_sign * num;
        if (num_absval < 9999) {
            sprintf(str, "%5d", num);
        } else if (num_absval < 999999) {
            sprintf(str, "%4dK", num_sign * (num_absval / 1000));
        } else {
            sprintf(str, "%4dM", num_sign * (num_absval / 1000000));
        }
    }
}
*/
void num_to_ansi(char *s, size_t size, long num)
{
    int triplets = 0;
    float numf = (float)num;

    if (numf > 9999 || numf < -9999)
    {
        while (numf > 999 || numf < -999)
        {
            numf /= 1000;
            triplets++;
        }
    }


    switch (triplets) {
    case 0:
        triplets = ' ';
        break;
    case 1:
        triplets = 'k';
        break;                  // kilo
    case 2:
        triplets = 'M';
        break;                  // mega
    case 3:
        triplets = 'G';
        break;                  // giga
    case 4:
        triplets = 'T';
        break;                  // tera
    case 5:
        triplets = 'P';
        break;                  // peta
    default:
        triplets = '?';
        break;
    }

    if (size == 4) {            /* to make up for format_pos_number4.  Eeewwwwwww. */
        if (numf < 10) {
            snprintf(s, size + 1, "%1.1f%c", numf, triplets);
        } else {
            snprintf(s, size + 1, "%3.0f%c", numf, triplets);
        }
    } else {
        if (triplets == ' ') {
            snprintf(s, size, "%4.0f", numf);
        } else {
            snprintf(s, size, "%3.1f%c", numf, triplets);
        }
    }
}
/*
void num_to_ansi_unit(char *s, size_t size, long num, char unit)
{
    int triplets = 0;
    float numf = (float)num;

    while (numf > 1000 || numf < -1000) {
        numf /= 1000;
        triplets++;
    }

    switch (triplets) {
    case 0:
        triplets = ' ';
        break;
    case 1:
        triplets = 'k';
        break;                  // kila
    case 2:
        triplets = 'M';
        break;                  // mega
    case 3:
        triplets = 'G';
        break;                  // giga
    case 4:
        triplets = 'T';
        break;                  // tera
    case 5:
        triplets = 'P';
        break;                  // peta
    default:
        triplets = '?';
        break;
    }

    if (size == 4)              //to make up for format_pos_number4
        if (numf < 10)
            snprintf(s, size, "%-4.1f%c%c", numf, triplets, unit);
        else
            snprintf(s, size, "%-4.0f%c%c", numf, triplets, unit);
    else
        snprintf(s, size, "%-5.1f%c%c", numf, triplets, unit);
}
*/
/* commify: take a number and convert it to a string grouped into triplets
   with commas; returns number of characters written, excluding trailing zero
*/
int commify(char *str, size_t size, int argnum)
{
    size_t count = 0;
    int i = 0;
    int triad = 1;
    int num = argnum;
    int kludge = 1;

    if (num < 0)
        count += snprintf(str, size, "-");

    num = abs(argnum);

    for (; num >= 1000; num /= 1000, triad++, kludge *= 1000) ;

    num = abs(argnum);

    for (; triad > 0; i++, triad--) {

        if (i == 0)
            if (triad == 1)
                count += snprintf(str + count, size - count, "%d", num);
            else
                count += snprintf(str + count, size - count, "%d,", num ? num / kludge : num);
        else if (triad == 1)
            count += snprintf(str + count, size - count, "%03d", num ? num / kludge : num);
        else
            count += snprintf(str + count, size - count, "%03d,", num ? num / kludge : num);

        if (num)                /* don't divide by zero */
            num %= kludge;

        kludge /= 1000;
    }

    return count;
}

/* GCS - make sure that the string has length at least size-1 */
void pad_with_blanks(char *str, int size)
{
    while (*str) {
        size--;
        str++;
    }
    while (size-- > 1) {
        *str++ = ' ';
    }
    *str = '\0';
}
/*
void format_pos_number4(char *str, int num)
{
    num_to_ansi(str, 4, num);
}
*/
/*
void format_power(char *str, size_t size, long power)
{
    // Lincity power unit is 1kW
    num_to_ansi_unit(str, size, power * 1000, 'W');
}
*/
int min_int(int i1, int i2)
{
    return i1 < i2 ? i1 : i2;
}

int max_int(int i1, int i2)
{
    return i1 > i2 ? i1 : i2;
}

void *lcalloc(size_t size)
{
    void *tmp;
    tmp = malloc(size);
    if (tmp == NULL) {
        /* prevent gcc warning on amd64: argument 2 has type 'long unsigned int' !!! */
        printf("couldn't malloc %d bytes!  Dying.\n", (int) size);
        exit(-1);
    }

    return tmp;
}

/** @file lincity/lclib.cpp */

