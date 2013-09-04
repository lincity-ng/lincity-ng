/* ---------------------------------------------------------------------- *
 * ConstructionCount.cpp
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */

#include <stdlib.h>
#include <iostream>
#include "ConstructionCount.h"
#include "lintypes.h"
#include "engine.h"

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
    //    construction->constructionGroup->name << " ID :" << construction->ID << std::endl;
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
        //construction->constructionGroup->name << " ID :" << construction->ID << std::endl;
        constructionVector[free_slot] = NULL;
        //constructionVector.erase(i,i+1);
        //update_permutator();
    }
/*
    //normal event if market or shanty is burning waste
    else
    {
        std::cout << "Could not find Construction in constructionCount: " <<
        construction->constructionGroup->name << " ID :" << construction->ID << std::endl;
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

void
ConstructionCount::sort()
{
/*
   //FIXME The resuluts with this one are quite good, but not really ordered
   std::sort(constructionVector.begin(), constructionVector.end(), earlier);
*/
    //Alternate Solution
    std::map <unsigned short,std::map <int, Construction*> > census;
    for(size_t i = 0; i < constructionVector.size(); ++i)
    {
        if (constructionVector[i])
        {
            Construction *cst = constructionVector[i];
            int ID = cst->ID;
            unsigned short group = cst->constructionGroup->group;
            census[group][ID] = cst;
            constructionVector[i] = 0;
        }
    }
    std::map <unsigned short,std::map <int, Construction*> >::iterator group_it;
    std::map <int, Construction*>::iterator cst_it;
    free_slot = 0;
    for(group_it = census.begin(); group_it != census.end(); ++group_it)
    {   //for every groups
        for(cst_it = group_it->second.begin(); cst_it != group_it->second.end(); ++cst_it)
        {   //for every construction
            constructionVector[free_slot++] = cst_it->second;
        }
    }
}

bool
ConstructionCount::earlier(Construction* a, Construction* b)
{
//sort works until all tests are false
return (a && b)?((a->constructionGroup->group == b->constructionGroup->group)?(a->ID < b->ID):
        (a->constructionGroup->group < b->constructionGroup->group)):false;
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
