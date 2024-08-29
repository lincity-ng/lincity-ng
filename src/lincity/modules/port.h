#define GROUP_PORT_COLOUR (blue(28))
#define GROUP_PORT_COST 100000
#define GROUP_PORT_COST_MUL 2
#define GROUP_PORT_BUL_COST 1000
#define GROUP_PORT_TECH 35
#define GROUP_PORT_FIREC 50
#define GROUP_PORT_RANGE 0
#define GROUP_PORT_SIZE 4
#define GROUP_PORT_NEED_CREDIT false

#define PORT_FOOD_RATE    1
#define PORT_LABOR_RATE    5
#define PORT_COAL_RATE    50
#define PORT_ORE_RATE     1
#define PORT_GOODS_RATE   3
#define PORT_STEEL_RATE   100
#define PORT_POLLUTION    1
//FIXME Guessing some values
#define PORT_LABOR  100
#define PORT_FOOD  500
#define PORT_COAL  100
#define PORT_GOODS 100
#define PORT_ORE   300
#define PORT_STEEL 50

#define MAX_LABOR_ON_PORT  (20 * PORT_LABOR)
#define MAX_FOOD_ON_PORT  (20 * PORT_FOOD)
#define MAX_COAL_ON_PORT  (20 * PORT_COAL)
#define MAX_GOODS_ON_PORT (20 * PORT_GOODS)
#define MAX_ORE_ON_PORT   (20 * PORT_ORE)
#define MAX_STEEL_ON_PORT (20 * PORT_STEEL)

/*
  These next two control the stuff bought or sold as a % of what's on the
  transport.  1000=100%  500=50% etc. Port will only work if % of stuff/available
 capacity excced capacity/PORT_TRIGGER_RATE
*/
#define PORT_EXPORT_RATE  500
#define PORT_IMPORT_RATE  500
#define PORT_TRIGGER_RATE  15

#define IMPORT_EXPORT_ENABLE_PERIOD  100
#define IMPORT_EXPORT_DISABLE_PERIOD 150

#include <array>                    // for array
#include <map>                      // for map
#include <string>                   // for basic_string

#include "modules.h"

class PortConstructionGroup: public ConstructionGroup {
public:
    PortConstructionGroup(
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
        commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_ON_PORT;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = false;
        commodityRuleCount[STUFF_FOOD].maxload = MAX_FOOD_ON_PORT;
        commodityRuleCount[STUFF_FOOD].take = true;
        commodityRuleCount[STUFF_FOOD].give = true;
        commodityRuleCount[STUFF_COAL].maxload = MAX_COAL_ON_PORT;
        commodityRuleCount[STUFF_COAL].take = true;
        commodityRuleCount[STUFF_COAL].give = true;
        commodityRuleCount[STUFF_GOODS].maxload = MAX_GOODS_ON_PORT;
        commodityRuleCount[STUFF_GOODS].take = true;
        commodityRuleCount[STUFF_GOODS].give = true;
        commodityRuleCount[STUFF_ORE].maxload = MAX_ORE_ON_PORT;
        commodityRuleCount[STUFF_ORE].take = true;
        commodityRuleCount[STUFF_ORE].give = true;
        commodityRuleCount[STUFF_STEEL].maxload = MAX_STEEL_ON_PORT;
        commodityRuleCount[STUFF_STEEL].take = true;
        commodityRuleCount[STUFF_STEEL].give = true;

        commodityRates[STUFF_FOOD] = PORT_FOOD_RATE;
        commodityRates[STUFF_COAL] = PORT_COAL_RATE;
        commodityRates[STUFF_GOODS] = PORT_GOODS_RATE;
        commodityRates[STUFF_ORE] = PORT_ORE_RATE;
        commodityRates[STUFF_STEEL] = PORT_STEEL_RATE;

        tradeRule = commodityRuleCount;
        tradeRule[STUFF_LABOR] = (CommodityRule){
          .maxload = 0,
          .take = false,
          .give = false
        };
    };
    //map that holds the Rates for the commodities
    std::map<Commodity, int> commodityRates;
    std::array<CommodityRule, STUFF_COUNT> tradeRule;
    // overriding method that creates a Port
    virtual Construction *createConstruction();
};

extern PortConstructionGroup portConstructionGroup;

class Port: public Construction {
public:
    Port(ConstructionGroup *cstgrp) {
        this->constructionGroup = cstgrp;
        init_resources();
        this->daily_ic = 0; this->daily_et = 0;
        this->monthly_ic = 0; this->monthly_et = 0;
        this->lastm_ic = 0; this->lastm_et = 0;
        this->pence = 0;
        this->working_days = 0;
        this->busy = 0;
        this->tech_made = 0;
        setMemberSaved(&this->tech_made, "tech_made");
        initialize_commodities();
        //local copy of commodityRuleCount
        commodityRuleCount = constructionGroup->commodityRuleCount;
        //do not trade labor
        // commodityRuleCount.erase (STUFF_LABOR);
        commodityRuleCount[STUFF_LABOR] = (CommodityRule){
          .maxload = 0,
          .take = false,
          .give = false
        };
        commodityRuleCount[STUFF_FOOD].take = false;
        commodityRuleCount[STUFF_FOOD].give = false;
        commodityRuleCount[STUFF_COAL].take = false;
        commodityRuleCount[STUFF_COAL].give = false;
        commodityRuleCount[STUFF_GOODS].take = false;
        commodityRuleCount[STUFF_GOODS].give = false;
        commodityRuleCount[STUFF_ORE].take = false;
        commodityRuleCount[STUFF_ORE].give = false;
        commodityRuleCount[STUFF_STEEL].take = false;
        commodityRuleCount[STUFF_STEEL].give = false;
        setCommodityRulesSaved(&commodityRuleCount);

        commodityMaxCons[STUFF_LABOR] = 100 * PORT_LABOR;
        for(Commodity stuff = STUFF_INIT ; stuff < STUFF_COUNT ; stuff++) {
            if(!commodityRuleCount[stuff].maxload) continue;
            commodityMaxCons[stuff] = 100 * ((
              portConstructionGroup.commodityRuleCount[stuff].maxload *
              PORT_EXPORT_RATE) / 1000);
            commodityMaxProd[stuff] = 100 * ((
              portConstructionGroup.commodityRuleCount[stuff].maxload *
              PORT_IMPORT_RATE) / 1000);
        }
    }
    virtual ~Port() { }
    virtual void update();
    virtual void report();
    int buy_stuff(Commodity stuff_ID);
    int sell_stuff(Commodity stuff_ID);
    void trade_connection();
    std::array<CommodityRule, STUFF_COUNT> commodityRuleCount;
    int daily_ic, monthly_ic, lastm_ic; //import cost
    int daily_et, monthly_et, lastm_et; //export tax
    int pence;
    int working_days, busy;
    int tech_made;
};


/** @file lincity/modules/port.h */
