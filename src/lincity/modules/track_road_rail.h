#include <assert.h>             // for assert
#include <array>                // for array
#include <iostream>             // for basic_ostream, operator<<, basic_ostr...
#include <list>                 // for _List_iterator, list
#include <map>                  // for map
#include <string>               // for char_traits, basic_string, operator<

#include "lincity/transport.h"  // for RAIL_GOODS_USED_MASK, RAIL_STEEL_USED...
#include "modules.h"            // for CommodityRule, Commodity, Counted

#ifdef DEBUG
#include <SDL.h>                // for SDL_GetKeyboardState, SDL_SCANCODE_LS...
#include <stddef.h>             // for NULL
#endif

class TransportConstructionGroup: public ConstructionGroup {
public:
    TransportConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance,
        int cost, int tech, int range
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance,
        cost, tech, range, 2/*mps_pages*/
    )
    {
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
    virtual Construction *createConstruction();
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
    Transport(ConstructionGroup *cstgrp) {
        this->constructionGroup = cstgrp;
        this->anim = 0;
        this->start_burning_waste = false;
        this->waste_fire_anim = 0;
        // register the construction as transport tile
        // disable evacuation
        //transparency is set and updated in connect_transport
        this->flags |= (FLAG_IS_TRANSPORT | FLAG_NEVER_EVACUATE);

#ifdef DEBUG
        const Uint8 *keystate = SDL_GetKeyboardState(NULL);
        if (world(x,y)->is_water() || keystate[SDL_SCANCODE_LSHIFT] || keystate[SDL_SCANCODE_RSHIFT] )
#else
        // set the constructionGroup to build bridges iff over water
        if (world(x,y)->is_water())
#endif
        {
          switch (constructionGroup->group) {
            case GROUP_TRACK:
              constructionGroup = &trackbridgeConstructionGroup;
            break;
            case GROUP_ROAD:
              constructionGroup = &roadbridgeConstructionGroup;
            break;
            case GROUP_RAIL:
              constructionGroup = &railbridgeConstructionGroup;
            break;
          }
        }
        else {
          switch (constructionGroup->group) {
            case GROUP_TRACK_BRIDGE:
              constructionGroup = &trackConstructionGroup;
            break;
            case GROUP_ROAD_BRIDGE:
              constructionGroup = &roadConstructionGroup;
            break;
            case GROUP_RAIL_BRIDGE:
              constructionGroup = &railConstructionGroup;
            break;
          }
        }

        init_resources();
        waste_fire_frit = world(x, y)->createframe();
        waste_fire_frit->resourceGroup = ResourceGroup::resMap["Fire"];
        waste_fire_frit->move_x = 0;
        waste_fire_frit->move_y = 0;
        waste_fire_frit->frame = -1;

        initialize_commodities();
        this->trafficCount = this->commodityCount;

        switch (constructionGroup->group) {
        case GROUP_ROAD:
        case GROUP_ROAD_BRIDGE:
            commodityMaxCons[STUFF_GOODS] =
              (100 - 1) / (ROAD_GOODS_USED_MASK + 1) + 1;
            commodityMaxProd[STUFF_WASTE] =
              (100 - 1) / (ROAD_GOODS_USED_MASK + 1) + 1;
            break;
        case GROUP_RAIL:
        case GROUP_RAIL_BRIDGE:
            commodityMaxCons[STUFF_GOODS] =
              (100 - 1) / (RAIL_GOODS_USED_MASK + 1) + 1;
            commodityMaxCons[STUFF_STEEL] =
              (100 - 1) / (RAIL_STEEL_USED_MASK + 1) + 1;
            commodityMaxProd[STUFF_WASTE] =
              (100 - 1) / (RAIL_GOODS_USED_MASK + 1) + 1 +
              (100 - 1) / (RAIL_STEEL_USED_MASK + 1) + 1;
            break;
        }
        commodityMaxCons[STUFF_LOVOLT] = 100 * LOVOLT_LOSS_ON_TRANSPORT;
        commodityMaxCons[STUFF_WASTE] = 100 * WASTE_BURN_ON_TRANSPORT;
    }
    ~Transport() {
        world(x,y)->killframe(waste_fire_frit);
    }
    virtual void update() override;
    virtual void report() override;
    virtual void animate() override;
    virtual void playSound(); //override random sound
    virtual bool canPlaceVehicle();
    std::array<int, STUFF_COUNT> trafficCount;
    void list_traffic( int* i);
    int anim;
    bool start_burning_waste;
    std::list<ExtraFrame>::iterator waste_fire_frit;
    int waste_fire_anim;
};
