/* ---------------------------------------------------------------------- *
 * engine.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __engine_h__
#define __engine_h__

int engine_place_item (int x, int y, short type);
int engine_bulldoze_item (int x, int y);
void engine_do_coal_survey (void);
void do_bulldoze_area (short fill, int xx, int yy);
void engine_launch_rocket (int x, int y);

#endif	/* __engine_h__ */
