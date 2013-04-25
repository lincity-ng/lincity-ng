#define HEALTH_CENTRE_JOBS   6
#define HEALTH_CENTRE_GET_JOBS 7
#define MAX_JOBS_AT_HEALTH_CENTRE (20 * HEALTH_CENTRE_JOBS * DAYS_BETWEEN_COVER)
#define HEALTH_CENTRE_GOODS  40
#define HEALTH_CENTRE_GET_GOODS 41
#define MAX_GOODS_AT_HEALTH_CENTRE (20 * HEALTH_CENTRE_GOODS * DAYS_BETWEEN_COVER)
#define MAX_WASTE_AT_HEALTH_CENTRE (20 * HEALTH_CENTRE_GOODS * DAYS_BETWEEN_COVER /3)
#define HEALTH_RUNNING_COST  2
#define HEALTH_RUNNING_COST_MUL 9

#define GROUP_HEALTH_COLOUR   (green(24))
#define GROUP_HEALTH_COST     100000
#define GROUP_HEALTH_COST_MUL 2
#define GROUP_HEALTH_BUL_COST     1000
#define GROUP_HEALTH_TECH     110
#define GROUP_HEALTH_FIREC 2

#include "modules.h"
#include "../lintypes.h"
#include "../lctypes.h"
#include "../range.h"

class HealthCentreConstructionGroup: public ConstructionGroup {
public:
    HealthCentreConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance, int cost, int tech
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance, cost, tech
    ) {
        commodityRuleCount[Construction::STUFF_JOBS].maxload = MAX_JOBS_AT_HEALTH_CENTRE;
        commodityRuleCount[Construction::STUFF_JOBS].take = true;
        commodityRuleCount[Construction::STUFF_JOBS].give = false;
        commodityRuleCount[Construction::STUFF_GOODS].maxload = MAX_GOODS_AT_HEALTH_CENTRE;
        commodityRuleCount[Construction::STUFF_GOODS].take = true;
        commodityRuleCount[Construction::STUFF_GOODS].give = false;
        commodityRuleCount[Construction::STUFF_WASTE].maxload = MAX_WASTE_AT_HEALTH_CENTRE;
        commodityRuleCount[Construction::STUFF_WASTE].take = false;
        commodityRuleCount[Construction::STUFF_WASTE].give = true;           
    }
    // overriding method that creates a HealthCentre
    virtual Construction *createConstruction(int x, int y, unsigned short type);
};

extern HealthCentreConstructionGroup healthCentreConstructionGroup;

class HealthCentre: public CountedConstruction<HealthCentre> { // HealthCentre inherits from its own CountedConstruction
public:
	HealthCentre(int x, int y, unsigned short type): CountedConstruction<HealthCentre>(x, y, type)
    {           
        constructionGroup = &healthCentreConstructionGroup;   
        this->busy = false;
        initialize_commodities();
	}
	virtual ~HealthCentre() { }
	virtual void update();
	virtual void report();
    void cover();	

    bool busy;
};

/** @file lincity/modules/health_centre.h */

