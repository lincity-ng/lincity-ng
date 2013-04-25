/* ---------------------------------------------------------------------- *
 * firestation.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */


#include "firestation.h"


// FireStation:
FireStationConstructionGroup fireStationConstructionGroup(
    "Fire station",
    FALSE,                     /* need credit? */
    GROUP_FIRESTATION,
    2,                         /* size */
    GROUP_FIRESTATION_COLOUR,
    GROUP_FIRESTATION_COST_MUL,
    GROUP_FIRESTATION_BUL_COST,
    GROUP_FIRESTATION_FIREC,
    GROUP_FIRESTATION_COST,
    GROUP_FIRESTATION_TECH
);

Construction *FireStationConstructionGroup::createConstruction(int x, int y, unsigned short type) {
    return new FireStation(x, y, type);
}

void FireStation::update() {
    /*
       // int_1 is the jobs stored at the fire station
       // int_2 is the goods stored at the fire station
       // int_3 is the animation flag
       // int_4 unused
       // int_5 is the pause counter
       // MP_ANIM is the time of the next frame since 1.91
     */
    /* XXX: should note whether we actually _produced_ fire cover in int_6 */

	// use "commodityCount[COMMODITY_JOBS]" instead of "jobs"

	// handled by transport:
    //if (jobs < (constructionGroup->commodityRuleCount[STUFF_JOBS].maxload - FIRESTATION_GET_JOBS))
    //    if (get_jobs(x, y, FIRESTATION_GET_JOBS) != 0){
    //        jobs += FIRESTATION_GET_JOBS;
    //        commodityCount[STUFF_JOBS] += FIRESTATION_GET_JOBS;
    //    }
	// also handled by transport:
    //if (goods < (constructionGroup->commodityRuleCount[STUFF_GOODS].maxload - FIRESTATION_GET_GOODS))
    //    if (get_goods(x, y, FIRESTATION_GET_GOODS) != 0){
    //        goods += FIRESTATION_GET_GOODS;
    //        commodityCount[STUFF_GOODS] += FIRESTATION_GET_GOODS;
    //    }
	// FIXME: if insufficient commodities -> return

    /* animate */
    if (animate && real_time > anim) 
    {
        anim = real_time + FIRESTATION_ANIMATION_SPEED;
        switch (type)
        {
            case (CST_FIRESTATION_1):
                type = CST_FIRESTATION_2;
                break;
            case (CST_FIRESTATION_2):
                type = CST_FIRESTATION_3;
                break;
            case (CST_FIRESTATION_3):
                type = CST_FIRESTATION_4;
                break;
            case (CST_FIRESTATION_4):
                type = CST_FIRESTATION_5;
                break;
            case (CST_FIRESTATION_5):
                type = CST_FIRESTATION_6;
                break;
            case (CST_FIRESTATION_6):
                type = CST_FIRESTATION_7;
                anim += 10 * FIRESTATION_ANIMATION_SPEED; /* pause */
                break;
            case (CST_FIRESTATION_7):
                type = CST_FIRESTATION_8;
                break;
            case (CST_FIRESTATION_8):
                type = CST_FIRESTATION_9;
                break;
            case (CST_FIRESTATION_9):
                type = CST_FIRESTATION_10;
                break;
            case (CST_FIRESTATION_10):
                type = CST_FIRESTATION_1;
                animate = false;        /* stop */
                break;

        }
            //compatibility for old map        
            //MP_TYPE(x,y)=type;         
    }
    /* That's all. Cover is done by different functions every 3 months or so. */

    fire_cost += FIRESTATION_RUNNING_COST;
}

void FireStation::cover() {
    int xx, x1, x2, y1, y2;
    if (commodityCount[STUFF_JOBS] < (FIRESTATION_JOBS * DAYS_BETWEEN_COVER)
    ||  commodityCount[STUFF_GOODS] < (FIRESTATION_GOODS * DAYS_BETWEEN_COVER)
    ||  commodityCount[STUFF_WASTE] + (FIRESTATION_GOODS * DAYS_BETWEEN_COVER / 3) > MAX_WASTE_AT_FIRESTATION  )
    {        
        busy = false;        
        return;
    }
    commodityCount[STUFF_JOBS] -= (FIRESTATION_JOBS * DAYS_BETWEEN_COVER);
    commodityCount[STUFF_GOODS] -= (FIRESTATION_GOODS * DAYS_BETWEEN_COVER);
    commodityCount[STUFF_WASTE] += (FIRESTATION_GOODS * DAYS_BETWEEN_COVER /3);
    animate = true;
    busy = true;

    x1 = x - FIRESTATION_RANGE;
    if (x1 < 0)
        x1 = 0;
    x2 = x + FIRESTATION_RANGE;
    if (x2 > world.len())
        x2 = world.len();
    y1 = y - FIRESTATION_RANGE;
    if (y1 < 0)
        y1 = 0;
    y2 = y + FIRESTATION_RANGE;
    if (y2 > world.len())
        y2 = world.len();
    for (; y1 < y2; y1++)
        for (xx = x1; xx < x2; xx++){
            world(xx, y1)->flags |= FLAG_FIRE_COVER;
            //MP_INFO(xx, y1).flags |= FLAG_FIRE_COVER;
        }
}

void FireStation::report() {
    int i = 0;
    const char* p;

    mps_store_sd(i++,constructionGroup->name,ID);   
    i++;
    list_commodities(&i);    
    //mps_store_title(i++, _("Accepting"));
    //mps_store_ssddp(i++,"=> ","Jobs", commodityCount[STUFF_JOBS], constructionGroup->commodityRuleCount[STUFF_JOBS].maxload);
    //mps_store_ssddp(i++, "=> ","Goods", commodityCount[STUFF_GOODS], constructionGroup->commodityRuleCount[STUFF_GOODS].maxload);
    //mps_store_title(i++, _("Providing"));
    //mps_store_ssddp(i++,"=> ", "Waste", commodityCount[STUFF_WASTE], constructionGroup->commodityRuleCount[STUFF_WASTE].maxload);
    p =  busy?"Yes":"No";    
    mps_store_ss(i++, "Fire Protection", p);

}

/** @file lincity/modules/firestation.cpp */

