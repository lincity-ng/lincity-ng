/* ---------------------------------------------------------------------- *
 * substation.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#ifndef __substation_h__
#define __substation_h__

void do_power_substation(int x, int y);
int add_a_substation(int x, int y);
void remove_a_substation(int x, int y);
void shuffle_substations(void);

#endif /* __substation_h__ */
