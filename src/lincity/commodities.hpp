/* ---------------------------------------------------------------------- *
 * src/lincity/commodities.hpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2022-2024 David Bears <dbear4q@gmail.com>
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

#ifndef __LINCITY_COMMODITIES_HPP__
#define __LINCITY_COMMODITIES_HPP__

enum Commodity : int {
  STUFF_INIT = 0,
  STUFF_FOOD = STUFF_INIT,
  STUFF_LABOR,
  STUFF_COAL,
  STUFF_GOODS,
  STUFF_ORE,
  STUFF_STEEL,
  STUFF_WASTE,
  STUFF_LOVOLT,
  STUFF_HIVOLT,
  STUFF_WATER,
  STUFF_COUNT
};
Commodity& operator++(Commodity& stuff);
Commodity operator++(Commodity& stuff, int);

extern const char *commodityNames[]; // deprecated: use commodityName

extern const char *commodityName(Commodity stuff); // localized for UI
extern const char *commodityStandardName(Commodity stuff); // standardized for load/save
/**
 * Used in load/save logic to get the commodity from the standard name or a past
 * standard name. Returns `STUFF_COUNT` if the name is not recognized.
**/
extern Commodity commodityFromStandardName(const char *name);

struct CommodityRule {
  int maxload;
  bool take;
  bool give;
};

#endif // __LINCITY_COMMODITIES_HPP__
