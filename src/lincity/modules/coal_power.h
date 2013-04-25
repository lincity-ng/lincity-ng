#define GROUP_COAL_POWER_COLOUR 0
#define GROUP_COAL_POWER_COST   100000
#define GROUP_COAL_POWER_COST_MUL 5
#define GROUP_COAL_POWER_BUL_COST   200000
#define GROUP_COAL_POWER_TECH   200
#define GROUP_COAL_POWER_FIREC  80

#define POWERS_COAL_OUTPUT 11000 //x2 for kWh
#define MAX_MWH_AT_COALPS (20 * POWERS_COAL_OUTPUT)
#define MAX_COAL_AT_POWER_STATION 100000
#define MAX_COAL_AT_COALPS (20 * POWERS_COAL_OUTPUT / 250)
#define POWERS_COAL_POLLUTION  20
#define JOBS_COALPS_GENERATE 100
#define MAX_JOBS_AT_COALPS (20 * JOBS_COALPS_GENERATE) 


#include "modules.h"
#include "../lintypes.h"
#include "../lctypes.h"


class Coal_powerConstructionGroup: public ConstructionGroup {
public:
    Coal_powerConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance, int cost, int tech
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance, cost, tech
    ) {
        commodityRuleCount[Construction::STUFF_JOBS].maxload = MAX_JOBS_AT_COALPS;
        commodityRuleCount[Construction::STUFF_JOBS].take = true;
        commodityRuleCount[Construction::STUFF_JOBS].give = false;
        commodityRuleCount[Construction::STUFF_COAL].maxload = MAX_COAL_AT_COALPS;
        commodityRuleCount[Construction::STUFF_COAL].take = true;
        commodityRuleCount[Construction::STUFF_COAL].give = false;
        commodityRuleCount[Construction::STUFF_MWH].maxload = MAX_MWH_AT_COALPS;
        commodityRuleCount[Construction::STUFF_MWH].take = false;
        commodityRuleCount[Construction::STUFF_MWH].give = true;    
    }
    // overriding method that creates a Coal_power
    virtual Construction *createConstruction(int x, int y, unsigned short type);
};

extern Coal_powerConstructionGroup coal_powerConstructionGroup;

class Coal_power: public CountedConstruction<Coal_power> { // Coal_power inherits from its own CountedConstruction
public:
	Coal_power(int x, int y, unsigned short type): CountedConstruction<Coal_power>(x, y, type)
    {        
        constructionGroup = &coal_powerConstructionGroup;               
        this->tech = tech_level;
        setMemberSaved(&this->tech, "tech");
        this->working_days = 0;
        this->busy = 0;
        this->mwh_output = (int)(POWERS_COAL_OUTPUT + (((double)tech_level * POWERS_COAL_OUTPUT) / MAX_TECH_LEVEL));
        setMemberSaved(&this->mwh_output, "mwh_output");
        initialize_commodities();      
    }
	virtual ~Coal_power() {}
	virtual void update();
	virtual void report();
    
    int  mwh_output; 
    int  tech;    	
    int  working_days;
    int  busy;      
};


/** @file lincity/modules/coal_power.h */

