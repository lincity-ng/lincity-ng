#define GROUP_SOLAR_POWER_COLOUR (yellow(22))
#define GROUP_SOLAR_POWER_COST 500000
#define GROUP_SOLAR_POWER_COST_MUL 5
#define GROUP_SOLAR_POWER_BUL_COST 100000
#define GROUP_SOLAR_POWER_TECH 500
#define GROUP_SOLAR_POWER_FIREC 33
#define GROUP_SOLAR_POWER_RANGE 0
#define GROUP_SOLAR_POWER_SIZE 4

#define SOLAR_POWER_LABOR 50
#define POWERS_SOLAR_OUTPUT 900 //1800
#define MAX_LABOR_AT_SOLARPS (20 * SOLAR_POWER_LABOR)
#define MAX_HIVOLT_AT_SOLARPS (20 * POWERS_SOLAR_OUTPUT)

#include <array>                    // for array
#include <string>                   // for basic_string

#include "modules.h"


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
        name, no_credit, group, size, colour, cost_mul, bul_cost, fire_chance,
        cost, tech, range, 2/*mps_pages*/
    )
    {
        commodityRuleCount[STUFF_LABOR].maxload = MAX_LABOR_AT_SOLARPS;
        commodityRuleCount[STUFF_LABOR].take = true;
        commodityRuleCount[STUFF_LABOR].give = false;
        commodityRuleCount[STUFF_HIVOLT].maxload = MAX_HIVOLT_AT_SOLARPS;
        commodityRuleCount[STUFF_HIVOLT].take = false;
        commodityRuleCount[STUFF_HIVOLT].give = true;
    };
    // overriding method that creates a Solar Power Plant
    virtual Construction *createConstruction();
};

extern SolarPowerConstructionGroup solarPowerConstructionGroup;

class SolarPower: public Construction {
public:
    SolarPower(ConstructionGroup *cstgrp) {
        this->constructionGroup = cstgrp;
        init_resources();
        this->tech = tech_level;
        setMemberSaved(&this->tech, "tech");
        this->working_days = 0;
        this->busy = 0;
        // this->hivolt_output = (int)(POWERS_SOLAR_OUTPUT + (((double)tech_level * POWERS_SOLAR_OUTPUT) / MAX_TECH_LEVEL));
        setMemberSaved(&this->hivolt_output, "mwh_output"); // compatibility
        initialize_commodities();

        commodityMaxCons[STUFF_LABOR] = 100 * SOLAR_POWER_LABOR;
    }

    virtual void initialize() override {
        Construction::initialize();

        this->hivolt_output = (int)(POWERS_SOLAR_OUTPUT +
          (((double)tech * POWERS_SOLAR_OUTPUT) / MAX_TECH_LEVEL));

        commodityMaxProd[STUFF_HIVOLT] = 100 * hivolt_output;
    }

    virtual void update() override;
    virtual void report() override;
    int  hivolt_output;
    int  tech;
    int  working_days, busy;
};


/** @file lincity/modules/solar_power.h */
