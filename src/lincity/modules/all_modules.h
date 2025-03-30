/* ---------------------------------------------------------------------- *
 * src/lincity/modules/all_modules.h
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#ifndef ALL_MODULES_H
#define ALL_MODULES_H

// IWYU pragma: begin_exports
#include "blacksmith.h"
#include "coal_power.h"
#include "coalmine.h"
#include "commune.h"
#include "cricket.h"
#include "fire.h"
#include "firestation.h"
#include "health_centre.h"
#include "heavy_industry.h"
#include "light_industry.h"
#include "market.h"
#include "mill.h"
#include "monument.h"
#include "oremine.h"
#include "organic_farm.h"
#include "parkland.h"
#include "port.h"
#include "pottery.h"
#include "power_line.h"
//#include "rail.h" //superseeded by track_road_rail_powerline.h
#include "recycle.h"
#include "residence.h"
//#include "road.h" //superseeded by track_road_rail_powerline.h
#include "rocket_pad.h"
#include "school.h"
#include "shanty.h"
#include "solar_power.h"
#include "substation.h"
#include "tile.hpp"
#include "tip.h"
#include "track_road_rail.h"
//#include "track.h" //superseeded by track_road_rail_powerline.h
#include "university.h"
#include "waterwell.h"
#include "windmill.h"
#include "windpower.h"
// IWYU pragma: end_exports

void initializeModules();

#endif


/** @file lincity/modules/all_modules.h */
