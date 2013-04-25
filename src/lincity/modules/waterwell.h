#define GROUP_WATERWELL_COLOUR    (blue(31))
#define GROUP_WATERWELL_COST      1
#define GROUP_WATERWELL_COST_MUL 2
#define GROUP_WATERWELL_BUL_COST      1
#define GROUP_WATERWELL_TECH      0
#define GROUP_WATERWELL_FIREC 0
#define MAX_POLLUTION_AT_WATERWELL 3000
#define WATER_PER_UGW 400
#define MAX_WATER_AT_WATERWELL (80 * WATER_PER_UGW)
#include "modules.h"
#include "../range.h"

class WaterwellConstructionGroup: public ConstructionGroup {
public:
    WaterwellConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance, int cost, int tech
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance, cost, tech
    ) {
        commodityRuleCount[Construction::STUFF_WATER].maxload = MAX_WATER_AT_WATERWELL;
        commodityRuleCount[Construction::STUFF_WATER].give = true;
        commodityRuleCount[Construction::STUFF_WATER].take = false;
    }
    // overriding method that creates a waterwell
    virtual Construction *createConstruction(int x, int y, unsigned short type);
};

extern WaterwellConstructionGroup waterwellConstructionGroup;

class Waterwell: public CountedConstruction<Waterwell> { // waterwell inherits from its own CountedConstruction
public:
	Waterwell(int x, int y, unsigned short type): CountedConstruction<Waterwell>(x, y, type) 
    {              
        constructionGroup = &waterwellConstructionGroup;
        this->busy = 0;
        this->working_days = 0;        
        //this->cover();
        initialize_commodities();
        int w = 0;                
        for (int i = 0; i < constructionGroup->size; i++)
        {
            for (int j = 0; j < constructionGroup->size; j++)
            {
                if (world(x+j, y+i)->flags & FLAG_HAS_UNDERGROUND_WATER)
                    w++;
            }// end j
        }//end i
        this->ugwCount = w;    
        this->water_output = w * WATER_PER_UGW;       
    }

	virtual ~Waterwell() { }
	virtual void update();
	virtual void report();
    //void cover();

    int water_output;
    int ugwCount;
    int working_days;    
    int busy;
};

/** @file lincity/modules/waterwell.h */

