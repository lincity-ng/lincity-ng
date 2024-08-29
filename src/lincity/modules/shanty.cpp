/* ---------------------------------------------------------------------- *
 * shanty.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "shanty.h"

#include <stdio.h>                        // for fprintf, printf, stderr
#include <stdlib.h>                       // for rand
#include <vector>                         // for vector

#include "commune.h"                      // for COMMUNE_POP, Commune (ptr o...
#include "fire.h"                         // for FIRE_ANIMATION_SPEED
#include "lincity/ConstructionManager.h"  // for ConstructionManager
#include "lincity/ConstructionRequest.h"  // for BurnDownRequest
#include "modules.h"                      // for Commodity, ExtraFrame, find...
#include "modules_interfaces.h"           // for add_a_shanty, update_shanty

// Shanty:
ShantyConstructionGroup shantyConstructionGroup(
    N_("Shanty Town"),
     FALSE,                     /* need credit? */
     GROUP_SHANTY,
     GROUP_SHANTY_SIZE,
     GROUP_SHANTY_COLOUR,
     GROUP_SHANTY_COST_MUL,
     GROUP_SHANTY_BUL_COST,
     GROUP_SHANTY_FIREC,
     GROUP_SHANTY_COST,
     GROUP_SHANTY_TECH,
     GROUP_SHANTY_RANGE
);

Construction *ShantyConstructionGroup::createConstruction() {
  return new Shanty(this);
}

//TODO remove_a_shanty() and update_shanty() should go to ConstructionRequest

void add_a_shanty(void)
{
    int r, x, y;
    int numof_shanties = shantyConstructionGroup.count;
    const int len = world.len();
    x = rand() % len;
    y = rand() % len;
    if (numof_shanties > 0 && rand() % 8 != 0)
    {
        r = find_group(x, y, GROUP_SHANTY);
        if (r == -1) {
            printf("Looked for a shanty, without any! x=%d y=%d\n", x, y);
            return;
        }
        y = r / len;
        x = r % len;
        r = find_bare_area(x, y, 2);
        if (r == -1)
        {
#ifdef DEBUG
            fprintf(stderr,"Adding a shanty (s), no space for it?!\n");
#endif
            return;
        }
        y = r / len;
        x = r % len;
    }
    else
    {
        r = find_group(x, y, GROUP_MARKET);
        if (r == -1)
            return;             /* silently return, we havn't started yet. */

        y = r / len;
        x = r % len;
        r = find_bare_area(x, y, 2);
        if (r == -1)
        {
#ifdef DEBUG
            fprintf(stderr,"Adding a shanty (r), no space for it?!\n");
#endif
            return;
        }
        y = r / len;
        x = r % len;
    }
    shantyConstructionGroup.placeItem( x, y);
}

void update_shanty(void)
{
    int numof_communes = communeConstructionGroup.count;
    int numof_shanties = shantyConstructionGroup.count;
    const int len = world.len();
    //Foersts make new people? Why not
    //people_pool += .3 * numof_communes;
    int pp = people_pool;
    people_pool -= 5 * numof_shanties;
    if (people_pool < 0)
    {   people_pool = 0;  }
    ddeaths += (pp - people_pool);
    pp = people_pool - (COMMUNE_POP * numof_communes);
    int i = (pp - SHANTY_MIN_PP) / SHANTY_POP;
    if (i < 0)
    {   i = 0;}
    if (i > numof_shanties)
    {
        for (int n = 0; n < 1 + (i - numof_shanties)/10; n++)
        {   add_a_shanty();}
    }
    else if (numof_shanties > 0 && (i < (numof_shanties - 1) ))
    {
        for (int n = 0; n < (1+(numof_shanties - i)/10); n++)
        {
            int x, y, r;
            x = rand() % len;
            y = rand() % len;
            r = find_group(x, y, GROUP_SHANTY);
            if (r == -1)
            {
                fprintf(stderr, "Can't find a shanty to remove!\n");
                return;
            }
            y = r / len;
            x = r % len;
            ConstructionManager::executeRequest(new BurnDownRequest(world(x,y)->reportingConstruction));
        }
    }
}

void Shanty::update()
{
    //steal stuff and make waste
    produceStuff(STUFF_WASTE, SHANTY_PUT_WASTE);
    if (commodityCount[STUFF_FOOD] >= SHANTY_GET_FOOD)
    {   consumeStuff(STUFF_FOOD, SHANTY_GET_FOOD);}
    if (commodityCount[STUFF_LABOR] >= SHANTY_GET_LABOR)
    {
        consumeStuff(STUFF_LABOR, SHANTY_GET_LABOR);
        if ((income_tax -= SHANTY_GET_LABOR * 2) < 0)
        {   income_tax = 0;}
    }
    if (commodityCount[STUFF_GOODS] >= SHANTY_GET_GOODS)
    {
        consumeStuff(STUFF_GOODS, SHANTY_GET_GOODS);
        produceStuff(STUFF_WASTE, SHANTY_GET_GOODS / 3);
        if ((goods_tax -= SHANTY_GET_GOODS * 2) < 0)
        {   goods_tax = 0;}
    }
    if (commodityCount[STUFF_COAL] >= SHANTY_GET_COAL)
    {
        consumeStuff(STUFF_COAL, SHANTY_GET_COAL);
        if ((coal_tax -= SHANTY_GET_COAL * 2) < 0)
        {   coal_tax = 0;}
    }
    if (commodityCount[STUFF_ORE] >= SHANTY_GET_ORE)
    {   consumeStuff(STUFF_ORE, SHANTY_GET_ORE);}
    if (commodityCount[STUFF_STEEL] >= SHANTY_GET_STEEL)
    {   consumeStuff(STUFF_STEEL, SHANTY_GET_STEEL);}
    produceStuff(STUFF_WASTE, SHANTY_PUT_WASTE);
    if (commodityCount[STUFF_WASTE] >= MAX_WASTE_AT_SHANTY)
    {
        world(x+1,y+1)->pollution += commodityCount[STUFF_WASTE];
        levelStuff(STUFF_WASTE, 0);
        start_burning_waste = true;
    }

    if(total_time % 100 == 99) {
        reset_prod_counters();
    }
}

void Shanty::animate() {
  if(start_burning_waste) { // start fire
    start_burning_waste = false;
    anim = real_time + ANIM_THRESHOLD(3 * WASTE_BURN_TIME);
  }
  if(real_time >= anim) { // stop fire
    waste_fire_frit->frame = -1;
  }
  else if(real_time >= waste_fire_anim) { // continue fire
    waste_fire_anim = real_time + ANIM_THRESHOLD(FIRE_ANIMATION_SPEED);
    int num_frames = waste_fire_frit->resourceGroup->graphicsInfoVector.size();
    if(++waste_fire_frit->frame >= num_frames)
      waste_fire_frit->frame = 0;
  }
}

void Shanty::report()
{
    int i = 0;
    mps_store_title(i, constructionGroup->name);
    mps_store_sd(i++, N_("Air Pollution"), world(x,y)->pollution);
    // i++;
    list_commodities(&i);
}

/** @file lincity/modules/shanty.cpp */
