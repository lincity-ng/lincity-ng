/* ---------------------------------------------------------------------- *
 * power_line.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "power_line.h"

#include <list>                     // for _List_iterator
#include <string>                   // for basic_string

#include "gui_interface/mps.h"      // for mps_store_sd, mps_store_sfp
#include "lincity/engine.h"         // for real_time
#include "lincity/groups.h"         // for GROUP_POWER_LINE
#include "lincity/lintypes.h"       // for ExtraFrame, Commodity, Constructi...
#include "lincity/transport.h"      // for POWER_MODULUS, GROUP_POWER_LINE_B...
#include "tinygettext/gettext.hpp"  // for N_

//Power line
PowerlineConstructionGroup powerlineConstructionGroup(
    N_("Power line"),
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
        consumeStuff(STUFF_MWH, 1);// loss on powerline
    } // endif MWH

    if(total_time % 100 == 99) {
        reset_prod_counters();
    }
}

void Powerline::animate() {
  switch(anim_counter) {
  case POWER_MODULUS - 2:
    if ( !(frameIt->frame >= 11) )
      break;
    flashing = false;
    frameIt->frame -= 11;
    break;
  case POWER_MODULUS:
    if ( !(frameIt->frame < 11) )
      break;
    flashing = true;
    frameIt->frame += 11;
    break;
  }
  if(anim_counter > 0 && real_time >= anim) {
    anim = real_time + ANIM_THRESHOLD(POWER_LINE_FLASH_SPEED);
    --anim_counter;
  }
}

void Powerline::report()
{
    int i = 0;

    mps_store_sd(i++, constructionGroup->name, ID);
    mps_store_sfp(i++, N_("usage"), trafficCount[STUFF_MWH] * 107.77 * TRANSPORT_RATE / TRANSPORT_QUANTA);
    // i++;
    list_commodities(&i);
}


/** @file lincity/modules/power_line.cpp */
