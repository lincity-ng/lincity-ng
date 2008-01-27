/* ---------------------------------------------------------------------- *
 * transport.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */

#ifndef __TRANSPORT_H__
#define __TRANSPORT_H__

#include "engglobs.h"

#define XY_IS_TRANSPORT(x,y) \
((MP_GROUP(x,y) == GROUP_TRACK) || \
 (MP_GROUP(x,y) == GROUP_ROAD) || \
 (MP_GROUP(x,y) == GROUP_RAIL))
#endif

void general_transport(int x, int y, int max_waste);
void connect_transport(int originx, int originy, int w, int h);
