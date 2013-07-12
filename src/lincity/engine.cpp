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
//#include "power.h"
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

int last_warning_message_group = 0;

/****** Private functions prototypes *****/
//static void bulldoze_mappoint(short fill, int x, int y);
//static int is_real_river(int x, int y);

/*************** Global functions ********************/
void fire_area(int xx, int yy)
{
    /* this happens when a rocket crashes or on random_fire. */
    int x = xx;
    int y = yy;
    int size = 1;
    if (world(x, y)->getGroup() == GROUP_WATER || world(x, y)->getGroup() == GROUP_FIRE)
        return;
    if(world(x, y)->reportingConstruction)
    {    
        x = world(xx,yy)->reportingConstruction->x;
        y = world(xx,yy)->reportingConstruction->y;
        size = world(xx,yy)->reportingConstruction->constructionGroup->size;  
    }
    do_bulldoze_area(x, y);

    /* put fire */
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {           
            fireConstructionGroup.placeItem(x+i,y+j,CST_FIRE_1);
        }
    }
    //green borders at fire are strange but consistent
    //TODO remove this and exclude fire in desert_frontier
    desert_frontier(x - 1, y - 1, size + 2, size + 2);      
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

bool is_allowed_here(int x, int y, short type, short msg)
{
    int group = get_group_of_type( type );
    int size; 
    int i,j;
    int has_ugw = 0;
    ConstructionGroup *constrGroup = NULL;
        
    if (ConstructionGroup::countConstructionGroup(group))
    {                         
        constrGroup = ConstructionGroup::getConstructionGroup(group);               
        size = constrGroup->size;
    } 
    else 
    { 
        size = main_groups[group].size; 
    }        
    if(!(world.is_visible(x,y) && world.is_visible(x + size - 1, y + size - 1)))
    {	return false;}
    
    switch (group) {
    case GROUP_SOLAR_POWER:
        if (total_money <= 0) {
            if (msg)
                ok_dial_box("no-credit-solar-power.mes", BAD, 0L);
            return false;
        }
        break;

    case GROUP_UNIVERSITY:
        if (total_money <= 0)
        {
            if (msg)
                ok_dial_box("no-credit-university.mes", BAD, 0L);
            return false;
        }
        else if ((Counted<School>::getInstanceCount()/4 - Counted<University>::getInstanceCount()) < 1)
        {
            if (msg)                
                ok_dial_box("warning.mes", BAD, "Not enough students, build more schools.");
            return false;            
        }
        break;

    case GROUP_RECYCLE:
        if (total_money <= 0) {
            if (msg)
                ok_dial_box("no-credit-recycle.mes", BAD, 0L);
            return false;
        }
        break;

    case GROUP_ROCKET:
        if (total_money <= 0) {
            if (msg)
                ok_dial_box("no-credit-rocket.mes", BAD, 0L);
            return false;
        }
        break;

        //The Harbour needs a River on the East side.
    case GROUP_PORT:
        for( j = 0; j < size; j++ )
        {
            if (!( world(x + size, y + j)->flags & FLAG_IS_RIVER ) ) 
            {
                if (msg)
                    ok_dial_box("warning.mes", BAD, _("Port must be connected to river all along right side."));
                return false;
            }
        }
        break;

    //Waterwell needs ... water :-)
    case GROUP_WATERWELL:
        for ( i = 0; i < size; i++)
            for ( j = 0; j < size; j++)
                has_ugw = has_ugw | (world(x + j,y + i)->flags & FLAG_HAS_UNDERGROUND_WATER);
        if (!has_ugw) {
            if (msg)
                ok_dial_box("warning.mes", BAD, _("You can't build a water well here: it is all desert."));
            return false;
        }
        break;
    //Oremine
    /* GCS: mines over old mines is OK if there is enough remaining 
     *  ore, as is the case when there is partial overlap. */
    case GROUP_OREMINE:
        {
            int total_ore = 0;
            for (i = 0; i < size; i++)
            {
                for (j = 0; j < size; j++)
                {
                    total_ore += world(x+j, y+i)->ore_reserve;                   
                }
            }
            if (total_ore < MIN_ORE_RESERVE_FOR_MINE) {
                if (msg) ok_dial_box("warning.mes", BAD, _("You can't build a mine here: there is no ore left at this site"));
                return false; // not enought ore
            }
        }
	break;
	
    //Parkland
    case GROUP_PARKLAND:
        if (!(world(x, y)->flags & FLAG_HAS_UNDERGROUND_WATER))
        {
            if (msg)
                ok_dial_box("warning.mes", BAD, _("You can't build a park here: it is a desert, parks need water"));
            return false;
        }
        if (total_money <= 0) {
            if (msg) 
                ok_dial_box("no-credit-parkland.mes", BAD, 0L);
            return false;
        }
        break;
    //Other cases
    }
    //double check cash
    if (constrGroup && (constrGroup->no_credit) && (total_money < 1))
    {
        if (msg)
        {
            ok_dial_box("warning.mes", BAD, "You cannot build this item on credit!");
        }
        return false;
    }
       
    return true;
}

int place_item(int x, int y, short type)
{
    int group;
    int size;
    int msg;

     group = get_group_of_type(type);
    if (group < 0) {
#ifdef DEBUG
        fprintf(stderr, "Error: group does not exist %i\n", group);
#endif
        ok_dial_box("warning.mes", BAD,
                _
                ("ERROR: group does not exist. This should not happen! Please consider filling a bug report to lincity-ng team, with the saved game and what you did :-) "));
        return -1000;
    }

    /* You can't build here because it's forbidden or impossible or not enough money */
    if (last_warning_message_group != group)
        msg = true;
    else
        msg =false;    
    if (!is_allowed_here(x,y,type,msg)) 
    {
        last_warning_message_group = group;
        return -2;
    } 
    else
        last_warning_message_group = 0;
    if(ConstructionGroup::countConstructionGroup(group))    
    {            
        ConstructionGroup::getConstructionGroup(group)->placeItem(x, y, type);
        size = world(x, y)->construction->constructionGroup->size;
        adjust_money(-world(x, y)->construction->constructionGroup->getCosts());
    } 
    else 
    {
        world(x, y)->setTerrain(type); //Treats inactive tiles and old stuctures alike
        if(type == CST_WATER)
            world(x, y)->flags |= FLAG_HAS_UNDERGROUND_WATER;
        size = main_groups[group].size;
        adjust_money(-selected_module_cost);// e.g. building water 
    }
    desert_frontier(x - 1, y - 1, size + 2, size + 2); 
    connect_rivers();
    connect_transport(x - 2, y - 2, x + size + 1, y + size + 1); 
    return 0;
}

int bulldoze_item(int x, int y)
{
    int g, size = 1;
    bool construction_found = false;
    if (world(x, y)->type == CST_USED || !world(x, y)->is_visible())
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
    }

    if (g == GROUP_DESERT) 
    {
        /* Nothing to do. */
        return -1;
    }
    else if (g == GROUP_SHANTY)
    {
        remove_a_shanty(x, y);
        adjust_money(-GROUP_SHANTY_BUL_COST);
    }
    else if (g == GROUP_FIRE)
    {
        return -1;          /* Can't bulldoze ? */
    }
    else
    {
        if (!construction_found)
        {       
            adjust_money(-main_groups[g].bul_cost);
        }
        else
        {
            adjust_money(-world(x, y)->reportingConstruction->constructionGroup->bul_cost);
        }

        if (g == GROUP_OREMINE)
        { 
            ConstructionManager::executeRequest
            (
                new OreMineDeletionRequest(world(x, y)->construction)
            );
        }
        else
        {
            do_bulldoze_area(x, y);
        }

    }
    /* Tell mps about it, in case its selected */
    mps_update();
    return size;                /* No longer used... */
}

void do_bulldoze_area(int x, int y) //arg1 was short fill
{

    if (world(x, y)->construction)
    {      
        ConstructionManager::executeRequest
        (
            new ConstructionDeletionRequest(world(x, y)->construction)
        );
    }
    else
    {
        if (world(x, y)->is_water())
        {
            world(x, y)->type = CST_GREEN;
            world(x, y)->group = GROUP_BARE;
        }
        else
        {
            world(x, y)->type = CST_DESERT;
            world(x, y)->group = GROUP_DESERT;
        }
        if (world(x, y)->reportingConstruction)
        {
            ok_dial_box("fire.mes", BAD, _("ups, Bulldozer found a dangling reportingConstruction"));
        }
        //Here size is always 1
        connect_rivers();    
        desert_frontier(x - 1, y - 1, 1 + 2, 1 + 2);
        connect_transport(x - 2, y - 2, x + 1 + 1, y + 1 + 1);
    }  
}

void do_pollution()
{
    
    const int len = world.len();
    const int area = len * len; 	
    //kill pollution from edges of map    
    for(int x = 0; x < world.len(); x++)
    {
        world(x, 0)->pollution /= POL_DIV; //top
        world(0, x)->pollution /= POL_DIV; //left
        world(x, world.len() - 1)->pollution /= POL_DIV; //bottom
        world(world.len() - 1, x)->pollution /= POL_DIV; //right
    }
    //diffuse pollution inside the map    
    for (int index = 0; index < area; ++index)   
    {
		int x = index % len;
		int y = index / len;
		if (world(x, y)->pollution > 10 && 
			world.is_visible(x,y))
		{
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
	total_pollution = 0;
	for (int index = 0; index < area; ++index)
	{
		int x = index % len;
		int y = index / len;
		total_pollution += world(x,y)->pollution;	
	}
}

void do_fire_health_cricket_power_cover(void)
{
    int x, y, m;
    // Clear the mapflags 
    m = ~(FLAG_FIRE_COVER | FLAG_HEALTH_COVER | FLAG_CRICKET_COVER | FLAG_MARKET_COVER ); //| FLAG_WATERWELL_COVER
    for (y = 0; y < world.len(); y++)
        for (x = 0; x < world.len(); x++)
        {
            world(x, y)->flags &= m;
        }
    // Check cover
    for (y = 0; y < world.len(); y++)
    {
        for (x = 0; x < world.len(); x++)
        {
            switch (world(x, y)->getTopGroup())
            {
                case GROUP_FIRESTATION:
                    static_cast<FireStation *>(world(x, y)->construction)->cover();
                break;
                case GROUP_HEALTH:
                    static_cast<HealthCentre *>(world(x, y)->construction)->cover();
                break;
                case GROUP_CRICKET:
                    static_cast<Cricket *>(world(x, y)->construction)->cover();
                break;
/*
                case GROUP_WATERWELL:
                    static_cast<Waterwell *>(world(x, y)->construction)->cover(); 
                break;
*/
                case GROUP_MARKET:
                    static_cast<Market *>(world(x, y)->construction)->cover(); 
                break;
            }//endswitch        
        }//end for x            
    }//end for y
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
        if (xx >= (main_groups[world(x, y)->getGroup()].fire_chance))
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
            if ((world(x, y)->getGroup() == GROUP_DESERT)
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


void connect_rivers(void)
{
    int x, y, count;
    count = 1;
    while (count > 0)
    {
        count = 0;
        for (y = 0; y < world.len(); y++)
            for (x = 0; x < world.len(); x++)
            {
                if (world(x, y)->is_river()) 
                {
                    if (x > 0 && world(x-1, y)->is_lake()) 
                    {
                        world(x-1, y)->flags |= FLAG_IS_RIVER;
                        count++;
                    }
                    if (y > 0 && world(x, y-1)->is_lake())
                    {
                        world(x, y-1)->flags |= FLAG_IS_RIVER;
                        count++;
                    }
                    if (x+1 < world.len() && world(x+1, y)->is_lake())
                    {
                        world(x+1, y)->flags |= FLAG_IS_RIVER;
                        count++;
                    }
                    if (y+1 < world.len() && world(x, y+1)->is_lake())
                    {
                        world(x, y+1)->flags |= FLAG_IS_RIVER;
                        count++;
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
    int x, y, mask;

    static const short desert_table[16] = {
        CST_DESERT_0, CST_DESERT_1D, CST_DESERT_1R, CST_DESERT_2RD,
        CST_DESERT_1L, CST_DESERT_2LD, CST_DESERT_2LR, CST_DESERT_3LRD,
        CST_DESERT_1U, CST_DESERT_2UD, CST_DESERT_2RU, CST_DESERT_3RUD,
        CST_DESERT_2LU, CST_DESERT_3LUD, CST_DESERT_3LRU, CST_DESERT
    };

#if	FLAG_LEFT != 1 || FLAG_UP != 2 || FLAG_RIGHT != 4 || FLAG_DOWN != 8
#error  desert_frontier(): you loose
#error  the algorithm depends on proper flag settings -- (ThMO)
#endif

    /* Adjust originx,originy,w,h to proper range */
    if (originx <= 0) {
        w -= 1 - originx;
        originx = 1;
    }
    if (originy <= 0) {
        h -= 1 - originy;
        originy = 1;
    }
    if (originx + w >= world.len()) {
        w = world.len() - originx;
    }
    if (originy + h >= world.len()) {
        h = world.len() - originy;
    }

    for (x = originx; x < originx + w; x++) {
        for (y = originy; y < originy + h; y++) {
            if (world(x, y)->getGroup() == GROUP_DESERT) 
            {
                mask = 0;
                /* up -- (ThMO) */
                if (check_group(x, y - 1) == GROUP_DESERT)
                    mask |= 8;

                /* left -- (ThMO) */
                if (check_group(x - 1, y) == GROUP_DESERT)
                    mask |= 4;

                /* right -- (ThMO) */
                if (check_group(x + 1, y) == GROUP_DESERT)
                    mask |= 2;

                /* down -- (ThMO) */
                if (check_group(x, y + 1) == GROUP_DESERT)
                    ++mask;
                //MP_TYPE(x, y) = desert_table[mask];
                world(x, y)->type = desert_table[mask];
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

