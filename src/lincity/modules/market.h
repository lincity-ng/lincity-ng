/* ---------------------------------------------------------------------- *
 * market.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */

//#define T_FOOD  0
//#define T_JOBS  1
//#define T_COAL  2
//#define T_GOODS 3
//#define T_ORE   4
//#define T_STEEL 5
//#define T_WASTE 6

#define GROUP_MARKET_COLOUR (blue(28))
#define GROUP_MARKET_COST 100
#define GROUP_MARKET_COST_MUL 25
#define GROUP_MARKET_BUL_COST 100
#define GROUP_MARKET_TECH 0
#define GROUP_MARKET_FIREC 80
#define GROUP_MARKET_RANGE 9

//#define MARKET_ANIM_SPEED 750

#define JOBS_MARKET_LOW     5
#define JOBS_MARKET_MED    12
#define JOBS_MARKET_FULL   28

#include "modules.h"
#include "../lintypes.h"
#include "../lctypes.h"
#include "../transport.h"
//#include "../range.h"


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
    virtual Construction *createConstruction(int x, int y, unsigned short type);
};

extern MarketConstructionGroup marketConstructionGroup;

class Market: public CountedConstruction<Market> { // Market inherits from Construction
public:
	Market(int x, int y, unsigned short type): CountedConstruction<Market>(x, y, type) 
    {
        constructionGroup = &marketConstructionGroup;
        this->jobs = JOBS_MARKET_LOW;
        //local copy of commodityRuleCount
        commodityRuleCount = constructionGroup->commodityRuleCount;
        setCommodityRulesSaved(&commodityRuleCount);        
        initialize_commodities();
        this->anim = 0;
        this->burning_waste = false;
        this->old_type = this->type;
        setMemberSaved(&(this->old_type),"type");
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
    
    int xs, ys, xe, ye;
    int jobs;
    std::map<Commodities, CommodityRule> commodityRuleCount;
    int anim;
    bool burning_waste;
    int old_type;   
};

/** @file lincity/modules/market.h */

