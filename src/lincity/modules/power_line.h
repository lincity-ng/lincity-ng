#include <array>               // for array

#include "modules.h"

#define POWER_LINE_FLASH_SPEED 100

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
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance,
        cost, tech, range, 2/*mps_pages*/
    )
    {
        commodityRuleCount[STUFF_MWH].maxload = MAX_MWH_ON_POWERLINE;
        commodityRuleCount[STUFF_MWH].take = true;
        commodityRuleCount[STUFF_MWH].give = true;
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
        init_resources();
        this->flags |= (FLAG_TRANSPARENT | FLAG_NEVER_EVACUATE);
        this->anim_counter = 0;
        this->anim = 0;
        this->flashing = false;
        initialize_commodities();
        this->trafficCount = this->commodityCount;

        commodityMaxCons[STUFF_MWH] = 100 * 1;
    }
    virtual ~Powerline() { }
    virtual void update() override;
    virtual void report() override;
    virtual void animate() override;
    void flow_power();
    std::array<int, STUFF_COUNT> trafficCount;
    int anim_counter;
    int anim;
    bool flashing;
};


/** @file lincity/modules/power_line.h */
