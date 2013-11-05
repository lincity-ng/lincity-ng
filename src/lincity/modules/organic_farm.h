#define GROUP_ORGANIC_FARM_COLOUR (green(30))
#define GROUP_ORGANIC_FARM_COST 1000
#define GROUP_ORGANIC_FARM_COST_MUL 20
#define GROUP_ORGANIC_FARM_BUL_COST 100
#define GROUP_ORGANIC_FARM_TECH 0
#define GROUP_ORGANIC_FARM_FIREC 20
#define GROUP_ORGANIC_FARM_RANGE 0
#define GROUP_ORGANIC_FARM_SIZE 4

#define ORGANIC_FARM_FOOD_OUTPUT 550
#define ORG_FARM_POWER_REC 50
#define ORG_FARM_WASTE_GET 6
#define FARM_JOBS_USED 13
#define WATER_FARM 50
#define FARM_WATER_GET (16 * WATER_FARM)
#define MAX_WATER_AT_FARM (20*FARM_WATER_GET)

#define MAX_ORG_FARM_FOOD  (ORGANIC_FARM_FOOD_OUTPUT * 20)
#define MAX_ORG_FARM_POWER (ORG_FARM_POWER_REC * 20)
#define MAX_ORG_FARM_WASTE (ORG_FARM_WASTE_GET * 20)
#define MAX_FARM_JOBS (FARM_JOBS_USED * 20)



/* gets waste only when powered */

#define MIN_FOOD_SOLD_FOR_ANIM 200


#include "modules.h"
#include "../lintypes.h"
#include "../lctypes.h"
#include "../lcconfig.h"
#include "../lin-city.h"
#include <stdlib.h>


class Organic_farmConstructionGroup: public ConstructionGroup {
public:
    Organic_farmConstructionGroup(
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
        commodityRuleCount[Construction::STUFF_FOOD].maxload = MAX_ORG_FARM_FOOD;
        commodityRuleCount[Construction::STUFF_FOOD].take = false;
        commodityRuleCount[Construction::STUFF_FOOD].give = true;
        commodityRuleCount[Construction::STUFF_KWH].maxload = MAX_ORG_FARM_POWER;
        commodityRuleCount[Construction::STUFF_KWH].take = true;
        commodityRuleCount[Construction::STUFF_KWH].give = false;
        commodityRuleCount[Construction::STUFF_JOBS].maxload = MAX_FARM_JOBS;
        commodityRuleCount[Construction::STUFF_JOBS].take = true;
        commodityRuleCount[Construction::STUFF_JOBS].give = false;
        commodityRuleCount[Construction::STUFF_WASTE].maxload = MAX_ORG_FARM_WASTE;
        commodityRuleCount[Construction::STUFF_WASTE].take = true;
        commodityRuleCount[Construction::STUFF_WASTE].give = false;
        commodityRuleCount[Construction::STUFF_WATER].maxload = MAX_WATER_AT_FARM;
        commodityRuleCount[Construction::STUFF_WATER].give = false;
        commodityRuleCount[Construction::STUFF_WATER].take = true;
    }
    // overriding method that creates a organic_farm
    virtual Construction *createConstruction(int x, int y);
};

extern Organic_farmConstructionGroup organic_farmConstructionGroup;

class Organic_farm: public RegisteredConstruction<Organic_farm> { // Organic_farm inherits from its own RegisteredConstruction
public:
    Organic_farm(int x, int y, ConstructionGroup *cstgrp): RegisteredConstruction<Organic_farm>(x, y)
    {
        this->constructionGroup = cstgrp;
        this->tech = tech_level;
        setMemberSaved(&this->tech, "tech");
        this->tech_bonus = (tech_level * ORGANIC_FARM_FOOD_OUTPUT) / MAX_TECH_LEVEL;
        setMemberSaved(&this->tech_bonus, "tech_bonus");
        this->crop_rotation_key = (rand() % 4) + 1;
        this->month_stagger = rand() % 100;
        this->food_this_month = 0;
        this->food_last_month = 0;
        //this->max_foodprod = 0;
        initialize_commodities();
        // Check underground water, and reduce food production accordingly
        int w = 0;
        if (use_waterwell)
        {
            for (int i = 0; i < constructionGroup->size; i++)
            {
                for (int j = 0; j < constructionGroup->size; j++)
                {
                    if (world(x + j, y + i)->flags & FLAG_HAS_UNDERGROUND_WATER)
                        w++;
                }// end j
            }//end i
            this->ugwCount = w;
        }
        else //no waterwell
        {
            this->ugwCount = 16;
        }
    }
    virtual ~Organic_farm() { }
    virtual void update();
    virtual void report();

    int  ugwCount;
    int  max_foodprod;
    int  food_this_month;
    int  food_last_month;
    int  crop_rotation_key;
    int  month_stagger;
    int  tech;
    int  tech_bonus;
};



/** @file lincity/modules/organic_farm.h */

