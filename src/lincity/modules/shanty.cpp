/* ---------------------------------------------------------------------- *
 * shanty.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
#include "shanty.h"
#include "commune.h"
#include "fire.h"

// Shanty:
ShantyConstructionGroup shantyConstructionGroup(
    "Shanty Town",
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

Construction *ShantyConstructionGroup::createConstruction(int x, int y)
{
    return new Shanty(x, y, this);
}

//TODO remove_a_shanty() and update_shanty() should go to ConstructionRequest

void add_a_shanty(void)
{
    int r, x, y;
    int numof_shanties = Counted<Shanty>::getInstanceCount();
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
        if (r == -1) {
            /* wck: These are annoying when the map is full */
            fprintf(stderr,"Adding a shanty (s), no space for it?!\n");
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
            /* see above */
            fprintf(stderr,"Adding a shanty (r), no space for it?!\n");
            return;
        }
        y = r / len;
        x = r % len;
    }
    shantyConstructionGroup.placeItem( x, y);
}

void update_shanty(void)
{
    int i, pp;
    int numof_communes = Counted<Commune>::getInstanceCount();
    int numof_shanties = Counted<Shanty>::getInstanceCount();
    const int len = world.len();
    //Foersts make new people? Why not
    //people_pool += .3 * numof_communes;
    people_pool -= 5 * numof_shanties;
    if (people_pool < 0)
    {   people_pool = 0;  }
    pp = people_pool - (COMMUNE_POP * numof_communes);
    i = (pp - SHANTY_MIN_PP) / SHANTY_POP;
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
    commodityCount[STUFF_WASTE] += SHANTY_PUT_WASTE;
    if (commodityCount[STUFF_FOOD] >= SHANTY_GET_FOOD)
    {   commodityCount[STUFF_FOOD] -= SHANTY_GET_FOOD;}
    if (commodityCount[STUFF_JOBS] >= SHANTY_GET_JOBS)
    {
        commodityCount[STUFF_JOBS] -= SHANTY_GET_JOBS;
        if ((income_tax -= SHANTY_GET_JOBS * 2) < 0)
        {   income_tax = 0;}
    }
    if (commodityCount[STUFF_GOODS] >= SHANTY_GET_GOODS)
    {
        commodityCount[STUFF_GOODS] -= SHANTY_GET_GOODS;
        commodityCount[STUFF_WASTE] += SHANTY_GET_GOODS / 3;
        if ((goods_tax -= SHANTY_GET_GOODS * 2) < 0)
        {   goods_tax = 0;}
    }
    if (commodityCount[STUFF_COAL] >= SHANTY_GET_COAL)
    {
        commodityCount[STUFF_COAL] -= SHANTY_GET_COAL;
        if ((coal_tax -= SHANTY_GET_COAL * 2) < 0)
        {   coal_tax = 0;}
    }
    if (commodityCount[STUFF_ORE] >= SHANTY_GET_ORE)
    {   commodityCount[STUFF_ORE] -= SHANTY_GET_ORE;}
    if (commodityCount[STUFF_STEEL] >= SHANTY_GET_STEEL)
    {   commodityCount[STUFF_STEEL] -= SHANTY_GET_STEEL;}
    if ((commodityCount[STUFF_WASTE]+= SHANTY_PUT_WASTE) >= MAX_WASTE_AT_SHANTY && !world(x+1,y+1)->construction)
    {
        anim = real_time + 3 * WASTE_BURN_TIME;
        world(x+1,y+1)->pollution += commodityCount[STUFF_WASTE];
        commodityCount[STUFF_WASTE] = 0;
        if(!world(x+1,y+1)->construction)
        {
            Construction *fire = fireConstructionGroup.createConstruction(x+1, y+1);
            world(x+1,y+1)->construction = fire;
            world(x+1,y+1)->reportingConstruction = fire;
            dynamic_cast<Fire*>(fire)->flags |= FLAG_IS_GHOST;
            ::constructionCount.add_construction(fire);
        }
    }
    else if ( real_time > anim && world(x+1,y+1)->construction)
    {
        ::constructionCount.remove_construction(world(x+1,y+1)->construction);
        delete world(x+1,y+1)->construction;
        world(x+1,y+1)->construction = NULL;
        world(x+1,y+1)->reportingConstruction = this;
    }
}

void Shanty::report()
{
    int i = 0;
    mps_store_sd(i++, constructionGroup->name, ID);
    mps_store_sd(i++, "Pollution", world(x,y)->pollution);
    i++;
    list_commodities(&i);
}

/** @file lincity/modules/shanty.cpp */

