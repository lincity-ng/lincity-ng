#include "modules.h"
#include "../lintypes.h"
#include "../lctypes.h"
#include "../transport.h"

class PowerlineConstructionGroup: public ConstructionGroup {
public:
    PowerlineConstructionGroup(
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
        commodityRuleCount[Construction::STUFF_MWH].maxload = MAX_MWH_ON_POWERLINE;
        commodityRuleCount[Construction::STUFF_MWH].take = true;
        commodityRuleCount[Construction::STUFF_MWH].give = true;
    }
    // overriding method that creates a power line
    virtual Construction *createConstruction(int x, int y);
};

extern PowerlineConstructionGroup powerlineConstructionGroup;

class Powerline: public RegisteredConstruction<Powerline> { // Powerlineinherits from its own RegisteredConstruction
public:
    Powerline(int x, int y, ConstructionGroup *cstgrp): RegisteredConstruction<Powerline>(x, y)
    {
        this->constructionGroup = cstgrp;
        this->flags |= (FLAG_POWER_LINE | FLAG_NEVER_EVACUATE);
        this->anim_counter = 0;
        this->flashing = false;
        initialize_commodities();
        this->trafficCount = this->commodityCount;
    }
    virtual ~Powerline() { }
    virtual void update();
    virtual void report();
    void flow_power();
    std::map<Commodities, int> trafficCount;
    int anim_counter;
    bool flashing;
};


/** @file lincity/modules/power_line.h */

