#define GROUP_COALMINE_COLOUR 0
#define GROUP_COALMINE_COST   10000
#define GROUP_COALMINE_COST_MUL 25
#define GROUP_COALMINE_BUL_COST   10000
#define GROUP_COALMINE_TECH   85
#define GROUP_COALMINE_FIREC  85
#define GROUP_COALMINE_RANGE  6
#define GROUP_COALMINE_SIZE  4


#define COALMINE_POLLUTION      3
#define COAL_PER_RESERVE   1000
#define JOBS_DIG_COAL 900
#define COALMINE_JOBS (JOBS_DIG_COAL + JOBS_LOAD_COAL)
#define MAX_JOBS_AT_COALMINE (20 * COALMINE_JOBS)
#define MAX_COAL_AT_MINE (20 * COAL_PER_RESERVE)


#define TARGET_COAL_LEVEL 80

#include "modules.h"
#include "../lintypes.h"
#include "../lctypes.h"



class CoalmineConstructionGroup: public ConstructionGroup {
public:
    CoalmineConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance,
        int cost, int tech, int range
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance, cost, tech, range
    ) {
        commodityRuleCount[Construction::STUFF_JOBS].maxload = MAX_JOBS_AT_COALMINE;
        commodityRuleCount[Construction::STUFF_JOBS].take = true;
        commodityRuleCount[Construction::STUFF_JOBS].give = false;
        commodityRuleCount[Construction::STUFF_COAL].maxload = MAX_COAL_AT_MINE;
        commodityRuleCount[Construction::STUFF_COAL].take = true;
        commodityRuleCount[Construction::STUFF_COAL].give = true;
    }
    // overriding method that creates an Coalmine
    virtual Construction *createConstruction(int x, int y);
};

extern CoalmineConstructionGroup coalmineConstructionGroup;
extern CoalmineConstructionGroup coalmine_L_ConstructionGroup;
extern CoalmineConstructionGroup coalmine_M_ConstructionGroup;
extern CoalmineConstructionGroup coalmine_H_ConstructionGroup;

class Coalmine: public RegisteredConstruction<Coalmine> { // Coalmine inherits from its RegisteredConstruction
public:
    Coalmine(int x, int y, ConstructionGroup *cstgrp): RegisteredConstruction<Coalmine>(x, y)
    {
        this->constructionGroup = cstgrp;
        this->working_days = 0;
        this->busy = 0;
        this->current_coal_reserve = 0;  // has to be auto updated since coalmines may compete
        initialize_commodities();

        int coal = 0;
        int lenm1 = world.len()-1;
        int tmp;
        tmp = x - constructionGroup->range;
        this->xs = (tmp < 1) ? 1 : tmp;
        tmp = y - constructionGroup->range;
        this->ys = (tmp < 1)? 1 : tmp;
        tmp = x + constructionGroup->range + constructionGroup->size;
        this->xe = (tmp > lenm1) ? lenm1 : tmp;
        tmp = y + constructionGroup->range + constructionGroup->size;
        this->ye = (tmp > lenm1)? lenm1 : tmp;

        for (int yy = ys; yy < ye ; yy++)
        {
            for (int xx = xs; xx < xe ; xx++)
            {   coal += world(xx,yy)->coal_reserve;}
        }
        //always provide some coal so player can
        //store sustainable coal
        if (coal < 20)
        {
            world(x,y)->coal_reserve += 20-coal;
            coal = 20;
        }
        this->initial_coal_reserve = coal;
        setMemberSaved(&this->initial_coal_reserve,"initial_coal_reserve");
        this->current_coal_reserve = coal;
    }
    virtual ~Coalmine() { }
    virtual void update();
    virtual void report();

    int xs, ys, xe, ye;
    int initial_coal_reserve;
    int current_coal_reserve;
    int working_days, busy;
};

/** @file lincity/modules/coalmine.h */

