#define GROUP_PARKLAND_COLOUR (green(31))
#define GROUP_PARKLAND_COST   1000
#define GROUP_PARKLAND_COST_MUL 25
#define GROUP_PARKLAND_BUL_COST   1000
#define GROUP_PARKLAND_TECH   2
#define GROUP_PARKLAND_FIREC 1
#define GROUP_PARKLAND_RANGE 0
#define GROUP_PARKLAND_SIZE 1

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
        int cost_mul, int bul_cost, int fire_chance,
        int cost, int tech, int range
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance, cost, tech, range
    ) {

    };
    // overriding method that creates a Park
    virtual Construction *createConstruction(int x, int y);
};

extern ParklandConstructionGroup parklandConstructionGroup;
extern ParklandConstructionGroup parkpondConstructionGroup;

class Parkland: public RegisteredConstruction<Parkland> { // park inherits from RegisteredConstruction
public:
    Parkland(int x, int y, ConstructionGroup *cstgrp): RegisteredConstruction<Parkland>(x, y)
    {
        this->constructionGroup = cstgrp;
        this->flags |= FLAG_NEVER_EVACUATE;
    }
    virtual void update();
    virtual void report();
};

/** @file lincity/modules/parkland.h */

