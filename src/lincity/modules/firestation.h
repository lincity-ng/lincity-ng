#define GROUP_FIRESTATION_COLOUR (red(20))
#define GROUP_FIRESTATION_COST   20000
#define GROUP_FIRESTATION_COST_MUL 10
#define GROUP_FIRESTATION_BUL_COST 1000
#define GROUP_FIRESTATION_TECH     22
#define GROUP_FIRESTATION_FIREC	0
#define GROUP_FIRESTATION_RANGE 17


#define FIRESTATION_JOBS   6
#define FIRESTATION_GET_JOBS 7
#define MAX_JOBS_AT_FIRESTATION (20 * FIRESTATION_JOBS * DAYS_BETWEEN_COVER)
#define FIRESTATION_GOODS  2
#define FIRESTATION_GET_GOODS 3
#define MAX_GOODS_AT_FIRESTATION (20 * FIRESTATION_GOODS * DAYS_BETWEEN_COVER)
#define MAX_WASTE_AT_FIRESTATION (20 * FIRESTATION_GOODS * DAYS_BETWEEN_COVER / 3)
#define FIRESTATION_RUNNING_COST 1
#define FIRESTATION_RUNNING_COST_MUL 6
#define FIRESTATION_ANIMATION_SPEED 250



#include "modules.h"
#include "../lintypes.h"
#include "../lctypes.h"
//#include "../range.h"

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
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance, cost, tech, range
    )
    {
        commodityRuleCount[Construction::STUFF_JOBS].maxload = MAX_JOBS_AT_FIRESTATION;
        commodityRuleCount[Construction::STUFF_JOBS].take = true;
        commodityRuleCount[Construction::STUFF_JOBS].give = false;
        commodityRuleCount[Construction::STUFF_GOODS].maxload = MAX_GOODS_AT_FIRESTATION;
        commodityRuleCount[Construction::STUFF_GOODS].take = true;
        commodityRuleCount[Construction::STUFF_GOODS].give = false;
        commodityRuleCount[Construction::STUFF_WASTE].maxload = MAX_WASTE_AT_FIRESTATION;
        commodityRuleCount[Construction::STUFF_WASTE].take = false;
        commodityRuleCount[Construction::STUFF_WASTE].give = true;
    }
    // overriding method that creates a firestation
    virtual Construction *createConstruction(int x, int y, unsigned short type);
};

extern FireStationConstructionGroup fireStationConstructionGroup;

class FireStation: public CountedConstruction<FireStation> { // FireStation inherits from CountedConstruction
public:
	FireStation(int x, int y, unsigned short type) : CountedConstruction<FireStation>(x ,y ,type)
    {     
        constructionGroup = &fireStationConstructionGroup;		     
        this->anim = 0; // or real_time?        
        this->animate = false;
        this->busy = false;
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
	}
	virtual ~FireStation() { }
	virtual void update();
	virtual void report();
    void cover();

	int xs, ys, xe, ye;
    int  anim;	
    bool animate;
    bool busy;
};

/** @file lincity/modules/firestation.h */

