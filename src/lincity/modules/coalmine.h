
#define COALMINE_POLLUTION      3
#define COAL_PER_RESERVE   1000
#define JOBS_DIG_COAL 900
#define MAX_JOBS_AT_COALMINE (20 * JOBS_DIG_COAL)
#define MAX_COAL_AT_MINE (20 * COAL_PER_RESERVE)

#define GROUP_COALMINE_COLOUR 0
#define GROUP_COALMINE_COST   10000
#define GROUP_COALMINE_COST_MUL 25
#define GROUP_COALMINE_BUL_COST   10000
#define GROUP_COALMINE_TECH   85
#define GROUP_COALMINE_FIREC  85

#define TARGET_COAL_LEVEL 90

#include "modules.h"
#include "../lintypes.h"
#include "../lctypes.h"
#include "../range.h"


class CoalmineConstructionGroup: public ConstructionGroup {
public:
    CoalmineConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance, int cost, int tech
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance, cost, tech
    ) {
        commodityRuleCount[Construction::STUFF_JOBS].maxload = MAX_JOBS_AT_COALMINE;
        commodityRuleCount[Construction::STUFF_JOBS].take = true;
        commodityRuleCount[Construction::STUFF_JOBS].give = false;
        commodityRuleCount[Construction::STUFF_COAL].maxload = MAX_COAL_AT_MINE;
        commodityRuleCount[Construction::STUFF_COAL].take = true;
        commodityRuleCount[Construction::STUFF_COAL].give = true;
          
    }
    // overriding method that creates an Coalmine
    virtual Construction *createConstruction(int x, int y, unsigned short type);
};

extern CoalmineConstructionGroup coalmineConstructionGroup;

class Coalmine: public CountedConstruction<Coalmine> { // Coalmine inherits from its CountedConstruction
public:
	Coalmine(int x, int y, unsigned short type): CountedConstruction<Coalmine>(x, y, type) 
    {        
        constructionGroup = &coalmineConstructionGroup;
        this->busy_days = 0;
        this->busy = 0;
        this->current_coal_reserve = 0;  // has to be auto updated since coalmines may compete
        initialize_commodities();

        int coal = 0;
        int xx, yy, xs, ys, xe, ye;        
        xs = x - COAL_RESERVE_SEARCH_RANGE;
        xs = (xs < 0) ? 0 : xs;         
        ys = y - COAL_RESERVE_SEARCH_RANGE;
        ys = (ys < 0)? 0 : ys; 
        xe = x + COAL_RESERVE_SEARCH_RANGE;
        xe = (xe > world.len()) ? world.len() : xe;         
        ye = y + COAL_RESERVE_SEARCH_RANGE;
        ye = (ye > world.len())? world.len() : ye; 

        for (yy = ys; yy < ye ; yy++)
        {
            for (xx = xs; xx < xe ; xx++)
            {
                coal += world(xx,yy)->coal_reserve;                
            }
        }
        if (!coal)
        { coal = 1;}
        this->initial_coal_reserve = coal;
        setMemberSaved(&this->initial_coal_reserve,"initial_coal_reserve");
        this->current_coal_reserve = coal;          
    }
	virtual ~Coalmine() { }
	virtual void update();
	virtual void report();
       
    int initial_coal_reserve;
    int current_coal_reserve;
    int busy_days;
    int busy;    
};

/** @file lincity/modules/coalmine.h */

