/* ---------------------------------------------------------------------- *
 * engine.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "lctypes.h"
#include "lin-city.h"
#include "engine.h"
#include "engglobs.h"
#include "ConstructionManager.h"
#include "simulate.h"
#include "tinygettext/gettext.hpp"
#include "gui_interface/pbar_interface.h"
#include "stats.h"
#include "gui_interface/dialbox_interface.h"
#include "gui_interface/mps.h"
#include "gui_interface/screen_interface.h"
#include "gui_interface/shared_globals.h"
#include "modules/modules_interfaces.h"
#include "modules/all_modules.h"
#include "transport.h"
#include "all_buildings.h"



extern void ok_dial_box(const char *, int, const char *);
extern void print_total_money(void);

/****** Private functions prototypes *****/
//static void bulldoze_mappoint(short fill, int x, int y);
//static int is_real_river(int x, int y);

/*************** Global functions ********************/
void fire_area(int x, int y)
{
    /* this happens when a rocket crashes or on random_fire. */
    if (world(x, y)->getGroup() == GROUP_WATER || world(x, y)->getGroup() == GROUP_FIRE)
    {   return;}
    if(world(x, y)->reportingConstruction)
    {
        //fire is an unatural death for one in two
        if(world(x,y)->is_residence())
        {   unnat_deaths += ((dynamic_cast<Residence*>(world(x,y)->reportingConstruction))->local_population/=2);}
        ConstructionManager::submitRequest( new SetOnFire(world(x,y)->reportingConstruction));
    }
}

int adjust_money(int value)
{
    total_money += value;
    print_total_money();
    mps_update();
    update_pbar(PMONEY, total_money, 0);
    refresh_pbars();            /* This could be more specific */
    // is not triggered in fresh game or during initial setup
    // because there money == 0
    if( (total_money < 0) && ((total_money - value) > 0) )
    {
        ok_dial_box("warning.mes", BAD, _("You just spent all your money."));
    }
    return total_money;
}


int place_item(int x, int y)
{
    int group =  (userOperation->action == UserOperation::ACTION_BUILD)?userOperation->constructionGroup->group:-1;
    int size = 1;

    if (group < 0) {
        ok_dial_box("warning.mes", BAD,
                _
                ("ERROR: group does not exist. This should not happen! Please consider filling a bug report to lincity-ng team, with the saved game and what you did :-) "));
        return -1000;
    }

    if(userOperation->action == UserOperation::ACTION_BUILD)
    {

        userOperation->constructionGroup->placeItem(x, y);
        size = userOperation->constructionGroup->size;
        adjust_money(-userOperation->constructionGroup->getCosts());
    }

    desert_frontier(x - 1, y - 1, size + 2, size + 2);
    connect_rivers(x,y);
    connect_transport(x - 2, y - 2, x + size + 1, y + size + 1);
    return 0;
}

int bulldoze_item(int x, int y)
{
    int g, size = 1;
    bool construction_found = false;
    if (!world(x, y)->is_visible())
    {
        /* This is considered "improper" input.  Silently ignore. */
#ifdef DEBUG
        fprintf(stderr, " try to improperly bulldoze_item CST_USED\n");
#endif
        return -1;
    }

    if (world(x, y)->reportingConstruction )
    {
        construction_found = true;
        size = world(x, y)->reportingConstruction->constructionGroup->size;
        g = world(x, y)->reportingConstruction->constructionGroup->group;
    }
    else
    {
        size = 1; //all non-constructions are 1// MP_SIZE(x, y);
        g = world(x, y)->group;
        //size = main_groups[g].size;
    }

    if (g == GROUP_DESERT)
    {
        /* Nothing to do. */
        return -1;
    }
    else if (g == GROUP_SHANTY)
    {
        ConstructionManager::executeRequest
                (
                    new BurnDownRequest(world(x,y)->reportingConstruction)
                );
        adjust_money(-GROUP_SHANTY_BUL_COST);
    }
    else if (g == GROUP_FIRE)
    {
        return -1;          /* Can't bulldoze ? */
    }
    else
    {
        if (!construction_found)
        {   adjust_money(-world(x,y)->getTileConstructionGroup()->bul_cost);}
        else
        {   adjust_money(-world(x, y)->reportingConstruction->constructionGroup->bul_cost);}

        if (g == GROUP_OREMINE)
        {
            ConstructionManager::executeRequest
            (   new OreMineDeletionRequest(world(x, y)->reportingConstruction));
        }
        else
        {   do_bulldoze_area(x, y);}
    }
    /* Tell mps about it, in case its selected */
    mps_update();
    return size;                /* No longer used... */
}

void do_bulldoze_area(int x, int y) //arg1 was short fill
{

    if (world(x, y)->reportingConstruction)
    {
        ConstructionManager::executeRequest
        (   new ConstructionDeletionRequest(world(x, y)->reportingConstruction));
    }
    else
    {
        if (world(x, y)->is_water())
        {
            world(x, y)->type = CST_GREEN;
            world(x, y)->group = GROUP_BARE;
            world(x, y)->flags &= ~(FLAG_IS_RIVER);
            world(x, y)->flags |= FLAG_ALTERED;
        }
        else
        {
            world(x, y)->type = CST_DESERT;
            world(x, y)->group = GROUP_DESERT;
        }
        if (world(x, y)->construction)
        {   ok_dial_box("fire.mes", BAD, _("ups, Bulldozer found a dangling reportingConstruction"));}
        //Here size is always 1
        connect_rivers(x,y);
        desert_frontier(x - 1, y - 1, 1 + 2, 1 + 2);
        connect_transport(x - 2, y - 2, x + 1 + 1, y + 1 + 1);
    }
}

void do_pollution()
{
    const int len = world.len();
    std::set<int>::iterator it;
    //kill pollution from edges of map
    //diffuse pollution inside the map

    for (it = world.polluted.begin(); it != world.polluted.end(); ++it)
    {
        if (world.is_border(*it))
        {
            world(*it)->pollution /= POL_DIV;
            continue;
        }

        if (world(*it)->pollution > 10 )
        {
            int x = *it % len;
            int y = *it / len;

            //assert(world.is_visible(x,y));

            int pflow;
            pflow = world(x, y)->pollution/16;
            world(x, y)->pollution -= pflow;
            switch (rand() % 11)
            {
                case 0:/* up */
                case 1:
                case 2:
                    world(x, y-1)->pollution += pflow;
                break;
                case 3:/* right */
                case 4:
                case 5:
                    world(x-1, y)->pollution += pflow;
                break;
                case 6:/* down */
                case 7:
                    world(x, y+1)->pollution += pflow;
                break;
                case 8:/* left */
                case 9:
                    world(x+1, y)->pollution += pflow;
                break;
                case 10:/* clean up*/
                    world(x, y)->pollution += (pflow - 2);
                break;
            }// endswitch
        }// endif
    }// endfor index
}

void scan_pollution()
{
    const int len = world.len();
    const int area = len * len;
    std::set<int>::iterator it;
    total_pollution = 0;
    for (int index = 0; index < area; ++index)
    {
        int x = index % len;
        int y = index / len;
        it = world.polluted.find(index);
        if( (world(x,y)->pollution > 10)
         && (it == world.polluted.end()))
        {   world.polluted.insert(index);}
        else if ((world(x,y)->pollution <= 10)
              && (it != world.polluted.end()))
        {   world.polluted.erase(it);}
        total_pollution += world(x,y)->pollution;
    }
}

void do_fire_health_cricket_power_cover(void)
{
    const int len = world.len();
    const int area = len * len;
    const int mask = ~(FLAG_FIRE_COVER | FLAG_HEALTH_COVER | FLAG_CRICKET_COVER | FLAG_MARKET_COVER );
    for(int index = 0; index < area; ++index)
    {   world(index)->flags &= mask;}
    refresh_cover = true; //constructions will call ::cover()
}

void do_random_fire(int x, int y, int pwarning)
{                               /* well random if x=y=-1 */
    int xx, yy;
    if (x == -1 && y == -1)
    {
        x = rand() % world.len();
        y = rand() % world.len();
    }
    else
    {
        if (!world.is_inside(x, y))
        {
            return;
        }
    }
    if(world(x, y)->reportingConstruction)
    {
        xx = world(x, y)->reportingConstruction->x;
        yy = world(x, y)->reportingConstruction->y;
        x = xx;
        y = yy;
    }

    xx = rand() % 100;
    if(world(x, y)->reportingConstruction)
    {
        if (xx >= world(x, y)->reportingConstruction->constructionGroup->fire_chance)
            return;
    }
    else
    {
        if (xx >= (world(x, y)->getConstructionGroup()->fire_chance))
            return;
    }
    if ((world(x, y)->flags & FLAG_FIRE_COVER) != 0)
        return;
    if (pwarning)
    {
        if(world(x, y)->reportingConstruction)
            ok_dial_box("fire.mes", BAD, world(x, y)->reportingConstruction->
constructionGroup->name);
        else
            ok_dial_box("fire.mes", BAD, _("UNKNOWN!"));
    }
    fire_area(x, y);
}

void do_daily_ecology() //should be going to MapTile:: und handled during simulation
{
    for (int y = 0; y < world.len(); y++)
        for (int x = 0; x < world.len(); x++)
        {   /* approximately 3 monthes needed to turn bulldoze area into green */
            if ((world(x, y)->getGroup() == GROUP_DESERT ||
                 world(x, y)->getGroup() == GROUP_POWER_LINE)
                && (world(x, y)->flags & FLAG_HAS_UNDERGROUND_WATER)
                && (rand() % 300 == 1))
            {
                world(x, y)->setTerrain(CST_GREEN);
                desert_frontier(x - 1, y - 1, 1 + 2, 1 + 2);
            }
        }
    //TODO: depending on water, green can become trees
    //      pollution can make desert
    //      etc ...
    /*TODO incorporate do_daily_ecology to simulate_mappoints. */
}

int check_group(int x, int y)
{
    if (! world.is_inside(x, y) )
        return -1;
    return world(x, y)->getGroup();
}

int check_topgroup(int x, int y)
{
    if (!world.is_inside(x, y) )
        return -1;
    return world(x, y)->getTopGroup();
}

bool check_water(int x, int y)
{
    if (!world.is_inside(x, y) )
        return false;
    return world(x, y)->is_water();
}


void connect_rivers(int x, int y)
{
    std::deque<int> line;
    const int len = world.len();

    line.clear();
    //only act on lakes
    if ( world(x,y)->is_lake())
    {   line.push_back(y*len+x);}

    while(line.size()>0)
    {
        int x = line.front() % len;
        int y = line.front() / len;
        line.pop_front();
        //check for close by river
        for(unsigned int i = 0;i<4;++i)
        {
            int xx = x + dx[i];
            int yy = y + dy[i];
            if(world(xx,yy)->is_river())
            {
                world(x, y)->flags |= FLAG_IS_RIVER;
                i = 4;
                //now check for more close by lakes
                for(unsigned int j = 0;j<4;++j)
                {
                    int x3 = x + dx[j];
                    int y3 = y + dy[j];
                    if (world(x3,y3)->is_lake())
                    {   line.push_back(y3*len+x3);}
                }
            }
        }
    }
}

/* Feature: coal survey should vary in price and accuracy with technology */
void do_coal_survey(void)
{
    if (coal_survey_done == 0)
    {
        adjust_money(-1000000);
        coal_survey_done = 1;
    }
}

void desert_frontier(int originx, int originy, int w, int h)
{
    /* copied from connect_transport */
    // sets the correct TYPE depending on neighbours, => gives the correct tile to display
    int mask;
/*
    static const short desert_table[16] = {
        CST_DESERT_0, CST_DESERT_1D, CST_DESERT_1R, CST_DESERT_2RD,
        CST_DESERT_1L, CST_DESERT_2LD, CST_DESERT_2LR, CST_DESERT_3LRD,
        CST_DESERT_1U, CST_DESERT_2UD, CST_DESERT_2RU, CST_DESERT_3RUD,
        CST_DESERT_2LU, CST_DESERT_3LUD, CST_DESERT_3LRU, CST_DESERT
    };

#if FLAG_LEFT != 1 || FLAG_UP != 2 || FLAG_RIGHT != 4 || FLAG_DOWN != 8
#error  desert_frontier(): you loose
#error  the algorithm depends on proper flag settings -- (ThMO)
#endif
*/
    /* Adjust originx,originy,w,h to proper range */
    if (originx <= 0)
    {
        w -= 1 - originx;
        originx = 1;
    }
    if (originy <= 0)
    {
        h -= 1 - originy;
        originy = 1;
    }
    if (originx + w >= world.len())
    {   w = world.len() - originx;}
    if (originy + h >= world.len())
    {   h = world.len() - originy;}

    for (int x = originx; x < originx + w; x++) {
        for (int y = originy; y < originy + h; y++) {
            if ( world(x, y)->getGroup() == GROUP_DESERT
            || (world(x, y)->getGroup() == GROUP_POWER_LINE
                &&   world(x, y)->group == GROUP_DESERT) )
            {
                mask = 0;
                /* up -- (ThMO) */
                if ( check_group(x, y - 1) == GROUP_DESERT
                 || (check_group(x, y - 1) == GROUP_POWER_LINE
                 && world(x, y - 1)->group == GROUP_DESERT ) )
                {   mask |= 8;}
                /* left -- (ThMO) */
                if ( check_group(x - 1, y) == GROUP_DESERT
                 || (check_group(x - 1, y) == GROUP_POWER_LINE
                 && world(x -1 , y)->group == GROUP_DESERT ) )
                {   mask |= 4;}
                /* right -- (ThMO) */
                if ( check_group(x + 1, y) == GROUP_DESERT
                 || (check_group(x + 1, y) == GROUP_POWER_LINE
                 && world(x + 1, y)->group == GROUP_DESERT ) )
                {   mask |= 2;}
                /* down -- (ThMO) */
                if ( check_group(x, y + 1) == GROUP_DESERT
                 || (check_group(x, y + 1) == GROUP_POWER_LINE
                 && world(x, y + 1)->group == GROUP_DESERT ) )
                {   ++mask;}
                world(x, y)->type = mask;
            }
        }
    }
}

/*
   // spiral arounf mapTile[x][y] until we hit something of group group.
   // return the x y coords encoded as x+y*world.len()
   // return -1 if we don't find one.
 */
int find_group(int x, int y, unsigned short group)
{
    int i, j;
    for (i = 1; i < (2 * world.len()); i++)
    {
        for (j = 0; j < i; j++) {
            x--;
            if (world.is_visible(x, y))
                if (world(x, y)->getTopGroup() == group)
                    return (x + y * world.len());
        }
        for (j = 0; j < i; j++) {
            y--;
            if (world.is_visible(x, y))
                if (world(x, y)->getTopGroup() == group)
                    return (x + y * world.len());
        }
        i++;
        for (j = 0; j < i; j++) {
            x++;
            if (world.is_visible(x, y))
                if (world(x, y)->getTopGroup() == group)
                    return (x + y * world.len());
        }
        for (j = 0; j < i; j++) {
            y++;
            if (world.is_visible(x, y))
                if (world(x, y)->getTopGroup() == group)
                    return (x + y * world.len());
        }
    }
    return (-1);
}

/*
   // spiral round from startx,starty until we hit a 2x2 space.
   // return the x y coords encoded as x+y*world.len()
   // return -1 if we don't find one.
 */
bool is_bare_area(int x, int y, int size)
{
    for(int j = 0; j<size; j++)
    {
        for(int i = 0; i<size; i++)
        {
            if(!world.is_visible(x+i, y+j) || !world(x+i, y+j)->is_bare())
            {
                return false;
            }
        }
    }
    return true;
}

int find_bare_area(int x, int y, int size)
{
    int i, j;
    for (i = 1; i < (2 * world.len()); i++)
    {
        for (j = 0; j < i; j++)
        {
            x--;
            if (world.is_visible(x, y))
                if ( is_bare_area(x, y, size) )
                    return (x + y * world.len());
        }
        for (j = 0; j < i; j++) {
            y--;
            if (world.is_visible(x, y))
                if ( is_bare_area(x, y, size) )
                    return (x + y * world.len());
        }
        i++;
        for (j = 0; j < i; j++) {
            x++;
            if (world.is_visible(x, y))
                if ( is_bare_area(x, y, size) )
                    return (x + y * world.len());
        }
        for (j = 0; j < i; j++) {
            y++;
            if (world.is_visible(x, y))
                if ( is_bare_area(x, y, size) )
                    return (x + y * world.len());
        }
    }
    return (-1);
}

/** @file lincity/engine.cpp */

