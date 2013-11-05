#define GROUP_CRICKET_COLOUR (white(20))
#define GROUP_CRICKET_COST   2000
#define GROUP_CRICKET_COST_MUL 3
#define GROUP_CRICKET_BUL_COST 1000
#define GROUP_CRICKET_TECH     12
#define GROUP_CRICKET_FIREC 20
#define GROUP_CRICKET_RANGE 9
#define GROUP_CRICKET_SIZE 2

#define CRICKET_JOBS   8
#define MAX_JOBS_AT_CRICKET (20 * CRICKET_JOBS)
#define CRICKET_GOODS  3
#define MAX_GOODS_AT_CRICKET (20 * CRICKET_GOODS)
#define MAX_WASTE_AT_CRICKET (20 * CRICKET_GOODS / 3)
#define CRICKET_RUNNING_COST 1
#define CRICKET_ANIMATION_SPEED 750

#define CRICKET_JOB_SWING 4

#include "modules.h"
#include "../lintypes.h"
#include "../lctypes.h"

class CricketConstructionGroup: public ConstructionGroup {
public:
    CricketConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance,
        int cost, int tech, int range
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance, cost, tech, range
    ) {
        commodityRuleCount[Construction::STUFF_JOBS].maxload = MAX_JOBS_AT_CRICKET;
        commodityRuleCount[Construction::STUFF_JOBS].take = true;
        commodityRuleCount[Construction::STUFF_JOBS].give = false;
        commodityRuleCount[Construction::STUFF_GOODS].maxload = MAX_GOODS_AT_CRICKET;
        commodityRuleCount[Construction::STUFF_GOODS].take = true;
        commodityRuleCount[Construction::STUFF_GOODS].give = false;
        commodityRuleCount[Construction::STUFF_WASTE].maxload = MAX_WASTE_AT_CRICKET;
        commodityRuleCount[Construction::STUFF_WASTE].take = false;
        commodityRuleCount[Construction::STUFF_WASTE].give = true;
    }
    // overriding method that creates a Cricket
    virtual Construction *createConstruction(int x, int y);
};

extern CricketConstructionGroup cricketConstructionGroup;

class Cricket: public RegisteredConstruction<Cricket> { // cricket inherits from Construction
public:
    Cricket(int x, int y, ConstructionGroup *cstgrp): RegisteredConstruction<Cricket>(x, y)
    {
        this->constructionGroup = cstgrp;
        this->anim = 0;
        this->animate = false;
        this->active = false;
        setMemberSaved(&(this->active),"active");
        this->busy = 0;
        this->daycount = 0;
        this->working_days = 0;
        setMemberSaved(&(this->daycount),"daycount");
        this->covercount = 0;
        setMemberSaved(&(this->covercount),"covercount");
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

    virtual ~Cricket() { }
    virtual void update();
    virtual void report();
    void cover();

    int xs, ys, xe, ye;
    int daycount, covercount;
    int anim;
    bool animate, active;
    int working_days, busy;
};

/** @file lincity/modules/cricket.h */

