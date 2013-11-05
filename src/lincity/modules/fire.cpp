/* ---------------------------------------------------------------------- *
 * fire.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */


#include "fire.h"
#include "lincity-ng/Sound.hpp"

FireConstructionGroup fireConstructionGroup(
    "Fire",
     FALSE,                     /* need credit? */
     GROUP_FIRE,
     GROUP_FIRE_SIZE,
     GROUP_FIRE_COLOUR,
     GROUP_FIRE_COST_MUL,
     GROUP_FIRE_BUL_COST,
     GROUP_FIRE_FIREC,
     GROUP_FIRE_COST,
     GROUP_FIRE_TECH,
     GROUP_FIRE_RANGE
);

//helper groups for graphics and sound sets, dont add them to ConstructionGroup::groupMap
FireConstructionGroup fireWasteLandConstructionGroup = fireConstructionGroup;

Construction *FireConstructionGroup::createConstruction(int x, int y) {
    return new Fire(x, y, this);
}

void Fire::update()
{
    int i;
    /* this so we don't get whole blocks changing in one go. */
    if (burning_days == 0)
    {   burning_days = rand() % (FIRE_LENGTH / 5);}

    if (burning_days > FIRE_LENGTH)
    {
        //is_burning = false;
        if (smoking_days == 0)   /* rand length here also */
        {   smoking_days = rand() % (AFTER_FIRE_LENGTH / 6);}
        if(constructionGroup == &fireConstructionGroup)
        {   constructionGroup = &fireWasteLandConstructionGroup;}
        smoking_days++;
        if (world(x,y)->flags & FLAG_FIRE_COVER)
            smoking_days += 4;
        if (smoking_days > AFTER_FIRE_LENGTH)
            ConstructionManager::submitRequest
                (
                    new ConstructionDeletionRequest(this)
                );
        else if (smoking_days > (3 * AFTER_FIRE_LENGTH) / 4)
            type = 3;
        else if (smoking_days > (2 * AFTER_FIRE_LENGTH) / 4)
            type = 2;
        else if (smoking_days > (AFTER_FIRE_LENGTH) / 4)
            type = 1;
        else
            type = 0;
        return;
    }

    burning_days++;
    if (world(x,y)->flags & FLAG_FIRE_COVER)
    {       burning_days += 4;}
    days_before_spread--;
    if( !(flags & FLAG_IS_GHOST) )
    {   world(x,y)->pollution++;}
    if (real_time > anim)
    {
        anim = real_time + FIRE_ANIMATION_SPEED;
        if(++type >= constructionGroup->graphicsInfoVector.size())
        {   type = 0;}
    }
    if ((days_before_spread == 0) && !(flags & FLAG_IS_GHOST))
    {
        days_before_spread = FIRE_DAYS_PER_SPREAD;
        if ((rand() % 20) == 1)
        {
            i = rand() % 4;
            switch (i)
            {
                case (0):
                    do_random_fire(x - 1, y, 0);
                break;
                case (1):
                    do_random_fire(x, y - 1, 0);
                break;
                case (2):
                    do_random_fire(x + 1, y, 0);
                break;
                case (3):
                    do_random_fire(x, y + 1, 0);
                break;
            }
        }
    }
}

void Fire::report()
{
    int i = 0;

    mps_store_sd(i++,constructionGroup->name,ID);
    i++;
    mps_store_sd(i++,"Air Pollution",world(x,y)->pollution);
    if (burning_days < FIRE_LENGTH)
    {   mps_store_sddp(i++,"burnt down",burning_days,FIRE_LENGTH);}
    else
    {   mps_store_sddp(i++,"degraded",smoking_days,AFTER_FIRE_LENGTH);}
}

/** @file lincity/modules/fire.cpp */

