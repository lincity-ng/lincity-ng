/* ---------------------------------------------------------------------- *
 * heavy_industry.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
#include "heavy_industry.h"
#include "../transport.h"

// IndustryHeavy:
IndustryHeavyConstructionGroup industryHeavyConstructionGroup(
    "Steel Works",
     FALSE,                     /* need credit? */
     GROUP_INDUSTRY_H,
     GROUP_INDUSTRY_H_SIZE,
     GROUP_INDUSTRY_H_COLOUR,
     GROUP_INDUSTRY_H_COST_MUL,
     GROUP_INDUSTRY_H_BUL_COST,
     GROUP_INDUSTRY_H_FIREC,
     GROUP_INDUSTRY_H_COST,
     GROUP_INDUSTRY_H_TECH,
     GROUP_INDUSTRY_H_RANGE
);

//helper groups for graphics and sound sets, dont add them to ConstructionGroup::groupMap
IndustryHeavyConstructionGroup industryHeavy_L_ConstructionGroup = industryHeavyConstructionGroup;
IndustryHeavyConstructionGroup industryHeavy_M_ConstructionGroup = industryHeavyConstructionGroup;
IndustryHeavyConstructionGroup industryHeavy_H_ConstructionGroup = industryHeavyConstructionGroup;

Construction *IndustryHeavyConstructionGroup::createConstruction(int x, int y) {
    return new IndustryHeavy(x, y, this);
}

void IndustryHeavy::update()
{
    // can we produce steel?
    int steel = ( commodityCount[STUFF_JOBS] >= MAX_ORE_USED / JOBS_MAKE_STEEL + JOBS_LOAD_ORE + JOBS_LOAD_COAL + JOBS_LOAD_STEEL
    && commodityCount[STUFF_ORE] >= MAX_ORE_USED
    && commodityCount[STUFF_STEEL] + MAX_ORE_USED / ORE_MAKE_STEEL <= MAX_STEEL_AT_INDUSTRY_H) ? MAX_ORE_USED / ORE_MAKE_STEEL : 0;

    if (steel > 0)
    {
        //Steel works may either use KWH, MWH or COAL to produce steel
        int used_MWH = 0, used_KWH = 0, used_COAL = 0;
        int powered_steel = 0;
        //First use up MWH
        used_MWH = commodityCount[STUFF_MWH];
        if (used_MWH > (steel - powered_steel) * POWER_MAKE_STEEL / 2)
            used_MWH = (steel - powered_steel) * POWER_MAKE_STEEL / 2;
        powered_steel += 2 * used_MWH / POWER_MAKE_STEEL;
        //Second use up KWH
        if (steel > powered_steel)
        {
            used_KWH = commodityCount[STUFF_KWH];
            if (used_KWH > (steel - powered_steel) * POWER_MAKE_STEEL)
                used_KWH = (steel - powered_steel) * POWER_MAKE_STEEL;
            powered_steel += used_KWH / POWER_MAKE_STEEL;
            //Third use up COAL
            if (steel > powered_steel)
            {
                used_COAL = commodityCount[STUFF_COAL];
                if (used_COAL > (steel - powered_steel) * COAL_MAKE_STEEL)
                    used_COAL = (steel - powered_steel) * COAL_MAKE_STEEL;
                powered_steel += used_COAL / COAL_MAKE_STEEL;
            }//end Third
        }//end Second
        if (powered_steel == steel)
        {
            commodityCount[STUFF_MWH] -= used_MWH;
            commodityCount[STUFF_KWH] -= used_KWH;
            if(used_COAL)// coal power is more laborous
            {
                commodityCount[STUFF_COAL] -= used_COAL;
                commodityCount[STUFF_JOBS] -= JOBS_LOAD_COAL;
            }
        }
        else
        {    steel /= 5;} //inefficient and still dirty unpowered production

        if (steel>0)
        {
            commodityCount[STUFF_JOBS] -= (MAX_ORE_USED / JOBS_MAKE_STEEL);
            //use jobs for loading the ore
            commodityCount[STUFF_JOBS] -= JOBS_LOAD_ORE;
            //use jobs for loading the steel
            commodityCount[STUFF_JOBS] -= JOBS_LOAD_STEEL;
            commodityCount[STUFF_ORE] -= MAX_ORE_USED;
            commodityCount[STUFF_STEEL] += steel;
            steel_this_month += steel;
            //cause some pollution and waste depending on bonuses
            world(x,y)->pollution += (int)(((double)(POL_PER_STEEL_MADE * steel) * (1 - bonus)));
            commodityCount[STUFF_WASTE] += (int)(((double)(POL_PER_STEEL_MADE * steel) * bonus)*(1-extra_bonus));
            // if the trash bin is full reburn the filterd pollution
            if (commodityCount[STUFF_WASTE] > MAX_WASTE_AT_INDUSTRY_H)
            {
                world(x,y)->pollution += (commodityCount[STUFF_WASTE] - MAX_WASTE_AT_INDUSTRY_H);
                commodityCount[STUFF_WASTE] = MAX_WASTE_AT_INDUSTRY_H;
            }
        }//endif steel still > 0
    }//endif steel > 0

    //monthly update
    if (total_time % 100 == 0)
    {
        output_level = steel_this_month * ORE_MAKE_STEEL / MAX_ORE_USED;
        steel_this_month = 0;
        type = 0;
        //choose graphics depending on output level
        if (output_level > 80)
        {   constructionGroup = &industryHeavy_H_ConstructionGroup;}
        else if (output_level > 30)
        {   constructionGroup = &industryHeavy_M_ConstructionGroup;}
        else if (output_level > 0)
        {   constructionGroup = &industryHeavy_L_ConstructionGroup;}
        else
        {   constructionGroup = &industryHeavyConstructionGroup;}
    }//end monthly update
    //animation
    if (real_time >= anim)
    {
        anim = real_time + INDUSTRY_H_ANIM_SPEED;
        if(++type >= constructionGroup->graphicsInfoVector.size())
        {   type = 0;}
    }
}

void IndustryHeavy::report()
{
    int i = 0;

    mps_store_sd(i++,constructionGroup->name,ID);
    i++;
    mps_store_sfp(i++, _("busy"), (output_level));
    mps_store_sfp(i++, _("Tech"), (tech * 100.0) / MAX_TECH_LEVEL);
    i++;
    list_commodities(&i);
}

/** @file lincity/modules/heavy_industry.cpp */

