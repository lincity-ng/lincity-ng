#define GROUP_ROCKET_COLOUR   (magenta(20))
#define GROUP_ROCKET_COST     700000
#define GROUP_ROCKET_COST_MUL 2
#define GROUP_ROCKET_BUL_COST     1
#define GROUP_ROCKET_TECH     750
#define GROUP_ROCKET_FIREC 0
#define GROUP_ROCKET_RANGE 0
#define GROUP_ROCKET_SIZE 4

#define ROCKET_PAD_JOBS         200
#define ROCKET_PAD_JOBS_STORE   (ROCKET_PAD_JOBS * 50)
#define MAX_JOBS_AT_ROCKET_PAD  (ROCKET_PAD_JOBS * 20)
#define ROCKET_PAD_GOODS        10000
#define ROCKET_PAD_GOODS_STORE  (ROCKET_PAD_GOODS * 50)
#define MAX_GOODS_AT_ROCKET_PAD (ROCKET_PAD_GOODS * 20)
#define MAX_WASTE_AT_ROCKET_PAD (MAX_GOODS_AT_ROCKET_PAD / 3)
#define ROCKET_PAD_STEEL        240
#define ROCKET_PAD_STEEL_STORE  (ROCKET_PAD_STEEL * 50)
#define MAX_STEEL_AT_ROCKET_PAD (ROCKET_PAD_STEEL * 20)
#define ROCKET_PAD_RUNNING_COST 200
#define ROCKET_PAD_LAUNCH       100
#define ROCKET_ANIMATION_SPEED  450
#define ROCKET_LAUNCH_BAD       1
#define ROCKET_LAUNCH_GOOD      2
#define ROCKET_LAUNCH_EVAC      3

#include "modules.h"
#include "../lintypes.h"
#include "../lctypes.h"


class RocketPadConstructionGroup: public ConstructionGroup {
public:
    RocketPadConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance,
        int cost, int tech, int range
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance, cost, tech, range
    ) {
        commodityRuleCount[Construction::STUFF_JOBS].maxload = MAX_JOBS_AT_ROCKET_PAD;
        commodityRuleCount[Construction::STUFF_JOBS].take = true;
        commodityRuleCount[Construction::STUFF_JOBS].give = false;
        commodityRuleCount[Construction::STUFF_GOODS].maxload = MAX_GOODS_AT_ROCKET_PAD;
        commodityRuleCount[Construction::STUFF_GOODS].take = true;
        commodityRuleCount[Construction::STUFF_GOODS].give = false;
        commodityRuleCount[Construction::STUFF_STEEL].maxload = MAX_STEEL_AT_ROCKET_PAD;
        commodityRuleCount[Construction::STUFF_STEEL].take = true;
        commodityRuleCount[Construction::STUFF_STEEL].give = false;
        commodityRuleCount[Construction::STUFF_WASTE].maxload = MAX_WASTE_AT_ROCKET_PAD;
        commodityRuleCount[Construction::STUFF_WASTE].take = false;
        commodityRuleCount[Construction::STUFF_WASTE].give = true;
    }
    // overriding method that creates a RocketPad
    virtual Construction *createConstruction(int x, int y);
};

extern RocketPadConstructionGroup rocketPadConstructionGroup;

class RocketPad: public RegisteredConstruction<RocketPad> { // rocketPad inherits from its own RegisteredConstruction
public:
    RocketPad(int x, int y, ConstructionGroup* cstgrp): RegisteredConstruction<RocketPad>(x, y)
    {
        this->constructionGroup = cstgrp;
        this->working_days = 0;
        this->busy = 0;
        this->anim = 0;
        this->completion = 0;
        setMemberSaved(&this->completion, "completion");
        this->step = 0;
        setMemberSaved(&this->step, "step");
        this->tech = tech_level;
        setMemberSaved(&this->tech, "tech");
        this->goods_stored = 0;
        setMemberSaved(&this->goods_stored, "goods_stored");
        this->jobs_stored = 0;
        setMemberSaved(&this->jobs_stored, "jobs_stored");
        this->steel_stored = 0;
        setMemberSaved(&this->steel_stored, "steel_stored");
        initialize_commodities();
    }

    virtual ~RocketPad() { }
    virtual void update();
    virtual void report();
    void launch_rocket();
    void remove_people(int num);
    int working_days, busy;
    int tech;
    int anim;
    int jobs_stored, goods_stored, steel_stored;
    int completion, step;
};


/** @file lincity/modules/rocket_pad.h */

