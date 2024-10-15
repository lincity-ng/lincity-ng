#define GROUP_WINDMILL_COLOUR (green(25))
#define GROUP_WINDMILL_COST   20000
#define GROUP_WINDMILL_COST_MUL 25
#define GROUP_WINDMILL_BUL_COST   1000
#define GROUP_WINDMILL_TECH   30
#define GROUP_WINDMILL_FIREC  10
#define GROUP_WINDMILL_RANGE  0
#define GROUP_WINDMILL_SIZE  2

#define WINDMILL_LOVOLT     450
#define WINDMILL_LABOR       10
#define MAX_LABOR_AT_WINDMILL 20*(WINDMILL_LABOR)
#define MAX_LOVOLT_AT_WINDMILL 20*(WINDMILL_LOVOLT)
/* WINDMILL_RCOST is days per quid */
#define WINDMILL_RCOST      4
#define ANTIQUE_WINDMILL_ANIM_SPEED 120

#define MODERN_WINDMILL_TECH 450000

#include <array>                    // for array
#include <string>                   // for basic_string

#include "modules.h"

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
        commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_AT_WINDMILL;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = false;
        commodityRuleCount[STUFF_LOVOLT].maxload = MAX_LOVOLT_AT_WINDMILL;
        commodityRuleCount[STUFF_LOVOLT].take = false;
        commodityRuleCount[STUFF_LOVOLT].give = true;
    }
    // overriding method that creates a Windmill
    virtual Construction *createConstruction();
};

extern WindmillConstructionGroup windmillConstructionGroup;

class Windmill: public Construction {
public:
    Windmill(ConstructionGroup *cstgrp) {
        this->constructionGroup = cstgrp;
        init_resources();
        // this->anim = 0;
        this->animate_enable = false;
        this->tech = tech_level;
        setMemberSaved(&this->tech, "tech");
        this->working_days = 0;
        this->busy = 0;
        // this->lovolt_output = (int)(WINDMILL_LOVOLT + (((double)tech_level * WINDMILL_LOVOLT) / MAX_TECH_LEVEL));
        setMemberSaved(&this->lovolt_output, "kwh_output"); // compatibility
        initialize_commodities();

        commodityMaxCons[STUFF_LABOR] = 100 * WINDMILL_LABOR;
        // commodityMaxProd[STUFF_LOVOLT] = 100 * lovolt_output;
    }

    virtual void initialize() override {
      Construction::initialize();

      this->lovolt_output = (int)(WINDMILL_LOVOLT +
        (((double)tech * WINDMILL_LOVOLT) / MAX_TECH_LEVEL));

      commodityMaxProd[STUFF_LOVOLT] = 100 * lovolt_output;
    }

    virtual ~Windmill() { }
    virtual void update() override;
    virtual void report() override;
    virtual void animate() override;

    int  lovolt_output;
    int  tech;
    int  anim;
    int  working_days, busy;
    bool animate_enable;
};

/** @file lincity/modules/windmill.h */
