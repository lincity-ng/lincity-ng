/* ---------------------------------------------------------------------- *
 * src/lincity/modules/track_road_rail.hpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 * Copyright (C) 2022-2025 David Bears <dbear4q@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#ifndef __LINCITYNG_LINCITY_MODULES_TRACKROADRAIL_HPP__
#define __LINCITYNG_LINCITY_MODULES_TRACKROADRAIL_HPP__

#include <array>                    // for array
#include <list>                     // for list
#include <string>                   // for string

#include "lincity/commodities.hpp"  // for CommodityRule, Commodity
#include "lincity/groups.hpp"       // for GROUP_RAIL, GROUP_RAIL_BRIDGE
#include "lincity/lintypes.hpp"     // for ConstructionGroup, Construction
#include "lincity/transport.hpp"    // for MAX_COAL_ON_RAIL, MAX_COAL_ON_ROAD

class Mps;
class World;
struct ExtraFrame;

class TransportConstructionGroup: public ConstructionGroup {
public:
    TransportConstructionGroup(
        const std::string& name,
        const std::string& name_plural,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance,
        int cost, int tech, int range
    ): ConstructionGroup(
        name, name_plural, no_credit, group, size, colour, cost_mul, bul_cost,
        fire_chance,   cost, tech, range
    ) {
        switch (group)
        {
            case GROUP_TRACK:
            case GROUP_TRACK_BRIDGE:
                commodityRuleCount[STUFF_FOOD].maxload = MAX_FOOD_ON_TRACK;
                commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_ON_TRACK;
                commodityRuleCount[STUFF_COAL].maxload = MAX_COAL_ON_TRACK;
                commodityRuleCount[STUFF_GOODS].maxload = MAX_GOODS_ON_TRACK;
                commodityRuleCount[STUFF_ORE].maxload = MAX_ORE_ON_TRACK;
                commodityRuleCount[STUFF_STEEL].maxload = MAX_STEEL_ON_TRACK;
                commodityRuleCount[STUFF_WASTE].maxload = MAX_WASTE_ON_TRACK;
                commodityRuleCount[STUFF_LOVOLT].maxload = MAX_LOVOLT_ON_TRACK;
                commodityRuleCount[STUFF_WATER].maxload = MAX_WATER_ON_TRACK;
            break;
            case GROUP_ROAD:
            case GROUP_ROAD_BRIDGE:
                commodityRuleCount[STUFF_FOOD].maxload = MAX_FOOD_ON_ROAD;
                commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_ON_ROAD;
                commodityRuleCount[STUFF_COAL].maxload = MAX_COAL_ON_ROAD;
                commodityRuleCount[STUFF_GOODS].maxload = MAX_GOODS_ON_ROAD;
                commodityRuleCount[STUFF_ORE].maxload = MAX_ORE_ON_ROAD;
                commodityRuleCount[STUFF_STEEL].maxload = MAX_STEEL_ON_ROAD;
                commodityRuleCount[STUFF_WASTE].maxload = MAX_WASTE_ON_ROAD;
                commodityRuleCount[STUFF_LOVOLT].maxload = MAX_LOVOLT_ON_ROAD;
                commodityRuleCount[STUFF_WATER].maxload = MAX_WATER_ON_ROAD;
            break;
            case GROUP_RAIL:
            case GROUP_RAIL_BRIDGE:
                commodityRuleCount[STUFF_FOOD].maxload = MAX_FOOD_ON_RAIL;
                commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_ON_RAIL;
                commodityRuleCount[STUFF_COAL].maxload = MAX_COAL_ON_RAIL;
                commodityRuleCount[STUFF_GOODS].maxload = MAX_GOODS_ON_RAIL;
                commodityRuleCount[STUFF_ORE].maxload = MAX_ORE_ON_RAIL;
                commodityRuleCount[STUFF_STEEL].maxload = MAX_STEEL_ON_RAIL;
                commodityRuleCount[STUFF_WASTE].maxload = MAX_WASTE_ON_RAIL;
                commodityRuleCount[STUFF_LOVOLT].maxload = MAX_LOVOLT_ON_RAIL;
                commodityRuleCount[STUFF_WATER].maxload = MAX_WATER_ON_RAIL;
            break;
        } // end switch group
        commodityRuleCount[STUFF_FOOD].take = true;
        commodityRuleCount[STUFF_FOOD].give = true;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = true;
        commodityRuleCount[STUFF_COAL].take = true;
        commodityRuleCount[STUFF_COAL].give = true;
        commodityRuleCount[STUFF_GOODS].take = true;
        commodityRuleCount[STUFF_GOODS].give = true;
        commodityRuleCount[STUFF_ORE].take = true;
        commodityRuleCount[STUFF_ORE].give = true;
        commodityRuleCount[STUFF_STEEL].take = true;
        commodityRuleCount[STUFF_STEEL].give = true;
        commodityRuleCount[STUFF_WASTE].take = true;
        commodityRuleCount[STUFF_WASTE].give = true;
        commodityRuleCount[STUFF_LOVOLT].take = true;
        commodityRuleCount[STUFF_LOVOLT].give = true;
        commodityRuleCount[STUFF_WATER].take = true;
        commodityRuleCount[STUFF_WATER].give = true;
    }
    // overriding method that creates a transport tile
    virtual Construction *createConstruction(World& world) override;
    virtual void placeItem(World& world, MapPoint point) override;
};

extern TransportConstructionGroup trackConstructionGroup, roadConstructionGroup, railConstructionGroup;
extern TransportConstructionGroup trackbridgeConstructionGroup, roadbridgeConstructionGroup, railbridgeConstructionGroup;

//Dummies for counting SubTypes of Transport
class Track{};
class Road{};
class Rail{};
class TrackBridge{};
class RoadBridge{};
class RailBridge{};

class Transport : public Construction {
public:
    Transport(World& world, ConstructionGroup *cstgrp);
    ~Transport();
    virtual void update() override;
    virtual void report(Mps& mps, bool production) const override;
    virtual void animate(unsigned long real_time) override;
    virtual void playSound(); //override random sound
    virtual bool canPlaceVehicle();
    virtual void init_resources() override;
    virtual void place(MapPoint point) override;
    virtual void detach() override;
    std::array<int, STUFF_COUNT> trafficCount;
    void list_traffic(Mps& mps) const;
    int anim;
    bool start_burning_waste;
    std::list<ExtraFrame>::iterator waste_fire_frit;
    int waste_fire_anim;
};

#endif // __LINCITYNG_LINCITY_MODULES_TRACKROADRAIL_HPP__
