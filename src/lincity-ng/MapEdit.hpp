/*
Copyright (C) 2005 David Kamphausen <david.kamphausen@web.de>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef __MAP_EDIT_HPP__
#define __MAP_EDIT_HPP__

#include "MapPoint.hpp"

void editMap(MapPoint point, int button);
void check_bulldoze_area (int x, int y);
void resetLastMessage();

extern int monument_bul_flag;
extern int river_bul_flag;
extern int shanty_bul_flag;
extern int build_bridge_flag;

#endif

/** @file lincity-ng/MapEdit.hpp */

