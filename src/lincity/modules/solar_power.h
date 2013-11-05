#define GROUP_SOLAR_POWER_COLOUR (yellow(22))
#define GROUP_SOLAR_POWER_COST 500000
#define GROUP_SOLAR_POWER_COST_MUL 5
#define GROUP_SOLAR_POWER_BUL_COST 100000
#define GROUP_SOLAR_POWER_TECH 500
#define GROUP_SOLAR_POWER_FIREC 33
#define GROUP_SOLAR_POWER_RANGE 0
#define GROUP_SOLAR_POWER_SIZE 4

#define SOLAR_POWER_JOBS 50
#define POWERS_SOLAR_OUTPUT 900 //1800
#define MAX_JOBS_AT_SOLARPS (20 * SOLAR_POWER_JOBS)
#define MAX_MWH_AT_SOLARPS (20 * POWERS_SOLAR_OUTPUT)

#include "modules.h"
#include "../lintypes.h"
#include "../lctypes.h"


class SolarPowerConstructionGroup: public ConstructionGroup {
public:
    SolarPowerConstructionGroup(
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
        commodityRuleCount[Construction::STUFF_JOBS].maxload = MAX_JOBS_AT_SOLARPS;
        commodityRuleCount[Construction::STUFF_JOBS].take = true;
        commodityRuleCount[Construction::STUFF_JOBS].give = false;
        commodityRuleCount[Construction::STUFF_MWH].maxload = MAX_MWH_AT_SOLARPS;
        commodityRuleCount[Construction::STUFF_MWH].take = false;
        commodityRuleCount[Construction::STUFF_MWH].give = true;
    };
    // overriding method that creates a Solar Power Plant
    virtual Construction *createConstruction(int x, int y);
};

extern SolarPowerConstructionGroup solarPowerConstructionGroup;

class SolarPower: public RegisteredConstruction<SolarPower> { // park inherits from RegisteredConstruction
public:
    SolarPower(int x, int y, ConstructionGroup *cstgrp): RegisteredConstruction<SolarPower>(x, y)
    {
        this->constructionGroup = cstgrp;
        this->tech = tech_level;
        setMemberSaved(&this->tech, "tech");
        this->working_days = 0;
        this->busy = 0;
        this->mwh_output = (int)(POWERS_SOLAR_OUTPUT + (((double)tech_level * POWERS_SOLAR_OUTPUT) / MAX_TECH_LEVEL));
        setMemberSaved(&this->mwh_output, "mwh_output");
        initialize_commodities();
    }
    virtual void update();
    virtual void report();
    int  mwh_output;
    int  tech;
    int  working_days, busy;
};


/** @file lincity/modules/solar_power.h */

