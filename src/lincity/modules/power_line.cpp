/* ---------------------------------------------------------------------- *
 * power_line.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */
#include "power_line.h"
#include "stdlib.h"

//Power line
PowerlineConstructionGroup powerlineConstructionGroup(
    "Power line",
    FALSE,                     /* need credit? */
    GROUP_POWER_LINE,
    GROUP_POWER_LINE_SIZE,
    GROUP_POWER_LINE_COLOUR,
    GROUP_POWER_LINE_COST_MUL,
    GROUP_POWER_LINE_BUL_COST,
    GROUP_POWER_LINE_FIREC,
    GROUP_POWER_LINE_COST,
    GROUP_POWER_LINE_TECH,
    GROUP_POWER_LINE_RANGE
);

Construction *PowerlineConstructionGroup::createConstruction(int x, int y)
{
    return new Powerline(x, y, this);
}

void Powerline::update()
{
    if (commodityCount[STUFF_MWH] > 0)
    {
        commodityCount[STUFF_MWH]--;// loss on powerline
        if (anim_counter > 0)
        {
            switch (anim_counter)
            {
                case POWER_MODULUS - 2:
                    if ( !(type >= 11) )
                        break;
                    flashing = false;
                    type -= 11;
                    break;
                case POWER_MODULUS:
                    if ( !(type <= 10) )
                        break;
                    flashing = true;
                    type += 11;
                    break;
            } //end switch anim_counter
            --anim_counter;
        }
    } // endif MWH
}

void Powerline::report()
{
    int i = 0;

    mps_store_sd(i++,constructionGroup->name,ID);
    mps_store_sfp(i++, "usage", trafficCount[STUFF_MWH] * 100 * TRANSPORT_RATE / TRANSPORT_QUANTA);
    i++;
    list_commodities(&i);
}


/** @file lincity/modules/power_line.cpp */

