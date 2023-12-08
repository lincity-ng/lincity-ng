#define GROUP_WINDMILL_COLOUR (green(25))
#define GROUP_WINDMILL_COST   20000
#define GROUP_WINDMILL_COST_MUL 25
#define GROUP_WINDMILL_BUL_COST   1000
#define GROUP_WINDMILL_TECH   30
#define GROUP_WINDMILL_FIREC  10
#define GROUP_WINDMILL_RANGE  0
#define GROUP_WINDMILL_SIZE  2

#define WINDMILL_KWH        450
#define WINDMILL_JOBS       10
#define MAX_JOBS_AT_WINDMILL 20*(WINDMILL_JOBS)
#define MAX_KWH_AT_WINDMILL 20*(WINDMILL_KWH)
/* WINDMILL_RCOST is days per quid */
#define WINDMILL_RCOST      4
#define ANTIQUE_WINDMILL_ANIM_SPEED 120

#define MODERN_WINDMILL_TECH 450000

#include "modules.h"
#include "../lintypes.h"
#include "../lctypes.h"

class WindmillConstructionGroup: public ConstructionGroup {
public:
    WindmillConstructionGroup(
        const char *name,
        unsigned short no_credit,
        unsigned short group,
        unsigned short size, int colour,
        int cost_mul, int bul_cost, int fire_chance,
        int cost, int tech, int range
    ): ConstructionGroup(
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance,
        cost, tech, range, 2/*mps_pages*/
    ) {
        commodityRuleCount[STUFF_JOBS].maxload = MAX_JOBS_AT_WINDMILL;
        commodityRuleCount[STUFF_JOBS].take = true;
        commodityRuleCount[STUFF_JOBS].give = false;
        commodityRuleCount[STUFF_KWH].maxload = MAX_KWH_AT_WINDMILL;
        commodityRuleCount[STUFF_KWH].take = false;
        commodityRuleCount[STUFF_KWH].give = true;
    }
    // overriding method that creates a Windmill
    virtual Construction *createConstruction(int x, int y);
};

extern WindmillConstructionGroup windmillConstructionGroup;

class Windmill: public RegisteredConstruction<Windmill> { // Windmill inherits from its own RegisteredConstruction
public:
    Windmill(int x, int y, ConstructionGroup *cstgrp): RegisteredConstruction<Windmill>(x, y)
    {
        this->constructionGroup = cstgrp;
        init_resources();
        // this->anim = 0;
        this->animate_enable = false;
        this->tech = tech_level;
        setMemberSaved(&this->tech, "tech");
        this->working_days = 0;
        this->busy = 0;
        // this->kwh_output = (int)(WINDMILL_KWH + (((double)tech_level * WINDMILL_KWH) / MAX_TECH_LEVEL));
        setMemberSaved(&this->kwh_output, "kwh_output"); // compatibility
        initialize_commodities();

        commodityMaxCons[STUFF_JOBS] = 100 * WINDMILL_JOBS;
        // commodityMaxProd[STUFF_KWH] = 100 * kwh_output;
    }

    virtual void initialize() override {
      RegisteredConstruction::initialize();

      this->kwh_output = (int)(WINDMILL_KWH +
        (((double)tech_level * WINDMILL_KWH) / MAX_TECH_LEVEL));

      commodityMaxProd[STUFF_KWH] = 100 * kwh_output;
    }

    virtual ~Windmill() { }
    virtual void update() override;
    virtual void report() override;
    virtual void animate() override;

    int  kwh_output;
    int  tech;
    int  anim;
    int  working_days, busy;
    bool animate_enable;
};

/** @file lincity/modules/windmill.h */
