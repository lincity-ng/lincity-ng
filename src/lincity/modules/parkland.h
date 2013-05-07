#define GROUP_PARKLAND_COLOUR (green(31))
#define GROUP_PARKLAND_COST   1000
#define GROUP_PARKLAND_COST_MUL 25
#define GROUP_PARKLAND_BUL_COST   1000
#define GROUP_PARKLAND_TECH   2
#define GROUP_PARKLAND_FIREC 1

#include "modules.h"
#include "../lintypes.h"
#include "../lctypes.h"


class ParklandConstructionGroup: public ConstructionGroup {
public:
    ParklandConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance, int cost, int tech
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance, cost, tech
    ) {
          
    };
    // overriding method that creates a Park
    virtual Construction *createConstruction(int x, int y, unsigned short type);
};

extern ParklandConstructionGroup parklandConstructionGroup;

class Parkland: public CountedConstruction<Parkland> { // park inherits from CountedConstruction
public:
	Parkland(int x, int y, unsigned short type): CountedConstruction<Parkland>(x, y, type)
    {
        constructionGroup = &parklandConstructionGroup;
        this->flags |= FLAG_NEVER_EVACUATE;
    }
    virtual void update();
	virtual void report();
};

/** @file lincity/modules/parkland.h */

