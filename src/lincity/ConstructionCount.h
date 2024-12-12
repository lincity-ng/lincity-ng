/* ---------------------------------------------------------------------- *
 * src/lincity/ConstructionCount.h
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

#ifndef __ConstructionCount_h__
#define __ConstructionCount_h__

#include <stddef.h>  // for size_t
#include <vector>    // for vector

class Construction;
class Permutator;

class ConstructionCount
{
public:
    ConstructionCount();
    ~ConstructionCount();
    void add_construction(Construction * construction);
    void remove_construction(Construction * construction);
    void shuffle(); //suffle the permutator
    int size();     //return the current size constructionVector NOT the number of Constructions
    int count();    //return the current number of constructions
    void size (int new_len); //set the new size of constructionVector
    void reset(); //FIXME Only for use in debugging
    Construction* operator[](unsigned int i);
    Construction* pos(unsigned int i);
protected:
    size_t free_slot;
    Permutator * permutator;
    std::vector <Construction*> constructionVector;
    void update_permutator();
};

#endif /* __ConstructionCount_h__ */

/** @file lincity/world.h */
