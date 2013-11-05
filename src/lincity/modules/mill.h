#define GROUP_MILL_COLOUR (white(15))
#define GROUP_MILL_COST   10000
#define GROUP_MILL_COST_MUL 25
#define GROUP_MILL_BUL_COST   1000
#define GROUP_MILL_TECH   25
#define GROUP_MILL_FIREC  60
#define GROUP_MILL_RANGE  0
#define GROUP_MILL_SIZE 2

#define MILL_JOBS          35
#define GOODS_MADE_BY_MILL 75
#define FOOD_USED_BY_MILL  (GOODS_MADE_BY_MILL/2)
#define MILL_POWER_PER_COAL 60
#define COAL_USED_BY_MILL  1

#define MAX_KWH_AT_MILL    (COAL_USED_BY_MILL * MILL_POWER_PER_COAL * 20)
#define MAX_JOBS_AT_MILL   (MILL_JOBS * 20)
#define MAX_FOOD_AT_MILL   (FOOD_USED_BY_MILL * 20)
#define MAX_COAL_AT_MILL   (COAL_USED_BY_MILL * 20)
#define MAX_GOODS_AT_MILL  (GOODS_MADE_BY_MILL * 20)

#define MILL_ANIM_SPEED    300

#include "modules.h"
#include "../lintypes.h"
#include "../lctypes.h"

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
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance, cost, tech, range
    )
    {
        commodityRuleCount[Construction::STUFF_JOBS].maxload = MAX_JOBS_AT_MILL;
        commodityRuleCount[Construction::STUFF_JOBS].take = true;
        commodityRuleCount[Construction::STUFF_JOBS].give = false;
        commodityRuleCount[Construction::STUFF_FOOD].maxload = MAX_FOOD_AT_MILL;
        commodityRuleCount[Construction::STUFF_FOOD].take = true;
        commodityRuleCount[Construction::STUFF_FOOD].give = false;
        commodityRuleCount[Construction::STUFF_COAL].maxload = MAX_COAL_AT_MILL;
        commodityRuleCount[Construction::STUFF_COAL].take = true;
        commodityRuleCount[Construction::STUFF_COAL].give = false;
        commodityRuleCount[Construction::STUFF_GOODS].maxload = MAX_GOODS_AT_MILL;
        commodityRuleCount[Construction::STUFF_GOODS].take = false;
        commodityRuleCount[Construction::STUFF_GOODS].give = true;
        commodityRuleCount[Construction::STUFF_KWH].maxload = MAX_KWH_AT_MILL;
        commodityRuleCount[Construction::STUFF_KWH].take = true;
        commodityRuleCount[Construction::STUFF_KWH].give = false;
    }
    // overriding method that creates a mill
    virtual Construction *createConstruction(int x, int y);
};

extern MillConstructionGroup millConstructionGroup;

class Mill: public RegisteredConstruction<Mill> { // Mill inherits from its own RegisteredConstruction
public:
    Mill(int x, int y, ConstructionGroup *cstgrp): RegisteredConstruction<Mill>(x, y)
    {
        this->constructionGroup = cstgrp;
        this->anim = 0;
        this->busy = 0;
        this->working_days = 0;
        this->animate = false;
        this->pol_count = 0;
        initialize_commodities();
    }
    virtual ~Mill() { }
    virtual void update();
    virtual void report();

    int  anim;
    int  pol_count;
    int  working_days, busy;
    bool animate;
};


/** @file lincity/modules/mill.h */

