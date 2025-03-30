/*
Copyright (C) 2005 David Kamphausen <david.kamphausen@web.de>
Copyright (C) 2024 David Bears <dbear4q@gmail.com>

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

#include <assert.h>                              // for assert
#include <stdio.h>                               // for printf
#include <iomanip>                               // for operator<<, setpreci...
#include <sstream>                               // for basic_ostream, opera...
#include <string>                                // for basic_string, char_t...

#include "Game.hpp"                              // for getGame
#include "MiniMap.hpp"                           // for getMiniMap, MiniMap
#include "Mps.hpp"                               // for Mps, globalMPS, envMPS
#include "gui_interface/mps.h"                   // for MPS_MAP, MPS_ENV
#include "lincity/commodities.hpp"               // for Commodity
#include "lincity/engglobs.h"                    // for world, people_pool
#include "lincity/groups.h"                      // for GROUP_WATER, GROUP_D...
#include "lincity/lclib.h"                       // for num_to_ansi
#include "lincity/lin-city.h"                    // for FLAG_CRICKET_COVER
#include "lincity/lintypes.h"                    // for ConstructionGroup
#include "lincity/modules/modules_interfaces.h"  // for mps_water
#include "lincity/modules/shanty.h"              // for ShantyConstructionGroup
#include "lincity/stats.h"                       // for ltdeaths, ly_coal_tax
#include "lincity/world.h"                       // for Map, MapTile, Ground
#include "tinygettext/gettext.hpp"               // for N_, _

// this is the MPS-status display, which describes the cells

int mps_x = 0;
int mps_y = 0;
int mps_style = 0;
int mps_map_page = 0;

/*
void mps_init()
{
}
*/

/** @file lincity-ng/MpsInterface.cpp */
