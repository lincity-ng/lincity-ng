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
#define SHANTY_GET_JOBS   5
#define SHANTY_GET_GOODS  50
#define SHANTY_GET_COAL   1
#define SHANTY_GET_ORE    10
#define SHANTY_GET_STEEL  1
#define SHANTY_PUT_WASTE  1
#define SHANTY_GET_KWH    50

#define MAX_FOOD_AT_SHANTY  (SHANTY_GET_FOOD * 20)
#define MAX_JOBS_AT_SHANTY  (SHANTY_GET_JOBS * 20)
#define MAX_GOODS_AT_SHANTY (SHANTY_GET_GOODS * 20)
#define MAX_COAL_AT_SHANTY  (SHANTY_GET_COAL * 20)
#define MAX_ORE_AT_SHANTY   (SHANTY_GET_ORE * 20)
#define MAX_STEEL_AT_SHANTY (SHANTY_GET_STEEL * 20)
#define MAX_WASTE_AT_SHANTY (SHANTY_PUT_WASTE * 20 + MAX_GOODS_AT_SHANTY / 3)
#define MAX_KWH_AT_SHANTY   (SHANTY_GET_KWH * 20)

#include "modules.h"
#include "../lintypes.h"
#include "../lctypes.h"

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
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance, cost, tech, range
    ) {
        commodityRuleCount[Construction::STUFF_FOOD].maxload = MAX_FOOD_AT_SHANTY;
        commodityRuleCount[Construction::STUFF_FOOD].take = true;
        commodityRuleCount[Construction::STUFF_FOOD].give = false;
        commodityRuleCount[Construction::STUFF_JOBS].maxload = MAX_JOBS_AT_SHANTY;
        commodityRuleCount[Construction::STUFF_JOBS].take = true;
        commodityRuleCount[Construction::STUFF_JOBS].give = false;
        commodityRuleCount[Construction::STUFF_GOODS].maxload = MAX_GOODS_AT_SHANTY;
        commodityRuleCount[Construction::STUFF_GOODS].take = true;
        commodityRuleCount[Construction::STUFF_GOODS].give = false;
        commodityRuleCount[Construction::STUFF_COAL].maxload = MAX_COAL_AT_SHANTY;
        commodityRuleCount[Construction::STUFF_COAL].take = true;
        commodityRuleCount[Construction::STUFF_COAL].give = false;
        commodityRuleCount[Construction::STUFF_ORE].maxload = MAX_ORE_AT_SHANTY;
        commodityRuleCount[Construction::STUFF_ORE].take = true;
        commodityRuleCount[Construction::STUFF_ORE].give = false;
        commodityRuleCount[Construction::STUFF_STEEL].maxload = MAX_STEEL_AT_SHANTY;
        commodityRuleCount[Construction::STUFF_STEEL].take = true;
        commodityRuleCount[Construction::STUFF_STEEL].give = false;
        commodityRuleCount[Construction::STUFF_WASTE].maxload = MAX_WASTE_AT_SHANTY;
        commodityRuleCount[Construction::STUFF_WASTE].take = false;
        commodityRuleCount[Construction::STUFF_WASTE].give = true;
        commodityRuleCount[Construction::STUFF_KWH].maxload = MAX_KWH_AT_SHANTY;
        commodityRuleCount[Construction::STUFF_KWH].take = true;
        commodityRuleCount[Construction::STUFF_KWH].give = false;
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
        initialize_commodities();
        this->flags |= FLAG_NEVER_EVACUATE;
        this->anim = 0;
    }
    virtual ~Shanty() { }
    virtual void update();
    virtual void report();
    int anim;
};




/** @file lincity/modules/shanty.h */

