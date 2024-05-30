#define GROUP_SHANTY_COLOUR    (red(22))
#define GROUP_SHANTY_COST      0   /* Unused */
#define GROUP_SHANTY_COST_MUL  1   /* Unused */
#define GROUP_SHANTY_BUL_COST  100000
#define GROUP_SHANTY_TECH      0   /* Unused */
#define GROUP_SHANTY_FIREC     25
#define GROUP_SHANTY_RANGE     0
#define GROUP_SHANTY_SIZE      2


#define SHANTY_MIN_PP     150
#define SHANTY_POP        50
#define DAYS_BETWEEN_SHANTY (NUMOF_DAYS_IN_MONTH * 1)
#define SHANTY_WASTE_BURN_DAYS 10;

#define SHANTY_GET_FOOD   50
#define SHANTY_GET_LABOR   5
#define SHANTY_GET_GOODS  50
#define SHANTY_GET_COAL   1
#define SHANTY_GET_ORE    10
#define SHANTY_GET_STEEL  1
#define SHANTY_PUT_WASTE  1
#define SHANTY_GET_LOVOLT    50

#define MAX_FOOD_AT_SHANTY  (SHANTY_GET_FOOD * 20)
#define MAX_LABOR_AT_SHANTY  (SHANTY_GET_LABOR * 20)
#define MAX_GOODS_AT_SHANTY (SHANTY_GET_GOODS * 20)
#define MAX_COAL_AT_SHANTY  (SHANTY_GET_COAL * 20)
#define MAX_ORE_AT_SHANTY   (SHANTY_GET_ORE * 20)
#define MAX_STEEL_AT_SHANTY (SHANTY_GET_STEEL * 20)
#define MAX_WASTE_AT_SHANTY (SHANTY_PUT_WASTE * 20 + MAX_GOODS_AT_SHANTY / 3)
#define MAX_LOVOLT_AT_SHANTY   (SHANTY_GET_LOVOLT * 20)

#include <array>                    // for array
#include <list>                     // for _List_iterator, list
#include <map>                      // for map
#include <string>                   // for basic_string, operator<

#include "modules.h"

class ShantyConstructionGroup: public ConstructionGroup {
public:
    ShantyConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance,
        int cost, int tech, int range
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance,
        cost, tech, range, 2/*mps_pages*/
    ) {
        commodityRuleCount[STUFF_FOOD].maxload = MAX_FOOD_AT_SHANTY;
        commodityRuleCount[STUFF_FOOD].take = true;
        commodityRuleCount[STUFF_FOOD].give = false;
        commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_AT_SHANTY;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = false;
        commodityRuleCount[STUFF_GOODS].maxload = MAX_GOODS_AT_SHANTY;
        commodityRuleCount[STUFF_GOODS].take = true;
        commodityRuleCount[STUFF_GOODS].give = false;
        commodityRuleCount[STUFF_COAL].maxload = MAX_COAL_AT_SHANTY;
        commodityRuleCount[STUFF_COAL].take = true;
        commodityRuleCount[STUFF_COAL].give = false;
        commodityRuleCount[STUFF_ORE].maxload = MAX_ORE_AT_SHANTY;
        commodityRuleCount[STUFF_ORE].take = true;
        commodityRuleCount[STUFF_ORE].give = false;
        commodityRuleCount[STUFF_STEEL].maxload = MAX_STEEL_AT_SHANTY;
        commodityRuleCount[STUFF_STEEL].take = true;
        commodityRuleCount[STUFF_STEEL].give = false;
        commodityRuleCount[STUFF_WASTE].maxload = MAX_WASTE_AT_SHANTY;
        commodityRuleCount[STUFF_WASTE].take = false;
        commodityRuleCount[STUFF_WASTE].give = true;
        commodityRuleCount[STUFF_LOVOLT].maxload = MAX_LOVOLT_AT_SHANTY;
        commodityRuleCount[STUFF_LOVOLT].take = true;
        commodityRuleCount[STUFF_LOVOLT].give = false;
    }
    // overriding method that creates a Shanty
    virtual Construction *createConstruction(int x, int y);
};

extern ShantyConstructionGroup shantyConstructionGroup;

class Shanty: public RegisteredConstruction<Shanty> { // Shanty inherits from Construction
public:
    Shanty(int x, int y, ConstructionGroup *cstgrp): RegisteredConstruction<Shanty>(x, y)
    {
        this->constructionGroup = cstgrp;
        init_resources();
        waste_fire_frit = world(x, y)->createframe();
        waste_fire_frit->resourceGroup = ResourceGroup::resMap["Fire"];
        waste_fire_frit->move_x = 0;
        waste_fire_frit->move_y = 0;
        waste_fire_frit->frame = -1;
        initialize_commodities();
        this->flags |= FLAG_NEVER_EVACUATE;
        this->anim = 0;
        this->start_burning_waste = false;
        this->waste_fire_anim = 0;

        commodityMaxProd[STUFF_WASTE] = 100 *
          (SHANTY_PUT_WASTE * 2 + SHANTY_GET_GOODS / 3);
        commodityMaxCons[STUFF_FOOD] = 100 * SHANTY_GET_FOOD;
        commodityMaxCons[STUFF_LABOR] = 100 * SHANTY_GET_LABOR;
        commodityMaxCons[STUFF_GOODS] = 100 * SHANTY_GET_GOODS;
        commodityMaxCons[STUFF_COAL] = 100 * SHANTY_GET_COAL;
        commodityMaxCons[STUFF_ORE] = 100 * SHANTY_GET_ORE;
        commodityMaxCons[STUFF_STEEL] = 100 * SHANTY_GET_STEEL;
        commodityMaxCons[STUFF_WASTE] = 100 *
          (MAX_WASTE_AT_SHANTY /*+ SHANTY_PUT_WASTE*2 + SHANTY_GET_GOODS/3*/);
    }
    virtual ~Shanty() {
        world(x,y)->killframe(waste_fire_frit);
    }

    virtual void update() override;
    virtual void report() override;
    virtual void animate() override;

    int anim;
    bool start_burning_waste;
    std::list<ExtraFrame>::iterator waste_fire_frit;
    int waste_fire_anim;
};




/** @file lincity/modules/shanty.h */
