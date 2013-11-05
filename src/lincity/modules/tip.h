
#define GROUP_TIP_COLOUR (white(16))
#define GROUP_TIP_COST 10000
#define GROUP_TIP_COST_MUL 25
#define GROUP_TIP_BUL_COST 1000000
#define GROUP_TIP_TECH 0
#define GROUP_TIP_FIREC 50
#define GROUP_TIP_RANGE 0
#define GROUP_TIP_SIZE 4

#define TIP_DEGRADE_TIME 200 * NUMOF_DAYS_IN_YEAR
#define MAX_WASTE_AT_TIP  10000000

#define WASTE_BURRIED 200
#define CRITICAL_WASTE_LEVEL 20 //gives waste if inbox is below and swallows if above
#define TIP_TAKES_WASTE (20 * WASTE_BURRIED)

#include "modules.h"
#include "../lintypes.h"
#include "../lctypes.h"


class TipConstructionGroup: public ConstructionGroup {
public:
    TipConstructionGroup(
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
        commodityRuleCount[Construction::STUFF_WASTE].maxload = TIP_TAKES_WASTE;
        commodityRuleCount[Construction::STUFF_WASTE].take = true;
        commodityRuleCount[Construction::STUFF_WASTE].give = true;
    }
    // overriding method that creates a tip
    virtual Construction *createConstruction(int x, int y);
};

extern TipConstructionGroup tipConstructionGroup;

class Tip: public RegisteredConstruction<Tip>{ // Tip inherits from its own RegisteredConstruction
public:
    Tip(int x, int y, ConstructionGroup *cstgrp): RegisteredConstruction<Tip>(x, y)
    {
        this->constructionGroup = cstgrp;
        this->flags |= FLAG_NEVER_EVACUATE;
        this->total_waste = 0;
        setMemberSaved(&this->total_waste,"total_waste");
        this->working_days = 0;
        this->busy = 0;
        this->degration_days = 0;
        setMemberSaved(&this->degration_days,"degration_days");
        initialize_commodities();
    }
    virtual ~Tip() { }
    virtual void update();
    virtual void report();

    int  working_days, busy;
    int  total_waste;
    int  degration_days;
};
