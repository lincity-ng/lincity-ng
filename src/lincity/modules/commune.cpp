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

Construction *CommuneConstructionGroup::createConstruction(int x, int y) {
    return new Commune(x, y, this);
}

void Commune::update()
{
    int tmpUgwCount = ugwCount;
    int tmpCoalprod = coalprod;
    const unsigned short s = constructionGroup->size;
    const unsigned short a = s*s;
    if(commodityCount[STUFF_WATER]>= (a-ugwCount)*WATER_FOREST)
    {
        tmpUgwCount = a;
        tmpCoalprod = COMMUNE_COAL_MADE;
        commodityCount[STUFF_WATER] -= (a-ugwCount)*WATER_FOREST;
    }
    if(//(total_time & 1) && //make coal every second day
       (tmpCoalprod > 0)
    && (commodityCount[STUFF_COAL] + tmpCoalprod <= MAX_COAL_AT_COMMUNE ))
    {
         commodityCount[STUFF_COAL] += tmpCoalprod;
         monthly_stuff_made++;
    }
    if(commodityCount[STUFF_ORE] + COMMUNE_ORE_MADE <= MAX_ORE_AT_COMMUNE)
    {
        commodityCount[STUFF_ORE] += COMMUNE_ORE_MADE;
        monthly_stuff_made++;
    }
    /* recycle a bit of waste if there is plenty*/
    if (commodityCount[STUFF_WASTE] >= 3 * COMMUNE_WASTE_GET)
    {
        commodityCount[STUFF_WASTE] -= COMMUNE_WASTE_GET;
        monthly_stuff_made++;
        if(commodityCount[STUFF_ORE] + COMMUNE_ORE_FROM_WASTE <= MAX_ORE_AT_COMMUNE )
        {   commodityCount[STUFF_ORE] += COMMUNE_ORE_FROM_WASTE;}
    }
    if (total_time % 10 == 0)
    {
        if(monthly_stuff_made)
        {   animate = true;}
        int modulus = ((total_time%20)?1:0);
        for(int idx = 0; idx < tmpUgwCount; idx++)
        {
            int i = x + idx % s;
            int j = y + idx / s;
            if((i+j)%2==modulus && world(i,j)->pollution)
            {   --world(i,j)->pollution;}
        }
        if (!steel_made && commodityCount[STUFF_STEEL] + COMMUNE_STEEL_MADE <= MAX_STEEL_AT_COMMUNE)
        {
            monthly_stuff_made++;
            steel_made = true;
            commodityCount[STUFF_STEEL] += COMMUNE_STEEL_MADE;
        }
        else
        {   steel_made = false;}
    }

    if (total_time % 100 == 1)
    {//each month
        if (steel_made)
        {//producing steel
            if (type < 6)
            {   type += 5;}
        }
        else if (type >= 6) // not producing steel
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
            type = 0;
            lazy_months++;
            /* Communes without production only last 10 years */
            if (lazy_months > 120)
            {   ConstructionManager::submitRequest(new CommuneDeletionRequest(this));}
        }//end we are lazy
    }//end each month
    /* animate */
    if (animate && real_time >= anim)
    {
        anim = real_time + COMMUNE_ANIM_SPEED - 25 + (rand() % 50);
        if (type < 6) //not producing steel
        {
            if( ++type >= 6 )
            {
                animate = false;
                type = 1;
            }
        }
        else //producing steel
        {
            if( ++type >= 10 )
            {
                animate = false;
                type = 6;
            }
        }
        if (type >= constructionGroup->graphicsInfoVector.size())
        {   type = 0;}
    }
}

void Commune::report()
{
    int i = 0;
    mps_store_sd(i++, constructionGroup->name,ID);
    mps_store_sddp(i++, "Fertility", ugwCount, constructionGroup->size * constructionGroup->size);
    mps_store_sfp(i++, "busy", (float)last_month_output / 3.05);
    mps_store_sd(i++, "Pollution", world(x,y)->pollution);
    if(lazy_months)
    {   mps_store_sddp(i++, "lazy months", lazy_months, 120);}
    else
    {   mps_store_title(i++, "");}
    list_commodities(&i);
}

/** @file lincity/modules/commune.cpp */

