#define GROUP_FIRESTATION_COLOUR (red(20))
#define GROUP_FIRESTATION_COST   20000
#define GROUP_FIRESTATION_COST_MUL 10
#define GROUP_FIRESTATION_BUL_COST 1000
#define GROUP_FIRESTATION_TECH     22
#define GROUP_FIRESTATION_FIREC 0
#define GROUP_FIRESTATION_RANGE 17
#define GROUP_FIRESTATION_SIZE 2

#define FIRESTATION_JOBS   6
#define MAX_JOBS_AT_FIRESTATION (20 * FIRESTATION_JOBS)
#define FIRESTATION_GOODS  3
#define MAX_GOODS_AT_FIRESTATION (20 * FIRESTATION_GOODS)
#define MAX_WASTE_AT_FIRESTATION (20 * FIRESTATION_GOODS / 3)
#define FIRESTATION_RUNNING_COST 1
#define FIRESTATION_RUNNING_COST_MUL 6
#define FIRESTATION_ANIMATION_SPEED 250

#include <array>                    // for array
#include <string>                   // for basic_string

#include "../lctypes.h"
#include "../lintypes.h"            // for CommodityRule, Commodity, Constru...
#include "lincity/all_buildings.h"  // for red
#include "lincity/world.h"          // for World
#include "modules.h"

class FireStationConstructionGroup: public ConstructionGroup {
public:
    FireStationConstructionGroup(
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
        commodityRuleCount[STUFF_JOBS].maxload = MAX_JOBS_AT_FIRESTATION;
        commodityRuleCount[STUFF_JOBS].take = true;
        commodityRuleCount[STUFF_JOBS].give = false;
        commodityRuleCount[STUFF_GOODS].maxload = MAX_GOODS_AT_FIRESTATION;
        commodityRuleCount[STUFF_GOODS].take = true;
        commodityRuleCount[STUFF_GOODS].give = false;
        commodityRuleCount[STUFF_WASTE].maxload = MAX_WASTE_AT_FIRESTATION;
        commodityRuleCount[STUFF_WASTE].take = false;
        commodityRuleCount[STUFF_WASTE].give = true;
    }
    // overriding method that creates a firestation
    virtual Construction *createConstruction(int x, int y);
};

extern FireStationConstructionGroup fireStationConstructionGroup;

class FireStation: public RegisteredConstruction<FireStation> { // FireStation inherits from RegisteredConstruction
public:
    FireStation(int x, int y, ConstructionGroup *cstgrp) : RegisteredConstruction<FireStation>(x ,y)
    {
        this->constructionGroup = cstgrp;
        init_resources();
        this->anim = 0;
        this->animate_enable = false;
        this->active = false;
        setMemberSaved(&(this->active),"active");
        this->busy = 0;
        this->working_days = 0;
        this->daycount = 0;
        setMemberSaved(&(this->daycount),"daycount");
        this->covercount = 0;
        setMemberSaved(&(this->covercount),"covercount");
        initialize_commodities();

        int tmp;
        int lenm1 = world.len()-1;
        tmp = x - constructionGroup->range;
        this->xs = (tmp < 1) ? 1 : tmp;
        tmp = y - constructionGroup->range;
        this->ys = (tmp < 1)? 1 : tmp;
        tmp = x + constructionGroup->range + constructionGroup->size;
        this->xe = (tmp > lenm1) ? lenm1 : tmp;
        tmp = y + constructionGroup->range + constructionGroup->size;
        this->ye = (tmp > lenm1)? lenm1 : tmp;

        commodityMaxCons[STUFF_JOBS] = 100 * FIRESTATION_JOBS;
        commodityMaxCons[STUFF_GOODS] = 100 * FIRESTATION_GOODS;
        commodityMaxProd[STUFF_WASTE] = 100 * (FIRESTATION_GOODS / 3);

    }
    virtual ~FireStation() { }
    virtual void update() override;
    virtual void report() override;
    virtual void animate() override;
    void cover();

    int xs, ys, xe, ye;
    int daycount, covercount;
    int anim;
    bool animate_enable, active;
    int working_days, busy;
};

/** @file lincity/modules/firestation.h */
