/* ---------------------------------------------------------------------- *
 * src/lincity/modules/tile.hpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 * Copyright (C) 2025      David Bears <dbear4q@gmail.com>
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

#ifndef __LINCITYNG_LINCITY_MODULES_TILE_HPP__
#define __LINCITYNG_LINCITY_MODULES_TILE_HPP__

#include <string>                // for string

#include "lincity/lintypes.hpp"  // for ConstructionGroup

/*
* TileConstructionGroups hold information about inactive tiles
* in city. They cannot create any constructions and should not be found
* in ConstructionGroup::groupmap. They also hold the sounds and graphics
* for the tiles, and are acessed via a switch of maptile.group
*/

class TileConstructionGroup: public ConstructionGroup {
public:
  TileConstructionGroup(
      const std::string& name,
      const std::string& name_plural,
      unsigned short no_credit,
      unsigned short group,
      unsigned short size, int colour,
      int cost_mul, int bul_cost, int fire_chance,
      int cost, int tech, int range
  ): ConstructionGroup(
      name, name_plural, no_credit, group, size, colour, cost_mul, bul_cost,
      fire_chance,   cost, tech, range
  ) { };
  // dont use that one tiles are no constructions
  virtual Construction *createConstruction(World& world);
};

extern TileConstructionGroup waterConstructionGroup;
extern TileConstructionGroup bareConstructionGroup;
extern TileConstructionGroup desertConstructionGroup;
extern TileConstructionGroup treeConstructionGroup;
extern TileConstructionGroup tree2ConstructionGroup;
extern TileConstructionGroup tree3ConstructionGroup;

#endif // __LINCITYNG_LINCITY_MODULES_TILE_HPP__
