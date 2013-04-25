/* ---------------------------------------------------------------------- *
 * windmill.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "windmill.h"


WindmillConstructionGroup windmillConstructionGroup(
    "Windmill",
     FALSE,                     /* need credit? */
     GROUP_WINDMILL,
     2,                         /* size */
     GROUP_WINDMILL_COLOUR,
     GROUP_WINDMILL_COST_MUL,
     GROUP_WINDMILL_BUL_COST,
     GROUP_WINDMILL_FIREC,
     GROUP_WINDMILL_COST,
     GROUP_WINDMILL_TECH
);

Construction *WindmillConstructionGroup::createConstruction(int x, int y, unsigned short type) {
    return new Windmill(x, y, type);
}


void Windmill::update()
{
    
    windmill_cost++;
    if ((commodityCount[STUFF_JOBS] >= WINDMILL_JOBS)
     && (commodityCount[STUFF_KWH] <= MAX_KWH_AT_WINDMILL-kwh_output))
    {
        commodityCount[STUFF_JOBS] -= WINDMILL_JOBS;
        commodityCount[STUFF_KWH] += kwh_output;
        animate = true;
        working_days++;  
    }
    else
    {      
        animate = false;      
    }
    //monthly update
    if (total_time % 100 == 0)
    {
        busy = working_days;
        working_days = 0;
    }
    //Animation
    if (animate && real_time > anim)
    {
        sail_count++;
        anim = real_time + is_modern?MODERN_WINDMILL_ANIM_SPEED:ANTIQUE_WINDMILL_ANIM_SPEED;
        sail_count %= 3;
        if (!is_modern)
        {
            type = CST_WINDMILL_1_W + sail_count;
        } else
        {
            if (commodityCount[STUFF_KWH]>MAX_KWH_AT_WINDMILL/2)
            {            
                type = CST_WINDMILL_1_G + sail_count;
            }
            else if (commodityCount[STUFF_KWH] > 0)
            {
                type = CST_WINDMILL_1_RG + sail_count;
            }
            else
            {
                type = CST_WINDMILL_1_R + sail_count;
            }    
        }   
    }
}


void Windmill::report()
{
    int i = 0;    
    mps_store_sd(i++,constructionGroup->name,ID);
    mps_store_sfp(i++, _("busy"), busy);    
    mps_store_sfp(i++, _("Tech"), (tech * 100.0) / MAX_TECH_LEVEL);
    mps_store_sd(i++, "Output", kwh_output); 
    i++;
    list_commodities(&i);
}

/*void mps_windmill(int x, int y)
{
    int i = 0;
    char s[12];

    mps_store_title(i++, _("Windmill"));
    mps_store_sfp(i++, _("Tech"), (MP_TECH(x, y) * 100.0) / MAX_TECH_LEVEL);
    mps_store_sfp(i++, _("Jobs"), (MP_INFO(x, y).int_5 * 100.0) / MP_INFO(x, y).int_1); // either 0 or 100%
    i++;

    if (MP_TECH(x, y) >= MODERN_WINDMILL_TECH) {
        mps_store_title(i++, _("Local Status"));

        format_power(s, sizeof(s), MP_INFO(x, y).int_5);
        mps_store_ss(i++, _("Prod."), s);

        format_power(s, sizeof(s), MP_INFO(x, y).int_4);
        mps_store_ss(i++, _("Demand"), s);
        i++;

        mps_store_title(i++, _("Grid Status"));

        format_power(s, sizeof(s), grid[MP_INFO(x, y).int_6]->max_power);
        mps_store_ss(i++, _("T. Cap."), s);

        format_power(s, sizeof(s), grid[MP_INFO(x, y).int_6]->avail_power);
        mps_store_ss(i++, _("A. Cap."), s);

        format_power(s, sizeof(s), grid[MP_INFO(x, y).int_6]->demand);
        mps_store_ss(i++, _("Demand"), s);
        mps_store_sd(i++, _("Grid ID"), MP_INFO(x, y).int_6);
    }
}
*/
/** @file lincity/modules/windmill.cpp */

