/* ---------------------------------------------------------------------- *
 * light_industry.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
#include "light_industry.h"

// IndustryLight:
IndustryLightConstructionGroup industryLightConstructionGroup(
    "Light Industry",
     FALSE,                     /* need credit? */
     GROUP_INDUSTRY_L,
     GROUP_INDUSTRY_L_SIZE,
     GROUP_INDUSTRY_L_COLOUR,
     GROUP_INDUSTRY_L_COST_MUL,
     GROUP_INDUSTRY_L_BUL_COST,
     GROUP_INDUSTRY_L_FIREC,
     GROUP_INDUSTRY_L_COST,
     GROUP_INDUSTRY_L_TECH,
     GROUP_INDUSTRY_L_RANGE
);

//helper groups for graphics and sound sets, dont add them to ConstructionGroup::groupMap
IndustryLightConstructionGroup industryLight_Q_ConstructionGroup = industryLightConstructionGroup;
IndustryLightConstructionGroup industryLight_L_ConstructionGroup = industryLightConstructionGroup;
IndustryLightConstructionGroup industryLight_M_ConstructionGroup = industryLightConstructionGroup;
IndustryLightConstructionGroup industryLight_H_ConstructionGroup = industryLightConstructionGroup;

Construction *IndustryLightConstructionGroup::createConstruction(int x, int y ) {
    return new IndustryLight(x, y, this);
}

void IndustryLight::update()
{
    int goods_today = 0;

    // make some goods with jobs and ore
    if ((commodityCount[STUFF_JOBS] >= (INDUSTRY_L_JOBS_USED + INDUSTRY_L_JOBS_LOAD_ORE + JOBS_LOAD_ORE))
     && (commodityCount[STUFF_ORE] >= INDUSTRY_L_ORE_USED)
     && (commodityCount[STUFF_GOODS] + INDUSTRY_L_MAKE_GOODS <= MAX_GOODS_AT_INDUSTRY_L))
    {
        commodityCount[STUFF_JOBS] -= (INDUSTRY_L_JOBS_USED + INDUSTRY_L_JOBS_LOAD_ORE + JOBS_LOAD_ORE);
        commodityCount[STUFF_ORE] -= INDUSTRY_L_ORE_USED;
        goods_today = INDUSTRY_L_MAKE_GOODS;
        //make some pollution and waste
        world(x,y)->pollution += (int)(((double)(INDUSTRY_L_POL_PER_GOOD * goods_today) * (1 - bonus)));
        commodityCount[STUFF_WASTE] += (int)(((double)(INDUSTRY_L_POL_PER_GOOD * goods_today) * bonus)*(1-extra_bonus));
        // if the trash bin is full reburn the filterd pollution
        if (commodityCount[STUFF_WASTE] > MAX_WASTE_AT_INDUSTRY_L)
            {
                world(x,y)->pollution += (commodityCount[STUFF_WASTE] - MAX_WASTE_AT_INDUSTRY_L);
                commodityCount[STUFF_WASTE] = MAX_WASTE_AT_INDUSTRY_L;
            }
        //now double goods_today if there are more jobs and steel
        if ((commodityCount[STUFF_JOBS] >= (INDUSTRY_L_JOBS_LOAD_STEEL + JOBS_LOAD_STEEL))
         && (commodityCount[STUFF_STEEL] >= INDUSTRY_L_STEEL_USED )
        && (commodityCount[STUFF_GOODS] + 2 * goods_today <= MAX_GOODS_AT_INDUSTRY_L))
        {
            commodityCount[STUFF_JOBS] -= (INDUSTRY_L_JOBS_LOAD_STEEL + JOBS_LOAD_STEEL);
            commodityCount[STUFF_STEEL] -= INDUSTRY_L_STEEL_USED;
            goods_today *= 2;
        }
        //now check for more ore and power to quadruple goods_today again
        //light industry can use KWH and MWH simultaneously
        int total_power_p_good = (commodityCount[STUFF_KWH] + 2 * commodityCount[STUFF_MWH]) / (4 * goods_today);
        if ((total_power_p_good >= INDUSTRY_L_POWER_PER_GOOD)
         && (commodityCount[STUFF_ORE] >= INDUSTRY_L_ORE_USED)
         && (commodityCount[STUFF_GOODS] + 4 * goods_today <= MAX_GOODS_AT_INDUSTRY_L))
        {
            goods_today *= 4;
            commodityCount[STUFF_ORE] -= INDUSTRY_L_ORE_USED;
            //prefer the mor abundant power
            if(2 * commodityCount[STUFF_MWH] > commodityCount[STUFF_KWH])
            {
                commodityCount[STUFF_MWH] -= INDUSTRY_L_POWER_PER_GOOD * (goods_today / 2);
                if(commodityCount[STUFF_MWH] < 0)
                {
                commodityCount[STUFF_KWH] += 2 * commodityCount[STUFF_MWH];
                commodityCount[STUFF_MWH] = 0;
                }
            }
            else
            {
                commodityCount[STUFF_KWH] -= INDUSTRY_L_POWER_PER_GOOD * goods_today;
                if(commodityCount[STUFF_KWH] < 0)
                {
                commodityCount[STUFF_MWH] += commodityCount[STUFF_KWH]/2;
                commodityCount[STUFF_KWH] = 0;
                }
            }
        }
        commodityCount[STUFF_GOODS] += goods_today;
        goods_this_month += goods_today;
    }// endif make some goods
    //monthly update
    if (total_time % 100 == 0)
    {
        int output_level = goods_this_month / (INDUSTRY_L_MAKE_GOODS * 8);
        busy = output_level;
        goods_this_month = 0;
        //Choose an animation set depending on output_level
        if (output_level > 80)
        {   constructionGroup = &industryLight_H_ConstructionGroup;}
        else if (output_level > 55)
        {   constructionGroup = &industryLight_M_ConstructionGroup;}
        else if (output_level > 25)
        {   constructionGroup = &industryLight_L_ConstructionGroup;}
        else if (output_level > 0)
        {   constructionGroup = &industryLight_Q_ConstructionGroup;}
        else
        {   constructionGroup = &industryLightConstructionGroup;}
        type = 0;
    }// end monthly update
    if ((real_time >= anim) && goods_today)
    {
        anim = real_time + INDUSTRY_L_ANIM_SPEED;
        if(++type >= constructionGroup->graphicsInfoVector.size())
        {   type = 0;}
    }// end animate
}

void IndustryLight::report()
{
    int i = 0;

    mps_store_sd(i++,constructionGroup->name,ID);
    i++;
    mps_store_sfp(i++, _("busy"), (busy));
    mps_store_sfp(i++, _("Tech"), (tech * 100.0) / MAX_TECH_LEVEL);
    i++;
    list_commodities(&i);
}

/** @file lincity/modules/light_industry.cpp */

