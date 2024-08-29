#define GROUP_MILL_COLOUR (white(15))
#define GROUP_MILL_COST   10000
#define GROUP_MILL_COST_MUL 25
#define GROUP_MILL_BUL_COST   1000
#define GROUP_MILL_TECH   25
#define GROUP_MILL_FIREC  60
#define GROUP_MILL_RANGE  0
#define GROUP_MILL_SIZE 2

#define MILL_LABOR          35
#define GOODS_MADE_BY_MILL 75
#define FOOD_USED_BY_MILL  (GOODS_MADE_BY_MILL/2)
#define MILL_POWER_PER_COAL 60
#define COAL_USED_BY_MILL  1

#define MAX_LOVOLT_AT_MILL    (COAL_USED_BY_MILL * MILL_POWER_PER_COAL * 20)
#define MAX_LABOR_AT_MILL   (MILL_LABOR * 20)
#define MAX_FOOD_AT_MILL   (FOOD_USED_BY_MILL * 20)
#define MAX_COAL_AT_MILL   (COAL_USED_BY_MILL * 20)
#define MAX_GOODS_AT_MILL  (GOODS_MADE_BY_MILL * 20)

#define MILL_ANIM_SPEED    300

#include <array>                    // for array

#include "modules.h"

class MillConstructionGroup: public ConstructionGroup {
public:
    MillConstructionGroup(
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
        commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_AT_MILL;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = false;
        commodityRuleCount[STUFF_FOOD].maxload = MAX_FOOD_AT_MILL;
        commodityRuleCount[STUFF_FOOD].take = true;
        commodityRuleCount[STUFF_FOOD].give = false;
        commodityRuleCount[STUFF_COAL].maxload = MAX_COAL_AT_MILL;
        commodityRuleCount[STUFF_COAL].take = true;
        commodityRuleCount[STUFF_COAL].give = false;
        commodityRuleCount[STUFF_GOODS].maxload = MAX_GOODS_AT_MILL;
        commodityRuleCount[STUFF_GOODS].take = false;
        commodityRuleCount[STUFF_GOODS].give = true;
        commodityRuleCount[STUFF_LOVOLT].maxload = MAX_LOVOLT_AT_MILL;
        commodityRuleCount[STUFF_LOVOLT].take = true;
        commodityRuleCount[STUFF_LOVOLT].give = false;
    }
    // overriding method that creates a mill
    virtual Construction *createConstruction();
};

extern MillConstructionGroup millConstructionGroup;

class Mill: public Construction {
public:
    Mill(ConstructionGroup *cstgrp) {
        this->constructionGroup = cstgrp;
        init_resources();
        // this->anim = 0;
        this->busy = 0;
        this->working_days = 0;
        this->animate_enable = false;
        this->pol_count = 0;
        initialize_commodities();

        commodityMaxCons[STUFF_COAL] = 100 * COAL_USED_BY_MILL;
        commodityMaxCons[STUFF_LOVOLT] = 100 *
          COAL_USED_BY_MILL * MILL_POWER_PER_COAL;
        commodityMaxCons[STUFF_FOOD] = 100 * FOOD_USED_BY_MILL;
        commodityMaxCons[STUFF_LABOR] = 100 * MILL_LABOR;
        commodityMaxProd[STUFF_GOODS] = 100 * GOODS_MADE_BY_MILL;
    }
    virtual ~Mill() { }
    virtual void update() override;
    virtual void report() override;
    virtual void animate() override;

    int  anim;
    int  pol_count;
    int  working_days, busy;
    bool animate_enable;
};


/** @file lincity/modules/mill.h */
