/* ---------------------------------------------------------------------- *
 * pottery.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "pottery.h"

PotteryConstructionGroup potteryConstructionGroup(
    "Pottery",
    FALSE,                     /* need credit? */
    GROUP_POTTERY,
    2,                         /* size */
    GROUP_POTTERY_COLOUR,
    GROUP_POTTERY_COST_MUL,
    GROUP_POTTERY_BUL_COST,
    GROUP_POTTERY_FIREC,
    GROUP_POTTERY_COST,
    GROUP_POTTERY_TECH,
    GROUP_POTTERY_RANGE
);

Construction *PotteryConstructionGroup::createConstruction(int x, int y, unsigned short type) {
    return new Pottery(x, y, type);
}

void Pottery::update()
{
     if (total_time % 100 == 0) {
        productivity = workingdays;
        workingdays = 0;
        }

    if (pauseCounter++ < 0)
        return;   
        if (   commodityCount[STUFF_GOODS] < (MAX_GOODS_AT_POTTERY - POTTERY_MADE_GOODS)
            && commodityCount[STUFF_ORE] > POTTERY_ORE_MAKE_GOODS
            && commodityCount[STUFF_COAL] > POTTERY_COAL_MAKE_GOODS
            && commodityCount[STUFF_JOBS] > POTTERY_JOBS) 
        {
            commodityCount[STUFF_GOODS] += POTTERY_MADE_GOODS;
            commodityCount[STUFF_ORE] -= POTTERY_ORE_MAKE_GOODS;
            commodityCount[STUFF_COAL] -= POTTERY_COAL_MAKE_GOODS;
            commodityCount[STUFF_JOBS] -= POTTERY_JOBS;
            workingdays++;
            animate = true;
        } 
        else 
        {
            type = CST_POTTERY_1;
            pauseCounter = -POTTERY_CLOSE_TIME;
            return;
        }
    if (animate && real_time > anim)
    {
        anim = real_time + POTTERY_ANIM_SPEED;
        switch (type)
        {
            case (CST_POTTERY_0):
                type = CST_POTTERY_1;
                break;
            case (CST_POTTERY_1):
                type = CST_POTTERY_2;
                break;
            case (CST_POTTERY_2):
                type = CST_POTTERY_3;
                break;
            case (CST_POTTERY_3):
                type = CST_POTTERY_4;
                break;
            case (CST_POTTERY_4):
                type = CST_POTTERY_5;
                break;
            case (CST_POTTERY_5):
                type = CST_POTTERY_6;
                break;
            case (CST_POTTERY_6):
                type = CST_POTTERY_7;
                break;
            case (CST_POTTERY_7):
                type = CST_POTTERY_8;
                break;
            case (CST_POTTERY_8):
                type = CST_POTTERY_9;
                break;
            case (CST_POTTERY_9):
                type = CST_POTTERY_10;
                break;
            case (CST_POTTERY_10):
                type = CST_POTTERY_1;
                world(x,y)->pollution++;
                animate = false;
                break;
        }   
    }
}

void Pottery::report()
{
    int i = 0;

    mps_store_sd(i++, constructionGroup->name,ID);
    i++;
    mps_store_sfp(i++, _("busy"), (float) productivity);
    i++;
    list_commodities(&i);    
}

/** @file lincity/modules/pottery.cpp */

