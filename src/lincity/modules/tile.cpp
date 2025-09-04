/* ---------------------------------------------------------------------- *
 * src/lincity/modules/tile.cpp
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

#include "tile.hpp"

#include <cassert>                  // for assert

#include "lincity/all_buildings.hpp"  // for GROUP_BARE_BUL_COST, GROUP_BARE_C...
#include "lincity/groups.hpp"         // for GROUP_BARE, GROUP_DESERT, GROUP_TREE
#include "util/gettextutil.hpp"

TileConstructionGroup waterConstructionGroup(
  N_("Water"),
  N_("Water"),
  false,                     //need credit
  GROUP_WATER,
  1,                         //tilesize is always 1
  GROUP_WATER_COLOUR,
  GROUP_WATER_COST_MUL,
  GROUP_WATER_BUL_COST,
  GROUP_WATER_FIREC,
  GROUP_WATER_COST,
  GROUP_WATER_TECH,
  0                         //range tiles is 0
);

TileConstructionGroup bareConstructionGroup(
  N_("Empty"),
  N_("Empty"),
  false,                     //need credit
  GROUP_BARE,
  1,                         //tilesize is always 1
  GROUP_BARE_COLOUR,
  GROUP_BARE_COST_MUL,
  GROUP_BARE_BUL_COST,
  GROUP_BARE_FIREC,
  GROUP_BARE_COST,
  GROUP_BARE_TECH,
  0                         //range tiles is 0
);

TileConstructionGroup desertConstructionGroup(
  N_("Desert"),
  N_("Desert"),
  false,                     //need credit
  GROUP_DESERT,
  1,                         //tilesize is always 1
  GROUP_DESERT_COLOUR,
  GROUP_DESERT_COST_MUL,
  GROUP_DESERT_BUL_COST,
  GROUP_DESERT_FIREC,
  GROUP_DESERT_COST,
  GROUP_DESERT_TECH,
  0                         //range tiles is 0
);


TileConstructionGroup treeConstructionGroup(
  N_("Tree"),
  N_("Trees"),
  false,                     //need credit
  GROUP_TREE,
  1,                         //tilesize is always 1
  GROUP_TREE_COLOUR,
  GROUP_TREE_COST_MUL,
  GROUP_TREE_BUL_COST,
  GROUP_TREE_FIREC,
  GROUP_TREE_COST,
  GROUP_TREE_TECH,
  0                         //range tiles is 0
);

TileConstructionGroup tree2ConstructionGroup(
  N_("Trees"),
  N_("Trees"),
  false,                     //need credit
  GROUP_TREE2,
  1,                         //tilesize is always 1
  GROUP_TREE2_COLOUR,
  GROUP_TREE2_COST_MUL,
  GROUP_TREE2_BUL_COST,
  GROUP_TREE2_FIREC,
  GROUP_TREE2_COST,
  GROUP_TREE2_TECH,
  0                         //range tiles is 0
);

TileConstructionGroup tree3ConstructionGroup(
  N_("Forest"),
  N_("Forests"),
  false,                     //need credit
  GROUP_TREE3,
  1,                         //tilesize is always 1
  GROUP_TREE3_COLOUR,
  GROUP_TREE3_COST_MUL,
  GROUP_TREE3_BUL_COST,
  GROUP_TREE3_FIREC,
  GROUP_TREE3_COST,
  GROUP_TREE3_TECH,
  0                         //range tiles is 0
);

//Dont use this one after all maptiles are no constructions
Construction *TileConstructionGroup::createConstruction(World& world) {
  assert(false);
  return nullptr;
}
