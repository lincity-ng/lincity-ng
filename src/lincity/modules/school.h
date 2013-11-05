#define GROUP_SCHOOL_COLOUR (white(15))
#define GROUP_SCHOOL_COST   10000
#define GROUP_SCHOOL_COST_MUL 25
#define GROUP_SCHOOL_BUL_COST   10000
#define GROUP_SCHOOL_TECH   1
#define GROUP_SCHOOL_FIREC 40
#define GROUP_SCHOOL_RANGE 0
#define GROUP_SCHOOL_SIZE 2

#define JOBS_MAKE_TECH_SCHOOL  200
#define GOODS_MAKE_TECH_SCHOOL  75
#define TECH_MADE_BY_SCHOOL    2
#define MAX_JOBS_AT_SCHOOL     (20 * JOBS_MAKE_TECH_SCHOOL)
#define MAX_GOODS_AT_SCHOOL    (20 * GOODS_MAKE_TECH_SCHOOL)
#define MAX_WASTE_AT_SCHOOL    (20 * GOODS_MAKE_TECH_SCHOOL / 3)
#define SCHOOL_RUNNING_COST    2


#include "modules.h"
#include "../lintypes.h"
#include "../lctypes.h"


class SchoolConstructionGroup: public ConstructionGroup {
public:
    SchoolConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance,
        int cost, int tech, int range
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance, cost, tech, range
    ) {
        commodityRuleCount[Construction::STUFF_JOBS].maxload = MAX_JOBS_AT_SCHOOL;
        commodityRuleCount[Construction::STUFF_JOBS].take = true;
        commodityRuleCount[Construction::STUFF_JOBS].give = false;
        commodityRuleCount[Construction::STUFF_GOODS].maxload = MAX_GOODS_AT_SCHOOL;
        commodityRuleCount[Construction::STUFF_GOODS].take = true;
        commodityRuleCount[Construction::STUFF_GOODS].give = false;
        commodityRuleCount[Construction::STUFF_WASTE].maxload = MAX_WASTE_AT_SCHOOL;
        commodityRuleCount[Construction::STUFF_WASTE].take = false;
        commodityRuleCount[Construction::STUFF_WASTE].give = true;
    }
    // overriding method that creates a School
    virtual Construction *createConstruction(int x, int y);
};

extern SchoolConstructionGroup schoolConstructionGroup;

class School: public RegisteredConstruction<School> { // School inherits from its own RegisteredConstruction
public:
    School(int x, int y, ConstructionGroup *cstgrp): RegisteredConstruction<School>(x, y)
    {
        this->constructionGroup = cstgrp;
        this->working_days = 0;
        this->busy = 0;
        this->total_tech_made = 0;
        setMemberSaved(&this->total_tech_made, "total_tech_made");
        initialize_commodities();
    }

    virtual ~School() { }
    virtual void update();
    virtual void report();

    int total_tech_made;
    int working_days, busy;
};


/** @file lincity/modules/school.h */

