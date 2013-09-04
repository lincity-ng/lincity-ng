/* ---------------------------------------------------------------------- *
 * ConstructionCount.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __ConstructionCount_h__
#define __ConstructionCount_h__

#include "lintypes.h"
#include "../lincity-ng/Permutator.hpp"
#include <vector>
#include <algorithm> //for std::sort

class Construction;

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
    void sort(); //Sort all contructions
    Construction* operator[](unsigned int i);
    Construction* pos(unsigned int i);
protected:
    size_t free_slot;
    Permutator * permutator;
    std::vector <Construction*> constructionVector;
    void update_permutator();
    static bool earlier(Construction* a, Construction* b); //first by group and then by ID
};

#endif /* __ConstructionCount_h__ */

/** @file lincity/world.h */


