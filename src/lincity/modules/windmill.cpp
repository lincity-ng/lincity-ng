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
    if (!(total_time%(WINDMILL_RCOST)))
    {	windmill_cost++;}  
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
    if (animate && (real_time > anim))
    {
        sail_count++;
        anim = real_time + ANTIQUE_WINDMILL_ANIM_SPEED;
        sail_count %= 3;
		type = CST_WINDMILL_1_W + sail_count;
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

/** @file lincity/modules/windmill.cpp */

