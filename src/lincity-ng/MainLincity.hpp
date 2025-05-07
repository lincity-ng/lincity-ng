/* ---------------------------------------------------------------------- *
 * src/lincity-ng/MainLincity.hpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2005      David Kamphausen <david.kamphausen@web.de>
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

#ifndef __main_lincity_hpp__
#define __main_lincity_hpp__

#include <filesystem>  // for path
#include <memory>

class World;

extern int simDelay;

void initLincity();
void doLincityStep();

/**
 * Sets the number of milliseconds to wait between simulation steps.
**/
void setSimulationDelay( int speed );

//get Data form Lincity NG and Save City
void saveCityNG(const World& world, const std::filesystem::path& newFilename);

//Load City and do setup for Lincity NG.
std::unique_ptr<World> loadCityNG(const std::filesystem::path& filename);

#endif

/** @file lincity-ng/MainLincity.hpp */
