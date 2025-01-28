/* ---------------------------------------------------------------------- *
 * src/lincity/ConstructionCount.cpp
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

#include "ConstructionCount.h"
#include "engglobs.h"                 // for world
#include "lincity-ng/Permutator.hpp"  // for Permutator
#include "lintypes.h"                 // for Construction
#include "world.h"                    // for Map

ConstructionCount::ConstructionCount()
{
    constructionVector.resize(100, NULL);
    permutator = new Permutator(100,1);
    free_slot = 0;
}

ConstructionCount:: ~ConstructionCount()
{
    delete permutator;
    permutator = NULL;
}

void
ConstructionCount::add_construction(Construction * construction)
{

    while(free_slot < constructionVector.size() && constructionVector[free_slot] ) {free_slot++;}
    if (free_slot == constructionVector.size())
    {
        constructionVector.resize(constructionVector.size()*3/2, NULL);
        update_permutator();
        //std::cout << "growing constructionCount " << constructionVector.size() << std::endl;
    }
    constructionVector[free_slot++] = construction;
    world.dirty = true;
    //std::cout << "Added Construction to constructionCount: " <<
    //    construction->constructionGroup->name << std::endl;
}

void
ConstructionCount::remove_construction(Construction * construction)
{
    //std::vector <Construction*>::iterator i;
    //i = constructionVector.begin();
    //while(i != constructionVector.end() && constructionVector.at(i) != construction) {i++;}
    free_slot = 0;
    while(free_slot < constructionVector.size() && constructionVector[free_slot] != construction) {free_slot++;}
    if (constructionVector[free_slot] == construction)
    {
        //std::cout << "Nullified Construction in constructionCount: " <<
        //construction->constructionGroup->name << std::endl;
        constructionVector[free_slot] = NULL;
        //constructionVector.erase(i,i+1);
        //update_permutator();
    }
/*
    //normal event if market or shanty is burning waste
    else
    {
        std::cout << "Could not find Construction in constructionCount: " <<
        construction->constructionGroup->name << std::endl;
        //assert(false);
    }
*/
    free_slot = 0;
}

void
ConstructionCount::shuffle()
{
    permutator->shuffle();
}

Construction*
ConstructionCount::operator[](unsigned int i)
{
    return constructionVector[permutator->getIndex(i)];
}

Construction*
ConstructionCount::pos(unsigned int i)
{
    return constructionVector[i];
}

void
ConstructionCount::update_permutator()
{
    if (permutator)
    {
        delete permutator;
    }
    permutator = new Permutator(constructionVector.size(),1);
}

int
ConstructionCount::size()
{
    return constructionVector.size();
}

void
ConstructionCount::size(int new_len)
{
    constructionVector.resize(new_len, NULL);
    update_permutator();
}

int
ConstructionCount::count()
{
    int n = 0;
    for(size_t i = 0; i < constructionVector.size(); ++i)
    {
        if (constructionVector[i])
        {   ++n;}
    }
    return n;
}

//FIXME Only for use in debugging
void
ConstructionCount::reset()
{
    constructionVector.clear();
    constructionVector.resize(100, NULL);
    update_permutator();
    free_slot = 0;
}
/** @file lincity/ConstructionCount.cpp */
