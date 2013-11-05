#define GROUP_COMMUNE_COLOUR (green(30))
#define GROUP_COMMUNE_COST  1
#define GROUP_COMMUNE_COST_MUL 2
#define GROUP_COMMUNE_BUL_COST  1000
#define GROUP_COMMUNE_TECH  0
#define GROUP_COMMUNE_FIREC 30
#define GROUP_COMMUNE_RANGE 0
#define GROUP_COMMUNE_SIZE 4

//#define JOBS_AT_COMMUNE_GATE 4
#define COMMUNE_ANIM_SPEED 750
#define COMMUNE_POP  5 //Used at shanty

#define COMMUNE_COAL_MADE 3
#define MAX_COAL_AT_COMMUNE (20*COMMUNE_COAL_MADE)
#define COMMUNE_ORE_MADE 6
#define COMMUNE_ORE_FROM_WASTE 1
#define MAX_ORE_AT_COMMUNE (20*(COMMUNE_ORE_MADE + COMMUNE_ORE_FROM_WASTE))
#define COMMUNE_STEEL_MADE 2
#define MAX_STEEL_AT_COMMUNE (20*COMMUNE_STEEL_MADE)
#define COMMUNE_WASTE_GET 20
#define MAX_WASTE_AT_COMMUNE (20*COMMUNE_WASTE_GET)
#define WATER_FOREST 5
#define COMMUNE_WATER_GET (16 * WATER_FOREST)
#define MAX_WATER_AT_COMMUNE (20*COMMUNE_WATER_GET)

#include "modules.h"
#include "../lintypes.h"
#include "../lctypes.h"
#include <cstdlib>

class CommuneConstructionGroup: public ConstructionGroup {
public:
    CommuneConstructionGroup(
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
        commodityRuleCount[Construction::STUFF_COAL].maxload = MAX_COAL_AT_COMMUNE;
        commodityRuleCount[Construction::STUFF_COAL].take = false;
        commodityRuleCount[Construction::STUFF_COAL].give = true;
        commodityRuleCount[Construction::STUFF_ORE].maxload = MAX_ORE_AT_COMMUNE;
        commodityRuleCount[Construction::STUFF_ORE].take = false;
        commodityRuleCount[Construction::STUFF_ORE].give = true;
        commodityRuleCount[Construction::STUFF_STEEL].maxload = MAX_STEEL_AT_COMMUNE;
        commodityRuleCount[Construction::STUFF_STEEL].take = false;
        commodityRuleCount[Construction::STUFF_STEEL].give = true;
        commodityRuleCount[Construction::STUFF_WASTE].maxload = MAX_WASTE_AT_COMMUNE;
        commodityRuleCount[Construction::STUFF_WASTE].take = true;
        commodityRuleCount[Construction::STUFF_WASTE].give = false;
        commodityRuleCount[Construction::STUFF_WATER].maxload = MAX_WATER_AT_COMMUNE;
        commodityRuleCount[Construction::STUFF_WATER].give = false;
        commodityRuleCount[Construction::STUFF_WATER].take = true;
    }
    // overriding method that creates a commune
    virtual Construction *createConstruction(int x, int y);
};

extern CommuneConstructionGroup communeConstructionGroup;

class Commune: public RegisteredConstruction<Commune> { // Commune inherits from Construction
public:
    Commune(int x, int y, ConstructionGroup *cstgrp): RegisteredConstruction<Commune>(x ,y)
    {
        this->constructionGroup = cstgrp;
        this->anim = 0; // or real_time?
        this->animate = false;
        this->steel_made = false;
        this->monthly_stuff_made = 0;
        this->last_month_output = 0;
        this->lazy_months = 0;
        initialize_commodities();
        // Check underground water, and reduce coal production accordingly
        int w = 0;
        for (int i = 0; i < constructionGroup->size; i++)
        {
            for (int j = 0; j < constructionGroup->size; j++)
            {
                if (world(x+j, y+i)->flags & FLAG_HAS_UNDERGROUND_WATER)
                {    w++;}
            }// end j
        }//end i
        this->ugwCount = w;
        if (w < 16 / 3)
        {   this->coalprod = COMMUNE_COAL_MADE/3;}
        else if (w < (2 * 16) / 3)
        {   this->coalprod = COMMUNE_COAL_MADE/2;}
        else
        {   this->coalprod = COMMUNE_COAL_MADE;}
    }
    virtual ~Commune() { }
    virtual void update();
    virtual void report();

    int  anim;
    int  ugwCount;
    int  coalprod;
    int  monthly_stuff_made;
    int  last_month_output;
    int  lazy_months;
    bool animate;
    bool steel_made;
};

/** @file lincity/modules/commune.h */

