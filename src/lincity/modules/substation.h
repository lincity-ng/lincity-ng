#define GROUP_SUBSTATION_COLOUR (yellow(18))
#define GROUP_SUBSTATION_COST 500
#define GROUP_SUBSTATION_COST_MUL 2
#define GROUP_SUBSTATION_BUL_COST 100
#define GROUP_SUBSTATION_TECH 200
#define GROUP_SUBSTATION_FIREC 50
#define GROUP_SUBSTATION_RANGE 0
#define GROUP_SUBSTATION_SIZE 2

#define SUBSTATION_MWH                          (1500)
#define MAX_MWH_AT_SUBSTATION    (20 * SUBSTATION_MWH)
#define SUBSTATION_KWH            (2 * SUBSTATION_MWH)
#define MAX_KWH_AT_SUBSTATION    (20 * SUBSTATION_KWH)


#include <array>                    // for array

#include "../lctypes.h"
#include "../lintypes.h"            // for CommodityRule, Commodity, Constru...
#include "lincity/all_buildings.h"  // for yellow
#include "modules.h"

class SubstationConstructionGroup: public ConstructionGroup {
public:
    SubstationConstructionGroup(
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

        commodityRuleCount[STUFF_MWH].maxload = MAX_MWH_AT_SUBSTATION;
        commodityRuleCount[STUFF_MWH].take = true;
        commodityRuleCount[STUFF_MWH].give = false;
        commodityRuleCount[STUFF_KWH].maxload = MAX_KWH_AT_SUBSTATION;
        commodityRuleCount[STUFF_KWH].take = false;
        commodityRuleCount[STUFF_KWH].give = true;
    }
    // overriding method that creates a Substation
    virtual Construction *createConstruction(int x, int y);
};

extern SubstationConstructionGroup substationConstructionGroup;
//extern SubstationConstructionGroup substation_RG_ConstructionGroup;
//extern SubstationConstructionGroup substation_G_ConstructionGroup;

class Substation: public RegisteredConstruction<Substation> { // Substation inherits from Construction
public:
    Substation(int x, int y, ConstructionGroup *cstgrp): RegisteredConstruction<Substation>(x, y)
    {
        this->constructionGroup = cstgrp;
        init_resources();
        this->working_days = 0;
        this->busy = 0;
        initialize_commodities();

        commodityMaxCons[STUFF_MWH] = 100 * SUBSTATION_MWH;
        commodityMaxProd[STUFF_KWH] = 100 * 2 * SUBSTATION_MWH;
    }
    virtual ~Substation() { }
    virtual void update() override;
    virtual void report() override;
    virtual void animate() override;

    int  working_days, busy;
};

/** @file lincity/modules/substation.h */
