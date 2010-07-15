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
#include "simulate.h"
#include "tinygettext/gettext.hpp"
#include "power.h"
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
static void bulldoze_mappoint(short fill, int x, int y);
static int is_real_river(int x, int y);

/*************** Global functions ********************/
void fire_area(int xx, int yy)
{
    /* this happens when a rocket crashes or on random_fire. */
    int x = xx;
    int y = yy;
    int size;
    if (MP_GROUP(x, y) == GROUP_WATER || MP_GROUP(x, y) == GROUP_FIRE)
        return;
    if (MP_TYPE(x, y) == CST_USED) {
        x = MP_INFO(xx, yy).int_1;
        y = MP_INFO(xx, yy).int_2;
    }
    size = MP_SIZE(x, y);

    /* Destroy the content of the building to prevent special management
     * when bulldozed.
     */

    /* Kill 'only' half of the people (bulldoze item put them in people_pool)
     * lincity NG 1.1 and previous killed all the people!
     */
    if ((MP_INFO(x, y).flags & FLAG_FIRE_COVER) != 0)
        MP_INFO(x, y).population = MP_INFO(x, y).population / 2;
    else
        MP_INFO(x, y).population = 0;
    MP_INFO(x, y).flags = 0;
    MP_INFO(x, y).int_1 = 0;
    MP_INFO(x, y).int_2 = 0;
    MP_INFO(x, y).int_3 = 0;
    MP_INFO(x, y).int_4 = 0;
    MP_INFO(x, y).int_5 = 0;
    MP_INFO(x, y).int_6 = 0;
    MP_INFO(x, y).int_7 = 0;

    /* Correctly remove buildings (substations...) and adjust count,
     * but don't count bulldoze cost
     * */
    adjust_money(+main_groups[MP_GROUP(x, y)].bul_cost);
    bulldoze_item(x, y);

    /* put fire */
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++) {
            bulldoze_mappoint(CST_FIRE_1, x + i, y + j);
            MP_GROUP(x + i, y + j) = GROUP_FIRE;
        }

    /* AL1: is it necessary ? It is the only place in lincity/. with such a call
     *  all other are in lincity-ng/.
     */
    refresh_main_screen();

    /* 
       // update transport or we get stuff put in
       // the area from connected tracks etc.
       // FIXME: AL1: NG 1.1: do the right thing and/or remove this comment
     */
}

int adjust_money(int value)
{
    total_money += value;
    print_total_money();
    mps_update();
    update_pbar(PMONEY, total_money, 0);
    refresh_pbars();            /* This could be more specific */
    return total_money;
}

int is_allowed_here(int x, int y, short type, short msg)
{
    int group = get_group_of_type( type );
    int size = main_groups[group].size; 
    int i,j;
    int has_ugw = 0;

    if( x + size > WORLD_SIDE_LEN - 1 || y + size > WORLD_SIDE_LEN - 1 || x < 1 || y < 1 )
        return false;

    switch (group) {
    case GROUP_SOLAR_POWER:
        if (total_money <= 0) {
            if (msg)
                ok_dial_box("no-credit-solar-power.mes", BAD, 0L);
            return false;
        }
        break;

    case GROUP_UNIVERSITY:
        if (total_money <= 0) {
            if (msg)
                ok_dial_box("no-credit-university.mes", BAD, 0L);
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
            if (!( MP_INFO(x + size, y + j).flags & FLAG_IS_RIVER ) ) {
                if (msg)
                    ok_dial_box("warning.mes", BAD, _("Port must be connected to river all along right side."));
                return false;
            }
        break;

    //Waterwell needs ... water :-)
    case GROUP_WATERWELL:
        for ( i = 0; i < size; i++)
            for ( j = 0; j < size; j++)
                has_ugw = has_ugw | HAS_UGWATER(x + i, y + j);
        if (!has_ugw) {
            if (msg)
                ok_dial_box("warning.mes", BAD, _("You can't build a water well here: it is all desert."));
            return false;
        }
        break;

    //Tip
    /* Don't build a tip if there has already been one.
     * This is marked permanently by setting the ore reserve to double of (max) ORE_RESERVE at tip build time
     */
    case GROUP_TIP:
        for (i = 0; i < size; i++)
            for (j = 0; j < size; j++)
                if (MP_INFO(x + i, y + j).ore_reserve > ORE_RESERVE) {
                    if (msg)
                        ok_dial_box("warning.mes", BAD, _("You can't build a tip here: this area was once a landfill"));
                    return false;
                }
	    break;
    //Oremine
    /* Don't allow new mines on old mines or old tips */
    /* GCS: mines over old mines is OK if there is enough remaining 
     *  ore, as is the case when there is partial overlap. */
    case GROUP_OREMINE:
        {

            for (i = 0; i < size; i++) 
                for (j = 0; j < size; j++)
                    if (MP_INFO(x + i, y + j).ore_reserve > ORE_RESERVE) {
                        if (msg)
                            ok_dial_box("warning.mes", BAD, _("You can't build a mine here: This area was once a landfill"));
                        return false; //previous tip
                    }

            int total_ore = 0;
            for (i = 0; i < size; i++) 
                for (j = 0; j < size; j++)
                    total_ore += MP_INFO(x + i, y + j).ore_reserve;

            if (total_ore < MIN_ORE_RESERVE_FOR_MINE) {
                if (msg)
                    ok_dial_box("warning.mes", BAD, _("You can't build a mine here: there is no ore left at this site"));
                return false; // not enought ore
            }
        }
	break;
	
    //Parkland
    case GROUP_PARKLAND:
        if (!HAS_UGWATER(x, y)) {
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

    //Substations and Windmills
    case GROUP_SUBSTATION:
    case GROUP_WINDMILL:
        if (numof_substations >= MAX_NUMOF_SUBSTATIONS) {
            if (msg)
                ok_dial_box("warning.mes", BAD, _("Too many substations + windmills. You cannot build one more"));
            return false;
        }
        break;

    //Market
    case GROUP_MARKET:
        if (numof_markets >= MAX_NUMOF_MARKETS) {
            if (msg)
                ok_dial_box("warning.mes", BAD, _("Too many markets. You cannot build one more"));
            return false;
        }
        break;

    //Other cases
    }
    return true;
}

int place_item(int x, int y, short type)
{
    int i, j;
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

    MP_DATE(x,y) = total_time;
    MP_TECH(x,y) = tech_level;
    MP_ANIM(x,y) = real_time;

    size = main_groups[group].size;
    if (last_warning_message_group != group)
        msg = true;
    else
        msg =false;
  
    if (GROUP_IS_RESIDENCE(group))
        group=GROUP_RESIDENCE_LL;

    /* You can't build here because it's forbidden or impossible or not enough money */
    if (!is_allowed_here(x,y,type,msg)) {
            last_warning_message_group = group;
            return -2;
    } else
        last_warning_message_group = 0;

    switch (group) {
    case GROUP_RESIDENCE_LL:
        /* all residences are treated. local variable 'group' is redefined to do so.*/
        /* int_2 is date of last starvation. Needed for correct display */
        MP_INFO(x, y).int_2 = MP_DATE(x,y);
        break;

    // bridge is build or upgraded as track/road/rail so set the group bridge according to water
    case GROUP_TRACK:
        if (MP_GROUP(x, y) == GROUP_WATER) {
            type = CST_TRACK_BRIDGE_LR;
            selected_module_cost = get_group_cost(GROUP_TRACK_BRIDGE);
        } else
            // return to normal cost in case of dragging
            selected_module_cost = get_group_cost(GROUP_TRACK);
        MP_INFO(x, y).flags |= FLAG_IS_TRANSPORT;
        break;

    case GROUP_ROAD:
        if (MP_GROUP(x, y) == GROUP_WATER || MP_GROUP(x, y) == GROUP_TRACK_BRIDGE) {
            type = CST_ROAD_BRIDGE_LR;
            selected_module_cost = get_group_cost(GROUP_ROAD_BRIDGE);
        } else
            selected_module_cost = get_group_cost(GROUP_ROAD);
        MP_INFO(x, y).flags |= FLAG_IS_TRANSPORT;
        break;

    case GROUP_RAIL:
        if (MP_GROUP(x, y) == GROUP_WATER || MP_GROUP(x, y) == GROUP_TRACK_BRIDGE
                || MP_GROUP(x, y) == GROUP_ROAD_BRIDGE) {
            type = CST_RAIL_BRIDGE_LR;
            selected_module_cost = get_group_cost(GROUP_RAIL_BRIDGE);
        } else
            selected_module_cost = get_group_cost(GROUP_RAIL);
        MP_INFO(x, y).flags |= FLAG_IS_TRANSPORT;
        break;

    case GROUP_SUBSTATION:
        add_a_substation (x, y);
        break;

    case GROUP_WINDMILL:
        add_a_substation (x, y);
        MP_INFO(x, y).int_1 = (int)(WINDMILL_POWER + (((double)MP_TECH(x, y) * WINDMILL_POWER) / MAX_TECH_LEVEL));
        /* Make sure that the correct windmill graphic shows up */
        if (tech_level > MODERN_WINDMILL_TECH) {
            type = CST_WINDMILL_1_R;
            MP_INFO(x, y).int_2 = true;
        } else {
            type = CST_WINDMILL_1_W;
            MP_INFO(x, y).int_2 = false;
        }
        break;

    case (GROUP_COAL_POWER):
        MP_INFO(x, y).int_1 = (int)(POWERS_COAL_OUTPUT + (((double)MP_TECH(x, y) * POWERS_COAL_OUTPUT)
                                                          / MAX_TECH_LEVEL));
        break;

    case (GROUP_SOLAR_POWER):
        MP_INFO(x, y).int_1 = (int)(POWERS_SOLAR_OUTPUT + (((double)MP_TECH(x, y) * POWERS_SOLAR_OUTPUT)
                                                           / MAX_TECH_LEVEL));
        break;

    case GROUP_COMMUNE:
        numof_communes++;
        break;

    case GROUP_MARKET:
        add_a_market(x,y);
        MP_INFO(x, y).flags += (FLAG_MB_FOOD | FLAG_MB_JOBS
                                | FLAG_MB_COAL | FLAG_MB_ORE | FLAG_MB_STEEL
                                | FLAG_MB_GOODS | FLAG_MS_FOOD | FLAG_MS_JOBS
                                | FLAG_MS_COAL | FLAG_MS_GOODS | FLAG_MS_ORE | FLAG_MS_STEEL);
        break;

    case GROUP_TIP:
        /* To prevent building a tip if there has already been one we
         * mark the spot permanently by "doubling" the ore reserve */
        for (i = 0; i < size; i++)
            for (j = 0; j < size; j++)
                MP_INFO(x + i, y + j).ore_reserve = ORE_RESERVE * 2;
        break;

    case GROUP_WATERWELL:
        do_waterwell_cover(x,y);
        numof_waterwell++; //AL1: unused so far.
        break;

    } /* end case */

    set_mappoint(x, y, type);
    desert_frontier(x - 1, y - 1, size + 2, size + 2);

    if (group == GROUP_RIVER)
        connect_rivers();

    connect_transport(x - 2, y - 2, x + size + 1, y + size + 1);

    adjust_money(-selected_module_cost);
    map_power_grid();
    return 0;
}

int bulldoze_item(int x, int y)
{
    int g, size;

    if (MP_TYPE(x, y) == CST_USED) {
        /* This is considered "improper" input.  Silently ignore. */
#ifdef DEBUG
        fprintf(stderr, " try to improperly bulldoze_item CST_USED\n");
#endif
        return -1;
    }

    size = MP_SIZE(x, y);
    g = MP_GROUP(x, y);
    people_pool += MP_INFO(x, y).population;

    if (g == GROUP_DESERT) {
        /* Nothing to do. */
        return -1;
    } else if (g == GROUP_SHANTY) {
        remove_a_shanty(x, y);
        adjust_money(-GROUP_SHANTY_BUL_COST);
    } else if (g == GROUP_FIRE) {
        if (MP_INFO(x, y).int_2 >= FIRE_LENGTH)
            return -1;          /* Can't bulldoze ? */
        MP_INFO(x, y).int_2 = FIRE_LENGTH + 1;
        MP_TYPE(x, y) = CST_FIRE_DONE1;
        MP_GROUP(x, y) = GROUP_BURNT;
        adjust_money(-GROUP_BURNT_BUL_COST);
    } else {
        adjust_money(-main_groups[g].bul_cost);

        if (g == GROUP_COMMUNE)
            numof_communes--;

        if (g == GROUP_MARKET)
            remove_a_market(x, y);

        if (g == GROUP_SUBSTATION || g == GROUP_WINDMILL)
            remove_a_substation(x, y);

        if (g == GROUP_TRACK_BRIDGE || g == GROUP_ROAD_BRIDGE || g == GROUP_RAIL_BRIDGE){
            do_bulldoze_area(CST_WATER, x, y);
	    return size;
	}

        /* keep compatibility for saving pre_waterwell loaded game */
        if (use_waterwell)
            do_bulldoze_area(CST_DESERT, x, y);
        else
            do_bulldoze_area(CST_GREEN, x, y);

        if (g == GROUP_OREMINE) {
            int i, j;
            for (j = 0; j < 4; j++)
                for (i = 0; i < 4; i++)
                    if (MP_INFO(x + i, y + j).ore_reserve < ORE_RESERVE / 2)
                        do_bulldoze_area(CST_WATER, x + i, y + j);
        }
    }

    /* Tell mps about it, in case its selected */
    mps_update();
    return size;                /* No longer used... */
}

void do_bulldoze_area(short fill, int xx, int yy)
{
    int size, x, y;
    if (MP_TYPE(xx, yy) == CST_USED) {
        x = MP_INFO(xx, yy).int_1;
        y = MP_INFO(xx, yy).int_2;
    } else {
        x = xx;
        y = yy;
    }
    size = MP_SIZE(x, y);
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            bulldoze_mappoint(fill, x + i, y + j);

    desert_frontier(x - 1, y - 1, size + 2, size + 2);
    connect_transport(x - 2, y - 2, x + size + 1, y + size + 1);

}

void do_pollution()
{
    int x, p;
    int *pol = &map.pollution[0][0];

    /* Kill pollution from top edge of map */
    do {
        if (*pol > 0)
            *pol /= POL_DIV;
    } while (++pol < &map.pollution[1][0]);

    x = 1;
    do {
        /* Kill some pollution from left edge of map */
        if (*pol++ > 0)
            *(pol - 1) /= POL_DIV;
        do {
            if (*pol > 10) {
                p = *pol / 16;
                *pol -= p;
                switch (rand() % 11) {  /* prevailing wind is *from* SW */
                case 0:
                case 1:        /* up */
                case 2:
                    *(pol - 1) += p;
                    break;
                case 3:
                case 4:        /* right */
                case 5:
                    *(pol + WORLD_SIDE_LEN) += p;
                    break;
                case 6:        /* down */
                case 7:
                    *(pol + 1) += p;
                    break;
                case 8:        /* left */
                case 9:
                    *(pol - WORLD_SIDE_LEN) += p;
                    break;
                case 10:
                    *pol += p - 2;
                    break;
                }
            }
        } while (++pol < &map.pollution[x][WORLD_SIDE_LEN - 1]);
        /* Kill some pollution from right edge of map */
        if (*pol > 0)
            *pol /= POL_DIV;
        ++x;
    }
    while (++pol < &map.pollution[WORLD_SIDE_LEN - 1][0]);

    /* Kill pollution from bottom edge of map */
    do {
        if (*pol > 0)
            *pol /= POL_DIV;
    } while (++pol < &map.pollution[WORLD_SIDE_LEN][0]);
}

void do_fire_health_cricket_power_cover(void)
{
    int x, y,m;
    /* Clear the flag */
    m = 0xffffffff - (FLAG_FIRE_COVER | FLAG_HEALTH_COVER
                      | FLAG_CRICKET_COVER | FLAG_WATERWELL_COVER);
    for (y = 0; y < WORLD_SIDE_LEN; y++)
        for (x = 0; x < WORLD_SIDE_LEN; x++)
            MP_INFO(x, y).flags &= m;

    /* Check cover */
    for (y = 0; y < WORLD_SIDE_LEN; y++)
        for (x = 0; x < WORLD_SIDE_LEN; x++) {
            /*  The next few lines need changing to test for */
            /*  the group if these areas are animated. */

            if (MP_GROUP(x, y) == GROUP_FIRESTATION)
                do_fire_cover(x, y);
            else if (MP_TYPE(x, y) == CST_HEALTH)
                do_health_cover(x, y);
            else if (MP_GROUP(x, y) == GROUP_CRICKET)
                do_cricket_cover(x, y);
            else if (MP_GROUP(x, y) == GROUP_WATERWELL)
                do_waterwell_cover(x, y);
        }
}

void do_random_fire(int x, int y, int pwarning)
{                               /* well random if x=y=-1 */
    int xx, yy;
    if (x == -1 && y == -1) {
        x = rand() % WORLD_SIDE_LEN;
        y = rand() % WORLD_SIDE_LEN;
    } else {
        if (x < 0 || x >= WORLD_SIDE_LEN || y < 0 || y >= WORLD_SIDE_LEN)
            return;
    }
    if (MP_TYPE(x, y) == CST_USED) {
        xx = MP_INFO(x, y).int_1;
        yy = MP_INFO(x, y).int_2;
        x = xx;
        y = yy;
    }
    xx = rand() % 100;
    if (xx >= (main_groups[MP_GROUP(x, y)].fire_chance))
        return;
    if ((MP_INFO(x, y).flags & FLAG_FIRE_COVER) != 0)
        return;
    if (pwarning) {
        if (MP_GROUP(x, y) == GROUP_POWER_LINE)
            ok_dial_box("fire.mes", BAD, _("It's at a power line."));
        else if (MP_GROUP(x, y) == GROUP_SOLAR_POWER)
            ok_dial_box("fire.mes", BAD, _("It's at a solar power station."));
        else if (MP_GROUP(x, y) == GROUP_SUBSTATION)
            ok_dial_box("fire.mes", BAD, _("It's at a substation."));
        else if (MP_GROUP_IS_RESIDENCE(x, y))
            ok_dial_box("fire.mes", BAD, _("It's at a residential area."));
        else if (MP_GROUP(x, y) == GROUP_ORGANIC_FARM)
            ok_dial_box("fire.mes", BAD, _("It's at a farm."));
        else if (MP_GROUP(x, y) == GROUP_MARKET)
            ok_dial_box("fire.mes", BAD, _("It's at a market."));
        else if (MP_GROUP(x, y) == GROUP_TRACK)
            ok_dial_box("fire.mes", BAD, _("It's at a track."));
        else if (MP_GROUP(x, y) == GROUP_COALMINE)
            ok_dial_box("fire.mes", BAD, _("It's at a coal mine."));
        else if (MP_GROUP(x, y) == GROUP_RAIL)
            ok_dial_box("fire.mes", BAD, _("It's at a railway."));
        else if (MP_GROUP(x, y) == GROUP_COAL_POWER)
            ok_dial_box("fire.mes", BAD, _("It's at a coal power station."));
        else if (MP_GROUP(x, y) == GROUP_ROAD)
            ok_dial_box("fire.mes", BAD, _("It's at a road."));
        else if (MP_GROUP(x, y) == GROUP_INDUSTRY_L)
            ok_dial_box("fire.mes", BAD, _("It's at light industry."));
        else if (MP_GROUP(x, y) == GROUP_UNIVERSITY)
            ok_dial_box("fire.mes", BAD, _("It's at a university."));
        else if (MP_GROUP(x, y) == GROUP_COMMUNE)
            ok_dial_box("fire.mes", BAD, _("It's at a commune."));
        else if (MP_GROUP(x, y) == GROUP_TIP)
            ok_dial_box("fire.mes", BAD, _("It's at a tip."));
        else if (MP_GROUP(x, y) == GROUP_PORT)
            ok_dial_box("fire.mes", BAD, _("It's at a port."));
        else if (MP_GROUP(x, y) == GROUP_INDUSTRY_H)
            ok_dial_box("fire.mes", BAD, _("It's at a steel works."));
        else if (MP_GROUP(x, y) == GROUP_RECYCLE)
            ok_dial_box("fire.mes", BAD, _("It's at a recycle centre."));
        else if (MP_GROUP(x, y) == GROUP_HEALTH)
            ok_dial_box("fire.mes", BAD, _("It's at a health centre."));
        else if (MP_GROUP(x, y) == GROUP_ROCKET)
            ok_dial_box("fire.mes", BAD, _("It's at a rocket site."));
        else if (MP_GROUP(x, y) == GROUP_WINDMILL)
            ok_dial_box("fire.mes", BAD, _("It's at a windmill."));
        else if (MP_GROUP(x, y) == GROUP_SCHOOL)
            ok_dial_box("fire.mes", BAD, _("It's at a school."));
        else if (MP_GROUP(x, y) == GROUP_BLACKSMITH)
            ok_dial_box("fire.mes", BAD, _("It's at a blacksmith."));
        else if (MP_GROUP(x, y) == GROUP_MILL)
            ok_dial_box("fire.mes", BAD, _("It's at a mill."));
        else if (MP_GROUP(x, y) == GROUP_POTTERY)
            ok_dial_box("fire.mes", BAD, _("It's at a pottery."));
        else if (MP_GROUP(x, y) == GROUP_FIRESTATION)
            ok_dial_box("fire.mes", BAD, _("It's at a fire station!!!."));
        else if (MP_GROUP(x, y) == GROUP_CRICKET)
            ok_dial_box("fire.mes", BAD, _("It's at a sports field!!!."));
        else if (MP_GROUP(x, y) == GROUP_SHANTY)
            ok_dial_box("fire.mes", BAD, _("It's at a shanty town."));
        else
            ok_dial_box("fire.mes", BAD, _("UNKNOWN!"));
    }
    fire_area(x, y);
}

void do_daily_ecology()
{
    for (int x = 0; x < WORLD_SIDE_LEN; x++)
        for (int y = 0; y < WORLD_SIDE_LEN; y++) {
            /* approximately 3 monthes needed to turn bulldoze area into green */
            if (MP_GROUP(x, y) == GROUP_DESERT && HAS_UGWATER(x, y)
                                        && rand() % 300 == 1) {
                do_bulldoze_area(CST_GREEN, x, y);
            }
        }
    //TODO: depending on water, green can become trees
    //      pollution can make desert 
    //      etc ...
}

void connect_rivers(void)
{
    int x, y, count;
    count = 1;
    while (count > 0) {
        count = 0;
        for (y = 0; y < WORLD_SIDE_LEN; y++)
            for (x = 0; x < WORLD_SIDE_LEN; x++) {
                if (is_real_river(x, y) == 1) {
                    if (is_real_river(x - 1, y) == -1) {
                        MP_INFO(x - 1, y).flags |= FLAG_IS_RIVER;
                        count++;
                    }
                    if (is_real_river(x, y - 1) == -1) {
                        MP_INFO(x, y - 1).flags |= FLAG_IS_RIVER;
                        count++;
                    }
                    if (is_real_river(x + 1, y) == -1) {
                        MP_INFO(x + 1, y).flags |= FLAG_IS_RIVER;
                        count++;
                    }
                    if (is_real_river(x, y + 1) == -1) {
                        MP_INFO(x, y + 1).flags |= FLAG_IS_RIVER;
                        count++;
                    }
                }
            }
    }
}

/* Feature: coal survey should vary in price and accuracy with technology */
void do_coal_survey(void)
{
    if (coal_survey_done == 0) {
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
    if (originx + w >= WORLD_SIDE_LEN) {
        w = WORLD_SIDE_LEN - originx;
    }
    if (originy + h >= WORLD_SIDE_LEN) {
        h = WORLD_SIDE_LEN - originy;
    }

    for (x = originx; x < originx + w; x++) {
        for (y = originy; y < originy + h; y++) {
            if (MP_GROUP(x,y) == GROUP_DESERT) {
                mask = 0;
                /* up -- (ThMO) */
                if (MP_GROUP(x, y - 1) == GROUP_DESERT)
                    mask |= 8;

                /* left -- (ThMO) */
                if (MP_GROUP(x - 1, y) == GROUP_DESERT)
                    mask |= 4;

                /* right -- (ThMO) */
                if (MP_GROUP(x + 1, y) == GROUP_DESERT)
                    mask |= 2;

                /* down -- (ThMO) */
                if (MP_GROUP(x, y + 1) == GROUP_DESERT)
                    ++mask;
                MP_TYPE(x, y) = desert_table[mask];
            }
        }
    }
}
/************ Private functions ***************************/
static void bulldoze_mappoint(short fill, int x, int y)
{
    /* bulldoze preserve underground resources */
    MP_TYPE(x, y) = fill;
    MP_GROUP(x, y) = get_group_of_type(fill);
    if (MP_GROUP(x, y) < 0)
        MP_GROUP(x, y) = GROUP_BARE;
    MP_INFO(x, y).population = 0;
    MP_INFO(x, y).flags &= FLAG_HAS_UNDERGROUND_WATER | FLAG_WATERWELL_COVER | FLAG_CRICKET_COVER | FLAG_HEALTH_COVER | FLAG_FIRE_COVER;
    MP_INFO(x, y).int_1 = 0;
    MP_INFO(x, y).int_2 = 0;
    MP_INFO(x, y).int_3 = 0;
    MP_INFO(x, y).int_4 = 0;
    MP_INFO(x, y).int_5 = 0;
    MP_INFO(x, y).int_6 = 0;
    MP_INFO(x, y).int_7 = 0;
}

static int is_real_river(int x, int y)
{
    /* returns zero if not water at all or if out of bounds. */
    if (x < 0 || x >= WORLD_SIDE_LEN || y < 0 || y >= WORLD_SIDE_LEN)
        return (0);
    if (MP_GROUP(x, y) != GROUP_WATER)
        return (0);
    if (MP_INFO(x, y).flags & FLAG_IS_RIVER)
        return (1);
    return (-1); // only water, not river
}

