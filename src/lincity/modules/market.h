/* ---------------------------------------------------------------------- *
 * market.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */

#define GROUP_MARKET_COLOUR (blue(28))
#define GROUP_MARKET_COST 100
#define GROUP_MARKET_COST_MUL 25
#define GROUP_MARKET_BUL_COST 100
#define GROUP_MARKET_TECH 0
#define GROUP_MARKET_FIREC 80
#define GROUP_MARKET_RANGE 9
#define GROUP_MARKET_SIZE 2

//#define MARKET_ANIM_SPEED 750

#define JOBS_MARKET_EMPTY   1
#define JOBS_MARKET_LOW     5
#define JOBS_MARKET_MED    12
#define JOBS_MARKET_FULL   28

#include "modules.h"
#include "../lintypes.h"
#include "../lctypes.h"
#include "../transport.h"


class MarketConstructionGroup: public ConstructionGroup {
public:
    MarketConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int market_chance,
        int cost, int tech, int range
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, market_chance, cost, tech, range
    ) {
        commodityRuleCount[Construction::STUFF_FOOD].maxload = MAX_FOOD_IN_MARKET;
        commodityRuleCount[Construction::STUFF_FOOD].take = true;
        commodityRuleCount[Construction::STUFF_FOOD].give = true;
        commodityRuleCount[Construction::STUFF_JOBS].maxload = MAX_JOBS_IN_MARKET;
        commodityRuleCount[Construction::STUFF_JOBS].take = true;
        commodityRuleCount[Construction::STUFF_JOBS].give = true;
        commodityRuleCount[Construction::STUFF_COAL].maxload = MAX_COAL_IN_MARKET;
        commodityRuleCount[Construction::STUFF_COAL].take = true;
        commodityRuleCount[Construction::STUFF_COAL].give = true;
        commodityRuleCount[Construction::STUFF_GOODS].maxload = MAX_GOODS_IN_MARKET;
        commodityRuleCount[Construction::STUFF_GOODS].take = true;
        commodityRuleCount[Construction::STUFF_GOODS].give = true;
        commodityRuleCount[Construction::STUFF_ORE].maxload = MAX_ORE_IN_MARKET;
        commodityRuleCount[Construction::STUFF_ORE].take = true;
        commodityRuleCount[Construction::STUFF_ORE].give = true;
        commodityRuleCount[Construction::STUFF_STEEL].maxload = MAX_STEEL_IN_MARKET;
        commodityRuleCount[Construction::STUFF_STEEL].take = true;
        commodityRuleCount[Construction::STUFF_STEEL].give = true;
        commodityRuleCount[Construction::STUFF_WASTE].maxload = MAX_WASTE_IN_MARKET;
        commodityRuleCount[Construction::STUFF_WASTE].take = true;
        commodityRuleCount[Construction::STUFF_WASTE].give = true;
    };
    // overriding method that creates a Market
    virtual Construction *createConstruction(int x, int y);
};

extern MarketConstructionGroup marketConstructionGroup;
extern MarketConstructionGroup market_low_ConstructionGroup;
extern MarketConstructionGroup market_med_ConstructionGroup;
extern MarketConstructionGroup market_full_ConstructionGroup;

class Market: public RegisteredConstruction<Market> { // Market inherits from Construction
public:
    Market(int x, int y, ConstructionGroup *cstgrp): RegisteredConstruction<Market>(x, y)
    {
        this->constructionGroup = cstgrp;
        //local copy of commodityRuCount
        commodityRuleCount = constructionGroup->commodityRuleCount;
        setCommodityRulesSaved(&commodityRuleCount);
        initialize_commodities();
        this->jobs = JOBS_MARKET_EMPTY;
        this->anim = 0;
        this->busy = 0;
        this->working_days = 0;
        //set the Searchrange of this Market
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
        this->cover();
    }
    virtual void update();
    virtual void report();
    void cover();
    void toggleEvacuation();

    int xs, ys, xe, ye;
    int working_days, busy;
    int jobs;
    std::map<Commodities, CommodityRule> commodityRuleCount;
    int anim;
};

/** @file lincity/modules/market.h */

