/* ---------------------------------------------------------------------- *
 * src/lincity/init_game.hpp
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

#ifndef __init_game_h__
#define __init_game_h__

#include <memory>

#include "world.hpp"

typedef struct _CitySettings city_settings;

struct _CitySettings {
  bool with_village;
  bool without_trees;
};

std::unique_ptr<World>
create_new_city(city_settings *city, int mapSize,
  int old_setup_ground, int climate);

std::unique_ptr<World>
new_city(city_settings *city, int mapSize);

std::unique_ptr<World>
new_desert_city(city_settings *city, int mapSize);

std::unique_ptr<World>
new_temperate_city(city_settings *city, int mapSize
);

std::unique_ptr<World>
new_swamp_city(city_settings *city, int mapSize);

#endif /* __init_game_h__ */

/** @file lincity/init_game.h */
