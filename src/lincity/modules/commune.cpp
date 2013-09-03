/* ---------------------------------------------------------------------- *
 * commune.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "commune.h"

CommuneConstructionGroup communeConstructionGroup(
    "Forest",
    FALSE,                     /* need credit? */
    GROUP_COMMUNE,
    GROUP_COMMUNE_SIZE,
    GROUP_COMMUNE_COLOUR,
    GROUP_COMMUNE_COST_MUL,
    GROUP_COMMUNE_BUL_COST,
    GROUP_COMMUNE_FIREC,
    GROUP_COMMUNE_COST,
    GROUP_COMMUNE_TECH,
    GROUP_COMMUNE_RANGE
);

Construction *CommuneConstructionGroup::createConstruction(int x, int y, unsigned short type) {
    return new Commune(x, y, type);
}

void Commune::update()
{
    int tmpUgwCount = ugwCount;
    int tmpCoalprod = coalprod;
    if(commodityCount[STUFF_WATER]>= (16-ugwCount)*WATER_FOREST)
    {
        tmpUgwCount = 16;
        tmpCoalprod = COMMUNE_COAL_MADE;
        commodityCount[STUFF_WATER] -= (16-ugwCount)*WATER_FOREST;
    }
    if((total_time & 1) //make coal every second day
    && (tmpCoalprod > 0)
    && (commodityCount[STUFF_COAL] <= MAX_COAL_AT_COMMUNE - tmpCoalprod))
    {
         commodityCount[STUFF_COAL] += tmpCoalprod;
         monthly_stuff_made++;
    }
    if(commodityCount[STUFF_ORE] <= MAX_ORE_AT_COMMUNE - COMMUNE_ORE_MADE)
        {
            commodityCount[STUFF_ORE] += COMMUNE_ORE_MADE;
            monthly_stuff_made++;
        }
    /* recycle a bit of waste if there is plenty*/
    if (commodityCount[STUFF_WASTE] >= 3 * COMMUNE_WASTE_GET)
    {
        commodityCount[STUFF_WASTE] -= COMMUNE_WASTE_GET;
        monthly_stuff_made++;
        if(commodityCount[STUFF_ORE] <= MAX_ORE_AT_COMMUNE - COMMUNE_ORE_FROM_WASTE)
        {   commodityCount[STUFF_ORE] += COMMUNE_ORE_FROM_WASTE;}
    }
    if (total_time % 10 == 0)
    {
        if (monthly_stuff_made)
            animate = true;
        if (!steel_made && commodityCount[STUFF_STEEL] <= MAX_STEEL_AT_COMMUNE - COMMUNE_STEEL_MADE)
        {
            monthly_stuff_made++;
            steel_made = true;
            commodityCount[STUFF_STEEL] += COMMUNE_STEEL_MADE;
            if (world(x,y)->pollution >= 10 + tmpUgwCount)
                world(x,y)->pollution -= tmpUgwCount;
        }
        else
        {   steel_made = false;}
    }

    if (total_time % 100 == 1)
    {//each month
        if (steel_made)
        {//producing steel
            if (type < CST_COMMUNE_7)
            {   type += 5;}
        }
        else if (type >= CST_COMMUNE_7) // not producing steel
        {   type -= 5;}
        last_month_output = monthly_stuff_made;
        monthly_stuff_made = 0;
        if (last_month_output)
        {//we were busy
            monthly_stuff_made = 0;
            if (lazy_months > 0)
            {   --lazy_months;}
        }
        else
        {//we are lazy
            lazy_months++;
            /* Communes without production only last 10 years */
            if (lazy_months > 120)
            {
                // commit suicide, some time later
                ConstructionManager::submitRequest
                (
                    new CommuneDeletionRequest(this)
                );
            } //end 10 years
        }//end we are lazy
    }//end each month
    /* animate */
    if (animate && real_time >= anim)
    {
        anim = real_time + COMMUNE_ANIM_SPEED - 25 + (rand() % 50);
        switch (type)
        {
            case (CST_COMMUNE_1):
                type = CST_COMMUNE_2;
                break;
            case (CST_COMMUNE_2):
                type = CST_COMMUNE_3;
                break;
            case (CST_COMMUNE_3):
                type = CST_COMMUNE_4;
                break;
            case (CST_COMMUNE_4):
                type = CST_COMMUNE_5;
                break;
            case (CST_COMMUNE_5):
                type = CST_COMMUNE_6;
                break;
            case (CST_COMMUNE_6):
                type = CST_COMMUNE_2;
                animate = false;
                break;
            case (CST_COMMUNE_7):
                type = CST_COMMUNE_8;
                break;
            case (CST_COMMUNE_8):
                type = CST_COMMUNE_9;
                break;
            case (CST_COMMUNE_9):
                type = CST_COMMUNE_10;
                break;
            case (CST_COMMUNE_10):
                type = CST_COMMUNE_11;
                break;
            case (CST_COMMUNE_11):
                type = CST_COMMUNE_7;
                animate = false;
                break;
            default:
                type = CST_COMMUNE_1;
                break;
        }
    }
}

void Commune::report()
{
    int i = 0;
    mps_store_sd(i++, constructionGroup->name,ID);
    mps_store_sddp(i++, "Fertility", ugwCount, 16);
    mps_store_sfp(i++, "busy", (float)last_month_output / 2.55);
    mps_store_sd(i++, "Pollution", world(x,y)->pollution);
    if(lazy_months)
    {
        mps_store_sddp(i++, "lazy months", lazy_months, 120);
    }
    else
    {
        mps_store_title(i++, "");
    }
    list_commodities(&i);
}

/** @file lincity/modules/commune.cpp */

