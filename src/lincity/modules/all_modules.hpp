/* ---------------------------------------------------------------------- *
 * src/lincity/modules/all_modules.hpp
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
#include "blacksmith.hpp"
#include "coal_power.hpp"
#include "coalmine.hpp"
#include "commune.hpp"
#include "cricket.hpp"
#include "fire.hpp"
#include "firestation.hpp"
#include "health_centre.hpp"
#include "heavy_industry.hpp"
#include "light_industry.hpp"
#include "market.hpp"
#include "mill.hpp"
#include "monument.hpp"
#include "oremine.hpp"
#include "organic_farm.hpp"
#include "parkland.hpp"
#include "port.hpp"
#include "pottery.hpp"
#include "power_line.hpp"
//#include "rail.h" //superseeded by track_road_rail_powerline.h
#include "recycle.hpp"
#include "residence.hpp"
//#include "road.h" //superseeded by track_road_rail_powerline.h
#include "rocket_pad.hpp"
#include "school.hpp"
#include "shanty.hpp"
#include "solar_power.hpp"
#include "substation.hpp"
#include "tile.hpp"
#include "tip.hpp"
#include "track_road_rail.hpp"
//#include "track.h" //superseeded by track_road_rail_powerline.h
#include "university.hpp"
#include "waterwell.hpp"
#include "windmill.hpp"
#include "windpower.hpp"
// IWYU pragma: end_exports

void initializeModules();

#endif


/** @file lincity/modules/all_modules.h */
