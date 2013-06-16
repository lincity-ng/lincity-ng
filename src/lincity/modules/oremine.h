// Some ore min related stuff has to be in all_buildings.h


#define ORE_PER_RESERVE   5000
#define MAX_ORE_AT_MINE (20 * ORE_PER_RESERVE)
#define MIN_ORE_RESERVE_FOR_MINE (ORE_RESERVE)
#define JOBS_DIG_ORE  200
#define MAX_JOBS_AT_OREMINE (20 * JOBS_DIG_ORE)

#define ORE_LEVEL_TARGET 80 //mine will only supply so much

#define GROUP_OREMINE_COLOUR (red(18))
#define GROUP_OREMINE_COST 500
#define GROUP_OREMINE_COST_MUL 10
#define GROUP_OREMINE_BUL_COST 500000
#define GROUP_OREMINE_TECH 0
#define GROUP_OREMINE_FIREC 0


#define OREMINE_ANIMATION_SPEED 200


#include "modules.h"
#include "../lintypes.h"
#include "../lctypes.h"
#include <cstdlib>

class OremineConstructionGroup: public ConstructionGroup {
public:
    OremineConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance, int cost, int tech
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance, cost, tech
    ) {
        commodityRuleCount[Construction::STUFF_JOBS].maxload = MAX_JOBS_AT_OREMINE;
        commodityRuleCount[Construction::STUFF_JOBS].take = true;
        commodityRuleCount[Construction::STUFF_JOBS].give = false;
        commodityRuleCount[Construction::STUFF_ORE].maxload = MAX_ORE_AT_MINE;
        commodityRuleCount[Construction::STUFF_ORE].take = true;
        commodityRuleCount[Construction::STUFF_ORE].give = true;
          
    }
    // overriding method that creates an Oremine
    virtual Construction *createConstruction(int x, int y, unsigned short type);
};
/*
class EmptyOremineConstructionGroup: public ConstructionGroup {
public:
    EmptyOremineConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance, int cost, int tech
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance, cost, tech
    ) {
        commodityRuleCount[Construction::STUFF_JOBS].maxload = 1;
        commodityRuleCount[Construction::STUFF_JOBS].take = false;
        commodityRuleCount[Construction::STUFF_JOBS].give = true;
        commodityRuleCount[Construction::STUFF_ORE].maxload = 1;
        commodityRuleCount[Construction::STUFF_ORE].take = false;
        commodityRuleCount[Construction::STUFF_ORE].give = true;
          
    }
    // overriding method that creates an Oremine
    virtual Construction *createConstruction(int x, int y, unsigned short type);
};
*/
extern OremineConstructionGroup oremineConstructionGroup;

class Oremine: public CountedConstruction<Oremine> { // Oremine inherits from its own CountedConstruction
public:
	Oremine(int x, int y, unsigned short type): CountedConstruction<Oremine>(x, y, type)  
    {    
        constructionGroup = &oremineConstructionGroup;
        this->anim = 0;
        this->animate = false;
        this->busy_days = 0;
        this->busy = 0;
        this->anim_count = 0;
        this->days_offset = 0;
        initialize_commodities();

        int ore = 0;
        for (int yy = y; (yy < y + constructionGroup->size) ; yy++)
        {
            for (int xx = x; (xx < x + constructionGroup->size); xx++)
            {
                ore += world(xx,yy)->ore_reserve;                
            }
        }
        if (ore < 1)
        { ore = 1;} 
        this->total_ore_reserve = ore;
        setMemberSaved(&this->total_ore_reserve, "total_ore_reserve");
    }
	virtual ~Oremine() {}
	virtual void update();
	virtual void report();

        
    int total_ore_reserve;
    int anim;
    bool animate;
    int busy_days;
    int busy;    
    int anim_count; 
    int days_offset;
};

/** @file lincity/modules/oremine.h */

