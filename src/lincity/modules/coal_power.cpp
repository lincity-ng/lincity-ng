/* ---------------------------------------------------------------------- *
 * coal_power.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "coal_power.h"
#include <stdlib.h>

Coal_powerConstructionGroup coal_powerConstructionGroup(
     N_("Coal Power Station"),
     FALSE,                     /* need credit? */
     GROUP_COAL_POWER,
     GROUP_COAL_POWER_SIZE,
     GROUP_COAL_POWER_COLOUR,
     GROUP_COAL_POWER_COST_MUL,
     GROUP_COAL_POWER_BUL_COST,
     GROUP_COAL_POWER_FIREC,
     GROUP_COAL_POWER_COST,
     GROUP_COAL_POWER_TECH,
     GROUP_COAL_POWER_RANGE
);

//helper groups for graphics and sound sets, dont add them to ConstructionGroup::groupMap
//Coal_powerConstructionGroup coal_power_low_ConstructionGroup  = coal_powerConstructionGroup;
//Coal_powerConstructionGroup coal_power_med_ConstructionGroup  = coal_powerConstructionGroup;
//Coal_powerConstructionGroup coal_power_full_ConstructionGroup = coal_powerConstructionGroup;

Construction *Coal_powerConstructionGroup::createConstruction(int x, int y) {
    return new Coal_power(x, y, this);
}

void Coal_power::update()
{
    int mwh_made = (commodityCount[STUFF_MWH] + mwh_output <= MAX_MWH_AT_COALPS)?mwh_output:MAX_MWH_AT_COALPS-commodityCount[STUFF_MWH];
    int jobs_used = JOBS_COALPS_GENERATE*(mwh_made/100)/(mwh_output/100);
    int coal_used = POWERS_COAL_OUTPUT / POWER_PER_COAL * (mwh_made/100) /(mwh_output/100);
    if ((commodityCount[STUFF_JOBS] >= jobs_used )
     && (commodityCount[STUFF_COAL] >= coal_used)
     && (mwh_made >= POWERS_COAL_OUTPUT))
    {
        commodityCount[STUFF_JOBS] -= jobs_used;
        commodityCount[STUFF_COAL] -= coal_used;
        commodityCount[STUFF_MWH] += mwh_made;
        world(x,y)->pollution += POWERS_COAL_POLLUTION *(mwh_made/100)/(mwh_output/100);
        working_days += (mwh_made/100);
    }
    //monthly update
    if (total_time % 100 == 0)
    {
        busy = working_days / (mwh_output/100);
        animate = (frames[0].resourceGroup->images_loaded);
        if(animate)
        {
            size_t active = frames.size()*busy/90;
            for(size_t i=0; i < frames.size(); ++i)
            {
                if (i < active)
                {
                    if(!frames[i].frame)
                    {   frames[i].frame = (rand()+1) % (frames[i].resourceGroup->graphicsInfoVector.size());}
                }
                else
                {   frames[i].frame = 0;}
            }
        }
        else
        {
            for(size_t i=0; i < frames.size(); ++i)
            {   frames[i].frame = 0;}
        }
        working_days = 0;
    }
    /* choose a graphic */

    if (commodityCount[STUFF_COAL] > (MAX_COAL_AT_COALPS*4/5))
    {   graphicsGroup = ResourceGroup::resMap["PowerCoalFull"];}
    else if (commodityCount[STUFF_COAL] > (MAX_COAL_AT_COALPS / 2))
    {   graphicsGroup = ResourceGroup::resMap["PowerCoalMed"];}
    else if (commodityCount[STUFF_COAL] > (MAX_COAL_AT_COALPS / 10))
    {   graphicsGroup = ResourceGroup::resMap["PowerCoalLow"];}
    else
    {   graphicsGroup = ResourceGroup::resMap["PowerCoalEmpty"];}
    soundGroup = graphicsGroup;

    if (animate && (real_time > anim))
    {
        anim = real_time + SMOKE_ANIM_SPEED;
        for(size_t i = 0; i < frames.size(); ++i)
        {
            if (frames[i].frame)
            {
                if(++(frames[i].frame) >= frames[i].resourceGroup->graphicsInfoVector.size())
                {   frames[i].frame = 1;}
            }
        }
    }

}

void Coal_power::report()
{
    int i = 0;
    mps_store_sd(i++,constructionGroup->getName(), ID);
    mps_store_sfp(i++, N_("busy"), busy);
    mps_store_sfp(i++, N_("Tech"), (float)(tech * 100.0) / MAX_TECH_LEVEL);
    mps_store_sd(i++, N_("Output"), mwh_output);
    i++;
    list_commodities(&i);
}


/** @file lincity/modules/coal_power.cpp */

